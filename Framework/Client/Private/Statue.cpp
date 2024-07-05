#include "stdafx.h"
#include "Statue.h"
#include"Player.h"

#include"Body_Statue.h"

CStatue::CStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CStatue::CStatue(const CStatue& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CStatue::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStatue::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	return S_OK;
}

void CStatue::Tick(_float fTimeDelta)
{
	__super::Tick_Col();

	if (!m_bVisible)
		return;

	__super::Tick(fTimeDelta);

}

void CStatue::Late_Tick(_float fTimeDelta)
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

HRESULT CStatue::Render()
{
	return S_OK;
}

HRESULT CStatue::Add_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(150.f);
	ColliderDesc.vCenter = _float3(-10.f, 1.f, 0.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Normal_Step0"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP0], &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.fRadius = _float(100.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Normal_Step1"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP1], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStatue::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CStatue::PART_END);

	/*Part_Body*/
	CPartObject* pBodyObj = { nullptr };
	CBody_Statue::PART_INTERACTPROPS_DESC BodyDesc = {};
	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pState = &m_eState;
	BodyDesc.strModelComponentName = m_tagPropDesc.strModelComponent;
	pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(m_tagPropDesc.strObjectPrototype, &BodyDesc));
	if (nullptr == pBodyObj)
		return E_FAIL;

	m_PartObjects[CStatue::PART_BODY] = pBodyObj;

	/*PART_PART*/
	m_PartObjects[CStatue::PART_PART] = nullptr;


	return S_OK;
}

HRESULT CStatue::Initialize_PartObjects()
{

	return S_OK;
}

HRESULT CStatue::Bind_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

void CStatue::Active()
{
	*m_pPlayerInteract = false;
	m_bActivity = true;

}

_float4 CStatue::Get_Object_Pos()
{
	return _float4();
}

CStatue* CStatue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStatue* pInstance = new CStatue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStatue"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CStatue::Clone(void* pArg)
{
	CStatue* pInstance = new CStatue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CStatue"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatue::Free()
{
	__super::Free();

}
