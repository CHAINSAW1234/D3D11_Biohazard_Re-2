#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"
#include "Zombie.h"

BEGIN(Client)

class CSet_Focus_Player : public CTask_Node
{
private:
	CSet_Focus_Player();
	CSet_Focus_Player(const CSet_Focus_Player& rhs);
	virtual ~CSet_Focus_Player() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }

private:
	class CBlackBoard_Zombie* m_pBlackBoard = { nullptr };

public:
	static CSet_Focus_Player* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END