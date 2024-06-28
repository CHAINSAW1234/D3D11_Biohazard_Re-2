#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Client)

class CEffect : public CBlendObject
{
protected:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* pArg);
	virtual void			Tick(_float fTimeDelta);
	virtual void			Late_Tick(_float fTimeDelta);
	virtual HRESULT			Render();

public:
	void					Compute_CurrentUV();

protected:
	CShader*				m_pShaderCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };

protected:
	_float					m_fFrame = { 0.f };
	_uint					m_iFrame = { 0.f };
	IMG_SIZE				m_ImgSize;
	pair<_uint, _uint>		m_DivideCount;
	_uint					m_iFrameX = { 0 };
	_uint					m_iFrameY = { 0 };
	_float					m_fMinUV_X = { 0.f };
	_float					m_fMaxUV_X = { 0.f };
	_float					m_fMinUV_Y = { 0.f };
	_float					m_fMaxUV_Y = { 0.f };
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END