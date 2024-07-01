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

	m_pSkinng = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Skinning.hlsl"), "CS_Skinnig");
	m_pRayCasting = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/RayCast.hlsl"), "CS_RayCast");
	m_pCalcDecalInfo = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/CalcDecalInfo.hlsl"), "CS_CalcDecalInfo");
	m_pCalcDecalMap = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/CalcDecalMap.hlsl"), "CS_CalcDecalMap");

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
	m_pSkinng->Bind_RawValue("g_NumVertices", &Input.iNumVertex, sizeof(_uint));
}

void CCompute_Shader_Manager::Bind_Essential_Resource_Skinning(_float4x4* pWorldMat, _float4x4* pBoneMatrices)
{
	m_pSkinng->Bind_RawValue("g_WorldMatrix", pWorldMat, sizeof(_float4x4));
	m_pSkinng->Bind_Matrices("g_BoneMatrices", pBoneMatrices, 512);
}

void CCompute_Shader_Manager::Perform_Skinning(_uint iNumVertices)
{
	_uint numThreadGroupsX = (iNumVertices + SKINNING_THREAD_GROUP_SIZE - 1) / SKINNING_THREAD_GROUP_SIZE;
	_uint numThreadGroupsY = 1;
	_uint numThreadGroupsZ = 1;
	m_pSkinng->Render(0, numThreadGroupsX, numThreadGroupsY, numThreadGroupsZ);
}

void CCompute_Shader_Manager::Bind_Resource_DecalInfo_RayCasting()
{
}

void CCompute_Shader_Manager::Bind_Resource_RayCasting(RAYCASTING_INPUT Input)
{
	m_pRayCasting->Bind_Uav("decalInfoBuffer", Input.pUav_Info);
	m_pRayCasting->Bind_Uav("positionBuffer", Input.pUav_Skinning);
	m_pRayCasting->Bind_Structured_Buffer("indexBuffer", Input.pSRV_Indices);
	m_pRayCasting->Bind_RawValue("g_NumTris", &Input.iNumTriangle, sizeof(_uint));
	m_pRayCasting->Bind_Constant_Buffer("decalCB", Input.pCB_Decal);
}

void CCompute_Shader_Manager::Perform_RayCasting(_uint iNumTris)
{
	_uint numThreadGroupsX = (iNumTris + RAYCAST_THREAD_GROUP_SIZE - 1) / RAYCAST_THREAD_GROUP_SIZE;
	_uint numThreadGroupsY = 1;
	_uint numThreadGroupsZ = 1;
	m_pRayCasting->Render(0, numThreadGroupsX, numThreadGroupsY, numThreadGroupsZ);
}

void CCompute_Shader_Manager::Bind_Resource_CalcDecalInfo(CALC_DECAL_INPUT Input)
{
	m_pCalcDecalInfo->Bind_Uav("decalInfoBuffer", Input.pUav_Info);
	m_pCalcDecalInfo->Bind_Uav("positionBuffer", Input.pUav_Skinning);
	m_pCalcDecalInfo->Bind_Structured_Buffer("indexBuffer", Input.pSRV_Indices);
	m_pCalcDecalInfo->Bind_Constant_Buffer("decalCB", Input.pCB_Decal);
}

void CCompute_Shader_Manager::Perform_Calc_Decal_Info()
{
	_uint numThreadGroupsX = 1;
	_uint numThreadGroupsY = 1;
	_uint numThreadGroupsZ = 1;
	m_pCalcDecalInfo->Render(0, numThreadGroupsX, numThreadGroupsY, numThreadGroupsZ);
}

void CCompute_Shader_Manager::Bind_Resource_CalcMap(CALC_DECAL_MAP_INPUT Input)
{
	m_pCalcDecalMap->Bind_Uav("g_Skinnig_Output",Input.pUav_Skinning);
	m_pCalcDecalMap->Bind_Structured_Buffer("g_Texcoords", Input.pSRV_Texcoords);
	m_pCalcDecalMap->Bind_Uav("g_DecalMap", Input.pDecalMap);
	m_pCalcDecalMap->Bind_Matrix("g_DecalMat_Inv", &Input.Decal_Matrix_Inv);
	m_pCalcDecalMap->Bind_RawValue("g_NumVertices", &Input.iNumVertex, sizeof(_uint));
	m_pCalcDecalMap->Bind_RawValue("g_Extent", &Input.vExtent, sizeof(_float3));
}

void CCompute_Shader_Manager::Perform_Calc_Decal_Map(_uint iNumVertices)
{
	_uint numThreadGroupsX = (iNumVertices + CALC_DECAL_MAP_THREAD_GROUP_SIZE - 1) / CALC_DECAL_MAP_THREAD_GROUP_SIZE;
	_uint numThreadGroupsY = 1;
	_uint numThreadGroupsZ = 1;
	m_pCalcDecalMap->Render(0, numThreadGroupsX, numThreadGroupsY, numThreadGroupsZ);
}

CCompute_Shader_Manager* CCompute_Shader_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCompute_Shader_Manager* pInstance = new CCompute_Shader_Manager();

	if (FAILED(pInstance->Initialize(pDevice, pContext)))
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
	Safe_Release(m_pCalcDecalMap);
}
