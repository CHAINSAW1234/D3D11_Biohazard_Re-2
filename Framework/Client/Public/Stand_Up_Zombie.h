#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"

BEGIN(Client)

class CStand_Up_Zombie : public CTask_Node
{
private:
	CStand_Up_Zombie();
	CStand_Up_Zombie(const CStand_Up_Zombie& rhs);
	virtual ~CStand_Up_Zombie() = default;

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

private:
	_bool							m_isFaceUp = { false };

public:
	static CStand_Up_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END