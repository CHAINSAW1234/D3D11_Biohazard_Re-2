#include "ComputeShader.h"

CComputeShader::CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

HRESULT CComputeShader::Initialize(const wstring& strShaderFilePath, const string& strEntryPoint)
{
	UINT iHlslFlag = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG 
	iHlslFlag |= D3DCOMPILE_DEBUG;
	iHlslFlag |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif


	ID3DBlob* ShaderBlob = { nullptr };
	ID3DBlob* ErrorBlob = { nullptr };

	if (FAILED(D3DCompileFromFile(strShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, strEntryPoint.c_str(), "cs_5_0", iHlslFlag, 0, &ShaderBlob, &ErrorBlob))) {
		if (ErrorBlob)
		{
			MSG_BOX_C(((char*)ErrorBlob->GetBufferPointer()));

			ErrorBlob->Release();
		}

		if (ShaderBlob)
			ShaderBlob->Release();

		return E_FAIL;
	}

	if (FAILED(m_pDevice->CreateComputeShader(ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize(), nullptr, &m_pComputeShader))) {
		
		if (ShaderBlob)
			ShaderBlob->Release();

		return E_FAIL;
	}

	ShaderBlob->Release();

	return S_OK;
}
CComputeShader* CComputeShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strShaderFilePath)
{
	return nullptr;
}

CComponent* CComputeShader::Clone(void* pArg)
{

	MSG_BOX(TEXT("Failed To Cloned : CShader"));

	return nullptr;
}

void CComputeShader::Free()
{
}
