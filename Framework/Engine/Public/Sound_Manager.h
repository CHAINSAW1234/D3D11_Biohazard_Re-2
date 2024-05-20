#pragma once

#include "Base.h"

BEGIN(Engine)



class CSound_Manager final : public CBase
{

private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Initialize();
	
public:
	//������ ����
	HRESULT Update_Listener(FMOD_3D_ATTRIBUTES& Attributes_desc);
	void Update_Sound(const wstring& pSoundKey, SOUND_DESC _SoundTag);
	HRESULT  Channel_Pause(_uint iID);
	HRESULT  Change_Channel_Sound(const wstring& SoundKeyTag, _uint iID);
	HRESULT Play_Sound(const wstring& SoundKeyTag, _uint iID);
	HRESULT Play_Sound_Again(const wstring& SoundKeyTag, _uint iID);
	HRESULT PlayBGM(_uint iID, const wstring& SoundKey);
	HRESULT Stop_Sound(_uint iID);
	HRESULT Stop_All();


private:
	void LoadSoundFile();
	SOUND_DESC* Find_Sound(const wstring& SoundKeyTag);
private:
	// ���� ���ҽ� ������ ���� ��ü 
	map<const wstring, SOUND_DESC*> m_Soundmap;
	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD_CHANNEL* m_pChannelArr[SOUND_CHANNEL_MAX];
	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	FMOD_SYSTEM* m_pSystem = { nullptr };

public:
	static	CSound_Manager* Create();
	virtual void Free() override;
};

END