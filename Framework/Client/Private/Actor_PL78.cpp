#include "stdafx.h"
#include "Actor_PL78.h"
#include "Actor_PartObject.h"

CActor_PL78::CActor_PL78(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActor{ pDevice, pContext }
{
}

CActor_PL78::CActor_PL78(const CActor& rhs)
	: CActor{ rhs }
{
}

HRESULT CActor_PL78::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_PL78::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_PL78::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_PL78::Add_PartObjects()
{
	m_PartObjects.resize(static_cast<size_t>(ACTOR_PL78_PART::_END));

	CActor_PartObject::ACTOR_PART_DESC			Body_Desc;
	Body_Desc.pParentsTransform = m_pTransformCom;
	Body_Desc.pRootTranslation = &m_vRootTranslation;
	Body_Desc.isBaseObject = true;
	Body_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_PL7800");
	Body_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF93_PL7800"));

	CActor_PartObject*			pPartObject_Body = { CActor_PartObject::Create(m_pDevice, m_pContext, &Body_Desc) };
	if (nullptr == pPartObject_Body)
		return E_FAIL;

	m_PartObjects[static_cast<_uint>(ACTOR_PL78_PART::_BODY)] = pPartObject_Body;

	CActor_PartObject::ACTOR_PART_DESC			Head_Desc;
	Head_Desc.pParentsTransform = m_pTransformCom;
	Head_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_PL7850");
	Head_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF93_PL7850"));

	CActor_PartObject*			pPartObject_Head = { CActor_PartObject::Create(m_pDevice, m_pContext, &Head_Desc) };
	if (nullptr == pPartObject_Head)
		return E_FAIL;

	m_PartObjects[static_cast<_uint>(ACTOR_PL78_PART::_HEAD)] = pPartObject_Head;

	CActor_PartObject::ACTOR_PART_DESC			Hair_Desc;
	Hair_Desc.pParentsTransform = m_pTransformCom;
	Hair_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_PL7870");

	CActor_PartObject*			pPartObject_Hair = { CActor_PartObject::Create(m_pDevice, m_pContext, &Hair_Desc) };
	if (nullptr == pPartObject_Hair)
		return E_FAIL;

	m_PartObjects[static_cast<_uint>(ACTOR_PL78_PART::_HAIR)] = pPartObject_Hair;

	CActor_PartObject::ACTOR_PART_DESC			Guts_Desc;
	Guts_Desc.pParentsTransform = m_pTransformCom;
	Guts_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_PL7880");
	Guts_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF93_PL7880"));

	CActor_PartObject*			pPartObject_Guts = { CActor_PartObject::Create(m_pDevice, m_pContext, &Guts_Desc) };
	if (nullptr == pPartObject_Guts)
		return E_FAIL;

	m_PartObjects[static_cast<_uint>(ACTOR_PL78_PART::_GUTS)] = pPartObject_Guts;

	return S_OK;
}

CActor_PL78* CActor_PL78::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CActor_PL78*			pInstance = new CActor_PL78(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CActor_PL78"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CActor_PL78::Clone(void* pArg)
{
	CActor_PL78* pInstance = new CActor_PL78(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CActor_PL78"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CActor_PL78::Free()
{
	__super::Free();
}
