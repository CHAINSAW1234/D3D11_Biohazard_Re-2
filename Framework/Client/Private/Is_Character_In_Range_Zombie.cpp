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

HRESULT CIs_Character_In_Range_Zombie::Initialize(_float fRange)
{
	m_fRange = fRange;

	return S_OK;
}

_bool CIs_Character_In_Range_Zombie::Condition_Check()
{
	if (nullptr == m_pBlackBoard)
		return false;

	auto vPos_Player = m_pBlackBoard->Get_Player()->GetPosition();
	auto vPos_AI = m_pBlackBoard->Get_AI()->GetPosition();

	auto vDelta = vPos_Player - vPos_AI;
	_float fDelta = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vDelta)));

	_bool			isInRange = { false };
	if (fDelta <= m_fRange)
	{
		isInRange = true;
	}
	else
	{
		isInRange = false;
	}

	return isInRange;
}

CIs_Character_In_Range_Zombie* CIs_Character_In_Range_Zombie::Create(_float fRange)
{
	CIs_Character_In_Range_Zombie*		pInstance = { new CIs_Character_In_Range_Zombie() };

	if (FAILED(pInstance->Initialize(fRange)))
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
