#include "BehaviorTree.h"
#include "GameInstance.h"
#include "Composite_Node.h"

CBehaviorTree::CBehaviorTree() : m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
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
	CComposite_Node::COMPOSITE_NODE_DESC			NodeDesc;
	m_pRootNode = CComposite_Node::Create(&NodeDesc);
	m_pRootNode->SetRootNode(true);

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

	if(nullptr != m_pRootNode)
		Safe_Release(m_pRootNode);
}
