#include "GameInstance.h"
#include "Composite_Node.h"
#include "Decorator_Node.h"
#include "Task_Node.h"

CComposite_Node::CComposite_Node() : m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

CComposite_Node::CComposite_Node(COMPOSITE_NODE_TYPE eType) : CComposite_Node()
{
	m_eComposite_Type = eType;
}

CComposite_Node::CComposite_Node(const CComposite_Node& rhs)
{
}

HRESULT CComposite_Node::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CComposite_Node::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

CComposite_Node* CComposite_Node::Create()
{
	CComposite_Node* pInstance = new CComposite_Node();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CComposite_Node"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

_bool CComposite_Node::Execute()
{
	if(m_bRootNode == false)
	{
		for (int i = 0; i < m_vecDecorator_Node.size(); ++i)
		{
			if (m_vecDecorator_Node[i] && m_vecDecorator_Node[i]->Condition_Check() == false)
			{
				return false;
			}
		}

		for (int i = 0; i < m_vecNode.size(); ++i)
		{
			if (m_eComposite_Type == CNT_SELECTOR)
			{
				if (m_vecNode[i] && m_vecNode[i]->Execute() == true)
					return true;
			}
			else
			{
				if (m_vecNode[i] && m_vecNode[i]->Execute() == false)
					return false;
			}
		}

		for (int i = 0; i < m_vecTask_Node.size(); ++i)
		{
			if (m_vecTask_Node[i])
				m_vecTask_Node[i]->Execute();
		}

		return true;
	}
	else
	{
		for (int i = 0; i < m_vecNode.size(); ++i)
		{
			m_vecNode[i] && m_vecNode[i]->Execute();
		}

		for (int i = 0; i < m_vecTask_Node.size(); ++i)
		{
			if (m_vecTask_Node[i])
				m_vecTask_Node[i]->Execute();
		}

		return true;
	}
}

void CComposite_Node::Free()
{
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

	for (int i = 0; i < m_vecTask_Node.size(); ++i)
	{
		if (m_vecTask_Node[i])
			Safe_Release(m_vecTask_Node[i]);
	}
}
