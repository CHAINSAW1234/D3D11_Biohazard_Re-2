#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CEvent abstract : public CBase
{
public:
	enum STATE{ EVENT_STATE_WAIT, EVENT_STATE_ACTIVE, EVENT_STATE_STOP }; // 
protected:
	CEvent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CEvent() = default;

public:
	virtual HRESULT Initialize(void* pArg = nullptr);
	virtual STATE Tick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual HRESULT Subscribe_News(void* pArg) { return S_OK; }; // CGameObject들이 자신의 this를 던지면 이벤트에서 처리해줄 수 있다. 
public:
	_bool	 Get_Dead() 
	{
		if (m_eState == EVENT_STATE_STOP) return true;
		else return false;
	}

protected:
	HRESULT	Add_List(const wstring& strLayerTag, class CGameObject* pGameObject);
	_bool Find_List(const wstring& strLayerTag);
	class CGameObject* Find_Object(const wstring& strLayerTag,_int iIndex = 0);
protected:
	STATE										m_eState	= { EVENT_STATE_WAIT };
	ID3D11Device*							m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
	class CGameInstance*				m_pGameInstance = { nullptr };
	map<const wstring ,list<class CGameObject*>> m_EventObjectMap; //이벤트 오브젝트에 대해 레이어를 돌리는 방식
public:
	virtual void Free() override;
};
END