#include "stdafx.h"
#include "GameInstance.h"
#include "Pivot_Turn_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"

#include "Body_Zombie.h"

CPivot_Turn_Zombie::CPivot_Turn_Zombie()
	: CTask_Node{}
{
}

CPivot_Turn_Zombie::CPivot_Turn_Zombie(const CPivot_Turn_Zombie& rhs)
	: CTask_Node{ rhs }
{
}

HRESULT CPivot_Turn_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CPivot_Turn_Zombie::Enter()
{
	
}

_bool CPivot_Turn_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->GetAI();
	pAI->SetState(MONSTER_STATE::MST_TURN);

	Change_Animation(fTimeDelta);

	return true;
}

void CPivot_Turn_Zombie::Exit()
{
}

void CPivot_Turn_Zombie::Change_Animation(_float fTimeDelta)
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*				pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;


	PLAYING_INDEX					eBasePlayingIndex = { PLAYING_INDEX::INDEX_0 };
	ZOMBIE_BODY_ANIM_GROUP			eCurrentAnimGroup = { m_pBlackBoard->Get_Current_AnimGroup(eBasePlayingIndex) };
	ZOMBIE_BODY_ANIM_TYPE			eCurrentAnimType = { m_pBlackBoard->Get_Current_AnimType(eBasePlayingIndex) };
	MOTION_TYPE						eCurrentMotionType = { static_cast<MOTION_TYPE>(m_pBlackBoard->Get_Current_MotionType_Body()) };

	_bool							isTurnEntry = { false };
	if (ZOMBIE_BODY_ANIM_TYPE::_TURN != eCurrentAnimType)
	{
		isTurnEntry = true;
	}

	_int			iResultAnimationIndex = { -1 };
	_bool			isLoop = { false };
	wstring			strBaseBoneLayerTag = { BONE_LAYER_DEFAULT_TAG };

	_float3			vDirectionToPlayerLocalFloat3;
	if (false == m_pBlackBoard->Compute_Direction_To_Player_Local(&vDirectionToPlayerLocalFloat3))
		return;

	_vector			vDirectionToPlayerLocal = { XMLoadFloat3(&vDirectionToPlayerLocalFloat3) };
	_vector			vDirectionToPlayerLocalXZPlane = { XMVector3Normalize(XMVectorSetY(vDirectionToPlayerLocal, 0.f)) };		//	회전량을 xz평면상에서만 고려하기위함
	_vector			vAILookLocal = { XMVectorSet(0.f, 0.f, 1.f, 0.f) };

	_bool			isRight = { XMVectorGetX(vDirectionToPlayerLocalXZPlane) > 0.f };
	_bool			isFront = { XMVectorGetZ(vDirectionToPlayerLocalXZPlane) > 0.f };

	_float			fDot = { XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDirectionToPlayerLocal), XMVector3Normalize(vAILookLocal))) };
	_float			fAngleToTarget = { acosf(fDot) };

	if (true == isRight)
	{
		if (true == isFront)
		{
			if (MOTION_TYPE::MOTION_A == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R90_A);
			else if (MOTION_TYPE::MOTION_B == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R90_B);
			else if (MOTION_TYPE::MOTION_C == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R90_C);
			else if (MOTION_TYPE::MOTION_D == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R90_D);
			else if (MOTION_TYPE::MOTION_E == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R90_E);
			else if (MOTION_TYPE::MOTION_F == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R90_F);
			else
				MSG_BOX(TEXT("이럴리없어 좀비 담당자 호출 ㄱ"));
		}
		
		else
		{
			if (MOTION_TYPE::MOTION_A == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R180_A);
			else if (MOTION_TYPE::MOTION_B == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R180_B);
			else if (MOTION_TYPE::MOTION_C == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R180_C);
			else if (MOTION_TYPE::MOTION_D == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R180_D);
			else if (MOTION_TYPE::MOTION_E == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R180_E);
			else if (MOTION_TYPE::MOTION_F == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R180_F);
			else
				MSG_BOX(TEXT("이럴리없어 좀비 담당자 호출 ㄱ"));
		}
	}

	else 
	{
		if (true == isFront)
		{
			if (MOTION_TYPE::MOTION_A == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L90_A);
			else if (MOTION_TYPE::MOTION_B == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L90_B);
			else if (MOTION_TYPE::MOTION_C == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L90_C);
			else if (MOTION_TYPE::MOTION_D == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L90_D);
			else if (MOTION_TYPE::MOTION_E == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L90_E);
			else if (MOTION_TYPE::MOTION_F == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L90_F);
			else
				MSG_BOX(TEXT("이럴리없어 좀비 담당자 호출 ㄱ"));
		}

		else
		{
			if (MOTION_TYPE::MOTION_A == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L180_A);
			else if (MOTION_TYPE::MOTION_B == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L180_B);
			else if (MOTION_TYPE::MOTION_C == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L180_C);
			else if (MOTION_TYPE::MOTION_D == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L180_D);
			else if (MOTION_TYPE::MOTION_E == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L180_E);
			else if (MOTION_TYPE::MOTION_F == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L180_F);
			else
				MSG_BOX(TEXT("이럴리없어 좀비 담당자 호출 ㄱ"));
		}		
	}

	if (-1 == iResultAnimationIndex)
		return;

	pBodyModel->Change_Animation(static_cast<_uint>(PLAYING_INDEX::INDEX_0), TEXT("Ordinary_PivotTurn"), iResultAnimationIndex);
	pBodyModel->Set_Loop(static_cast<_uint>(PLAYING_INDEX::INDEX_0), isLoop);
	pBodyModel->Set_BoneLayer_PlayingInfo(static_cast<_uint>(PLAYING_INDEX::INDEX_0), strBaseBoneLayerTag);
	pBodyModel->Set_BlendWeight(static_cast<_uint>(PLAYING_INDEX::INDEX_0), 1.f);
}

CPivot_Turn_Zombie* CPivot_Turn_Zombie::Create(void* pArg)
{
	CPivot_Turn_Zombie*			pInstance = { new CPivot_Turn_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPivot_Turn_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPivot_Turn_Zombie::Free()
{
	__super::Free();
}
