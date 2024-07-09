#include "stdafx.h"
#include "GameInstance.h"
#include "Is_Same_Location_To_Player_Zombie.h"
#include "BlackBoard_Zombie.h"
#include "Player.h"
#include "Zombie.h"

CIs_Same_Location_To_Player_Zombie::CIs_Same_Location_To_Player_Zombie()
	: CDecorator_Node()
{

}

CIs_Same_Location_To_Player_Zombie::CIs_Same_Location_To_Player_Zombie(const CIs_Same_Location_To_Player_Zombie& rhs)
{
}

HRESULT CIs_Same_Location_To_Player_Zombie::Initialize(_float fRange)
{
	return S_OK;
}

_bool CIs_Same_Location_To_Player_Zombie::Condition_Check()
{
	if (nullptr == m_pBlackBoard)
		return false;

	return m_pBlackBoard->Get_AI()->Is_In_Location(static_cast<LOCATION_MAP_VISIT>(m_pBlackBoard->Get_Player()->Get_Player_Region()));
}

CIs_Same_Location_To_Player_Zombie* CIs_Same_Location_To_Player_Zombie::Create(_float fRange)
{
	CIs_Same_Location_To_Player_Zombie* pInstance = { new CIs_Same_Location_To_Player_Zombie() };

	if (FAILED(pInstance->Initialize(fRange)))
	{
		MSG_BOX(TEXT("Failed To Created : CIs_Same_Location_To_Player_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIs_Same_Location_To_Player_Zombie::Free()
{
	__super::Free();
}
