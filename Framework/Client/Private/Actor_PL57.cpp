#include "stdafx.h"
#include "Actor_PL57.h"
#include "Actor_PartObject.h"

CActor_PL57::CActor_PL57(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActor{ pDevice, pContext }
{
}

CActor_PL57::CActor_PL57(const CActor& rhs)
	: CActor{ rhs }
{
}

HRESULT CActor_PL57::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_PL57::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_PL57::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_PL57::Add_PartObjects()
{
	m_PartObjects.resize(static_cast<size_t>(ACTOR_PL57_PART::_END));

	CActor_PartObject::ACTOR_PART_DESC			Body_Desc;
	Body_Desc.pParentsTransform = m_pTransformCom;
	Body_Desc.pRootTranslation = &m_vRootTranslation;
	Body_Desc.isBaseObject = true;
	Body_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_PL5700");
	Body_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF94_PL5700"));
	Body_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF120_PL5700"));
	Body_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF190_PL5700"));
	Body_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("Body_PL5700"));

	CActor_PartObject* pPartObject_Body = { CActor_PartObject::Create(m_pDevice, m_pContext, &Body_Desc) };
	if (nullptr == pPartObject_Body)
		return E_FAIL;

	m_PartObjects[static_cast<_uint>(ACTOR_PL57_PART::_BODY)] = pPartObject_Body;

	CActor_PartObject::ACTOR_PART_DESC			Head_Desc;
	Head_Desc.pParentsTransform = m_pTransformCom;
	Head_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_PL5750");
	Head_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF94_PL5750"));
	Head_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF120_PL5750"));
	Head_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF190_PL5750"));
	Head_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("Face_PL5750"));

	CActor_PartObject* pPartObject_Head = { CActor_PartObject::Create(m_pDevice, m_pContext, &Head_Desc) };
	if (nullptr == pPartObject_Head)
		return E_FAIL;

	m_PartObjects[static_cast<_uint>(ACTOR_PL57_PART::_HEAD)] = pPartObject_Head;

	return S_OK;
}

void CActor_PL57::Set_Idle_Loop()
{
	CActor_PartObject*			pPartObject_Head = { m_PartObjects[static_cast<_uint>(ACTOR_PL57_PART::_HEAD)] };
	CActor_PartObject*			pPartObject_Body = { m_PartObjects[static_cast<_uint>(ACTOR_PL57_PART::_BODY)] };

	CModel*						pHead_Model = { static_cast<CModel*>(pPartObject_Head->Get_Component(TEXT("Com_Model"))) };
	CModel*						pBody_Model = { static_cast<CModel*>(pPartObject_Body->Get_Component(TEXT("Com_Model"))) };

	/*_matrix						RootCombinedMatrix = { XMLoadFloat4x4(pBody_Model->Get_CombinedMatrix("root")) };
	_matrix						WorldMatrix = { m_pTransformCom->Get_WorldMatrix() };

	WorldMatrix = RootCombinedMatrix * WorldMatrix;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);*/

	pHead_Model->Change_Animation(0, TEXT("Face_PL5750"), 0);
	pHead_Model->Set_Loop(0, true);

	pBody_Model->Change_Animation(0, TEXT("Body_PL5700"), 0);
	pBody_Model->Set_Loop(0, true);
}

CActor_PL57* CActor_PL57::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CActor_PL57* pInstance = new CActor_PL57(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CActor_PL57"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CActor_PL57::Clone(void* pArg)
{
	CActor_PL57* pInstance = new CActor_PL57(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CActor_PL57"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CActor_PL57::Free()
{
	__super::Free();
}
