#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"
#include "Zombie.h"

#define ZOMBIE_RAISE_UP_HAND_MAX_TIME			3.f
#define ZOMBIE_RAISE_UP_HAND_MAX_RATIO			0.8f

BEGIN(Client)

class CRaise_Up_Hand_Zombie : public CTask_Node
{
private:
	CRaise_Up_Hand_Zombie();
	CRaise_Up_Hand_Zombie(const CRaise_Up_Hand_Zombie& rhs);
	virtual ~CRaise_Up_Hand_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

private:
	void							Set_Hand_AdditionalMatrices();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }

private:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };

	_float							m_fAccActiveTime = { 0.f };

private:

public:
	static CRaise_Up_Hand_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END