#include "stdafx.h"
#include "Tool.h"
#include "GameInstance.h"

CTool::CTool()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CTool::Initialize(void* pArg)
{
	m_fHeight = 200.f;

	return S_OK;
}

void CTool::Tick(_float fTimeDelta)
{
}

void CTool::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
