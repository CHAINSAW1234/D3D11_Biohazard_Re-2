#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"

BEGIN(Client)

class CDead_Zombie : public CTask_Node
{
public:
	enum class ZOMBIE_DEAD_ANIM_TYPE { _1, _2, _3, _4, _END };
	enum class ZOMBIE_DEAD_ANIM_STATE { _LOOK_PLAYER, _HIT, _END };

private:
	CDead_Zombie();
	CDead_Zombie(const CDead_Zombie& rhs);
	virtual ~CDead_Zombie() = default;

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

private:
	wstring							m_strAnimLayerTag = { TEXT("Undiscovered_Dead") };
	PLAYING_INDEX					m_eBasePlayingIndex = { PLAYING_INDEX::INDEX_0 };

	ZOMBIE_DEAD_ANIM_TYPE			m_eDeadAnimType = { ZOMBIE_DEAD_ANIM_TYPE::_END };
	ZOMBIE_DEAD_ANIM_STATE			m_eDeadAnimState = { ZOMBIE_DEAD_ANIM_STATE::_END };

	_bool							m_isEnd = { false };

public:
	static CDead_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END