#include "stdafx.h"
#include "GameInstance.h"
#include "Set_Focus_Player_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"
#include "Call_Center.h"

CSet_Focus_Player::CSet_Focus_Player()
	: CTask_Node()
{
}

CSet_Focus_Player::CSet_Focus_Player(const CSet_Focus_Player& rhs)
{
}

HRESULT CSet_Focus_Player::Initialize(void* pArg)
{
	return S_OK;
}

void CSet_Focus_Player::Enter()
{

}

_bool CSet_Focus_Player::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion	

	if(true == m_pBlackBoard->Is_LookTarget())
		CCall_Center::Get_Instance()->Set_Focus_Player();

	return true;
}

void CSet_Focus_Player::Exit()
{
}

CSet_Focus_Player* CSet_Focus_Player::Create(void* pArg)
{
	CSet_Focus_Player* pInstance = { new CSet_Focus_Player() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CSet_Focus_Player"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSet_Focus_Player::Free()
{
	__super::Free();
}
