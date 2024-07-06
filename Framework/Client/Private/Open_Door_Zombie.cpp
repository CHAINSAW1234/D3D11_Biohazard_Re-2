#include "stdafx.h"
#include "GameInstance.h"
#include "Open_Door_Zombie.h"
#include "BlackBoard_Zombie.h"

#include "Player.h"
#include "Zombie.h"
#include "Body_Zombie.h"

COpen_Door_Zombie::COpen_Door_Zombie()
	: CTask_Node()
{
}

COpen_Door_Zombie::COpen_Door_Zombie(const COpen_Door_Zombie& rhs)
{
}

HRESULT COpen_Door_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void COpen_Door_Zombie::Enter()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

#ifdef _DEBUG

	cout << "Enter Knock Door" << endl;

#endif 
}

_bool COpen_Door_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	if (false == m_pBlackBoard->Is_LookTarget())
		return false;

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_WALK);

	Change_Animation(fTimeDelta);

	return true;
}

void COpen_Door_Zombie::Exit()
{
	if (nullptr == m_pBlackBoard)
		return;
}

void COpen_Door_Zombie::Change_Animation(_float fTimeDelta)
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_int			iResultAnimationIndex = { -1 };

#pragma endregion
}

COpen_Door_Zombie* COpen_Door_Zombie::Create(void* pArg)
{
	COpen_Door_Zombie* pInstance = { new COpen_Door_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : COpen_Door_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void COpen_Door_Zombie::Free()
{
	__super::Free();
}
