#pragma once

#include "Client_Defines.h"
#include "Decorator_Node.h"

BEGIN(Client)

class CIs_Start_Zombie : public CDecorator_Node
{
private:
	CIs_Start_Zombie();
	CIs_Start_Zombie(const CIs_Start_Zombie& rhs);
	virtual ~CIs_Start_Zombie() = default;

public:
	virtual HRESULT					Initialize();

	virtual _bool					Condition_Check();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard)
	{
		m_pBlackBoard = pBlackBoard;
	}
private:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };
	_bool							m_isStart = { false };

public:
	static CIs_Start_Zombie* Create();

public:
	virtual void Free() override;
};

END