#include "stdafx.h"
#include "GameInstance.h"
#include "Sleep_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CSleep_Zombie::CSleep_Zombie()
	: CTask_Node()
{
}

CSleep_Zombie::CSleep_Zombie(const CSleep_Zombie& rhs)
{
}

HRESULT CSleep_Zombie::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSleep_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CSleep_Zombie::Enter()
{
}

_bool CSleep_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	_bool			isSleep = { m_pBlackBoard->Get_AI()->Is_Sleep() };
	if (false == isSleep)
		return false;

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->SetState(MONSTER_STATE::MST_SLEEP);

	return true;
}

void CSleep_Zombie::Exit()
{
}

CSleep_Zombie* CSleep_Zombie::Create()
{
	CSleep_Zombie* pInstance = new CSleep_Zombie();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSleep_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSleep_Zombie::Free()
{
	__super::Free();
}
