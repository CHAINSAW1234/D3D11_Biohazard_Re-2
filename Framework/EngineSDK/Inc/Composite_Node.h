#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CComposite_Node : public CBase
{
public:
	CComposite_Node();
	CComposite_Node(const CComposite_Node& rhs);
	virtual ~CComposite_Node() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);

	void							Insert_Child_Node(class CComposite_Node* pNode)
	{
		if (m_bLeaf_Node)
			return;

		m_vecNode.push_back(pNode);
	}

	void							Insert_Task_Node(class CTask_Node* pTask_Node)
	{
		m_vecTask_Node.push_back(pTask_Node);

		m_bLeaf_Node = true;
	}
	void							Insert_Decorator_Node(class CDecorator_Node* pDecorator_Node)
	{
		m_vecDecorator_Node.push_back(pDecorator_Node);
	}

	_bool							Execute();
protected:
	class CGameInstance*					m_pGameInstance = { nullptr };

	COMPOSITE_NODE_TYPE						m_eComposite_Type = { CNT_DEFAULT };
	vector<class CComposite_Node*>			m_vecNode;
	vector<class CDecorator_Node*>			m_vecDecorator_Node;
	vector<class CTask_Node*>				m_vecTask_Node = { nullptr };

	_bool									m_bLeaf_Node = { false };
public:
	static CComposite_Node* Create();

public:
	virtual void Free() override;
};

END