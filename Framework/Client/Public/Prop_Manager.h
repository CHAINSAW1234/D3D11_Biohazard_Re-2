#pragma once
#include"Client_Defines.h"
#include "Base.h"

/*
�� UI���� ��ȣ�ۿ� ������ ������Ʈ�� �������� ������ Ŭ����
������ ���������󼭿� ���ؼ��� 
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
	list<class CGameObject*>*		Get_Region_Props(_int iRegion) { return m_RegionProps[iRegion]; }
	list<class CGameObject*>*		Get_Type_Props(_int iType) { return m_TypeProps[iType]; }
	
private:	
	class CGameInstance*				m_pGameInstance = { nullptr };
	map<_int, list<class CGameObject*>*>				m_RegionProps;
	
	map<_int, list<class CGameObject*>*>				m_TypeProps;
	
private:
	HRESULT								Initialize_List();
	list<class CGameObject*>*			Find_List(_int iTag);
public:
	static void Release_Manager();
	virtual void Free() override;
};

END