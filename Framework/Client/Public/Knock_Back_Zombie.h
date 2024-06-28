#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"

BEGIN(Client)

class CKnock_Back_Zombie : public CTask_Node
{
private:
	CKnock_Back_Zombie();
	CKnock_Back_Zombie(const CKnock_Back_Zombie& rhs);
	virtual ~CKnock_Back_Zombie() = default;

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

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }
private:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };

private:	/* For. Active FirstTime */
	_bool							m_isEntry = { false };
	COLLIDER_TYPE					m_eCurrentHitCollider = { COLLIDER_TYPE::_END };

	const wstring&					m_strAnimLayerTag = { TEXT("Damage_Knockback") };
	const wstring&					m_strBoneLayerTag = { BONE_LAYER_DEFAULT_TAG };
	const PLAYING_INDEX				m_ePlayingIndex = { PLAYING_INDEX::INDEX_0 };

public:
	static CKnock_Back_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END