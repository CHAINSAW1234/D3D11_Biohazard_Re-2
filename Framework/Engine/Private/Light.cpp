#include "..\Public\Light.h"

#include "GameInstance.h"

CLight::CLight()
{
}

HRESULT CLight::Initialize(const LIGHT_DESC& LightDesc, _float fFovY, _float fAspect, _float fNearZ, _float fFarZ)
{
	m_LightDesc = LightDesc;

	if (LIGHT_DESC::TYPE_POINT == m_LightDesc.eType) {
		m_LightViewMatrix = new _float4x4[6];

		XMStoreFloat4x4(&m_LightViewMatrix[0], XMMatrixLookToLH(XMLoadFloat4(&m_LightDesc.vPosition), XMVectorSet(1.f, 0.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMStoreFloat4x4(&m_LightViewMatrix[1], XMMatrixLookToLH(XMLoadFloat4(&m_LightDesc.vPosition), XMVectorSet(-1.f, 0.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMStoreFloat4x4(&m_LightViewMatrix[2], XMMatrixLookToLH(XMLoadFloat4(&m_LightDesc.vPosition), XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMStoreFloat4x4(&m_LightViewMatrix[3], XMMatrixLookToLH(XMLoadFloat4(&m_LightDesc.vPosition), XMVectorSet(0.f, -1.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMStoreFloat4x4(&m_LightViewMatrix[4], XMMatrixLookToLH(XMLoadFloat4(&m_LightDesc.vPosition), XMVectorSet(0.f, 0.f, 1.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMStoreFloat4x4(&m_LightViewMatrix[5], XMMatrixLookToLH(XMLoadFloat4(&m_LightDesc.vPosition), XMVectorSet(0.f, 0.f, -1.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	}
	else {
		m_LightViewMatrix = new _float4x4;
		XMStoreFloat4x4(m_LightViewMatrix, XMMatrixLookToLH(XMLoadFloat4(&m_LightDesc.vPosition), XMLoadFloat4(&m_LightDesc.vDirection), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	}

	XMStoreFloat4x4(&m_LightProjMatrix, XMMatrixPerspectiveFovLH(fFovY, fAspect, fNearZ, fFarZ));


	return S_OK;
}

HRESULT CLight::Tick(const LIGHT_DESC& Light_Desc)
{
	 if (LIGHT_DESC::TYPE_POINT == m_LightDesc.eType)
	{
		 m_LightDesc.vDiffuse = Light_Desc.vDiffuse;
		 m_LightDesc.vAmbient = Light_Desc.vAmbient;
		 m_LightDesc.vSpecular = Light_Desc.vSpecular;
		 m_LightDesc.fRange = Light_Desc.fRange;
		 m_LightDesc.bRender = Light_Desc.bRender;

	}
	else if (LIGHT_DESC::TYPE_SPOT == m_LightDesc.eType)
	{
		 m_LightDesc.vDiffuse = Light_Desc.vDiffuse;
		 m_LightDesc.vAmbient = Light_Desc.vAmbient;
		 m_LightDesc.vSpecular = Light_Desc.vSpecular;
		 m_LightDesc.vPosition= Light_Desc.vPosition;
		 m_LightDesc.vDirection= Light_Desc.vDirection;
		 m_LightDesc.fRange = Light_Desc.fRange;
		 m_LightDesc.fOutCutOff= Light_Desc.fOutCutOff;
		 m_LightDesc.fCutOff = Light_Desc.fCutOff;
		 m_LightDesc.bRender = Light_Desc.bRender;

		 XMStoreFloat4x4(m_LightViewMatrix, XMMatrixLookToLH(XMLoadFloat4(&m_LightDesc.vPosition), XMLoadFloat4(&m_LightDesc.vDirection), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	}
	else if (LIGHT_DESC::TYPE_DIRECTIONAL == m_LightDesc.eType)
	 {
		 m_LightDesc.vDiffuse = Light_Desc.vDiffuse;
		 m_LightDesc.vAmbient = Light_Desc.vAmbient;
		 m_LightDesc.vSpecular = Light_Desc.vSpecular;
		 m_LightDesc.bRender = Light_Desc.bRender;
	 }
	return S_OK;
}

HRESULT CLight::Render(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	if (m_LightDesc.bRender == false)
		return S_OK;

	_uint		iPassIndex = { 0 };

	/* 빛 연산을 위한 정보들을 던져주낟. */

	if (LIGHT_DESC::TYPE_DIRECTIONAL == m_LightDesc.eType)
	{
		iPassIndex = 1;

		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
			return E_FAIL;
	}

	else if (LIGHT_DESC::TYPE_POINT == m_LightDesc.eType)
	{
		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;

		iPassIndex = 2;
	}
	else if (LIGHT_DESC::TYPE_SPOT == m_LightDesc.eType)
	{
		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_fCutOff", &m_LightDesc.fCutOff, sizeof(_float))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_fOutCutOff", &m_LightDesc.fOutCutOff, sizeof(_float))))
			return E_FAIL;

		iPassIndex = 3;
	}
	if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
		return E_FAIL;

	pShader->Begin(iPassIndex);

	pVIBuffer->Render();

	return S_OK;
}

CLight * CLight::Create(const LIGHT_DESC& LightDesc, _float fFovY, _float fAspect, _float fNearZ, _float fFarZ)
{
	CLight*		pInstance = new CLight();

	if (FAILED(pInstance->Initialize(LightDesc, fFovY, fAspect, fNearZ, fFarZ)))
	{
		MSG_BOX(TEXT("Failed To Created : CLight"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	if (LIGHT_DESC::TYPE_SPOT == m_LightDesc.eType) {
		Safe_Delete_Array(m_LightViewMatrix);
	}
	else
		Safe_Delete(m_LightViewMatrix);


}

