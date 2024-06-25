#pragma once

#include "Node.h"

BEGIN(Engine)

class ENGINE_DLL CComposite_Node : public CNode
{
public:
	CComposite_Node();
	CComposite_Node(COMPOSITE_NODE_TYPE eType);
	CComposite_Node(const CComposite_Node& rhs);
	virtual ~CComposite_Node() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);
	virtual _bool					Execute() override;
protected:

public:
	static CComposite_Node* Create();

public:
	virtual void Free() override;
};

END