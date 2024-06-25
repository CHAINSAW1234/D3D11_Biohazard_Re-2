#include "stdafx.h"
#include "GameInstance.h"
#include "Is_Hit_Zombie.h"
#include "BlackBoard_Zombie.h"
#include "Player.h"
#include "Zombie.h"

CIs_Hit_Zombie::CIs_Hit_Zombie()
	: CDecorator_Node()
{

}

CIs_Hit_Zombie::CIs_Hit_Zombie(const CIs_Hit_Zombie& rhs)
	: CDecorator_Node{ rhs }
{
}

HRESULT CIs_Hit_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

_bool CIs_Hit_Zombie::Condition_Check()
{
	if (nullptr == m_pBlackBoard)
		return false;

	CZombie*		pZombie = { m_pBlackBoard->GetAI() };
	if (nullptr == pZombie)
		return false;

	COLLIDER_TYPE			eCurrentIntersectCollider = { pZombie->Get_Current_IntersectCollider() };
	if (COLLIDER_TYPE::_END == eCurrentIntersectCollider)
		return false;

	return true;
}

ZOMBIE_BODY_ANIM_TYPE CIs_Hit_Zombie::Get_AnimType()
{
	ZOMBIE_BODY_ANIM_TYPE		eType = { ZOMBIE_BODY_ANIM_TYPE::_END };
	if (nullptr == m_pBlackBoard)
		return eType;

	eType = m_pBlackBoard->Get_Current_AnimType(PLAYING_INDEX::INDEX_0);

	return eType;
}

CIs_Hit_Zombie* CIs_Hit_Zombie::Create(void* pArg)
{
	CIs_Hit_Zombie* pInstance = { new CIs_Hit_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CIs_Hit_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIs_Hit_Zombie::Free()
{
	__super::Free();
}

