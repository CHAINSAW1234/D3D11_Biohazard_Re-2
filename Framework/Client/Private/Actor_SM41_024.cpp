#include "stdafx.h"
#include "Actor_SM41_024.h"
#include "Actor_PartObject.h"

CActor_SM41_024::CActor_SM41_024(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActor{ pDevice, pContext }
{
}

CActor_SM41_024::CActor_SM41_024(const CActor& rhs)
	: CActor{ rhs }
{
}

HRESULT CActor_SM41_024::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_SM41_024::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_SM41_024::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_SM41_024::Add_PartObjects()
{
	m_PartObjects.resize(static_cast<size_t>(ACTOR_SM41_024_PART::_END));

	CActor_PartObject::ACTOR_PART_DESC			Body_Desc;
	Body_Desc.pParentsTransform = m_pTransformCom;
	Body_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_SM41_024");
	Body_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF150_SM41_024"));
	Body_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF151_SM41_024"));
	Body_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF152_SM41_024"));
	Body_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF153_SM41_024"));
	Body_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF190_SM41_024"));

	CActor_PartObject* pPartObject_Body = { CActor_PartObject::Create(m_pDevice, m_pContext, &Body_Desc) };
	if (nullptr == pPartObject_Body)
		return E_FAIL;

	m_PartObjects[static_cast<_uint>(ACTOR_SM41_024_PART::_BODY)] = pPartObject_Body;

	return S_OK;
}

CActor_SM41_024* CActor_SM41_024::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CActor_SM41_024* pInstance = new CActor_SM41_024(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CActor_SM41_024"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CActor_SM41_024::Clone(void* pArg)
{
	CActor_SM41_024* pInstance = new CActor_SM41_024(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CActor_SM41_024"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CActor_SM41_024::Free()
{
	__super::Free();
}
