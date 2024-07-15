#include "stdafx.h"	
#include "Call_Center.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CCall_Center)

CCall_Center::CCall_Center()
{
}

void CCall_Center::Free()
{
	__super::Free();

	for (auto& pCaller : m_Callers)
	{
		Safe_Release(pCaller);
		pCaller = nullptr;
	}
	m_Callers.clear();
}
