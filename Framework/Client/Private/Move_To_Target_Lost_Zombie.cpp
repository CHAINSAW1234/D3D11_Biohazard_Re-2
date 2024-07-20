#include "stdafx.h"
#include "GameInstance.h"
#include "Move_To_Target_Lost_Zombie.h"
#include "BlackBoard_Zombie.h"

#include "Zombie.h"
#include "Body_Zombie.h"

#include "Player.h"
#include "Window.h"
#include "Door.h"


CMove_To_Target_Lost_Zombie::CMove_To_Target_Lost_Zombie()
	: CTask_Node()
{
}

CMove_To_Target_Lost_Zombie::CMove_To_Target_Lost_Zombie(const CMove_To_Target_Lost_Zombie& rhs)
{
}

HRESULT CMove_To_Target_Lost_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CMove_To_Target_Lost_Zombie::Enter()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*				pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

#ifdef _DEBUG
	cout << "Enter Move To Tareget Lost" << endl;
#endif 
}

_bool CMove_To_Target_Lost_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	MONSTER_STATE			eMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };

	if (MONSTER_STATE::MST_WALK_LOST != eMonsterState)
	{
		if (false == m_pBlackBoard->Is_LookTarget())
			return false;

		CZombie::POSE_STATE			ePoseState = { m_pBlackBoard->Get_AI()->Get_PoseState() };
		CZombie::FACE_STATE			eFaceState = { m_pBlackBoard->Get_AI()->Get_FaceState() };
		if (CZombie::POSE_STATE::_CREEP != ePoseState)
			return false;

		if (CZombie::FACE_STATE::_DOWN != eFaceState)
			return false;

		m_eMoveLostAnimType = ZOMBIE_MOVE_LOST_TYPE::_END;
		m_isFinishedStartAnim = false;
		m_isHaveStartAnim = false;

		_bool				isBreak_L_Humerous = { m_pBlackBoard->Is_Break_L_Humerous() };
		_bool				isBreak_R_Humerous = { m_pBlackBoard->Is_Break_R_Humerous() };
		_bool				isBreak_L_Tabia = { m_pBlackBoard->Is_Break_L_Tabia() };
		_bool				isBreak_R_Tabia = { m_pBlackBoard->Is_Break_R_Tabia() };
		_bool				isBreak_L_Femur = { m_pBlackBoard->Is_Break_L_Femur() };
		_bool				isBreak_R_Femur = { m_pBlackBoard->Is_Break_R_Femur() };

		_bool				isNonBreakArms = { !(isBreak_L_Humerous || isBreak_R_Humerous) };
		_bool				isNonBreakLegs = { !(isBreak_L_Femur || isBreak_R_Femur) };

		if (true == isBreak_L_Humerous && true == isBreak_R_Humerous &&
			true == isBreak_L_Femur && true == isBreak_R_Femur)
		{
			m_eMoveLostAnimType = ZOMBIE_MOVE_LOST_TYPE::_J;
		}

		if (true == isBreak_L_Femur && true == isBreak_R_Femur)
		{
			if (true == isNonBreakArms)
				m_eMoveLostAnimType = ZOMBIE_MOVE_LOST_TYPE::_I;

			else if (true == isBreak_L_Humerous)
				m_eMoveLostAnimType = ZOMBIE_MOVE_LOST_TYPE::_F;
			else if (true == isBreak_R_Humerous)
				m_eMoveLostAnimType = ZOMBIE_MOVE_LOST_TYPE::_F_SYM;
			else
				return false;
		}

		else if (true == isBreak_L_Femur)
		{
			if(true == isNonBreakArms)
				m_eMoveLostAnimType = ZOMBIE_MOVE_LOST_TYPE::_C_SYM;
			else if (true == isBreak_L_Humerous && true == isBreak_R_Humerous)
				m_eMoveLostAnimType = ZOMBIE_MOVE_LOST_TYPE::_G;
			else if (true == isBreak_L_Humerous)
				m_eMoveLostAnimType = ZOMBIE_MOVE_LOST_TYPE::_D;
			else if (true == isBreak_R_Humerous)
				m_eMoveLostAnimType = ZOMBIE_MOVE_LOST_TYPE::_E;
			else
				return false;
		}

		else if (true == isBreak_R_Femur)
		{
			if (true == isNonBreakArms)
				m_eMoveLostAnimType = ZOMBIE_MOVE_LOST_TYPE::_C;
			else if (true == isBreak_L_Humerous && true == isBreak_R_Humerous)
				m_eMoveLostAnimType = ZOMBIE_MOVE_LOST_TYPE::_G_SYM;
			else if (true == isBreak_R_Humerous)
				m_eMoveLostAnimType = ZOMBIE_MOVE_LOST_TYPE::_D_SYM;
			else if (true == isBreak_L_Humerous)
				m_eMoveLostAnimType = ZOMBIE_MOVE_LOST_TYPE::_E_SYM;
			else
				return false;
		}

		else if (true == isNonBreakLegs)
		{
			if (true == isBreak_R_Tabia)
				m_eMoveLostAnimType = ZOMBIE_MOVE_LOST_TYPE::_B;
			else if (true == isBreak_L_Tabia)
				m_eMoveLostAnimType = ZOMBIE_MOVE_LOST_TYPE::_B_SYM;
			else if (true == isBreak_L_Humerous && true == isBreak_R_Humerous)
				m_eMoveLostAnimType = ZOMBIE_MOVE_LOST_TYPE::_H;
			else
				return false;
		}

		else if (true == isNonBreakArms)
		{
			if (true == isBreak_R_Tabia)
				m_eMoveLostAnimType = ZOMBIE_MOVE_LOST_TYPE::_B;
			else if (true == isBreak_L_Tabia)
				m_eMoveLostAnimType = ZOMBIE_MOVE_LOST_TYPE::_B_SYM;
			else
				return false;
		}




		if (ZOMBIE_MOVE_LOST_TYPE::_END == m_eMoveLostAnimType)
			return false;

		if (ZOMBIE_MOVE_LOST_TYPE::_D == m_eMoveLostAnimType || ZOMBIE_MOVE_LOST_TYPE::_D_SYM == m_eMoveLostAnimType ||
			ZOMBIE_MOVE_LOST_TYPE::_E == m_eMoveLostAnimType || ZOMBIE_MOVE_LOST_TYPE::_E_SYM == m_eMoveLostAnimType ||
			ZOMBIE_MOVE_LOST_TYPE::_F == m_eMoveLostAnimType || ZOMBIE_MOVE_LOST_TYPE::_F_SYM == m_eMoveLostAnimType ||
			ZOMBIE_MOVE_LOST_TYPE::_J == m_eMoveLostAnimType)
		{
			m_isHaveStartAnim = true;
		}

		Change_Animation();
	}

	else
	{
		if (true == m_isHaveStartAnim && false == m_isFinishedStartAnim)
		{
			CModel*			pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
			if (nullptr == pBody_Model)
				return false;

			if (true == pBody_Model->isFinished(static_cast<_uint>(m_eBase_PlayingIndex)))
			{
				m_isFinishedStartAnim = true;
				Change_Animation();
			}
		}

		if (false == m_pBlackBoard->Is_LookTarget())
			return false;
	}	

	m_pBlackBoard->Organize_PreState(this);

	m_fRemainSoundTime -= fTimeDelta;
	if (0.f > m_fRemainSoundTime)
	{
		m_fRemainSoundTime = m_pGameInstance->GetRandom_Real(ZOMBIE_MOVE_SOUND_MIN_REMAIN, ZOMBIE_MOVE_SOUND_MAX_REMAIN);
		m_pBlackBoard->Get_AI()->Play_Random_Move_Sound();
	}

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_WALK_LOST);
		
	return true;
}

void CMove_To_Target_Lost_Zombie::Exit()
{
	if (nullptr == m_pBlackBoard)
		return;

	m_eMoveLostAnimType = ZOMBIE_MOVE_LOST_TYPE::_END;
}

void CMove_To_Target_Lost_Zombie::Change_Animation()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*			pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBody_Model)
		return;

	_int			iResultAnimIndex = { -1 };
	_bool			isLoop = { false };

	if (ZOMBIE_MOVE_LOST_TYPE::_A == m_eMoveLostAnimType)
	{
		iResultAnimIndex = { static_cast<_int>(ANIM_LOST_WALK::_LOOP_A) };
		isLoop = true;
	}

	else if (ZOMBIE_MOVE_LOST_TYPE::_B == m_eMoveLostAnimType)
	{
		iResultAnimIndex = { static_cast<_int>(ANIM_LOST_WALK::_LOOP_B) };
		isLoop = true;
	}

	else if (ZOMBIE_MOVE_LOST_TYPE::_B_SYM == m_eMoveLostAnimType)
	{
		iResultAnimIndex = { static_cast<_int>(ANIM_LOST_WALK::_LOOP_B_SYM) };
		isLoop = true;
	}

	else if (ZOMBIE_MOVE_LOST_TYPE::_C == m_eMoveLostAnimType)
	{
		iResultAnimIndex = { static_cast<_int>(ANIM_LOST_WALK::_LOOP_C) };
		isLoop = true;
	}

	else if (ZOMBIE_MOVE_LOST_TYPE::_C_SYM == m_eMoveLostAnimType)
	{
		iResultAnimIndex = { static_cast<_int>(ANIM_LOST_WALK::_LOOP_C_SYM) };
		isLoop = true;
	}


	else if (ZOMBIE_MOVE_LOST_TYPE::_D == m_eMoveLostAnimType)
	{
		if (false == m_isFinishedStartAnim)
		{
			iResultAnimIndex = { static_cast<_int>(ANIM_LOST_WALK::_START_D) };
			isLoop = false;
		}
		else
		{
			iResultAnimIndex = { static_cast<_int>(ANIM_LOST_WALK::_LOOP_D) };
			isLoop = true;
		}
	}

	else if (ZOMBIE_MOVE_LOST_TYPE::_D_SYM == m_eMoveLostAnimType)
	{
		if (false == m_isFinishedStartAnim)
		{
			iResultAnimIndex = { static_cast<_int>(ANIM_LOST_WALK::_START_D_SYM) };
			isLoop = false;
		}
		else
		{
			iResultAnimIndex = { static_cast<_int>(ANIM_LOST_WALK::_LOOP_D_SYM) };
			isLoop = true;
		}
	}


	else if (ZOMBIE_MOVE_LOST_TYPE::_E == m_eMoveLostAnimType)
	{
		if (false == m_isFinishedStartAnim)
		{
			iResultAnimIndex = { static_cast<_int>(ANIM_LOST_WALK::_START_E) };
			isLoop = false;
		}
		else
		{
			iResultAnimIndex = { static_cast<_int>(ANIM_LOST_WALK::_LOOP_E) };
			isLoop = true;
		}
	}

	else if (ZOMBIE_MOVE_LOST_TYPE::_E_SYM == m_eMoveLostAnimType)
	{
		if (false == m_isFinishedStartAnim)
		{
			iResultAnimIndex = { static_cast<_int>(ANIM_LOST_WALK::_START_E_SYM) };
			isLoop = false;
		}
		else
		{
			iResultAnimIndex = { static_cast<_int>(ANIM_LOST_WALK::_LOOP_E_SYM) };
			isLoop = true;
		}
	}


	else if (ZOMBIE_MOVE_LOST_TYPE::_F == m_eMoveLostAnimType)
	{
		if (false == m_isFinishedStartAnim)
		{
			iResultAnimIndex = { static_cast<_int>(ANIM_LOST_WALK::_START_F) };
			isLoop = false;
		}
		else
		{
			iResultAnimIndex = { static_cast<_int>(ANIM_LOST_WALK::_LOOP_F) };
			isLoop = true;
		}
	}

	else if (ZOMBIE_MOVE_LOST_TYPE::_F_SYM == m_eMoveLostAnimType)
	{
		if (false == m_isFinishedStartAnim)
		{
			iResultAnimIndex = { static_cast<_int>(ANIM_LOST_WALK::_START_F_SYM) };
			isLoop = false;
		}
		else
		{
			iResultAnimIndex = { static_cast<_int>(ANIM_LOST_WALK::_LOOP_F_SYM) };
			isLoop = true;
		}
	}

	else if (ZOMBIE_MOVE_LOST_TYPE::_G == m_eMoveLostAnimType)
	{
		iResultAnimIndex = { static_cast<_int>(ANIM_LOST_WALK::_LOOP_G) };
		isLoop = true;
	}

	else if (ZOMBIE_MOVE_LOST_TYPE::_G_SYM == m_eMoveLostAnimType)
	{
		iResultAnimIndex = { static_cast<_int>(ANIM_LOST_WALK::_LOOP_G_SYM) };
		isLoop = true;
	}

	else if (ZOMBIE_MOVE_LOST_TYPE::_H == m_eMoveLostAnimType)
	{
		iResultAnimIndex = { static_cast<_int>(ANIM_LOST_WALK::_LOOP_H) };
		isLoop = true;
	}

	else if (ZOMBIE_MOVE_LOST_TYPE::_I == m_eMoveLostAnimType)
	{
		iResultAnimIndex = { static_cast<_int>(ANIM_LOST_WALK::_LOOP_I) };
		isLoop = true;
	}


	else if (ZOMBIE_MOVE_LOST_TYPE::_J == m_eMoveLostAnimType)
	{
		if (false == m_isFinishedStartAnim)
		{
			iResultAnimIndex = { static_cast<_int>(ANIM_LOST_WALK::_START_J) };
			isLoop = false;
		}
		else
		{
			iResultAnimIndex = { static_cast<_int>(ANIM_LOST_WALK::_LOOP_J) };
			isLoop = true;
		}
	}


	if (-1 == iResultAnimIndex)
		return;


	pBody_Model->Change_Animation(static_cast<_uint>(m_eBase_PlayingIndex), m_strAnimLayerTag, iResultAnimIndex);
	pBody_Model->Set_Loop(static_cast<_uint>(m_eBase_PlayingIndex), isLoop);
}

CMove_To_Target_Lost_Zombie* CMove_To_Target_Lost_Zombie::Create(void* pArg)
{
	CMove_To_Target_Lost_Zombie* pInstance = { new CMove_To_Target_Lost_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CMove_To_Target_Lost_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMove_To_Target_Lost_Zombie::Free()
{
	__super::Free();
}
