#include "stdafx.h"
#include "GameInstance.h"
#include "Bite_Lost_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"

CBite_Lost_Zombie::CBite_Lost_Zombie()
	: CTask_Node()
{
}

CBite_Lost_Zombie::CBite_Lost_Zombie(const CBite_Lost_Zombie& rhs)
{
}

HRESULT CBite_Lost_Zombie::Initialize(void* pArg)
{
	m_fTotalLinearTime_HalfMatrix = 0.2f;

	return S_OK;
}

void CBite_Lost_Zombie::Enter()
{

	m_isSendMSG_To_Player = false;
#ifdef _DEBUG

	cout << "Enter Lost Bite " << endl;

#endif 
}

_bool CBite_Lost_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	MONSTER_STATE			eMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };
	if (MONSTER_STATE::MST_BITE_LOST != eMonsterState)
	{
		CModel*				pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
		if (nullptr == pBody_Model)
			return false;

		if (false == m_pBlackBoard->Is_LookTarget())
			return false;

		_float				fDistanceToPlayer = { };
		if (false == m_pBlackBoard->Compute_Distance_To_Player_World(&fDistanceToPlayer))
			return false;

		if (fDistanceToPlayer > m_pBlackBoard->Get_ZombieStatus_Ptr()->fBiteRange)
			return false;

		_bool				isBreak_L_Arm = { m_pBlackBoard->Is_Break_L_Arm() };
		_bool				isBreak_R_Arm = { m_pBlackBoard->Is_Break_R_Arm() };

		_bool				isBreakArms = { isBreak_L_Arm && isBreak_R_Arm };

		if (true == isBreakArms)
		{
			CZombie::POSE_STATE		ePoseState = { m_pBlackBoard->Get_AI()->Get_PoseState() };

			if (CZombie::POSE_STATE::_UP == ePoseState)
			{
				m_eBite_Lost_Anim_Type = ZOMBIE_BITE_LOST_ANIM_TYPE::_DIRECT_1;
			}

			else if (CZombie::POSE_STATE::_CREEP == ePoseState)
			{
				CZombie::FACE_STATE			eFaceState = { m_pBlackBoard->Get_AI()->Get_FaceState() };
				if (CZombie::FACE_STATE::_DOWN != eFaceState)
					return false;

				m_eBite_Lost_Anim_Type = ZOMBIE_BITE_LOST_ANIM_TYPE::_DIRECT_2;
			}

			else
				return false;
		}

		else
			return false;


		Change_Animation();
	}

	else
	{
		CModel*			pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
		if (nullptr == pBody_Model)
			return false;

		if (true == pBody_Model->isFinished(static_cast<_uint>(m_ePlayingIndex)))
		{
			if (ZOMBIE_BITE_LOST_ANIM_TYPE::_DIRECT_1 == m_eBite_Lost_Anim_Type)
			{
				m_eBite_Lost_Anim_Type = ZOMBIE_BITE_LOST_ANIM_TYPE::_DIRECT1_EM;

				Change_Animation();
			}
			else
				return false;
		}
	}

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_BITE_LOST);


	if (false == m_isSendMSG_To_Player)
	{
		m_isSendMSG_To_Player = true;
	}

	return true;
}

void CBite_Lost_Zombie::Exit()
{
}

void CBite_Lost_Zombie::Change_Animation()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*				pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBody_Model)
		return;

	_int				iResultAnimIndex = { -1 };
	wstring				strAnimLayerTag = { TEXT("") };

	if (ZOMBIE_BITE_LOST_ANIM_TYPE::_DIRECT_1 == m_eBite_Lost_Anim_Type)
	{
		iResultAnimIndex = static_cast<_int>(ANIM_LOST_BITE::_DIRECT1);
		strAnimLayerTag = m_strLostBiteAnimLayerTag;
	}

	else if (ZOMBIE_BITE_LOST_ANIM_TYPE::_DIRECT1_EM == m_eBite_Lost_Anim_Type)
	{
		iResultAnimIndex = static_cast<_int>(ANIM_BITE_ETC::_DIRECT);
		strAnimLayerTag = m_strBiteETCAnimLayerTag;
	}

	else if (ZOMBIE_BITE_LOST_ANIM_TYPE::_DIRECT_2 == m_eBite_Lost_Anim_Type)
	{
		iResultAnimIndex = static_cast<_int>(ANIM_LOST_BITE::_DIRECT2);
		strAnimLayerTag = m_strLostBiteAnimLayerTag;
	}

	else
		return;

	if (-1 == iResultAnimIndex)
		return;

	pBody_Model->Change_Animation(static_cast<_uint>(m_ePlayingIndex), strAnimLayerTag, iResultAnimIndex);
	pBody_Model->Set_Loop(static_cast<_uint>(m_ePlayingIndex), false);
}

CBite_Lost_Zombie* CBite_Lost_Zombie::Create(void* pArg)
{
	CBite_Lost_Zombie* pInstance = { new CBite_Lost_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBite_Lost_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBite_Lost_Zombie::Free()
{
	__super::Free();
}