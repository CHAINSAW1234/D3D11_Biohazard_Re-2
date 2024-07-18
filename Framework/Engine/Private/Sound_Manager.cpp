#include "Sound_Manager.h"

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Initialize()
{
	FMOD_RESULT eResult = { FMOD_OK };
	eResult = FMOD_System_Create(&m_pSystem, FMOD_VERSION);
	if (eResult != FMOD_OK)
		return E_FAIL;

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	eResult = FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL , NULL);
	if (eResult != FMOD_OK)
		return E_FAIL;
	LoadSoundFile();
	LoadSoundFile_Zombie();

	return S_OK;
}

HRESULT CSound_Manager::Update_Listener(FMOD_3D_ATTRIBUTES& Attributes_desc)
{
	FMOD_RESULT eResult = { FMOD_OK };
	eResult = FMOD_System_Set3DListenerAttributes(m_pSystem, 0, &Attributes_desc.position, &Attributes_desc.velocity, &Attributes_desc.forward, &Attributes_desc.up);
	if (eResult != FMOD_OK)
		return E_FAIL;

	eResult = FMOD_System_Get3DListenerAttributes(m_pSystem, 0, &Attributes_desc.position, &Attributes_desc.velocity, &Attributes_desc.forward, &Attributes_desc.up);
	if (eResult != FMOD_OK)
		return E_FAIL;


	eResult = FMOD_System_Update(m_pSystem);
	if (eResult != FMOD_OK)
		return E_FAIL;

	return S_OK;
}

void CSound_Manager::Update_Sound(const wstring& pSoundKey, SOUND_DESC _SoundTag)
{
	SOUND_DESC* pDesc = Find_Sound(pSoundKey);
	if (nullptr == pDesc)
		return;

	pDesc->bPause = _SoundTag.bPause;
	pDesc->fRange = _SoundTag.fRange;
	pDesc->eMode = _SoundTag.eMode;
	pDesc->fVolume = _SoundTag.fVolume;
	pDesc->vPos = _SoundTag.vPos;
	pDesc->vSpeedDir = _SoundTag.vSpeedDir;

}



HRESULT CSound_Manager::Channel_Pause(_uint iID)
{

	FMOD_RESULT eResult = { FMOD_OK };
	FMOD_BOOL bPaused = { FALSE };
	eResult = FMOD_Channel_GetPaused(m_pChannelArr[iID], &bPaused);
	eResult = FMOD_Channel_SetPaused(m_pChannelArr[iID], ~bPaused);
	eResult = FMOD_System_Update(m_pSystem);
	if (eResult != FMOD_OK)
		return E_FAIL;

	return S_OK;
}

HRESULT CSound_Manager::Change_Channel_Sound(const wstring& SoundKeyTag, _uint iID)
{
	SOUND_DESC* pDesc = Find_Sound(SoundKeyTag);
	if (nullptr == pDesc)
		return E_FAIL;
	Stop_Sound(iID);
	FMOD_RESULT eResult = { FMOD_OK };
	eResult = FMOD_System_PlaySound(m_pSystem, pDesc->pSound, nullptr, FALSE, &m_pChannelArr[iID]);
	if (eResult != FMOD_OK)
		return E_FAIL;
	eResult = FMOD_Channel_SetPaused(m_pChannelArr[iID], pDesc->bPause);
	if (eResult != FMOD_OK)
		return E_FAIL;
	eResult = FMOD_Channel_Set3DAttributes(m_pChannelArr[iID], &pDesc->vPos, &pDesc->vSpeedDir);
	if (eResult != FMOD_OK)
		return E_FAIL;
	if (pDesc->fVolume >= SOUND_MAX)
		pDesc->fVolume = 1.f;
	else if (pDesc->fVolume <= SOUND_MIN)
		pDesc->fVolume = 0.f;
	eResult = FMOD_Channel_SetMode(m_pChannelArr[iID], pDesc->eMode);//FMOD_3D_INVERSEROLLOFF
	if (eResult != FMOD_OK)
		return E_FAIL;
	eResult = FMOD_Channel_Set3DMinMaxDistance(m_pChannelArr[iID], pDesc->fRange.x, pDesc->fRange.y);
	if (eResult != FMOD_OK)
		return E_FAIL;
	eResult = FMOD_Channel_SetVolume(m_pChannelArr[iID], pDesc->fVolume);
	if (eResult != FMOD_OK)
		return E_FAIL;
	
	eResult = FMOD_System_Update(m_pSystem);
	if (eResult != FMOD_OK)
		return E_FAIL;
	return S_OK;
}

HRESULT CSound_Manager::Play_Sound(const wstring& pSoundKey, _uint iID)
{
	SOUND_DESC* pDesc = Find_Sound(pSoundKey);
	if (nullptr == pDesc)
		return E_FAIL;

	FMOD_RESULT eResult = { FMOD_OK };
	FMOD_BOOL bIsPlay = { FALSE };
	eResult = FMOD_Channel_IsPlaying(m_pChannelArr[iID], &bIsPlay);
	if (!bIsPlay|| eResult== FMOD_ERR_INVALID_HANDLE)
	{
		// 재생이 종료되었음
		eResult = FMOD_System_PlaySound(m_pSystem, pDesc->pSound, nullptr, FALSE, &m_pChannelArr[iID]);
		if (eResult != FMOD_OK)
			return E_FAIL;
		eResult = FMOD_Channel_Set3DAttributes(m_pChannelArr[iID], &pDesc->vPos, &pDesc->vSpeedDir);
		if (eResult != FMOD_OK)
			return E_FAIL;
		if (pDesc->fVolume >= SOUND_MAX)
			pDesc->fVolume = 1.f;
		else if (pDesc->fVolume <= SOUND_MIN)
			pDesc->fVolume = 0.f;
		eResult = FMOD_Channel_SetMode(m_pChannelArr[iID], pDesc->eMode);
		if (eResult != FMOD_OK)
			return E_FAIL;
		eResult = FMOD_Channel_SetVolume(m_pChannelArr[iID], pDesc->fVolume);
		if (eResult != FMOD_OK)
			return E_FAIL;

		eResult = FMOD_Channel_Set3DMinMaxDistance(m_pChannelArr[iID], pDesc->fRange.x, pDesc->fRange.y);
		if (eResult != FMOD_OK)
			return E_FAIL;
		_float2 fCheck = { 0.f,0.f };
		eResult = FMOD_Channel_Get3DMinMaxDistance(m_pChannelArr[iID], &fCheck.x, &fCheck.y);
		if (eResult != FMOD_OK)
			return E_FAIL;
		eResult = FMOD_Channel_SetPaused(m_pChannelArr[iID], false);
		if (eResult != FMOD_OK)
			return E_FAIL;
	}
	else
	{
		eResult = FMOD_Channel_Set3DAttributes(m_pChannelArr[iID], &pDesc->vPos, &pDesc->vSpeedDir);
		if (eResult != FMOD_OK)
			return E_FAIL;
		eResult = FMOD_Channel_SetMode(m_pChannelArr[iID], pDesc->eMode);
		if (eResult != FMOD_OK)
			return E_FAIL;
		eResult = FMOD_Channel_GetPaused(m_pChannelArr[iID], &bIsPlay);
		if (eResult != FMOD_OK)
			return E_FAIL;
		if(bIsPlay)
			eResult = FMOD_Channel_SetPaused(m_pChannelArr[iID], false);
		if (eResult != FMOD_OK)
			return E_FAIL;
	}
	eResult = FMOD_System_Update(m_pSystem);
	if (eResult != FMOD_OK)
		return E_FAIL;

	return S_OK;
}

HRESULT CSound_Manager::Play_Sound_Again(const wstring& SoundKeyTag, _uint iID)
{
	SOUND_DESC* pDesc = Find_Sound(SoundKeyTag);
	if (nullptr == pDesc)
		return E_FAIL;

	FMOD_RESULT eResult = { FMOD_OK };
	FMOD_BOOL bIsPlay = { FALSE };

	eResult = FMOD_System_PlaySound(m_pSystem, pDesc->pSound, nullptr, FALSE, &m_pChannelArr[iID]);
	if (eResult != FMOD_OK)
		return E_FAIL;
	eResult = FMOD_Channel_Set3DAttributes(m_pChannelArr[iID], &pDesc->vPos, &pDesc->vSpeedDir);
	if (eResult != FMOD_OK)
		return E_FAIL;
	if (pDesc->fVolume >= SOUND_MAX)
		pDesc->fVolume = 1.f;
	else if (pDesc->fVolume <= SOUND_MIN)
		pDesc->fVolume = 0.f;
	eResult = FMOD_Channel_SetMode(m_pChannelArr[iID], pDesc->eMode);
	if (eResult != FMOD_OK)
		return E_FAIL;
	eResult = FMOD_Channel_SetVolume(m_pChannelArr[iID], pDesc->fVolume);
	if (eResult != FMOD_OK)
		return E_FAIL;

	eResult = FMOD_Channel_Set3DMinMaxDistance(m_pChannelArr[iID], pDesc->fRange.x, pDesc->fRange.y);
	if (eResult != FMOD_OK)
		return E_FAIL;
	_float2 fCheck = { 0.f,0.f };
	eResult = FMOD_Channel_Get3DMinMaxDistance(m_pChannelArr[iID], &fCheck.x, &fCheck.y);
	if (eResult != FMOD_OK)
		return E_FAIL;
	eResult = FMOD_Channel_SetPaused(m_pChannelArr[iID], false);
	if (eResult != FMOD_OK)
		return E_FAIL;


	eResult = FMOD_System_Update(m_pSystem);
	if (eResult != FMOD_OK)
		return E_FAIL;

	return S_OK;
}

HRESULT CSound_Manager::PlayBGM(_uint iID, const wstring& pSoundKey)
{

	FMOD_RESULT eResult = { FMOD_OK };	
	FMOD_BOOL bIsPlay = { FALSE };
	
	if (iID > SOUND_CHANNEL_MAX)
		return E_FAIL;
	SOUND_DESC* pDesc = Find_Sound(pSoundKey);
	if (nullptr == pDesc)
		return E_FAIL;
	eResult = FMOD_Channel_IsPlaying(m_pChannelArr[iID], &bIsPlay);
	if (!bIsPlay || eResult == FMOD_ERR_INVALID_HANDLE)
	{
		eResult = FMOD_System_PlaySound(m_pSystem, pDesc->pSound, nullptr, FALSE, &m_pChannelArr[iID]);
		if (eResult != FMOD_OK)
			return E_FAIL;
	}
	eResult = FMOD_Channel_SetMode(m_pChannelArr[iID], FMOD_DEFAULT);
	if (eResult != FMOD_OK)
		return E_FAIL;
	eResult = FMOD_System_Update(m_pSystem);
	if (eResult != FMOD_OK)
		return E_FAIL;
	return S_OK;
}

HRESULT CSound_Manager::Stop_Sound(_uint iID)
{

	FMOD_RESULT eResult = { FMOD_OK };
	eResult = FMOD_Channel_Stop(m_pChannelArr[iID]);
	if (eResult != FMOD_OK)
		return E_FAIL;
	eResult = FMOD_System_Update(m_pSystem);
	if (eResult != FMOD_OK)
		return E_FAIL;
	return S_OK;
}

HRESULT CSound_Manager::Stop_All()
{
	FMOD_RESULT eResult = { FMOD_OK };
	for (int i = 0; i < SOUND_CHANNEL_MAX; ++i)
	{
		eResult = FMOD_Channel_Stop(m_pChannelArr[i]);
		if (eResult != FMOD_OK)
			return E_FAIL;
	}
	eResult = FMOD_System_Update(m_pSystem);
	if (eResult != FMOD_OK)
		return E_FAIL;
	return S_OK;
}

void CSound_Manager::LoadSoundFile()
{
	_tfinddata64_t fd;
	__int64 handle = _tfindfirst64(L"../Bin/Resources/Sound/*.*", &fd);
	if (handle == -1 || handle == 0)
		return;

	_int iResult = 0;

	char szCurPath[MAX_PATH] = "../Bin/Resources/Sound/";
	char szFullPath[MAX_PATH] = "";
	char szFilename[MAX_PATH];
	while (iResult != -1)
	{
		WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, szFilename);
		SOUND_DESC* Sound_desc = new SOUND_DESC;
		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_3D, 0, &Sound_desc->pSound);//|FMOD_3D_HEADRELATIVE  | FMOD_3D_INVERSEROLLOFF
		if (eRes == FMOD_OK)
		{
			_int iLength = (_int)strlen(szFilename) + 1;

			TCHAR pSoundKey[MAX_PATH] = { TEXT("") };
			MultiByteToWideChar(CP_ACP, 0, szFilename, iLength, pSoundKey, iLength);
			m_Soundmap.emplace(pSoundKey, Sound_desc);
		}
		else
			delete Sound_desc;
		iResult = _tfindnext64(handle, &fd);
	}
	FMOD_System_Update(m_pSystem);

	_findclose(handle);
}

void CSound_Manager::LoadSoundFile_Zombie()
{
	_tfinddata64_t fd;
	__int64 handle = _tfindfirst64(L"../Bin/Resources/Sound/em/*.*", &fd);
	if (handle == -1 || handle == 0)
		return;

	_int iResult = 0;

	char szCurPath[MAX_PATH] = "../Bin/Resources/Sound/em/";
	char szFullPath[MAX_PATH] = "";
	char szFilename[MAX_PATH];
	while (iResult != -1)
	{
		WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, szFilename);
		SOUND_DESC* Sound_desc = new SOUND_DESC;
		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_3D, 0, &Sound_desc->pSound);//|FMOD_3D_HEADRELATIVE  | FMOD_3D_INVERSEROLLOFF
		if (eRes == FMOD_OK)
		{
			_int iLength = (_int)strlen(szFilename) + 1;

			TCHAR pSoundKey[MAX_PATH] = { TEXT("") };
			MultiByteToWideChar(CP_ACP, 0, szFilename, iLength, pSoundKey, iLength);
			m_Soundmap.emplace(pSoundKey, Sound_desc);
		}
		else
			delete Sound_desc;
		iResult = _tfindnext64(handle, &fd);
	}
	FMOD_System_Update(m_pSystem);

	_findclose(handle);
}

SOUND_DESC* CSound_Manager::Find_Sound(const wstring& SoundKeyTag)
{

	auto& iter = m_Soundmap.find(SoundKeyTag);

	if (iter == m_Soundmap.end())
		return nullptr;

	return iter->second;

}

CSound_Manager* CSound_Manager::Create()
{
	CSound_Manager* pInstance = new CSound_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CSound_Manager"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_Soundmap)
	{
		FMOD_Sound_Release(Pair.second->pSound);
		Safe_Delete(Pair.second);
	}
	m_Soundmap.clear();
	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}
