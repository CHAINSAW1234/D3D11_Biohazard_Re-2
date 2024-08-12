#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CBGM_Player final : public CBase
{
	DECLARE_SINGLETON(CBGM_Player)

private:
	CBGM_Player();
	virtual ~CBGM_Player() = default;

public:
	void Tick(_float fTimeDelta);
	void Set_Focused(_bool isFocus);

private:
	_bool					m_isFocus = { false };
	_float					m_fAccTime = { 0.f };
	_float					m_fMaxTime = { 2.f };

	_int					m_iLevel = { 0 };
	_int					m_iChannel = { 0 };

public:
	virtual void Free() override;
};

END