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
		if (m_dwDirection & RIGHT)
			m_Prev_FRONT_BACK_Direction = FRONT;
		else if (m_dwDirection & LEFT)
			m_Prev_FRONT_BACK_Direction = BACK;
	}
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
	DWORD dwDirection = 0;

	if (m_pGameInstance->Get_KeyState('W') == PRESSING &&
		m_pGameInstance->Get_KeyState('S') == PRESSING)
		;
	else if (m_pGameInstance->Get_KeyState('W') == PRESSING)
		dwDirection |= FRONT;
	else if (m_pGameInstance->Get_KeyState('S') == PRESSING)
		dwDirection |= BACK;

	if (m_pGameInstance->Get_KeyState('A') == PRESSING &&
		m_pGameInstance->Get_KeyState('D') == PRESSING)
		;
	else if (m_pGameInstance->Get_KeyState('A') == PRESSING)
		dwDirection |= LEFT;
	else if (m_pGameInstance->Get_KeyState('D') == PRESSING)
		dwDirection |= RIGHT;

	m_dwDirection = dwDirection;

	if (m_dwDirection & FRONT)
		m_Prev_FRONT_BACK_Direction = FRONT;
	else if (m_dwDirection & BACK)
		m_Prev_FRONT_BACK_Direction = BACK;
}

void CPlayer_State_Move_Walk::Set_MoveAnimation(_float fTimeDelta)
{
#pragma region 1

	static _float fCurWeight0 = 1.f;
	static _float fCurWeight1 = 0.f;

	static _float fTargetWeight0 = 1.f;
	static _float fTargetWeight1 = 0.f;

	if (m_dwDirection & FRONT) {
		if (m_dwDirection & LEFT) {
			m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::WALK_F_LOOP);
			m_pPlayer->Get_Body_Model()->Set_Loop(0, true);
			fTargetWeight0 = 0.5f;
			//m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 0.5f);

			m_pPlayer->Get_Body_Model()->Change_Animation(1, CPlayer::WALK_L_LOOP);
			m_pPlayer->Get_Body_Model()->Set_Loop(1, true);
			fTargetWeight1 = 0.5f;
			//m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.5f);

		}
		else if (m_dwDirection & RIGHT) {
			m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::WALK_F_LOOP);
			m_pPlayer->Get_Body_Model()->Set_Loop(0, true);
			fTargetWeight0 = 0.5f;
			//m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 0.5f);

			m_pPlayer->Get_Body_Model()->Change_Animation(1, CPlayer::WALK_R_LOOP);
			m_pPlayer->Get_Body_Model()->Set_Loop(1, true);
			fTargetWeight1 = 0.5f;
			//m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.5f);
		}
		else {
			m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::WALK_F_LOOP);
			m_pPlayer->Get_Body_Model()->Set_Loop(0, true);
			fTargetWeight0 = 1.f;
			//m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f);
			fTargetWeight1 = 0.0f;
			//m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f);
		}
	}
	else if (m_dwDirection & BACK) {
		if (m_dwDirection & LEFT) {
			m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::WALK_BACK_B_LOOP);
			m_pPlayer->Get_Body_Model()->Set_Loop(0, true);
			fTargetWeight0 = 0.5f;
			//m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 0.5f);

			m_pPlayer->Get_Body_Model()->Change_Animation(1, CPlayer::WALK_BACK_L_LOOP);
			m_pPlayer->Get_Body_Model()->Set_Loop(1, true);
			fTargetWeight1 = 0.5f;
			//m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.5f);

		}
		else if (m_dwDirection & RIGHT) {
			m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::WALK_BACK_B_LOOP);
			m_pPlayer->Get_Body_Model()->Set_Loop(0, true);
			fTargetWeight0 = 0.5f;
			//m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 0.5f);

			m_pPlayer->Get_Body_Model()->Change_Animation(1, CPlayer::WALK_BACK_R_LOOP);
			m_pPlayer->Get_Body_Model()->Set_Loop(1, true);
			fTargetWeight1 = 0.5f;
			//m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.5f);
		}
		else {
			m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::WALK_BACK_B_LOOP);
			m_pPlayer->Get_Body_Model()->Set_Loop(0, true);
			fTargetWeight0 = 1.f;
			//m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f);
			fTargetWeight1 = 0.f;
			//m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f);
		}
	}
	else {
		m_pPlayer->Get_Body_Model()->Set_Loop(0, true);
		fTargetWeight0 = 1.f;
		//m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f);
		fTargetWeight1 = 0.f;
		//m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f);
		if (m_dwDirection & LEFT) {
			if (m_Prev_FRONT_BACK_Direction == FRONT) {
				m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::WALK_L_LOOP);
			}
			else {
				m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::WALK_BACK_L_LOOP);
			}

		}
		else {
			if (m_Prev_FRONT_BACK_Direction == BACK) {
				m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::WALK_BACK_R_LOOP);
			}
			else {
				m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::WALK_R_LOOP);
			}
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

void CPlayer_State_Move_Walk::Look_Cam(_float fTimeDelta)
{
	// freind 적극 사용에 대해 이후 처리할 것
	_float4 vCamLook = m_pPlayer->m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_LOOK);
	vCamLook.y = 0;
	vCamLook = XMVector3Normalize(vCamLook);

	_float4 vPlayerLook = m_pPlayer->m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK);
	vPlayerLook.y = 0;
	vPlayerLook = XMVector3Normalize(vPlayerLook);
	
	_float fDegree = Cal_Degree_From_Directions_Between_Min180_To_180(vPlayerLook, vCamLook);

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
