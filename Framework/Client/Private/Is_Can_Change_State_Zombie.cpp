#include "stdafx.h"
#include "GameInstance.h"
#include "Is_Can_Change_State_Zombie.h"
#include "BlackBoard_Zombie.h"
#include "Player.h"
#include "Zombie.h"

CIs_Can_Change_State_Zombie::CIs_Can_Change_State_Zombie()
	: CDecorator_Node()
{

}

CIs_Can_Change_State_Zombie::CIs_Can_Change_State_Zombie(const CIs_Can_Change_State_Zombie& rhs)
	: CDecorator_Node{ rhs }
{
}

HRESULT CIs_Can_Change_State_Zombie::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		CAN_CHANGE_STATE_ZOMBIE_DESC*			pDesc = { static_cast<CAN_CHANGE_STATE_ZOMBIE_DESC*>(pArg) };

		list< ZOMBIE_BODY_ANIM_TYPE>&			NonBlockTypes = { pDesc->NonBlockTypes };
		for (auto& eNonBlockType : NonBlockTypes)
		{
			m_NonBlockTypes.emplace(eNonBlockType);
		}
	}

	return S_OK;
}

_bool CIs_Can_Change_State_Zombie::Condition_Check()
{
	if (nullptr == m_pBlackBoard)
		return false;

	unordered_set<ZOMBIE_BODY_ANIM_TYPE>::iterator			iterTurn = { m_NonBlockTypes.find(ZOMBIE_BODY_ANIM_TYPE::_TURN) };
	if (iterTurn != m_NonBlockTypes.end() && 
		Get_AnimType() == ZOMBIE_BODY_ANIM_TYPE::_TURN)
	{
		_int			iAnimIndex = { m_pBlackBoard->Get_Current_AnimIndex(CMonster::PART_BODY, PLAYING_INDEX::INDEX_0) };
		if (-1 == iAnimIndex)
			return false;

		_bool			isFinishTurn = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY)->isFinished(static_cast<_uint>(PLAYING_INDEX::INDEX_0)) };

		return isFinishTurn;
	}

	return true;
}

ZOMBIE_BODY_ANIM_TYPE CIs_Can_Change_State_Zombie::Get_AnimType()
{
	ZOMBIE_BODY_ANIM_TYPE		eType = { ZOMBIE_BODY_ANIM_TYPE::_END };
	if (nullptr == m_pBlackBoard)
		return eType;

	eType = m_pBlackBoard->Get_Current_AnimType(PLAYING_INDEX::INDEX_0);
	
	return eType;
}

CIs_Can_Change_State_Zombie* CIs_Can_Change_State_Zombie::Create(void* pArg)
{
	CIs_Can_Change_State_Zombie*			pInstance = { new CIs_Can_Change_State_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CIs_Can_Change_State_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIs_Can_Change_State_Zombie::Free()
{
	__super::Free();
}

