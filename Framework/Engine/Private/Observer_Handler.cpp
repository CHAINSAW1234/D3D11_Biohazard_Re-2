#include "Observer_Handler.h"
#include "Observer.h"

CObserver_Handler::CObserver_Handler()
{
}

void CObserver_Handler::RegisterObserver(CObserver* observer)
{
	if (observer == nullptr) return;
	m_vecObserver.push_back(observer);
}

void CObserver_Handler::RemoveObserver(CObserver* observer)
{
	if (observer == nullptr) return;
	m_vecObserver.erase(std::remove(m_vecObserver.begin(), m_vecObserver.end(), observer), m_vecObserver.end());
}

void CObserver_Handler::NotifyObserver()
{
	for_each(m_vecObserver.begin(), m_vecObserver.end(), [](CObserver* _observer)
		{
			_observer->OnNotify();
		});
}

//void CObserver_Handler::Free()
//{
//	// JYWI : 이거 다중상속에 문제되는 지 잘 모르겠음. 일단 간접상속처리해서 중복 막게 처리.
//	__super::Free();
//}

