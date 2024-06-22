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

}

void CPlayer_State_Move_Idle::OnStateUpdate(_float fTimeDelta)
{
	// 1. spotlight인 경우 wheel 돌리게 처리
	if (m_pPlayer->Get_Spotlight()) {
		; // 돌리기
	}

	// 카메라 위치에 맞춰서 대가리 돌리기

}

void CPlayer_State_Move_Idle::OnStateExit()
{
}

void CPlayer_State_Move_Idle::Start()
{
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
