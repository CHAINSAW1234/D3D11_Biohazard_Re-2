#pragma once

#include "Client_Defines.h"
#include "Decorator_Node.h"
#include "Body_Zombie.h"

BEGIN(Client)

class CIs_Hit_Zombie : public CDecorator_Node
{
public:
	typedef struct tagIsHitZombieDesc
	{
		list<COLLIDER_TYPE>					CheckColliderTypes;
		list<HIT_TYPE>						CheckHitTypes;
	}IS_HIT_ZOMBIE_DESC;

private:
	CIs_Hit_Zombie();
	CIs_Hit_Zombie(const CIs_Hit_Zombie& rhs);
	virtual ~CIs_Hit_Zombie() = default;

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
	unordered_set<COLLIDER_TYPE>				m_CheckColliderTypes;
	unordered_set<HIT_TYPE>						m_CheckHitTypes;	

public:
	static CIs_Hit_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END