#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Body_Zombie.h"

#define ZOMBIE_MOVE_SOUND_MAX_REMAIN		6.f
#define ZOMBIE_MOVE_SOUND_MIN_REMAIN		4.f

BEGIN(Client)

class CMove_To_Target_Zombie : public CTask_Node
{
public:
	enum TURN_DIR { DIR_LEFT, DIR_RIGHT, DIR_END };

private:
	CMove_To_Target_Zombie();
	CMove_To_Target_Zombie(const CMove_To_Target_Zombie& rhs);
	virtual ~CMove_To_Target_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

private:
	void							Change_Animation_Front_Only(_float fTimeDelta);
	void							Change_Animation_Include_Rotation(_float fTimeDelta);

	void							Turn_To_TargetObject_Manual(_float fTimeDelta);

	_bool							Is_CurrentAnim_StartAnim(PLAYING_INDEX eIndex);
	_bool							Is_CurrentAnim_LoopAnim(PLAYING_INDEX eIndex);

private:
	_int							Compute_Base_Anim();
	_int							Compute_Blend_Anim();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }

protected:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };

	_int							m_iBlendPlayingIndex = { -1 };
	_int							m_iBasePlayingIndex = { -1 };

	_bool							m_isPreBlended = { false };
	TURN_DIR						m_ePreTurnDir = { DIR_END };
	_float							m_fAccBlendTime = { 0.f };

	_bool							m_isIncludeRotation = { false };

	CGameObject*					m_pTargetObject = { nullptr };

	unordered_map<wstring, unordered_set<_uint>>			m_StartAnimIndicesLayer;
	unordered_map<wstring, unordered_set<_uint>>			m_LoopAnimIndicesLayer;

	_float							m_fRemainSoundTime = { 0.f };
	_bool							m_isEnterNextRoom = { false };
	_bool							m_isMoveToPlayer_LinkedRoom = { false };
	LOCATION_MAP_VISIT				m_eNextRoom = { LOCATION_MAP_VISIT_END };

public:
	static CMove_To_Target_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END