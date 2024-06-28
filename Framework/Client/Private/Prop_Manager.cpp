#include "stdafx.h"
#include "GameInstance.h"
#include "Prop_Manager.h"
#include "InteractProps.h"

CProp_Manager::CProp_Manager()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);

}

HRESULT CProp_Manager::Initialize()
{
	if (FAILED(Initialize_List()))
		return E_FAIL;
	return S_OK;
}

HRESULT CProp_Manager::Initialize_List()
{
	list<class CGameObject*>* pInteractList = m_pGameInstance->Find_Layer(g_Level,TEXT("Layer_InteractObj"));

	for (auto& iter: *pInteractList)
	{
		_int iRegion = static_cast<CInteractProps*>(iter)->Get_Region();
		list<class CGameObject*>* pList = Find_List(iRegion);
		if (pList == nullptr)
		{
			list<class CGameObject*>* pnewList = new list<class CGameObject*>;
			pnewList->emplace_back(iter);
			Safe_AddRef(iter);
			m_RegionProps[iRegion] = pnewList;
		}
		else
		{
			pList->emplace_back(iter);
			Safe_AddRef(iter);
		}

	}

	return S_OK;
}

list<class CGameObject*>* CProp_Manager::Find_List(_int iTag)
{
	auto		iter = m_RegionProps.find(iTag);

	if (iter == m_RegionProps.end())
		return nullptr;

	return iter->second;
}

CProp_Manager* CProp_Manager::Create()
{
	CProp_Manager* pInstance = new CProp_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : Prop_Manager"));

		Safe_Release(pInstance);
		pInstance = nullptr;
	}

	return pInstance;
}

void CProp_Manager::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);

	for (auto& list: m_RegionProps)
	{
		for (auto& iter: *(list.second))
		{
			Safe_Release(iter);
			iter = nullptr;
		}
		(*list.second).clear();
		Safe_Delete((list.second));
	}
	m_RegionProps.clear();
}
