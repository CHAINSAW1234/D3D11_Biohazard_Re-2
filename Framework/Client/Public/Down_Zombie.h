#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"


BEGIN(Client)

class CDown_Zombie : public CTask_Node
{
private:
	CDown_Zombie();
	CDown_Zombie(const CDown_Zombie& rhs);
	virtual ~CDown_Zombie() = default;

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

private:	/* For. Active FirstTime */
	_bool							m_isEntry = { false };
	COLLIDER_TYPE					m_eCurrentHitCollider = { COLLIDER_TYPE::_END };

public:
	static CDown_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END