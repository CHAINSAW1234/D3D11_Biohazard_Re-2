#include "stdafx.h"	
#include "BGM_Player.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CBGM_Player)

CBGM_Player::CBGM_Player()
{
}

void CBGM_Player::Tick(_float fTimeDelta)
{
	if (true == m_isFocus)
	{
		m_fAccTime += fTimeDelta;
		if (m_fAccTime > m_fMaxTime)
			m_fAccTime = m_fMaxTime;
	}

	else
	{
		m_fAccTime -= fTimeDelta;
		if (0.f > m_fAccTime)
			m_fAccTime = 0.f;
	}

	if (0.f < m_fAccTime)
	{
		CGameInstance* pGameInstance = { CGameInstance::Get_Instance() };
		if (nullptr != pGameInstance)
		{
			if(m_iLevel == 0)
				m_iChannel = pGameInstance->PlaySoundEffect_2D_Using_Index(TEXT("BGM"), TEXT("cf092_music_en.bnk.2_1.mp3"), 0.001f);

			_float			fVolume = { m_fAccTime / m_fMaxTime * 0.5f };
			_int			iLevel = { static_cast<_int>(fVolume * 1000.f) };
			if (iLevel != m_iLevel && -1 != m_iChannel)
			{				
				pGameInstance->SetVolume_2D(m_iChannel, fVolume);
			}

			m_iLevel = iLevel;
		}
	}

	else
	{
		m_iLevel = 0; 
		CGameInstance* pGameInstance = { CGameInstance::Get_Instance() };
		if (nullptr != pGameInstance)
			pGameInstance->StopSound_2D(m_iChannel);
		m_iChannel = -1;
	}

	m_isFocus = false;
}

void CBGM_Player::Set_Focused(_bool isFocus)
{
	m_isFocus = isFocus;
}

void CBGM_Player::Free()
{
	__super::Free();
}
