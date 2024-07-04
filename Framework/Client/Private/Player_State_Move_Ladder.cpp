#include "stdafx.h"
#include "Player_State_Move.h"
#include "Player_State_Move_Ladder.h"
#include "Player.h"

CPlayer_State_Move_Ladder::CPlayer_State_Move_Ladder(CPlayer* pPlayer, CFSM_HState* pHState)
{
	m_pPlayer = pPlayer;
	m_pHState = pHState;
}

void CPlayer_State_Move_Ladder::OnStateEnter()
{
	m_pPlayer->Stop_UpperBody();

	m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.f);
	m_pPlayer->Get_Body_Model()->Set_Loop(0, false);
	m_pPlayer->Get_Body_Model()->Set_Loop(1, false);
	
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f, 10.f);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f, 10.f);

	m_pPlayer->Get_Body_Model()->Set_TrackPosition(0, 0);
	m_pPlayer->Get_Body_Model()->Set_TrackPosition(1, 0);

	Set_StartAnimation();
	m_eState = START;
	m_fLerpTimeDelta = 0.f;
}

void CPlayer_State_Move_Ladder::OnStateUpdate(_float fTimeDelta)
{
	switch(m_eState) {
	case START:
		Start(fTimeDelta);
		break;
	case IDLE:
		Idle();
		break;
	case FINISH:
		Finish();
		break;
	}
}

void CPlayer_State_Move_Ladder::OnStateExit()
{
	m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.2f);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f);
	m_pPlayer->Get_Body_Model()->Set_Loop(0, true);
}

// 이하 함수 3개는 그 state에서의 행동을 정의
// 행동의 정의 이전에 탈출 조건을 정의

void CPlayer_State_Move_Ladder::Start(_float fTimeDelta)
{
	Interpolate_Location(fTimeDelta);

	if (m_pPlayer->Get_Body_Model()->isFinished(0)) {
		m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.1f);
		m_eState = IDLE;

		switch (m_pPlayer->Get_Body_Model()->Get_CurrentAnimIndex(0)) {
		case CPlayer::LADDER_UP_FINE_START:
			m_iLadderCnt += 1;
			m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::Get_AnimSetEtcName(CPlayer::COMMON),
				CPlayer::LADDER_UP_FINE_R);
			break;
		case CPlayer::LADDER_DOWN_FINE_START:
			m_iLadderCnt -= 1;
			m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::Get_AnimSetEtcName(CPlayer::COMMON),
				CPlayer::LADDER_DOWN_FINE_L);
			break;
		}
	}
}

void CPlayer_State_Move_Ladder::Idle()
{
	if (m_pGameInstance->Get_KeyState('W') == PRESSING || m_pGameInstance->Get_KeyState('D') == PRESSING) {
		m_KeyInput = UP;
	}
	else if (m_pGameInstance->Get_KeyState('S') == PRESSING || m_pGameInstance->Get_KeyState('A') == PRESSING) {
		m_KeyInput = DOWN;
	}

	if (m_pPlayer->Get_Body_Model()->isFinished(0)) {
		if (m_iLadderCnt == 3 && m_KeyInput == DOWN) {
			m_eState = FINISH;
			m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::Get_AnimSetEtcName(CPlayer::COMMON),
				CPlayer::LADDER_DOWN_FINE_R_END);
			return;
		}
		if (m_iLadderCnt == 13 && m_KeyInput == UP) {
			m_eState = FINISH;
			m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::Get_AnimSetEtcName(CPlayer::COMMON),
				CPlayer::LADDER_UP_FINE_R_END);
			return;
		}

		CPlayer::ANIMASTION_COMMON iCurrentAnimIndex = (CPlayer::ANIMASTION_COMMON)m_pPlayer->Get_Body_Model()->Get_CurrentAnimIndex(0);

		switch(iCurrentAnimIndex) {
		case CPlayer::LADDER_UP_FINE_L:
		case CPlayer::LADDER_DOWN_FINE_L:
			if (m_KeyInput == UP) {
				m_iLadderCnt += 1;
				m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::Get_AnimSetEtcName(CPlayer::COMMON),
					CPlayer::LADDER_UP_FINE_R);
			}
			else {
				m_iLadderCnt -= 1;
				m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::Get_AnimSetEtcName(CPlayer::COMMON),
					CPlayer::LADDER_DOWN_FINE_R);
			}
			break;
		case CPlayer::LADDER_UP_FINE_R:
		case CPlayer::LADDER_DOWN_FINE_R:
			if (m_KeyInput == UP) {
				m_iLadderCnt += 1;
				m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::Get_AnimSetEtcName(CPlayer::COMMON),
					CPlayer::LADDER_UP_FINE_L);
			}
			else {
				m_iLadderCnt -= 1;
				m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::Get_AnimSetEtcName(CPlayer::COMMON),
					CPlayer::LADDER_DOWN_FINE_L);
			}
			break;
		}
	}
}

void CPlayer_State_Move_Ladder::Finish()
{
	if (m_pPlayer->Get_Body_Model()->isFinished(0)) {
		m_pHState->Change_State(CPlayer_State_Move::IDLE);
		m_pPlayer->Set_Ladder_Setting(CPlayer::LADDER_BEHAVE_NOTHING);
	}
}

void CPlayer_State_Move_Ladder::Set_StartAnimation()
{
	// 내가 올라가라
	if (m_pPlayer->Get_Ladder_Setting() == CPlayer::LADDER_BEHAVE_UP) {
		m_iLadderCnt = 3;
		m_KeyInput = UP;
		m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::Get_AnimSetEtcName(CPlayer::COMMON), CPlayer::LADDER_UP_FINE_START);
	}
	// LADDER_BEHAVE_DOWN : 내려가라
 	else {
		m_iLadderCnt = 13;
		m_KeyInput = DOWN;
		m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::Get_AnimSetEtcName(CPlayer::COMMON), CPlayer::LADDER_DOWN_FINE_START);
	}
}

void CPlayer_State_Move_Ladder::Interpolate_Location(_float fTimeDelta)
{
	m_fLerpTimeDelta += fTimeDelta;

	_vector vScale, vRotation, vTranslate;
	XMMatrixDecompose(&vScale, &vRotation, &vTranslate, m_pPlayer->Get_Transform()->Get_WorldFloat4x4());

	_vector vTartetScale, vTargetRotation, vTaergetTranslate;

	XMMatrixDecompose(&vTartetScale, &vTargetRotation, &vTaergetTranslate, m_pPlayer->Get_Ladder_WorldMatrix());




	if()
}

CPlayer_State_Move_Ladder* CPlayer_State_Move_Ladder::Create(CPlayer* pPlayer, CFSM_HState* pHState)
{
	CPlayer_State_Move_Ladder* pInstance = new CPlayer_State_Move_Ladder(pPlayer, pHState);

	return pInstance;
}

void CPlayer_State_Move_Ladder::Free()
{
	__super::Free();
}
