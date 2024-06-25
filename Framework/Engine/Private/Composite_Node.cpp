#include "GameInstance.h"
#include "Composite_Node.h"
#include "Decorator_Node.h"
#include "Task_Node.h"

CComposite_Node::CComposite_Node()
{
}

CComposite_Node::CComposite_Node(const CComposite_Node& rhs)
{
}

HRESULT CComposite_Node::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	COMPOSITE_NODE_DESC*			pDesc = { static_cast<COMPOSITE_NODE_DESC*>(pArg) };
	m_eComposite_Type = pDesc->eType;

	return S_OK;
}

CComposite_Node* CComposite_Node::Create(void* pArg)
{
	CComposite_Node* pInstance = new CComposite_Node();

	if (FAILED(pInstance->Initialize(pArg)))
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
