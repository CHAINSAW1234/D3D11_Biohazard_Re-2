#pragma once
#include"Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
END


BEGIN(Client)

class CProp_Manager final : public CBase
{
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
	static CProp_Manager* Create();
	virtual void Free() override;
};

END