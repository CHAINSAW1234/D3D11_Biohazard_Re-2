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
	CModel*			pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBody_Model)
		return;

	pBody_Model->Set_TotalLinearInterpolation(0.3f);
	pBody_Model->Set_Loop(static_cast<_uint>(m_eBasePlayingIndex), false);
	pBody_Model->Set_Loop(static_cast<_uint>(m_eBlendPlayingIndex), false);

#ifdef _DEBUG

	cout << "Enter Pivot Turn" << endl;

#endif 
}

_bool CPivot_Turn_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	MONSTER_STATE			eMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };
	_bool					isEntry = { eMonsterState != MST_TURN };
	if (true == isEntry)
	{
		CZombie::POSE_STATE			ePoseState = { m_pBlackBoard->Get_AI()->Get_PoseState() };
		if (CZombie::POSE_STATE::_UP != ePoseState)
			return false;

		if (false == m_pBlackBoard->Is_LookTarget())
			return false;

		Change_Animation(fTimeDelta);
	}

	else
	{
		if (true == Is_StateFinished())
			return false;
	}

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_TURN);

	return true;
}

void CPivot_Turn_Zombie::Exit()
{
	CModel*				pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBody_Model)
		return;

	pBody_Model->Set_BlendWeight(static_cast<_uint>(m_eBasePlayingIndex), 1.f, 0.3f);
	pBody_Model->Set_BlendWeight(static_cast<_uint>(m_eBlendPlayingIndex), 0.f, 0.3f);
}

void CPivot_Turn_Zombie::Change_Animation(_float fTimeDelta)
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*				pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_int			iBaseAnimIndex = { -1 };
	_int			iBlendAnimIndex = { -1 };

	_float3			vDirectionToPlayerLocalFloat3;
	if (false == m_pBlackBoard->Compute_Direction_To_Player_Local(&vDirectionToPlayerLocalFloat3))
		return;

	_vector			vDirectionToPlayerLocal = { XMLoadFloat3(&vDirectionToPlayerLocalFloat3) };
	_vector			vDirectionToPlayerLocalXZPlane = { XMVector3Normalize(XMVectorSetY(vDirectionToPlayerLocal, 0.f)) };		//	회전량을 xz평면상에서만 고려하기위함
	_vector			vAILookLocal = { XMVectorSet(0.f, 0.f, 1.f, 0.f) };

	_bool			isRight = { XMVectorGetX(vDirectionToPlayerLocalXZPlane) > 0.f };

	_float			fDot = { XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDirectionToPlayerLocal), XMVector3Normalize(vAILookLocal))) };
	_float			fAngleToTarget = { acosf(fDot) };

	MOTION_TYPE		eCurrentMotionType = { static_cast<MOTION_TYPE>(m_pBlackBoard->Get_Current_MotionType_Body()) };

	if (true == isRight)
	{
			if (MOTION_TYPE::MOTION_A == eCurrentMotionType)
				iBaseAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R90_A);
			else if (MOTION_TYPE::MOTION_B == eCurrentMotionType)
				iBaseAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R90_B);
			else if (MOTION_TYPE::MOTION_C == eCurrentMotionType)
				iBaseAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R90_C);
			else if (MOTION_TYPE::MOTION_D == eCurrentMotionType)
				iBaseAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R90_D);
			else if (MOTION_TYPE::MOTION_E == eCurrentMotionType)
				iBaseAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R90_E);
			else if (MOTION_TYPE::MOTION_F == eCurrentMotionType)
				iBaseAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R90_F);
			else
				MSG_BOX(TEXT("이럴리없어 좀비 담당자 호출 ㄱ"));

			if (MOTION_TYPE::MOTION_A == eCurrentMotionType)
				iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R180_A);
			else if (MOTION_TYPE::MOTION_B == eCurrentMotionType)
				iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R180_B);
			else if (MOTION_TYPE::MOTION_C == eCurrentMotionType)
				iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R180_C);
			else if (MOTION_TYPE::MOTION_D == eCurrentMotionType)
				iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R180_D);
			else if (MOTION_TYPE::MOTION_E == eCurrentMotionType)
				iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R180_E);
			else if (MOTION_TYPE::MOTION_F == eCurrentMotionType)
				iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_R180_F);
			else
				MSG_BOX(TEXT("이럴리없어 좀비 담당자 호출 ㄱ"));
	}

	else 
	{
		if (MOTION_TYPE::MOTION_A == eCurrentMotionType)
			iBaseAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L90_A);
		else if (MOTION_TYPE::MOTION_B == eCurrentMotionType)
			iBaseAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L90_B);
		else if (MOTION_TYPE::MOTION_C == eCurrentMotionType)
			iBaseAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L90_C);
		else if (MOTION_TYPE::MOTION_D == eCurrentMotionType)
			iBaseAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L90_D);
		else if (MOTION_TYPE::MOTION_E == eCurrentMotionType)
			iBaseAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L90_E);
		else if (MOTION_TYPE::MOTION_F == eCurrentMotionType)
			iBaseAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L90_F);
		else
			MSG_BOX(TEXT("이럴리없어 좀비 담당자 호출 ㄱ"));

		if (MOTION_TYPE::MOTION_A == eCurrentMotionType)
			iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L180_A);
		else if (MOTION_TYPE::MOTION_B == eCurrentMotionType)
			iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L180_B);
		else if (MOTION_TYPE::MOTION_C == eCurrentMotionType)
			iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L180_C);
		else if (MOTION_TYPE::MOTION_D == eCurrentMotionType)
			iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L180_D);
		else if (MOTION_TYPE::MOTION_E == eCurrentMotionType)
			iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L180_E);
		else if (MOTION_TYPE::MOTION_F == eCurrentMotionType)
			iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_PIVOT_TURN::_L180_F);
		else
			MSG_BOX(TEXT("이럴리없어 좀비 담당자 호출 ㄱ"));
	}

	if (-1 == iBaseAnimIndex || -1 == iBlendAnimIndex)
		return;

	_float			fBlendRatio = { fminf(fAngleToTarget - XMConvertToRadians(90.f), XMConvertToRadians(0.f)) };

	pBodyModel->Change_Animation(static_cast<_uint>(m_eBasePlayingIndex), TEXT("Ordinary_PivotTurn"), iBaseAnimIndex);
	pBodyModel->Change_Animation(static_cast<_uint>(m_eBlendPlayingIndex), TEXT("Ordinary_PivotTurn"), iBlendAnimIndex);
	pBodyModel->Set_BoneLayer_PlayingInfo(static_cast<_uint>(m_eBasePlayingIndex), m_strBoneLayerTag);
	pBodyModel->Set_BoneLayer_PlayingInfo(static_cast<_uint>(m_eBlendPlayingIndex), m_strBoneLayerTag);
	pBodyModel->Set_BlendWeight(static_cast<_uint>(m_eBasePlayingIndex), 1.f - fBlendRatio);
	pBodyModel->Set_BlendWeight(static_cast<_uint>(m_eBlendPlayingIndex), fBlendRatio);
}

_bool CPivot_Turn_Zombie::Is_StateFinished()
{
	_bool					isFinished = { false };

	CModel*					pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBody_Model)
		return isFinished;

	isFinished = pBody_Model->isFinished(static_cast<_uint>(m_eBasePlayingIndex));

	return isFinished;
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
