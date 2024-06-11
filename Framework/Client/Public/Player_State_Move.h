#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"
#include "Player.h"

BEGIN(Client)
class CPlayer_State_Move final : public CFSM_State
{
private:
	CPlayer_State_Move(CPlayer* pPlayer);
	virtual ~CPlayer_State_Move() = default;

public:
	// 상태 진입했을 때 처음만 호출
	virtual void				OnStateEnter() override;
	// 상태 진입되어 있는 상태에서 매 tick마다 호출
	virtual void				OnStateUpdate(_float fTimeDelta) override;
	// 상태가 변경될 때 호출
	virtual void				OnStateExit() override;
	virtual void				Start() override;

private:
	void Update_KeyInput();

private:
	CPlayer*					m_pPlayer = { nullptr };

public:
	static	CPlayer_State_Move* Create(CPlayer* pPlayer);
	virtual void Free() override;

};

END