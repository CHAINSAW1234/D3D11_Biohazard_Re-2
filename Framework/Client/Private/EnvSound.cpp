#include "stdafx.h"
#include "EnvSound.h"
#include"Player.h"
CEnvSound::CEnvSound(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject{ pDevice, pContext }
{
}

CEnvSound::CEnvSound(const CEnvSound & rhs)
	: CGameObject{ rhs }
{

}

HRESULT CEnvSound::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEnvSound::Initialize(void * pArg)
{
	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;		

	if (FAILED(m_pGameInstance->Add_Object_Sound(m_pTransformCom, ENV_CHENNEL_END)))
		return E_FAIL;
	for (size_t i = 0; i < ENV_CHENNEL_END; i++)
	{
		_float fMax = m_pGameInstance->GetRandom_Real(10.f, 30.f);

		m_pGameInstance->Set_Volume_3D(m_pTransformCom, i, 0.2f);
		m_pGameInstance->Set_Distance_3D(m_pTransformCom, i, 1.f, fMax);
	}
	return S_OK;
}

void CEnvSound::Start()
{
	m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0));


}

void CEnvSound::Tick(_float fTimeDelta)
{
	m_pGameInstance->BGMVolume_2D(m_fCurSound, BGM_2D);

	Set_SoundRegion(m_pPlayer->Get_Player_Region());
	_bool bPlaying = { false };
	m_pGameInstance->IsPlaying_2D(BGM_2D, &bPlaying);
	if (m_fRegionChangeTimeDelay > 0.f)
	{
		m_fRegionChangeTimeDelay -= fTimeDelta;
		m_fCurSound = Lerp(m_fCurSound, m_fMinSound, fTimeDelta * 3.f);
		//m_pGameInstance->PlayBGM_2D(m_BGMSoundMap[m_iPreRegion], BGM_2D);
		if (!bPlaying)
			m_pGameInstance->PlaySoundEffect_2D(TEXT("location"), m_BGMSoundMap[m_iPreRegion], BGM_2D, m_fCurSound);
	}
	else if (m_fRegionChangeTimeDelay < 0.f)
	{
		m_fRegionChangeTimeDelay = 0.f;
		m_fRegionChangeTimeDelay2 = 2.f;
		m_iPreRegion = m_iCurRegion;
	}

	if (m_fRegionChangeTimeDelay2 > 0.f)
	{
		m_fRegionChangeTimeDelay2 -= fTimeDelta;
		m_fCurSound = Lerp(m_fCurSound, m_fMaxSound, fTimeDelta * 3.f);
		//m_pGameInstance->PlayBGM_2D(m_BGMSoundMap[m_iPreRegion], BGM_2D);
		if (!bPlaying)
			m_pGameInstance->PlaySoundEffect_2D(TEXT("location"), m_BGMSoundMap[m_iPreRegion], BGM_2D, m_fCurSound);

	}
	else if (m_fRegionChangeTimeDelay2 < 0.f)
		m_fRegionChangeTimeDelay2 = 0.f;


	if (m_fRegionChangeTimeDelay == 0.f && m_fRegionChangeTimeDelay2 == 0.f && !bPlaying)
		m_pGameInstance->PlaySoundEffect_2D(TEXT("location"), m_BGMSoundMap[m_iPreRegion], BGM_2D, m_fCurSound);
	//_int iRandom = m_pGameInstance->GetRandom_Int(0,5);
	//for (_int i = 0; i < m_EnvSoundMap[m_iCurRegion].size(); i++)
	//{
	//	if (!m_pGameInstance->Is_Playing_Sound(m_pTransformCom, ENV_CHENNEL_LOCAL0)&& iRandom>1)
	//	{
	//		m_pTransformCom->Set_WorldMatrix(m_EnvSoundMap[m_iCurRegion][i].WorldMatrix);
	//		m_pGameInstance->Change_Sound_3D(m_pTransformCom, m_EnvSoundMap[m_iCurRegion][i].wstrSoundName, ENV_CHENNEL_LOCAL0, true);
	//	}
	//	else if (!m_pGameInstance->Is_Playing_Sound(m_pTransformCom, ENV_CHENNEL_LOCAL1)&&iRandom >4)
	//	{
	//		m_pTransformCom->Set_WorldMatrix(m_EnvSoundMap[m_iCurRegion][i].WorldMatrix);
	//		m_pGameInstance->Change_Sound_3D(m_pTransformCom, m_EnvSoundMap[m_iCurRegion][i].wstrSoundName, ENV_CHENNEL_LOCAL1, true);
	//	}
	//	else if (!m_pGameInstance->Is_Playing_Sound(m_pTransformCom, ENV_CHENNEL_LOCAL2)&& iRandom > 3)
	//	{
	//		m_pTransformCom->Set_WorldMatrix(m_EnvSoundMap[m_iCurRegion][i].WorldMatrix);
	//		m_pGameInstance->Change_Sound_3D(m_pTransformCom, m_EnvSoundMap[m_iCurRegion][i].wstrSoundName, ENV_CHENNEL_LOCAL2, true);
	//	}
	//	else if (!m_pGameInstance->Is_Playing_Sound(m_pTransformCom, ENV_CHENNEL_LOCAL3)&& iRandom > 2)
	//	{
	//		m_pTransformCom->Set_WorldMatrix(m_EnvSoundMap[m_iCurRegion][i].WorldMatrix);
	//		m_pGameInstance->Change_Sound_3D(m_pTransformCom, m_EnvSoundMap[m_iCurRegion][i].wstrSoundName, ENV_CHENNEL_LOCAL3, true);
	//	}
	//	//m_pGameInstance->Change_Sound_3D(m_pTransformCom, m_EnvSoundMap[m_iCurRegion][i].wstrSoundName, ENV_CHENNEL_LOCAL0, true);
	//}


	return;
}

void CEnvSound::Late_Tick(_float fTimeDelta)
{

}

HRESULT CEnvSound::Render()
{


	return S_OK;
}



HRESULT CEnvSound::Add_Components()
{
	HANDLE		hSoundLayerFile = CreateFile(TEXT("../Bin/Data/Level_InteractObj/BGM_Sound.dat"), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hSoundLayerFile)
		return E_FAIL;

	DWORD	dwByte = { 0 };
	for (_int i = 0; i < 51; i++)
	{
		_int iSize = { 0 };
		if (!ReadFile(hSoundLayerFile, &iSize, sizeof(_int), &dwByte, nullptr))
		{
			CloseHandle(hSoundLayerFile);
			return E_FAIL;
		}
		for (_int j = 0; j < iSize; j++)
		{
			CEnvSound::ENVSOUND_DESC newdesc = {};
			if (!ReadFile(hSoundLayerFile, &newdesc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr))
			{
				CloseHandle(hSoundLayerFile);
				return E_FAIL;
			}

			_uint iLength = { 0 };
			if (!ReadFile(hSoundLayerFile, &iLength, sizeof(_uint), &dwByte, nullptr))
			{
				CloseHandle(hSoundLayerFile);
				return E_FAIL;
			}
			wchar_t* strName = new wchar_t[iLength / sizeof(wchar_t) + 1];
			if (!ReadFile(hSoundLayerFile, strName, iLength, &dwByte, nullptr))
			{
				CloseHandle(hSoundLayerFile);
				return E_FAIL;
			}
			strName[iLength / sizeof(wchar_t)] = L'\0';
			newdesc.wstrSoundName = strName;
			delete[] strName;
			m_BGMSoundMap.insert({ i,newdesc.wstrSoundName });
		}
	}
	CloseHandle(hSoundLayerFile);
	
	HANDLE		hSoundFile = CreateFile(TEXT("../Bin/Data/Level_InteractObj/Region_Sound.dat"), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hSoundFile)
		return E_FAIL;

	//DWORD	dwByte = { 0 };
	for (_int i = 0; i < 51; i++)
	{
		_int iSize = { 0 };
		if (!ReadFile(hSoundFile, &iSize, sizeof(_int), &dwByte, nullptr))
		{
			CloseHandle(hSoundFile);
			return E_FAIL;
		}
		for (_int j = 0; j < iSize; j++)
		{
			CEnvSound::ENVSOUND_DESC newdesc = {};
			if (!ReadFile(hSoundFile, &newdesc.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr))
			{
				CloseHandle(hSoundFile);
				return E_FAIL;
			}

			_uint iLength = { 0 };
			if (!ReadFile(hSoundFile, &iLength, sizeof(_uint), &dwByte, nullptr))
			{
				CloseHandle(hSoundFile);
				return E_FAIL;
			}
			wchar_t* strName = new wchar_t[iLength / sizeof(wchar_t) + 1];
			if (!ReadFile(hSoundFile, strName, iLength, &dwByte, nullptr))
			{
				CloseHandle(hSoundFile);
				return E_FAIL;
			}
			strName[iLength / sizeof(wchar_t)] = L'\0';
			newdesc.wstrSoundName = strName;
			delete[] strName;

			m_EnvSoundMap[i].emplace_back(newdesc);
		}
	}
	CloseHandle(hSoundFile);


	return S_OK;
}



HRESULT CEnvSound::Bind_ShaderResources()
{
	
	return S_OK;
}

CEnvSound * CEnvSound::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEnvSound*		pInstance = new CEnvSound(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEnvSound"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CEnvSound::Clone(void * pArg)
{
	CEnvSound*		pInstance = new CEnvSound(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CEnvSound"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnvSound::Free()
{
	__super::Free();

}
