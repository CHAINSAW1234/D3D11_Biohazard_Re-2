#include "stdafx.h"
#include "Player_State_Move_DoorStop.h"
#include "Player_State_Move.h"
#include "Player.h"

CPlayer_State_Move_DoorStop::CPlayer_State_Move_DoorStop(CPlayer* pPlayer, CFSM_HState* pHState)
{
	m_pPlayer = pPlayer;
	m_pHState = pHState;
}

void CPlayer_State_Move_DoorStop::OnStateEnter()
{
	m_pPlayer->Stop_UpperBody();

	m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.f);
	m_pPlayer->Get_Body_Model()->Set_Loop(0, false);
	m_pPlayer->Get_Body_Model()->Set_Loop(1, false);

	m_pPlayer->Get_Body_Model()->Set_TrackPosition(0, 0);
	m_pPlayer->Get_Body_Model()->Set_TrackPosition(1, 0);

	Set_Door_Lock_Anim();
}

void CPlayer_State_Move_DoorStop::OnStateUpdate(_float fTimeDelta)
{
	if (m_pPlayer->Get_Body_Model()->isFinished(0) || 
		m_pPlayer->Get_Body_Model()->isFinished(1)) {
		m_pHState->Change_State(CPlayer_State_Move::IDLE);
	}
	}

void CPlayer_State_Move_DoorStop::OnStateExit()
{
	m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.2f);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f);
	m_pPlayer->Get_Body_Model()->Set_Loop(0, true);

}

void CPlayer_State_Move_DoorStop::Start()
{
}

void CPlayer_State_Move_DoorStop::Change_State(_uint iState)
{
}

void CPlayer_State_Move_DoorStop::Set_Door_Lock_Anim()
{
	_float fDegree = m_pPlayer->Get_Door_Degree();

	m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::Get_AnimSetEtcName(CPlayer::COMMON), CPlayer::DOOR_LOCK);

	if (fDegree < 0.f) {
		m_pPlayer->Get_Body_Model()->Change_Animation(1, CPlayer::Get_AnimSetEtcName(CPlayer::COMMON), CPlayer::DOOR_LOCK_L);
	}
	else {
		m_pPlayer->Get_Body_Model()->Change_Animation(1, CPlayer::Get_AnimSetEtcName(CPlayer::COMMON), CPlayer::DOOR_LOCK_R);
	}
	_float fRatio = abs(fDegree) / 90.f;

	m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f - fRatio);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, fRatio);

}

CPlayer_State_Move_DoorStop* CPlayer_State_Move_DoorStop::Create(CPlayer* pPlayer, CFSM_HState* pHState)
{
	CPlayer_State_Move_DoorStop* pInstance = new CPlayer_State_Move_DoorStop(pPlayer, pHState);

	return pInstance;
}

void CPlayer_State_Move_DoorStop::Free()
{
	__super::Free();
}
