#include "stdafx.h"
#include "EventProp.h"
#include"Player.h"

#include"Body_EventProp.h"
CEventProp::CEventProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CEventProp::CEventProp(const CEventProp& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CEventProp::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEventProp::Initialize(void* pArg)
{
	/*문자식 파트오브젝트 붙혀야하는데 뼈가 문고리에 없어서 직접 찍어야 하는데
	프로토타입 끝나고 뼈 붙혀보겠나이다*/

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	return S_OK;
}

void CEventProp::Tick(_float fTimeDelta)
{
	__super::Check_Player();
	if (!m_bVisible)
	{
		m_pColliderCom[INTERACTPROPS_COL_SPHERE]->Tick(m_pTransformCom->Get_WorldMatrix());

		return;
	}

	if (m_pPlayer == nullptr)
		return;
	__super::Tick(fTimeDelta);

	m_pColliderCom[INTERACTPROPS_COL_SPHERE]->Tick(m_pTransformCom->Get_WorldMatrix());


}

void CEventProp::Late_Tick(_float fTimeDelta)
{
	if (!Visible())
		return;

	if (m_bRender == false)
		return;
	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom[INTERACTPROPS_COL_SPHERE]);
#endif
}

HRESULT CEventProp::Render()
{
	return S_OK;
}

HRESULT CEventProp::Add_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(100.f);
	ColliderDesc.vCenter = _float3(-10.f, 1.f, 0.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom[INTERACTPROPS_COL_SPHERE], &ColliderDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CEventProp::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CEventProp::PART_END);

	/*Part_Body*/
	CPartObject* pBodyObj = { nullptr };
	CBody_EventProp::PART_INTERACTPROPS_DESC BodyDesc = {};
	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pState = &m_eState;
	BodyDesc.strModelComponentName = m_tagPropDesc.strModelComponent;
	pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(m_tagPropDesc.strObjectPrototype, &BodyDesc));
	if (nullptr == pBodyObj)
		return E_FAIL;

	m_PartObjects[CEventProp::PART_BODY] = pBodyObj;


	return S_OK;
}

HRESULT CEventProp::Initialize_PartObjects()
{

	return S_OK;
}

HRESULT CEventProp::Bind_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

void CEventProp::Active()
{
	*m_pPlayerInteract = false;
	m_bActive = true;





}

CEventProp* CEventProp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEventProp* pInstance = new CEventProp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEventProp"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CEventProp::Clone(void* pArg)
{
	CEventProp* pInstance = new CEventProp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CEventProp"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEventProp::Free()
{
	__super::Free();

}
