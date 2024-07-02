#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CPlayer;
class CPlayer_State_Move_DoorStop final : public CFSM_State
{
private:
	CPlayer_State_Move_DoorStop(CPlayer* pPlayer, CFSM_HState* pHState);
	virtual ~CPlayer_State_Move_DoorStop() = default;

public:
	virtual void				OnStateEnter() override;
	virtual void				OnStateUpdate(_float fTimeDelta) override;
	virtual void				OnStateExit() override;
	virtual void				Start() override;
	virtual void				Change_State(_uint iState);

private:
	void						Set_Door_Lock_Anim();

private:
	CPlayer*					m_pPlayer = { nullptr };
	CFSM_HState*				m_pHState = { nullptr };

public:
	static	CPlayer_State_Move_DoorStop* Create(CPlayer* pPlayer, CFSM_HState* pHState);
	virtual void Free() override;

};

END