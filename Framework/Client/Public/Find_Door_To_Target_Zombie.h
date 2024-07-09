#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"

BEGIN(Client)

class CFind_Door_To_Target_Zombie : public CTask_Node
{
private:
	CFind_Door_To_Target_Zombie();
	CFind_Door_To_Target_Zombie(const CFind_Door_To_Target_Zombie& rhs);
	virtual ~CFind_Door_To_Target_Zombie() = default;

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
	

public:
	static CFind_Door_To_Target_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END