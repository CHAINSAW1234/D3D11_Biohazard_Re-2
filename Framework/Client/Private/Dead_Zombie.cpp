#include "stdafx.h"
#include "GameInstance.h"
#include "Dead_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CDead_Zombie::CDead_Zombie()
	: CTask_Node()
{
}

CDead_Zombie::CDead_Zombie(const CDead_Zombie& rhs)
{
}

HRESULT CDead_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CDead_Zombie::Enter()
{

#ifdef _DEBUG
	cout << "Enter Dead" << endl;
#endif 
}

_bool CDead_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	if (true == m_isEnd)
		return false;

	ZOMBIE_START_TYPE			eStartType = { m_pBlackBoard->Get_AI()->Get_StartType() };
	if (static_cast<_int>(ZOMBIE_START_TYPE::_DEAD_FAKE1) > static_cast<_int>(eStartType) ||
		static_cast<_int>(ZOMBIE_START_TYPE::_DEAD_FAKE4) < static_cast<_int>(eStartType))
		return false;

	CModel*						pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBody_Model)
		return false;

	MONSTER_STATE				eMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };
	if (MONSTER_STATE::MST_START_DEAD != eMonsterState)
	{
		HIT_TYPE			eHitType = { m_pBlackBoard->Get_AI()->Get_Current_HitType() };
		_bool				isLookPlayer = { m_pBlackBoard->Is_LookTarget() };

		if (false == isLookPlayer && HIT_TYPE::HIT_END == eHitType)
			return false;

		if (HIT_TYPE::HIT_END != eHitType)
		{
			m_eDeadAnimState = ZOMBIE_DEAD_ANIM_STATE::_HIT;
		}

		else if (true == isLookPlayer)
		{
			m_eDeadAnimState = ZOMBIE_DEAD_ANIM_STATE::_LOOK_PLAYER;
		}

		if(ZOMBIE_START_TYPE::_DEAD_FAKE1 == eStartType)
			m_eDeadAnimType = ZOMBIE_DEAD_ANIM_TYPE::_1;
		else if (ZOMBIE_START_TYPE::_DEAD_FAKE2 == eStartType)
			m_eDeadAnimType = ZOMBIE_DEAD_ANIM_TYPE::_2;
		else if (ZOMBIE_START_TYPE::_DEAD_FAKE3 == eStartType)
			m_eDeadAnimType = ZOMBIE_DEAD_ANIM_TYPE::_3;
		else if (ZOMBIE_START_TYPE::_DEAD_FAKE4 == eStartType)
			m_eDeadAnimType = ZOMBIE_DEAD_ANIM_TYPE::_4;

	}
	
	else
	{
		if (true == pBody_Model->isFinished(static_cast<_uint>(m_eBasePlayingIndex)))
		{
			m_pBlackBoard->Get_AI()->Set_ManualMove(false);
			m_isEnd = true;
			return true;
		}
	}

	if (m_pBlackBoard->Get_AI())
		m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_START_DEAD);

	return true;
}

void CDead_Zombie::Exit()
{
}

CDead_Zombie* CDead_Zombie::Create(void* pArg)
{
	CDead_Zombie* pInstance = { new CDead_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CDead_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDead_Zombie::Free()
{
	__super::Free();
}
