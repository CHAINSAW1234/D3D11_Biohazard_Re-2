#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"


BEGIN(Client)

class CStart_Zombie : public CTask_Node
{
private:
	CStart_Zombie();
	CStart_Zombie(const CStart_Zombie& rhs);
	virtual ~CStart_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }
private:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };

private:
	_bool							m_isStart = { false };

public:
	static CStart_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END