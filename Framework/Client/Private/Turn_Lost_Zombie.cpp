#include "stdafx.h"
#include "GameInstance.h"
#include "Turn_Lost_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"

#include "Body_Zombie.h"

CTurn_Lost_Zombie::CTurn_Lost_Zombie()
	: CTask_Node{}
{
}

CTurn_Lost_Zombie::CTurn_Lost_Zombie(const CTurn_Lost_Zombie& rhs)
	: CTask_Node{ rhs }
{
}

HRESULT CTurn_Lost_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CTurn_Lost_Zombie::Enter()
{
	CModel*			pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBody_Model)
		return;

	pBody_Model->Set_TotalLinearInterpolation(0.8f);

#ifdef _DEBUG
	cout << "Enter Turn Lost" << endl;
#endif 
}

_bool CTurn_Lost_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	MONSTER_STATE			eMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };

	if (MONSTER_STATE::MST_TURN != eMonsterState)
	{
		CZombie::POSE_STATE			ePoseState = { m_pBlackBoard->Get_AI()->Get_PoseState() };
		if (CZombie::POSE_STATE::_CREEP != ePoseState)
			return false;

		if (false == m_pBlackBoard->Is_LookTarget())
			return false;

		_float3			vDirectionToPlayerLocalFloat3 = {};
		if (false == m_pBlackBoard->Compute_Direction_To_Player_Local(&vDirectionToPlayerLocalFloat3))
			return false;
		
		_vector				vDirectionToPlayerLocalXZPlane = { XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&vDirectionToPlayerLocalFloat3), 0.f)) };
		_vector				vAbsoluteLookDir = { XMVectorSet(0.f, 0.f, 1.f, 0.f) };

		_float				fDot = { XMVectorGetX(XMVector3Dot(vDirectionToPlayerLocalXZPlane, vAbsoluteLookDir)) };
		_float				fAngle = { acosf(fDot) };

		if (fAngle < XMConvertToRadians(30.f))
			return false;

		m_isRight = XMVectorGetX(vDirectionToPlayerLocalXZPlane) > 0.f;

		_bool				isBreak_L_Humerous = { m_pBlackBoard->Is_Break_L_Humerous() };
		_bool				isBreak_R_Humerous = { m_pBlackBoard->Is_Break_R_Humerous() };
		_bool				isBreak_L_Femur = { m_pBlackBoard->Is_Break_L_Femur() };
		_bool				isBreak_R_Femur = { m_pBlackBoard->Is_Break_R_Femur() };

		_bool				isNonBreakArms = { !(isBreak_L_Humerous || isBreak_R_Humerous) };
		_bool				isNonBreakLegs = { !(isBreak_L_Femur || isBreak_R_Femur) };

		if (true == isNonBreakArms && true == isNonBreakArms)
			m_eTurnAnimType = ZOMBIE_TURN_LOST_TYPE::_A;

		else if (true == isBreak_L_Femur && true == isBreak_R_Femur &&
			true == isBreak_L_Humerous && true == isBreak_R_Humerous)
		{
			m_eTurnAnimType = ZOMBIE_TURN_LOST_TYPE::_J;
		}

		else if (true == isNonBreakArms)
		{
			if (true == isBreak_L_Femur && true == isBreak_R_Femur)
				m_eTurnAnimType = ZOMBIE_TURN_LOST_TYPE::_I;
			else if (true == isBreak_L_Femur)
				m_eTurnAnimType = ZOMBIE_TURN_LOST_TYPE::_C;
			else if (true == isBreak_R_Femur)
				m_eTurnAnimType = ZOMBIE_TURN_LOST_TYPE::_C_SYM;
			else
				return false;
		}

		else if(true == isNonBreakLegs)
		{
			if (true == isBreak_L_Humerous && true == isBreak_R_Humerous)
				m_eTurnAnimType = ZOMBIE_TURN_LOST_TYPE::_H;
			else
				return false;
		}

		else if (true == isBreak_L_Femur && true == isBreak_R_Femur)
		{
			if (true == isBreak_L_Humerous)
				m_eTurnAnimType = ZOMBIE_TURN_LOST_TYPE::_F;
			else if (true == isBreak_R_Humerous)
				m_eTurnAnimType = ZOMBIE_TURN_LOST_TYPE::_F_SYM;
			else
				return false;
		}

		else if (true == isBreak_L_Femur)
		{
			if (true == isBreak_L_Humerous && true == isBreak_R_Humerous)
				m_eTurnAnimType = ZOMBIE_TURN_LOST_TYPE::_G;
			else if (true == isBreak_L_Humerous)
				m_eTurnAnimType = ZOMBIE_TURN_LOST_TYPE::_D;
			else if (true == isBreak_R_Humerous)
				m_eTurnAnimType = ZOMBIE_TURN_LOST_TYPE::_E;
			else
				return false;
		}

		else if (true == isBreak_R_Femur)
		{
			if (true == isBreak_L_Humerous && true == isBreak_R_Humerous)
				m_eTurnAnimType = ZOMBIE_TURN_LOST_TYPE::_G_SYM;
			else if (true == isBreak_L_Humerous)
				m_eTurnAnimType = ZOMBIE_TURN_LOST_TYPE::_E_SYM;
			else if (true == isBreak_R_Humerous)
				m_eTurnAnimType = ZOMBIE_TURN_LOST_TYPE::_D_SYM;
			else
				return false;
		}

		Change_Animation(fTimeDelta);
	}

	else
	{
		_float3			vDirectionToPlayerLocalFloat3 = {};
		if (false == m_pBlackBoard->Compute_Direction_To_Player_Local(&vDirectionToPlayerLocalFloat3))
			return false;

		_vector				vDirectionToPlayerLocalXZPlane = { XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&vDirectionToPlayerLocalFloat3), 0.f)) };
		_vector				vAbsoluteLookDir = { XMVectorSet(0.f, 0.f, 1.f, 0.f) };

		_float				fDot = { XMVectorGetX(XMVector3Dot(vDirectionToPlayerLocalXZPlane, vAbsoluteLookDir)) };
		_float				fAngle = { acosf(fDot) };

		if (fAngle < XMConvertToRadians(10.f))
			return false;
	}

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_TURN);

	return true;
}

void CTurn_Lost_Zombie::Exit()
{
}

void CTurn_Lost_Zombie::Change_Animation(_float fTimeDelta)
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_int			iResultAnimIndex = { -1 };

	if (ZOMBIE_TURN_LOST_TYPE::_A == m_eTurnAnimType)
	{
		if (true == m_isRight)
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_R_A_SYM);
		else
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_L_A);
	}

	else if (ZOMBIE_TURN_LOST_TYPE::_C  == m_eTurnAnimType)
	{
		if (true == m_isRight)
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_R_C);
		else
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_L_C);
	}

	else if (ZOMBIE_TURN_LOST_TYPE::_C_SYM == m_eTurnAnimType)
	{
		if (true == m_isRight)
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_R_C_SYM);
		else
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_L_C_SYM);
	}

	else if (ZOMBIE_TURN_LOST_TYPE::_D == m_eTurnAnimType)
	{
		if (true == m_isRight)
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_R_D);
		else
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_L_D);
	}

	else if (ZOMBIE_TURN_LOST_TYPE::_D_SYM == m_eTurnAnimType)
	{
		if (true == m_isRight)
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_R_D_SYM);
		else
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_L_D_SYM);
	}

	else if (ZOMBIE_TURN_LOST_TYPE::_E == m_eTurnAnimType)
	{
		if (true == m_isRight)
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_R_E);
		else
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_L_E);
	}

	else if (ZOMBIE_TURN_LOST_TYPE::_E_SYM == m_eTurnAnimType)
	{
		if (true == m_isRight)
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_R_E_SYM);
		else
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_L_E_SYM);
	}

	else if (ZOMBIE_TURN_LOST_TYPE::_F == m_eTurnAnimType)
	{
		if (true == m_isRight)
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_R_F);
		else
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_L_F);
	}

	else if (ZOMBIE_TURN_LOST_TYPE::_F_SYM == m_eTurnAnimType)
	{
		if (true == m_isRight)
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_R_F_SYM);
		else
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_L_F_SYM);
	}

	else if (ZOMBIE_TURN_LOST_TYPE::_G == m_eTurnAnimType)
	{
		if (true == m_isRight)
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_R_G);
		else
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_L_G);
	}

	else if (ZOMBIE_TURN_LOST_TYPE::_G_SYM == m_eTurnAnimType)
	{
		if (true == m_isRight)
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_R_G_SYM);
		else
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_L_G_SYM);
	}

	else if (ZOMBIE_TURN_LOST_TYPE::_H == m_eTurnAnimType)
	{
		if (true == m_isRight)
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_R_H);
		else
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_L_H);
	}

	else if (ZOMBIE_TURN_LOST_TYPE::_I == m_eTurnAnimType)
	{
		if (true == m_isRight)
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_R_I);
		else
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_L_I);
	}

	else if (ZOMBIE_TURN_LOST_TYPE::_J == m_eTurnAnimType)
	{
		if (true == m_isRight)
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_R_J);
		else
			iResultAnimIndex = static_cast<_int>(ANIM_LOST_TURN::_LOOP_L_J);
	}

	if (-1 == iResultAnimIndex)
		return;

	pBodyModel->Change_Animation(static_cast<_uint>(m_eBasePlayingIndex), m_strAnimLayerTag, iResultAnimIndex);
	pBodyModel->Set_Loop(static_cast<_uint>(m_eBasePlayingIndex), true);
}

CTurn_Lost_Zombie* CTurn_Lost_Zombie::Create(void* pArg)
{
	CTurn_Lost_Zombie* pInstance = { new CTurn_Lost_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTurn_Lost_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTurn_Lost_Zombie::Free()
{
	__super::Free();
}
