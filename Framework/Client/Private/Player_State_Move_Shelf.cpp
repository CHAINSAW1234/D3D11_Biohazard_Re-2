#include "stdafx.h"
#include "Player_State_Move.h"
#include "Player_State_Move_Shelf.h"
#include "Player.h"
#include "MovingShelf.h"
#include "Character_Controller.h"

CPlayer_State_Move_Shelf::CPlayer_State_Move_Shelf(CPlayer* pPlayer, CFSM_HState* pHState)
{
    m_pPlayer = pPlayer;
    m_pHState = pHState;
}

void CPlayer_State_Move_Shelf::OnStateEnter()
{
	//m_pPlayer->Get_Body_Model()->Active_RootMotion_XZ(false);
	//m_pPlayer->Get_Body_Model()->Active_RootMotion_Rotation(false);
	m_pPlayer->Stop_UpperBody();

	m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.f);
	m_pPlayer->Get_Body_Model()->Set_Loop(0, false);
	m_pPlayer->Get_Body_Model()->Set_Loop(1, false);

	m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f);

	m_pPlayer->Get_Body_Model()->Set_TrackPosition(0, 0);
	m_pPlayer->Get_Body_Model()->Set_TrackPosition(1, 0);

	m_pPlayer->Set_ManualMove(true);

	m_eState = START;

	m_eEquip = m_pPlayer->Get_Equip();
	CPlayer::EQUIP eEquip = CPlayer::NONE;
	m_pPlayer->Set_Equip(&eEquip);

	m_pPlayer->Stop_UpperBody();

	Change_Animation(CPlayer::RICKERSHELF_START);

	m_fLerpTimeDelta = 0.f;
	m_PlayerTransform = m_pPlayer->Get_Transform()->Get_WorldFloat4x4();

	m_ShelfTransform = m_pPlayer->Get_Shelf_WorldMatrix();
	m_ShelfTransform = XMMatrixRotationY(XMConvertToRadians(90.f)) * XMLoadFloat4x4(&m_ShelfTransform) ;
	m_ShelfTransform._41 -= -0.413361f * 2;
	m_ShelfTransform._43 -= 0.970557f / 2;
	//_vector vLeverLook = XMVector3Normalize(m_ShelfTransform.Forward());
	//m_ShelfTransform._41 += XMVectorGetX(vLeverLook) * 1.f;
	//m_ShelfTransform._43 += XMVectorGetZ(vLeverLook) * 1.f;
}

void CPlayer_State_Move_Shelf::OnStateUpdate(_float fTimeDelta)
{
	switch (m_eState) {
	case START:
		Start(fTimeDelta);
		break;
	case IDLE:
		Idle();
		break;
	case FINISH:
		Finish();
		break;
	case STOP:
		Stop();
		break;
	}
}

void CPlayer_State_Move_Shelf::OnStateExit()
{
	m_pPlayer->Get_Body_Model()->Active_RootMotion_XZ(true);
	m_pPlayer->Get_Body_Model()->Active_RootMotion_Rotation(true);

	if (CPlayer::NONE != m_eEquip)
		m_pPlayer->Requst_Change_Equip(m_eEquip);

//	m_pPlayer->Set_Shelf_Setting(nullptr);
	m_pPlayer->Set_ManualMove(false);

	m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.2f);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f);
	m_pPlayer->Get_Body_Model()->Set_Loop(0, true);
}

void CPlayer_State_Move_Shelf::Interpolate_Location(_float fTimeDelta)
{
	m_fLerpTimeDelta += fTimeDelta;

	if (m_fLerpTimeDelta >= m_fTotalLerpTime)
		m_fLerpTimeDelta = m_fTotalLerpTime;

	_vector vScale, vRotation, vTranslate;
	XMMatrixDecompose(&vScale, &vRotation, &vTranslate, m_PlayerTransform);

	_vector vTartetScale, vTargetRotation, vTargetTranslate;

	XMMatrixDecompose(&vTartetScale, &vTargetRotation, &vTargetTranslate, m_ShelfTransform);

	// 1. 회전 보간
	m_pPlayer->Get_Transform()->Set_RotationMatrix_Pure(XMMatrixRotationQuaternion(XMQuaternionSlerp(vRotation, vTargetRotation, m_fLerpTimeDelta / m_fTotalLerpTime)));

	// 2. 위치 보간
	_float4 vCurTranslate;
	vCurTranslate = XMVectorLerp(vTranslate, vTargetTranslate, m_fLerpTimeDelta / m_fTotalLerpTime);
	vCurTranslate.y = m_pPlayer->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION).y;

	m_pPlayer->Get_Transform()->Set_State(CTransform::STATE_POSITION, vCurTranslate);
}

void CPlayer_State_Move_Shelf::Start(_float fTimeDelta)
{
	Interpolate_Location(fTimeDelta);

	if (m_pPlayer->Get_Body_Model()->isFinished(0)) {

		m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.f);
		m_eState = IDLE;
		Change_Animation(CPlayer::RICKERSHELF_MOVE);
	}

}

void CPlayer_State_Move_Shelf::Idle()
{
	if (m_pPlayer->Get_Body_Model()->isFinished(0)) {
		m_eState = FINISH;
		Change_Animation(CPlayer::RICKERSHELF_END);
	}

	if (m_pGameInstance->Get_KeyState(VK_LBUTTON) != PRESSING) {
		m_eState = STOP;
		Change_Animation(CPlayer::RICKERSHELF_STOP);
	}
}

void CPlayer_State_Move_Shelf::Finish()
{
	if (m_pPlayer->Get_Body_Model()->isFinished(0)) {
		m_pHState->Change_State(IDLE);
	}
}

void CPlayer_State_Move_Shelf::Stop()
{
	if (m_pPlayer->Get_Body_Model()->isFinished(0)) {
		m_pHState->Change_State(IDLE);
	}
}

void CPlayer_State_Move_Shelf::Change_Animation(_uint iAnimIndex)
{
	m_pPlayer->Set_Shelf_State(iAnimIndex - CPlayer::RICKERSHELF_END);

	if (m_pPlayer->Get_Shelf_Type() == CMovingShelf::SHELF_232_MOVE)
		iAnimIndex += 4;

	m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::Get_AnimSetEtcName(CPlayer::COMMON), iAnimIndex);

}

CPlayer_State_Move_Shelf* CPlayer_State_Move_Shelf::Create(CPlayer* pPlayer, CFSM_HState* pHState)
{
	CPlayer_State_Move_Shelf* pInstance = new CPlayer_State_Move_Shelf(pPlayer, pHState);

	return pInstance;
}

void CPlayer_State_Move_Shelf::Free()
{
	__super::Free();
}
