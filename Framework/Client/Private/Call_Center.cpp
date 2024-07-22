#include "stdafx.h"	
#include "Call_Center.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CCall_Center)

CCall_Center::CCall_Center()
{
	m_Callers.resize(static_cast<_uint>(CALLER::_END));
}

_bool CCall_Center::Is_Focus_Player()
{
	_bool			isFocusPlayer = { m_isFocusPlayer };
	
	m_isFocusPlayer = false;

	return isFocusPlayer;
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
