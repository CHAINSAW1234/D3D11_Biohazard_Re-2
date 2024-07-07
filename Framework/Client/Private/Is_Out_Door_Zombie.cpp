#include "stdafx.h"
#include "GameInstance.h"
#include "Is_Out_Door_Zombie.h"
#include "BlackBoard_Zombie.h"
#include "Player.h"
#include "Zombie.h"

CIs_Out_Door_Zombie::CIs_Out_Door_Zombie()
	: CDecorator_Node()
{

}

CIs_Out_Door_Zombie::CIs_Out_Door_Zombie(const CIs_Out_Door_Zombie& rhs)
{
}

HRESULT CIs_Out_Door_Zombie::Initialize()
{
	return S_OK;
}

_bool CIs_Out_Door_Zombie::Condition_Check()
{
	if (nullptr == m_pBlackBoard)
		return false;

	return m_pBlackBoard->Get_AI()->Is_OutDoor();
}

CIs_Out_Door_Zombie* CIs_Out_Door_Zombie::Create()
{
	CIs_Out_Door_Zombie*			pInstance = { new CIs_Out_Door_Zombie() };

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CIs_Out_Door_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIs_Out_Door_Zombie::Free()
{
	__super::Free();
}
