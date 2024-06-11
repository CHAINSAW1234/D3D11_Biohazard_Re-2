#include "stdafx.h"
#include "Player_State_Move.h"

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
}

void CPlayer_State_Move::OnStateExit()
{
}

void CPlayer_State_Move::Start()
{
}

void CPlayer_State_Move::Update_KeyInput()
{
	if (m_pGameInstance->Get_KeyState('W')) {

	}



}

CPlayer_State_Move* CPlayer_State_Move::Create(CPlayer* pPlayer)
{
	CPlayer_State_Move* pInstance = new CPlayer_State_Move(pPlayer);

	return pInstance;
}

void CPlayer_State_Move	::Free()
{
	__super::Free();
}
