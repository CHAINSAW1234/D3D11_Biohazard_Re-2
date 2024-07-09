#pragma once

#include "Client_Defines.h"
#include "Decorator_Node.h"

BEGIN(Client)

class CIs_Different_Location_To_Player_Zombie : public CDecorator_Node
{
private:
	CIs_Different_Location_To_Player_Zombie();
	CIs_Different_Location_To_Player_Zombie(const CIs_Different_Location_To_Player_Zombie& rhs);
	virtual ~CIs_Different_Location_To_Player_Zombie() = default;

public:
	virtual HRESULT					Initialize(_float fRange);

	virtual _bool					Condition_Check();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }

private:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };

public:
	static CIs_Different_Location_To_Player_Zombie* Create(_float fRange = 0.f);

public:
	virtual void Free() override;
};

END