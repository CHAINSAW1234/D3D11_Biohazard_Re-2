#pragma once

#include "Client_Defines.h"
#include "FSM_HState.h"
#include "Player.h"

BEGIN(Client)
class CPlayer_State_Bite : public CFSM_HState
{
	//
	enum STATE { START, DEFAULT, REJECT, KILL, SPECIAL, STATE_END };
	// start -> default ->reject -> end
	// start -> default ->kill
	// start -> reject
	// start -> kill
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
	CPlayer*					m_pPlayer = { nullptr };
	STATE						m_eState = { STATE_END };

	_bool m_isLight = { false };
	CPlayer::EQUIP m_eEquip = { CPlayer::NONE };

public:
	static	CPlayer_State_Bite* Create(CPlayer* pPlayer);
	virtual void Free() override;

};

END