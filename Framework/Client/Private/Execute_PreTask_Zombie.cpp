#include "stdafx.h"
#include "GameInstance.h"
#include "Execute_PreTask_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"
#include "Stun_Zombie.h"

CExecute_PreTask_Zombie::CExecute_PreTask_Zombie()
	: CTask_Node()
{
}

CExecute_PreTask_Zombie::CExecute_PreTask_Zombie(const CExecute_PreTask_Zombie& rhs)
{
}

HRESULT CExecute_PreTask_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CExecute_PreTask_Zombie::Enter()
{
}

_bool CExecute_PreTask_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	//	이전 노드를 실행해야함 
	//	m_pBlackBoard->Organize_PreState(this);

	if (COLLIDER_TYPE::_END != m_pBlackBoard->Get_AI()->Get_Current_IntersectCollider())
	{
		CStun_Zombie*			pTaskStun = { dynamic_cast<CStun_Zombie*>(m_pPreTaskNode) };
		if (nullptr != pTaskStun)
		{
			pTaskStun->Reset_Entry();
		}
	}

	m_pPreTaskNode = { m_pBlackBoard->Get_PreTaskNode() };
	if (nullptr != m_pPreTaskNode)
	{
		return m_pPreTaskNode->Execute(fTimeDelta);
	}

#ifdef _DEBUG

	else
	{
		MSG_BOX(TEXT("Called : _bool CExecute_PreTask_Zombie::Execute(_float fTimeDelta)"));
	}

#endif

	return false;
}

void CExecute_PreTask_Zombie::Exit()
{
}

CExecute_PreTask_Zombie* CExecute_PreTask_Zombie::Create(void* pArg)
{
	CExecute_PreTask_Zombie* pInstance = { new CExecute_PreTask_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CExecute_PreTask_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CExecute_PreTask_Zombie::Free()
{
	__super::Free();
}
