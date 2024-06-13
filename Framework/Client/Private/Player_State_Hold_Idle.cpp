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



}

void CPlayer_State_Hold_Idle::OnStateUpdate(_float fTimeDelta)
{
	Update_Degree();

	Set_StartAnimation();

	if (m_pPlayer->Get_Body_Model()->isFinished(0)) {
		m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::HOLD_IDLE_LOOP);
		m_pPlayer->Get_Body_Model()->Set_Loop(0, true);
		m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.f);
		m_pPlayer->m_pTransformCom->Turn(_float4(0.f, 1.f, 0.f, 0.f), m_fDegree / 90);
	}

	// 카메라 위치에 맞춰서 회전

	// 카메라 위치 에 맞춰서 상체 상하 회전 


}

void CPlayer_State_Hold_Idle::OnStateExit()
{
	m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.2f);
}

void CPlayer_State_Hold_Idle::Start()
{
}

void CPlayer_State_Hold_Idle::Set_StartAnimation()
{
	if (!(m_pPlayer->Get_Body_Model()->Get_CurrentAnimIndex(0) >= 42 &&
		m_pPlayer->Get_Body_Model()->Get_CurrentAnimIndex(0) <= 59)) {

		m_pPlayer->Get_Body_Model()->Set_Loop(0, false);

		if (abs(m_fDegree) < 75) {
			m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::HOLD_START_L0);
		}
		else if (abs(m_fDegree) < 150) {
			if (m_fDegree < 0) {
				m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::HOLD_START_L90);
			}
			else {
				m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::HOLD_START_R90);
			}
		}
		else {
			if (m_fDegree < 0) {
				m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::HOLD_START_L180);
			}
			else {
				m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::HOLD_START_R180);
			}
		}

		m_pPlayer->Get_Body_Model()->Set_Loop(0, false);
	}

}

void CPlayer_State_Hold_Idle::Update_Degree()
{
	_float4 vCamLook = m_pPlayer->m_pTransformCom_Camera->Get_State_Float4(CTransform::STATE_LOOK);
	vCamLook.y = 0;
	vCamLook = XMVector3Normalize(vCamLook);

	_float4 vPlayerLook = m_pPlayer->m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK);
	vPlayerLook.y = 0;
	vPlayerLook = XMVector3Normalize(vPlayerLook);

	m_fDegree = Cal_Degree_From_Directions_Between_Min180_To_180(vPlayerLook, vCamLook);

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
