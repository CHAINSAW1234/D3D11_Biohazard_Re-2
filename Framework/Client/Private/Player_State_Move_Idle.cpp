#include "stdafx.h"
#include "Player_State_Move_Idle.h"
#include "Player.h"
CPlayer_State_Move_Idle::CPlayer_State_Move_Idle(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
}

void CPlayer_State_Move_Idle::OnStateEnter()
{
	CModel::ANIM_PLAYING_DESC		AnimDesc;
	AnimDesc.iAnimIndex = CPlayer::ANIM_IDLE;
	AnimDesc.isLoop = true;
	AnimDesc.fWeight = 1.f;
	AnimDesc.strBoneLayerTag = TEXT("Default");

	m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 0);
	AnimDesc.fWeight = 0.f;
	m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 1);
	m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 2);
}

void CPlayer_State_Move_Idle::OnStateUpdate(_float fTimeDelta)
{
	// 1. spotlight�� ��� wheel ������ ó��
	if (m_pPlayer->Get_Spotlight()) {
		; // ������
	}

	// ī�޶� ��ġ�� ���缭 �밡�� ������

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
