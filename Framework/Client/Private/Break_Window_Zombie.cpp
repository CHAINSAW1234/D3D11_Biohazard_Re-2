#include "stdafx.h"
#include "GameInstance.h"
#include "Break_Window_Zombie.h"
#include "BlackBoard_Zombie.h"

#include "Window.h"
#include "Player.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CBreak_Window_Zombie::CBreak_Window_Zombie()
	: CTask_Node()
{
}

CBreak_Window_Zombie::CBreak_Window_Zombie(const CBreak_Window_Zombie& rhs)
{
}

HRESULT CBreak_Window_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CBreak_Window_Zombie::Enter()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*					pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

#ifdef _DEBUG

	cout << "Enter Knock Door" << endl;

#endif 
}

_bool CBreak_Window_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	//	�ʿ� ���� => ��ġ ����, â�� �ȱ������� ü�� 0�Ǵ� ������ Knock���� �̾����Բ�, �ٸ�����Ʈ ��� ����, 
	MONSTER_STATE		eCurrentMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };
	if (MONSTER_STATE::MST_KNOCK_WINDOW != eCurrentMonsterState)
		return false;

	CWindow*			pWindow = { m_pBlackBoard->Get_Nearest_Window() };
	if (nullptr == pWindow)
		return false;
	_int				iWindowHP = { pWindow->Get_HP_Body() };
	_bool				isCanBreakWindow = { 1 == iWindowHP };
	if (false == isCanBreakWindow)
		return false;

	if (false == m_pBlackBoard->Is_LookTarget())
		return false;

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_WALK);

	Change_Animation(fTimeDelta);

	return true;
}

void CBreak_Window_Zombie::Exit()
{
	if (nullptr == m_pBlackBoard)
		return;
}

void CBreak_Window_Zombie::Change_Animation(_float fTimeDelta)
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*			pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_int			iResultAnimationIndex = { -1 };

#pragma endregion
}

CBreak_Window_Zombie* CBreak_Window_Zombie::Create(void* pArg)
{
	CBreak_Window_Zombie*			pInstance = { new CBreak_Window_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBreak_Window_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBreak_Window_Zombie::Free()
{
	__super::Free();
}
