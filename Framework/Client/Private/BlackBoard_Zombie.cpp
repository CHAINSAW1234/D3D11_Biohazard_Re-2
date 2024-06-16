#include "stdafx.h"
#include "GameInstance.h"
#include "BlackBoard_Zombie.h"
#include "Player.h"

CBlackBoard_Zombie::CBlackBoard_Zombie()
	: CBlackBoard()
{
}

CBlackBoard_Zombie::CBlackBoard_Zombie(const CBlackBoard_Zombie& rhs)
{
}

HRESULT CBlackBoard_Zombie::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlackBoard_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CBlackBoard_Zombie::Initialize_BlackBoard(CZombie* pAI)
{
	m_pAI = pAI;

	auto pPlayerLayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, L"Layer_Player");
	m_pPlayer = dynamic_cast<CPlayer*>(*(*pPlayerLayer).begin());
}

CBlackBoard_Zombie* CBlackBoard_Zombie::Create()
{
	CBlackBoard_Zombie* pInstance = new CBlackBoard_Zombie();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBlackBoard_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlackBoard_Zombie::Free()
{
	__super::Free();
}
