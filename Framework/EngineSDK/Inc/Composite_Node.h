#pragma once

#include "Node.h"

BEGIN(Engine)

class ENGINE_DLL CComposite_Node : public CNode
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
	virtual _bool					Execute() override;
protected:

public:
	static CComposite_Node* Create(void* pArg);

public:
	virtual void Free() override;
};

END