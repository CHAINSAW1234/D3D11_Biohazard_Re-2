#include "stdafx.h"
#include "Actor_SM60_033_00.h"
#include "Actor_PartObject.h"

#include "Call_Center.h"
#include "Body_Shutter.h"
#include "Shutter.h"

CActor_SM60_033_00::CActor_SM60_033_00(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActor{ pDevice, pContext }
{
}

CActor_SM60_033_00::CActor_SM60_033_00(const CActor& rhs)
	: CActor{ rhs }
{
}

HRESULT CActor_SM60_033_00::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_SM60_033_00::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CActor_SM60_033_00::Start()
{
	CGameObject*				pGameObject = { CCall_Center::Get_Instance()->Get_Caller(CCall_Center::CALLER::_SM60_033) };
	if (nullptr == pGameObject)
		return;

	CShutter*					pShutter = { static_cast<CShutter*>(pGameObject) };

	_matrix						WorldMatrix = { pShutter->Get_Transform()->Get_WorldMatrix() };
	m_PartObjects[static_cast<_uint>(ACTOR_SM60_033_PART::_BODY)]->Get_Transform()->Set_WorldMatrix(WorldMatrix);
}

HRESULT CActor_SM60_033_00::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_SM60_033_00::Add_PartObjects()
{
	m_PartObjects.resize(static_cast<size_t>(ACTOR_SM60_033_PART::_END));

	CActor_PartObject::ACTOR_PART_DESC			Body_Desc;
	Body_Desc.pParentsTransform = m_pTransformCom;
	Body_Desc.pRootTranslation = &m_vRootTranslation;
	Body_Desc.isBaseObject = true;
	Body_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_SM60_033");
	Body_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF92_SM60_033_00"));

	CActor_PartObject* pPartObject_Body = { CActor_PartObject::Create(m_pDevice, m_pContext, &Body_Desc) };
	if (nullptr == pPartObject_Body)
		return E_FAIL;

	m_PartObjects[static_cast<_uint>(ACTOR_SM60_033_PART::_BODY)] = pPartObject_Body;

	return S_OK;
}

CActor_SM60_033_00* CActor_SM60_033_00::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CActor_SM60_033_00* pInstance = new CActor_SM60_033_00(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CActor_SM60_033_00"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CActor_SM60_033_00::Clone(void* pArg)
{
	CActor_SM60_033_00* pInstance = new CActor_SM60_033_00(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CActor_SM60_033_00"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CActor_SM60_033_00::Free()
{
	__super::Free();
}
