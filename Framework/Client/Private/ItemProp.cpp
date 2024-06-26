#include "stdafx.h"
#include "ItemProp.h"
#include"Player.h"

#include"Body_ItemProp.h"
CItemProp::CItemProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CItemProp::CItemProp(const CItemProp& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CItemProp::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItemProp::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;
	if (!m_tagPropDesc.bAnim)
		m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
	return S_OK;
}

void CItemProp::Tick(_float fTimeDelta)
{
	__super::Check_Player();

	if (m_bDead)
		return;

	if (!m_bVisible)
		return;
#ifdef _DEBUG
#ifdef UI_POS
	Get_Object_Pos();
#endif
#endif


	if (m_pPlayer == nullptr)
		return;
	if (m_bCol)
	{
		//UI¶ç¿ì°í
		if (*m_pPlayerInteract)
			Active();
		m_bCol = false;
	}
	m_pColliderCom[INTERACTPROPS_COL_SPHERE]->Tick(m_pTransformCom->Get_WorldMatrix());

	__super::Tick(fTimeDelta);


}

void CItemProp::Late_Tick(_float fTimeDelta)
{
	if (m_bDead)
		return;

	if (!Visible())
		return;
	if (m_bRender == false)
		return;
	Check_Col_Sphere_Player();
	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom[INTERACTPROPS_COL_SPHERE]);
#endif
}

HRESULT CItemProp::Render()
{
	return S_OK;
}

HRESULT CItemProp::Add_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(100.f);
	ColliderDesc.vCenter = _float3(0.f, -50.f, 0.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom[INTERACTPROPS_COL_SPHERE], &ColliderDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CItemProp::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CItemProp::PART_END);

	/*Part_Body*/
	CPartObject* pBodyObj = { nullptr };
	CBody_ItemProp::PART_INTERACTPROPS_DESC BodyDesc = {};
	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pState = &m_eState;
	BodyDesc.strModelComponentName = TEXT("Prototype_Component_Model_") + m_tagPropDesc.tagItemDesc.Name;
	pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_") + m_tagPropDesc.tagItemDesc.Name, &BodyDesc));
	if (nullptr == pBodyObj)
		return E_FAIL;

	m_PartObjects[CItemProp::PART_BODY] = pBodyObj;




	return S_OK;
}

HRESULT CItemProp::Initialize_PartObjects()
{

	return S_OK;
}

HRESULT CItemProp::Bind_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

_float4 CItemProp::Get_Object_Pos()
{
	if (!m_bDead)
		return static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos();
	else
		return _float4();
}

void CItemProp::Active()
{
	*m_pPlayerInteract = false;
	m_bDead = true;

}

CItemProp* CItemProp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItemProp* pInstance = new CItemProp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CItemProp"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CItemProp::Clone(void* pArg)
{
	CItemProp* pInstance = new CItemProp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CItemProp"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItemProp::Free()
{
	__super::Free();

}
