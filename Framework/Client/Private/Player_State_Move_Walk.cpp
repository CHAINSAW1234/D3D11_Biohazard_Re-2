#include "stdafx.h"
#include "Player_State_Move_Walk.h"
#include "Player.h"

CPlayer_State_Move_Walk::CPlayer_State_Move_Walk(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
}

void CPlayer_State_Move_Walk::OnStateEnter()
{
	Update_KeyInput();

	if (m_Prev_FRONT_BACK_Direction == DIRECTION_END) {
		if (m_dwDirection & DIRECTION_RIGHT)
			m_Prev_FRONT_BACK_Direction = DIRECTION_FRONT;
		else if (m_dwDirection & DIRECTION_LEFT)
			m_Prev_FRONT_BACK_Direction = DIRECTION_BACK;
	}

	m_pPlayer->Get_Body_Model()->Set_Loop(0, true);
	m_pPlayer->Get_Body_Model()->Set_Loop(1, true);

	m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.2f);
}

void CPlayer_State_Move_Walk::OnStateUpdate(_float fTimeDelta)
{
	Update_KeyInput();

	Set_MoveAnimation(fTimeDelta);

	Look_Cam(fTimeDelta);
}

void CPlayer_State_Move_Walk::OnStateExit()
{
	m_Prev_FRONT_BACK_Direction = DIRECTION_END;
}

void CPlayer_State_Move_Walk::Start()
{
}

void CPlayer_State_Move_Walk::Update_KeyInput()
{
	m_dwDirection = m_pPlayer->Get_Direction();

	if (m_dwDirection & DIRECTION_FRONT)
		m_Prev_FRONT_BACK_Direction = DIRECTION_FRONT;
	else if (m_dwDirection & DIRECTION_BACK)
		m_Prev_FRONT_BACK_Direction = DIRECTION_BACK;
}

void CPlayer_State_Move_Walk::Set_MoveAnimation(_float fTimeDelta)
{
#pragma region 1

	if (m_dwDirection & DIRECTION_FRONT) {
		m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::WALK_F_LOOP);
		m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 0.5f);
		m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.5f);

		if (m_dwDirection & DIRECTION_LEFT) {
			m_pPlayer->Get_Body_Model()->Change_Animation(1, CPlayer::WALK_L_LOOP);

		}
		else if (m_dwDirection & DIRECTION_RIGHT) {
			m_pPlayer->Get_Body_Model()->Change_Animation(1, CPlayer::WALK_R_LOOP);

		}
		else {
			m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f);
			m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f);
		}
	}
	else if (m_dwDirection & DIRECTION_BACK) {
		m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::WALK_BACK_B_LOOP);
		m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 0.5f);
		m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.5f);

		if (m_dwDirection & DIRECTION_LEFT) {
			m_pPlayer->Get_Body_Model()->Change_Animation(1, CPlayer::WALK_BACK_L_LOOP);
		}
		else if (m_dwDirection & DIRECTION_RIGHT) {
			m_pPlayer->Get_Body_Model()->Change_Animation(1, CPlayer::WALK_BACK_R_LOOP);
		}
		else {
			m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 0.5f);
			m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.5f);
		}
	}
	else {
		m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f);
		m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f);
		if (m_dwDirection & DIRECTION_LEFT) {
			if (m_Prev_FRONT_BACK_Direction == DIRECTION_FRONT) {
				m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::WALK_L_LOOP);
			}
			else {
				m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::WALK_BACK_L_LOOP);
			}

		}
		else if (m_dwDirection & DIRECTION_RIGHT) {
			if (m_Prev_FRONT_BACK_Direction == DIRECTION_BACK) {
				m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::WALK_BACK_R_LOOP);
			}
			else {
				m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::WALK_R_LOOP);
			}
		}
	}
#pragma endregion
}

void CPlayer_State_Move_Walk::Look_Cam(_float fTimeDelta)
{
	_float fDegree = m_pPlayer->Get_CamDegree();

	if (abs(fDegree) > 5) {
		m_pPlayer->m_pTransformCom->Turn(_float4(0.f, 1.f, 0.f, 0.f), fTimeDelta * fDegree / 10 );
	}
}

CPlayer_State_Move_Walk* CPlayer_State_Move_Walk::Create(CPlayer* pPlayer)
{
	CPlayer_State_Move_Walk* pInstance = new CPlayer_State_Move_Walk(pPlayer);

	return pInstance;
}

void CPlayer_State_Move_Walk::Free()
{
	__super::Free();
}
