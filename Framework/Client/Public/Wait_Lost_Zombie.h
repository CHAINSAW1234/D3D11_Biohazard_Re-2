#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Zombie.h"
#include "Body_Zombie.h"

#define ZOMBIE_IDLE_SOUND_MAX_REMAIN		6.f
#define ZOMBIE_IDLE_SOUND_MIN_REMAIN		4.f

BEGIN(Client)

class CWait_Lost_Zombie : public CTask_Node
{
private:
	CWait_Lost_Zombie();
	CWait_Lost_Zombie(const CWait_Lost_Zombie& rhs);
	virtual ~CWait_Lost_Zombie() = default;

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
	PLAYING_INDEX					m_eBasePlayingIndex = { PLAYING_INDEX::INDEX_0 };
	wstring							m_strAnimLayerTag = { TEXT("Lost_Idle") };
	_int							m_iAnimationIndex = { -1 };
	_float							m_fRemainSoundTime = { 0.f };

public:
	static CWait_Lost_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END