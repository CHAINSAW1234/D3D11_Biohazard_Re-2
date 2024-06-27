#include "GameInstance.h"
#include "Composite_Node.h"
#include "Decorator_Node.h"
#include "Task_Node.h"

CComposite_Node::CComposite_Node() : CNode()
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
