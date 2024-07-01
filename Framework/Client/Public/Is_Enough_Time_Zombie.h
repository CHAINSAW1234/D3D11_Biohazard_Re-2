#pragma once

#include "Client_Defines.h"
#include "Decorator_Node.h"

BEGIN(Client)

class CIs_Enough_Time_Zombie : public CDecorator_Node
{
public:
	CIs_Enough_Time_Zombie();
	CIs_Enough_Time_Zombie(const CIs_Enough_Time_Zombie& rhs);
	virtual ~CIs_Enough_Time_Zombie() = default;

public:
	virtual HRESULT					Initialize(_float* pAccTime, _float* pNeedTime);

	virtual _bool					Condition_Check();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }
private:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };

	const _float*					m_pAccTime = { nullptr };
	const _float*					m_pNeedTime = { nullptr };

public:
	static CIs_Enough_Time_Zombie* Create(_float* pAccTime, _float* pNeedTime);

public:
	virtual void Free() override;
};

END