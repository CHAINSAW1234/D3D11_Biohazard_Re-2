#include "Sound_Manager_2D.h"

CSound_Manager_2D::CSound_Manager_2D()
{
	m_pSystem = nullptr;
}


HRESULT CSound_Manager_2D::Initialize()
{
	FMOD_System_Create(&m_pSystem, FMOD_VERSION);

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	FMOD_System_Init(m_pSystem, SOUND_CHANNEL_SIZE, FMOD_INIT_NORMAL, NULL);

	LoadSoundFile();

	return S_OK;
}

void CSound_Manager_2D::ApplyLowPass_2D(_bool bSet, _int eID)
{
	if (m_bSetLowPass == bSet)
		return;

	if (bSet)
	{
		FMOD_Channel_AddDSP(*m_pChannelArr, eID, m_LowPassFilter);
		m_bSetLowPass = true;
	}
	else
	{
		FMOD_Channel_RemoveDSP(*m_pChannelArr, m_LowPassFilter);
		m_bSetLowPass = false;
	}

}

void CSound_Manager_2D::ApplyChorus_2D(_int eID)
{
	FMOD_Channel_AddDSP(*m_pChannelArr, eID, m_pChorusFilter);
}

void CSound_Manager_2D::ClearChorus_2D(_int eID)
{
	FMOD_Channel_RemoveDSP(*m_pChannelArr, m_pChorusFilter);
}

void CSound_Manager_2D::CrossFade_2D(_int eIDA, _int eIDB, _float fFadeOutValue, _float fFadeInValue)
{
	FMOD_Channel_AddDSP(*m_pChannelArr, eIDA, m_pFaderForA);
	FMOD_Channel_AddDSP(*m_pChannelArr, eIDB, m_pFaderForB);

	FMOD_DSP_SetParameterFloat(m_pFaderForA, FMOD_DSP_FADER_GAIN, fFadeOutValue);
	FMOD_DSP_SetParameterFloat(m_pFaderForB, FMOD_DSP_FADER_GAIN, fFadeInValue);
}


HRESULT CSound_Manager_2D::Set_Volume_2D(_int eID, _float _vol)
{
	m_volume = _vol;

	if (m_volume > SOUND_MIN && m_volume < SOUND_MAX) 
	{
		m_volume = _vol;
	}

	else if (m_volume <= SOUND_MIN)
	{
		m_volume = SOUND_MIN;
	}

	else if (m_volume >= SOUND_MAX)
	{
		m_volume = SOUND_MAX;
	}


	FMOD_Channel_SetVolume(m_pChannelArr[eID], m_volume);

	return S_OK;
}



int CSound_Manager_2D::VolumeUp_2D(_int eID, _float _vol)
{
	if (m_volume < SOUND_MAX) {
		m_volume += _vol;
	}

	FMOD_Channel_SetVolume(m_pChannelArr[eID], m_volume);

	return 0;
}


int CSound_Manager_2D::VolumeDown_2D(_int eID, _float _vol)
{
	if (m_volume > SOUND_MIN) {
		m_volume -= _vol;
	}

	FMOD_Channel_SetVolume(m_pChannelArr[eID], m_volume);

	return 0;
}


int CSound_Manager_2D::BGMVolumeUp_2D(_float _vol , _int _CHID)
{
	if (m_BGMvolume < SOUND_MAX) {
		m_BGMvolume += _vol;
	}

	FMOD_Channel_SetVolume(m_pChannelArr[_CHID], m_BGMvolume);

	return 0;
}


int CSound_Manager_2D::BGMVolumeDown_2D(_float _vol, _int _CHID)
{
	if (m_BGMvolume > SOUND_MIN) 
{
		m_BGMvolume -= _vol;
	}

	FMOD_Channel_SetVolume(m_pChannelArr[_CHID], m_BGMvolume);

	return 0;
}


int CSound_Manager_2D::Pause_2D(_int eID, _bool bPause)
{
	//m_bPause = !m_bPause;
	FMOD_Channel_SetPaused(m_pChannelArr[eID], bPause);

	return 0;
}

void CSound_Manager_2D::PauseAll_2D()
{
	FMOD_BOOL bPlay = TRUE;
	for (int i = 0; i < CHID_END; i++)
	{
		FMOD_Channel_IsPlaying(m_pChannelArr[i], &bPlay);
		if (bPlay == TRUE)
		{
			FMOD_Channel_SetPaused(m_pChannelArr[i], true);
		}
	}
}

void CSound_Manager_2D::ReplayAll_2D()
{
	FMOD_BOOL bPause = TRUE;
	for (int i = 0; i < CHID_END; i++)
	{
		FMOD_Channel_GetPaused(m_pChannelArr[i], &bPause);
		if (bPause == TRUE)
		{
			FMOD_Channel_SetPaused(m_pChannelArr[i], false);
		}
	}
}


void CSound_Manager_2D::PlayMySound_2D(wstring TypeKey, wstring FileKey, _int eID, _float _vol)
{
	map<pair<wstring, wstring>, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSoundSeparate.begin(), m_mapSoundSeparate.end(), [&](const pair<const pair<wstring, wstring>, FMOD_SOUND*>& iter)
		{
			if (TypeKey != iter.first.first || FileKey != iter.first.second)
			{
				return false;
			}
			else if (TypeKey == iter.first.first && FileKey == iter.first.second)
			{
				return true;
			}
		}
	);

	if (iter == m_mapSoundSeparate.end())
		return;

	FMOD_BOOL bPlay = FALSE;
	FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay);
	if(bPlay == FALSE)
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[eID]);

		FMOD_Channel_SetVolume(m_pChannelArr[eID], _vol);
	}

	FMOD_System_Update(m_pSystem);
}


void CSound_Manager_2D::PlayBGM_2D(wstring FileKey, _int _CHID)
{
	map<pair<wstring, wstring>, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSoundSeparate.begin(), m_mapSoundSeparate.end(), [&](const pair<const pair<wstring, wstring>, FMOD_SOUND*>& iter)
		{
			if (L"BGM" != iter.first.first || FileKey != iter.first.second)
			{
				return false;
			}
			else if (L"BGM" == iter.first.first && FileKey == iter.first.second)
			{
				return true;
			}
		});

	if (iter == m_mapSoundSeparate.end())
		return;

	FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[_CHID]);
	FMOD_Channel_SetMode(m_pChannelArr[_CHID], FMOD_LOOP_NORMAL);
	FMOD_Channel_SetVolume(m_pChannelArr[_CHID], 0.25f);
	FMOD_System_Update(m_pSystem);
}

void CSound_Manager_2D::PlayLoopSound_2D(wstring TypeKey, wstring FileKey, _int eID, _float _vol)
{
	map<pair<wstring, wstring>, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSoundSeparate.begin(), m_mapSoundSeparate.end(), [&](const pair<const pair<wstring, wstring>, FMOD_SOUND*>& iter)
		{
			if (TypeKey != iter.first.first || FileKey != iter.first.second)
			{
				return false;
			}
			else if (TypeKey == iter.first.first && FileKey == iter.first.second)
			{
				return true;
			}
		});

	if (iter == m_mapSoundSeparate.end())
		return;

	FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[eID]);
	FMOD_Channel_SetMode(m_pChannelArr[eID], FMOD_LOOP_NORMAL);
	FMOD_Channel_SetVolume(m_pChannelArr[eID], _vol);
	FMOD_System_Update(m_pSystem);
}

void CSound_Manager_2D::StopSound_2D(_int eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
	m_pChannelArr[eID] = nullptr;
}


void CSound_Manager_2D::StopAll_2D()
{
	for (int i = 1; i < SOUND_CHANNEL_SIZE; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}

void CSound_Manager_2D::MuteAll_2D()
{
	for (int i = 1; i < SOUND_CHANNEL_SIZE; ++i)
		FMOD_Channel_SetVolume(m_pChannelArr[i], 0.0f);
}

void CSound_Manager_2D::ResetAllSlowMotion_2D()
{
	for (int i = 0; i < CHID_END; i++)
	{
		FMOD_Channel_RemoveDSP(m_pChannelArr[i], m_pPitch);
	}
	FMOD_System_Update(m_pSystem);
}

_bool CSound_Manager_2D::IsPlaying_2D(_int eID,_bool* boolean)
{
	_bool Result = false;
	FMOD_BOOL Bool = FALSE;
	FMOD_Channel_IsPlaying(m_pChannelArr[eID],&Bool);
	Result = (_bool)Bool;
	*boolean = Result;
	return Result;
}


void CSound_Manager_2D::LoadSoundFile()
{
	_tfinddata64_t TypePathFinder;
	_tfinddata64_t FileNameFinder;

	int iTypeResult = 0;
	int iFileResult = 0;

	wstring szCurPath = TEXT("../Bin/Resources/Sound/");
	wstring szFilePath = TEXT("");
	wstring szFullPath = TEXT("");


	__int64 Typehandle = _tfindfirst64(L"../Bin/Resources/Sound/*", &TypePathFinder);

	while (wcscmp(TypePathFinder.name, L".") == 0 || wcscmp(TypePathFinder.name, L"..") == 0) 
	{
		if (_tfindnext64(Typehandle, &TypePathFinder) != 0) {
			return;
		}
	}

	__int64 Filehandle = 0;

	while (iTypeResult != -1)
	{
		szFilePath = szCurPath + TypePathFinder.name + L"/*";
		szFullPath = szCurPath + TypePathFinder.name + L"/";

		Filehandle = _tfindfirst64(szFilePath.c_str(), &FileNameFinder);

		while (wcscmp(FileNameFinder.name, L".") == 0 || wcscmp(FileNameFinder.name, L"..") == 0)
		{
			if (_tfindnext64(Filehandle, &FileNameFinder) != 0)
			{
				return;
			}
		}

		szFullPath = szCurPath + TypePathFinder.name + L"/" + FileNameFinder.name;

		iFileResult = 0;
		while (iFileResult != -1)
		{
			szFullPath = szCurPath + TypePathFinder.name + L"/" + FileNameFinder.name;
			int size_needed = WideCharToMultiByte(CP_UTF8, 0, &szFullPath[0], (int)szFullPath.size(), NULL, 0, NULL, NULL);
			string str(size_needed, 0);
			WideCharToMultiByte(CP_UTF8, 0, &szFullPath[0], (int)szFullPath.size(), &str[0], size_needed, NULL, NULL);
			char* cstr = &str[0];

			FMOD_SOUND* pSound = nullptr;
			FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, cstr, FMOD_DEFAULT, 0, &pSound);

			m_mapSoundSeparate.emplace(make_pair(TypePathFinder.name, FileNameFinder.name), pSound);

			iFileResult = _tfindnext64(Filehandle, &FileNameFinder);//다음파일이 있다없다
		}

		FMOD_System_Update(m_pSystem);
		iTypeResult = _tfindnext64(Typehandle, &TypePathFinder);//다음파일이 있다없다
	}

	_findclose(Filehandle);
	_findclose(Typehandle);
}


CSound_Manager_2D* CSound_Manager_2D::Create()
{
	CSound_Manager_2D* pInstance = new CSound_Manager_2D();

	if (FAILED(pInstance->Initialize()))
	{
		_ASSERT_EXPR(FALSE, TEXT("Failed To Create : CSound_Manager_2D"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CSound_Manager_2D::Free()
{
	FMOD_DSP_Release(m_LowPassFilter);
	FMOD_DSP_Release(m_pChorusFilter);
	FMOD_DSP_Release(m_pFaderForA);
	FMOD_DSP_Release(m_pFaderForB);

	for (auto& Mypair : m_mapSoundSeparate)
	{
		FMOD_Sound_Release(Mypair.second);
	}
	m_mapSoundSeparate.clear();

	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);

}
