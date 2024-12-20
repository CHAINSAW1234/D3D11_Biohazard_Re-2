#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CPlayer;
class CPlayer_State_Move_Idle final : public CFSM_State
{
private:
	CPlayer_State_Move_Idle(CPlayer* pPlayer);
	virtual ~CPlayer_State_Move_Idle() = default;

public:
	virtual void				OnStateEnter() override;
	virtual void				OnStateUpdate(_float fTimeDelta) override;
	virtual void				OnStateExit() override;
	virtual void				Start() override;

private:
	void Look_Cam(_float fTimeDelta);

private:
	CPlayer* m_pPlayer = { nullptr };

public:
	static	CPlayer_State_Move_Idle* Create(CPlayer* pPlayer);
	virtual void Free() override;

};

END