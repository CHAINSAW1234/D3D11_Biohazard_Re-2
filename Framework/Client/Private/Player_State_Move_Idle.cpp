#include "stdafx.h"
#include "Player_State_Move_Idle.h"
#include "Player.h"
CPlayer_State_Move_Idle::CPlayer_State_Move_Idle(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
}

void CPlayer_State_Move_Idle::OnStateEnter()
{
	m_pPlayer->Change_Body_Animation_Move(0, CPlayer::ANIM_IDLE);
	m_pPlayer->Get_Body_Model()->Set_Loop(0, true);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f);

	m_pPlayer->Set_TurnSpineDefualt(true);
}

void CPlayer_State_Move_Idle::OnStateUpdate(_float fTimeDelta)
{
	// 1. spotlight인 경우 wheel 돌리게 처리
	//if (m_pPlayer->Get_Spotlight()) {
	//	Look_Cam(fTimeDelta);
	//}

	// 카메라 위치에 맞춰서 대가리 돌리기 : main에서 처리해야함

}

void CPlayer_State_Move_Idle::OnStateExit()
{
	m_pPlayer->Set_TurnSpineDefualt(false);
}

void CPlayer_State_Move_Idle::Start()
{
}

void CPlayer_State_Move_Idle::Look_Cam(_float fTimeDelta)
{
	_float fDegree = m_pPlayer->Get_CamDegree();

	if (abs(fDegree) > 10.f) {
		if (fDegree < 0.f) {
			m_pPlayer->Change_Body_Animation_Move(0, CPlayer::TURN_L180);
		}
		else {
			m_pPlayer->Change_Body_Animation_Move(0, CPlayer::TURN_R180);
		}
	}
	else if (abs(fDegree) < 5.f) {
		m_pPlayer->Change_Body_Animation_Move(0, CPlayer::ANIM_IDLE);
	}

}

CPlayer_State_Move_Idle* CPlayer_State_Move_Idle::Create(CPlayer* pPlayer)
{
	CPlayer_State_Move_Idle* pInstance = new CPlayer_State_Move_Idle(pPlayer);

	return pInstance;
}

void CPlayer_State_Move_Idle::Free()
{
	__super::Free();
}
