#pragma once

#include "Client_Defines.h"
#include "Decorator_Node.h"

BEGIN(Client)

class CIs_Character_In_Range_Zombie : public CDecorator_Node
{
public:
	CIs_Character_In_Range_Zombie();
	CIs_Character_In_Range_Zombie(const CIs_Character_In_Range_Zombie& rhs);
	virtual ~CIs_Character_In_Range_Zombie() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);

	virtual _bool					Condition_Check() { return false; }

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard)
	{
		m_pBlackBoard = pBlackBoard;
	}
protected:
	class CGameInstance*			m_pGameInstance = { nullptr };
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };
public:
	static CIs_Character_In_Range_Zombie* Create();

public:
	virtual void Free() override;
};

END