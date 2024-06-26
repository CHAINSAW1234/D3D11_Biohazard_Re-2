#pragma once

#include "Client_Defines.h"
#include "Decorator_Node.h"

BEGIN(Client)

class CIs_Maintain_PreTask_Zombie : public CDecorator_Node
{
private:
	CIs_Maintain_PreTask_Zombie();
	CIs_Maintain_PreTask_Zombie(const CIs_Maintain_PreTask_Zombie& rhs);
	virtual ~CIs_Maintain_PreTask_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual _bool					Condition_Check();

private:
	_bool							Is_Maintain_State(MONSTER_STATE eState);

	_bool							Is_CanFinish(MONSTER_STATE eState);

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard)
	{
		m_pBlackBoard = pBlackBoard;
	}
private:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };
	unordered_set<MONSTER_STATE>	m_MaintainStates;


public:
	static CIs_Maintain_PreTask_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END