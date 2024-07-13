#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"

BEGIN(Client)

class CDamage_Lost_Zombie : public CTask_Node
{
public:
	enum class ZOMBIE_DAMAGE_LOST_TYPE { _A_L, _A_R, _B_L, _B_R, _C_L, _C_R, _END };

private:
	CDamage_Lost_Zombie();
	CDamage_Lost_Zombie(const CDamage_Lost_Zombie& rhs);
	virtual ~CDamage_Lost_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

public:
	inline void						Reset_Entry() { m_isEntry = true; }

private:
	void							Update_Current_Collider();
	void							Change_Animation();
	void							Change_Animation_StandUp();
	void							Change_Animation_Creep();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }
private:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };

private:	/* For. Active FirstTime */
	_bool							m_isEntry = { false };
	COLLIDER_TYPE					m_eCurrentHitCollider = { COLLIDER_TYPE::_END };

	const wstring					m_strStunAnimLayerTag = { TEXT("Damage_Stun") };
	const wstring					m_strDefualtStunAnimLayerTag = { TEXT("Damage_Default") };
	const wstring					m_strBoneLayerTag = { BONE_LAYER_DEFAULT_TAG };
	const PLAYING_INDEX				m_ePlayingIndex = { PLAYING_INDEX::INDEX_0 };

public:
	static CDamage_Lost_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END