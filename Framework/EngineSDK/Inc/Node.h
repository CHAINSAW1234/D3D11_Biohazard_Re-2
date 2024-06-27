#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CNode : public CBase
{
public:
	CNode();
	CNode(COMPOSITE_NODE_TYPE eType);
	CNode(const CNode& rhs);
	virtual ~CNode() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);

	void							Insert_Child_Node(class CNode* pNode)
	{

		m_vecNode.push_back(pNode);
	}
	void							Insert_Decorator_Node(class CDecorator_Node* pDecorator_Node)
	{
		m_vecDecorator_Node.push_back(pDecorator_Node);
	}
	virtual _bool					Execute(_float fTimeDelta = 0.f) { return false; }
	void							SetRootNode(_bool boolean)
	{
		m_bRootNode = boolean;
	}
	virtual _bool					Check_Permition_To_Execute();
protected:
	class CGameInstance*					m_pGameInstance = { nullptr };

	COMPOSITE_NODE_TYPE						m_eComposite_Type = { CNT_DEFAULT };
	vector<class CNode*>					m_vecNode;
	vector<class CDecorator_Node*>			m_vecDecorator_Node;
	_bool									m_bRootNode = { false };
public:
	static CNode* Create();

public:
	virtual void Free() override;
};

END