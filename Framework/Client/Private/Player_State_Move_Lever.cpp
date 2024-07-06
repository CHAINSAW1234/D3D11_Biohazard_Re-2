#include "stdafx.h"
#include "Player_State_Move.h"
#include "Player_State_Move_Lever.h"
#include "Player.h"
#include "Character_Controller.h"

CPlayer_State_Move_Lever::CPlayer_State_Move_Lever(CPlayer* pPlayer, CFSM_HState* pHState)
{
	m_pPlayer = pPlayer;
	m_pHState = pHState;
}

void CPlayer_State_Move_Lever::OnStateEnter()
{
	m_pPlayer->Stop_UpperBody();

	m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.f);
	m_pPlayer->Get_Body_Model()->Set_Loop(0, false);
	m_pPlayer->Get_Body_Model()->Set_Loop(1, false);

	m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f);

	m_pPlayer->Get_Body_Model()->Set_TrackPosition(0, 0);
	m_pPlayer->Get_Body_Model()->Set_TrackPosition(1, 0);

	m_pPlayer->Set_ManualMove(true);

	m_eEquip = m_pPlayer->Get_Equip();
	CPlayer::EQUIP eEquip = CPlayer::NONE;
	m_pPlayer->Set_Equip(&eEquip);

	m_pPlayer->Stop_UpperBody();
	m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::Get_AnimSetEtcName(CPlayer::COMMON),
		CPlayer::LEVER_DOWN);

	m_fLerpTimeDelta = 0.f;
	m_PlayerTransform = m_pPlayer->Get_Transform()->Get_WorldFloat4x4();

	m_LeverTransform = m_pPlayer->Get_Lever_WorldMatrix();
	m_LeverTransform = XMMatrixRotationY(XMConvertToRadians(180.f)) * XMLoadFloat4x4(&m_LeverTransform);

	_vector vLeverLook = XMVector3Normalize(m_LeverTransform.Forward());
	m_LeverTransform._41 += XMVectorGetX(vLeverLook) * 0.5f;
	m_LeverTransform._43 += XMVectorGetZ(vLeverLook) * 0.5f;
}

void CPlayer_State_Move_Lever::OnStateUpdate(_float fTimeDelta)
{
	Interpolate_Location(fTimeDelta);

	if (m_pPlayer->Get_Body_Model()->isFinished(0)) {
		m_pHState->Change_State(CPlayer_State_Move::IDLE);
		m_pPlayer->Set_Lever_Setting(CPlayer::LEVER_BEHAVE_NOTHING);
	}
}

void CPlayer_State_Move_Lever::OnStateExit()
{
	if (CPlayer::NONE != m_eEquip)
		m_pPlayer->Requst_Change_Equip(m_eEquip);

	m_pPlayer->Set_ManualMove(false);

	m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.2f);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f);
	m_pPlayer->Get_Body_Model()->Set_Loop(0, true);
}

void CPlayer_State_Move_Lever::Interpolate_Location(_float fTimeDelta)
{
	m_fLerpTimeDelta += fTimeDelta;

	if (m_fLerpTimeDelta >= m_fTotalLerpTime)
		m_fLerpTimeDelta = m_fTotalLerpTime;

	_vector vScale, vRotation, vTranslate;
	XMMatrixDecompose(&vScale, &vRotation, &vTranslate, m_PlayerTransform);

	_vector vTartetScale, vTargetRotation, vTargetTranslate;

	XMMatrixDecompose(&vTartetScale, &vTargetRotation, &vTargetTranslate, m_LeverTransform);

	// 1. 회전 보간
	m_pPlayer->Get_Transform()->Set_RotationMatrix_Pure(XMMatrixRotationQuaternion(XMQuaternionSlerp(vRotation, vTargetRotation, m_fLerpTimeDelta / m_fTotalLerpTime)));

	// 2. 위치 보간
	_float4 vCurTranslate;
	vCurTranslate = XMVectorLerp(vTranslate, vTargetTranslate, m_fLerpTimeDelta / m_fTotalLerpTime);
	vCurTranslate.y = m_pPlayer->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION).y;

	m_pPlayer->Get_Transform()->Set_State(CTransform::STATE_POSITION, vCurTranslate);
}

CPlayer_State_Move_Lever* CPlayer_State_Move_Lever::Create(CPlayer* pPlayer, CFSM_HState* pHState)
{
	CPlayer_State_Move_Lever* pInstance = new CPlayer_State_Move_Lever(pPlayer, pHState);

	return pInstance;
}

void CPlayer_State_Move_Lever::Free()
{
	__super::Free();
}