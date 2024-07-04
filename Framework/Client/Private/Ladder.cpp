#include "stdafx.h"
#include "Ladder.h"
#include"Player.h"

#include"Body_Statue.h"

CLadder::CLadder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CLadder::CLadder(const CLadder& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CLadder::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLadder::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	return S_OK;
}

void CLadder::Tick(_float fTimeDelta)
{
	__super::Tick_Col();
	if (m_bActivity)
		m_fTimeDelay += fTimeDelta;

	if (m_fTimeDelay > 1.f)
	{
		m_fTimeDelay = 0.f;
		m_bActivity = false;

	}


	
	
	if (m_bCol[INTER_COL_NORMAL][COL_STEP1] || m_bCol[INTER_COL_DOUBLE][COL_STEP1])
		if (!m_bActivity && (*m_pPlayerInteract || m_bCol[INTER_COL_NORMAL][COL_STEP2] || m_bCol[INTER_COL_DOUBLE][COL_STEP2]))
			Active();

	if (!m_bVisible)
		return;
	
	__super::Tick(fTimeDelta);

}

void CLadder::Late_Tick(_float fTimeDelta)
{
	if (m_pPlayer == nullptr)
		return;
	if (!Visible())
		return;

	if (Activate_Col(Get_Collider_World_Pos(_float4(0.f, -0.5f, -0.2f, 1.f))) || Activate_Col(Get_Collider_World_Pos(_float4(0.1f, 5.8f, 0.f, 1.f))))
	{
		if (Check_Col_Player(INTER_COL_NORMAL, COL_STEP0))
		{
			if (Check_Col_Player(INTER_COL_NORMAL, COL_STEP1))
				Check_Col_Player(INTER_COL_NORMAL, COL_STEP2);
			else
				m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;
		}
		else
		{
			m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
			m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;

		}


		if (Check_Col_Player(INTER_COL_DOUBLE, COL_STEP0))
		{
			if (Check_Col_Player(INTER_COL_DOUBLE, COL_STEP1))
				Check_Col_Player(INTER_COL_DOUBLE, COL_STEP2);
			else
				m_bCol[INTER_COL_DOUBLE][COL_STEP2] = false;
		}
		else
		{
			m_bCol[INTER_COL_DOUBLE][COL_STEP1] = false;
			m_bCol[INTER_COL_DOUBLE][COL_STEP2] = false;

		}

	}
	else
	{
		m_bCol[INTER_COL_NORMAL][COL_STEP0] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;
		m_bCol[INTER_COL_DOUBLE][COL_STEP0] = false;
		m_bCol[INTER_COL_DOUBLE][COL_STEP1] = false;
		m_bCol[INTER_COL_DOUBLE][COL_STEP2] = false;
	}


	if (m_bRender == false)
		return;
	else
	{
		for (auto& it : m_PartObjects)
		{
			if (it != nullptr)
				it->Set_Render(true);
		}

		m_bRender = false;
	}
	__super::Late_Tick(fTimeDelta);


#ifdef _DEBUG
	__super::Add_Col_DebugCom();
#endif
}

HRESULT CLadder::Render()
{
	return S_OK;
}

HRESULT CLadder::Add_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(1.5f);
	ColliderDesc.vCenter = _float3(0.f, -0.1f, -0.2f);
	/* For.Com_Collider */

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Normal_Step0"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP0], &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.fRadius = _float(1.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Normal_Step1"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP1], &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.fRadius = _float(0.5f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Normal_Step2"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP2], &ColliderDesc)))
		return E_FAIL;
	

	ColliderDesc.fRadius = _float(1.5f);
	ColliderDesc.vCenter = _float3(-0.1f, 5.3f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Double_Step0"), (CComponent**)&m_pColliderCom[INTER_COL_DOUBLE][COL_STEP0], &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.fRadius = _float(1.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Double_Step1"), (CComponent**)&m_pColliderCom[INTER_COL_DOUBLE][COL_STEP1], &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.fRadius = _float(0.5f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Double_Step2"), (CComponent**)&m_pColliderCom[INTER_COL_DOUBLE][COL_STEP2], &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLadder::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CLadder::PART_END);

	/*Part_Body*/
	CPartObject* pBodyObj = { nullptr };
	CBody_Statue::PART_INTERACTPROPS_DESC BodyDesc = {};
	BodyDesc.pParentsTransform = m_pTransformCom;
	//BodyDesc.pState = &m_eState;
	BodyDesc.strModelComponentName = m_tagPropDesc.strModelComponent;
	pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(m_tagPropDesc.strObjectPrototype, &BodyDesc));
	if (nullptr == pBodyObj)
		return E_FAIL;

	m_PartObjects[CLadder::PART_BODY] = pBodyObj;

	return S_OK;
}

HRESULT CLadder::Initialize_PartObjects()
{

	return S_OK;
}

HRESULT CLadder::Bind_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

void CLadder::Active()
{
	*m_pPlayerInteract = false;
	m_bActivity = true;
	if (m_bCol[INTER_COL_DOUBLE][COL_STEP1])
		m_pPlayer->Set_Ladder_Setting(CPlayer::LADDER_BEHAVE_DOWN, m_pTransformCom->Get_WorldFloat4x4());
	else if (m_bCol[INTER_COL_NORMAL][COL_STEP1])
		m_pPlayer->Set_Ladder_Setting(CPlayer::LADDER_BEHAVE_UP, m_pTransformCom->Get_WorldFloat4x4());
	//--m_iActive;
}

_float4 CLadder::Get_Object_Pos()
{

	return _float4();
}

CLadder* CLadder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLadder* pInstance = new CLadder(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CLadder"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CLadder::Clone(void* pArg)
{
	CLadder* pInstance = new CLadder(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CLadder"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLadder::Free()
{
	__super::Free();

}
