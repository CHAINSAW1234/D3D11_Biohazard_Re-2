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

	m_pPlayer->Set_ManualMove(true);

	m_eEquip = m_pPlayer->Get_Equip();
	CPlayer::EQUIP eEquip = CPlayer::NONE;
	m_pPlayer->Set_Equip(&eEquip);

	m_pPlayer->Stop_UpperBody();
	m_pPlayer->Get_Body_Model()->Change_Animation(0, CPlayer::Get_AnimSetEtcName(CPlayer::COMMON),
		CPlayer::LEVER_DOWN);

	m_fLerpTimeDelta = 0.f;
	m_PlayerTransform = m_pPlayer->Get_Transform()->Get_WorldFloat4x4();
	Set_InterpolateMatrix();
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
	if (m_fLerpTimeDelta >= 0.2f)
		return;

	_float				fTime = fTimeDelta;
	m_fLerpTimeDelta += fTime;
	if (m_fLerpTimeDelta >= 0.2f)
	{
		fTime -= m_fLerpTimeDelta - 0.2f;
	}

	_float				fRatio = { fTime / 0.2f };

	_matrix				InterpolationMatrix = { XMLoadFloat4x4(&m_LeverTransform) };

	_vector				vScale, vQuaternion, vTranslation;
	XMMatrixDecompose(&vScale, &vQuaternion, &vTranslation, InterpolationMatrix);

	_vector				vDevideQuaternion = { XMQuaternionSlerp(XMQuaternionIdentity(), XMQuaternionNormalize(vQuaternion), fRatio) };
	_vector				vDevideTranslation = { XMVectorSetW(vTranslation * fRatio, 0.f) };


	_matrix				WorldMatrix = { m_pPlayer->Get_Transform()->Get_WorldMatrix() };
	_vector				vWorldScale, vWorldQuaternion, vWorldTranslation;
	XMMatrixDecompose(&vWorldScale, &vWorldQuaternion, &vWorldTranslation, WorldMatrix);

	_vector				vResultQuaternion = { XMQuaternionMultiply(XMQuaternionNormalize(vWorldQuaternion), XMQuaternionNormalize(vDevideQuaternion)) };
	_vector				vResultTranslation = { XMVectorSetW(vWorldTranslation + vDevideTranslation, 1.f) };

	_matrix				AplliedMatrix = { XMMatrixAffineTransformation(vWorldScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vResultQuaternion, vResultTranslation) };
	m_pPlayer->Get_Transform()->Set_WorldMatrix(AplliedMatrix);
}

void CPlayer_State_Move_Lever::Set_InterpolateMatrix()
{
	_matrix			WindowWorldMatrix = { XMMatrixRotationY(XMConvertToRadians(180.f)) * XMLoadFloat4x4(&m_pPlayer->Get_Lever_WorldMatrix()) };
	_matrix			Player_WorldMatrix = { m_pPlayer->Get_Transform()->Get_WorldMatrix() };

	_vector			vPlayerScale, vPlayerQuaternion, vPlayerTranslation;
	_vector			vWindowScale, vWindowQuaternion, vWindowTranslation;

	XMMatrixDecompose(&vPlayerScale, &vPlayerQuaternion, &vPlayerTranslation, Player_WorldMatrix);
	XMMatrixDecompose(&vWindowScale, &vWindowQuaternion, &vWindowTranslation, WindowWorldMatrix);

	_matrix			TargetWorldMatrix = { XMMatrixAffineTransformation(vPlayerScale, XMVectorSet(0.f, 0.f, 0.f ,1.f), vWindowQuaternion, vWindowTranslation) };

	_matrix			RootFirstKeyFrameMatrix = { m_pPlayer->Get_Body_Model()->Get_FirstKeyFrame_Root_TransformationMatrix(m_pPlayer->Get_Body_Model()->Get_CurrentAnimLayerTag(0), m_pPlayer->Get_Body_Model()->Get_CurrentAnimIndex(0)) };
	_matrix			ModelTransformMatrix = { XMLoadFloat4x4(&m_pPlayer->Get_Body_Model()->Get_TransformationMatrix()) };

	_vector					vRootScale, vRootQuaternion, vRootTranslation;
	XMMatrixDecompose(&vRootScale, &vRootQuaternion, &vRootTranslation, RootFirstKeyFrameMatrix);

	vRootTranslation = XMVector3TransformNormal(vRootTranslation, ModelTransformMatrix);

	_vector			vRootRotateAxis = { XMVectorSetW(vRootQuaternion, 0.f) };
	vRootRotateAxis = XMVector3TransformNormal(vRootRotateAxis, ModelTransformMatrix);
	vRootQuaternion = XMVectorSetW(vRootRotateAxis, XMVectorGetW(vRootQuaternion));
	RootFirstKeyFrameMatrix = XMMatrixAffineTransformation(vRootScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRootQuaternion, vRootTranslation);

	_matrix			InterpolateTargetMatrix = { RootFirstKeyFrameMatrix * TargetWorldMatrix };

	_vector			vInterpolateScale, vInterpolateQuaternion, vInterpolateTranslation;
	XMMatrixDecompose(&vInterpolateScale, &vInterpolateQuaternion, &vInterpolateTranslation, InterpolateTargetMatrix);

	_vector			vDeltaQuaternion = { XMQuaternionMultiply(XMQuaternionNormalize(XMQuaternionInverse(vPlayerQuaternion)), XMQuaternionNormalize(vInterpolateQuaternion)) };
	_vector			vDeltaTranslation = { vInterpolateTranslation - vPlayerTranslation };

	_matrix			DeltaMatrix = { XMMatrixAffineTransformation(vPlayerScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vDeltaQuaternion, vDeltaTranslation) };
	XMStoreFloat4x4(&m_LeverTransform, DeltaMatrix);
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