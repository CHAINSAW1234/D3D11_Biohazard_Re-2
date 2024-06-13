#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CPlayer;
class CPlayer_State_Move_Jog final : public CFSM_State
{
	enum DIRECTION { FRONT = 1, BACK = 2, RIGHT = 4, LEFT = 8, DIRECTION_END };
private:
	CPlayer_State_Move_Jog(CPlayer* pPlayer);
	virtual ~CPlayer_State_Move_Jog() = default;

public:
	virtual void				OnStateEnter() override;
	virtual void				OnStateUpdate(_float fTimeDelta) override;
	virtual void				OnStateExit() override;
	virtual void				Start() override;

private:
	void Update_KeyInput();
	void Set_MoveAnimation(_float fTimeDelta);
	void Look_Cam(_float fTimeDelta);
	void Update_Degree();
private:
	CPlayer* m_pPlayer = { nullptr };

	DWORD m_dwDirection = { 0 };
	_float m_fDegree = { 0.f };

public:
	static	CPlayer_State_Move_Jog* Create(CPlayer* pPlayer);
	virtual void Free() override;

};

END