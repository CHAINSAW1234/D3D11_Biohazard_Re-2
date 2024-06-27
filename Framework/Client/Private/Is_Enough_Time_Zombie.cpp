#include "stdafx.h"
#include "GameInstance.h"
#include "Is_Enough_Time_Zombie.h"
#include "BlackBoard_Zombie.h"
#include "Player.h"
#include "Zombie.h"
#include "Is_Charactor_In_ViewAngle_Zombie.h"

CIs_Enough_Time_Zombie::CIs_Enough_Time_Zombie()
	: CDecorator_Node{}
{
}

CIs_Enough_Time_Zombie::CIs_Enough_Time_Zombie(const CIs_Enough_Time_Zombie& rhs)
	:CDecorator_Node{ rhs }
{
}

HRESULT CIs_Enough_Time_Zombie::Initialize(_float* pAccTime, _float* pNeedTime)
{
	if (nullptr == pAccTime || nullptr == pNeedTime)
		return E_FAIL;

	m_pAccTime = pAccTime;
	m_pNeedTime = pNeedTime;

	return S_OK;
}

_bool CIs_Enough_Time_Zombie::Condition_Check()
{
	if (nullptr == m_pBlackBoard)
		return false;

	_bool			isEnough = { *m_pAccTime >= *m_pNeedTime };

	return isEnough;
}

CIs_Enough_Time_Zombie* CIs_Enough_Time_Zombie::Create(_float* pAccTime, _float* pNeedTime)
{
	CIs_Enough_Time_Zombie* pInstance = { new CIs_Enough_Time_Zombie() };

	if (FAILED(pInstance->Initialize(pAccTime, pNeedTime)))
	{
		MSG_BOX(TEXT("Failed To Created : CIs_Enough_Time_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIs_Enough_Time_Zombie::Free()
{
	__super::Free();
}