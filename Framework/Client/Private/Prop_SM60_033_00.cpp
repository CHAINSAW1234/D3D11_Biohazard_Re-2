#include "stdafx.h"
#include "Prop_SM60_033_00.h"

#include "Call_Center.h"
#include "Body_Shutter.h"
#include "Shutter.h"

CProp_SM60_033::CProp_SM60_033(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CProp_Controller{ pDevice, pContext }
{
}

CProp_SM60_033::CProp_SM60_033(const CProp_SM60_033& rhs)
	: CProp_Controller{ rhs }
{
}

HRESULT CProp_SM60_033::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iCallerType = static_cast<_int>(CCall_Center::CALLER::_SM60_033);

	return S_OK;
}

HRESULT CProp_SM60_033::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CProp_SM60_033::Late_Tick(_float fTimeDelta)
{
	return;
}

void CProp_SM60_033::Start()
{
	__super::Start();

	if (nullptr == m_pPropObject)
		return;

	CGameObject*			pBody_Shutter = { static_cast<CShutter*>(m_pPropObject)->Get_PartObject(CShutter::PART_BODY) };
	if (nullptr == pBody_Shutter)
		return;

	CModel*					pBody_Model = { static_cast<CModel*>(pBody_Shutter->Get_Component(TEXT("Com_Body_Model"))) };
	if (nullptr == pBody_Model)
	{
		MSG_BOX(TEXT("Called : void CProp_SM60_033::Start()   Prop Body Model Component Tag ¿À·ù "));
		return;
	}

	/*CShutter*				pShutter = { static_cast<CShutter*>(m_pPropObject) };
	pShutter->Set_OutOfControll(true);*/

	m_pBodyModel = pBody_Model;
	Safe_AddRef(m_pBodyModel);

	m_pBodyModel->Add_Animations(m_strAnimLayerTag, m_strAnimLayerTag);

	Late_Initialize();
}

CProp_SM60_033* CProp_SM60_033::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProp_SM60_033* pInstance = new CProp_SM60_033(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CProp_SM60_033"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProp_SM60_033::Clone(void* pArg)
{
	CProp_SM60_033* pInstance = new CProp_SM60_033(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CProp_SM60_033"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProp_SM60_033::Free()
{
	__super::Free();
}
