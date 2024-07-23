#include "stdafx.h"
#include "Actor_SM69_015.h"
#include "Actor_PartObject.h"

CActor_SM69_015::CActor_SM69_015(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActor{ pDevice, pContext }
{
}

CActor_SM69_015::CActor_SM69_015(const CActor& rhs)
	: CActor{ rhs }
{
}

HRESULT CActor_SM69_015::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_SM69_015::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_SM69_015::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_SM69_015::Add_PartObjects()
{
	m_PartObjects.resize(static_cast<size_t>(ACTOR_SM69_015_PART::_END));

	CActor_PartObject::ACTOR_PART_DESC			Head_Desc;
	Head_Desc.pParentsTransform = m_pTransformCom;
	Head_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_SM69_015");
	Head_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF94_SM69_015_00"));

	CActor_PartObject* pPartObject_Head = { CActor_PartObject::Create(m_pDevice, m_pContext, &Head_Desc) };
	if (nullptr == pPartObject_Head)
		return E_FAIL;

	m_PartObjects[static_cast<_uint>(ACTOR_SM69_015_PART::_HEAD)] = pPartObject_Head;

	return S_OK;
}

CActor_SM69_015* CActor_SM69_015::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CActor_SM69_015* pInstance = new CActor_SM69_015(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CActor_SM69_015"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CActor_SM69_015::Clone(void* pArg)
{
	CActor_SM69_015* pInstance = new CActor_SM69_015(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CActor_SM69_015"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CActor_SM69_015::Free()
{
	__super::Free();
}
