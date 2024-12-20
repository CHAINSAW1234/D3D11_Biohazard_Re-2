#include "stdafx.h"
#include "Actor_EM00.h"
#include "Actor_PartObject.h"

CActor_EM00::CActor_EM00(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActor{ pDevice, pContext }
{
}

CActor_EM00::CActor_EM00(const CActor& rhs)
	: CActor{ rhs }
{
}

HRESULT CActor_EM00::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_EM00::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	ACTOR_EM00_DESC*		pDesc = { static_cast<ACTOR_EM00_DESC*>(pArg) };
	m_eFaceType = pDesc->eFaceType;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CModel*			pBody_Model = { dynamic_cast<CModel*>(m_PartObjects[static_cast<_uint>(ACTOR_EM00_PART::_BODY)]->Get_Component(TEXT("Com_Model"))) };
	vector<string>			MeshTags = { pBody_Model->Get_MeshTags() };
	vector<string>			ResultMeshTags;
	for (auto& strMeshTag : MeshTags)
	{
		if (strMeshTag.find("Body") != string::npos)
		{
			ResultMeshTags.push_back(strMeshTag);
		}
	}

	for (auto& strMeshTag : MeshTags)
	{
		pBody_Model->Hide_Mesh(strMeshTag, true);
	}

	for (auto& strMeshTag : ResultMeshTags)
	{
		pBody_Model->Hide_Mesh(strMeshTag, false);
	}

	return S_OK;
}

HRESULT CActor_EM00::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_EM00::Add_PartObjects()
{
	m_PartObjects.resize(static_cast<size_t>(ACTOR_EM00_PART::_END));

	CActor_PartObject::ACTOR_PART_DESC			Body_Desc;
	Body_Desc.pParentsTransform = m_pTransformCom;
	Body_Desc.pRootTranslation = &m_vRootTranslation;
	Body_Desc.isBaseObject = true;
	Body_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_Zombie_Body_Male");
	Body_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF93_EM0000"));
	Body_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF94_EM0000"));
	Body_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF95_EM0000"));

	CActor_PartObject* pPartObject_Body = { CActor_PartObject::Create(m_pDevice, m_pContext, &Body_Desc) };
	if (nullptr == pPartObject_Body)
		return E_FAIL;

	m_PartObjects[static_cast<_uint>(ACTOR_EM00_PART::_BODY)] = pPartObject_Body;

	CActor_PartObject::ACTOR_PART_DESC			Head_Desc;
	Head_Desc.pParentsTransform = m_pTransformCom;
	if (ACTOR_EM00_FACE_TYPE::_DEFAULT == m_eFaceType)
	{
		Head_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_Zombie_Face00_Male");
		Head_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF94_EM0050"));
	}
	else if (ACTOR_EM00_FACE_TYPE::_BROKEN == m_eFaceType)
	{
		Head_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_EM0000_Face11");
		Head_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF95_EM0050"));
	}

	CActor_PartObject* pPartObject_Head = { CActor_PartObject::Create(m_pDevice, m_pContext, &Head_Desc) };
	if (nullptr == pPartObject_Head)
		return E_FAIL;

	m_PartObjects[static_cast<_uint>(ACTOR_EM00_PART::_HEAD)] = pPartObject_Head;

	CActor_PartObject::ACTOR_PART_DESC			Shirts_Desc;
	Shirts_Desc.pParentsTransform = m_pTransformCom;
	Shirts_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_Zombie_Shirts00_Male");

	CActor_PartObject* pPartObject_Shirts = { CActor_PartObject::Create(m_pDevice, m_pContext, &Shirts_Desc) };
	if (nullptr == pPartObject_Shirts)
		return E_FAIL;

	m_PartObjects[static_cast<_uint>(ACTOR_EM00_PART::_SHIRTS)] = pPartObject_Shirts;

	CActor_PartObject::ACTOR_PART_DESC			Pants_Desc;
	Pants_Desc.pParentsTransform = m_pTransformCom;
	Pants_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_Zombie_Pants00_Male");

	CActor_PartObject* pPartObject_Pants = { CActor_PartObject::Create(m_pDevice, m_pContext, &Pants_Desc) };
	if (nullptr == pPartObject_Pants)
		return E_FAIL;

	m_PartObjects[static_cast<_uint>(ACTOR_EM00_PART::_PANTS)] = pPartObject_Pants;

	CModel* pBodyModel = { static_cast<CModel*>(m_PartObjects[static_cast<_uint>(ACTOR_EM00_PART::_BODY)]->Get_Component(TEXT("Com_Model"))) };
	CModel* pHeadModel = { static_cast<CModel*>(m_PartObjects[static_cast<_uint>(ACTOR_EM00_PART::_HEAD)]->Get_Component(TEXT("Com_Model"))) };
	CModel* pShirtsModel = { static_cast<CModel*>(m_PartObjects[static_cast<_uint>(ACTOR_EM00_PART::_SHIRTS)]->Get_Component(TEXT("Com_Model"))) };
	CModel* pPantsModel = { static_cast<CModel*>(m_PartObjects[static_cast<_uint>(ACTOR_EM00_PART::_PANTS)]->Get_Component(TEXT("Com_Model"))) };
	pHeadModel->Link_Bone_Auto(pBodyModel);
	pShirtsModel->Link_Bone_Auto(pBodyModel);
	pPantsModel->Link_Bone_Auto(pBodyModel);

	return S_OK;
}

CActor_EM00* CActor_EM00::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CActor_EM00* pInstance = new CActor_EM00(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CActor_EM00"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CActor_EM00::Clone(void* pArg)
{
	CActor_EM00* pInstance = new CActor_EM00(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CActor_EM00"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CActor_EM00::Free()
{
	__super::Free();
}
