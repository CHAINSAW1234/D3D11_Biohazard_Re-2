#pragma once

#include "Client_Defines.h"
#include "Decorator_Node.h"

BEGIN(Client)

class CIs_Can_Patrol_Zombie : public CDecorator_Node
{
private:
	CIs_Can_Patrol_Zombie();
	CIs_Can_Patrol_Zombie(const CIs_Can_Patrol_Zombie& rhs);
	virtual ~CIs_Can_Patrol_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual _bool					Condition_Check();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard)
	{
		m_pBlackBoard = pBlackBoard;
	}
protected:
	class CBlackBoard_Zombie* m_pBlackBoard = { nullptr };
public:
	static CIs_Can_Patrol_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END