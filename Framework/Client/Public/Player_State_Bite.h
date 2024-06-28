#pragma once

#include "Client_Defines.h"
#include "FSM_HState.h"

BEGIN(Client)
class CPlayer;

class CPlayer_State_Bite : public CFSM_HState
{

	enum STATE { IDLE, WALK, JOG, STAIR, STATE_END };
private:
	CPlayer_State_Bite(CPlayer* pPlayer);
	virtual ~CPlayer_State_Bite() = default;

public:
	virtual void				OnStateEnter() override;
	virtual void				OnStateUpdate(_float fTimeDelta) override;
	virtual void				OnStateExit() override;
	virtual void				Start() override;
	virtual void				Change_State(_uint iState);

private:
	void						Update_State();

private:
	HRESULT						Add_States();

private:
	CPlayer* m_pPlayer = { nullptr };
	STATE						m_eState = { IDLE };

public:
	static	CPlayer_State_Move* Create(CPlayer* pPlayer);
	virtual void Free() override;

};

END