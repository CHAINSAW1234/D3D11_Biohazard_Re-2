#pragma once

#include "Client_Defines.h"
#include "Decorator_Node.h"

BEGIN(Client)

class CIs_Character_In_Range_Zombie : public CDecorator_Node
{
private:
	CIs_Character_In_Range_Zombie();
	CIs_Character_In_Range_Zombie(const CIs_Character_In_Range_Zombie& rhs);
	virtual ~CIs_Character_In_Range_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual _bool					Condition_Check();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard)
	{
		m_pBlackBoard = pBlackBoard;
	}
protected:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };
public:
	static CIs_Character_In_Range_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END