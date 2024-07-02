#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CPlayer;
class CPlayer_State_SubHold_Start final : public CFSM_State
{
private:
	CPlayer_State_SubHold_Start(CPlayer* pPlayer, CFSM_HState* pHState);
	virtual ~CPlayer_State_SubHold_Start() = default;

public:
	virtual void				OnStateEnter() override;
	virtual void				OnStateUpdate(_float fTimeDelta) override;
	virtual void				OnStateExit() override;
	virtual void				Start() override;

private:
	void						Set_StartAnimation();
	void						Move(_float fTimeDelta);
	void						Turn(_float fTimeDelta);

private:
	CPlayer*					m_pPlayer = { nullptr };
	CFSM_HState*				m_pHState = { nullptr };

public:
	static	CPlayer_State_SubHold_Start* Create(CPlayer* pPlayer, CFSM_HState* pHState);
	virtual void Free() override;
};

END