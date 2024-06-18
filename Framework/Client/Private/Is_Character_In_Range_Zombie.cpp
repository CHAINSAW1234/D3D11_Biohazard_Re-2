#include "stdafx.h"
#include "GameInstance.h"
#include "Is_Character_In_Range_Zombie.h"
#include "BlackBoard_Zombie.h"
#include "Player.h"
#include "Zombie.h"

CIs_Character_In_Range_Zombie::CIs_Character_In_Range_Zombie()
	: CDecorator_Node()
{

}

CIs_Character_In_Range_Zombie::CIs_Character_In_Range_Zombie(const CIs_Character_In_Range_Zombie& rhs)
{
}

HRESULT CIs_Character_In_Range_Zombie::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CIs_Character_In_Range_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

_bool CIs_Character_In_Range_Zombie::Condition_Check()
{
	auto vPos_Player = m_pBlackBoard->GetPlayer()->GetPosition();
	auto vPos_AI = m_pBlackBoard->GetAI()->GetPosition();

	auto vDelta = vPos_Player - vPos_AI;
	_float fDelta = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vDelta)));

	if (fDelta < 3.f)
	{
		return true;
	}
	else
	{
		return false;
	}
}

CIs_Character_In_Range_Zombie* CIs_Character_In_Range_Zombie::Create()
{
	CIs_Character_In_Range_Zombie* pInstance = new CIs_Character_In_Range_Zombie();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CIs_Character_In_Range_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIs_Character_In_Range_Zombie::Free()
{
	__super::Free();
}
