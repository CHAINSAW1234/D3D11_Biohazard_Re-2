#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"


BEGIN(Client)

class CShake_Skin_Zombie : public CTask_Node
{
private:
	CShake_Skin_Zombie();
	CShake_Skin_Zombie(const CShake_Skin_Zombie& rhs);
	virtual ~CShake_Skin_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

private:
	void							Change_Animation();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard)
	{
		m_pBlackBoard = pBlackBoard;
	}
private:
	class CBlackBoard_Zombie* m_pBlackBoard = { nullptr };

private:	/* For. Active FirstTime */
	_bool							m_isWake = { false };

public:
	static CShake_Skin_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END