#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"

BEGIN(Client)

class CMoveTo_Zombie : public CTask_Node
{
public:
	CMoveTo_Zombie();
	CMoveTo_Zombie(const CMoveTo_Zombie& rhs);
	virtual ~CMoveTo_Zombie() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute() override;
	virtual void					Exit() override;

private:
	void							Change_Animation();

	void							Move_Front();
	void							Move_Front_Include_Rotaiton(_bool isRight, _float fAngle);
	void							Turn();

	_bool							Is_CurrentAnim_StartAnim(PLAYING_INDEX eIndex);
	_bool							Is_CurrentAnim_LoopAnim(PLAYING_INDEX eIndex);

private:


	_int							Compute_Base_Anim();
	_int							Compute_Blend_Anim();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard)
	{
		m_pBlackBoard = pBlackBoard;
	}

protected:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };

	_int							m_iBlendPlayingIndex = { -1 };
	_int							m_iBasePlayingIndex = { -1 };

	unordered_map<wstring, unordered_set<_uint>>			m_StartAnimIndicesLayer;
	unordered_map<wstring, unordered_set<_uint>>			m_LoopAnimIndicesLayer;

public:
	static CMoveTo_Zombie* Create();

public:
	virtual void Free() override;
};

END