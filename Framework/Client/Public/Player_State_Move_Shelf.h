#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"
#include "Player.h"

BEGIN(Client)
class CPlayer_State_Move_Shelf final : public CFSM_State
{
	enum STATE { START, IDLE, FINISH, STOP, STATE_END };
private:
	CPlayer_State_Move_Shelf(CPlayer* pPlayer, CFSM_HState* pHState);
	virtual ~CPlayer_State_Move_Shelf() = default;

public:
	virtual void				OnStateEnter() override;
	virtual void				OnStateUpdate(_float fTimeDelta) override;
	virtual void				OnStateExit() override;

private:
	void						Interpolate_Location(_float fTimeDelta);

	void						Start(_float fTimeDelta);
	void						Idle();
	void						Finish();
	void						Stop();

	void						Set_InterpolateMatrix();
	void						Change_Animation(_uint iAnimIndex);

private:
	CPlayer* m_pPlayer = { nullptr };
	CFSM_HState* m_pHState = { nullptr };

	CPlayer::EQUIP				m_eEquip = { CPlayer::NONE };
	STATE						m_eState = { STATE_END };

	_float						m_fInterpolateValue = { 0.f };
	_float						m_fTotalLerpTime = { 0.1f };	// 위치, 방향을 선형 보간해야함
	_float						m_fLerpTimeDelta = { 0.f };	// 위치, 방향을 선형 보간해야함

	_float4x4					m_ShelfTransform = {};
	_float4x4					m_PlayerTransform = {};

public:
	static	CPlayer_State_Move_Shelf* Create(CPlayer* pPlayer, CFSM_HState* pHState);
	virtual void Free() override;
};

END