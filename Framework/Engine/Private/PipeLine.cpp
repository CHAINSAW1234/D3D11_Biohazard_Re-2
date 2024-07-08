#include "..\Public\PipeLine.h"
#include "GameInstance.h"
#include "Light.h"

#include "Texture.h"
#include "ComputeShader.h"
#include "RenderTarget.h"

CPipeLine::CPipeLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
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

void CPipeLine::Set_CubeMap(CTexture* pTexture)
{
	if (m_pCubeMapTexture == pTexture)
		return;

	if (nullptr != m_pCubeMapTexture)
		Safe_Release(m_pCubeMapTexture);

	m_isRender = true;
	m_pCubeMapTexture = pTexture;
	Safe_AddRef(m_pCubeMapTexture);
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

HRESULT CPipeLine::Bind_IrradianceTexture(CShader* pShader, const _char* pConstantName)
{
	return m_pIrradialTexture->Bind_ShaderResource(pShader, pConstantName);
}

HRESULT CPipeLine::Bind_CubeMapTexture(CShader* pShader, const _char* pConstantName)
{
	if (nullptr == m_pCubeMapTexture)
		return E_FAIL;

	return m_pCubeMapTexture->Bind_ShaderResource(pShader, pConstantName);
}

HRESULT CPipeLine::Initialize()
{
	for (size_t j = 0; j < D3DTS_END; j++)
	{
		XMStoreFloat4x4(&m_TransformMatrices[j], XMMatrixIdentity());
		XMStoreFloat4x4(&m_TransformInverseMatrices[j], XMMatrixIdentity());
	}
	m_vCamPosition = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(Create_IrradianceTexture()))
		return E_FAIL;

	m_pShaderCom = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_IrradianceMap.hlsl"), "CS_Irradiance");
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	return S_OK;
}

void CPipeLine::Tick()
{
	for (size_t i = 0; i < D3DTS_END; i++)
	{
		XMStoreFloat4x4(&m_TransformInverseMatrices[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrices[i])));
	}

	memcpy(&m_vCamPosition, &m_TransformInverseMatrices[D3DTS_VIEW].m[3][0], sizeof(_float4));

	//// For. Cascade
	//Update_CascadeFrustum();
	//Update_CascadeProjMatrices();
}

HRESULT CPipeLine::Render()
{
	if (m_pCubeMapTexture == nullptr)
		return S_OK;

	if (!m_isRender)
		return S_OK;
	m_isRender = false;

	if (FAILED(m_pCubeMapTexture->Bind_ShaderResource(m_pShaderCom, "g_CubeTexture")))
		return E_FAIL;

	if (FAILED(m_pIrradialTexture->Bind_OutputShaderResource(m_pShaderCom, "OutputTexture")))
		return E_FAIL;

	if(FAILED(m_pShaderCom->Render(0, 8, 8, 6)))
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

HRESULT CPipeLine::Create_IrradianceTexture()
{
	CRenderTarget* pRenderTarget = CRenderTarget::Create_Cube(m_pDevice, m_pContext, 256, 6, DXGI_FORMAT_R8G8B8A8_UNORM,
		TEXT("Target_Irradiance"), _float4(0.f,0.f,0.f,0.f), false);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	m_pIrradialTexture = pRenderTarget;

	return S_OK;
}

void CPipeLine::Update_CascadeFrustum()
{
	//_vector vPoints[8];

	//for (size_t i = 0; i < 8; i++)
	//{
	//	vPoints[i] = XMVector3TransformCoord(XMLoadFloat3(&m_vOriginalPoints[i]), XMLoadFloat4x4(&m_TransformInverseMatrices[D3DTS_PROJ]));
	//	vPoints[i] = XMVector3TransformCoord(vPoints[i], XMLoadFloat4x4(&m_TransformInverseMatrices[D3DTS_VIEW][CAMERA]));
	//}

	//_float fLamda = 0.5f;

	//m_fCascadeSplitZ[0] = 0;
	//m_fCascadeSplitZ[CASCADE_END] = 1;

	//for (_int i = 1; i < CASCADE_END; ++i)
	//{
	//	_float fIndex = (i / (_float)CASCADE_END);
	//	_float fUniformSplit = m_Frustum[SHADOW].fNear + (m_Frustum[SHADOW].fFar - m_Frustum[SHADOW].fNear) * fIndex;
	//	_float fLogarithmSplit = m_Frustum[SHADOW].fNear * std::powf((m_Frustum[SHADOW].fFar / m_Frustum[SHADOW].fNear), fIndex);
	//	m_fCascadeSplitZ[i] = fLogarithmSplit + fLamda * (fUniformSplit - fLogarithmSplit);
	//	m_fCascadeSplitZ[i] -= m_Frustum[SHADOW].fNear;
	//	m_fCascadeSplitZ[i] /= (m_Frustum[SHADOW].fFar - m_Frustum[SHADOW].fNear);
	//}

	//// 절투체를 구성하는 점 8개를 구하는 작업인데 일단 보류

	//for (_int i = 0; i < CASCADE_END; ++i) {
	//	//_float fSplitZ[2] = { m_fCascadeSplitZ[i],m_fCascadeSplitZ[i + 1] };
	//	//fSplitZ -= m_Frustum[SHADOW].fNear;
	//	//fSplitZ /= (m_Frustum[SHADOW].fFar - m_Frustum[SHADOW].fNear);
	//
	//	XMStoreFloat3(&m_vCascadePoints[i][0], XMVectorLerp(vPoints[0], vPoints[4], m_fCascadeSplitZ[i]));
	//	XMStoreFloat3(&m_vCascadePoints[i][1], XMVectorLerp(vPoints[1], vPoints[5], m_fCascadeSplitZ[i]));
	//	XMStoreFloat3(&m_vCascadePoints[i][2], XMVectorLerp(vPoints[2], vPoints[6], m_fCascadeSplitZ[i]));
	//	XMStoreFloat3(&m_vCascadePoints[i][3], XMVectorLerp(vPoints[3], vPoints[7], m_fCascadeSplitZ[i]));
	//	XMStoreFloat3(&m_vCascadePoints[i][4], XMVectorLerp(vPoints[0], vPoints[4], m_fCascadeSplitZ[i + 1]));
	//	XMStoreFloat3(&m_vCascadePoints[i][5], XMVectorLerp(vPoints[1], vPoints[5], m_fCascadeSplitZ[i + 1]));
	//	XMStoreFloat3(&m_vCascadePoints[i][6], XMVectorLerp(vPoints[2], vPoints[6], m_fCascadeSplitZ[i + 1]));
	//	XMStoreFloat3(&m_vCascadePoints[i][7], XMVectorLerp(vPoints[3], vPoints[7], m_fCascadeSplitZ[i + 1]));
	//}
}

void CPipeLine::Update_CascadeProjMatrices()
{
	//for (_int i = 0; i < CASCADE_END; ++i) {
	//
	//	// 1. 절두체의 중심 위치 구하기
	//	_vector vCenterPoints = {};
	//	for (_int j = 0; j < 8; ++j) {
	//		vCenterPoints += XMLoadFloat3(&m_vCascadePoints[i][j]);
	//	}
	//	vCenterPoints /= 8.f;
	//
	//	// 2. 중점 위치와 각 꼭지점 사이의 가장 큰 길이 구하기
	//	_float fRadius = 0.f;
	//	for (_int j = 0; j < 8; ++j) {
	//		_float fDistance = XMVector3Length(vCenterPoints - XMLoadFloat3(&m_vCascadePoints[i][j])).m128_f32[0];
	//		fRadius = max(fRadius, fDistance);
	//	}
	//}

	// 1. 절두체의 중점 위치 저장할 변수
	// 2. 중점 위치와 꼭지점 사이의 가장 큰 길이 구하기
	// 3. 구한 반지름을 기준으로 AABB 바운딩 박스 구성                                                                                                                                                                 
	//XMMatrixOrthographicOffCenterLH();
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

	Safe_Release(m_pCubeMapTexture);
	Safe_Release(m_pIrradialTexture);
	Safe_Release(m_pShaderCom);
}
