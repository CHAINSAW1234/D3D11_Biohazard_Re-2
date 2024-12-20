#include "stdafx.h"
#include "Player_State_Bite.h"

#include "Body_Zombie.h"

CPlayer_State_Bite::CPlayer_State_Bite(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
}

void CPlayer_State_Bite::OnStateEnter()
{
	m_pPlayer->Set_isBite(true);

	m_eEquip = m_pPlayer->Get_Equip();
	m_isLight = m_pPlayer->Get_Spotlight();

	m_pPlayer->Set_Spotlight(false);
	CPlayer::EQUIP eEquip = CPlayer::NONE;
	m_pPlayer->Set_Equip(&eEquip);

	m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f, 10.f);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f, 0.f);

	m_pPlayer->Get_Body_Model()->Set_Loop(0, false);
	//m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f, 10.f);

	// 조건 : 3 또느 4번 본 레이어 사용중 -> 무기 교체 또는 재장전등의 행동중에 물리는 경우 : 취소
	m_pPlayer->Stop_UpperBody();

	m_pPlayer->Get_Body_Model()->Change_Animation(0, m_pPlayer->Get_BiteLayerTag(), m_pPlayer->Get_BiteAnimIndex());

	m_pPlayer->Set_ManualMove(true);

	m_pPlayer->Change_Sound_3D(TEXT("Sound_Player_Hit_Start"), 11, 4);
}

void CPlayer_State_Bite::OnStateUpdate(_float fTimeDelta)
{
	m_pPlayer->Update_InterplationMatrix(fTimeDelta);

	if (m_pPlayer->Get_Body_Model()->isFinished(0)) 
	{
		CModel* pBody_Model = { m_pPlayer->Get_Body_Model() };
		if (nullptr == pBody_Model)
			return;

		if (TEXT("Bite_Lightly_Hold") == pBody_Model->Get_CurrentAnimLayerTag(0))
		{
			_int			iAnimIndex = { pBody_Model->Get_CurrentAnimIndex(0) };
			if (ANIM_BITE_LIGHTLY_HOLD::_LIGHTLY_HOLD_END_L == static_cast<ANIM_BITE_LIGHTLY_HOLD>(iAnimIndex) ||
				ANIM_BITE_LIGHTLY_HOLD::_LIGHTLY_HOLD_END_R == static_cast<ANIM_BITE_LIGHTLY_HOLD>(iAnimIndex))
			{
				if (true == pBody_Model->isFinished(0))
				{
					m_pPlayer->Change_State(CPlayer::MOVE);
				}
			}
			else
			{
				m_pPlayer->Get_Body_Model()->Change_Animation(0, m_pPlayer->Get_BiteLayerTag(), m_pPlayer->Get_BiteAnimIndex());

				if (m_pPlayer->Get_Hp() <= 0) {
					m_pPlayer->Change_Sound_3D(TEXT("Sound_Player_Dead"), 6, 4);
				}
				else {
					m_pPlayer->Change_Sound_3D(TEXT("Sound_Player_Hit"), 8, 4);
				}
				////////////////////TEST///////////////
				pBody_Model->Set_TickPerSec(m_pPlayer->Get_BiteLayerTag(), m_pPlayer->Get_BiteAnimIndex(), 10.f);
			}
		}
		else
		{
			if ((m_pPlayer->Get_BiteAnimIndex() == -1))
			{
				m_pPlayer->Change_State(CPlayer::MOVE);
			}
			else {
				if (m_pPlayer->Get_Hp() <= 0) {
					m_pPlayer->Change_Sound_3D(TEXT("Sound_Player_Dead"), 6, 4);
				}
				else {
					m_pPlayer->Change_Sound_3D(TEXT("Sound_Player_Hit"), 8, 4);
				}

				m_pPlayer->Get_Body_Model()->Change_Animation(0, m_pPlayer->Get_BiteLayerTag(), m_pPlayer->Get_BiteAnimIndex());
			}
		}		
	}

	//// 애니메이션이 좀 많이 길면 사운드 하나 더
	//static _bool isPlay = false;

	//_float fTrackPosition = m_pPlayer->Get_Body_Model()->Get_TrackPosition(0);
	//
	//_int iTrackPosition = static_cast<_int>(floor(fTrackPosition));

	//if (!isPlay && iTrackPosition >= 200 && m_pPlayer->Get_Body_Model()->Get_Duration_From_PlayingInfo(0) >= 300) {
	//	isPlay = true;

	//	if (m_pPlayer->Get_Hp() <= 0) {
	//		m_pPlayer->Change_Sound_3D(TEXT("Sound_Player_Hit"), 6, 4);
	//	}
	//	else {
	//		m_pPlayer->Change_Sound_3D(TEXT("Sound_Player_Hit_Start"), 8, 4);
	//	}

	//}

	//if (isPlay && iTrackPosition <= 10) {
	//	isPlay = false;
	//}

}

void CPlayer_State_Bite::OnStateExit()
{
	m_pPlayer->Set_isBite(false);
	m_pPlayer->Set_Spotlight(m_isLight);
	m_pPlayer->Set_Equip(&m_eEquip);

	m_eEquip = CPlayer::NONE;
	m_isLight = false;

	m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.2f);

	m_pPlayer->Set_ManualMove(false);
}

void CPlayer_State_Bite::Start()
{
}

CPlayer_State_Bite* CPlayer_State_Bite::Create(CPlayer* pPlayer)
{
	CPlayer_State_Bite* pInstance = new CPlayer_State_Bite(pPlayer);

	return pInstance;
}

void CPlayer_State_Bite::Free()
{
	__super::Free();
}
