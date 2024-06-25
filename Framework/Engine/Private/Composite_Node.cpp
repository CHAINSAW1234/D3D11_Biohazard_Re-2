#include "GameInstance.h"
#include "Composite_Node.h"
#include "Decorator_Node.h"
#include "Task_Node.h"

CComposite_Node::CComposite_Node() : CNode()
{

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
		if (Check_Permition_To_Execute() == false)
			return false;

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

		return true;
	}
	else
	{
		for (int i = 0; i < m_vecNode.size(); ++i)
		{
			m_vecNode[i] && m_vecNode[i]->Execute();
		}

		return true;
	}
}

void CComposite_Node::Free()
{
	__super::Free();
}
