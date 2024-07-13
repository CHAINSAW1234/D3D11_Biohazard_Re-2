#include "..\Public\PipeLine.h"
#include "GameInstance.h"
#include "Light.h"

#include "Texture.h"
#include "ComputeShader.h"
#include "RenderTarget.h"

CPipeLine::CPipeLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pGameInstance { CGameInstance::Get_Instance()}
	, m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

void CPipeLine::Add_ShadowLight(SHADOWLIGHT eShadowLight, CLight* pLight)
{
	switch (eShadowLight) {
	case DIRECTION:
		if (m_pDirectionLight == nullptr) {
			m_pDirectionLight = pLight;
			Safe_AddRef(pLight);
		}
		break;
	case POINT:
		if (m_iNumLight < m_iMaxLight) {
			++m_iNumLight;
			m_Lights.push_back(pLight);
			Safe_AddRef(pLight);
		}
		break;
	case SPOT:
		if (m_pSpotLight == nullptr) {
			m_pSpotLight = pLight;
			Safe_AddRef(pLight);
		}
		break;
	}
}

void CPipeLine::Set_CubeMap(CTexture* pTexture, _uint iIndex)
{
	if (m_pCurCubeMapTexture == pTexture &&
		m_iCurCubeMapIndex == iIndex)
		return;

	if (nullptr != m_pPrevCubeMapTexture)
		Safe_Release(m_pPrevCubeMapTexture);

	m_isRender = true;
	m_fLerpTimeDelta = 0.f;
	m_iPrevCubeMapIndex = m_iCurCubeMapIndex;
	m_pPrevCubeMapTexture = m_pCurCubeMapTexture;

	m_pPrevIrradialTexture->Copy_Resource(m_pCurIrradialTexture->Get_Texture2D());

	m_iCurCubeMapIndex = iIndex;
	m_pCurCubeMapTexture = pTexture;
	Safe_AddRef(m_pCurCubeMapTexture);
}

list<LIGHT_DESC*> CPipeLine::Get_ShadowPointLightDesc_List()
{
	list<LIGHT_DESC*> ShadowLightDesc;

	for (auto& pLight : m_Lights) {
		list<LIGHT_DESC*>* pList = pLight->Get_Light_List();

		for (auto& pDesc : *pList) {
			if (pDesc->bShadow) {
				ShadowLightDesc.push_back(pDesc);
			}
		}
	}

	return ShadowLightDesc;
}

HRESULT CPipeLine::Bind_PrevIrradianceTexture(CShader* pShader, const _char* pConstantName)
{
	return m_pCurIrradialTexture->Bind_ShaderResource(pShader, pConstantName);
}

HRESULT CPipeLine::Bind_CurIrradianceTexture(CShader* pShader, const _char* pConstantName)
{
	return m_pPrevIrradialTexture->Bind_ShaderResource(pShader, pConstantName);
}

HRESULT CPipeLine::Bind_PrevCubeMapTexture(CShader* pShader, const _char* pConstantName)
{
	if (nullptr == m_pPrevCubeMapTexture) {
		if (nullptr == m_pCurCubeMapTexture)
			return E_FAIL;

		return m_pCurCubeMapTexture->Bind_ShaderResource(pShader, pConstantName, m_iCurCubeMapIndex);
	}
		

	return m_pPrevCubeMapTexture->Bind_ShaderResource(pShader, pConstantName, m_iPrevCubeMapIndex);
}

HRESULT CPipeLine::Bind_CurCubeMapTexture(CShader* pShader, const _char* pConstantName)
{
	if (nullptr == m_pCurCubeMapTexture)
		return E_FAIL;

	return m_pCurCubeMapTexture->Bind_ShaderResource(pShader, pConstantName, m_iCurCubeMapIndex);
}

HRESULT CPipeLine::Initialize()
{
	for (size_t j = 0; j < D3DTS_END; j++)
	{
		XMStoreFloat4x4(&m_TransformMatrices[j], XMMatrixIdentity());
		XMStoreFloat4x4(&m_TransformInverseMatrices[j], XMMatrixIdentity());
	}
	m_vCamPosition = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(Create_Texture()))
		return E_FAIL;

	if (FAILED(Set_Components()))
		return E_FAIL;

	return S_OK;
}

void CPipeLine::Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < D3DTS_END; i++)
	{
		XMStoreFloat4x4(&m_TransformInverseMatrices[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrices[i])));
	}

	memcpy(&m_vCamPosition, &m_TransformInverseMatrices[D3DTS_VIEW].m[3][0], sizeof(_float4));

	m_fLerpTimeDelta += fTimeDelta;
	m_fLerpTimeDelta = min(1.f, m_fLerpTimeDelta);

	//// For. Cascade
	//Update_CascadeFrustum();
	//Update_CascadeProjMatrices();
}

HRESULT CPipeLine::Render()
{
	if (m_pCurCubeMapTexture == nullptr)
		return S_OK;

	if (!m_isRender)
		return S_OK;
	m_isRender = false;

	m_pHDRTexture->Clear();
	m_pCurIrradialTexture->Clear();

	//1. HDR
	if (FAILED(m_pCurCubeMapTexture->Bind_ShaderResource(m_pShaderCom, "g_CubeTexture", m_iCurCubeMapIndex)))
		return E_FAIL;
	if (FAILED(m_pHDRTexture->Bind_OutputShaderResource(m_pShaderCom, "OutputTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Render(0, 8, 8, 6)))
		return E_FAIL;


	// 2. Irradiance
	if (FAILED(m_pHDRTexture->Bind_ShaderResource(m_pShaderCom, "g_CubeTexture")))
		return E_FAIL;
	if (FAILED(m_pCurIrradialTexture->Bind_OutputShaderResource(m_pShaderCom, "OutputTexture")))
		return E_FAIL;

	if(FAILED(m_pShaderCom->Render(1, 8,8, 6)))
		return E_FAIL;

	return S_OK;
}

void CPipeLine::Reset()
{
	// 이번 프레임의 카메라 정보를 이전 프레임 정보로 이동
	memcpy_s(&m_PrevTransformMatrices, sizeof(_float4x4) * D3DTS_END, &m_TransformMatrices, sizeof(_float4x4) * D3DTS_END);
	memcpy_s(&m_PrevTransformInverseMatrices, sizeof(_float4x4) * D3DTS_END, &m_TransformInverseMatrices, sizeof(_float4x4) * D3DTS_END);
	m_vPrevCamPosition = m_vCamPosition;

	// 파이프라인에서 라이트 초기화
	for (auto& pLight : m_Lights) {
		Safe_Release(pLight);
	}
	m_Lights.clear();
	m_iNumLight = 0;

	Safe_Release(m_pDirectionLight);
	m_pDirectionLight = nullptr;

	Safe_Release(m_pSpotLight);
	m_pSpotLight = nullptr;

}

#ifdef _DEBUG

void CPipeLine::Render_Debug()
{
	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	static _float4x4 WorldMatrix = XMMatrixIdentity();
	WorldMatrix._11 = ViewportDesc.Width;
	WorldMatrix._22 = ViewportDesc.Height;
	WorldMatrix._41 = 0.f;
	WorldMatrix._42 = 0.f;

	static _float4x4 ViewMatrix = XMMatrixIdentity();
	static _float4x4 ProjMatrix = XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f);

	m_pVIBuffer->Bind_Buffers();
	m_pGameInstance->GetDeferredShader()->Bind_Matrix("g_ViewMatrix", &ViewMatrix);
	m_pGameInstance->GetDeferredShader()->Bind_Matrix("g_ProjMatrix", &ProjMatrix);
	m_pCurIrradialTexture->Render_Debug(m_pGameInstance->GetDeferredShader(), m_pVIBuffer);
}

#endif

HRESULT CPipeLine::Set_Components()
{
	m_pShaderCom = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_CubeMap.hlsl"), "CS_Irradiance");
	if (nullptr == m_pShaderCom)
		return E_FAIL;

#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);

#endif 

	return S_OK;
}

HRESULT CPipeLine::Create_Texture()
{
	CRenderTarget* pRenderTarget;
		
	pRenderTarget = CRenderTarget::Create_Cube(m_pDevice, m_pContext, 256, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, TEXT("Target_HDR"), _float4(1.f, 1.f, 1.f, 1.f), false);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_pHDRTexture = pRenderTarget;
	m_pHDRTexture->Clear();

	pRenderTarget = CRenderTarget::Create_Cube(m_pDevice, m_pContext, 256, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, TEXT("Target_Irradiance"), _float4(1.f,1.f,1.f,1.f), false);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_pCurIrradialTexture = pRenderTarget;
	m_pCurIrradialTexture->Clear();

	pRenderTarget = CRenderTarget::Create_Cube(m_pDevice, m_pContext, 256, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, TEXT("Target_Irradiance"), _float4(1.f, 1.f, 1.f, 1.f), false);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_pPrevIrradialTexture = pRenderTarget;
	m_pPrevIrradialTexture->Clear();


#ifdef _DEBUG
	_float fSize = 150;
	pRenderTarget->Ready_Debug(-fSize / 2 + fSize, -fSize / 2 + fSize * 5, fSize, fSize);
#endif 



	return S_OK;
}

CPipeLine * CPipeLine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPipeLine*		pInstance = new CPipeLine(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CPipeLine"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPipeLine::Free()
{
	// 파이프라인에서 라이트 초기화
	for (auto& pLight : m_Lights) {
		Safe_Release(pLight);
	}
	m_Lights.clear();
	m_iNumLight = 0;

	Safe_Release(m_pDirectionLight);
	m_pDirectionLight = nullptr;

	Safe_Release(m_pSpotLight);
	m_pSpotLight = nullptr;

	Safe_Release(m_pCurCubeMapTexture);
	Safe_Release(m_pHDRTexture);
	Safe_Release(m_pCurIrradialTexture);

	Safe_Release(m_pPrevCubeMapTexture);
	Safe_Release(m_pPrevIrradialTexture);

	Safe_Release(m_pShaderCom);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);


#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif

}
