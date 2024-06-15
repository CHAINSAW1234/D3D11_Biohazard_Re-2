#include "stdafx.h"
#include "Player_State_Hold_Idle.h"
#include "Player.h"

CPlayer_State_Hold_Idle::CPlayer_State_Hold_Idle(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CPlayer_State_Hold_Idle::OnStateEnter()
{
	m_pPlayer->Get_Body_Model()->Set_TickPerSec(CPlayer::WHEEL_L180, 300.f);
	m_pPlayer->Get_Body_Model()->Set_TickPerSec(CPlayer::WHEEL_R180, 300.f);

	m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f);

	m_pPlayer->Get_Body_Model()->Set_Loop(0, true);
	m_pPlayer->Get_Body_Model()->Set_Loop(1, true);
	m_pPlayer->Get_Body_Model()->Set_Loop(2, true);

	m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(.5f);
}

void CPlayer_State_Hold_Idle::OnStateUpdate(_float fTimeDelta)
{
	m_fDegree = m_pPlayer->Get_CamDegree();

	Set_MoveAnimation(fTimeDelta);
	Look_Cam(fTimeDelta);

	// 카메라 위치에 맞춰서 회전

	// 카메라 위치 에 맞춰서 상체 상하 회전 

}

void CPlayer_State_Hold_Idle::OnStateExit()
{
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(2, 0);
	m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.2f);
}

void CPlayer_State_Hold_Idle::Start()
{
}

void CPlayer_State_Hold_Idle::Set_MoveAnimation(_float fTimeDelta)
{
	DWORD dwDirection = m_pPlayer->Get_Direction();

#pragma region 1

	static _float fCurWeight0 = .5f;
	static _float fCurWeight1 = .5f;

	static _float fTargetWeight0 = .5f;
	static _float fTargetWeight1 = .5f;

	if (dwDirection & DIRECTION_FRONT) {
		m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::STRAFEL_F);
		fTargetWeight0 = 0.5f;
		fTargetWeight1 = 0.5f;

		if (dwDirection & DIRECTION_LEFT) {
			m_pPlayer->Get_Body_Model()->Change_Animation(1, CPlayer::STRAFEL_L);
		}
		else if (dwDirection & DIRECTION_RIGHT) {
			m_pPlayer->Get_Body_Model()->Change_Animation(1, CPlayer::STRAFEL_R);
		}
		else {
			fTargetWeight0 = 1.f;
			fTargetWeight1 = 0.0f;
		}
	}
	else if (dwDirection & DIRECTION_BACK) {
		m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::STRAFEL_B);
		fTargetWeight0 = 0.5f;
		fTargetWeight1 = 0.5f;

		if (dwDirection & DIRECTION_LEFT) {
			m_pPlayer->Get_Body_Model()->Change_Animation(1, CPlayer::STRAFEL_L);
		}
		else if (dwDirection & DIRECTION_RIGHT) {
			m_pPlayer->Get_Body_Model()->Change_Animation(1, CPlayer::STRAFEL_R);
		}
		else {
			fTargetWeight0 = 1.f;
			fTargetWeight1 = 0.f;
		}
	}
	else {
		fTargetWeight0 = 1.f;
		fTargetWeight1 = 0.f;

		if (dwDirection & DIRECTION_LEFT) {
			m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::STRAFEL_L);
		}
		else if (dwDirection & DIRECTION_RIGHT) {
			m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::STRAFEL_R);
		}
		else {
			m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::HOLD_IDLE_LOOP);
		}
	}

	if (abs(fTargetWeight0 - fCurWeight0) > 0.1) {
		if (fCurWeight0 > fTargetWeight0) {
			fCurWeight0 -= fTimeDelta;
		}
		else {
			fCurWeight0 += fTimeDelta;
		}
	}
	else {
		fCurWeight0 = fTargetWeight0;
	}

	if (abs(fTargetWeight1 - fCurWeight1) > 0.1) {
		if (fCurWeight1 > fTargetWeight1) {
			fCurWeight1 -= fTimeDelta;
		}
		else {
			fCurWeight1 += fTimeDelta;
		}
	}
	else {
		fCurWeight1 = fTargetWeight1;
	}

	m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, fCurWeight0);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, fCurWeight1);
#pragma endregion
}


void CPlayer_State_Hold_Idle::Look_Cam(_float fTimeDelta)
{
	static _float fCurWeight = 0.f;
	static _float fTargetWeight = 0.f;

	if (abs(m_fDegree) > 5) {
		fTargetWeight = 1.f;
		m_pPlayer->Get_Body_Model()->Set_Loop(2, true);
		//awm_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0);
		if (m_fDegree < 0) {
			m_pPlayer->Get_Body_Model()->Change_Animation(2, CPlayer::WHEEL_L180);
		}
		else {
			m_pPlayer->Get_Body_Model()->Change_Animation(2, CPlayer::WHEEL_R180);
		}
	}
	else {
		m_pPlayer->Get_Body_Model()->Change_Animation(2, CPlayer::HOLD_IDLE_LOOP);
		fTargetWeight = 0.f;
	}

	if (abs(fTargetWeight - fCurWeight) > 0.1) {
		if (fCurWeight > fTargetWeight) {
			fCurWeight -= fTimeDelta * 5;
		}
		else {
			fCurWeight += fTimeDelta * 5;
		}
	}
	else {
		fCurWeight = fTargetWeight;
	}
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(2, fTargetWeight);

}

CPlayer_State_Hold_Idle* CPlayer_State_Hold_Idle::Create(CPlayer* pPlayer)
{
	CPlayer_State_Hold_Idle* pInstance = new CPlayer_State_Hold_Idle(pPlayer);

	return pInstance;
}

void CPlayer_State_Hold_Idle::Free()
{
	__super::Free();
}
