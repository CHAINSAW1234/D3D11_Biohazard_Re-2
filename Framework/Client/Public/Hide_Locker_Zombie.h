#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"

BEGIN(Client)

class CHide_Locker_Zombie : public CTask_Node
{
private:
	CHide_Locker_Zombie();
	CHide_Locker_Zombie(const CHide_Locker_Zombie& rhs);
	virtual ~CHide_Locker_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }

private:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };
	
private:
	wstring							m_strAnimLayerTag = { TEXT("Dead_Hide_Locker") };
	PLAYING_INDEX					m_eBasePlayingIndex = { PLAYING_INDEX::INDEX_0 };

	class CCabinet*					m_pTarget_Cabinet = { nullptr };

	_float							m_fAcc_Wait_Time = { 0.f };
	_float							m_fTotal_Wait_Time = { 0.f };
	_bool							m_isStart = { false };
	_bool							m_isEnd = { false };	

public:
	static CHide_Locker_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END