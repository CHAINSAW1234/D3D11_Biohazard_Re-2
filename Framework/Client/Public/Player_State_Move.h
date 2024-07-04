#pragma once
#include "Client_Defines.h"
#include "FSM_HState.h"

BEGIN(Client)
class CPlayer;
class CPlayer_State_Move final : public CFSM_HState
{
public:
	enum STATE { IDLE, WALK, JOG, STAIR, DOOR_STOP, LADDER, STATE_END };

private:
	CPlayer_State_Move(CPlayer* pPlayer);
	virtual ~CPlayer_State_Move() = default;

public:
	virtual void				OnStateEnter() override;
	virtual void				OnStateUpdate(_float fTimeDelta) override;
	virtual void				OnStateExit() override;
	virtual void				Start() override;
	virtual void				Change_State(_uint iState);

private:
	void						Update_State();
	void						Open_Door();

private:
	HRESULT						Add_States();

private:
	CPlayer*					m_pPlayer = { nullptr };
	STATE						m_eState = { IDLE };

public:
	static	CPlayer_State_Move* Create(CPlayer* pPlayer);
	virtual void Free() override;

};

END