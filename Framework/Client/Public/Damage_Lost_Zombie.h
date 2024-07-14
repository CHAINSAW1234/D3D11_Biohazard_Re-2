#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"

BEGIN(Client)

class CDamage_Lost_Zombie : public CTask_Node
{
public:
	//	Stand UP 
	enum class ZOMBIE_DAMAGE_LOST_TYPE { 
		_A_F_L,			//	Lost Ankle
		_A_F_R,	
		_B_F_L,			//	Lost Tabia
		_B_F_R, 
		_C_F_L,			//	Lost Femur
		_C_F_R, 
		_END };

private:
	CDamage_Lost_Zombie();
	CDamage_Lost_Zombie(const CDamage_Lost_Zombie& rhs);
	virtual ~CDamage_Lost_Zombie() = default;

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

private:	/* For. Active FirstTime */
	ZOMBIE_DAMAGE_LOST_TYPE			m_eLostType = { ZOMBIE_DAMAGE_LOST_TYPE::_END };
	wstring							m_strAnimLayerTag = { TEXT("Damage_Lost") };
	PLAYING_INDEX					m_eBasePlayingIndex = { PLAYING_INDEX::INDEX_0 };

public:
	static CDamage_Lost_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END