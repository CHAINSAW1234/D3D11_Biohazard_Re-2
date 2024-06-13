#pragma once
#include "Client_Defines.h"
#include "FSM_HState.h"
#include "Player.h"

BEGIN(Client)
class CPlayer_State_Hold final : public CFSM_HState
{
	enum STATE { IDLE, SHOT, STATE_END };
private:	
	CPlayer_State_Hold(CPlayer* pPlayer);
	virtual ~CPlayer_State_Hold() = default;

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
	static	CPlayer_State_Hold* Create(CPlayer* pPlayer);
	virtual void Free() override;

};

END