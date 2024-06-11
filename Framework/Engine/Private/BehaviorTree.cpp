#include "BehaviorTree.h"
#include "GameInstance.h"
#include "Composite_Node.h"

CBehaviorTree::CBehaviorTree() : m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);

	m_pRootNode = new CComposite_Node();
}

CBehaviorTree::CBehaviorTree(const CBehaviorTree& rhs)
{
}

HRESULT CBehaviorTree::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBehaviorTree::Initialize()
{
	return S_OK;
}

void CBehaviorTree::Initiate()
{
	m_pRootNode->Execute();
}

CBehaviorTree* CBehaviorTree::Create()
{
	CBehaviorTree* pInstance = new CBehaviorTree();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CBehaviorTree"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBehaviorTree::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pRootNode);
}
