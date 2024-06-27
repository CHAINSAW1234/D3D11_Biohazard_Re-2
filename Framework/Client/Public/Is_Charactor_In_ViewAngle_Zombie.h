#pragma once

#include "Client_Defines.h"
#include "Decorator_Node.h"

BEGIN(Client)

class CIs_Charactor_In_ViewAngle_Zombie : public CDecorator_Node
{
public:
	CIs_Charactor_In_ViewAngle_Zombie();
	CIs_Charactor_In_ViewAngle_Zombie(const CIs_Charactor_In_ViewAngle_Zombie& rhs);
	virtual ~CIs_Charactor_In_ViewAngle_Zombie() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);

	virtual _bool					Condition_Check();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard;	}
protected:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };

public:
	static CIs_Charactor_In_ViewAngle_Zombie* Create();

public:
	virtual void Free() override;
};

END