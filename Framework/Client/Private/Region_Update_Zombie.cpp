#include "stdafx.h"
#include "GameInstance.h"
#include "Region_Update_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"
#include "Stun_Zombie.h"

CRegion_Update_Zombie::CRegion_Update_Zombie()
	: CTask_Node()
{
}

CRegion_Update_Zombie::CRegion_Update_Zombie(const CRegion_Update_Zombie& rhs)
{
}

HRESULT CRegion_Update_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CRegion_Update_Zombie::Enter()
{
}

_bool CRegion_Update_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion
}

void CRegion_Update_Zombie::Exit()
{
}

CRegion_Update_Zombie* CRegion_Update_Zombie::Create(void* pArg)
{
	CRegion_Update_Zombie* pInstance = { new CRegion_Update_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CRegion_Update_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRegion_Update_Zombie::Free()
{
	__super::Free();
}
