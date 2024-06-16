#include "stdafx.h"
#include "GameInstance.h"
#include "MoveTo_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"

CMoveTo_Zombie::CMoveTo_Zombie() 
	: CTask_Node()
{
}

CMoveTo_Zombie::CMoveTo_Zombie(const CMoveTo_Zombie& rhs)
{
}

HRESULT CMoveTo_Zombie::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMoveTo_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CMoveTo_Zombie::Execute()
{
	auto pAI = m_pBlackBoard->GetAI();
	pAI->SetState(MONSTER_STATE::MST_WALK);
}

void CMoveTo_Zombie::Set_Animation()
{
	if (nullptr == m_pBlackBoard)
		return;

	CZombie*		pZombie = { m_pBlackBoard->GetAI() };
	if (nullptr == pZombie)
		return;

	CModel*			pModel = { pZombie->Get_Model_From_PartObject(CZombie::PART_BODY) };
	if (nullptr == pModel)
		return;

	//	pModel->Change_Animation(0, )
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
	__super::Free();
}
