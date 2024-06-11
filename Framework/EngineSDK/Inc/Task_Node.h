#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CTask_Node : public CBase
{
protected:
	CTask_Node();
	CTask_Node(const CTask_Node& rhs);
	virtual ~CTask_Node() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Execute() {}
protected:
	class CGameInstance*			m_pGameInstance = { nullptr };
public:
	static CTask_Node* Create();

public:
	virtual void Free() override;
};

END