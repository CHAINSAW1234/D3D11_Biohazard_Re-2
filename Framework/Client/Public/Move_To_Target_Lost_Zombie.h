#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"

BEGIN(Client)

class CMove_To_Target_Lost_Zombie : public CTask_Node
{
public:
	enum class ZOMBIE_MOVE_LOST_TYPE { _A, _B, _B_SYM, _C, _C_SYM, _D, _D_SYM, _E, _E_SYM, _F, _F_SYM, _G, _G_SYM, _H, _I, _J, _L_ANKLE, _R_ANKLE, _END};

public:
	enum TURN_DIR { DIR_LEFT, DIR_RIGHT, DIR_END };

private:
	CMove_To_Target_Lost_Zombie();
	CMove_To_Target_Lost_Zombie(const CMove_To_Target_Lost_Zombie& rhs);
	virtual ~CMove_To_Target_Lost_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

private:
	void							Change_Animation();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }

protected:
	class CBlackBoard_Zombie* m_pBlackBoard = { nullptr };

	PLAYING_INDEX					m_eBase_PlayingIndex = { PLAYING_INDEX::INDEX_0 };
	wstring							m_strAnimLayerTag = { TEXT("Lost_Walk") };

	ZOMBIE_MOVE_LOST_TYPE			m_eMoveLostAnimType = { ZOMBIE_MOVE_LOST_TYPE::_END };	

public:
	static CMove_To_Target_Lost_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END