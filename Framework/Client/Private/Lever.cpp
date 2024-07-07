#include "stdafx.h"
#include "Lever.h"
#include"Player.h"
#include "PxCollider.h"
#include "Bone.h"

//part-obj
#include"Body_Lever.h"
#include"Shutter.h"
CLever::CLever(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CLever::CLever(const CLever& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CLever::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLever::Initialize(void* pArg)
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

void CLever::Start()
{
	__super::Start();
	list<CGameObject*>* pShutter = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_HShutter"));
	if (pShutter == nullptr)
		return;
	for (auto& iter : *pShutter)
		if (m_pColliderCom[INTER_COL_NORMAL][COL_STEP0]->Intersect(static_cast<CCollider*>(iter->Get_Component(TEXT("Com_Collider_Normal_Step0")))))
		{
			m_pShutter = static_cast<CShutter*>(iter);
			break;
		}
		
}

void CLever::Tick(_float fTimeDelta)
{
	__super::Tick_Col();
	if (!m_bVisible)
		return;
	
#ifdef _DEBUG
#ifdef UI_POS
	Get_Object_Pos();
#endif
#endif

	if (m_eState == LEVER_RESET)
	{
		__super::Tick(fTimeDelta);
		return;
	}
	_bool bTrue = { false };
	if (m_bActivity)
		m_fTimeDelay += fTimeDelta;
	if (m_fTimeDelay > 3.5f)
	{
		m_fTimeDelay = 0.f;
		bTrue = true;
	}
	if (m_eState == LEVER_DOWN && bTrue)
	{
		m_eState = LEVER_RESET;
		return;
	}
	if (m_fTimeDelay > 2.f)
	{
		m_pShutter->Set_Shutter_Open_State();
		return;
	}

	


	if (m_bCol[INTER_COL_NORMAL][COL_STEP1] && !m_bActivity)
	{
		if (*m_pPlayerInteract)
			Active();
	}
	__super::Tick(fTimeDelta);
}

void CLever::Late_Tick(_float fTimeDelta)
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
	if (Activate_Col(Get_Collider_World_Pos(_float4(50.f, 1.f, 50.f,1.f))))
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
	}

	__super::Late_Tick(fTimeDelta);


#ifdef _DEBUG
	__super::Add_Col_DebugCom();
#endif
}

HRESULT CLever::Render()
{
	return S_OK;
}

HRESULT CLever::Add_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(150.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Normal_Step0"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP0], &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.fRadius = _float(120.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Normal_Step1"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP1], &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLever::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CLever::PART_END);

	/*Part_Body*/
	CPartObject* pBodyObj = { nullptr };
	CBody_Lever::PART_INTERACTPROPS_DESC BodyDesc = {};
	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pState = &m_eState;
	BodyDesc.strModelComponentName = m_tagPropDesc.strModelComponent;
	BodyDesc.iPropType = m_tagPropDesc.iPropType;
	pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(m_tagPropDesc.strObjectPrototype, &BodyDesc));
	if (nullptr == pBodyObj)
		return E_FAIL;

	m_PartObjects[CLever::PART_BODY] = pBodyObj;

	return S_OK;
}

HRESULT CLever::Initialize_PartObjects()
{

	return S_OK;
}

HRESULT CLever::Bind_ShaderResources()
{
	return S_OK;
}

void CLever::Active()
{
	*m_pPlayerInteract = false;
	m_bActivity = true;
	
	m_eState = LEVER_DOWN;
	m_pPlayer->Set_Lever_Setting(CPlayer::LEVER_BEHAVE_DOWN, m_pTransformCom->Get_WorldFloat4x4());
	
}

CLever* CLever::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLever* pInstance = new CLever(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CLever"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CLever::Clone(void* pArg)
{
	CLever* pInstance = new CLever(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CLever"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLever::Free()
{
	__super::Free();

}

_float4 CLever::Get_Object_Pos()
{
	return static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos();
}
