#include "stdafx.h"
#include "Player_State_Hold.h"


CPlayer_State_Hold::CPlayer_State_Hold(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
}

void CPlayer_State_Hold::OnStateEnter()
{
}

void CPlayer_State_Hold::OnStateUpdate(_float fTimeDelta)
{
	__super::OnStateUpdate(fTimeDelta);

	Update_State();
}

void CPlayer_State_Hold::OnStateExit()
{
	Change_State(IDLE);
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
