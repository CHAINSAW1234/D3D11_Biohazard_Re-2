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
	m_pPlayer->Get_Body_Model()->Set_Loop(1, false);
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
	m_pPlayer->Get_Body_Model()->Set_TrackPosition(0, 0);
	m_pPlayer->Get_Body_Model()->Set_TrackPosition(1, 0);
	m_Prev_FRONT_BACK_Direction = DIRECTION_END;
}

void CPlayer_State_Move_Walk::Start()
{
}

void CPlayer_State_Move_Walk::Update_KeyInput()
{
	m_dwPrevDirection = m_dwDirection;
	m_dwDirection = m_pPlayer->Get_Direction();

	if (m_dwDirection & DIRECTION_FRONT)
		m_Prev_FRONT_BACK_Direction = DIRECTION_FRONT;
	else if (m_dwDirection & DIRECTION_BACK)
		m_Prev_FRONT_BACK_Direction = DIRECTION_BACK;
}

void CPlayer_State_Move_Walk::Set_MoveAnimation(_float fTimeDelta)
{
#pragma region 키 입력에 따른 애니메이션 변경
		if (m_dwDirection & DIRECTION_FRONT) {
			//		m_pPlayer->Get_Body_Model()->Change_Animation(0, TEXT("Default"), CPlayer::WALK_F_LOOP);
			m_pPlayer->Get_Body_Model()->Change_Animation(0, TEXT("Test"), 3);
			m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 0.5f, 0.1f);
			m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.5f, 0.1f);

			if (m_dwDirection & DIRECTION_LEFT) {
				m_pPlayer->Get_Body_Model()->Change_Animation(1, TEXT("Default"), CPlayer::WALK_L_LOOP);
			}
			else if (m_dwDirection & DIRECTION_RIGHT) {
				m_pPlayer->Get_Body_Model()->Change_Animation(1, TEXT("Default"), CPlayer::WALK_R_LOOP);
			}
			else {
				m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f, 0.1f);
				m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f, 0.1f);
			}
		}
		else if (m_dwDirection & DIRECTION_BACK) {
			m_pPlayer->Get_Body_Model()->Change_Animation(0, TEXT("Default"), CPlayer::WALK_BACK_B_LOOP);
			m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 0.5f, 0.1f);
			m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.5f, 0.1f);

			if (m_dwDirection & DIRECTION_LEFT) {
				m_pPlayer->Get_Body_Model()->Change_Animation(1, TEXT("Default"), CPlayer::WALK_BACK_L_LOOP);
			}
			else if (m_dwDirection & DIRECTION_RIGHT) {
				m_pPlayer->Get_Body_Model()->Change_Animation(1, TEXT("Default"), CPlayer::WALK_BACK_R_LOOP);
			}
			else {
				m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f, 0.1f);
				m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f, 0.1f);
			}
		}
		else {
			m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f, 0.1f);
			m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f, 0.1f);
			if (m_dwDirection & DIRECTION_LEFT) {
				if (m_Prev_FRONT_BACK_Direction == DIRECTION_FRONT) {
					m_pPlayer->Get_Body_Model()->Change_Animation(0, TEXT("Default"), CPlayer::WALK_L_LOOP);
				}
				else {
					m_pPlayer->Get_Body_Model()->Change_Animation(0, TEXT("Default"), CPlayer::WALK_BACK_L_LOOP);
				}

			}
			else if (m_dwDirection & DIRECTION_RIGHT) {
				if (m_Prev_FRONT_BACK_Direction == DIRECTION_BACK) {
					m_pPlayer->Get_Body_Model()->Change_Animation(0, TEXT("Default"), CPlayer::WALK_BACK_R_LOOP);
				}
				else {
					m_pPlayer->Get_Body_Model()->Change_Animation(0, TEXT("Default"), CPlayer::WALK_R_LOOP);
				}
			}
	}
#pragma endregion

#pragma region 애니메이션 변경시 발 맞추기
	// 애니메이션 변경시 두 애니메이션의 위치를 동일하게 처리
	if (m_dwDirection != m_dwPrevDirection) {

		_float fTrackPosition = m_pPlayer->Get_Body_Model()->Get_TrackPosition(0);
		_float fIntPart;
		_float fFracPart = modf(fTrackPosition, &fIntPart);

		fIntPart = _float(static_cast<_int>(fIntPart) % 64);

		fTrackPosition = fIntPart + fFracPart;

		_float fDuration0 = m_pPlayer->Get_Body_Model()->Get_Duration_From_PlayingInfo(0) + 1;

		if (fDuration0 == 384.f)
			fDuration0 = 64.f;

		_float fDuration1 = m_pPlayer->Get_Body_Model()->Get_Duration_From_PlayingInfo(1) + 1;

		// fDuration0 : fDuration1 = X : fTrackposition
		//m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.2f);
		m_pPlayer->Get_Body_Model()->Reset_PreAnimation(1);
		m_pPlayer->Get_Body_Model()->Set_TrackPosition(1,  fDuration0 * fTrackPosition / fDuration1, true );
		//m_pPlayer->Get_Body_Model()->Active_RootMotion_XZ(true);
	}
#pragma endregion

#pragma region 종료시 초기화
	if (m_pPlayer->Get_Body_Model()->Get_BlendWeight(1) != 0 && m_pPlayer->Get_Body_Model()->isFinished(1)) {
		m_pPlayer->Get_Body_Model()->Set_TrackPosition(0, 0.f);
		m_pPlayer->Get_Body_Model()->Set_TrackPosition(1, 0.f);
		m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.f);
	}
	else {
		m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.2f);
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
