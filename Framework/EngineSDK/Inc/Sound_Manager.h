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

public:
	//������ ����
	HRESULT										Update_Listener(class CTransform* pTransform, _float3 vVelocity);
	HRESULT										Stop_Sound_3D(class CTransform* pTransform, _uint iSoundIndex);
	HRESULT										Stop_All();


public:
	void										Change_Sound_3D(class CTransform* pTransform, const wstring& strSoundTag, _uint iSoundIndex);
	void										Set_Volume_3D(class CTransform* pTransform, _uint iSoundIndex, _float fVolume);
	void										Set_Pause_3D(class CTransform* pTransform, _uint iSoundIndex, _bool isPause);
	void										Set_Distance_3D(class CTransform* pTransform, _uint iSoundIndex, _float fMinDistance, _float fMaxDistance);

	HRESULT										Update_Sounds();

private:
	HRESULT										Update_Listener_Camera();

	HRESULT										Update_Sounds_Position();
	HRESULT										Update_Sounds_Volume();
	HRESULT										Update_Sounds_Mode();
	HRESULT										Update_Sounds_Paused();
	HRESULT										Update_Sounds_Distance();

	HRESULT										Update_Sounds_System();
public:
	HRESULT										Add_Object_Sound(class CTransform* pTransform, _uint iNumChannel);

private:
	void										LoadSoundFile();
	void										LoadSoundFile_Zombie();
	void										LoadSoundFile_Player();

	FMOD_SOUND*									Find_Sound(const wstring& strSoundTag);
	SOUND_DESC*									Find_SoundDesc_3D(class CTransform* pTransform, _uint iSoudIndex);
private:
	// ���� ���ҽ� ������ ���� ��ü 
	map<const wstring, FMOD_SOUND*>				m_Soundmap;
	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD_CHANNEL*								m_pChannelArr[SOUND_CHANNEL_MAX] = {};
	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	FMOD_SYSTEM*								m_pSystem = { nullptr };

	unordered_map<class CTransform*, vector<SOUND_DESC>>				m_Sound_Descs_3D;

	class CTransform*							m_pListener_Transform = { nullptr };
	_uint										m_iNumUsningChannel = { SOUND_2D_NUM_CHANNEL };
	_float3										m_vVelocity = {};

public:
	static	CSound_Manager* Create();
	virtual void Free() override;
};

END