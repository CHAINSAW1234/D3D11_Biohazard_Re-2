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

	// ���� : 3 �Ǵ� 4�� �� ���̾� ����� -> ���� ��ü �Ǵ� ���������� �ൿ�߿� ������ ��� : ���
	m_pPlayer->Stop_UpperBody();

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
				m_pPlayer->Get_Body_Model()->Change_Animation(0, m_pPlayer->Get_BiteLayerTag(), m_pPlayer->Get_BiteAnimIndex());
			}
		}		
	}


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
