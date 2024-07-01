#include "Decal_Blood.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "Engine_Struct.h"
#include "Mesh.h"
#include "GameInstance.h"

CDecal_Blood::CDecal_Blood(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CDecal{ pDevice, pContext }
{
}

CDecal_Blood::CDecal_Blood(const CDecal_Blood& rhs)
	: CDecal{ rhs }
{

}

void CDecal_Blood::Add_Skinned_Decal(AddDecalInfo Info,RAYCASTING_INPUT Input)
{
	m_DecalConstData.rayOrigin = Info.rayOrigin;
	m_DecalConstData.rayDir = Info.rayDir;
	m_DecalConstData.decalTangent = Info.decalTangent;
	float hitDistanceRange = Info.maxHitDistance - Info.minHitDistance;
	m_DecalConstData.hitDistances=_float4(Info.minHitDistance, hitDistanceRange, 1.0f / hitDistanceRange, 0.0f);
	m_DecalConstData.decalSizeX = Info.decalSize.x;
	m_DecalConstData.decalSizeY = Info.decalSize.y;
	m_DecalConstData.decalSizeZ = Info.decalSize.z;
	m_DecalConstData.decalMaterialIndex = Info.decalMaterialIndex;
	m_DecalConstData.decalIndex++;
	Input.pUav_Info = m_pUAV_DecalInfo;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = m_pContext->Map(m_pCB_DecalConstData, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//if (SUCCEEDED(hr))
	{
		DecalConstData* bufferData = (DecalConstData*)mappedResource.pData;
		*bufferData = m_DecalConstData;
		m_pContext->Unmap(m_pCB_DecalConstData, 0);
	}

	Input.pCB_Decal = m_pCB_DecalConstData;

	m_pGameInstance->Bind_Resource_RayCasting(Input);
}

void CDecal_Blood::Bind_Resource_DecalInfo()
{

}

_uint CDecal_Blood::Staging_DecalInfo_RayCasting(_float* pDist)
{
	m_pContext->CopyResource(m_pStaging_Buffer_Decal_Info, m_pSB_DecalInfo);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	m_pContext->Map(m_pStaging_Buffer_Decal_Info, 0, D3D11_MAP_READ_WRITE, 0, &mappedResource);

	DecalInfo* pDecalInfo = reinterpret_cast<DecalInfo*>(mappedResource.pData);

	_uint iMeshIndex = (pDecalInfo->decalHitMask >> 20u) & 0xf;
	_uint iResult = pDecalInfo->isHit;
	_uint iHitDistance = (pDecalInfo->decalHitMask >> 24u) & 0xff;
	pDecalInfo->isHit = 0xffff;

	float hitDistanceN = iHitDistance / 255.0f;

	float hitDistancesX = m_DecalConstData.hitDistances.x;
	float hitDistancesZ = m_DecalConstData.hitDistances.z;
	float hitDistance = hitDistanceN / hitDistancesZ + hitDistancesX;

	*pDist = hitDistance;
	//pDecalInfo->decalHitMask = 0;

	m_pContext->Unmap(m_pStaging_Buffer_Decal_Info, 0);

	m_pContext->CopyResource(m_pSB_DecalInfo, m_pStaging_Buffer_Decal_Info);

	if (iResult == 0xfff)
		return iMeshIndex;
	else
		return 999;
}

void CDecal_Blood::Staging_Calc_Decal_Info()
{
	m_pContext->CopyResource(m_pStaging_Buffer_Decal_Info, m_pSB_DecalInfo);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	m_pContext->Map(m_pStaging_Buffer_Decal_Info, 0, D3D11_MAP_READ_WRITE, 0, &mappedResource);

	DecalInfo* pDecalInfo = reinterpret_cast<DecalInfo*>(mappedResource.pData);

	_uint iMeshIndex = (pDecalInfo->decalHitMask >> 20u) & 0xf;
	_uint iResult = pDecalInfo->isHit;
	_uint iHitDistance = (pDecalInfo->decalHitMask >> 24u) & 0xff;
	//pDecalInfo->isHit = 0xffff;
	//pDecalInfo->decalHitMask = 0;

	m_pContext->Unmap(m_pStaging_Buffer_Decal_Info, 0);

	//m_pContext->CopyResource(m_pSB_DecalInfo, m_pStaging_Buffer_Decal_Info);
}

void CDecal_Blood::Calc_Decal_Info(CALC_DECAL_INPUT Input)
{
	Input.pCB_Decal = m_pCB_DecalConstData;
	Input.pUav_Info = m_pUAV_DecalInfo;

	m_pGameInstance->Bind_Resource_Calc_Decal_Info(Input);
}

void CDecal_Blood::Bind_Resource_DecalMap(CALC_DECAL_MAP_INPUT Input)
{
	_matrix WorldInv = XMMatrixInverse(nullptr, m_pTransformCom->Get_WorldMatrix());
	_float4x4 WorldInv_Float4x4;
	XMStoreFloat4x4(&WorldInv_Float4x4, WorldInv);
	Input.Decal_Matrix_Inv = WorldInv_Float4x4;


	Input.pDecalMap = m_pUAV_DecalMap;
	Input.vExtent = m_vExtent;

	m_pGameInstance->Bind_Resource_Calc_Decal_Map(Input);
}

void CDecal_Blood::Bind_DecalMap(CShader* pShader)
{
	pShader->Bind_Uav("g_DecalMap", m_pUAV_DecalMap);
}

void CDecal_Blood::Staging_DecalMap()
{
	m_pContext->CopyResource(m_pStaging_Buffer_Decal_Map, m_pSB_DecalMap);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	m_pContext->Map(m_pStaging_Buffer_Decal_Map, 0, D3D11_MAP_READ, 0, &mappedResource);

	_float2* pDecalMap = reinterpret_cast<_float2*>(mappedResource.pData);

	m_pContext->Unmap(m_pStaging_Buffer_Decal_Map, 0);
}

HRESULT CDecal_Blood::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDecal_Blood::Initialize(void* pArg)
{
	__super::Initialize(nullptr);

	if (pArg != nullptr)
	{
		DECAL_BLOOD_DESC* pDesc = (DECAL_BLOOD_DESC*)pArg;
		m_DecalConstData.subModelInfo.firstIndex = 0;
		m_DecalConstData.subModelInfo.numTris = pDesc->iNumIndices / 3;
		m_DecalConstData.subModelInfo.decalLookupMapWidth = static_cast<float>(512);
		m_DecalConstData.subModelInfo.decalLookupMapHeight = static_cast<float>(512);
		m_DecalConstData.decalLookupRtWidth = static_cast<float>(512);
		m_DecalConstData.decalLookupRtHeight = static_cast<float>(512);
		m_DecalConstData.iMeshIndex = pDesc->iMeshIndex;
	}
	/*decalLookupTexture = Demo::resourceManager->CreateTexture(desc, "Decal lookup map");
	if (!decalLookupTextures[numDecalLookupTextures])
		return false;
	subModel.materialDT.AddTextureSrv(decalLookupTextures[numDecalLookupTextures]);*/

	//DXGI_FORMAT_R32_UINT

	// 상수 버퍼 설명
	D3D11_BUFFER_DESC cbufferDesc = {};
	cbufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbufferDesc.ByteWidth = sizeof(DecalConstData);
	cbufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = &m_DecalConstData;

	HRESULT hr = m_pDevice->CreateBuffer(&cbufferDesc, &initData, &m_pCB_DecalConstData);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	m_DecalInfo = new DecalInfo;
	m_DecalInfo->decalHitMask = 0;
	m_DecalInfo->decalIndex = 0;
	m_DecalInfo->isHit = 0xffff;

	//DecalInfo Buffer for Write
	{
		D3D11_BUFFER_DESC sbDesc = {};
		sbDesc.Usage = D3D11_USAGE_DEFAULT;
		sbDesc.ByteWidth = sizeof(DecalInfo);
		sbDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		sbDesc.StructureByteStride = sizeof(DecalInfo);
		sbDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // 추가

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = &m_DecalInfo;

		HRESULT hr = m_pDevice->CreateBuffer(&sbDesc, &initData, &m_pSB_DecalInfo);

		if (FAILED(hr))
			return E_FAIL;

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.NumElements = sbDesc.ByteWidth / sbDesc.StructureByteStride;

		hr = m_pDevice->CreateUnorderedAccessView(m_pSB_DecalInfo, &uavDesc, &m_pUAV_DecalInfo);

		if (FAILED(hr))
			return E_FAIL;
	}

	//Create Staging Buffer
	D3D11_BUFFER_DESC stagingDesc = {};
	stagingDesc.Usage = D3D11_USAGE_STAGING;
	stagingDesc.ByteWidth = sizeof(DecalInfo);
	stagingDesc.BindFlags = 0;
	stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	stagingDesc.StructureByteStride = sizeof(DecalInfo);

	m_pDevice->CreateBuffer(&stagingDesc, nullptr, &m_pStaging_Buffer_Decal_Info);


	//Create Staging Buffer
	stagingDesc.Usage = D3D11_USAGE_STAGING;
	stagingDesc.ByteWidth = sizeof(_float2)*m_iNumVertices;
	stagingDesc.BindFlags = 0;
	stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	stagingDesc.StructureByteStride = sizeof(_float2);

	m_pDevice->CreateBuffer(&stagingDesc, nullptr, &m_pStaging_Buffer_Decal_Map);



	m_pDecal_Map = new _float2[m_iNumVertices];
	ZeroMemory(m_pDecal_Map, sizeof(_float2) * m_iNumVertices);
	//Buffer for Write
	{
		D3D11_BUFFER_DESC sbDesc = {};
		sbDesc.Usage = D3D11_USAGE_DEFAULT;
		sbDesc.ByteWidth = sizeof(_float2) * m_iNumVertices;
		sbDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		sbDesc.StructureByteStride = sizeof(_float2);
		sbDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // 추가

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = m_pDecal_Map;

		HRESULT hr = m_pDevice->CreateBuffer(&sbDesc, &initData, &m_pSB_DecalMap);

		if (FAILED(hr))
			return E_FAIL;

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.NumElements = sbDesc.ByteWidth / sbDesc.StructureByteStride;

		hr = m_pDevice->CreateUnorderedAccessView(m_pSB_DecalMap, &uavDesc, &m_pUAV_DecalMap);

		if (FAILED(hr))
			return E_FAIL;
	}

	m_vExtent = _float3(0.1f,0.1f,0.1f);

	return S_OK;
}

void CDecal_Blood::Tick(_float fTimeDelta)
{
	
}

void CDecal_Blood::Late_Tick(_float fTimeDelta)
{
	
}

HRESULT CDecal_Blood::Render()
{
	return S_OK;
}

HRESULT CDecal_Blood::Add_Components()
{
	return S_OK;
}

HRESULT CDecal_Blood::Bind_ShaderResources()
{
	return S_OK;
}

CDecal_Blood* CDecal_Blood::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDecal_Blood* pInstance = new CDecal_Blood(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDecal_Blood"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CDecal_Blood::Clone(void* pArg)
{
	CDecal_Blood* pInstance = new CDecal_Blood(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CDecal_Blood"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDecal_Blood::Free()
{
	__super::Free();
}
