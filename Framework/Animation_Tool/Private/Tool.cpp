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

wstring CTool::Convert_String_Wstring(const string& strText)
{
	const _char*		szBuf = { strText.c_str() };
	TCHAR				lszBuf[MAX_PATH] = { L"" };

	MultiByteToWideChar(CP_ACP, 0, szBuf, (_uint)strlen(szBuf), lszBuf, MAX_PATH);

	wstring				strResult = { lszBuf };

	return strResult;
}

string CTool::Convert_Wstring_String(const wstring& strText)
{
	const TCHAR*		lszBuf = { strText.c_str() };
	_char				szBuf[MAX_PATH] = { "" };

	WideCharToMultiByte(CP_UTF8, 0, lszBuf, (_uint)lstrlen(lszBuf), szBuf, MAX_PATH, NULL, NULL);

	string				strResult = { szBuf };

	return strResult;
}

void CTool::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
