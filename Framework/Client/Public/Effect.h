#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
class CModel;
END

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
	virtual void			Compute_CurrentUV();
	void					SetPosition(_float4 Pos);
	virtual void			Setup_Billboard() {}
	void					SetType(_uint iType)
	{
		m_iType = iType;
	}
protected:
	_float					m_fSizeX = { 0.f };
	_float					m_fSizeY = { 0.f };
	_float					m_fSizeZ = { 0.f };

protected:
	CShader*				m_pShaderCom = { nullptr };
	CTexture*				m_pTextureCom = { nullptr };
	CVIBuffer_Rect*			m_pVIBufferCom = { nullptr };

protected:
	_float					m_fFrame = { 0.f };
	_uint					m_iFrame = { 0 };
	_uint					m_iFrame_Total = { 0 };
	IMG_SIZE				m_ImgSize;
	pair<_uint, _uint>		m_DivideCount;
	_uint					m_iFrameX = { 0 };
	_uint					m_iFrameY = { 0 };
	_float					m_fMinUV_X = { 0.f };
	_float					m_fMaxUV_X = { 0.f };
	_float					m_fMinUV_Y = { 0.f };
	_float					m_fMaxUV_Y = { 0.f };

	_uint					m_iType = { 0 };
	ULONGLONG				m_FrameTime = { 0 };
	ULONGLONG				m_FrameDelay = { 0 };
	_float					m_fAlpha = { 1.f };
	_float					m_fAlpha_Delta = { 0.f };
	_float					m_fAlpha_Delta_Sum = { 0.f };
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END