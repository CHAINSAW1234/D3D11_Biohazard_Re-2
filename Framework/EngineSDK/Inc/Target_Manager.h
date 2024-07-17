#pragma once

#include "Base.h"

BEGIN(Engine)

class CTarget_Manager final : public CBase
{
private:
	CTarget_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTarget_Manager() = default;

public:
	HRESULT													Initialize();
	HRESULT													Add_RenderTarget(const wstring& strRenderTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _bool isTickClear);
	HRESULT													Add_RenderTarget_Cube(const wstring& strRenderTargetTag, _uint iSize, _uint iArraySize, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _bool isTickClear);
	HRESULT													Add_RenderTarget_3D(const wstring& strRenderTargetTag, _uint iWidth, _uint iHeight, _uint iDepth, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _bool isTickClear);
	HRESULT													Clear_RenderTarget_All();
	HRESULT													Clear_RenderTarget(const wstring& strRenderTargetTag);
	HRESULT													Add_MRT(const wstring& strMRTTag, const wstring& strRenderTargetTag);
	HRESULT													Begin_MRT(const wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT													End_MRT();
	HRESULT													Bind_ShaderResource(class CShader* pShader, const wstring& strRenderTargetTag, const _char* pConstantName);
	HRESULT													Bind_ShaderResource(class CComputeShader* pShader, const wstring& strRenderTargetTag, const _char* pConstantName);
	HRESULT													Bind_OutputShaderResource(class CComputeShader* pShader, const wstring& strRenderTargetTag, const _char* pConstantName);
	HRESULT													Copy_Resource(const wstring& strRenderTargetTag, ID3D11Texture2D** ppTextureHub);
	HRESULT													Copy_Resource(const wstring& strDestRenderTargetTag, const wstring& strSrcRenderTargetTag);
	HRESULT													Copy_BackBuffer(const wstring& strRenderTargetTag);
#ifdef _DEBUG
public:
	HRESULT													Ready_Debug(const wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT													Render_Debug(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

private:
	class CGameInstance*									m_pGameInstance = { nullptr };
	ID3D11Device*											m_pDevice = { nullptr };
	ID3D11DeviceContext*									m_pContext = { nullptr };
	map<const wstring, class CRenderTarget*>				m_RenderTargets;
	map<const wstring, list<class CRenderTarget*>>			m_MRTs;

	ID3D11RenderTargetView*									m_pBackBufferRTV = { nullptr };
	ID3D11DepthStencilView*									m_pDSV = { nullptr };

private:
	class CRenderTarget*									Find_RenderTarget(const wstring& strRenderTargetTag);
	list<class CRenderTarget*>* Find_MRT(const wstring& strMRTTag);

public:
	static CTarget_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END