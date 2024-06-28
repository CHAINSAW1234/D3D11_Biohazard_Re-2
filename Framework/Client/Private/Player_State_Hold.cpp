#include "stdafx.h"
#include "Player_State_Hold.h"

#include "Player_State_Hold_Start.h"
#include "Player_State_Hold_Idle.h"
#include "Weapon.h"

CPlayer_State_Hold::CPlayer_State_Hold(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
}

void CPlayer_State_Hold::OnStateEnter()
{
	m_pPlayer->Get_Body_Model()->Set_BoneLayer_PlayingInfo(1, TEXT("LowerBody"));

	Change_State(START);
	m_pPlayer->Set_TurnSpineHold(true);
	m_pPlayer->Set_TurnSpineLight(false);
	m_pPlayer->Get_Weapon()->Set_RenderLocation(CWeapon::HOLD);

}

void CPlayer_State_Hold::OnStateUpdate(_float fTimeDelta)
{
	__super::OnStateUpdate(fTimeDelta);

	Update_State();

	if (m_pPlayer->Get_Body_Model()->Is_Loop_PlayingInfo(4)) {
		if (m_pPlayer->Get_Spotlight()) {
			if (m_pPlayer->Get_Equip() == CPlayer::HG) {
				// ¿ÞÆÈ µé¾î³õ±â
				m_pPlayer->Get_Body_Model()->Change_Animation(4, CPlayer::Get_AnimSetEtcName(CPlayer::COMMON), 0);
				m_pPlayer->Get_Body_Model()->Set_BlendWeight(4, 10, 0.2f);

			}
			else if (m_pPlayer->Get_Equip() == CPlayer::STG) {
				// ½ºÆ÷Æ®¶óÀÌÆ® ·»´õ´Â ²ô°í ºÒÀº ÄÑ
				;
			}
		}
		else {
			if (m_pPlayer->Get_Body_Model()->Get_BlendWeight(4) == 10) {
				m_pPlayer->Get_Body_Model()->Set_BlendWeight(4, 0.f, 0.2f);
			}
		}
	}

	
}

void CPlayer_State_Hold::OnStateExit()
{
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(4, 0.f, 0.2f);
	
	m_pPlayer->Get_Weapon()->Set_RenderLocation(CWeapon::MOVE);
	m_pPlayer->Set_TurnSpineHold(false);
	Reset_State();
	m_eState = STATE_END;
}

void CPlayer_State_Hold::Start()
{
}

void CPlayer_State_Hold::Change_State(_uint iState)
{
	__super::Change_State(iState);
	m_eState = (STATE)iState;
}

void CPlayer_State_Hold::Update_State()
{
}

HRESULT CPlayer_State_Hold::Add_States()
{
	Add_State(START, CPlayer_State_Hold_Start::Create(m_pPlayer, this));
	Add_State(IDLE, CPlayer_State_Hold_Idle::Create(m_pPlayer));
	//Change_State(START);

	return S_OK;
}

CPlayer_State_Hold* CPlayer_State_Hold::Create(CPlayer* pPlayer)
{
	CPlayer_State_Hold* pInstance = new CPlayer_State_Hold(pPlayer);

	if (FAILED(pInstance->Add_States()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer_State_Hold"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_State_Hold::Free()
{
	__super::Free();
}
