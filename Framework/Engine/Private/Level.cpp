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

HRESULT CLevel::Load_Layer(const wstring& strFilePath, _uint iLevel)
{


	wstring strPerfectFilePath = strFilePath + TEXT("\\\\Layer_List.dat");



	HANDLE		hFile = CreateFile(strPerfectFilePath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;


	DWORD	dwByte(0);

	_uint iObjectNum = { 0 };
	if (!ReadFile(hFile, &iObjectNum, sizeof(_uint), &dwByte, nullptr))
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	for (_uint i = 0; iObjectNum > i; ++i)
	{
		string strLayer;
		_uint dwLen = { 0 };
		if (!ReadFile(hFile, &dwLen, sizeof(_uint), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
		char* szLayerListName = new char[dwLen / sizeof(char) + 1];
		if (!ReadFile(hFile, szLayerListName, dwLen, &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
		szLayerListName[dwLen / sizeof(char)] = '\0';
		strLayer = szLayerListName;
		delete[] szLayerListName;

		_tchar	szTemp[MAX_PATH] = { L"" };
		MultiByteToWideChar(CP_ACP, 0, strLayer.c_str(), (_uint)strlen(strLayer.c_str()), szTemp, MAX_PATH);
		if (FAILED(m_pGameInstance->Add_Layer(iLevel, szTemp)))
			continue;


		if (FAILED(Load_Object(strFilePath ,szTemp, iLevel)))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}




	}
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLevel::Load_Object(const wstring& strFilePath, const wstring& strLayerName, _uint iLevel)
{
	wstring strLayerFile = strFilePath+TEXT("\\\\")+strLayerName + TEXT(".dat");
	_uint iMonsterNum = { 0 };
	HANDLE		hFile = CreateFile(strLayerFile.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD	dwByte(0);

	_uint iObjectNum = { 0 };
	if (!ReadFile(hFile, &iObjectNum, sizeof(_uint), &dwByte, nullptr))
		return E_FAIL;

	for (_uint i = 0; iObjectNum > i; ++i)
	{
		_uint iLength = { 0 };


		CGameObject::GAMEOBJECT_DESC ObjectDesc = {};

		if (!ReadFile(hFile, &ObjectDesc.bAnim, sizeof(_bool), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (!ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
		wchar_t* strModelComponent = new wchar_t[iLength / sizeof(wchar_t) + 1];
		if (!ReadFile(hFile, strModelComponent, iLength, &dwByte, nullptr))
		{
			delete[] strModelComponent;

			CloseHandle(hFile);
			return E_FAIL;
		}
		strModelComponent[iLength / sizeof(wchar_t)] = L'\0';
		ObjectDesc.strModelComponent = strModelComponent;
		delete[] strModelComponent;


		if (!ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
		wchar_t* strObjectPrototype = new wchar_t[iLength / sizeof(wchar_t) + 1];
		if (!ReadFile(hFile, strObjectPrototype, iLength, &dwByte, nullptr))
		{
			delete[] strObjectPrototype;

			CloseHandle(hFile);
			return E_FAIL;
		}
		strObjectPrototype[iLength / sizeof(wchar_t)] = L'\0';
		ObjectDesc.strObjectPrototype = strObjectPrototype;
		delete[] strObjectPrototype;


		if (!ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
		char* strGamePrototypeName = new char[iLength / sizeof(char) + 1];
		if (!ReadFile(hFile, strGamePrototypeName, iLength, &dwByte, nullptr))
		{
			delete[] strGamePrototypeName;

			CloseHandle(hFile);
			return E_FAIL;
		}
		strGamePrototypeName[iLength / sizeof(char)] = '\0';
		ObjectDesc.strGamePrototypeName = strGamePrototypeName;
		delete[] strGamePrototypeName;

		_uint iIndex = { 0 };
		if (!ReadFile(hFile, &iIndex, sizeof(_uint), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (!ReadFile(hFile, &ObjectDesc.worldMatrix, sizeof(_float4x4), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		ObjectDesc.iIndex = iMonsterNum++;


		_uint iSize;
		if (!ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, NULL)) {
			CloseHandle(hFile);
			return E_FAIL;
		}


		ObjectDesc.BelongIndexs.resize(iSize);



		for (size_t i = 0; i < iSize; i++)
		{
			_int iNum = { 0 };

			if (!ReadFile(hFile, &iNum, sizeof(_int), &dwByte, NULL)) {
				CloseHandle(hFile);
				return E_FAIL;
			}

			ObjectDesc.BelongIndexs[i] = iNum;


		}

		if (!ReadFile(hFile, &ObjectDesc.iRegionDir, sizeof(_int), &dwByte, NULL)) {
			CloseHandle(hFile);
			return E_FAIL;
		}


		if (FAILED(m_pGameInstance->Add_Clone(iLevel, strLayerName, ObjectDesc.strObjectPrototype, &ObjectDesc)))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}


	}
	CloseHandle(hFile);
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
