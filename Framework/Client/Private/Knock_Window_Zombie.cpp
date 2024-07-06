#include "stdafx.h"
#include "GameInstance.h"
#include "Knock_Window_Zombie.h"
#include "BlackBoard_Zombie.h"

#include "Window.h"
#include "Player.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CKnock_Window_Zombie::CKnock_Window_Zombie()
	: CTask_Node()
{
}

CKnock_Window_Zombie::CKnock_Window_Zombie(const CKnock_Window_Zombie& rhs)
{
}

HRESULT CKnock_Window_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CKnock_Window_Zombie::Enter()
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

_bool CKnock_Window_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	//	필요 조건 => 위치 도달, 창문 안깨짐, 바리게이트 여부(상관없음)
	CWindow*			pWindow = { m_pBlackBoard->Get_Nearest_Window() };
	if (nullptr == pWindow)
		return false;
	_int				iWindowHP = { pWindow->Get_HP_Body() };
	_bool				isWindowBroken = { 0 >= iWindowHP };
	if (true == isWindowBroken)
		return false;

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_WALK);

	Change_Animation(fTimeDelta);

	return true;
}

void CKnock_Window_Zombie::Exit()
{
	if (nullptr == m_pBlackBoard)
		return;
}

void CKnock_Window_Zombie::Change_Animation(_float fTimeDelta)
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_int			iResultAnimationIndex = { -1 };

#pragma endregion
}

CKnock_Window_Zombie* CKnock_Window_Zombie::Create(void* pArg)
{
	CKnock_Window_Zombie* pInstance = { new CKnock_Window_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CKnock_Window_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKnock_Window_Zombie::Free()
{
	__super::Free();
}
