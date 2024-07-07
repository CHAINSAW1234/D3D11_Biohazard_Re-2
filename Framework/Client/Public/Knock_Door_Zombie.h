#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"

#define ZOMBIE_ATTACK_DOOR_NEED_TIME							2.f
#define ZOMBIE_DOOR_KNOCK_TOTAL_INTERPOLATE_TO_WINDOW_TIME		0.5f

BEGIN(Client)

class CKnock_Door_Zombie : public CTask_Node
{
public:
	enum class ANIM_STATE { _START, _LOOP, _FINISH, _END };
	enum class ANIM_TYPE { _A, _B, _END };

private:
	CKnock_Door_Zombie();
	CKnock_Door_Zombie(const CKnock_Door_Zombie& rhs);
	virtual ~CKnock_Door_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

private:
	void							Change_Animation(_float fTimeDelta);

public:
	inline void						SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard;	}

protected:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };

	PLAYING_INDEX					m_eBasePlayingIndex = { PLAYING_INDEX::INDEX_0 };
	const wstring					m_strAnimLayerTag = { TEXT("Gimmick_Door") };

	ANIM_TYPE						m_eAnimType = { ANIM_TYPE::_END };
	ANIM_STATE						m_eAnimState = { ANIM_STATE::_END };

	_float							m_fAccKnockTime = { 0.f };

	_float							m_fAccLinearInterpolateTime = { 0.f };
	_float4x4						m_InterpolateDeltaMatrix = {};

public:
	static CKnock_Door_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END