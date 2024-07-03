#include "stdafx.h"
#include "Player_State_SubHold_Start.h"
#include "Player_State_SubHold.h"
#include "Player.h"

CPlayer_State_SubHold_Start::CPlayer_State_SubHold_Start(CPlayer* pPlayer, CFSM_HState* pHState)
{
	m_pPlayer = pPlayer;
	m_pHState = pHState;
}

void CPlayer_State_SubHold_Start::OnStateEnter()
{
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f);

	m_pPlayer->Get_Body_Model()->Set_Loop(0, true);
	m_pPlayer->Get_Body_Model()->Set_Loop(1, true);

}

void CPlayer_State_SubHold_Start::OnStateUpdate(_float fTimeDelta)
{
	if (!m_pPlayer->m_isRequestChangeEquip && m_pPlayer->Get_Body_Model()->Is_Loop_PlayingInfo(3)) {
		m_pHState->Change_State(CPlayer_State_SubHold::IDLE);
		return;
	}
	else {
		Move(fTimeDelta);
	}
}

void CPlayer_State_SubHold_Start::OnStateExit()
{
	m_pPlayer->Get_Body_Model()->Set_TrackPosition(0, 0.f);
	m_pPlayer->Get_Body_Model()->Set_TrackPosition(1, 0.f);
	m_pPlayer->Change_Body_Animation_Hold(0, CPlayer::HOLD_IDLE_LOOP);
	m_pPlayer->Change_Body_Animation_Hold(1, CPlayer::HOLD_IDLE_LOOP);
}

void CPlayer_State_SubHold_Start::Start()
{
}

void CPlayer_State_SubHold_Start::Move(_float fTimeDelta)
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
			m_pPlayer->Change_Body_Animation_Hold(1, CPlayer::STRAFEL_R);
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
			return;
		}
	}
	Turn(fTimeDelta);
#pragma endregion
}

void CPlayer_State_SubHold_Start::Turn(_float fTimeDelta)
{
	_float fDegree = m_pPlayer->Get_CamDegree();

	if (abs(fDegree) > 5) {
		m_pPlayer->Get_Transform()->Turn(_float4(0.f, 1.f, 0.f, 0.f), fTimeDelta * /*XMConvertToRadians(180 * fDegree / abs(fDegree))*/  fDegree / 20);
	}
}

CPlayer_State_SubHold_Start* CPlayer_State_SubHold_Start::Create(CPlayer* pPlayer, CFSM_HState* pHState)
{
	CPlayer_State_SubHold_Start* pInstance = new CPlayer_State_SubHold_Start(pPlayer, pHState);

	return pInstance;
}

void CPlayer_State_SubHold_Start::Free()
{
	__super::Free();
}
