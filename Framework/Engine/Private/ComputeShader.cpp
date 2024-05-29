#include "ComputeShader.h"

#include "RenderTarget.h"

CComputeShader::CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

HRESULT CComputeShader::Initialize(const wstring& strShaderFilePath, const string& strEntryPoint)
{
//	UINT iHlslFlag = D3DCOMPILE_ENABLE_STRICTNESS;
//#ifdef _DEBUG 
//	iHlslFlag |= D3DCOMPILE_DEBUG;
//	iHlslFlag |= D3DCOMPILE_SKIP_OPTIMIZATION;
//#endif

	_uint		iHlslFlag = { 0 };
#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else	
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif

	if (FAILED(D3DX11CompileEffectFromFile(strShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;

	ID3DX11EffectTechnique* tech = m_pEffect->GetTechniqueByIndex(0);

	ID3DX11EffectPass* pPass = tech->GetPassByIndex(0);

	D3DX11_PASS_DESC Desc = {};

	D3DX11_PASS_SHADER_DESC ComputeshaderDesc;

	pPass->GetComputeShaderDesc(&ComputeshaderDesc);

	D3DX11_EFFECT_SHADER_DESC shaderDesc = {};

	ComputeshaderDesc.pShaderVariable->GetShaderDesc(ComputeshaderDesc.ShaderIndex, &shaderDesc);


	if(FAILED(m_pDevice->CreateComputeShader(shaderDesc.pBytecode, shaderDesc.BytecodeLength, nullptr, &m_pComputeShader)))
		return E_FAIL;




	//pPass->GetComputeShaderDesc(&Desc);

	//if(FAILED(m_pDevice->CreateComputeShader(Desc.pIAInputSignature, Desc.IAInputSignatureSize, nullptr, &m_pComputeShader)))
	//	return E_FAIL;


	//ID3DBlob* ShaderBlob = { nullptr };
	//ID3DBlob* ErrorBlob = { nullptr };

	//if (FAILED(D3DCompileFromFile(strShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, strEntryPoint.c_str(), "cs_5_0", iHlslFlag, 0, &ShaderBlob, &ErrorBlob))) {
	//	if (ErrorBlob)
	//	{
	//		MSG_BOX_C(((char*)ErrorBlob->GetBufferPointer()));

	//		ErrorBlob->Release();
	//	}

	//	if (ShaderBlob)
	//		ShaderBlob->Release();

	//	return E_FAIL;
	//}

	//if (FAILED(m_pDevice->CreateComputeShader(ShaderBlob->GetBufferPointer(), ShaderBlob->GetBufferSize(), nullptr, &m_pComputeShader))) {
	//	
	//	if (ShaderBlob)
	//		ShaderBlob->Release();

	//	return E_FAIL;
	//}

	//ShaderBlob->Release();

	return S_OK;
}

HRESULT CComputeShader::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix)
{
	/* 타입의 구분없이 pConstantName이름을 가진 전역변수에 기능을 할 수 있는 컴객체를 얻어온다. */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	/* 실제 얻어온 전역변수 객체의 타입을 명시하여 변환한다. */
	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrix((_float*)pMatrix);
}

HRESULT CComputeShader::Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrices, _uint iNumMatrices)
{
	/* 타입의 구분없이 pConstantName이름을 가진 전역변수에 기능을 할 수 있는 컴객체를 얻어온다. */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	/* 실제 얻어온 전역변수 객체의 타입을 명시하여 변환한다. */
	ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrixArray((_float*)pMatrices, 0, iNumMatrices);
}

HRESULT CComputeShader::Bind_Texture(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
	/* 타입의 구분없이 pConstantName이름을 가진 전역변수에 기능을 할 수 있는 컴객체를 얻어온다. */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	/* 실제 얻어온 전역변수 객체의 타입을 명시하여 변환한다. */
	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		return E_FAIL;

	return pSRVariable->SetResource(pSRV);
}

HRESULT CComputeShader::Bind_Textures(const _char* pConstantName, ID3D11ShaderResourceView** ppSRVs, _uint iNumTextures)
{
	/* 타입의 구분없이 pConstantName이름을 가진 전역변수에 기능을 할 수 있는 컴객체를 얻어온다. */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	/* 실제 얻어온 전역변수 객체의 타입을 명시하여 변환한다. */
	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		return E_FAIL;

	return pSRVariable->SetResourceArray(ppSRVs, 0, iNumTextures);
}

HRESULT CComputeShader::Bind_Texture_Cube(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
	/* 타입의 구분없이 pConstantName이름을 가진 전역변수에 기능을 할 수 있는 컴객체를 얻어온다. */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;
	/* 실제 얻어온 전역변수 객체의 타입을 명시하여 변환한다. */

	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		return E_FAIL;
	return pSRVariable->SetResource(pSRV);
}

HRESULT CComputeShader::Bind_RawValue(const _char* pConstantName, const void* pData, _uint iLength)
{
	/* 타입의 구분없이 pConstantName이름을 가진 전역변수에 기능을 할 수 있는 컴객체를 얻어온다. */
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pData, 0, iLength);
}

HRESULT CComputeShader::Bind_Texture(const _char* pConstantName, ID3D11UnorderedAccessView* pUAV)
{
	ID3DX11EffectUnorderedAccessViewVariable* pUAVVariable = m_pEffect->GetVariableByName(pConstantName)->AsUnorderedAccessView();
	if (nullptr == pUAVVariable)
		return E_FAIL;

	return pUAVVariable->SetUnorderedAccessView(pUAV);
}

HRESULT CComputeShader::Render()
{
	ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(0);
	if (nullptr == pPass)
		return E_FAIL;

	pPass->Apply(0, m_pContext);


	m_pContext->Dispatch(8, 8, 8);
	
	ID3D11UnorderedAccessView* NullUAV = { nullptr };
	m_pContext->CSSetUnorderedAccessViews(0, 1, &NullUAV, nullptr);

	return S_OK;
}

CComputeShader* CComputeShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strShaderFilePath, const string& entryPoint)
{
	CComputeShader* pInstance = new CComputeShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(strShaderFilePath, entryPoint)))
	{
		MSG_BOX(TEXT("Failed To Created : CShader"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CComputeShader::Free()
{
	__super::Free();
	Safe_Release(m_pEffect);
	Safe_Release(m_pComputeShader);
	
}
