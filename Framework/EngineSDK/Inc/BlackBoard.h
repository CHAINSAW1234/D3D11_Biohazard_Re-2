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
	virtual HRESULT					Initialize(void* pArg);
	
public:
	virtual void					Priority_Tick(_float fTimeDelta) = 0;
	virtual void					Late_Tick(_float fTimeDelta) = 0;

public:
	void							Organize_PreState(class CTask_Node* pCurrentNode);

protected:
	class CGameInstance*			m_pGameInstance = { nullptr };
	class CTask_Node*				m_pPreTaskNode = { nullptr };

public:
	virtual void Free() override;
};

END