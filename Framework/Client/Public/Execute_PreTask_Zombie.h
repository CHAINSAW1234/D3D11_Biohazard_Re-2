#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"


BEGIN(Client)

class CExecute_PreTask_Zombie : public CTask_Node
{
private:
	CExecute_PreTask_Zombie();
	CExecute_PreTask_Zombie(const CExecute_PreTask_Zombie& rhs);
	virtual ~CExecute_PreTask_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard)
	{
		m_pBlackBoard = pBlackBoard;
	}
private:
	class CBlackBoard_Zombie* m_pBlackBoard = { nullptr };

private:	/* For. Active FirstTime */
	_bool							m_isWake = { false };
	CTask_Node*						m_pPreTaskNode = { nullptr };

public:
	static CExecute_PreTask_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END