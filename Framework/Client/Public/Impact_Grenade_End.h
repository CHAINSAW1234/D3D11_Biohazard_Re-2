#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CImpact_Grenade_End final : public CEffect
{
public:
	CImpact_Grenade_End(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CImpact_Grenade_End(const CImpact_Grenade_End& rhs);
	virtual ~CImpact_Grenade_End() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
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
	static CImpact_Grenade_End* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END