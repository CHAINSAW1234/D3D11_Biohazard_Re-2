#include "stdafx.h"
#include "Player_State_Bite.h"

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
	m_pPlayer->Set_Equip(CPlayer::NONE);
}

void CPlayer_State_Bite::OnStateUpdate(_float fTimeDelta)
{
	__super::OnStateUpdate(fTimeDelta);

	m_pPlayer->Update_InterplationMatrix(fTimeDelta);


}

void CPlayer_State_Bite::OnStateExit()
{
	m_pPlayer->Set_isBite(false);
	m_pPlayer->Set_Spotlight(m_isLight);
	m_pPlayer->Set_Equip(m_eEquip);

	m_eEquip = CPlayer::NONE;
	m_isLight = false;
}

void CPlayer_State_Bite::Start()
{
}

void CPlayer_State_Bite::Change_State(_uint iState)
{
	__super::Change_State(iState);
	m_eState = (STATE)iState;
}

void CPlayer_State_Bite::Update_State()
{
}

HRESULT CPlayer_State_Bite::Add_States()
{
	return S_OK;
}

CPlayer_State_Bite* CPlayer_State_Bite::Create(CPlayer* pPlayer)
{
	CPlayer_State_Bite* pInstance = new CPlayer_State_Bite(pPlayer);

	if (FAILED(pInstance->Add_States()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer_State_Bite"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_State_Bite::Free()
{
	__super::Free();
}
