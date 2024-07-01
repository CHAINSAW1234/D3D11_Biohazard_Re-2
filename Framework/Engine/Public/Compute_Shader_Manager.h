#pragma once

#include "Base.h"


BEGIN(Engine)

class CCompute_Shader_Manager final : public CBase
{
private:
	CCompute_Shader_Manager();
	virtual ~CCompute_Shader_Manager() = default;

public:
	HRESULT					Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	//Skinning
	void					Bind_Resource_Skinning(SKINNING_INPUT Input);
	void					Bind_Essential_Resource_Skinning(_float4x4 WorldMat, _float4x4* pBoneMatrices);
	void					Perform_Skinning(_uint iNumVertices);

public:
	//RayCasting
	void					Bind_Resource_DecalInfo_RayCasting();
	void					Bind_Resource_RayCasting(RAYCASTING_INPUT Input);
	void					Perform_RayCasting(_uint iNumTris);

public:
	//Calc Decal Info
	void					Bind_Resource_CalcDecalInfo(CALC_DECAL_INPUT Input);
	void					Perform_Calc_Decal_Info();

public:
	//Calc Decal Map
	void					Bind_Resource_CalcMap(CALC_DECAL_MAP_INPUT Input);
	void					Perform_Calc_Decal_Map(_uint iNumVertices);

public:

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
private:
	class CComputeShader* m_pSkinng = { nullptr };
	class CComputeShader* m_pRayCasting = { nullptr };
	class CComputeShader* m_pCalcDecalInfo = { nullptr };
	class CComputeShader* m_pCalcDecalMap = { nullptr };

public:
	static CCompute_Shader_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END