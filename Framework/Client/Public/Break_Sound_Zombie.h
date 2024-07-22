#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"
#include "Zombie.h"

#define FOOT_UP_IGNORE_TIME				0.1f
#define FOOT_DOWN_IGNORE_TIME			0.1f

BEGIN(Client)

class CBreak_Sound_Zombie : public CTask_Node
{
private:
	CBreak_Sound_Zombie();
	CBreak_Sound_Zombie(const CBreak_Sound_Zombie& rhs);
	virtual ~CBreak_Sound_Zombie() = default;

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
	static CBreak_Sound_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END