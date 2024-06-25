#include "GameInstance.h"
#include "Node.h"
#include "Decorator_Node.h"
#include "Task_Node.h"

CNode::CNode() : m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

CNode::CNode(COMPOSITE_NODE_TYPE eType) : CNode()
{
	m_eComposite_Type = eType;
}

CNode::CNode(const CNode& rhs)
{
}

HRESULT CNode::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CNode::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

_bool CNode::Check_Permition_To_Execute()
{
	for (int i = 0; i < m_vecDecorator_Node.size(); ++i)
	{
		if (m_vecDecorator_Node[i] && m_vecDecorator_Node[i]->Condition_Check() == false)
		{
			return false;
		}
	}

	return true;
}

CNode* CNode::Create()
{
	CNode* pInstance = new CNode();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CNode"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNode::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);

	for (int i = 0; i < m_vecNode.size(); ++i)
	{
		if(m_vecNode[i])
			Safe_Release(m_vecNode[i]);
	}

	for (int i = 0; i < m_vecDecorator_Node.size(); ++i)
	{
		if(m_vecDecorator_Node[i])
			Safe_Release(m_vecDecorator_Node[i]);
	}
}
