#include "stdafx.h"
#include "GameInstance.h"
#include "Shake_Skin_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

const wstring CShake_Skin_Zombie::m_strL_Arm_AnimLayerTag = { TEXT("Add_Arm_L") };
const wstring CShake_Skin_Zombie::m_strR_Arm_AnimLayerTag = { TEXT("Add_Arm_R") };
const wstring CShake_Skin_Zombie::m_strL_Leg_AnimLayerTag = { TEXT("Add_Leg_L") };
const wstring CShake_Skin_Zombie::m_strR_Leg_AnimLayerTag = { TEXT("Add_Leg_R") };
const wstring CShake_Skin_Zombie::m_strL_Shoulder_AnimLayerTag = { TEXT("Add_Shoulder_L") };
const wstring CShake_Skin_Zombie::m_strR_Shoulder_AnimLayerTag = { TEXT("Add_Shoulder_R") };
const wstring CShake_Skin_Zombie::m_strBody_AnimLayerTag = { TEXT("Add_Body") };
const wstring CShake_Skin_Zombie::m_strHead_AnimLayerTag= { TEXT("Add_Head") };

const wstring CShake_Skin_Zombie::m_strL_Leg_Twist_BoneLayerTag = { BONE_LAYER_L_LEG_TWIST_TAG };
const wstring CShake_Skin_Zombie::m_strR_Leg_Twist_BoneLayerTag = { BONE_LAYER_R_LEG_TWIST_TAG };
const wstring CShake_Skin_Zombie::m_strL_Arm_Twist_BoneLayerTag = { BONE_LAYER_L_ARM_TWIST_TAG };
const wstring CShake_Skin_Zombie::m_strR_Arm_Twist_BoneLayerTag = { BONE_LAYER_R_ARM_TWIST_TAG };
const wstring CShake_Skin_Zombie::m_strL_Shoulder_Twist_BoneLayerTag = { BONE_LAYER_L_SHOULDER_TWIST_TAG };
const wstring CShake_Skin_Zombie::m_strR_Shoulder_Twist_BoneLayerTag = { BONE_LAYER_R_SHOULDER_TWIST_TAG };
const wstring CShake_Skin_Zombie::m_str_Body_Twist_BoneLayerTag = { BONE_LAYER_BODY_TWIST_TAG };
const wstring CShake_Skin_Zombie::m_str_Head_Twist_BoneLayerTag = { BONE_LAYER_HEAD_TWIST_TAG };

CShake_Skin_Zombie::CShake_Skin_Zombie()
	: CTask_Node()
{
}

CShake_Skin_Zombie::CShake_Skin_Zombie(const CShake_Skin_Zombie& rhs)
{
}

HRESULT CShake_Skin_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CShake_Skin_Zombie::Enter()
{

}

_bool CShake_Skin_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion
	CZombie*			pZombie = { m_pBlackBoard->Get_AI() };
	if (nullptr == pZombie)
		return false;

	Update_BlendWeights();

	HIT_TYPE			eCurrentHitType = { pZombie->Get_Current_HitType() };
	if (HIT_TYPE::HIT_END == eCurrentHitType)
		return true;

	_float3				vHitDirectionLocalFloat3 = {};
	if (false == m_pBlackBoard->Compute_Direction_From_Hit_Local(&vHitDirectionLocalFloat3))
		return true;

	COLLIDER_TYPE		eIntersectCollider = { pZombie->Get_Current_IntersectCollider() };
	if (COLLIDER_TYPE::_END == eIntersectCollider)
		return true;

	m_pBlackBoard->Get_AI()->Play_Random_Hit_Sound();

	DIRECTION			eHitDirection = { vHitDirectionLocalFloat3.z < 0.f ? DIRECTION::_F : DIRECTION::_B };

	Add_Blend_Animation(eIntersectCollider, eHitDirection);

	return true;
}

void CShake_Skin_Zombie::Exit()
{
}

void CShake_Skin_Zombie::Add_Blend_Animation(COLLIDER_TYPE eIntersectCollider, DIRECTION eHitDirection)
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*			pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBody_Model)
		return;

	_int			iResultAnimationIndex = { -1 };
	wstring			strAnimLayerTag = { TEXT("") };
	wstring			strBoneLayerTag = { TEXT("") };
	PLAYING_INDEX	ePlayingIndex = { PLAYING_INDEX::INDEX_END };

	if (COLLIDER_TYPE::FOREARM_L == eIntersectCollider ||
		COLLIDER_TYPE::HAND_L == eIntersectCollider)
	{
		if(DIRECTION::_F == eHitDirection)
			iResultAnimationIndex = static_cast<_uint>(ANIM_ADD_ARM_L::_FRONT);
		else if(DIRECTION::_B == eHitDirection)
			iResultAnimationIndex = static_cast<_uint>(ANIM_ADD_ARM_L::_BACK);

		ePlayingIndex = m_eL_Arm_PlayingIndex;
		strAnimLayerTag = m_strL_Arm_AnimLayerTag;
		strBoneLayerTag = m_strL_Arm_Twist_BoneLayerTag;
	}

	else if (COLLIDER_TYPE::ARM_L == eIntersectCollider)
	{
		if (DIRECTION::_F == eHitDirection)
			iResultAnimationIndex = static_cast<_uint>(ANIM_ADD_SHOULDER_L::_FRONT);
		else if (DIRECTION::_B == eHitDirection)
			iResultAnimationIndex = static_cast<_uint>(ANIM_ADD_SHOULDER_L::_BACK);

		ePlayingIndex = m_eL_Shoulder_PlayingIndex;
		strAnimLayerTag = m_strL_Shoulder_AnimLayerTag;
		strBoneLayerTag = m_strL_Shoulder_Twist_BoneLayerTag;
	}

	else if (COLLIDER_TYPE::FOREARM_R == eIntersectCollider ||
		COLLIDER_TYPE::HAND_R == eIntersectCollider)
	{
		if (DIRECTION::_F == eHitDirection)
			iResultAnimationIndex = static_cast<_uint>(ANIM_ADD_ARM_R::_FRONT);
		else if (DIRECTION::_B == eHitDirection)
			iResultAnimationIndex = static_cast<_uint>(ANIM_ADD_ARM_R::_BACK);

		ePlayingIndex = m_eR_Arm_PlayingIndex;
		strAnimLayerTag = m_strR_Arm_AnimLayerTag;
		strBoneLayerTag = m_strR_Arm_Twist_BoneLayerTag;
	}

	else if (COLLIDER_TYPE::ARM_R == eIntersectCollider)
	{
		if (DIRECTION::_F == eHitDirection)
			iResultAnimationIndex = static_cast<_uint>(ANIM_ADD_SHOULDER_R::_FRONT);
		else if (DIRECTION::_B == eHitDirection)
			iResultAnimationIndex = static_cast<_uint>(ANIM_ADD_SHOULDER_R::_BACK);

		ePlayingIndex = m_eR_Shoulder_PlayingIndex;
		strAnimLayerTag = m_strR_Shoulder_AnimLayerTag;
		strBoneLayerTag = m_strR_Shoulder_Twist_BoneLayerTag;
	}

	else if (COLLIDER_TYPE::LEG_L == eIntersectCollider ||
		COLLIDER_TYPE::CALF_L == eIntersectCollider ||
		COLLIDER_TYPE::FOOT_L == eIntersectCollider)
	{
		if (DIRECTION::_F == eHitDirection)
			iResultAnimationIndex = static_cast<_uint>(ANIM_ADD_LEG_L::_FRONT);
		else if (DIRECTION::_B == eHitDirection)
			iResultAnimationIndex = static_cast<_uint>(ANIM_ADD_LEG_L::_BACK);

		ePlayingIndex = m_eL_Leg_PlayingIndex;
		strAnimLayerTag = m_strL_Leg_AnimLayerTag;
		strBoneLayerTag = m_strL_Leg_Twist_BoneLayerTag;
	}

	else if (COLLIDER_TYPE::LEG_R == eIntersectCollider ||
		COLLIDER_TYPE::CALF_R == eIntersectCollider ||
		COLLIDER_TYPE::FOOT_R == eIntersectCollider)
	{
		if (DIRECTION::_F == eHitDirection)
			iResultAnimationIndex = static_cast<_uint>(ANIM_ADD_LEG_R::_FRONT);
		else if (DIRECTION::_B == eHitDirection)
			iResultAnimationIndex = static_cast<_uint>(ANIM_ADD_LEG_R::_BACK);

		ePlayingIndex = m_eR_Leg_PlayingIndex;
		strAnimLayerTag = m_strR_Leg_AnimLayerTag;
		strBoneLayerTag = m_strR_Leg_Twist_BoneLayerTag;
	}

	else if (COLLIDER_TYPE::HEAD == eIntersectCollider || 
		COLLIDER_TYPE::CHEST == eIntersectCollider ||
		COLLIDER_TYPE::PELVIS == eIntersectCollider)
	{
		CZombie*			pZombie = { m_pBlackBoard->Get_AI() };
		if (nullptr == pZombie)
			return;

		CZombie::POSE_STATE				ePoseState = { pZombie->Get_PoseState() };
		if (CZombie::POSE_STATE::_CREEP == ePoseState)
		{
			CZombie::FACE_STATE			eFaceState = { pZombie->Get_FaceState() };			
			if (COLLIDER_TYPE::HEAD == eIntersectCollider)
			{
				if (CZombie::FACE_STATE::_UP == eFaceState)
					iResultAnimationIndex = static_cast<_uint>(ANIM_ADD_HEAD::_FACE_UP);
				else if (CZombie::FACE_STATE::_DOWN == eFaceState)
					iResultAnimationIndex = static_cast<_uint>(ANIM_ADD_HEAD::_FACE_DOWN);

				ePlayingIndex = m_eHead_PlayingIndex;
				strAnimLayerTag = m_strHead_AnimLayerTag;
				strBoneLayerTag = m_str_Head_Twist_BoneLayerTag;
			}

			else if (COLLIDER_TYPE::CHEST == eIntersectCollider ||
				COLLIDER_TYPE::PELVIS == eIntersectCollider)
			{
				if (CZombie::FACE_STATE::_UP == eFaceState)
					iResultAnimationIndex = static_cast<_uint>(ANIM_ADD_BODY::_FACE_UP);
				else if (CZombie::FACE_STATE::_DOWN == eFaceState)
					iResultAnimationIndex = static_cast<_uint>(ANIM_ADD_BODY::_FACE_DOWN);

				ePlayingIndex = m_eBody_PlayingIndex;
				strAnimLayerTag = m_strHead_AnimLayerTag;
				strBoneLayerTag = m_str_Head_Twist_BoneLayerTag;
			}

#ifdef _DEBUG
			else
			{
				MSG_BOX(TEXT("Called : void CShake_Skin_Zombie::Add_Blend_Animation(COLLIDER_TYPE eIntersectCollider, DIRECTION eHitDirection) 좀비 담당자 호출 "));
			}
#endif
		}

		else if (CZombie::POSE_STATE::_UP == ePoseState)
		{
			if (COLLIDER_TYPE::HEAD == eIntersectCollider)
			{
				if (DIRECTION::_F == eHitDirection)
					iResultAnimationIndex = static_cast<_uint>(ANIM_ADD_HEAD::_FRONT);
				else if (DIRECTION::_B == eHitDirection)
					iResultAnimationIndex = static_cast<_uint>(ANIM_ADD_HEAD::_BACK);

				ePlayingIndex = m_eHead_PlayingIndex;
				strAnimLayerTag = m_strHead_AnimLayerTag;
				strBoneLayerTag = m_str_Head_Twist_BoneLayerTag;
			}

			else if (COLLIDER_TYPE::CHEST == eIntersectCollider ||
				COLLIDER_TYPE::PELVIS == eIntersectCollider)
			{
				if (DIRECTION::_F == eHitDirection)
					iResultAnimationIndex = static_cast<_uint>(ANIM_ADD_BODY::_FRONT);
				else if (DIRECTION::_B == eHitDirection)
					iResultAnimationIndex = static_cast<_uint>(ANIM_ADD_BODY::_BACK);

				ePlayingIndex = m_eBody_PlayingIndex;
				strAnimLayerTag = m_strBody_AnimLayerTag;
				strBoneLayerTag = m_str_Body_Twist_BoneLayerTag;
			}

#ifdef _DEBUG
			else
			{
				MSG_BOX(TEXT("Called : void CShake_Skin_Zombie::Add_Blend_Animation(COLLIDER_TYPE eIntersectCollider, DIRECTION eHitDirection) 좀비 담당자 호출 "));
			}
#endif
		}
	}

	if (TEXT("") == strAnimLayerTag)
		return;

	if (TEXT("") == strBoneLayerTag)
		return;

	if (-1 == iResultAnimationIndex)
		return;

	if (PLAYING_INDEX::INDEX_END == ePlayingIndex)
		return;

	strBoneLayerTag = TEXT("Default");

	unordered_set<PLAYING_INDEX>::iterator			iterPlayingIndex = { m_ActivePlayingIndcies.find(ePlayingIndex) };
	if (iterPlayingIndex == m_ActivePlayingIndcies.end())
	{
		pBody_Model->Reset_PreAnim_CurrentAnim(static_cast<_uint>(ePlayingIndex));
		pBody_Model->Change_Animation(static_cast<_uint>(ePlayingIndex), strAnimLayerTag, iResultAnimationIndex);
		pBody_Model->Set_Loop(static_cast<_uint>(ePlayingIndex), false);
		pBody_Model->Set_BlendWeight(static_cast<_uint>(ePlayingIndex), ZOMBIE_BLEND_MAX, ZOMBIE_SHAKE_SKIN_BLEND_ON_TIME);
		pBody_Model->Set_BoneLayer_PlayingInfo(static_cast<_uint>(ePlayingIndex), strBoneLayerTag);
		pBody_Model->Set_Additional_Masking(static_cast<_uint>(ePlayingIndex), true, 4);

		m_ActivePlayingIndcies.emplace(ePlayingIndex);
	}
	else
	{
		pBody_Model->Set_TrackPosition(static_cast<_uint>(ePlayingIndex), 0.f, false);
		pBody_Model->Set_BoneLayer_PlayingInfo(static_cast<_uint>(ePlayingIndex), strBoneLayerTag);
	}
}

void CShake_Skin_Zombie::Update_BlendWeights()
{
	CModel*			pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBody_Model)
		return;

	for (auto& iter = m_ActivePlayingIndcies.begin(); iter != m_ActivePlayingIndcies.end(); )
	{
		PLAYING_INDEX			ePlayingIndex = { *iter };
		if (true == pBody_Model->isFinished(static_cast<_uint>(ePlayingIndex)))
		{
			pBody_Model->Set_BlendWeight(static_cast<_uint>(ePlayingIndex), ZOMBIE_BLEND_ZERO, ZOMBIE_SHAKE_SKIN_BLEND_OFF_TIME);
			iter = m_ActivePlayingIndcies.erase(iter);
		}
		else
			++iter;
	}
}

CShake_Skin_Zombie* CShake_Skin_Zombie::Create(void* pArg)
{
	CShake_Skin_Zombie* pInstance = { new CShake_Skin_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CShake_Skin_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShake_Skin_Zombie::Free()
{
	__super::Free();
}
