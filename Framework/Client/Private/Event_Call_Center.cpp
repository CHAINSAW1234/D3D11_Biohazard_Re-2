#include "stdafx.h"	
#include "Event_Call_Center.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CEvent_Call_Center)

CEvent_Call_Center::CEvent_Call_Center()
{
	m_Callers.resize(static_cast<_uint>(CALLER::_END));
}

void CEvent_Call_Center::Free()
{
	__super::Free();

	for (auto& pCaller : m_Callers)
	{
		Safe_Release(pCaller);
		pCaller = nullptr;
	}
	m_Callers.clear();
}
