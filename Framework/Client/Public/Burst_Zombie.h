#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"


BEGIN(Client)

class CBurst_Zombie : public CTask_Node
{
private:
	CBurst_Zombie();
	CBurst_Zombie(const CBurst_Zombie& rhs);
	virtual ~CBurst_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

private:
	void							Change_Animation();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard)
	{
		m_pBlackBoard = pBlackBoard;
	}
private:
	class CBlackBoard_Zombie* m_pBlackBoard = { nullptr };

private:	/* For. Active FirstTime */
	_bool							m_isWake = { false };

public:
	static CBurst_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END