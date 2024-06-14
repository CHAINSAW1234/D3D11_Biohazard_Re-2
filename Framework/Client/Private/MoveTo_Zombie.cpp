#include "stdafx.h"
#include "GameInstance.h"
#include "MoveTo_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"

CMoveTo_Zombie::CMoveTo_Zombie() : m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

CMoveTo_Zombie::CMoveTo_Zombie(const CMoveTo_Zombie& rhs)
{
}

HRESULT CMoveTo_Zombie::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CMoveTo_Zombie::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CMoveTo_Zombie::Execute()
{
}

CMoveTo_Zombie* CMoveTo_Zombie::Create()
{
	CMoveTo_Zombie* pInstance = new CMoveTo_Zombie();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMoveTo_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoveTo_Zombie::Free()
{
	Safe_Release(m_pGameInstance);
}
