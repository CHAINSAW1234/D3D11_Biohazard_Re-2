#pragma once

#include "Client_Defines.h"
#include "Decorator_Node.h"
#include "Body_Zombie.h"

BEGIN(Client)

class CIs_Hit_Zombie : public CDecorator_Node
{
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

public:
	static CIs_Hit_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END