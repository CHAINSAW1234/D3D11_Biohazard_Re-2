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
	Set_MoveAnimation();

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
	if (m_dwDirection & BACK)
		m_Prev_FRONT_BACK_Direction = BACK;

}

void CPlayer_State_Move_Walk::Set_MoveAnimation()
{
	if (m_dwDirection & FRONT) {
		if (m_dwDirection & LEFT) {
			CModel::ANIM_PLAYING_DESC		AnimDesc;
			AnimDesc.iAnimIndex = CPlayer::WALK_F_LOOP;
			AnimDesc.isLoop = true;
			AnimDesc.fWeight = .5f;
			AnimDesc.strBoneLayerTag = TEXT("Default");

			m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 0);

			AnimDesc.iAnimIndex = CPlayer::WALK_L_LOOP;
			m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 1);

			AnimDesc.fWeight = 0.f;
			m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 2);
		}
		else if (m_dwDirection & RIGHT) {
			CModel::ANIM_PLAYING_DESC		AnimDesc;
			AnimDesc.iAnimIndex = CPlayer::WALK_F_LOOP;
			AnimDesc.isLoop = true;
			AnimDesc.fWeight = .5f;
			AnimDesc.strBoneLayerTag = TEXT("Default");

			m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 0);

			AnimDesc.iAnimIndex = CPlayer::WALK_R_LOOP;
			m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 1);

			AnimDesc.fWeight = 0.f;
			m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 2);
		}
		else {
			CModel::ANIM_PLAYING_DESC		AnimDesc;
			AnimDesc.iAnimIndex = CPlayer::WALK_F_LOOP;
			AnimDesc.isLoop = true;
			AnimDesc.fWeight = 1.f;
			AnimDesc.strBoneLayerTag = TEXT("Default");

			m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 0);

			AnimDesc.fWeight = 0.f;
			m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 1);
			m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 2);
		}
	}

	else if (m_dwDirection & BACK) {
		if (m_dwDirection & LEFT) {
			CModel::ANIM_PLAYING_DESC		AnimDesc;
			AnimDesc.iAnimIndex = CPlayer::WALK_BACK_B_LOOP;
			AnimDesc.isLoop = true;
			AnimDesc.fWeight = 0.5f;
			AnimDesc.strBoneLayerTag = TEXT("Default");

			m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 0);

			AnimDesc.iAnimIndex = CPlayer::WALK_BACK_L_LOOP;
			m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 1);

			AnimDesc.fWeight = 0.f;
			m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 2);
		}
		else if (m_dwDirection & RIGHT) {
			CModel::ANIM_PLAYING_DESC		AnimDesc;
			AnimDesc.iAnimIndex = CPlayer::WALK_BACK_B_LOOP;
			AnimDesc.isLoop = true;
			AnimDesc.fWeight = .5f;
			AnimDesc.strBoneLayerTag = TEXT("Default");

			m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 0);

			AnimDesc.iAnimIndex = CPlayer::WALK_BACK_R_LOOP;
			m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 1);

			AnimDesc.fWeight = 0.f;
			m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 2);
		}
		else {
			CModel::ANIM_PLAYING_DESC		AnimDesc;
			AnimDesc.iAnimIndex = CPlayer::WALK_BACK_B_LOOP;
			AnimDesc.isLoop = true;
			AnimDesc.fWeight = 1.f;
			AnimDesc.strBoneLayerTag = TEXT("Default");

			m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 0);

			AnimDesc.fWeight = 0.f;
			m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 1);
			m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 2);
		}
	}
	else {
		CModel::ANIM_PLAYING_DESC		AnimDesc;
		AnimDesc.isLoop = true;
		AnimDesc.fWeight = 1.f;
		AnimDesc.strBoneLayerTag = TEXT("Default");

		if (m_dwDirection & LEFT) {
			if (m_Prev_FRONT_BACK_Direction == FRONT) {
				AnimDesc.iAnimIndex = CPlayer::WALK_L_LOOP;
			}
			else {
				AnimDesc.iAnimIndex = CPlayer::WALK_BACK_L_LOOP;
			}

			m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 0);
		}
		else {
			if (m_Prev_FRONT_BACK_Direction == FRONT) {
				AnimDesc.iAnimIndex = CPlayer::WALK_R_LOOP;
			}
			else {
				AnimDesc.iAnimIndex = CPlayer::WALK_BACK_R_LOOP;
			}
			m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 0);
		}

		AnimDesc.fWeight = 0.f;
		m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 1);
		m_pPlayer->Get_Body_Model()->Set_Animation_Blend(AnimDesc, 2);
	}

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
	
	_float fDegree = Cal_Degree_From_Directions_Between_Min180_To_180(vCamLook, vPlayerLook);

	if (fDegree > 10) {
		m_pPlayer->m_pTransformCom->Turn(_float4(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	}
	else if(fDegree < -10) {
		m_pPlayer->m_pTransformCom->Turn(_float4(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	}
/*
	135  -> 90 ~180 비율
	135 - 90  45
	120 - 90 30 /90 * 100  = t    -> 1-t : t
	45 / 90 * 100 = 50 : 0.5 0.5 비율로 섞으면 됨




*/


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
