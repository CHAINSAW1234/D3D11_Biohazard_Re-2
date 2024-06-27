#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"


BEGIN(Client)

class CHold_Zombie : public CTask_Node
{
private:
	CHold_Zombie();
	CHold_Zombie(const CHold_Zombie& rhs);
	virtual ~CHold_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

private:
	void							Change_Animation();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard;	}
private:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };

	_bool							m_isEntry = { false };

public:
	static CHold_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END