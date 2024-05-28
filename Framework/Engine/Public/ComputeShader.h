#pragma once
#include "Component.h"

BEGIN(Engine)

// Renderer������ ���
class ENGINE_DLL CComputeShader final : public CComponent
{
private:
	CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CComputeShader() = default;

public:
	virtual HRESULT Initialize(const wstring& strShaderFilePath, const string& entryPoint);

public:

	HRESULT Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix);
	HRESULT Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrices, _uint iNumMatrices);
	HRESULT Bind_Texture(const _char* pConstantName, ID3D11ShaderResourceView* pSRV);
	HRESULT Bind_Textures(const _char* pConstantName, ID3D11ShaderResourceView** ppSRVs, _uint iNumTextures);
	HRESULT Bind_Texture_Cube(const _char* pConstantName, ID3D11ShaderResourceView* pSRV);
	// UAV : For Output
	HRESULT Bind_Texture(const _char* pConstantName, ID3D11UnorderedAccessView* pUAV);

	HRESULT Bind_RawValue(const _char* pConstantName, const void* pData, _uint iLength);

	HRESULT Render();

private:
	ID3D11ComputeShader* m_pComputeShader = { nullptr };
	ID3DX11Effect* m_pEffect = { nullptr };

public:
	static CComputeShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strShaderFilePath, const string& entryPoint);
	virtual CComponent* Clone(void* pArg) override { return nullptr; }
	virtual void Free() override;
};

END