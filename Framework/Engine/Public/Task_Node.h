#pragma once

#include "Node.h"

BEGIN(Engine)

class ENGINE_DLL CTask_Node : public CNode
{
protected:
	CTask_Node();
	CTask_Node(const CTask_Node& rhs);
	virtual ~CTask_Node() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() = 0;
	virtual _bool					Execute() override;
	virtual void					Exit() = 0;


public:
	virtual void Free() override;
};

END