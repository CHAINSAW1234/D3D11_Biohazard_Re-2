#pragma once

#include "Client_Defines.h"
#include "FSM_State.h"
#include "Player.h"

BEGIN(Client)
class CPlayer_State_Bite_kill final : public CFSM_State
{
private:
	CPlayer_State_Bite_kill(CPlayer* pPlayer);
	virtual ~CPlayer_State_Bite_kill() = default;

public:
	virtual void				OnStateEnter() override;
	virtual void				OnStateUpdate(_float fTimeDelta) override;
	virtual void				OnStateExit() override;
	virtual void				Start() override;


private:
	CPlayer* m_pPlayer = { nullptr };

public:
	static	CPlayer_State_Bite_kill* Create(CPlayer* pPlayer);
	virtual void Free() override;

};

END