#include "stdafx.h"
#include "Player_State_SubHold.h"
#include "Player_State_SubHold_Start.h"
#include "Player_State_SubHold_Idle.h"
#include "Player.h"

CPlayer_State_SubHold::CPlayer_State_SubHold(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
}

void CPlayer_State_SubHold::OnStateEnter()
{
	m_pPlayer->Get_Body_Model()->Set_BoneLayer_PlayingInfo(3, TEXT("Right_Arm"));
	m_pPlayer->Get_Body_Model()->Reset_PreAnim_CurrentAnim(3);

	Change_State(START);
	m_pPlayer->Change_Equip_State(CPlayer::SUB);
}

void CPlayer_State_SubHold::OnStateUpdate(_float fTimeDelta)
{
	__super::OnStateUpdate(fTimeDelta);
}

void CPlayer_State_SubHold::OnStateExit()
{
	Reset_State();
	m_eState = STATE_END;


	m_pPlayer->Get_Body_Model()->Set_BoneLayer_PlayingInfo(3, TEXT("UpperBody"));
	m_pPlayer->Get_Body_Model()->Reset_PreAnim_CurrentAnim(3);

	m_pPlayer->Change_Equip_State(CPlayer::GUN);
}

void CPlayer_State_SubHold::Start()
{
}

void CPlayer_State_SubHold::Change_State(_uint iState)
{
	__super::Change_State(iState);
	m_eState = (STATE)iState;
}

void CPlayer_State_SubHold::Update_State()
{
}

HRESULT CPlayer_State_SubHold::Add_States()
{
	__super::Add_State(START, CPlayer_State_SubHold_Start::Create(m_pPlayer, this));
	__super::Add_State(IDLE, CPlayer_State_SubHold_Idle::Create(m_pPlayer));
	return S_OK;
}

CPlayer_State_SubHold* CPlayer_State_SubHold::Create(CPlayer* pPlayer)
{
	CPlayer_State_SubHold* pInstance = new CPlayer_State_SubHold(pPlayer);

	if (FAILED(pInstance->Add_States()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer_State_SubHold"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_State_SubHold::Free()
{
	__super::Free();
}
