#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CPathFinder : public CBase
{
protected:
	CPathFinder();
	CPathFinder(const CPathFinder& rhs);
	virtual ~CPathFinder() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);

protected:
	class CGameInstance*			m_pGameInstance = { nullptr };
public:
	static CPathFinder* Create();

public:
	virtual void Free() override;
};

END