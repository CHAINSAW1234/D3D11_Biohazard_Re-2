#include "Event_Manager.h"
#include "GameObject.h"
#include "Event.h"

CEvent_Manager::CEvent_Manager()
{
}


HRESULT CEvent_Manager::Initialize()
{
	return S_OK;
}


HRESULT CEvent_Manager::Add_Event(CEvent* pNewEvent, const wstring& strEventTag)
{
	CEvent* pEvent = Find_Event(strEventTag);
	if (nullptr == pEvent)
	{
		m_pEventsQueue.push(pNewEvent);
		m_pEventsMap.emplace(strEventTag, pNewEvent);
	}
	else	
	{
		Safe_Release(pNewEvent);
		pNewEvent = nullptr;
		return E_FAIL;
	}

	return S_OK;
}




 CEvent* CEvent_Manager::Get_Event( const wstring& strEventTag)
{
	 CEvent* CEvent = Find_Event(strEventTag);
	if (nullptr == CEvent)
		return nullptr;
	
	return CEvent;
}


_bool CEvent_Manager::Check_Event(const wstring& strEventTag)
{
	auto		iter = m_pEventsMap.find(strEventTag);

	if (iter == m_pEventsMap.end())
		return true;

	return false;
}

CEvent* CEvent_Manager::Get_Now_Event()
{
	if (m_pEventsQueue.empty())
		return nullptr;

	CEvent* CEvent = m_pEventsQueue.front();
	if (nullptr == CEvent)
		return nullptr;
	
	return CEvent;
}

void CEvent_Manager::Tick(_float fTimeDelta)
{
	if (m_pEventsQueue.empty())
		return;

	auto iter = m_pEventsQueue.front();
	CEvent::STATE eResult = iter->Tick(fTimeDelta);
	if (eResult == CEvent::EVENT_STATE_STOP)
		m_pEventsQueue.pop();
	

}


void CEvent_Manager::Clear()
{
	for (auto& Pair : m_pEventsMap)
	{
		Safe_Release(Pair.second);
	}
	m_pEventsMap.clear();
}

CEvent * CEvent_Manager::Find_Event(const wstring& strEventTag)
{
	auto		iter = m_pEventsMap.find(strEventTag);

	if (iter == m_pEventsMap.end())
		return nullptr;

	return iter->second;
}


CEvent_Manager * CEvent_Manager::Create()
{
	CEvent_Manager*		pInstance = new CEvent_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CEvent_Manager"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEvent_Manager::Free()
{
	for (auto& pPair : m_pEventsMap)
		Safe_Release(pPair.second);

}
