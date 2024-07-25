#pragma once

#include "Client_Defines.h"
#include "Actor.h"

BEGIN(Client)

class CActor_WP4530 : public CActor
{
public:
	enum class ACTOR_WP4530_PART { _BODY, _END };

private:
	CActor_WP4530(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CActor_WP4530(const CActor& rhs);
	virtual ~CActor_WP4530() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Tick(_float fTimeDelta) override;
	virtual void			Tick(_float fTimeDelta) override;
	virtual void			Late_Tick(_float fTimeDelta) override;

private:
	HRESULT					Add_Components();
	virtual HRESULT			Add_PartObjects();

public:
	void					Set_LightSpot(_bool isLightSpot) { m_isLightSpot = isLightSpot; }

private:
	void					Render_Light();
	
private:
	_bool					m_isLightSpot = { true };

public:
	static CActor_WP4530* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END