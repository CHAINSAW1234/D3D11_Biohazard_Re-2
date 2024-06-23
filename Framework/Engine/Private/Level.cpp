#include "..\Public\Level.h"
#include "GameInstance.h"
#include"Light.h"
#include"GameObject.h"

CLevel::CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

}

HRESULT CLevel::Initialize()
{
	return S_OK;
}

void CLevel::Tick(_float fTimeDelta)
{
}

HRESULT CLevel::Render()
{
	return S_OK;
}

HRESULT CLevel::Load_Light(const wstring& strFilePath, _uint iLevel)
{
	wstring	strLightListFilePath = strFilePath + TEXT("\\\\Light_Layer_info.dat");

	if (!filesystem::exists(strLightListFilePath))
	{
		return S_OK;
	}

	//_uint iMonsterNum = { 0 };
	HANDLE		hLightLayerFile = CreateFile(strLightListFilePath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hLightLayerFile)
		return E_FAIL;

	wstring	strLightFilePath = strFilePath + TEXT("\\\\Light_info.dat");

	if (!filesystem::exists(strLightFilePath))
	{
		CloseHandle(hLightLayerFile);
		return S_OK;
	}

	_uint iMonsterNum = { 0 };
	HANDLE		hFile = CreateFile(strLightFilePath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hLightLayerFile);
		return E_FAIL;
	}

	_uint iLightLayerNum = { 0 };

	DWORD	dwByte(0);

	if (!ReadFile(hLightLayerFile, &iLightLayerNum, sizeof(_uint), &dwByte, nullptr))
	{
		CloseHandle(hLightLayerFile);
		CloseHandle(hFile);
		return E_FAIL;
	}




	if (!ReadFile(hFile, &iLightLayerNum, sizeof(_uint), &dwByte, nullptr))
		return E_FAIL;



	for (_uint i = 0; iLightLayerNum > i; ++i)
	{

		_uint iLength = { 0 };

		LIGHT_DESC tagLight_desc = {};

		if (!ReadFile(hLightLayerFile, &iLength, sizeof(_uint), &dwByte, nullptr))
		{
			CloseHandle(hLightLayerFile);
			CloseHandle(hFile);
			return E_FAIL;
		}
		char* szLightTag = new char[iLength / sizeof(char) + 1];
		if (!ReadFile(hLightLayerFile, szLightTag, iLength, &dwByte, nullptr))
		{
			CloseHandle(hLightLayerFile);
			CloseHandle(hFile);
			return E_FAIL;
		}
		szLightTag[iLength / sizeof(char)] = '\0';
		string strLightTag = szLightTag;
		delete[] szLightTag;


		_tchar	wszLightTag[MAX_PATH] = { L"" };
		MultiByteToWideChar(CP_ACP, 0, strLightTag.c_str(), (_uint)strlen(strLightTag.c_str()), wszLightTag, MAX_PATH);

		m_pGameInstance->Add_Light_Layer(wszLightTag);

		CLight* pLight = m_pGameInstance->Get_Light(wszLightTag);
		_uint iLightListSize = { 0 };
		if (!ReadFile(hFile, &iLightListSize, sizeof(_uint), &dwByte, nullptr))
		{
			CloseHandle(hLightLayerFile);
			CloseHandle(hFile);
			return E_FAIL;
		}
		for (_uint j = 0; j < iLightListSize; j++)
		{

			LIGHT_DESC tagLight_desc = {};
			if (!ReadFile(hFile, &tagLight_desc.vDiffuse, sizeof(_float4), &dwByte, nullptr))
			{
				CloseHandle(hFile);
				CloseHandle(hLightLayerFile);
				return E_FAIL;
			}
			if (!ReadFile(hFile, &tagLight_desc.vAmbient, sizeof(_float4), &dwByte, nullptr))
			{
				CloseHandle(hFile);
				CloseHandle(hLightLayerFile);
				return E_FAIL;
			}

			if (!ReadFile(hFile, &tagLight_desc.vSpecular, sizeof(_float4), &dwByte, nullptr))
			{
				CloseHandle(hFile);
				CloseHandle(hLightLayerFile);
				return E_FAIL;
			}

			if (!ReadFile(hFile, &tagLight_desc.eType, sizeof(_int), &dwByte, nullptr))
			{
				CloseHandle(hFile);
				CloseHandle(hLightLayerFile);
				return E_FAIL;
			}

			if (!ReadFile(hFile, &tagLight_desc.bShadow, sizeof(_bool), &dwByte, nullptr))
			{
				CloseHandle(hFile);
				CloseHandle(hLightLayerFile);
				return E_FAIL;
			}


			switch (tagLight_desc.eType)
			{
			case LIGHT_DESC::TYPE_DIRECTIONAL:
				if (!ReadFile(hFile, &tagLight_desc.vDirection, sizeof(_float4), &dwByte, nullptr))
				{
					CloseHandle(hFile);
					CloseHandle(hLightLayerFile);
					return E_FAIL;
				}


				break;
			case LIGHT_DESC::TYPE_POINT:
				if (!ReadFile(hFile, &tagLight_desc.vPosition, sizeof(_float4), &dwByte, nullptr))
				{
					CloseHandle(hFile);
					CloseHandle(hLightLayerFile);
					return E_FAIL;
				}
				if (!ReadFile(hFile, &tagLight_desc.fRange, sizeof(_float), &dwByte, nullptr))
				{
					CloseHandle(hFile);
					CloseHandle(hLightLayerFile);
					return E_FAIL;
				}
				break;
			case LIGHT_DESC::TYPE_SPOT:
				if (!ReadFile(hFile, &tagLight_desc.vDirection, sizeof(_float4), &dwByte, nullptr))
				{
					CloseHandle(hFile);
					CloseHandle(hLightLayerFile);
					return E_FAIL;
				}
				if (!ReadFile(hFile, &tagLight_desc.vPosition, sizeof(_float4), &dwByte, nullptr))
				{
					CloseHandle(hFile);
					CloseHandle(hLightLayerFile);
					return E_FAIL;
				}
				if (!ReadFile(hFile, &tagLight_desc.fRange, sizeof(_float), &dwByte, nullptr))
				{
					CloseHandle(hFile);
					CloseHandle(hLightLayerFile);
					return E_FAIL;
				}
				if (!ReadFile(hFile, &tagLight_desc.fCutOff, sizeof(_float), &dwByte, nullptr))
				{
					CloseHandle(hFile);
					CloseHandle(hLightLayerFile);
					return E_FAIL;
				}
				if (!ReadFile(hFile, &tagLight_desc.fOutCutOff, sizeof(_float), &dwByte, nullptr))
				{
					CloseHandle(hFile);
					CloseHandle(hLightLayerFile);
					return E_FAIL;
				}
				break;

			}

			m_pGameInstance->Add_Light(wszLightTag, tagLight_desc);


		}






	}

	CloseHandle(hLightLayerFile);



	CloseHandle(hFile);


	return S_OK;
}



void CLevel::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
