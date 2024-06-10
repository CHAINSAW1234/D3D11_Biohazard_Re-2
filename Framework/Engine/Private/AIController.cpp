#include "AIController.h"
#include "GameInstance.h"
#include "BehaviorTree.h"

CAIController::CAIController(): m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

CAIController::CAIController(const CAIController& rhs)
{
}

HRESULT CAIController::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAIController::Initialize(void* pArg)
{
	return S_OK;
}

CBehaviorTree* CAIController::Create_BehaviorTree(_uint* iId)
{
	auto pBehaviorTree = CBehaviorTree::Create();
	Safe_AddRef(pBehaviorTree);
	*iId = m_iBehaviorTree_Count;
	++m_iBehaviorTree_Count;

	m_vecBehaviorTree.push_back(pBehaviorTree);

	return pBehaviorTree;
}

CBehaviorTree* CAIController::Get_BehaviorTree(_uint* iId)
{
	CBehaviorTree* pBehaviorTree = new CBehaviorTree();

	return pBehaviorTree;
}

CAIController* CAIController::Create()
{
	CAIController* pInstance = new CAIController();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CAIController"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAIController::Free()
{
	Safe_Release(m_pGameInstance);

	for (int i = 0; i < m_vecBehaviorTree.size(); ++i)
	{
		Safe_Release(m_vecBehaviorTree[i]);
	}
}
