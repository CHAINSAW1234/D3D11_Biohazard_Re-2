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


	m_iHp = m_pPlayer->Get_Hp();

	m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f, 10.f);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f, 10.f);

	m_pPlayer->Get_Body_Model()->Set_BlendWeight(3, 0.f);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(4, 0.f);

	m_pPlayer->Get_Body_Model()->Set_Loop(0, false);
	//m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f, 10.f);

	m_pPlayer->Get_Body_Model()->Set_Loop(3, true);
	m_pPlayer->Get_Body_Model()->Set_Loop(4, true);

	m_pPlayer->Get_Body_Model()->Set_TotalLinearInterpolation(0.f);
	// 조건 : 3 또느 4번 본 레이어 사용중 -> 무기 교체 또는 재장전등의 행동중에 물리는 경우 : 취소

	m_pPlayer->Get_Body_Model()->Change_Animation(0, m_pPlayer->Get_BiteLayerTag(), m_pPlayer->Get_BiteAnimIndex());

	m_pPlayer->Set_ManualMove(true);

}

void CPlayer_State_Bite::OnStateUpdate(_float fTimeDelta)
{

	m_pPlayer->Update_InterplationMatrix(fTimeDelta);

	if (m_pPlayer->Get_BiteLayerTag() == TEXT("Bite_Default") && (
		m_pPlayer->Get_Body_Model()->Get_CurrentAnimIndex(0) == 0 ||
		m_pPlayer->Get_Body_Model()->Get_CurrentAnimIndex(0) == 2)) {
		;
	}

	if (m_pPlayer->Get_Body_Model()->isFinished(0)) 
	{
		if ((m_pPlayer->Get_BiteAnimIndex() == -1)) 
		{
			m_pPlayer->Change_State(CPlayer::MOVE);
		}
		else {
			m_pPlayer->Get_Body_Model()->Change_Animation(0, m_pPlayer->Get_BiteLayerTag(), m_pPlayer->Get_BiteAnimIndex());
		}
	}


}

void CPlayer_State_Bite::OnStateExit()
{
	m_iHp = -1;
	m_pPlayer->Set_isBite(false);
	m_pPlayer->Set_Spotlight(m_isLight);
	m_pPlayer->Set_Equip(m_eEquip);

	m_eEquip = CPlayer::NONE;
	m_isLight = false;

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
