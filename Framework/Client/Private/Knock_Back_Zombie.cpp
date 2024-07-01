#include "stdafx.h"
#include "GameInstance.h"
#include "Knock_Back_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CKnock_Back_Zombie::CKnock_Back_Zombie()
	: CTask_Node()
{
}

CKnock_Back_Zombie::CKnock_Back_Zombie(const CKnock_Back_Zombie& rhs)
{
}

HRESULT CKnock_Back_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CKnock_Back_Zombie::Enter()
{
	CModel*			pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	pBodyModel->Set_TotalLinearInterpolation(0.2f);
	pBodyModel->Set_Loop(static_cast<_uint>(PLAYING_INDEX::INDEX_0), false);

	m_isEntry = true;
}

_bool CKnock_Back_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->SetState(MONSTER_STATE::MST_DAMAGE);

	if (true == m_isEntry)
	{
		Update_Current_Collider();
		Change_Animation();
		m_isEntry = false;
	}

	return true;
}

void CKnock_Back_Zombie::Exit()
{
	m_eCurrentHitCollider = COLLIDER_TYPE::_END;
}

void CKnock_Back_Zombie::Update_Current_Collider()
{
	m_eCurrentHitCollider = { m_pBlackBoard->Get_AI()->Get_Current_IntersectCollider() };
}

void CKnock_Back_Zombie::Change_Animation()
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


	if (m_eCurrentHitCollider == COLLIDER_TYPE::ARM_L || m_eCurrentHitCollider == COLLIDER_TYPE::FOREARM_L || m_eCurrentHitCollider == COLLIDER_TYPE::HAND_L ||
		m_eCurrentHitCollider == COLLIDER_TYPE::LEG_L || m_eCurrentHitCollider == COLLIDER_TYPE::CALF_L || m_eCurrentHitCollider == COLLIDER_TYPE::FOOT_L
		)
	{
		if (true == isFromFront)
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_KNOCKBACK::_F_L_SLANT);

		else
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_KNOCKBACK::_B_L_SLANT);
	}

	else if (m_eCurrentHitCollider == COLLIDER_TYPE::ARM_R || m_eCurrentHitCollider == COLLIDER_TYPE::FOREARM_R || m_eCurrentHitCollider == COLLIDER_TYPE::HAND_R ||
		m_eCurrentHitCollider == COLLIDER_TYPE::LEG_R || m_eCurrentHitCollider == COLLIDER_TYPE::CALF_R || m_eCurrentHitCollider == COLLIDER_TYPE::FOOT_R)
	{
		if (true == isFromFront)
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_KNOCKBACK::_F_R_SLANT);

		else
			iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_KNOCKBACK::_B_R_SLANT);
	}

	else if (m_eCurrentHitCollider == COLLIDER_TYPE::CHEST || m_eCurrentHitCollider == COLLIDER_TYPE::PELVIS)
	{
		if (true == isBigXAxis)
		{
			if (true == isFromRight)
				iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_KNOCKBACK::_BODY_R);

			else
				iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_KNOCKBACK::_BODY_L);

		}
		else
		{
			if (true == isFromFront)
				iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_KNOCKBACK::_BODY_F);

			else
				iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_KNOCKBACK::_BODY_B);
		}		
	}

	else if (m_eCurrentHitCollider == COLLIDER_TYPE::HEAD)
	{
		if (true == isBigXAxis)
		{
			if (true == isFromFront)
			{
				if (true == isFromRight)
				{
					_int		iRandomKnockBack = { m_pGameInstance->GetRandom_Int(1, 2) };
					if (1 == iRandomKnockBack)
						iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_KNOCKBACK::_HEAD_FL1);

					else if (2 == iRandomKnockBack)
						iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_KNOCKBACK::_HEAD_FL2);

					iResultAnimationIndex = iRandomKnockBack;
				}

				else
				{
					_int		iRandomKnockBack = { m_pGameInstance->GetRandom_Int(1, 2) };
					if (1 == iRandomKnockBack)
						iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_KNOCKBACK::_HEAD_FR1);

					else if (2 == iRandomKnockBack)
						iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_KNOCKBACK::_HEAD_FR2);

					iResultAnimationIndex = iRandomKnockBack;
				}
			}

			else
			{
				if (true == isFromRight)
					iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_KNOCKBACK::_HEAD_R);

				else
					iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_KNOCKBACK::_HEAD_L);
			}			
		}
		else
		{
			if (true == isFromFront)
			{
				_int		iRandomStun = { m_pGameInstance->GetRandom_Int(static_cast<_int>(ANIM_DAMAGE_KNOCKBACK::_HEAD_F1), static_cast<_int>(ANIM_DAMAGE_KNOCKBACK::_HEAD_F4)) };
				iResultAnimationIndex = iRandomStun;
			}

			else
			{
				iResultAnimationIndex = static_cast<_int>(ANIM_DAMAGE_KNOCKBACK::_HEAD_B);
			}
		}
	}

	if (-1 == iResultAnimationIndex)
		return;

	_int				iCurrentAnimIndex = { pBodyModel->Get_CurrentAnimIndex(static_cast<_uint>(m_ePlayingIndex)) };
	_bool				isSameAnim = { iCurrentAnimIndex == iResultAnimationIndex };

	if (false == isSameAnim)
	{
		pBodyModel->Change_Animation(static_cast<_uint>(m_ePlayingIndex), m_strAnimLayerTag, iResultAnimationIndex);
		pBodyModel->Set_BoneLayer_PlayingInfo(static_cast<_uint>(m_ePlayingIndex), m_strBoneLayerTag);
	}
	else
	{
		pBodyModel->Set_TrackPosition(static_cast<_uint>(m_ePlayingIndex), 0.f, true);
	}
}

CKnock_Back_Zombie* CKnock_Back_Zombie::Create(void* pArg)
{
	CKnock_Back_Zombie* pInstance = { new CKnock_Back_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CKnock_Back_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKnock_Back_Zombie::Free()
{
	__super::Free();
}
