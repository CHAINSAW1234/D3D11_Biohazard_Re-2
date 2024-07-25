#include "stdafx.h"
#include "Prop_SM41_024.h"

#include "Call_Center.h"
#include "Body_NewPoliceStatue.h"
#include "Shutter.h"

CProp_SM41_024::CProp_SM41_024(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CProp_Controller{ pDevice, pContext }
{
}

CProp_SM41_024::CProp_SM41_024(const CProp_SM41_024& rhs)
	: CProp_Controller{ rhs }
{
}

HRESULT CProp_SM41_024::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_iCallerType = static_cast<_int>(CCall_Center::CALLER::_SM41_024);

	return S_OK;
}

HRESULT CProp_SM41_024::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CProp_SM41_024::Late_Tick(_float fTimeDelta)
{
	return;
}

void CProp_SM41_024::Start()
{
	__super::Start();

	if (nullptr == m_pPropObject)
		return;

	_float4x4			m_OriginalTransformation = { m_pPropObject->Get_Transform()->Get_WorldFloat4x4() };

	CGameObject*		pBody_Statue = { static_cast<CShutter*>(m_pPropObject)->Get_PartObject(CShutter::PART_BODY) };
	if (nullptr == pBody_Statue)
		return;

	CModel*				pBody_Model = { static_cast<CModel*>(pBody_Statue->Get_Component(TEXT("Com_Body_Model"))) };
	if (nullptr == pBody_Model)
	{
		MSG_BOX(TEXT("Called : void CProp_SM41_024::Start()   Prop Body Model Component Tag ¿À·ù "));
		return;
	}

	m_pBodyModel = pBody_Model;
	Safe_AddRef(m_pBodyModel);

	m_pBodyModel->Add_Animations(m_strAnimLayerTag, m_strAnimLayerTag);

	m_pTempIdentity = new _float4x4;
	XMStoreFloat4x4(m_pTempIdentity, XMMatrixIdentity());
	m_pBodyModel->Set_Surbodinate("root", true);
	m_pBodyModel->Set_Parent_CombinedMatrix_Ptr("root", m_pTempIdentity);

	Late_Initialize();
}

void CProp_SM41_024::Set_Transformation_Origin()
{
	_matrix			ScaleMatrix = { XMMatrixScaling(0.01f, 0.01f, 0.01f) };

	m_pPropObject->Get_Transform()->Set_WorldMatrix(ScaleMatrix);
}

void CProp_SM41_024::Reset_Transformation()
{
	m_pPropObject->Get_Transform()->Set_WorldMatrix(XMLoadFloat4x4(&m_OriginalTransformation));
}

CProp_SM41_024* CProp_SM41_024::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProp_SM41_024* pInstance = new CProp_SM41_024(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CProp_SM41_024"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProp_SM41_024::Clone(void* pArg)
{
	CProp_SM41_024* pInstance = new CProp_SM41_024(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CProp_SM41_024"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProp_SM41_024::Free()
{
	__super::Free();
}
