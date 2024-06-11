#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CBlackBoard : public CBase
{
protected:
	CBlackBoard();
	CBlackBoard(const CBlackBoard& rhs);
	virtual ~CBlackBoard() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);

protected:
	class CGameInstance*			m_pGameInstance = { nullptr };
public:
	static CBlackBoard* Create();

public:
	virtual void Free() override;
};

END