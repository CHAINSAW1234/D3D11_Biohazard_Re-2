#include "stdafx.h"
#include "Actor_PL00.h"
#include "Actor_PartObject.h"

CActor_PL00::CActor_PL00(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActor{ pDevice, pContext }
{
}

CActor_PL00::CActor_PL00(const CActor& rhs)
	: CActor{ rhs }
{
}

HRESULT CActor_PL00::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_PL00::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_PL00::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_PL00::Add_PartObjects()
{
	m_PartObjects.resize(static_cast<size_t>(ACTOR_PL00_PART::_END));

	CActor_PartObject::ACTOR_PART_DESC			Body_Desc;
	Body_Desc.pParentsTransform = m_pTransformCom;
	Body_Desc.pRootTranslation = &m_vRootTranslation;
	Body_Desc.isBaseObject = true;
	Body_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_LeonBody");
	Body_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF93_PL0000"));

	CActor_PartObject* pPartObject_Body = { CActor_PartObject::Create(m_pDevice, m_pContext, &Body_Desc) };
	if (nullptr == pPartObject_Body)
		return E_FAIL;

	m_PartObjects[static_cast<_uint>(ACTOR_PL00_PART::_BODY)] = pPartObject_Body;

	CActor_PartObject::ACTOR_PART_DESC			Head_Desc;
	Head_Desc.pParentsTransform = m_pTransformCom;
	Head_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_LeonFace");
	Head_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF93_PL0050"));

	CActor_PartObject* pPartObject_Head = { CActor_PartObject::Create(m_pDevice, m_pContext, &Head_Desc) };
	if (nullptr == pPartObject_Head)
		return E_FAIL;

	m_PartObjects[static_cast<_uint>(ACTOR_PL00_PART::_HEAD)] = pPartObject_Head;

	CActor_PartObject::ACTOR_PART_DESC			Hair_Desc;
	Hair_Desc.pParentsTransform = m_pTransformCom;
	Hair_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_LeonHair");

	CActor_PartObject* pPartObject_Hair = { CActor_PartObject::Create(m_pDevice, m_pContext, &Hair_Desc) };
	if (nullptr == pPartObject_Hair)
		return E_FAIL;

	m_PartObjects[static_cast<_uint>(ACTOR_PL00_PART::_HAIR)] = pPartObject_Hair;

	return S_OK;
}

CActor_PL00* CActor_PL00::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CActor_PL00* pInstance = new CActor_PL00(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CActor_PL00"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CActor_PL00::Clone(void* pArg)
{
	CActor_PL00* pInstance = new CActor_PL00(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CActor_PL00"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CActor_PL00::Free()
{
	__super::Free();
}
