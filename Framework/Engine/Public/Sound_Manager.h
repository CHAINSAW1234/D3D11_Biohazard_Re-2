#pragma once

#include "Base.h"

BEGIN(Engine)

class CSound_Manager final : public CBase
{
private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT										Initialize();
	
private:
	HRESULT										Initialize_SoundDesc_2D();

public:
	//리스너 설정
	HRESULT										Update_Listener(class CTransform* pTransform, _float3 vVelocity);
	HRESULT										PlayBGM(_uint iChannelIndex, const wstring& strSoundTag);
	HRESULT										Stop_Sound_2D(_uint iID);
	HRESULT										Stop_Sound_3D(class CTransform* pTransform, _uint iSoundIndex);
	HRESULT										Stop_All();

public:
	void										Change_Sound_2D(_uint iChannelIndex, const wstring& strSoundTag);
	void										Change_Sound_3D(class CTransform* pTransform, const wstring& strSoundTag, _uint iSoundIndex);

	void										Set_Volume_2D(_uint iChannelIndex, _float fVolume);
	void										Set_Volume_3D(class CTransform* pTransform, _uint iSoundIndex, _float fVolume);
	void										Set_Pause_2D(_uint iChannelIndex, _bool isPause);
	void										Set_Pause_3D(class CTransform* pTransform, _uint iSoundIndex, _bool isPause);


	void										Set_Distance_3D(class CTransform* pTransform, _uint iSoundIndex, _float fMinDistance, _float fMaxDistance);

	HRESULT										Update_Sounds();

private:
	HRESULT										Update_Sounds_Position();
	HRESULT										Update_Sounds_Volume();
	HRESULT										Update_Sounds_Mode();
	HRESULT										Update_Sounds_Paused();
	HRESULT										Update_Sounds_Distance();

	HRESULT										Update_Sounds_System();
public:
	HRESULT										Add_Object_Sound(class CTransform* pTransform, _uint iNumChannel);

private:
	void												LoadSoundFile();
	void												LoadSoundFile_Zombie();
	FMOD_SOUND*								Find_Sound(const wstring& strSoundTag);
	SOUND_DESC*								Find_SoundDesc_2D(_uint iChannelIndex);
	SOUND_DESC*								Find_SoundDesc_3D(class CTransform* pTransform, _uint iSoudIndex);
private:
	// 사운드 리소스 정보를 갖는 객체 
	map<const wstring, FMOD_SOUND*>				m_Soundmap;
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL*								m_pChannelArr[SOUND_CHANNEL_MAX] = {};
	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM*								m_pSystem = { nullptr };

	vector<SOUND_DESC>													m_Sound_Descs_2D;
	unordered_map<class CTransform*, vector<SOUND_DESC>>				m_Sound_Descs_3D;

	class CTransform*							m_pListener_Transform = { nullptr };
	_uint										m_iNumUsningChannel = { SOUND_2D_NUM_CHANNEL };

public:
	static	CSound_Manager* Create();
	virtual void Free() override;
};

END