#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"


BEGIN(Client)

class CSleep_Zombie : public CTask_Node
{
public:
	CSleep_Zombie();
	CSleep_Zombie(const CSleep_Zombie& rhs);
	virtual ~CSleep_Zombie() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute() override;
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
	static CSleep_Zombie* Create();

public:
	virtual void Free() override;
};

END