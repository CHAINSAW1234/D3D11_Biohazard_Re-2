#include "stdafx.h"
#include "GameInstance.h"
#include "Is_In_Door_Zombie.h"
#include "BlackBoard_Zombie.h"
#include "Player.h"
#include "Zombie.h"

CIs_In_Door_Zombie::CIs_In_Door_Zombie()
	: CDecorator_Node()
{

}

CIs_In_Door_Zombie::CIs_In_Door_Zombie(const CIs_In_Door_Zombie& rhs)
{
}

HRESULT CIs_In_Door_Zombie::Initialize()
{
	return S_OK;
}

_bool CIs_In_Door_Zombie::Condition_Check()
{
	if (nullptr == m_pBlackBoard)
		return false;

	return !(m_pBlackBoard->Get_AI()->Is_OutDoor());
}

CIs_In_Door_Zombie* CIs_In_Door_Zombie::Create()
{
	CIs_In_Door_Zombie* pInstance = { new CIs_In_Door_Zombie() };

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CIs_In_Door_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIs_In_Door_Zombie::Free()
{
	__super::Free();
}
