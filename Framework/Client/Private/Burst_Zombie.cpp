#include "stdafx.h"
#include "GameInstance.h"
#include "Burst_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CBurst_Zombie::CBurst_Zombie()
	: CTask_Node()
{
}

CBurst_Zombie::CBurst_Zombie(const CBurst_Zombie& rhs)
{
}

HRESULT CBurst_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CBurst_Zombie::Enter()
{

}

_bool CBurst_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->GetAI();
	pAI->SetState(MONSTER_STATE::MST_DAMAGE);

	Change_Animation();

	return true;
}

void CBurst_Zombie::Exit()
{
}

void CBurst_Zombie::Change_Animation()
{
	if (nullptr == m_pBlackBoard)
		return;
}

CBurst_Zombie* CBurst_Zombie::Create(void* pArg)
{
	CBurst_Zombie* pInstance = { new CBurst_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBurst_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBurst_Zombie::Free()
{
	__super::Free();
}
