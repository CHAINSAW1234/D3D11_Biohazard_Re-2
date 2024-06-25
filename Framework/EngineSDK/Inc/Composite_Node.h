#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CComposite_Node : public CBase
{
public:
	typedef struct tagCompositeNodeDesc
	{
		COMPOSITE_NODE_TYPE			eType = { CNT_END };
	}COMPOSITE_NODE_DESC;
protected:
	CComposite_Node();
	CComposite_Node(const CComposite_Node& rhs);
	virtual ~CComposite_Node() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	void							Insert_Child_Node(class CComposite_Node* pNode)
	{
		m_vecNode.push_back(pNode);
	}

	void							Insert_Task_Node(class CTask_Node* pTask_Node)
	{
		m_vecTask_Node.push_back(pTask_Node);
	}
	void							Insert_Decorator_Node(class CDecorator_Node* pDecorator_Node)
	{
		m_vecDecorator_Node.push_back(pDecorator_Node);
	}
	_bool							Execute();
	void							SetRootNode(_bool boolean)
	{
		m_bRootNode = boolean;
	}
protected:
	COMPOSITE_NODE_TYPE						m_eComposite_Type = { CNT_DEFAULT };
	vector<class CComposite_Node*>			m_vecNode;
	vector<class CDecorator_Node*>			m_vecDecorator_Node;
	vector<class CTask_Node*>				m_vecTask_Node = { nullptr };
	_bool									m_bRootNode = { false };

public:
	static CComposite_Node* Create(void* pArg);

public:
	virtual void Free() override;
};

END