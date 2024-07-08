#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"


BEGIN(Client)

class CRegion_Update_Zombie : public CTask_Node
{
private:
	CRegion_Update_Zombie();
	CRegion_Update_Zombie(const CRegion_Update_Zombie& rhs);
	virtual ~CRegion_Update_Zombie() = default;

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
	static CRegion_Update_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END