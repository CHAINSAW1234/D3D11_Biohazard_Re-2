#include "stdafx.h"
#include "GameInstance.h"
#include "Is_Start_Rub_Door_Zombie.h"
#include "BlackBoard_Zombie.h"
#include "Player.h"
#include "Zombie.h"

CIs_Start_Rub_Door_Zombie::CIs_Start_Rub_Door_Zombie()
	: CDecorator_Node()
{

}

CIs_Start_Rub_Door_Zombie::CIs_Start_Rub_Door_Zombie(const CIs_Start_Rub_Door_Zombie& rhs)
{
}

HRESULT CIs_Start_Rub_Door_Zombie::Initialize()
{
	return S_OK;
}

_bool CIs_Start_Rub_Door_Zombie::Condition_Check()
{
	if (nullptr == m_pBlackBoard)
		return false;

	if (ZOMBIE_START_TYPE::_DOOR_RUB != m_pBlackBoard->Get_AI()->Get_StartType())
		return false;

	if (true == m_isStart)
		return false;

	m_isStart = true;

	return true;
}

CIs_Start_Rub_Door_Zombie* CIs_Start_Rub_Door_Zombie::Create()
{
	CIs_Start_Rub_Door_Zombie* pInstance = { new CIs_Start_Rub_Door_Zombie() };

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CIs_Start_Rub_Door_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIs_Start_Rub_Door_Zombie::Free()
{
	__super::Free();
}
