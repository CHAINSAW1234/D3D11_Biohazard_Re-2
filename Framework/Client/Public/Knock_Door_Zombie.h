#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"

BEGIN(Client)

class CKnock_Door_Zombie : public CTask_Node
{
private:
	CKnock_Door_Zombie();
	CKnock_Door_Zombie(const CKnock_Door_Zombie& rhs);
	virtual ~CKnock_Door_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

private:
	void							Change_Animation(_float fTimeDelta);

public:
	inline void						SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard;	}

protected:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };

	_int							m_iBasePlayingIndex = { -1 };
	const wstring					m_strAnimLayerTag = { TEXT("Body_Zombie_Gimmick_Door")};

public:
	static CKnock_Door_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END