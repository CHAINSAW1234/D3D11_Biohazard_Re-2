#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CGrenade_Explosion final : public CEffect
{
public:
	CGrenade_Explosion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGrenade_Explosion(const CGrenade_Explosion& rhs);
	virtual ~CGrenade_Explosion() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void			Set_Render(_bool boolean) override
	{
		m_bRender = boolean;
		m_FrameTime = GetTickCount64();
		m_iFrame = 0;
		m_fAlpha_Delta_Sum = 0.f;
	}
public:
	virtual void			Compute_CurrentUV();
	virtual void			Setup_Billboard() override;
public:
	void					SetSize(_float fSizeX, _float fSizeY);
	virtual void			SetPosition(_float4 Pos) override;

private:
	virtual HRESULT			Add_Components();
	virtual HRESULT			Bind_ShaderResources();

private:
	_float					m_fDefaultSize_X = { 0.f };
	_float					m_fDefaultSize_Y = { 0.f };
public:
	static CGrenade_Explosion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END