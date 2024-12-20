#include "..\Public\Light.h"

#include "GameInstance.h"

CLight::CLight()
{
}

HRESULT CLight::Add_LightDesc(LIGHT_DESC desc, _float fFovY, _float fAspect, _float fNearZ, _float fFarZ)
{
	LIGHT_DESC* pNewDesc = new LIGHT_DESC();

	pNewDesc->bRender = desc.bRender;
	pNewDesc->bShadow = desc.bShadow;
	pNewDesc->eType = desc.eType;
	pNewDesc->fCutOff = desc.fCutOff;
	pNewDesc->fOutCutOff = desc.fOutCutOff;
	pNewDesc->fRange = desc.fRange;
	pNewDesc->vAmbient = desc.vAmbient;
	pNewDesc->vDiffuse = desc.vDiffuse;
	pNewDesc->vDirection = desc.vDirection;
	pNewDesc->vPosition = desc.vPosition;
	pNewDesc->vSpecular = desc.vSpecular;
	pNewDesc->BelongNumVec = vector<_int>(desc.BelongNumVec); //vec로 받고
	
	for (auto iter : pNewDesc->BelongNumVec) // 실제로 사용할 배열에 넣는다.
	{
		pNewDesc->BelongNum[iter] = true;
	}


	if (LIGHT_DESC::TYPE_POINT == pNewDesc->eType) {
		XMStoreFloat4x4(&pNewDesc->ViewMatrix[0], XMMatrixLookToLH(XMLoadFloat4(&pNewDesc->vPosition), XMVectorSet(1.f, 0.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMStoreFloat4x4(&pNewDesc->ViewMatrix[1], XMMatrixLookToLH(XMLoadFloat4(&pNewDesc->vPosition), XMVectorSet(-1.f, 0.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMStoreFloat4x4(&pNewDesc->ViewMatrix[2], XMMatrixLookToLH(XMLoadFloat4(&pNewDesc->vPosition), XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVectorSet(0.f, 0.f, -1.f, 0.f)));
		XMStoreFloat4x4(&pNewDesc->ViewMatrix[3], XMMatrixLookToLH(XMLoadFloat4(&pNewDesc->vPosition), XMVectorSet(0.f, -1.f, 0.f, 0.f), XMVectorSet(0.f, 0.f, 1.f, 0.f)));
		XMStoreFloat4x4(&pNewDesc->ViewMatrix[4], XMMatrixLookToLH(XMLoadFloat4(&pNewDesc->vPosition), XMVectorSet(0.f, 0.f, 1.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
		XMStoreFloat4x4(&pNewDesc->ViewMatrix[5], XMMatrixLookToLH(XMLoadFloat4(&pNewDesc->vPosition), XMVectorSet(0.f, 0.f, -1.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	}
	else {
		// Up방향 계산
		_vector vWorldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		// If the direction is too close to the world up vector, choose a different world up vector
		// This is to avoid numerical instability
		if (fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat4(&pNewDesc->vDirection), vWorldUp))) > 0.99f) {
			vWorldUp = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		}
		XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(vWorldUp, XMLoadFloat4(&pNewDesc->vDirection)));
		XMVECTOR vUp = XMVector3Cross(XMLoadFloat4(&pNewDesc->vDirection), vRight);

		XMStoreFloat4x4(&pNewDesc->ViewMatrix[0], XMMatrixLookToLH(XMLoadFloat4(&pNewDesc->vPosition), XMLoadFloat4(&pNewDesc->vDirection), vUp));

	}

	XMStoreFloat4x4(&pNewDesc->ProjMatrix, XMMatrixPerspectiveFovLH(fFovY, fAspect, fNearZ, fFarZ));

	m_Lights.emplace_back(pNewDesc);
	return S_OK;
}

HRESULT CLight::Set_Render(_bool bRender)
{
	for (auto& desc : m_Lights)
	{
		desc->bRender = bRender;
	}
	return S_OK;
}

HRESULT CLight::Initialize()
{
	return S_OK;
}

HRESULT CLight::Update(const LIGHT_DESC& Light_Desc, _uint iIndex, _float fLerp)
{
	if (m_Lights.size() < iIndex)
		return E_FAIL;

	auto& iter = m_Lights.begin();
	for (_uint i = 0; i < iIndex; i++)
		iter++;

	if (LIGHT_DESC::TYPE_POINT == (*iter)->eType)
	{
		(*iter)->vDiffuse =XMVectorLerp((*iter)->vDiffuse, Light_Desc.vDiffuse, fLerp);
		(*iter)->vAmbient = XMVectorLerp((*iter)->vAmbient, Light_Desc.vAmbient, fLerp);
		(*iter)->vSpecular = XMVectorLerp((*iter)->vSpecular, Light_Desc.vSpecular, fLerp);
		(*iter)->fRange = Lerp((*iter)->fRange, Light_Desc.fRange, fLerp);
		(*iter)->bRender = Light_Desc.bRender;

	}
	else if (LIGHT_DESC::TYPE_SPOT == (*iter)->eType)
	{
		(*iter)->vDiffuse = XMVectorLerp((*iter)->vDiffuse, Light_Desc.vDiffuse, fLerp);
		(*iter)->vAmbient = XMVectorLerp((*iter)->vAmbient, Light_Desc.vAmbient, fLerp);
		(*iter)->vSpecular = XMVectorLerp((*iter)->vSpecular, Light_Desc.vSpecular, fLerp);
		(*iter)->vPosition =XMVectorLerp((*iter)->vPosition, Light_Desc.vPosition, fLerp);
		(*iter)->vDirection = XMVectorLerp((*iter)->vDirection, Light_Desc.vDirection, fLerp);
		(*iter)->fRange =Lerp((*iter)->fRange, Light_Desc.fRange, fLerp);
		(*iter)->fOutCutOff =Lerp((*iter)->fOutCutOff, Light_Desc.fOutCutOff, fLerp);
		(*iter)->fCutOff = Lerp((*iter)->fCutOff, Light_Desc.fCutOff, fLerp);
		(*iter)->bRender = Light_Desc.bRender;
		XMStoreFloat4x4(((*iter)->ViewMatrix), XMMatrixLookToLH(XMLoadFloat4(&(*iter)->vPosition), XMLoadFloat4(&(*iter)->vDirection), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	}
	else if (LIGHT_DESC::TYPE_DIRECTIONAL == (*iter)->eType)
	{

		(*iter)->vDiffuse =XMVectorLerp((*iter)->vDiffuse, Light_Desc.vDiffuse, fLerp);
		(*iter)->vAmbient =XMVectorLerp((*iter)->vAmbient, Light_Desc.vAmbient, fLerp);
		(*iter)->vSpecular = XMVectorLerp((*iter)->vSpecular, Light_Desc.vSpecular, fLerp);
		(*iter)->bRender = Light_Desc.bRender;
	}


	return S_OK;
}

HRESULT CLight::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	for (auto& Light_desc : m_Lights)
	{
		if (Light_desc->bRender == false)
			continue;
		_uint		iPassIndex = { 0 };

		/* 빛 연산을 위한 정보들을 던져주낟. */

		if (LIGHT_DESC::TYPE_DIRECTIONAL == Light_desc->eType)
		{
			iPassIndex = (_uint)SHADER_PASS_DEFERRED::PASS_LIGHT_DIRECTIONAL;

			if (FAILED(pShader->Bind_RawValue("g_vLightDir", &Light_desc->vDirection, sizeof(_float4))))
				return E_FAIL;
			if (FAILED(pShader->Bind_RawValue("g_vLightPos", &Light_desc->vPosition, sizeof(_float4))))
				return E_FAIL;
		}

		else if (LIGHT_DESC::TYPE_POINT == Light_desc->eType)
		{
			iPassIndex = (_uint)SHADER_PASS_DEFERRED::PASS_LIGHT_POINT;

			if (FAILED(pShader->Bind_RawValue("g_vLightPos", &Light_desc->vPosition, sizeof(_float4))))
				return E_FAIL;

			if (FAILED(pShader->Bind_RawValue("g_fLightRange", &Light_desc->fRange, sizeof(_float))))
				return E_FAIL;

		}
		else if (LIGHT_DESC::TYPE_SPOT == Light_desc->eType)
		{
			iPassIndex = (_uint)SHADER_PASS_DEFERRED::PASS_LIGHT_SPOT;

			if (FAILED(pShader->Bind_RawValue("g_vLightDir", &Light_desc->vDirection, sizeof(_float4))))
				return E_FAIL;

			if (FAILED(pShader->Bind_RawValue("g_vLightPos", &Light_desc->vPosition, sizeof(_float4))))
				return E_FAIL;

			if (FAILED(pShader->Bind_RawValue("g_fLightRange", &Light_desc->fRange, sizeof(_float))))
				return E_FAIL;

			if (FAILED(pShader->Bind_RawValue("g_fCutOff", &Light_desc->fCutOff, sizeof(_float))))
				return E_FAIL;

			if (FAILED(pShader->Bind_RawValue("g_fOutCutOff", &Light_desc->fOutCutOff, sizeof(_float))))
				return E_FAIL;
		}

		if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &Light_desc->vDiffuse, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &Light_desc->vAmbient, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &Light_desc->vSpecular, sizeof(_float4))))
			return E_FAIL;

		pShader->Begin(iPassIndex);

		pVIBuffer->Render();
	}
	return S_OK;
}

HRESULT CLight::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer, _uint iIndex)
{
	if (iIndex >= m_Lights.size())
		return E_FAIL;
	
	auto iter = m_Lights.begin();
	advance(iter, iIndex);

	LIGHT_DESC eDesc = **iter;

	if (eDesc.bRender == false)
		return S_OK;

	_uint		iPassIndex = { 0 };

	if (LIGHT_DESC::TYPE_DIRECTIONAL == eDesc.eType)
	{
		iPassIndex = (_uint)SHADER_PASS_DEFERRED::PASS_LIGHT_DIRECTIONAL;

		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &eDesc.vDirection, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &eDesc.vPosition, sizeof(_float4))))
			return E_FAIL;
	}

	else if (LIGHT_DESC::TYPE_POINT == eDesc.eType)
	{
		iPassIndex = (_uint)SHADER_PASS_DEFERRED::PASS_LIGHT_POINT;

		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &eDesc.vPosition, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_fLightRange", &eDesc.fRange, sizeof(_float))))
			return E_FAIL;

	}
	else if (LIGHT_DESC::TYPE_SPOT == eDesc.eType)
	{
		iPassIndex = (_uint)SHADER_PASS_DEFERRED::PASS_LIGHT_SPOT;

		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &eDesc.vDirection, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &eDesc.vPosition, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_fLightRange", &eDesc.fRange, sizeof(_float))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_fCutOff", &eDesc.fCutOff, sizeof(_float))))
			return E_FAIL;

		if (FAILED(pShader->Bind_RawValue("g_fOutCutOff", &eDesc.fOutCutOff, sizeof(_float))))
			return E_FAIL;
	}

	if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &eDesc.vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &eDesc.vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &eDesc.vSpecular, sizeof(_float4))))
		return E_FAIL;


	if(FAILED(pShader->Begin(iPassIndex)))
		return E_FAIL;

	if (FAILED(pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

CLight* CLight::Create()
{
	CLight* pInstance = new CLight();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLight"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	for (auto& iter : m_Lights)
	{
		Safe_Delete(iter);
		iter = nullptr;
	}
	m_Lights.clear();

}

