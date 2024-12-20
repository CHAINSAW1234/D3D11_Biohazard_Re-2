#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Zombie.h"
#include "Body_Zombie.h"

BEGIN(Client)

class CCreep_Zombie : public CTask_Node
{
private:
	CCreep_Zombie();
	CCreep_Zombie(const CCreep_Zombie& rhs);
	virtual ~CCreep_Zombie() = default;

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
	CZombie::FACE_STATE				m_eStartFaceState = { CZombie::FACE_STATE::_END };

private:	/* For. Active FirstTime */
	const wstring					m_strCreepAnimLayerTag = { TEXT("Lost_Idle") };	
	const PLAYING_INDEX				m_ePlayingIndex = { PLAYING_INDEX::INDEX_0 };

public:
	static CCreep_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END