#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Engine)
class CFSM_HState;
END

BEGIN(Client)
class CPlayer;
class CPlayer_State_Hold_Start final : public CFSM_State
{
private:
	CPlayer_State_Hold_Start(CPlayer* pPlayer, CFSM_HState* pHState);
	virtual ~CPlayer_State_Hold_Start() = default;

public:
	virtual void				OnStateEnter() override;
	virtual void				OnStateUpdate(_float fTimeDelta) override;
	virtual void				OnStateExit() override;
	virtual void				Start() override;

private:
	void Update_KeyInput();
	void Set_StartAnimation();

private:
	CPlayer* m_pPlayer = { nullptr };
	CFSM_HState* m_pHState = { nullptr };
	_float m_fDegree = { 0.f };
	
public:
	static	CPlayer_State_Hold_Start* Create(CPlayer* pPlayer, CFSM_HState* pHState);
	virtual void Free() override;

};

END