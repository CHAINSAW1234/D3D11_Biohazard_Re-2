#include "stdafx.h"
#include "GameInstance.h"
#include "Is_Can_Link_Pre_State_Zombie.h"
#include "BlackBoard_Zombie.h"
#include "Player.h"
#include "Zombie.h"

CIs_Can_Link_Pre_State_Zombie::CIs_Can_Link_Pre_State_Zombie()
	: CDecorator_Node{}
{
}

CIs_Can_Link_Pre_State_Zombie::CIs_Can_Link_Pre_State_Zombie(const CIs_Can_Link_Pre_State_Zombie& rhs)
	:CDecorator_Node{ rhs }
{
}

HRESULT CIs_Can_Link_Pre_State_Zombie::Initialize(list<MONSTER_STATE> CanLinkStates)
{
	if (true == CanLinkStates.empty())
		return E_FAIL;

	for (auto& eMonsterState : CanLinkStates)
	{
		m_CanLinkStates.emplace(eMonsterState);
	}

	return S_OK;
}

_bool CIs_Can_Link_Pre_State_Zombie::Condition_Check()
{
	if (nullptr == m_pBlackBoard)
		return false;

	_bool					isCanLink = { false };
	MONSTER_STATE			ePreState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };
	unordered_set<MONSTER_STATE>::iterator			iter = { m_CanLinkStates.find(ePreState) };
	
	isCanLink = iter != m_CanLinkStates.end();

	return isCanLink;
}

CIs_Can_Link_Pre_State_Zombie* CIs_Can_Link_Pre_State_Zombie::Create(list<MONSTER_STATE> CanLinkStates)
{
	CIs_Can_Link_Pre_State_Zombie* pInstance = { new CIs_Can_Link_Pre_State_Zombie() };

	if (FAILED(pInstance->Initialize(CanLinkStates)))
	{
		MSG_BOX(TEXT("Failed To Created : CIs_Can_Link_Pre_State_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIs_Can_Link_Pre_State_Zombie::Free()
{
	__super::Free();
}