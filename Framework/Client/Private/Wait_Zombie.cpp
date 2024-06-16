#include "stdafx.h"
#include "GameInstance.h"
#include "Wait_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"

CWait_Zombie::CWait_Zombie() 
	: CTask_Node()
{
}

CWait_Zombie::CWait_Zombie(const CWait_Zombie& rhs)
{
}

HRESULT CWait_Zombie::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CWait_Zombie::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CWait_Zombie::Execute()
{
	auto pAI = m_pBlackBoard->GetAI();
	pAI->SetState(MONSTER_STATE::MST_IDLE);
}

CWait_Zombie* CWait_Zombie::Create()
{
	CWait_Zombie* pInstance = new CWait_Zombie();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CWait_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWait_Zombie::Free()
{
	__super::Free();
}
