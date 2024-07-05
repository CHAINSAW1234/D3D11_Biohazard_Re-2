#include "stdafx.h"
#include "MovingShelf.h"
#include"Player.h"
#include "PxCollider.h"
#include "Bone.h"

//part-obj
#include"Body_MovingShelf.h"
CMovingShelf::CMovingShelf(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CMovingShelf::CMovingShelf(const CMovingShelf& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CMovingShelf::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMovingShelf::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;	
	
	if (FAILED(Initialize_PartObjects()))
		return E_FAIL;	



	return S_OK;
}

void CMovingShelf::Tick(_float fTimeDelta)
{
	__super::Tick_Col();
	if (!m_bVisible)
		return;
	
#ifdef _DEBUG
#ifdef UI_POS
	Get_Object_Pos();
#endif
#endif
	if (m_bActivity)
		m_fTimeDelay += fTimeDelta;
	if (m_fTimeDelay > 1.f)
	{
		m_bActivity = false;
		m_fTimeDelay = 0.f;
	}


	if ((m_bCol[INTER_COL_NORMAL][COL_STEP1]|| m_bCol[INTER_COL_DOUBLE][COL_STEP1]) && !m_bActivity)
	{
		if (*m_pPlayerInteract)
			Active();
	}
	__super::Tick(fTimeDelta);
}

void CMovingShelf::Late_Tick(_float fTimeDelta)
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
	if (Activate_Col(Get_Collider_World_Pos(_float4(50.f, 1.f, 50.f,1.f)))|| Activate_Col(Get_Collider_World_Pos(_float4(-50.f, 1.f, 50.f, 1.f))))
	{
		if (Check_Col_Player(INTER_COL_NORMAL, COL_STEP0))
			Check_Col_Player(INTER_COL_NORMAL, COL_STEP1);
		else
			m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
	}
	else
	{
		m_bCol[INTER_COL_NORMAL][COL_STEP0] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;
	}

	__super::Late_Tick(fTimeDelta);


#ifdef _DEBUG
	__super::Add_Col_DebugCom();
#endif
}

HRESULT CMovingShelf::Render()
{
	return S_OK;
}

HRESULT CMovingShelf::Add_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(80.f);
	ColliderDesc.vCenter = _float3(50.f, 1.f, 50.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Normal_Step0"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP0], &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.fRadius = _float(50.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Normal_Step1"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP1], &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CMovingShelf::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CMovingShelf::PART_END);

	/*Part_Body*/
	CPartObject* pBodyObj = { nullptr };
	CBody_MovingShlef::PART_INTERACTPROPS_DESC BodyDesc = {};
	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pState = &m_eState;
	BodyDesc.strModelComponentName = m_tagPropDesc.strModelComponent;
	BodyDesc.iPropType = m_tagPropDesc.iPropType;
	pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(m_tagPropDesc.strObjectPrototype, &BodyDesc));
	if (nullptr == pBodyObj)
		return E_FAIL;

	m_PartObjects[CMovingShelf::PART_BODY] = pBodyObj;
	


	return S_OK;
}

HRESULT CMovingShelf::Initialize_PartObjects()
{

	return S_OK;
}

HRESULT CMovingShelf::Bind_ShaderResources()
{
	return S_OK;
}

void CMovingShelf::Active()
{
	*m_pPlayerInteract = false;
	m_bActivity = true;
	
	m_eState = SHELF_UPLRIGHT;

}

CMovingShelf* CMovingShelf::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMovingShelf* pInstance = new CMovingShelf(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMovingShelf"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CMovingShelf::Clone(void* pArg)
{
	CMovingShelf* pInstance = new CMovingShelf(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CMovingShelf"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMovingShelf::Free()
{
	__super::Free();

}

_float4 CMovingShelf::Get_Object_Pos()
{
	
	return;

}
