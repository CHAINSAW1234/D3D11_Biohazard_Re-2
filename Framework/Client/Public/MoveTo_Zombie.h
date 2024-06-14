#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"


BEGIN(Client)

class CMoveTo_Zombie : public CTask_Node
{
public:
	CMoveTo_Zombie();
	CMoveTo_Zombie(const CMoveTo_Zombie& rhs);
	virtual ~CMoveTo_Zombie() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Execute() override;

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard)
	{
		m_pBlackBoard = pBlackBoard;
	}
protected:
	class CGameInstance*			m_pGameInstance = { nullptr };
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };
public:
	static CMoveTo_Zombie* Create();

public:
	virtual void Free() override;
};

END