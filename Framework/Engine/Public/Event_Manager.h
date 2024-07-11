#pragma once

#include "Base.h"

BEGIN(Engine)

class CEvent_Manager final : public CBase
{
private:
	CEvent_Manager();
	virtual ~CEvent_Manager() = default;

public:
	HRESULT Add_Event(class CEvent* pNewEvent, const wstring& strEventTag);
	class CEvent* Get_Event(const wstring& strEventTag);
	_bool Check_Event(const wstring& strPrototypeTag);
	class CEvent* Get_Now_Event();
public:
	HRESULT Initialize();
	void Tick(_float fTimeDelta);
	void Clear();


private:
	map<const wstring, class CEvent*>					m_pEventsMap; //search��, ����, ������
	queue<class CEvent*>									m_pEventsQueue; //�����
private:
	class CEvent* Find_Event(const wstring& strEventTag);

public:
	static CEvent_Manager* Create();
	virtual void Free() override;
};

END