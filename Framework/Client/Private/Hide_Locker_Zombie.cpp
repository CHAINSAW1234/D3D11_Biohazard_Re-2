#include "stdafx.h"
#include "GameInstance.h"
#include "Hide_Locker_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"
#include "Cabinet.h"

CHide_Locker_Zombie::CHide_Locker_Zombie()
	: CTask_Node()
{
}

CHide_Locker_Zombie::CHide_Locker_Zombie(const CHide_Locker_Zombie& rhs)
{
}

HRESULT CHide_Locker_Zombie::Initialize(void* pArg)
{
	m_fTotal_Wait_Time = 1.f;

	return S_OK;
}

void CHide_Locker_Zombie::Enter()
{

#ifdef _DEBUG
	cout << "Enter Hide Locker" << endl;
#endif 
}

_bool CHide_Locker_Zombie::Execute(_float fTimeDelta)
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
	if (ZOMBIE_START_TYPE::_HIDE_LOCKER != eStartType)
		return false;

	CModel*						pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBody_Model)
		return false;

	MONSTER_STATE				eMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };
	if (MONSTER_STATE::MST_START_HIDE_LOCKER != eMonsterState)
	{
		if (nullptr == m_pTarget_Cabinet)
		{
			m_pTarget_Cabinet = static_cast<CCabinet*>(m_pBlackBoard->Get_Target_Object());
			Safe_AddRef(m_pTarget_Cabinet);
		}
	}

	else
	{
		if (true == m_isStart && true == pBody_Model->isFinished(static_cast<_uint>(m_eBasePlayingIndex)))
		{
			m_pBlackBoard->Get_AI()->SetRagdoll_StartPose();
			m_pBlackBoard->Get_AI()->Set_ManualMove(false);

			m_isEnd = true;
			return true;
		}
	}



	if(m_pBlackBoard->Get_AI())
		m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_START_HIDE_LOCKER);

	if (false == m_pTarget_Cabinet->Is_Opened())
		return true;

	m_fAcc_Wait_Time += fTimeDelta;
	if (m_fAcc_Wait_Time >= m_fTotal_Wait_Time)
	{
		pBody_Model->Set_TotalLinearInterpolation(0.f);
		pBody_Model->Change_Animation(static_cast<_uint>(m_eBasePlayingIndex), m_strAnimLayerTag, static_cast<_int>(ANIM_DEAD_HIDE_LOCKER::_FINISH));
		m_isStart = true;
	}

	return true;
}

void CHide_Locker_Zombie::Exit()
{
}

CHide_Locker_Zombie* CHide_Locker_Zombie::Create(void* pArg)
{
	CHide_Locker_Zombie* pInstance = { new CHide_Locker_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHide_Locker_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHide_Locker_Zombie::Free()
{
	__super::Free();

	Safe_Release(m_pTarget_Cabinet);
}
