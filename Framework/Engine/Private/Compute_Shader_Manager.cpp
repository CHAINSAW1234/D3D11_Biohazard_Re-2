#include "..\Public\Compute_Shader_Manager.h"

#include "GameInstance.h"
#include "ComputeShader.h"

CCompute_Shader_Manager::CCompute_Shader_Manager()
{
}

HRESULT CCompute_Shader_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(m_pGameInstance);

	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);

	m_pSkinng = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Skinning.csh"), "CS_Skinnig");
	m_pRayCasting = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/RayCast.csh"), "CS_RayCast");

	return S_OK;
}

void CCompute_Shader_Manager::Bind_Resource_Skinning(SKINNING_INPUT Input)
{
	m_pSkinng->Bind_Uav("g_Skinnig_Output", Input.pUav);
	m_pSkinng->Bind_Structured_Buffer("g_VertexPositions", Input.pSRV_Vertex_Position);
	m_pSkinng->Bind_Structured_Buffer("g_VertexNormals", Input.pSRV_Vertex_Normal);
	m_pSkinng->Bind_Structured_Buffer("g_VertexTangents", Input.pSRV_Vertex_Tangent);
	m_pSkinng->Bind_Structured_Buffer("g_VertexBlendIndices", Input.pSRV_Vertex_BlendIndices);
	m_pSkinng->Bind_Structured_Buffer("g_VertexBlendWeights", Input.pSRV_Vertex_BlendWeights);
	m_pSkinng->Bind_RawValue("g_NumVertices",&Input.iNumVertex,sizeof(_uint));
}

void CCompute_Shader_Manager::Bind_Essential_Resource_Skinning(_float4x4 WorldMat, _float4x4* pBoneMatrices)
{
	m_pSkinng->Bind_RawValue("g_WorldMatrix", &WorldMat, sizeof(_float4x4));
	m_pSkinng->Bind_Matrices("g_BoneMatrices", pBoneMatrices, 512);
}

void CCompute_Shader_Manager::Perform_Skinning(_uint iNumVertices)
{
	_uint numThreadGroupsX = (iNumVertices + SKINNING_THREAD_GROUP_SIZE - 1) / SKINNING_THREAD_GROUP_SIZE;
	_uint numThreadGroupsY = 1;
	_uint numThreadGroupsZ = 1;
	m_pSkinng->Render(0, numThreadGroupsX, numThreadGroupsY, numThreadGroupsZ);
}

CCompute_Shader_Manager * CCompute_Shader_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCompute_Shader_Manager*		pInstance = new CCompute_Shader_Manager();

	if (FAILED(pInstance->Initialize(pDevice,pContext)))
	{
		MSG_BOX(TEXT("Failed To Created : CCompute_Shader_Manager"));

		Safe_Release(pInstance);
	}

	return pInstance;
}


void CCompute_Shader_Manager::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pSkinng);
	Safe_Release(m_pRayCasting);
	Safe_Release(m_pCalcDecalInfo);
}
