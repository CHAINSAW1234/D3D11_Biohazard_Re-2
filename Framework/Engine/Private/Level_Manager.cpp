#include "..\Public\Level_Manager.h"

#include "GameInstance.h"
#include "Level.h"

CLevel_Manager::CLevel_Manager()
{
}

HRESULT CLevel_Manager::Initialize()
{
	m_pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(m_pGameInstance);

	return S_OK;
}

HRESULT CLevel_Manager::Open_Level()
{
	if (m_queueRequestLevel.empty())
		return S_OK;

	_uint iNewLevelID = m_queueRequestLevel.front().first;
	CLevel* pNewLevel = m_queueRequestLevel.front().second;
	m_queueRequestLevel.pop();


	if (nullptr != m_pCurrentLevel)
	{
		CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);

		pGameInstance->Clear(m_iCurrentLevelID);

		Safe_Release(pGameInstance);
	}	

	Safe_Release(m_pCurrentLevel);

	m_pCurrentLevel = pNewLevel;
	m_iCurrentLevelID = iNewLevelID;

	m_pGameInstance->Set_RenderFieldShadow(true);

	m_pGameInstance->Start();

	return S_OK;
}

HRESULT CLevel_Manager::Request_Open_Level(_uint iNewLevelID, CLevel* pNewLevel)
{
	m_queueRequestLevel.push({ iNewLevelID , pNewLevel });

	return S_OK;
}

void CLevel_Manager::Tick(_float fTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Tick(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	if (nullptr == m_pCurrentLevel)
		return E_FAIL;

	return m_pCurrentLevel->Render();
}

_uint CLevel_Manager::Get_CurrentLevel()
{
	return m_iCurrentLevelID;
}

CLevel_Manager * CLevel_Manager::Create()
{
	CLevel_Manager*		pInstance = new CLevel_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_Manager"));

		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Manager::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pCurrentLevel);
}
