#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CComputeShader final : public CComponent
{
private:
	CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CComputeShader() = default;

public:
	virtual HRESULT Initialize(const wstring& strShaderFilePath, const string& entryPoint);

public:
	//HRESULT Begin(_uint iPassIndex);
	//HRESULT Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix);
	//HRESULT Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrices, _uint iNumMatrices);
	//HRESULT Bind_Texture(const _char* pConstantName, ID3D11ShaderResourceView* pSRV);
	//HRESULT Bind_Textures(const _char* pConstantName, ID3D11ShaderResourceView** ppSRVs, _uint iNumTextures);
	//HRESULT Bind_Texture_Cube(const _char* pConstantName, ID3D11ShaderResourceView* pSRV);
	//HRESULT Bind_RawValue(const _char* pConstantName, const void* pData, _uint iLength);

private:
	ID3D11ComputeShader* m_pComputeShader = { nullptr };




public:
	static CComputeShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strShaderFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END