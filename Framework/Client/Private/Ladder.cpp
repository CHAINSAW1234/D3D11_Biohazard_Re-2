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
	__super::Check_Player();
	m_pColliderCom[INTERACTPROPS_COL_SPHERE]->Tick(m_pTransformCom->Get_WorldMatrix());
	m_pColliderCom[INTERACTPROPS_COL_AABB]->Tick(m_pTransformCom->Get_WorldMatrix());

	if (!m_bVisible)
		return;
	

	if (m_pPlayer == nullptr)
		return;
	
	if (m_bCol || m_bDownCol)
	{
		//UI띄우고
		if (*m_pPlayerInteract)
			Active();
		m_bCol = false;
	}
	
	
	__super::Tick(fTimeDelta);

}

void CLadder::Late_Tick(_float fTimeDelta)
{
	if (!Visible())
		return;

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
	m_bCol =  Check_Col_Sphere_Player();
	m_bDownCol = Check_Col_AABB_Player();

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom[INTERACTPROPS_COL_SPHERE]);
	m_pGameInstance->Add_DebugComponents(m_pColliderCom[INTERACTPROPS_COL_AABB]);
#endif
}

HRESULT CLadder::Render()
{
	return S_OK;
}

HRESULT CLadder::Add_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(100.f);
	ColliderDesc.vCenter = _float3(0.f, 1.f, 0.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Shpere"), (CComponent**)&m_pColliderCom[INTERACTPROPS_COL_SPHERE], &ColliderDesc)))
		return E_FAIL;	
	
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderDesc_aabb{};

	ColliderDesc_aabb.vCenter = _float3(0.f, 1.f, 0.f);
	ColliderDesc_aabb.vSize = _float3(100.f, 100.f, 100.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_AABB"), (CComponent**)&m_pColliderCom[INTERACTPROPS_COL_AABB], &ColliderDesc_aabb)))
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
	m_bActive = true;
	if (m_bDownCol)
	{
		//밑으로 가게 하기
	}
	else
	{
		// 위로 가게 하기
	}
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
