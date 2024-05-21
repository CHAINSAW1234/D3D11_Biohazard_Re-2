#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV() const {
		return m_pRTV;
	}

public:
	HRESULT Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Initialize_Cube(_uint iSize, _uint iArraySize, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Clear();
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);
	HRESULT Copy_Resource(ID3D11Texture2D** ppTextureHub);


#ifdef _DEBUG
public:
	HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_Debug(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

#endif 
private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };
	ID3D11Texture2D*				m_pTexture2D = { nullptr };
	ID3D11RenderTargetView*			m_pRTV = { nullptr };
	ID3D11ShaderResourceView*		m_pSRV = { nullptr };
	_float4							m_vClearColor = {};

#ifdef _DEBUG
private:
	_float4x4						m_WorldMatrix;
#endif

public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	static CRenderTarget* Create_Cube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSize, _uint iArraySize, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	virtual void Free() override;
};

END