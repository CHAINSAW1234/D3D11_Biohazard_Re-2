#pragma once

#include "Client_Defines.h"
#include "Decorator_Node.h"

BEGIN(Client)

class CIs_Out_Door_Zombie : public CDecorator_Node
{
private:
	CIs_Out_Door_Zombie();
	CIs_Out_Door_Zombie(const CIs_Out_Door_Zombie& rhs);
	virtual ~CIs_Out_Door_Zombie() = default;

public:
	virtual HRESULT					Initialize();

	virtual _bool					Condition_Check();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard)
	{
		m_pBlackBoard = pBlackBoard;
	}
protected:
	class CBlackBoard_Zombie* m_pBlackBoard = { nullptr };

public:
	static CIs_Out_Door_Zombie* Create();

public:
	virtual void Free() override;
};

END