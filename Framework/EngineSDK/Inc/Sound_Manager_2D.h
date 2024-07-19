#pragma once
#include "Base.h""
#include "Engine_Defines.h"

#include "fmod.h"
#include "fmod.hpp"
#include "fileapi.h"
BEGIN(Engine)

#define SOUND_MAX 1.0f
#define SOUND_MIN 0.0f
#define SOUND_DEFAULT 0.25f
#define SOUND_WEIGHT 0.1f

class ENGINE_DLL CSound_Manager_2D final : public CBase
{
public:
private:
	CSound_Manager_2D();
	~CSound_Manager_2D() = default;

public:
	HRESULT Initialize();

	HRESULT Set_Volume_2D(_int eID, _float _vol);
	int  VolumeUp_2D(_int eID, _float _vol);
	int  VolumeDown_2D(_int eID, _float _vol);
	int  BGMVolumeUp_2D(_float _vol, _int _CHID);
	int  BGMVolumeDown_2D(_float _vol, _int _CHID);
	int  Pause_2D(_int eID, _bool bPause);
	void PauseAll_2D();
	void ReplayAll_2D();
	void PlayMySound_2D(wstring TypeKey,wstring FileKey, _int eID, _float _vol);
	void PlayBGM_2D(wstring FileKey, _int _CHID);
	void PlayLoopSound_2D(wstring TypeKey, wstring FileKey, _int eID, _float _vol);
	void StopSound_2D(_int eID);
	void StopAll_2D();
	void MuteAll_2D();
	void ResetAllSlowMotion_2D();
	_bool IsPlaying_2D(_int eID,_bool* boolean);
	void ApplyLowPass_2D(_bool bSet, _int eID);
	void ApplyChorus_2D(_int eID);
	void ClearChorus_2D(_int eID);
	void CrossFade_2D(_int eIDA, _int eIDB, _float fFadeOutValue , _float fFadeInValue);

public:
	void SetSlowMotionValue_2D(_bool _bSlowMotion) { m_bSlowMotion = _bSlowMotion; }

private:
	_float m_volume = SOUND_DEFAULT;
	_float m_BGMvolume = SOUND_DEFAULT;
	_bool m_bSetLowPass = { false };
	FMOD_BOOL m_bool;
	_bool m_bSlowMotion = { false };

private:
	void LoadSoundFile();

private:
	//static CSound_Manager_2D* m_pInstance;
	// 사운드 리소스 정보를 갖는 객체 
	//map<TCHAR*, FMOD_SOUND*>	m_mapSound;
	map< pair<wstring, wstring>, FMOD_SOUND*> m_mapSoundSeparate;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL*				m_pChannelArr[1024];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM*				m_pSystem;

	_bool						m_bPause = false;

	FMOD_DSP*					m_LowPassFilter = { nullptr };
	FMOD_DSP*					m_pChorusFilter = { nullptr };

	FMOD_DSP*					m_pFaderForA = { nullptr };
	FMOD_DSP*					m_pFaderForB = { nullptr };

	FMOD_DSP*					m_pPitch = { nullptr };


public:
	static CSound_Manager_2D* Create();
	virtual void Free() override;
};

END