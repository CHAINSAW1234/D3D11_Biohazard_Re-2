#include "stdafx.h"
#include "Room_Finder.h"
#include "Door.h"

IMPLEMENT_SINGLETON(CRoom_Finder)

CRoom_Finder::CRoom_Finder()
{
}

HRESULT CRoom_Finder::Initialize()
{
	m_Doors_In_Locations.clear();
	for(_uint i = 0; i < LOCATION_MAP_VISIT::LOCATION_MAP_VISIT_END; ++i)
		m_Doors_In_Locations.emplace_back(list<CDoor*>());

	return S_OK;
}

list<CDoor*> CRoom_Finder::Find_Linked_Doors_From_Location(LOCATION_MAP_VISIT eLocation)
{
	list<CDoor*>							Linked_Doors;

	_uint				iNumLocations = { static_cast<_uint>(m_Doors_In_Locations.size()) };
	if (iNumLocations > static_cast<_uint>(eLocation))
	{
		Linked_Doors = m_Doors_In_Locations[static_cast<_uint>(eLocation)];
	}

	return Linked_Doors;
}

list<LOCATION_MAP_VISIT> CRoom_Finder::Find_Linked_Loctaion_From_Door(CDoor* pDoor)
{
	list<LOCATION_MAP_VISIT>				Linked_Locations;

	for (_uint i = 0; i < LOCATION_MAP_VISIT::LOCATION_MAP_VISIT_END; ++i)
	{
		list<CDoor*>::iterator			iter = { find(m_Doors_In_Locations[i].begin(), m_Doors_In_Locations[i].end(), pDoor) };

		if (iter != m_Doors_In_Locations[i].end())
		{
			Linked_Locations = { pDoor->Get_LinkedLocations() };
			break;
		}		
	}

	return Linked_Locations;
}

HRESULT CRoom_Finder::Add_Door(LOCATION_MAP_VISIT eLocation, CDoor* pDoor)
{
	if (true == Is_Exist_Door_Already(eLocation, pDoor))
		return E_FAIL;

	_uint				iNumLocations = { static_cast<_uint>(m_Doors_In_Locations.size()) };
	if (iNumLocations <= static_cast<_uint>(eLocation))
		return E_FAIL;

	m_Doors_In_Locations[static_cast<_uint>(eLocation)].emplace_back(pDoor);
	Safe_AddRef(pDoor);

	return S_OK;
}

_bool CRoom_Finder::Is_Exist_Door_Already(LOCATION_MAP_VISIT eLocation, CDoor* pDoor)
{
	_uint				iNumLocations = { static_cast<_uint>(m_Doors_In_Locations.size()) };
	if (iNumLocations <= static_cast<_uint>(eLocation))
		return false;

	list<CDoor*>&		LinkedDoors = { m_Doors_In_Locations[static_cast<_uint>(eLocation)] };
	list<CDoor*>::iterator			iterDoor = { find(LinkedDoors.begin(), LinkedDoors.end(), pDoor) };

	if (iterDoor == LinkedDoors.end())
		return false;

	return true;
}

void CRoom_Finder::Free()
{
	__super::Free();

	for (auto& Doors : m_Doors_In_Locations)
	{
		for (auto& pDoor : Doors)
		{
			Safe_Release(pDoor);
			pDoor = nullptr;
		}
		Doors.clear();
	}
	m_Doors_In_Locations.clear();
}
