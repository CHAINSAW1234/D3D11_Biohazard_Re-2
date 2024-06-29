#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"

BEGIN(Client)

class CHold_Zombie : public CTask_Node
{	
private:
	CHold_Zombie();
	CHold_Zombie(const CHold_Zombie& rhs);
	virtual ~CHold_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

private:
	void							Change_Animation();

	_bool							Is_StateFinished();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard;	}
private:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };

	_bool							m_isEntry = { false };
	const PLAYING_INDEX				m_eBasePlayingIndex = { PLAYING_INDEX::INDEX_0 };
	const PLAYING_INDEX				m_eBlendPlayingIndex = { PLAYING_INDEX::INDEX_1 };

public:
	static CHold_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END