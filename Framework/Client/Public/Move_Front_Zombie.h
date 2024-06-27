#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"

BEGIN(Client)

class CMove_Front_Zombie : public CTask_Node
{
public:
	enum TURN_DIR { DIR_LEFT, DIR_RIGHT, DIR_END };

private:
	CMove_Front_Zombie();
	CMove_Front_Zombie(const CMove_Front_Zombie& rhs);
	virtual ~CMove_Front_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

private:
	void							Change_Animation(_float fTimeDelta);

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

	_bool							m_isPreBlended = { false };
	TURN_DIR						m_ePreTurnDir = { DIR_END };
	_float							m_fAccBlendTime = { 0.f };

	unordered_map<wstring, unordered_set<_uint>>			m_StartAnimIndicesLayer;
	unordered_map<wstring, unordered_set<_uint>>			m_LoopAnimIndicesLayer;

public:
	static CMove_Front_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END