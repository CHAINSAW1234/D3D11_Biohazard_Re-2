#include "stdafx.h"
#include "GameInstance.h"
#include "Stun_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CStun_Zombie::CStun_Zombie()
	: CTask_Node()
{
}

CStun_Zombie::CStun_Zombie(const CStun_Zombie& rhs)
{
}

HRESULT CStun_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CStun_Zombie::Enter()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	pBodyModel->Set_TotalLinearInterpolation(0.2f);
	pBodyModel->Set_Loop(static_cast<_uint>(m_ePlayingIndex), false);

	m_isEntry = true;

#ifdef _DEBUG

	cout << "Enter Stun" << endl;

#endif 
}

_bool CStun_Zombie::Execute(_float fTimeDelta)
{


	return false;
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	if (m_pBlackBoard->Get_AI()->Get_Current_MonsterState() == MONSTER_STATE::MST_DAMAGE)
	{
		_bool			isFinsihed = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY)->isFinished(static_cast<_uint>(m_ePlayingIndex)) };
		if (true == isFinsihed)
			return false;
	}

	else
	{
		HIT_TYPE		eHitType = { m_pBlackBoard->Get_AI()->Get_Current_HitType() };
		if (HIT_TYPE::HIT_SMALL != eHitType)
			return false;
	}
	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_DAMAGE);

	if (true == m_isEntry)
	{
		Update_Current_Collider();
		Change_Animation();
		m_isEntry = false;
	}

	return true;
}


void CStun_Zombie::Exit()
{
	m_eCurrentHitCollider = COLLIDER_TYPE::_END;
}

void CStun_Zombie::Update_Current_Collider()
{
	m_eCurrentHitCollider = { m_pBlackBoard->Get_AI()->Get_Current_IntersectCollider() };
}

void CStun_Zombie::Change_Animation()
{
	CZombie::POSE_STATE			ePoseState = { m_pBlackBoard->Get_AI()->Get_PoseState() };
	if (CZombie::POSE_STATE::_CREEP == ePoseState)
	{
		Change_Animation_Creep();
	}

	else if (CZombie::POSE_STATE::_UP == ePoseState)
	{
		Change_Animation_StandUp();
	}

}

void CStun_Zombie::Change_Animation_StandUp()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*			pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_int			iResultAnimationIndex = { -1 };

	_float3			vDirectionFromHitFloat3;
	if (false == m_pBlackBoard->Compute_Direction_From_Hit_Local(&vDirectionFromHitFloat3))
		return;

	_vector			vDirectionFromHit = { XMLoadFloat3(&vDirectionFromHitFloat3) };
	_vector			vDirectionFromHitXZPlane = XMVectorSetY(vDirectionFromHit, 0.f);

	_bool			isFromRight = { 0.f > XMVectorGetX(vDirectionFromHitXZPlane) };
	_bool			isFromFront = { 0.f > XMVectorGetZ(vDirectionFromHitXZPlane) };
	_bool			isBigXAxis = { fabsf(XMVectorGetX(vDirectionFromHitXZPlane)) > fabsf(XMVectorGetZ(vDirectionFromHitXZPlane)) };

	if (m_eCurrentHitCollider == COLLIDER_TYPE::ARM_L || m_eCurrentHitCollider == COLLIDER_TYPE::FOREARM_L || m_eCurrentHitCollider == COLLIDER_TYPE::HAND_L)
	{
		if (true == isFromFront)
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_SHOULDER_L_F);

		else
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_SHOULDER_L_B);
	}

	else if (m_eCurrentHitCollider == COLLIDER_TYPE::ARM_R || m_eCurrentHitCollider == COLLIDER_TYPE::FOREARM_R || m_eCurrentHitCollider == COLLIDER_TYPE::HAND_R)
	{
		if (true == isFromFront)
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_SHOULDER_R_F);

		else
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_SHOULDER_R_B);
	}

	else if (m_eCurrentHitCollider == COLLIDER_TYPE::LEG_L || m_eCurrentHitCollider == COLLIDER_TYPE::CALF_L || m_eCurrentHitCollider == COLLIDER_TYPE::FOOT_L)
	{
		if (true == isFromFront)
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_LEG_L_F);

		else
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_LEG_L_B);
	}

	else if (m_eCurrentHitCollider == COLLIDER_TYPE::LEG_R || m_eCurrentHitCollider == COLLIDER_TYPE::CALF_R || m_eCurrentHitCollider == COLLIDER_TYPE::FOOT_R)
	{
		if (true == isFromFront)
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_LEG_R_F);

		else
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_LEG_R_B);
	}

	else if (m_eCurrentHitCollider == COLLIDER_TYPE::CHEST || m_eCurrentHitCollider == COLLIDER_TYPE::PELVIS)
	{
		if (true == isFromFront)
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_BODY_F);

		else
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_BODY_B);
	}

	else if (m_eCurrentHitCollider == COLLIDER_TYPE::HEAD)
	{
		if (true == isBigXAxis)
		{
			if (true == isFromRight)
				iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_HEAD_LEFT_SIDE_F);

			else
				iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_HEAD_RIGHT_SIDE_F);
		}
		else
		{
			if (true == isFromFront)
			{
				_int		iRandomStun = { m_pGameInstance->GetRandom_Int(static_cast<_int>(ANIM_DAMAGE_STUN::_HEAD_F1), static_cast<_int>(ANIM_DAMAGE_STUN::_HEAD_F4)) };
				iResultAnimationIndex = iRandomStun;
			}

			else
			{
				iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_HEAD_B);
			}
		}
	}

	if (-1 == iResultAnimationIndex)
		return;

	_int				iCurrentAnimIndex = { pBodyModel->Get_CurrentAnimIndex(static_cast<_uint>(m_ePlayingIndex)) };
	_bool				isSameAnim = { iCurrentAnimIndex == iResultAnimationIndex };

	if (false == isSameAnim)
	{
		pBodyModel->Change_Animation(static_cast<_uint>(m_ePlayingIndex), m_strStunAnimLayerTag, iResultAnimationIndex);
		pBodyModel->Set_BoneLayer_PlayingInfo(static_cast<_uint>(m_ePlayingIndex), m_strBoneLayerTag);
	}
	else
	{
		pBodyModel->Set_TrackPosition(static_cast<_uint>(m_ePlayingIndex), 0.f, true);
	}
}

void CStun_Zombie::Change_Animation_Creep()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_int						iResultAnimationIndex = { -1 };
	CZombie::FACE_STATE			eFaceState = { m_pBlackBoard->Get_AI()->Get_FaceState() };



	if (m_eCurrentHitCollider == COLLIDER_TYPE::ARM_L || m_eCurrentHitCollider == COLLIDER_TYPE::FOREARM_L || m_eCurrentHitCollider == COLLIDER_TYPE::HAND_L)
	{
		if (CZombie::FACE_STATE::_UP == eFaceState)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_DEFAULT::_FACEUP_ARM_L);
		}

		else if (CZombie::FACE_STATE::_DOWN == eFaceState)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_DEFAULT::_FACEDOWN_ARM_L);
		}
#ifdef _DEBUG 
		else
			MSG_BOX(TEXT("Calld : void CStun_Zombie::Change_Animation_Creep() 좀비 담당자 호출 "));
#endif 
	}

	else if (m_eCurrentHitCollider == COLLIDER_TYPE::ARM_R || m_eCurrentHitCollider == COLLIDER_TYPE::FOREARM_R || m_eCurrentHitCollider == COLLIDER_TYPE::HAND_R)
	{
		if (CZombie::FACE_STATE::_UP == eFaceState)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_DEFAULT::_FACEUP_ARM_R);
		}

		else if (CZombie::FACE_STATE::_DOWN == eFaceState)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_DEFAULT::_FACEDOWN_ARM_R);
		}
#ifdef _DEBUG 
		else
			MSG_BOX(TEXT("Calld : void CStun_Zombie::Change_Animation_Creep() 좀비 담당자 호출 "));
#endif 
	}

	else if (m_eCurrentHitCollider == COLLIDER_TYPE::LEG_L || m_eCurrentHitCollider == COLLIDER_TYPE::CALF_L || m_eCurrentHitCollider == COLLIDER_TYPE::FOOT_L)
	{
		if (CZombie::FACE_STATE::_UP == eFaceState)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_DEFAULT::_FACEUP_LEG_L);
		}

		else if (CZombie::FACE_STATE::_DOWN == eFaceState)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_DEFAULT::_FACEDOWN_LEG_L);
		}
#ifdef _DEBUG 
		else
			MSG_BOX(TEXT("Calld : void CStun_Zombie::Change_Animation_Creep() 좀비 담당자 호출 "));
#endif 
	}

	else if (m_eCurrentHitCollider == COLLIDER_TYPE::LEG_R || m_eCurrentHitCollider == COLLIDER_TYPE::CALF_R || m_eCurrentHitCollider == COLLIDER_TYPE::FOOT_R)
	{
		if (CZombie::FACE_STATE::_UP == eFaceState)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_DEFAULT::_FACEUP_LEG_R);
		}

		else if (CZombie::FACE_STATE::_DOWN == eFaceState)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_DEFAULT::_FACEDOWN_LEG_R);
		}
#ifdef _DEBUG 
		else
			MSG_BOX(TEXT("Calld : void CStun_Zombie::Change_Animation_Creep() 좀비 담당자 호출 "));
#endif 
	}

	/*else if (m_eCurrentHitCollider == COLLIDER_TYPE::CHEST || m_eCurrentHitCollider == COLLIDER_TYPE::PELVIS || m_eCurrentHitCollider == COLLIDER_TYPE::HEAD)
	{
		iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_DEFAULT::_FACEDOWN_BODY);
	}*/

	if (-1 == iResultAnimationIndex)
		return;

	_int				iCurrentAnimIndex = { pBodyModel->Get_CurrentAnimIndex(static_cast<_uint>(m_ePlayingIndex)) };
	_bool				isSameAnim = { iCurrentAnimIndex == iResultAnimationIndex };

	if (false == isSameAnim)
	{
		pBodyModel->Change_Animation(static_cast<_uint>(m_ePlayingIndex), m_strDefualtStunAnimLayerTag, iResultAnimationIndex);
		pBodyModel->Set_BoneLayer_PlayingInfo(static_cast<_uint>(m_ePlayingIndex), m_strBoneLayerTag);
	}
	else
	{
		pBodyModel->Set_TrackPosition(static_cast<_uint>(m_ePlayingIndex), 0.f, true);
	}
}

CStun_Zombie* CStun_Zombie::Create(void* pArg)
{
	CStun_Zombie* pInstance = { new CStun_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CStun_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStun_Zombie::Free()
{
	__super::Free();
}
