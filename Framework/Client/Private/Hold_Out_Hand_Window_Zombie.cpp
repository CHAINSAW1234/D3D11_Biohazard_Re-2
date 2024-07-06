#include "stdafx.h"
#include "GameInstance.h"
#include "Hold_Out_Hand_Window_Zombie.h"
#include "BlackBoard_Zombie.h"

#include "Window.h"
#include "Player.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CHold_Out_Hand_Window_Zombie::CHold_Out_Hand_Window_Zombie()
	: CTask_Node()
{
}

CHold_Out_Hand_Window_Zombie::CHold_Out_Hand_Window_Zombie(const CHold_Out_Hand_Window_Zombie& rhs)
{
}

HRESULT CHold_Out_Hand_Window_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CHold_Out_Hand_Window_Zombie::Enter()
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

_bool CHold_Out_Hand_Window_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	CWindow*			pWindow = { m_pBlackBoard->Get_Nearest_Window() };
	if (nullptr == pWindow)
		return false;

	//	ÇÊ¿ä Á¶°Ç => Ã¢¹® ±úÁü, ¹Ù¸®°ÔÀÌÆ® ÃÄÁü
	_int				iHpWindow = { pWindow->Get_HP_Body() };
	_bool				isWindowBroken = { 0 >= iHpWindow };
	if (false == isWindowBroken)
		return false;
	_bool				isSetBarrigate = { pWindow->Is_Set_Barrigate() };
	if (false == isSetBarrigate)
		return false;


	if (false == m_pBlackBoard->Is_LookTarget())
		return false;

	if (false == m_pBlackBoard->Is_LookTarget())
		return false;

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_WALK);

	Change_Animation(fTimeDelta);

	return true;
}

void CHold_Out_Hand_Window_Zombie::Exit()
{
	if (nullptr == m_pBlackBoard)
		return;
}

void CHold_Out_Hand_Window_Zombie::Change_Animation(_float fTimeDelta)
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_int			iResultAnimationIndex = { -1 };

#pragma endregion
}

CHold_Out_Hand_Window_Zombie* CHold_Out_Hand_Window_Zombie::Create(void* pArg)
{
	CHold_Out_Hand_Window_Zombie* pInstance = { new CHold_Out_Hand_Window_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHold_Out_Hand_Window_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHold_Out_Hand_Window_Zombie::Free()
{
	__super::Free();
}
