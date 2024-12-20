#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"

BEGIN(Client)

class CWait_Zombie : public CTask_Node
{
private:
	CWait_Zombie();
	CWait_Zombie(const CWait_Zombie& rhs);
	virtual ~CWait_Zombie() = default;

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
	_float							m_fRemainSoundTime = { 0.f };

public:
	static CWait_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END