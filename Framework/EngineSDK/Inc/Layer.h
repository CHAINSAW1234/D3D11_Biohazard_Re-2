#pragma once

#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	const class CComponent*			Get_Component(const wstring& strComTag, _uint iIndex);
	list <class CGameObject*>*		Get_Layer();
	void							Release_Layer();
public:
	HRESULT							Initialize();
	HRESULT							Add_GameObject(class CGameObject* pGameObject);
	void							Priority_Tick(_float fTimeDelta);
	void							Tick(_float fTimeDelta);
	void							Late_Tick(_float fTimeDelta);
	void							Start();

public:
	CGameObject*					Get_GameObject(_uint iIndex);
	list<class CGameObject*>*		Get_ObjectList_Ptr();

private:
	list<class CGameObject*>		m_GameObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END