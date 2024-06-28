#pragma once

#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Client)

class CMuzzle_Flash final : public CEffect
{
protected:
	CMuzzle_Flash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMuzzle_Flash(const CMuzzle_Flash& rhs);
	virtual ~CMuzzle_Flash() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

private:
	_float					m_fFrame = { 0.f };
private:
	virtual HRESULT			Add_Components();
	virtual HRESULT			Bind_ShaderResources();

public:
	static CMuzzle_Flash* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END