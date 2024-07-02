#include "stdafx.h"
#include "Player_State_SubHold_Idle.h"
#include "Player.h"

CPlayer_State_SubHold_Idle::CPlayer_State_SubHold_Idle(CPlayer* pPlayer)
{
    m_pPlayer = pPlayer;
}

void CPlayer_State_SubHold_Idle::OnStateEnter()
{
	m_pPlayer->Set_TurnSpineHold(true);
	m_pPlayer->Set_TurnSpineLight(false);

	m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f);

	m_pPlayer->Get_Body_Model()->Set_Loop(0, true);
	m_pPlayer->Get_Body_Model()->Set_Loop(1, true);
	m_pPlayer->Get_Body_Model()->Set_Loop(2, false);

	m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.2f);

	m_isShot = false;
}

void CPlayer_State_SubHold_Idle::OnStateUpdate(_float fTimeDelta)
{
	m_fDegree = m_pPlayer->Get_CamDegree();

	if (m_isShot) {
		if(m_pPlayer->Get_Body_Model()->isFinished(3) && 
			((m_pPlayer->Get_Body_Model()->Get_CurrentAnimIndex(3) == CPlayer::HOLD_SHOT) ||
			m_pPlayer->Get_Body_Model()->Get_CurrentAnimIndex(3) == CPlayer::HOLD_SHOT_NO_AMMO)) {
			m_pPlayer->Get_Body_Model()->Set_Loop(3, true);
			m_pPlayer->Get_Body_Model()->Set_BlendWeight(3, 0.f, 5.f);
			m_pPlayer->Change_State(CPlayer::MOVE);
			return;
		}
	}
	Set_MoveAnimation(fTimeDelta);
	Look_Cam(fTimeDelta);
	Shot();

}

void CPlayer_State_SubHold_Idle::OnStateExit()
{
	m_pPlayer->Set_TurnSpineHold(false);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0);
	m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.2f);
}

void CPlayer_State_SubHold_Idle::Start()
{
}

void CPlayer_State_SubHold_Idle::Set_MoveAnimation(_float fTimeDelta)
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
		}
	}
#pragma endregion
}

void CPlayer_State_SubHold_Idle::Look_Cam(_float fTimeDelta)
{
	_float fDegree = m_pPlayer->Get_CamDegree();

	if (abs(fDegree) > 5) {
		m_pPlayer->Get_Transform()->Turn(_float4(0.f, 1.f, 0.f, 0.f), fTimeDelta * /*XMConvertToRadians(180 * fDegree / abs(fDegree))*/  fDegree / 20);
	}
}

void CPlayer_State_SubHold_Idle::Shot()
{
	if (m_pGameInstance->Get_KeyState(VK_LBUTTON) == DOWN) {
		m_pPlayer->Get_Body_Model()->Set_Loop(0, false);
		m_pPlayer->Get_Body_Model()->Set_Loop(1, false);

		m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f, 10.f);


		_vector				vMyLook = { m_pPlayer->Get_Transform()->Get_State_Vector(CTransform::STATE_LOOK) };
		_float4				vHeadWorldLook = m_pPlayer->Get_Transform()->Get_State_Float4(CTransform::STATE_LOOK);
		_float				fHeadMagnitude = sqrt(vHeadWorldLook.x * vHeadWorldLook.x + vHeadWorldLook.y * vHeadWorldLook.y + vHeadWorldLook.z * vHeadWorldLook.z);
		_float				fHeadWorldAngle = acosf(vHeadWorldLook.y / fHeadMagnitude);

		_matrix				CamWorldMatrix = { m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW) };
		_float4				vCamLook = { CamWorldMatrix.r[CTransform::STATE_LOOK] };
		_float				fCamMagnitude = sqrt(vCamLook.x * vCamLook.x + vCamLook.y * vCamLook.y + vCamLook.z * vCamLook.z);
		_float				fCamAngle = acosf(vCamLook.y / fCamMagnitude);

		_vector				vRotateAxis = { m_pPlayer->Get_Transform()->Get_State_Vector(CTransform::STATE_RIGHT) };
		_float				fAngle = fabs(fHeadWorldAngle - fCamAngle);

		if (vHeadWorldLook.y < vCamLook.y) {
			fAngle *= -1;
		}

		// 현재 상체의 꺾임 정도에 따라 다른 애니메이션 돌리기
		if (fAngle >= 0) {
			m_pPlayer->Change_Body_Animation_Hold(0, CPlayer::HOLD_SHOT);
		}
		else {
			m_pPlayer->Change_Body_Animation_Hold(0, CPlayer::HOLD_SHOT_NO_AMMO);
		}

		m_isShot = true;
	}
}

CPlayer_State_SubHold_Idle* CPlayer_State_SubHold_Idle::Create(CPlayer* pPlayer)
{
	CPlayer_State_SubHold_Idle* pInstance = new CPlayer_State_SubHold_Idle(pPlayer);

	return pInstance;
}

void CPlayer_State_SubHold_Idle::Free()
{
	__super::Free();
}
