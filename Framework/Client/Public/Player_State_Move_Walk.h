#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CPlayer;
class CPlayer_State_Move_Walk final : public CFSM_State
{
private:
	CPlayer_State_Move_Walk(CPlayer* pPlayer, CFSM_HState* pHState);
	virtual ~CPlayer_State_Move_Walk() = default;

public:
	virtual void				OnStateEnter() override;
	virtual void				OnStateUpdate(_float fTimeDelta) override;
	virtual void				OnStateExit() override;
	virtual void				Start() override;

private:
	void Update_KeyInput();
	void Set_MoveAnimation(_float fTimeDelta);
	void Look_Cam(_float fTimeDelta);
	void Open_Door();


private:
	CPlayer* m_pPlayer = { nullptr };
	CFSM_HState* m_pHState = { nullptr };
	PLAYER_DIREECTION m_Prev_FRONT_BACK_Direction = { DIRECTION_END };
	DWORD m_dwDirection = { 0 };
	DWORD m_dwPrevDirection = { 0 };

public:
	static	CPlayer_State_Move_Walk* Create(CPlayer* pPlayer, CFSM_HState* pHState);
	virtual void Free() override;

};

END