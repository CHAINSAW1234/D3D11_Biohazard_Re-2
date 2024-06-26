#include "stdafx.h"
#include "GameInstance.h"
#include "Shake_Skin_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CShake_Skin_Zombie::CShake_Skin_Zombie()
	: CTask_Node()
{
}

CShake_Skin_Zombie::CShake_Skin_Zombie(const CShake_Skin_Zombie& rhs)
{
}

HRESULT CShake_Skin_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CShake_Skin_Zombie::Enter()
{

}

_bool CShake_Skin_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->GetAI();
	pAI->SetState(MONSTER_STATE::MST_IDLE);

	//	cout << "Wait" << endl;
	Change_Animation();

	return true;
}

void CShake_Skin_Zombie::Exit()
{
}

void CShake_Skin_Zombie::Change_Animation()
{
	if (nullptr == m_pBlackBoard)
		return;
}

CShake_Skin_Zombie* CShake_Skin_Zombie::Create(void* pArg)
{
	CShake_Skin_Zombie* pInstance = { new CShake_Skin_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CShake_Skin_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShake_Skin_Zombie::Free()
{
	__super::Free();
}
