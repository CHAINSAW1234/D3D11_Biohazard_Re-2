#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"

BEGIN(Client)

class CStun_Zombie : public CTask_Node
{
public:
	enum class COLLISION_SECTION { _SHOULDER_L, _SHOULDER_R, _LEG_L, _LEG_R, _BODY, _HEAD, _END };

private:
	CStun_Zombie();
	CStun_Zombie(const CStun_Zombie& rhs);
	virtual ~CStun_Zombie() = default;

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
	static CStun_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END