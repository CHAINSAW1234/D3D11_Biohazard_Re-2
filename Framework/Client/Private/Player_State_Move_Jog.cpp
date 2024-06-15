#include "stdafx.h"
#include "Player_State_Move_Jog.h"
#include "Player.h"
#include "Camera_Free.h"

CPlayer_State_Move_Jog::CPlayer_State_Move_Jog(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
}

void CPlayer_State_Move_Jog::OnStateEnter()
{
	m_pPlayer->Get_Body_Model()->Set_Loop(0, false);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f);
	m_dwDirection = 0;
	m_fDegree = 0.f;
}

void CPlayer_State_Move_Jog::OnStateUpdate(_float fTimeDelta)
{
	CPlayer::ANIMATION_MOVE Animation = (CPlayer::ANIMATION_MOVE)m_pPlayer->Get_Body_Model()->Get_CurrentAnimIndex(0);
	_bool isFinish = m_pPlayer->Get_Body_Model()->isFinished(0);
	if (isFinish) {
		int a = 1;
	}

	Update_KeyInput();
	Update_Degree();
	Set_MoveAnimation(fTimeDelta);
	Look_Cam(fTimeDelta);
}

void CPlayer_State_Move_Jog::OnStateExit()
{
	m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.2f);
}

void CPlayer_State_Move_Jog::Start()
{
}

void CPlayer_State_Move_Jog::Update_KeyInput()
{
	DWORD dwDirection = 0;

	if (m_pGameInstance->Get_KeyState('W') != NONE &&
		m_pGameInstance->Get_KeyState('S') != NONE) {
		;
	}
	else if (m_pGameInstance->Get_KeyState('W') != NONE) {
		dwDirection |= FRONT;
	}
	else if (m_pGameInstance->Get_KeyState('S') != NONE) {
		dwDirection |= BACK;
	}

	if (m_pGameInstance->Get_KeyState('A') != NONE &&
		m_pGameInstance->Get_KeyState('D') != NONE) {
		;
	}
	else if (m_pGameInstance->Get_KeyState('A') != NONE) {
		dwDirection |= LEFT;
	}
	else if (m_pGameInstance->Get_KeyState('D') != NONE) {
		dwDirection |= RIGHT;
	}

	m_dwDirection = dwDirection;
}

void CPlayer_State_Move_Jog::Set_MoveAnimation(_float fTimeDelta)
{

#pragma region Start
	if (!(m_pPlayer->Get_Body_Model()->Get_CurrentAnimIndex(0) >= 15 &&
		m_pPlayer->Get_Body_Model()->Get_CurrentAnimIndex(0) <= 26) /*||
		(m_pPlayer->Get_Body_Model()->Get_CurrentAnimIndex(0) == CPlayer::JOG_STRAIGHT_LOOP && abs(m_fDegree) > 80) */) {

		m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.f);
		m_pPlayer->Get_Body_Model()->Set_Loop(0, false);
		m_pPlayer->Get_Body_Model()->Set_Loop(1, false);

		if (abs(m_fDegree) <= 90) {
			if (m_fDegree < 0) {
				m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::JOG_START_L0);
				m_pPlayer->Get_Body_Model()->Change_Animation(1, CPlayer::JOG_START_L90);
			}
			else {
				m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::JOG_START_R0);
				m_pPlayer->Get_Body_Model()->Change_Animation(1, CPlayer::JOG_START_R90);
			}
			_float fRatio = abs(m_fDegree) / 90;

			m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f - fRatio);
			m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, fRatio);

		}
		else {
			if (m_fDegree < 0) {
				m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::JOG_START_L90);
				m_pPlayer->Get_Body_Model()->Change_Animation(1, CPlayer::JOG_START_L180);
			}
			else {
				m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::JOG_START_R90);
				m_pPlayer->Get_Body_Model()->Change_Animation(1, CPlayer::JOG_START_R180);
			}
			_float fRatio = (abs(m_fDegree)- 90) / 90;

			m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f - fRatio);
			m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, fRatio);
		}
	}
		//	if (abs(m_fDegree) < 75) {

		//		m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::JOG_START_L0_LR);
		//	}
		//	else if (abs(m_fDegree) < 150) {
		//		if (m_fDegree < 0) {
		//			m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::JOG_START_L90_LR);
		//		}
		//		else {
		//			m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::JOG_START_R90_RL);
		//		}
		//	}
		//	else {
		//		if (m_fDegree < 0) {
		//			m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::JOG_START_L180_LR);
		//		}
		//		else {
		//			m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::JOG_START_R180_RL);
		//		}
		//	}

		//	m_pPlayer->Get_Body_Model()->Set_Loop(0, false); 
		//}

#pragma endregion

#pragma region LOOP

	if (m_pPlayer->Get_Body_Model()->isFinished(0)) {

		//m_pPlayer->m_pTransformComssssssss->Turn(_float4(0.f, 1.f, 0.f, 0.f), m_fDegree/90);
		m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::JOG_STRAIGHT_LOOP);
		m_pPlayer->Get_Body_Model()->Set_Loop(0, true);
		m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.1f);

		m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1);
		m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0);
	}

#pragma endregion
	
}

void CPlayer_State_Move_Jog::Look_Cam(_float fTimeDelta)
{
	_float fDegree = m_fDegree;

	switch (m_pPlayer->Get_Body_Model()->Get_CurrentAnimIndex(0)) {
	case CPlayer::JOG_START_L0:
	case CPlayer::JOG_START_R0:
	case CPlayer::JOG_STRAIGHT_LOOP:
		break;
	case CPlayer::JOG_START_L90:
		fDegree += 90;
		break;
	case CPlayer::JOG_START_L180:
		fDegree += 180;
		break;
	case CPlayer::JOG_START_R90:
		fDegree -= 90;
		break;
	case CPlayer::JOG_START_R180:
		fDegree -= 180;
		break;
	}

	if (/*m_pPlayer->Get_Body_Model()->Get_CurrentAnimIndex(0) == CPlayer::JOG_STRAIGHT_LOOP &&*/ abs(m_fDegree) > 2) {
		m_pPlayer->m_pTransformCom->Turn(_float4(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_fDegree / 30);
	}

	//if (!(m_pPlayer->Get_Body_Model()->Get_CurrentAnimIndex(0) >= 15 &&
	//	m_pPlayer->Get_Body_Model()->Get_CurrentAnimIndex(0) <= 20)) {
	//	
	//}


	//if (fDegree > 5) {
	//	m_pPlayer->Get_Body_Model()->Change_Animation(2, CPlayer::TURN_L180);
	//	m_pPlayer->Get_Body_Model()->Set_BlendWeight(2, 0.3f);
	//}
	//else if (fDegree < -5) {
	//	m_pPlayer->Get_Body_Model()->Change_Animation(2, CPlayer::TURN_R180);
	//	m_pPlayer->Get_Body_Model()->Set_BlendWeight(2, 0.3f);
	//}
	//else {
	//	m_pPlayer->Get_Body_Model()->Set_BlendWeight(2, 0);
	//}

/*
	135  -> 90 ~180 ºñÀ²
	135 - 90  45
	120 - 90 30 /90 * 100  = t    -> 1-t : t
	45 / 90 * 100 = 50 : 0.5 0.5 ºñÀ²·Î ¼¯À¸¸é µÊ

*/

}

void CPlayer_State_Move_Jog::Update_Degree()
{
	_float4 vCamLook = m_pPlayer->m_pCamera->Get_Transform()->Get_State_Float4(CTransform::STATE_LOOK);
	vCamLook.y = 0;
	vCamLook = XMVectorSetW(XMVector3Normalize(vCamLook), 0.f);

	_float4 vCamRight = m_pPlayer->m_pCamera->Get_Transform()->Get_State_Float4(CTransform::STATE_RIGHT);
	vCamRight.y = 0;
	vCamRight = XMVectorSetW(XMVector3Normalize(vCamRight), 0.f);

	_float4 vPlayerLook = m_pPlayer->m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK);
	vPlayerLook.y = 0;
	vPlayerLook = XMVectorSetW(XMVector3Normalize(vPlayerLook), 0.f);

	_float4 vCamDir = { 0.f,0.f,0.f,0.f };

	if (m_dwDirection & FRONT)
		vCamDir += vCamLook;
	if (m_dwDirection & BACK)
		vCamDir -= vCamLook;
	if (m_dwDirection & RIGHT)
		vCamDir += vCamRight;
	if (m_dwDirection & LEFT)
		vCamDir -= vCamRight;


	vCamDir.y = 0.f;
	vCamDir = XMVectorSetW(XMVector3Normalize(vCamDir), 0);
	m_fDegree = Cal_Degree_From_Directions_Between_Min180_To_180(vPlayerLook, vCamDir);

}

CPlayer_State_Move_Jog* CPlayer_State_Move_Jog::Create(CPlayer* pPlayer)
{
	CPlayer_State_Move_Jog* pInstance = new CPlayer_State_Move_Jog(pPlayer);

	return pInstance;
}

void CPlayer_State_Move_Jog::Free()
{
	__super::Free();
}
