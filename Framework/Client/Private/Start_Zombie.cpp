#include "stdafx.h"
#include "GameInstance.h"
#include "Start_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"
#include "Stun_Zombie.h"

CStart_Zombie::CStart_Zombie()
	: CTask_Node()
{
}

CStart_Zombie::CStart_Zombie(const CStart_Zombie& rhs)
{
}

HRESULT CStart_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CStart_Zombie::Enter()
{
}

_bool CStart_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	return true;
}

void CStart_Zombie::Exit()
{
}

CStart_Zombie* CStart_Zombie::Create(void* pArg)
{
	CStart_Zombie*			pInstance = { new CStart_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CStart_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStart_Zombie::Free()
{
	__super::Free();
}
