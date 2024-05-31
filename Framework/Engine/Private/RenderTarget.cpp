#include "..\Public\RenderTarget.h"

#include "GameInstance.h"
#include "ComputeShader.h"

CRenderTarget::CRenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pGameInstance{ CGameInstance::Get_Instance()}
	, m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget::Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const wstring& strRenderTargetTag, const _float4 & vClearColor)
{
	m_strRenderTargetTag = strRenderTargetTag;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iSizeX;
	TextureDesc.Height = iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = ePixelFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;		
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV)))
		return E_FAIL;

	m_vClearColor = vClearColor;

	return S_OK;
}

HRESULT CRenderTarget::Initialize_Cube(_uint iSize, _uint iArraySize, DXGI_FORMAT ePixelFormat, const wstring& strRenderTargetTag, const _float4& vClearColor)
{
	m_strRenderTargetTag = strRenderTargetTag;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iSize;
	TextureDesc.Height = iSize;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 6* iArraySize;
	TextureDesc.Format = ePixelFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	D3D11_RENDER_TARGET_VIEW_DESC RenderTargetDesc = {};
	RenderTargetDesc.Format = TextureDesc.Format;
	RenderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	RenderTargetDesc.Texture2DArray.ArraySize = 6 * iArraySize;
	RenderTargetDesc.Texture2DArray.FirstArraySlice = 0;
	RenderTargetDesc.Texture2DArray.MipSlice = 0;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, &RenderTargetDesc, &m_pRTV)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC ResourceViewDesc = {};
	ResourceViewDesc.Format = TextureDesc.Format;
	ResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
	ResourceViewDesc.TextureCubeArray.First2DArrayFace = 0;
	ResourceViewDesc.TextureCubeArray.NumCubes = iArraySize;
	ResourceViewDesc.TextureCubeArray.MipLevels = 1;
	ResourceViewDesc.TextureCubeArray.MostDetailedMip = 0;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, &ResourceViewDesc, &m_pSRV)))
		return E_FAIL;

	m_vClearColor = vClearColor;

	return S_OK;
}

HRESULT CRenderTarget::Initialize_3D(_uint iWidth, _uint iHeight, _uint iDepth, DXGI_FORMAT ePixelFormat, const wstring& strRenderTargetTag, const _float4& vClearColor)
{
	m_strRenderTargetTag = strRenderTargetTag;

	D3D11_TEXTURE3D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE3D_DESC));
	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.Depth = iDepth;
	TextureDesc.MipLevels = 1;
	TextureDesc.Format = ePixelFormat;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture3D(&TextureDesc, nullptr, &m_pTexture3D)))
		return E_FAIL;

	D3D11_RENDER_TARGET_VIEW_DESC RenderTargetDesc = {};
	RenderTargetDesc.Format = TextureDesc.Format;
	RenderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
	RenderTargetDesc.Texture3D.FirstWSlice = 0;
	RenderTargetDesc.Texture3D.MipSlice = 0;
	RenderTargetDesc.Texture3D.WSize = -1;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture3D, &RenderTargetDesc, &m_pRTV)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC ResourceViewDesc = {};
	ResourceViewDesc.Format = TextureDesc.Format;
	ResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	ResourceViewDesc.Texture3D.MipLevels = 1;
	ResourceViewDesc.Texture3D.MostDetailedMip = 0;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture3D, &ResourceViewDesc, &m_pSRV)))
		return E_FAIL;

	D3D11_UNORDERED_ACCESS_VIEW_DESC   UnorderedAccessDesc = {};
	UnorderedAccessDesc.Format = TextureDesc.Format;
	UnorderedAccessDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
	UnorderedAccessDesc.Texture3D.MipSlice = 0;
	UnorderedAccessDesc.Texture3D.FirstWSlice = 0;
	UnorderedAccessDesc.Texture3D.WSize = -1;

	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pTexture3D, &UnorderedAccessDesc, &m_pUAV)))
		return E_FAIL;

	m_vClearColor = vClearColor;

	return S_OK;
}

HRESULT CRenderTarget::Clear()
{
	m_pContext->ClearRenderTargetView(m_pRTV, (_float*)&m_vClearColor);
	return S_OK;
}

HRESULT CRenderTarget::Bind_ShaderResource(CShader * pShader, const _char * pConstantName)
{
	return pShader->Bind_Texture(pConstantName, m_pSRV);
}

HRESULT CRenderTarget::Bind_ShaderResource(CComputeShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_Texture(pConstantName, m_pSRV);
}

HRESULT CRenderTarget::Bind_OutputShaderResource(CComputeShader* pShader, const _char* pConstantName)
{
	if (nullptr == m_pUAV)
		return E_FAIL;

	return pShader->Bind_Texture(pConstantName, m_pUAV);
}

HRESULT CRenderTarget::Copy_Resource(ID3D11Texture2D ** ppTextureHub)
{
	m_pContext->CopyResource(*ppTextureHub, m_pTexture2D);

	return S_OK;
}

HRESULT CRenderTarget::Copy_Resource(ID3D11Texture3D** ppTextureHub)
{
	m_pContext->CopyResource(*ppTextureHub, m_pTexture3D);

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderTarget::Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(	&iNumViewports, &ViewportDesc);

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_WorldMatrix._11 = fSizeX;
	m_WorldMatrix._22 = fSizeY;
	m_WorldMatrix._41 = fX - ViewportDesc.Width * 0.5f;
	m_WorldMatrix._42 = -fY + ViewportDesc.Height * 0.5f;

	return S_OK;
}

HRESULT CRenderTarget::Render_Debug(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	// 1.  texture3d
	if (m_pTexture3D != nullptr) {
		if (FAILED(pShader->Bind_Texture("g_3DTexture", m_pSRV)))
			return E_FAIL;

		if (FAILED(pShader->Begin((_uint)SHADER_PASS_DEFERRED::PASS_DEBUG_3D)))
			return E_FAIL;

		if (FAILED(pVIBuffer->Render()))
			return E_FAIL;
	}
	else {
		D3D11_TEXTURE2D_DESC pDesc = {};
		m_pTexture2D->GetDesc(&pDesc);
		if (pDesc.MiscFlags == D3D11_RESOURCE_MISC_TEXTURECUBE) {
			if (FAILED(pShader->Bind_Texture("g_CubeTexture", m_pSRV)))
				return E_FAIL;

			if (FAILED(pShader->Begin((_uint)SHADER_PASS_DEFERRED::PASS_DEBUG_CUBE)))
				return E_FAIL;

			if (FAILED(pVIBuffer->Render()))
				return E_FAIL;
		}
		// 3. 2d
		else {
			if (FAILED(pShader->Bind_Texture("g_Texture", m_pSRV)))
				return E_FAIL;

			if (FAILED(pShader->Begin((_uint)SHADER_PASS_DEFERRED::PASS_DEBUG)))
				return E_FAIL;

			if (FAILED(pVIBuffer->Render()))
				return E_FAIL;
		}
	}

	//m_pGameInstance->Render_Font(g_strFontTag, m_strRenderTargetTag, _float2(0.f, 0.f), _float4(1.f, 1.f, 1.f, 1.f), 0);
	return S_OK;
}

#endif

CRenderTarget * CRenderTarget::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const wstring& strRenderTargetTag, const _float4 & vClearColor)
{
	CRenderTarget*		pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iSizeX, iSizeY, ePixelFormat, strRenderTargetTag, vClearColor)))
	{
		MSG_BOX(TEXT("Failed to Created : CRenderTarget"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRenderTarget* CRenderTarget::Create_Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSize, _uint iArraySize, DXGI_FORMAT ePixelFormat, const wstring& strRenderTargetTag, const _float4& vClearColor)
{
	CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Cube(iSize, iArraySize, ePixelFormat, strRenderTargetTag, vClearColor)))
	{
		MSG_BOX(TEXT("Failed to Created : CRenderTarget"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRenderTarget* CRenderTarget::Create_3D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight, _uint iDepth, DXGI_FORMAT ePixelFormat, const wstring& strRenderTargetTag, const _float4& vClearColor)
{
	CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Initialize_3D(iWidth, iHeight, iDepth, ePixelFormat, strRenderTargetTag,  vClearColor)))
	{
		MSG_BOX(TEXT("Failed to Created : CRenderTarget"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderTarget::Free()
{
	Safe_Release(m_pSRV);
	Safe_Release(m_pRTV);
	Safe_Release(m_pTexture2D);

	Safe_Release(m_pTexture3D);
	Safe_Release(m_pUAV);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
