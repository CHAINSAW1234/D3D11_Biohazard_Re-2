#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CPlayer;
class CPlayer_State_Hold_Idle final : public CFSM_State
{
private:
	CPlayer_State_Hold_Idle(CPlayer* pPlayer);
	virtual ~CPlayer_State_Hold_Idle() = default;

public:
	virtual void				OnStateEnter() override;
	virtual void				OnStateUpdate(_float fTimeDelta) override;
	virtual void				OnStateExit() override;
	virtual void				Start() override;

private:

	void Set_MoveAnimation(_float fTimeDelta);
	void Look_Cam(_float fTimeDelta);

private:
	CPlayer*	m_pPlayer = { nullptr };
	_float		m_fDegree = { 0.f };

public:
	static	CPlayer_State_Hold_Idle* Create(CPlayer* pPlayer);
	virtual void Free() override;

};

END
