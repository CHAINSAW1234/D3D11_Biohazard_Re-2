#include "..\Public\Shader.h"

CShader::CShader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent{ pDevice, pContext }
{
}

CShader::CShader(const CShader & rhs)
	: CComponent{ rhs }
	, m_pEffect { rhs.m_pEffect }
	, m_InputLayouts { rhs.m_InputLayouts }
{
	Safe_AddRef(m_pEffect);

	for (auto& pInputLayout : m_InputLayouts)
		Safe_AddRef(pInputLayout);
}

HRESULT CShader::Initialize_Prototype(const wstring & strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	_uint		iHlslFlag = { 0 };
#ifdef _DEBUG
	iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else	
	iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif 
	if (FAILED(D3DX11CompileEffectFromFile(strShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;
	
	ID3DX11EffectTechnique*		pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC		TechniqueDesc{};
	pTechnique->GetDesc(&TechniqueDesc);

	for (_uint i = 0; i < TechniqueDesc.Passes; ++i)
	{
		ID3DX11EffectPass*		pPass = pTechnique->GetPassByIndex(i);
		
		D3DX11_PASS_DESC		PassDesc{};
		pPass->GetDesc(&PassDesc);

		ID3D11InputLayout*		pInputLayout = { nullptr };
		
		if (FAILED(m_pDevice->CreateInputLayout(pElements,
			iNumElements, 
			PassDesc.pIAInputSignature,
			PassDesc.IAInputSignatureSize,
			&pInputLayout)))
			return E_FAIL;

		m_InputLayouts.push_back(pInputLayout);
	}
	return S_OK;
}

HRESULT CShader::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CShader::Begin(_uint iPassIndex)
{
	if (iPassIndex >= m_InputLayouts.size())
		return E_FAIL;

	ID3DX11EffectTechnique*		pTechnique = m_pEffect->GetTechniqueByIndex(0);
	if (nullptr == pTechnique)
		return E_FAIL;

	ID3DX11EffectPass*			pPass = pTechnique->GetPassByIndex(iPassIndex);
	if (nullptr == pPass)
		return E_FAIL;	

	m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);

	pPass->Apply(0, m_pContext);

	//if (iPassIndex >= m_InputLayouts.size())
	//	return E_FAIL;

	//ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
	//if (nullptr == pTechnique)
	//	return E_FAIL;

	//ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(iPassIndex);
	//if (nullptr == pPass)
	//	return E_FAIL;

	///*m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);

	//pPass->Apply(0, m_pContext);*/

	//// 디퍼드 컨텍스트 생성
	//ID3D11DeviceContext* pDeferredContext = nullptr;
	//HRESULT hr = m_pDevice->CreateDeferredContext(0, &pDeferredContext);
	//if (FAILED(hr))
	//	return hr;

	//// 디퍼드 컨텍스트에서 명령 기록
	//pDeferredContext->IASetInputLayout(m_InputLayouts[iPassIndex]);
	//pPass->Apply(0, pDeferredContext);

	//// 명령 리스트 생성
	//ID3D11CommandList* pCommandList = nullptr;
	//hr = pDeferredContext->FinishCommandList(FALSE, &pCommandList);
	//if (FAILED(hr)) {
	//	pDeferredContext->Release();
	//	return hr;
	//}

	//// 메인 컨텍스트에서 명령 리스트 실행
	//m_pContext->ExecuteCommandList(pCommandList, TRUE);

	//// 리소스 해제
	//pCommandList->Release();
	//pDeferredContext->Release();	

	return S_OK;
}

HRESULT CShader::Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix)
{
	ID3DX11EffectVariable*	pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable*	pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrix((_float*)pMatrix);	
	}

HRESULT CShader::Bind_Matrices(const _char * pConstantName, const _float4x4 * pMatrices, _uint iNumMatrices)
{
	ID3DX11EffectVariable*	pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable*	pMatrixVariable = pVariable->AsMatrix();
	if (nullptr == pMatrixVariable)
		return E_FAIL;

	return pMatrixVariable->SetMatrixArray((_float*)pMatrices, 0, iNumMatrices);
}

HRESULT CShader::Bind_Texture(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
	ID3DX11EffectVariable*	pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable*	pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		return E_FAIL;

	return pSRVariable->SetResource(pSRV);
}

HRESULT CShader::Bind_Textures(const _char * pConstantName, ID3D11ShaderResourceView ** ppSRVs, _uint iNumTextures)
{
	ID3DX11EffectVariable*	pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable*	pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		return E_FAIL;

	return pSRVariable->SetResourceArray(ppSRVs, 0, iNumTextures);
}

HRESULT CShader::Bind_Texture_Cube(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pSRVariable = pVariable->AsShaderResource();
	if (nullptr == pSRVariable)
		return E_FAIL;
	return pSRVariable->SetResource(pSRV);
	
}

HRESULT CShader::Bind_RawValue(const _char * pConstantName, const void * pData, _uint iLength)
{
	ID3DX11EffectVariable*	pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pData, 0, iLength);	
}


HRESULT CShader::Bind_Constant_Buffer(const _char* pConstantName, ID3D11Buffer* pCB)
{
	/*ID3DX11EffectConstantBuffer* pEffectCB = m_pEffect->GetVariableByName(pConstantName)->AsConstantBuffer();
	if (nullptr == pEffectCB)
		return E_FAIL;*/

	ID3DX11EffectConstantBuffer* pEffectCB = m_pEffect->GetConstantBufferByName(pConstantName);
	if (nullptr == pEffectCB)
		return E_FAIL;

	return pEffectCB->SetConstantBuffer(pCB);
}

HRESULT CShader::Bind_Structured_Buffer(const _char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
	ID3DX11EffectShaderResourceVariable* pEffectSRV = m_pEffect->GetVariableByName(pConstantName)->AsShaderResource();
	if (nullptr == pEffectSRV)
		return E_FAIL;

	return pEffectSRV->SetResource(pSRV);
}

HRESULT CShader::Bind_Uav(const _char* pConstantName, ID3D11UnorderedAccessView* pUav)
{
	ID3DX11EffectUnorderedAccessViewVariable* pUAVVariable = m_pEffect->GetVariableByName(pConstantName)->AsUnorderedAccessView();
	if (nullptr == pUAVVariable)
		return E_FAIL;

	return pUAVVariable->SetUnorderedAccessView(pUav);
}

CShader * CShader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
	CShader*		pInstance = new CShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strShaderFilePath, pElements, iNumElements)))
	{
		MSG_BOX(TEXT("Failed To Created : CShader"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CShader::Clone(void * pArg)
{
	CShader*		pInstance = new CShader(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CShader"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShader::Free()
{
	__super::Free();

	Safe_Release(m_pEffect);

	for (auto& pInputLayout : m_InputLayouts)
		Safe_Release(pInputLayout);

	m_InputLayouts.clear();

}
