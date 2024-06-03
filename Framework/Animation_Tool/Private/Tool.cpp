#include "stdafx.h"
#include "Tool.h"
#include "GameInstance.h"

CTool::CTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pGameInstance{ CGameInstance::Get_Instance() }
	, m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
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
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
