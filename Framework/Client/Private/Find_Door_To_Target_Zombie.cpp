#include "stdafx.h"
#include "GameInstance.h"
#include "Find_Door_To_Target_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

#include "Door.h"
#include "Room_Finder.h"

CFind_Door_To_Target_Zombie::CFind_Door_To_Target_Zombie()
{
}

CFind_Door_To_Target_Zombie::CFind_Door_To_Target_Zombie(const CFind_Door_To_Target_Zombie& rhs)
{
}

HRESULT CFind_Door_To_Target_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CFind_Door_To_Target_Zombie::Enter()
{
}

_bool CFind_Door_To_Target_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	CPlayer*		pPlayer = { m_pBlackBoard->Get_Player() };
	CZombie*		pZombie = { m_pBlackBoard->Get_AI() };

	if (nullptr == pPlayer || nullptr == pZombie)
		return false;

	LOCATION_MAP_VISIT		ePlayerLocation = { static_cast<LOCATION_MAP_VISIT>(pPlayer->Get_Player_Region()) };
	LOCATION_MAP_VISIT		eZombieLocation = { static_cast<LOCATION_MAP_VISIT>(pZombie->Get_Location()) };
	
	CDoor*					pTargetDoor = { nullptr };
	list<CDoor*>			Doors = { CRoom_Finder::Get_Instance()->Find_Linked_Doors_From_Location(eZombieLocation) };
	for (auto& pDoor : Doors)
	{
		list<LOCATION_MAP_VISIT>		Locations = { CRoom_Finder::Get_Instance()->Find_Linked_Loctaion_From_Door(pDoor) };
		for (auto& eLocation : Locations)
		{
			if (eLocation == ePlayerLocation)
			{
				pTargetDoor = pDoor;
				break;
			}
		}
	}

	m_pBlackBoard->Set_TargetDoor(pTargetDoor);
	_bool					isFindTarget = { pTargetDoor != nullptr };

	return isFindTarget;
}

void CFind_Door_To_Target_Zombie::Exit()
{
}

CFind_Door_To_Target_Zombie* CFind_Door_To_Target_Zombie::Create(void* pArg)
{
	CFind_Door_To_Target_Zombie*			pInstance = { new CFind_Door_To_Target_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CFind_Door_To_Target_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFind_Door_To_Target_Zombie::Free()
{
	__super::Free();
}