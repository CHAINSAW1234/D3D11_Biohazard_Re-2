#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"


BEGIN(Client)

class CLightly_Hold_Zombie : public CTask_Node
{
private:
	CLightly_Hold_Zombie();
	CLightly_Hold_Zombie(const CLightly_Hold_Zombie& rhs);
	virtual ~CLightly_Hold_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

private:
	void							Change_Animation();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }
private:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };

	_bool							m_isEntry = { false };

public:
	static CLightly_Hold_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END