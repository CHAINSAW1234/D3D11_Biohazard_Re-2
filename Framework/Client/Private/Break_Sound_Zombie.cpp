#include "stdafx.h"
#include "GameInstance.h"
#include "Break_Sound_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CBreak_Sound_Zombie::CBreak_Sound_Zombie()
	: CTask_Node()
{
}

CBreak_Sound_Zombie::CBreak_Sound_Zombie(const CBreak_Sound_Zombie& rhs)
{
}

HRESULT CBreak_Sound_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CBreak_Sound_Zombie::Enter()
{

}

_bool CBreak_Sound_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion	

	if (CZombie::POSE_STATE::_UP != m_pBlackBoard->Get_AI()->Get_PoseState())
		return true;

	if (true == m_pBlackBoard->Is_New_Part_Break())
		m_pBlackBoard->Get_AI()->Play_Random_Broken_Part_Sound();

	return true;
}

void CBreak_Sound_Zombie::Exit()
{
}

CBreak_Sound_Zombie* CBreak_Sound_Zombie::Create(void* pArg)
{
	CBreak_Sound_Zombie* pInstance = { new CBreak_Sound_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBreak_Sound_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBreak_Sound_Zombie::Free()
{
	__super::Free();
}
