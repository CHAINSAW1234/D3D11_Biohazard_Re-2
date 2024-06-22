#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"


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
	virtual void					Execute() override;
	virtual void					Exit() override;

private:
	void							Change_Animation();

	void							Move_Front();
	void							Move_Front_Include_Rotaiton(_bool isRight, _float fAngle);
	void							Turn();

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

public:
	static CMoveTo_Zombie* Create();

public:
	virtual void Free() override;
};

END