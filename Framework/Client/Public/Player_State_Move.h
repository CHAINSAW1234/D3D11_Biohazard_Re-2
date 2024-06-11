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
	// ���� �������� �� ó���� ȣ��
	virtual void				OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void				OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
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