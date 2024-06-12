#include "stdafx.h"
#include "Player_State_Move.h"

#include "Player_State_Move_Idle.h"
#include "Player_State_Move_Walk.h"

CPlayer_State_Move::CPlayer_State_Move(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
	// Safe_AddRef ±ÝÁö
}

void CPlayer_State_Move::OnStateEnter()
{
}

void CPlayer_State_Move::OnStateUpdate(_float fTimeDelta)
{
	__super::OnStateUpdate(fTimeDelta);

	Update_State();


}

void CPlayer_State_Move::OnStateExit()
{
	Change_State(IDLE);
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
			m_pGameInstance->Get_KeyState('D') == PRESSING)
			Change_State(WALK);
		break;
	case WALK:
		if (m_pGameInstance->Get_KeyState('W') != PRESSING &&
			m_pGameInstance->Get_KeyState('A') != PRESSING &&
			m_pGameInstance->Get_KeyState('S') != PRESSING &&
			m_pGameInstance->Get_KeyState('D') != PRESSING)
			Change_State(IDLE);

		//if (m_pGameInstance->Get_KeyState(VK_SHIFT) == PRESSING)
		//	Change_State(JOG);

		break;
	case JOG:
		//if (m_pGameInstance->Get_KeyState('W') != PRESSING &&
		//	m_pGameInstance->Get_KeyState('A') != PRESSING &&
		//	m_pGameInstance->Get_KeyState('S') != PRESSING &&
		//	m_pGameInstance->Get_KeyState('D') != PRESSING)
		//	Change_State(IDLE);
		break;
	case STAIR:
		break;
	}
}

HRESULT CPlayer_State_Move::Add_States()
{
	Add_State(IDLE, CPlayer_State_Move_Idle::Create(m_pPlayer));
	Add_State(WALK, CPlayer_State_Move_Walk::Create(m_pPlayer));

	Change_State(IDLE);

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
