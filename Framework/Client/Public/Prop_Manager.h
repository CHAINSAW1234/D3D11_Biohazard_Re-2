#pragma once
#include"Client_Defines.h"
#include "Base.h"

/*
맵 UI에게 상호작용 가능한 오브젝트를 구역별로 던지는 클래스
아이템 종류에따라서에 대해서도 
*/
BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CProp_Manager final : public CBase
{
	DECLARE_SINGLETON(CProp_Manager)
private:
	CProp_Manager();
	virtual ~CProp_Manager() = default;
public:
	HRESULT									Initialize();
public:
	list<class CGameObject*>*		Get_Props(_int iRegion) { return m_RegionProps[iRegion]; }

private:	
	class CGameInstance*				m_pGameInstance = { nullptr };
	map<_int, list<class CGameObject*>*>				m_RegionProps;
private:
	HRESULT									Initialize_List();
	list<class CGameObject*>*			Find_List(_int iTag);
public:
	virtual void Free() override;
};

END