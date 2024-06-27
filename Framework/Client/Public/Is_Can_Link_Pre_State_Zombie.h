#pragma once

#include "Client_Defines.h"
#include "Decorator_Node.h"

BEGIN(Client)

class CIs_Can_Link_Pre_State_Zombie : public CDecorator_Node
{
public:
	CIs_Can_Link_Pre_State_Zombie();
	CIs_Can_Link_Pre_State_Zombie(const CIs_Can_Link_Pre_State_Zombie& rhs);
	virtual ~CIs_Can_Link_Pre_State_Zombie() = default;

public:
	virtual HRESULT					Initialize(list<MONSTER_STATE> CanLinkStates);

	virtual _bool					Condition_Check();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }
private:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };

	unordered_set<MONSTER_STATE>	m_CanLinkStates;

public:
	static CIs_Can_Link_Pre_State_Zombie* Create(list<MONSTER_STATE> CanLinkStates);

public:
	virtual void Free() override;
};

END