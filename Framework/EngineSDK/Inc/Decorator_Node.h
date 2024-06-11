#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CDecorator_Node : public CBase
{
protected:
	CDecorator_Node();
	CDecorator_Node(const CDecorator_Node& rhs);
	virtual ~CDecorator_Node() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);

	virtual _bool					Condition_Check() { return false; }
protected:
	class CGameInstance*			m_pGameInstance = { nullptr };
public:
	static CDecorator_Node* Create();

public:
	virtual void Free() override;
};

END