#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"


BEGIN(Client)

class CKnock_Back_Zombie : public CTask_Node
{
private:
	CKnock_Back_Zombie();
	CKnock_Back_Zombie(const CKnock_Back_Zombie& rhs);
	virtual ~CKnock_Back_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

public:
	inline void						Reset_Entry() { m_isEntry = true; }

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
	static CKnock_Back_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END