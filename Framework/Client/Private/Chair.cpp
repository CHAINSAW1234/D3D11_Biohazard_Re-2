#include "stdafx.h"
#include "Chair.h"
#include"Player.h"
#include "PxCollider.h"
#include "Bone.h"
#include "Selector_UI.h"

//part-obj
#include"Body_Chair.h"
#include"Door.h"

#include "Call_Center.h"


CChair::CChair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CChair::CChair(const CChair& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CChair::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChair::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	if (m_tagPropDesc.strGamePrototypeName.find("zombie")!=string::npos)
	{
		m_eType = CHAIR_ZOMBIE;
		//if (FAILED(CCall_Center::Get_Instance()->Add_Caller(this, CCall_Center::CALLER::_ZOMBIE_CHAIR)))
		//	return E_FAIL;
	}
	else
		m_eType = CHAIR_BARRIGATE;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;
	
	if (FAILED(Initialize_PartObjects()))
		return E_FAIL;	

	if (FAILED(m_pGameInstance->Add_Object_Sound(m_pTransformCom, 1)))
		return E_FAIL;


	return S_OK;
}

void CChair::Start()
{
	__super::Start();
	if (m_eType == CHAIR_BARRIGATE)
	{
		list<CGameObject*>* pDoor = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Door"));
		if (pDoor == nullptr)
			return;
		for (auto& iter : *pDoor)
			if(static_cast<CDoor*>(iter)->Get_DoorType()==CDoor::TYPE_DOOR::DOOR_DOUBLE)
				if (m_pColliderCom[INTER_COL_NORMAL][COL_STEP0]->Intersect(static_cast<CCollider*>(iter->Get_Component(TEXT("Com_Collider_Normal_Step0")))))
				{
					m_pDoor = static_cast<CDoor*>(iter);
					m_pDoor->Set_Lock(true);
					break;
				}

	}
	
}

void CChair::Tick(_float fTimeDelta)
{
	__super::Tick_Col();
	if (!m_bVisible)
	{
		if (nullptr != m_pSelector)
		{
			m_pSelector = static_cast<CSelector_UI*>(m_pSelector->Destroy_Selector());

			m_pSelector = nullptr;
		}
		return;
	}

	m_eType == CHAIR_ZOMBIE ? Zombie_Tick(fTimeDelta) : Barrigate_Tick(fTimeDelta);


	__super::Tick(fTimeDelta);
}

void CChair::Late_Tick(_float fTimeDelta)
{
	if (m_pPlayer == nullptr)
		return;
	if (!Visible())
		return;
	if (m_bRender == false)
		return;
	else
	{
		for (auto& it : m_PartObjects)
		{
			if(it != nullptr)
				it->Set_Render(true);
		}

		m_bRender = false;
	}
	if (m_eType == CHAIR_BARRIGATE)
	{
		if (Activate_Col(Get_Collider_World_Pos(_float4(50.f, 1.f, 50.f, 1.f))))
		{
			if (Check_Col_Player(INTER_COL_NORMAL, COL_STEP0))
			{
				Check_Col_Player(INTER_COL_NORMAL, COL_STEP1);
				Opreate_Selector_UI(true, Get_Object_Pos());

			}
			else
			{
				m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;

				if (nullptr != m_pSelector)
				{
					m_pSelector = static_cast<CSelector_UI*>(m_pSelector->Destroy_Selector());

					m_pSelector = nullptr;
				}
			}
		}
		else
		{
			m_bCol[INTER_COL_NORMAL][COL_STEP0] = false;
			m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;

			if (nullptr != m_pSelector)
			{
				m_pSelector = static_cast<CSelector_UI*>(m_pSelector->Destroy_Selector());

				m_pSelector = nullptr;
			}
		}
	}
	

	__super::Late_Tick(fTimeDelta);


#ifdef _DEBUG
	__super::Add_Col_DebugCom();
#endif
}

HRESULT CChair::Render()
{
	return S_OK;
}

HRESULT CChair::Add_Components()
{

	if (m_eType == CHAIR_BARRIGATE)
	{
		CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

		ColliderDesc.fRadius = _float(100.f);
		ColliderDesc.vCenter = _float3(0.f, 0.f, 90.f);
		/* For.Com_Collider */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Normal_Step0"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP0], &ColliderDesc)))
			return E_FAIL;

		ColliderDesc.fRadius = _float(80.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Normal_Step1"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP1], &ColliderDesc)))
			return E_FAIL;
	}



	return S_OK;
}

HRESULT CChair::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CChair::PART_END);

	switch (m_eType)
	{
	case CHAIR_ZOMBIE:
		/*Part_Body*/
	{
		CPartObject* pBodyObj = { nullptr };
		CBody_Chair::PART_INTERACTPROPS_DESC BodyDesc = {};
		BodyDesc.pParentsTransform = m_pTransformCom;
		BodyDesc.pSoundCueSign = &m_bSoundCueSign;
		BodyDesc.pState = &m_eZombieState;
		BodyDesc.strModelComponentName = m_tagPropDesc.strModelComponent;
		BodyDesc.iPropType = m_tagPropDesc.iPropType;
		pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(m_tagPropDesc.strObjectPrototype, &BodyDesc));
		if (nullptr == pBodyObj)
			return E_FAIL;
		m_PartObjects[CChair::PART_BODY] = pBodyObj;

	}
	break;

	case CHAIR_BARRIGATE:
	{
		CPartObject* pBodyObj = { nullptr };
		CBody_Chair::PART_INTERACTPROPS_DESC BodyDesc = {};
		BodyDesc.pParentsTransform = m_pTransformCom;
		BodyDesc.pSoundCueSign = &m_bSoundCueSign;
		BodyDesc.pState = &m_eBarriState;
		BodyDesc.strModelComponentName = m_tagPropDesc.strModelComponent;
		BodyDesc.iPropType = m_tagPropDesc.iPropType;
		pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(m_tagPropDesc.strObjectPrototype, &BodyDesc));
		if (nullptr == pBodyObj)
			return E_FAIL;
		m_PartObjects[CChair::PART_BODY] = pBodyObj;

	}
	break;
	}


	return S_OK;
}

HRESULT CChair::Initialize_PartObjects()
{
	return S_OK;
}

HRESULT CChair::Bind_ShaderResources()
{
	return S_OK;
}


void CChair::Zombie_Tick(_float fTimeDelta)
{
	
}

void CChair::Barrigate_Tick(_float fTimeDelta)
{
	if (m_bCol[INTER_COL_NORMAL][COL_STEP1] && !m_bActivity)
	{
		if (*m_pPlayerInteract)
			Barrigate_Active();
	}
}



void CChair::Barrigate_Active()
{
	*m_pPlayerInteract = false;
	m_bActivity = true;
	
	m_eBarriState = BARRI_MOVE;
	m_pDoor->Set_Lock();

	m_pPlayer->Set_Lever_Setting(CPlayer::LEVER_BEHAVE_DOWN, m_pTransformCom->Get_WorldFloat4x4());
	
}

CChair* CChair::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CChair* pInstance = new CChair(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CChair"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CChair::Clone(void* pArg)
{
	CChair* pInstance = new CChair(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CChair"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChair::Free()
{
	__super::Free();

}

_float4 CChair::Get_Object_Pos()
{
	return static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos();
}
