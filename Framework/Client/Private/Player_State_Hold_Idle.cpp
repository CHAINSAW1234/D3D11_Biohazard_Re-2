#include "stdafx.h"
#include "Player_State_Hold_Idle.h"
#include "Player.h"

CPlayer_State_Hold_Idle::CPlayer_State_Hold_Idle(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CPlayer_State_Hold_Idle::OnStateEnter()
{
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f);

	m_pPlayer->Get_Body_Model()->Set_Loop(0, true);
	m_pPlayer->Get_Body_Model()->Set_Loop(1, true);
	m_pPlayer->Get_Body_Model()->Set_Loop(2, false);

	m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.2f);

	m_isShot = false;
}

void CPlayer_State_Hold_Idle::OnStateUpdate(_float fTimeDelta)
{
	m_fDegree = m_pPlayer->Get_CamDegree();

	Set_MoveAnimation(fTimeDelta);
	Look_Cam(fTimeDelta);
	Shot();

	// 카메라 위치에 맞춰서 회전
	// 카메라 위치 에 맞춰서 상체 상하 회전 

}

void CPlayer_State_Hold_Idle::OnStateExit()
{
	//m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f);
	//m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0);
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
	if (dwDirection & DIRECTION_FRONT) {
		m_pPlayer->Change_Body_Animation_Hold(0, CPlayer::STRAFEL_F);
		m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, .5f, 10.f);
		m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, .5f, 10.f);

		if (dwDirection & DIRECTION_LEFT) {
			m_pPlayer->Change_Body_Animation_Hold(1, CPlayer::STRAFEL_L);
		}
		else if (dwDirection & DIRECTION_RIGHT) {
			m_pPlayer->Change_Body_Animation_Hold(1,CPlayer::STRAFEL_R);
		}
		else {
			m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f, 10.f);
			m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f, 10.f);
		}
	}
	else if (dwDirection & DIRECTION_BACK) {
		m_pPlayer->Change_Body_Animation_Hold(0, CPlayer::STRAFEL_B);
		m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, .5f, 10.f);
		m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, .5f, 10.f);

		if (dwDirection & DIRECTION_LEFT) {
			m_pPlayer->Change_Body_Animation_Hold(1, CPlayer::STRAFEL_L);
		}
		else if (dwDirection & DIRECTION_RIGHT) {
			m_pPlayer->Change_Body_Animation_Hold(1, CPlayer::STRAFEL_R);
		}
		else {
			m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f, 10.f);
			m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f, 10.f);
		}
	}
	else {
		m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f, 0.2f);
		m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f, 0.2f);

		if (dwDirection & DIRECTION_LEFT) {
			m_pPlayer->Change_Body_Animation_Hold(0, CPlayer::STRAFEL_L);
		}
		else if (dwDirection & DIRECTION_RIGHT) {
			m_pPlayer->Change_Body_Animation_Hold(0, CPlayer::STRAFEL_R);
		}
		else {
			m_pPlayer->Change_Body_Animation_Hold(0, CPlayer::HOLD_IDLE_LOOP);
		}
	}
#pragma endregion
}

void CPlayer_State_Hold_Idle::Look_Cam(_float fTimeDelta)
{
	//if (abs(m_fDegree) > 5) {
	//	m_pPlayer->Get_Body_Model()->Set_BlendWeight(2, 2.f, 0.2f);
	//	m_pPlayer->Get_Body_Model()->Set_Loop(2, true);

	//	if (m_fDegree < 0) {
	//		m_pPlayer->Change_Body_Animation_Hold(2, CPlayer::WHEEL_L180);
	//	}
	//	else {
	//		m_pPlayer->Change_Body_Animation_Hold(2, CPlayer::WHEEL_R180);
	//	}
	//}
	//else {
	//	m_pPlayer->Change_Body_Animation_Hold(2, CPlayer::HOLD_IDLE_LOOP);
	//	m_pPlayer->Get_Body_Model()->Set_BlendWeight(2, 0.f, 0.2f);
	//}
	_float fDegree = m_pPlayer->Get_CamDegree();

	if (abs(fDegree) > 5) {
		m_pPlayer->m_pTransformCom->Turn(_float4(0.f, 1.f, 0.f, 0.f), fTimeDelta * /*XMConvertToRadians(180 * fDegree / abs(fDegree))*/  fDegree / 20);
	}
}

void CPlayer_State_Hold_Idle::Shot()
{
	if (m_isShot) {
		if (m_pPlayer->Get_Body_Model()->isFinished(2)) {
			m_pPlayer->Get_Body_Model()->Set_BlendWeight(2, 0, 20.f);
			m_isShot = false;
		}
	}

	if (m_pGameInstance->Get_KeyState(VK_LBUTTON) == DOWN && m_pPlayer->Get_Body_Model()->Is_Loop_PlayingInfo(3)) {
		if (!m_isShot && m_pPlayer->Get_Body_Model()->Get_BlendWeight(2) == 0.f) {
			m_pPlayer->Get_Body_Model()->Set_BlendWeight(2, 1, 20.f);
			if (m_pPlayer->IsShotAble()) {
				// 총알 있으면
				m_pPlayer->Shot();
				m_pPlayer->Change_Body_Animation_Hold(2, CPlayer::HOLD_SHOT);
			}
			else {
				// 총알 없음
				if (m_pPlayer->IsReloadAble()) {
					m_pPlayer->Reload();
					// 인벤토리에 총알 있음
				}
				else {
					// 인벤토리에 총알 없음
					m_pPlayer->Change_Body_Animation_Hold(2, CPlayer::HOLD_SHOT_NO_AMMO);
				}
			}

			m_pPlayer->Get_Body_Model()->Set_TrackPosition(2, 0);
			m_isShot = true;
		}
	}
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
