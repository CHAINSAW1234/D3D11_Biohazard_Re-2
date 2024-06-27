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

	CModel*			pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	pBodyModel->Set_TotalLinearInterpolation(0.2f);
	pBodyModel->Set_Loop(static_cast<_uint>(PLAYING_INDEX::INDEX_0), false);

	m_isEntry = true;
}

_bool CStun_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->GetAI();
	pAI->SetState(MONSTER_STATE::MST_DAMAGE);

	//	cout << "Wait" << endl;
	Change_Animation();

	return true;
}

void CStun_Zombie::Exit()
{
	m_eCurrentHitCollider = COLLIDER_TYPE::_END;
}

void CStun_Zombie::Change_Animation()
{
	if (nullptr == m_pBlackBoard)
		return;

	_bool						isSameColliderPreCollision = { false }; 
	if (true == m_isEntry)
	{
		m_isEntry = false;
		COLLIDER_TYPE			eCurrentCollisionColliderType = { m_pBlackBoard->GetAI()->Get_Current_IntersectCollider() };
		if (m_eCurrentHitCollider == eCurrentCollisionColliderType)
		{
			isSameColliderPreCollision = true;
		}	

		m_eCurrentHitCollider = eCurrentCollisionColliderType;
	}

	else
	{
		return;
	}

	CModel*			pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_uint			iPlayingIndex = { static_cast<_uint>(PLAYING_INDEX::INDEX_0) };
	_int			iResultAnimationIndex = { -1 };
	wstring			strBoneLayerTag = { BONE_LAYER_DEFAULT_TAG };
	wstring			strAnimLayerTag = { TEXT("Damage_Stun") };

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
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_SHOULDERR_L_F);
		}

		else
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_SHOULDERR_L_B);
		}
	}
	
	else if (m_eCurrentHitCollider == COLLIDER_TYPE::ARM_R || m_eCurrentHitCollider == COLLIDER_TYPE::FOREARM_R || m_eCurrentHitCollider == COLLIDER_TYPE::HAND_R)
	{
		if (true == isFromFront)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_SHOULDERR_R_F);
		}

		else
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_SHOULDERR_R_B);
		}
	}

	else if (m_eCurrentHitCollider == COLLIDER_TYPE::LEG_L || m_eCurrentHitCollider == COLLIDER_TYPE::CALF_L || m_eCurrentHitCollider == COLLIDER_TYPE::FOOT_L)
	{
		if (true == isFromFront)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_LEG_L_F);
		}

		else
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_LEG_L_B);
		}
	}

	else if (m_eCurrentHitCollider == COLLIDER_TYPE::LEG_R || m_eCurrentHitCollider == COLLIDER_TYPE::CALF_R || m_eCurrentHitCollider == COLLIDER_TYPE::FOOT_R)
	{
		if (true == isFromFront)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_LEG_R_F);
		}

		else
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_LEG_R_B);
		}
	}

	else if (m_eCurrentHitCollider == COLLIDER_TYPE::CHEST || m_eCurrentHitCollider == COLLIDER_TYPE::PELVIS)
	{
		if (true == isFromFront)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_BODY_F);
		}

		else
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_BODY_B);
		}
	}

	else if (m_eCurrentHitCollider == COLLIDER_TYPE::HEAD)
	{
		if (true == isBigXAxis)
		{
			if (true == isFromRight)
			{
				iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_HEAD_LEFT_SIDE_F);
			}

			else
			{
				iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_STUN::_HEAD_RIGHT_SIDE_F);
			}
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

	if (false == isSameColliderPreCollision)
	{
		pBodyModel->Change_Animation(iPlayingIndex, strAnimLayerTag, iResultAnimationIndex);
		pBodyModel->Set_BoneLayer_PlayingInfo(iPlayingIndex, strBoneLayerTag);
	}
	else
	{
		pBodyModel->Set_TrackPosition(iPlayingIndex, 0.f, true);
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
