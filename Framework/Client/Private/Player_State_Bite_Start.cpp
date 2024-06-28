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
	// 조건 : 3 또느 4번 본 레이어 사용중 -> 무기 교체 또는 재장전등의 행동중에 물리는 경우 : 취소

	// 1. animation 바꾸기
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
