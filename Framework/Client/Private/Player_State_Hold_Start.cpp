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
	m_fDegree = m_pPlayer->Get_CamDegree();
	Set_StartAnimation();
}

void CPlayer_State_Hold_Start::OnStateUpdate(_float fTimeDelta)
{
	if (m_pPlayer->Get_Body_Model()->isFinished(0)) {
		m_pHState->Change_State(CPlayer_State_Hold::IDLE);
	}

}

void CPlayer_State_Hold_Start::OnStateExit()
{
	m_pPlayer->Get_Body_Model()->Set_Loop(0, true);
	m_pPlayer->Get_Body_Model()->Set_Loop(1, false);
}

void CPlayer_State_Hold_Start::Start()
{
}

void CPlayer_State_Hold_Start::Update_KeyInput()
{
}

void CPlayer_State_Hold_Start::Set_StartAnimation()
{
	m_pPlayer->Get_Body_Model()->Set_Loop(0, false);
	m_pPlayer->Get_Body_Model()->Set_Loop(1, false);

	if (abs(m_fDegree) <= 90) {
		if (m_fDegree < 0) {
			m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::HOLD_START_L0);
			m_pPlayer->Get_Body_Model()->Change_Animation(1, CPlayer::HOLD_START_L90);
		}
		else {
			m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::HOLD_START_R0);
			m_pPlayer->Get_Body_Model()->Change_Animation(1, CPlayer::HOLD_START_R90);
		}
		_float fRatio = abs(m_fDegree) / 90;

		m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f - fRatio);
		m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, fRatio);

	}
	else {
		if (m_fDegree < 0) {
			m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::HOLD_START_L90);
			m_pPlayer->Get_Body_Model()->Change_Animation(1, CPlayer::HOLD_START_L180);
		}
		else {
			m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::HOLD_START_R90);
			m_pPlayer->Get_Body_Model()->Change_Animation(1, CPlayer::HOLD_START_R180);
		}
		_float fRatio = (abs(m_fDegree) - 90) / 90;

		m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f - fRatio);
		m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, fRatio);
	}

	m_pPlayer->Get_Body_Model()->Set_Loop(0, false);
	m_pPlayer->Get_Body_Model()->Set_Loop(1, false);

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
