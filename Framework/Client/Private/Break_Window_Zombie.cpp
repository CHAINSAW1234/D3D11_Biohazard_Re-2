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

	cout << "Enter Break Window" << endl;

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

	CWindow* pWindow = { m_pBlackBoard->Get_Nearest_Window() };
	if (nullptr == pWindow)
		return false;

	//	필요 조건 => 위치 도달, 창문 안깨졌으나 체력 0되는 순간에 Knock에서 이어지게끔, 바리게이트 상관 없음, 
	MONSTER_STATE		eCurrentMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };
	if (MONSTER_STATE::MST_BREAK_WINDOW == eCurrentMonsterState)
	{
		CModel* pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
		if (nullptr == pBody_Model)
			return false;

		if (true == pBody_Model->isFinished(static_cast<_uint>(m_eBasePlayingIndex)))
			return false;
	}
	else
	{		
		_int				iWindowHP = { pWindow->Get_HP_Body() };
		_bool				isCanBreakWindow = { 1 == iWindowHP };
		if (false == isCanBreakWindow)
			return false;
	}	

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_BREAK_WINDOW);

	pWindow->Attack_Prop();

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

	_int			iResultAnimationIndex = { static_cast<_int>(ANIM_GIMMICK_WINDOW::_BREAK) };
	_bool			isLoop = { false };

	pBodyModel->Change_Animation(static_cast<_uint>(m_eBasePlayingIndex), m_strAnimLayerTag, iResultAnimationIndex);
	pBodyModel->Set_Loop(static_cast<_uint>(m_eBasePlayingIndex), isLoop);


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
