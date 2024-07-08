#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CRoom_Finder final : public CBase
{
	DECLARE_SINGLETON(CRoom_Finder)
private:
	CRoom_Finder();
	virtual ~CRoom_Finder() = default;

public:
	HRESULT									Initialize();

public:
	list<class CDoor*>						Find_Linked_Doors_From_Location(LOCATION_MAP_VISIT eLocation);
	list<LOCATION_MAP_VISIT>				Find_Linked_Loctaion_From_Door(class CDoor* pDoor);

public:
	HRESULT									Add_Door(LOCATION_MAP_VISIT eLocation, class CDoor* pDoor);

private:
	_bool									Is_Exist_Door_Already(LOCATION_MAP_VISIT eLocation, class CDoor* pDoor);

private:
	//	인데스 접근			=> LOCATION_MAP_VISIT Enum 값 활용하기
	vector<list<class CDoor*>>				m_Doors_In_Locations;

public:
	virtual void Free() override;
};

END