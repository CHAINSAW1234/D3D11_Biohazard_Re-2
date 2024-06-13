#include "stdafx.h"
#include "Player_State_Hold_Idle.h"
#include "Player.h"

CPlayer_State_Hold_Idle::CPlayer_State_Hold_Idle(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CPlayer_State_Hold_Idle::OnStateEnter()
{
	m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::ANIM_IDLE);
	m_pPlayer->Get_Body_Model()->Set_Loop(0, true);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f);

	m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f);
}

void CPlayer_State_Hold_Idle::OnStateUpdate(_float fTimeDelta)
{
}

void CPlayer_State_Hold_Idle::OnStateExit()
{
}

void CPlayer_State_Hold_Idle::Start()
{
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
