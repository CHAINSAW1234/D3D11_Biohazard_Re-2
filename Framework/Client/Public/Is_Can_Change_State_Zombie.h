#pragma once

#include "Client_Defines.h"
#include "Decorator_Node.h"
#include "Body_Zombie.h"

BEGIN(Client)

class CIs_Can_Change_State_Zombie : public CDecorator_Node
{
public:
	typedef struct tagCanChangeStateZombieDesc
	{
		list<ZOMBIE_BODY_ANIM_TYPE>				NonBlockTypes;
	}CAN_CHANGE_STATE_ZOMBIE_DESC;
	
private:
	CIs_Can_Change_State_Zombie();
	CIs_Can_Change_State_Zombie(const CIs_Can_Change_State_Zombie& rhs);
	virtual ~CIs_Can_Change_State_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual _bool					Condition_Check();

	ZOMBIE_BODY_ANIM_TYPE			Get_AnimType();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard)
	{
		m_pBlackBoard = pBlackBoard;
	}
private:
	class CBlackBoard_Zombie*					m_pBlackBoard = { nullptr };
	//	상태 변화를 가능하게 할 타입들
	unordered_set<ZOMBIE_BODY_ANIM_TYPE>		m_NonBlockTypes;

public:
	static CIs_Can_Change_State_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END