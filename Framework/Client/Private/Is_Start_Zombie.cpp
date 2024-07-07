#include "stdafx.h"
#include "GameInstance.h"
#include "Is_Start_Zombie.h"
#include "BlackBoard_Zombie.h"
#include "Player.h"
#include "Zombie.h"

CIs_Start_Zombie::CIs_Start_Zombie()
	: CDecorator_Node()
{

}

CIs_Start_Zombie::CIs_Start_Zombie(const CIs_Start_Zombie& rhs)
{
}

HRESULT CIs_Start_Zombie::Initialize()
{
	m_isStart = true;

	return S_OK;
}

_bool CIs_Start_Zombie::Condition_Check()
{
	if (nullptr == m_pBlackBoard)
		return false;

	if (false == m_isStart)
		return false;

	m_isStart = false;

	return true;
}

CIs_Start_Zombie* CIs_Start_Zombie::Create()
{
	CIs_Start_Zombie*				pInstance = { new CIs_Start_Zombie() };

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CIs_Start_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIs_Start_Zombie::Free()
{
	__super::Free();
}
