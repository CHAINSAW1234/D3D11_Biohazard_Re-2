#include "stdafx.h"
#include "Player_State_Hold_Start.h"
#include "Player_State_Hold.h"
#include "Player.h"

CPlayer_State_Hold_Start::CPlayer_State_Hold_Start(CPlayer* pPlayer, CFSM_HState* pHState)
{
	m_pPlayer = pPlayer;
	m_pHState = pHState;
}

void CPlayer_State_Hold_Start::OnStateEnter()
{
	m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.2f);

	m_pPlayer->Get_Body_Model()->Set_Loop(0, false);
	m_pPlayer->Get_Body_Model()->Set_Loop(1, false);

	
	Set_StartAnimation();
}

void CPlayer_State_Hold_Start::OnStateUpdate(_float fTimeDelta)
{
	if (m_pPlayer->Get_Body_Model()->isFinished(1)) {
		m_pHState->Change_State(CPlayer_State_Hold::IDLE);
	}
}

void CPlayer_State_Hold_Start::OnStateExit()
{
	m_pPlayer->Get_Body_Model()->Set_TrackPosition(0, 0.f);
	m_pPlayer->Get_Body_Model()->Set_TrackPosition(1, 0.f);
	m_pPlayer->Change_Body_Animation_Hold(0, CPlayer::HOLD_IDLE_LOOP);
	m_pPlayer->Change_Body_Animation_Hold(1, CPlayer::HOLD_IDLE_LOOP);
}

void CPlayer_State_Hold_Start::Start()
{
}

void CPlayer_State_Hold_Start::Set_StartAnimation()
{
	_float fDegree = m_pPlayer->Get_CamDegree();

	if (abs(fDegree) <= 90) {
		if (fDegree < 0) {
			m_pPlayer->Change_Body_Animation_Hold(0, CPlayer::HOLD_START_L0);
			m_pPlayer->Change_Body_Animation_Hold(1, CPlayer::HOLD_START_L90);
		}
		else {
			m_pPlayer->Change_Body_Animation_Hold(0, CPlayer::HOLD_START_R0);
			m_pPlayer->Change_Body_Animation_Hold(1, CPlayer::HOLD_START_R90);
		}
		_float fRatio = abs(fDegree) / 90;

		m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f - fRatio);
		m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, fRatio);

	}
	else {
		if (fDegree < 0) {
			m_pPlayer->Change_Body_Animation_Hold(0, CPlayer::HOLD_START_L90);
			m_pPlayer->Change_Body_Animation_Hold(1, CPlayer::HOLD_START_L180);
		}
		else {
			m_pPlayer->Change_Body_Animation_Hold(0, CPlayer::HOLD_START_R90);
			m_pPlayer->Change_Body_Animation_Hold(1, CPlayer::HOLD_START_R180);
		}
		_float fRatio = (abs(fDegree) - 90) / 90;

		m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f - fRatio);
		m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, fRatio);
	}
}

CPlayer_State_Hold_Start* CPlayer_State_Hold_Start::Create(CPlayer* pPlayer, CFSM_HState* pHState)
{
	CPlayer_State_Hold_Start* pInstance = new CPlayer_State_Hold_Start(pPlayer, pHState);

	return pInstance;
}

void CPlayer_State_Hold_Start::Free()
{
	__super::Free();
}
