#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"

BEGIN(Client)

class CPivot_Turn_Zombie : public CTask_Node
{
public:
	CPivot_Turn_Zombie();
	CPivot_Turn_Zombie(const CPivot_Turn_Zombie& rhs);
	virtual ~CPivot_Turn_Zombie() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual void					Execute() override;
	virtual void					Exit() override;

private:
	void							Change_Animation();

	void							Turn();

	_bool							Is_CurrentAnim_StartAnim(PLAYING_INDEX eIndex);
	_bool							Is_CurrentAnim_LoopAnim(PLAYING_INDEX eIndex);

private:
	_int							Compute_Base_Anim();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard)
	{
		m_pBlackBoard = pBlackBoard;
	}

protected:
	class CBlackBoard_Zombie*								m_pBlackBoard = { nullptr };

	_int													m_iBasePlayingIndex = { -1 };

	unordered_map<wstring, unordered_set<_uint>>			m_StartAnimIndicesLayer;
	unordered_map<wstring, unordered_set<_uint>>			m_LoopAnimIndicesLayer;

public:
	static CPivot_Turn_Zombie* Create();

public:
	virtual void Free() override;
};

END