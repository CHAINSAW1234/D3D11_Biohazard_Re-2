#include "Event.h"
#include "GameInstance.h"
#include"GameObject.h"
CEvent::CEvent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

}

HRESULT CEvent::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CEvent::Add_List(const wstring& strLayerTag, CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;
	if (false == Find_List(strLayerTag))
	{
		list<CGameObject*> List;
		List.emplace_back(pGameObject);
		m_EventObjectMap.emplace(strLayerTag, List);
		return S_OK;
	}
	else
	{
		auto& iter = m_EventObjectMap.find(strLayerTag);
		iter->second.emplace_back(pGameObject);
		return S_OK;
	}
}

_bool CEvent::Find_List(const wstring& strLayerTag)
{
	if (m_EventObjectMap.end() == m_EventObjectMap.find(strLayerTag))
		return false;

	return true;
}

CGameObject* CEvent::Find_Object(const wstring& strLayerTag, _int iIndex)
{
	auto& Pair = m_EventObjectMap.find(strLayerTag);
	if (m_EventObjectMap.end() == Pair)
		return nullptr;

	if (Pair->second.size() < iIndex)
		return nullptr;

	auto& iter =  Pair->second.begin();
	for (size_t i = 0; i < iIndex; i++)
		iter++;

	return *iter;
}

CEvent::STATE CEvent::Tick(_float fTimeDelta)
{


	return m_eState;
}

HRESULT CEvent::Render()
{
	return S_OK;
}



void CEvent::Free()
{
	__super::Free();
	for (auto& pPair : m_EventObjectMap)
		for (auto& iter : pPair.second)
			Safe_Release(iter);

	m_EventObjectMap.clear();
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
