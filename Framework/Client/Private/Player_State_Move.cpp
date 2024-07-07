#include "stdafx.h"
#include "Player_State_Move.h"

#include "Player_State_Move_Idle.h"
#include "Player_State_Move_Walk.h"
#include "Player_State_Move_Jog.h"
#include "Player_State_Move_DoorStop.h"
#include "Player_State_Move_Ladder.h"
#include "Player_State_Move_Lever.h"
#include "Weapon.h"

CPlayer_State_Move::CPlayer_State_Move(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
	// Safe_AddRef ±ÝÁö
}

void CPlayer_State_Move::OnStateEnter()
{
	m_pPlayer->Get_Body_Model()->Set_BoneLayer_PlayingInfo(1, TEXT("Default"));
	m_pPlayer->Get_Body_Model()->Reset_PreAnimation(1);
	Change_State(IDLE);
}

void CPlayer_State_Move::OnStateUpdate(_float fTimeDelta)
{
	__super::OnStateUpdate(fTimeDelta);

	Update_State();
	Open_Door();

	if (m_pPlayer->Get_Spotlight()) {
		m_pPlayer->Set_TurnSpineLight(true);
	}
	else {
		m_pPlayer->Set_TurnSpineLight(false);
	}

	if (m_pPlayer->Get_Weapon() != nullptr) {
		if (m_pPlayer->Get_Spotlight() &&
			m_pPlayer->Get_Body_Model()->Is_Loop_PlayingInfo(3)) {
			m_pPlayer->Get_Weapon()->Set_RenderLocation(CWeapon::MOVE_LIGHT);
		}
		else {
			if (m_pPlayer->Get_Hp() >= 4 ||
				!m_pPlayer->Get_Body_Model()->Is_Loop_PlayingInfo(3)	) {
				m_pPlayer->Get_Weapon()->Set_RenderLocation(CWeapon::MOVE);
			}
			else {
				m_pPlayer->Get_Weapon()->Set_RenderLocation(CWeapon::MOVE_LIGHT);
			}

		}
	}
}

void CPlayer_State_Move::OnStateExit()
{
	Reset_State();
	m_eState = STATE_END;
	
	//Change_State(IDLE);
}

void CPlayer_State_Move::Start()
{
}

void CPlayer_State_Move::Change_State(_uint iState)
{
	__super::Change_State(iState);
	m_eState = (STATE)iState;
}

void CPlayer_State_Move::Update_State()
{
	switch (m_eState) {
	case IDLE:
		if (m_pGameInstance->Get_KeyState('W') == PRESSING ||
			m_pGameInstance->Get_KeyState('A') == PRESSING ||
			m_pGameInstance->Get_KeyState('S') == PRESSING ||
			m_pGameInstance->Get_KeyState('D') == PRESSING) {

			if (m_pGameInstance->Get_KeyState(VK_SHIFT) == PRESSING) {
				Change_State(JOG);
			}
			else {
				Change_State(WALK);
			}
		}
		break;
	case WALK:
		if (m_pGameInstance->Get_KeyState('W') == NONE &&
			m_pGameInstance->Get_KeyState('A') == NONE &&
			m_pGameInstance->Get_KeyState('S') == NONE &&
			m_pGameInstance->Get_KeyState('D') == NONE)
			Change_State(IDLE);
		if (m_pGameInstance->Get_KeyState(VK_SHIFT) == PRESSING)
			Change_State(JOG);
		break;
	case JOG:
		if (m_pGameInstance->Get_KeyState('W') == NONE &&
			m_pGameInstance->Get_KeyState('A') == NONE &&
			m_pGameInstance->Get_KeyState('S') == NONE &&
			m_pGameInstance->Get_KeyState('D') == NONE)
			Change_State(IDLE);
		break;
	case STAIR:
	case LADDER:
	case LEVER:
		break;
	}

	if (m_pPlayer->Get_Door_Setting() == CPlayer::DOOR_BEHAVE_LOCK) {
		Change_State(DOOR_STOP);
		m_pPlayer->Set_Door_Setting(CPlayer::DOOR_BEHAVE_NOTHING);
		return;
	}

	if (m_pPlayer->Get_Ladder_Setting() != CPlayer::LADDER_BEHAVE_NOTHING) {
		Change_State(LADDER);
		return;
	}

	if (m_pPlayer->Get_Lever_Setting() != CPlayer::LEVER_BEHAVE_NOTHING) {
		if (m_pGameInstance->Get_KeyState(VK_LBUTTON) == PRESSING) {
			Change_State(LEVER);
			return;
		}

	}

}

void CPlayer_State_Move::Open_Door()
{
	if ((m_pPlayer->Get_Body_Model()->isFinished(3) &&
		m_pPlayer->Get_Body_Model()->Get_CurrentAnimLayerTag(3) == CPlayer::Get_AnimSetEtcName(CPlayer::COMMON) &&
		m_pPlayer->Get_Body_Model()->Get_CurrentAnimIndex(3) == CPlayer::DOOR_OPEN_JOG)) {
		m_pPlayer->Get_Body_Model()->Set_Loop(3, true);
		m_pPlayer->Get_Body_Model()->Set_BlendWeight(3, 0.f, 10.f);
	}

	if ((m_pPlayer->Get_Body_Model()->isFinished(4) &&
		m_pPlayer->Get_Body_Model()->Get_CurrentAnimLayerTag(4) == CPlayer::Get_AnimSetEtcName(CPlayer::COMMON) &&
		m_pPlayer->Get_Body_Model()->Get_CurrentAnimIndex(4) == CPlayer::DOOR_PASS)) {
		m_pPlayer->Get_Body_Model()->Set_Loop(4, true);
		m_pPlayer->Get_Body_Model()->Set_BlendWeight(4, 0.f, 10.f);
	}

	if (m_pPlayer->Get_Door_Setting() == CPlayer::DOOR_BEHAVE_OPEN) {
		if (m_pPlayer->Get_Body_Model()->Is_Loop_PlayingInfo(3) &&
			m_pPlayer->Get_Body_Model()->Is_Loop_PlayingInfo(4)) {

			if (m_eState == WALK) {
				m_pPlayer->Get_Body_Model()->Change_Animation(4, CPlayer::Get_AnimSetEtcName(CPlayer::COMMON), CPlayer::DOOR_PASS);
				m_pPlayer->Get_Body_Model()->Set_Loop(4, false);
				m_pPlayer->Get_Body_Model()->Set_BlendWeight(4, 10.f, 6.f);
			}
			if (m_eState == JOG) {
				m_pPlayer->Get_Body_Model()->Change_Animation(3, CPlayer::Get_AnimSetEtcName(CPlayer::COMMON), CPlayer::DOOR_OPEN_JOG);
				m_pPlayer->Get_Body_Model()->Set_Loop(3, false);
				m_pPlayer->Get_Body_Model()->Set_BlendWeight(3, 10.f, 6.f);
			}
		}
		m_pPlayer->Set_Door_Setting(CPlayer::DOOR_BEHAVE_NOTHING);
	}
}

HRESULT CPlayer_State_Move::Add_States()
{
	Add_State(IDLE, CPlayer_State_Move_Idle::Create(m_pPlayer));
	Add_State(WALK, CPlayer_State_Move_Walk::Create(m_pPlayer, this));
	Add_State(JOG, CPlayer_State_Move_Jog::Create(m_pPlayer, this));
	Add_State(DOOR_STOP, CPlayer_State_Move_DoorStop::Create(m_pPlayer, this));
	Add_State(LADDER, CPlayer_State_Move_Ladder::Create(m_pPlayer, this));
	Add_State(LEVER, CPlayer_State_Move_Lever::Create(m_pPlayer, this));
	//Change_State(IDLE);

	return S_OK;
}

CPlayer_State_Move* CPlayer_State_Move::Create(CPlayer* pPlayer)
{
	CPlayer_State_Move* pInstance = new CPlayer_State_Move(pPlayer);

	if (FAILED(pInstance->Add_States()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer_State_Move"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_State_Move	::Free()
{
	__super::Free();
}
