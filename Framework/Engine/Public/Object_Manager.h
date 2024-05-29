#pragma once

#include "Base.h"

BEGIN(Engine)

class CObject_Manager final : public CBase
{
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	const class CComponent*								Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComTag, _uint iIndex);

public:
	HRESULT												Initialize(_uint iNumLevels);
	HRESULT												Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT												Add_Clone(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg);
	class CGameObject*									Clone_GameObject(const wstring& strPrototypeTag, void* pArg);
	void												Priority_Tick(_float fTimeDelta);
	void												Tick(_float fTimeDelta);
	void												Late_Tick(_float fTimeDelta);
	void												Clear(_uint iLevelIndex);
	void												Release_Layer(_uint iLevelIndex, const wstring& LayerTag);
	class CLayer*										Find_Layer(_uint iLevelIndex, const wstring& strLayerTag);
private:	
	map<const wstring, class CGameObject*>				m_Prototypes;

	_uint												m_iNumLevels = { 0 };
	map<const wstring, class CLayer*>*					m_pLayers = { nullptr };

private:
	class CGameObject*									Find_Prototype(const wstring& strPrototypeTag);

public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END