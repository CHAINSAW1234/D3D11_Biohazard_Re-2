#include "stdafx.h"
#include "Player_State_Bite_Start.h"

CPlayer_State_Bite_Start::CPlayer_State_Bite_Start(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
}

void CPlayer_State_Bite_Start::OnStateEnter()
{
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(0, 1.f, 10.f);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(1, 0.f, 10.f);

	m_pPlayer->Get_Body_Model()->Set_BlendWeight(3, 0.f);
	m_pPlayer->Get_Body_Model()->Set_BlendWeight(4, 0.f);

	m_pPlayer->Get_Body_Model()->Set_Loop(3, true);
	m_pPlayer->Get_Body_Model()->Set_Loop(4, true);
	// ���� : 3 �Ǵ� 4�� �� ���̾� ����� -> ���� ��ü �Ǵ� ���������� �ൿ�߿� ������ ��� : ���

	// 1. animation �ٲٱ�
}

void CPlayer_State_Bite_Start::OnStateUpdate(_float fTimeDelta)
{
	if (0) {

	}
}

void CPlayer_State_Bite_Start::OnStateExit()
{
}

void CPlayer_State_Bite_Start::Start()
{
}

CPlayer_State_Bite_Start* CPlayer_State_Bite_Start::Create(CPlayer* pPlayer)
{
	CPlayer_State_Bite_Start* pInstance = new CPlayer_State_Bite_Start(pPlayer);

	return pInstance;
}

void CPlayer_State_Bite_Start::Free()
{
	__super::Free();
}
