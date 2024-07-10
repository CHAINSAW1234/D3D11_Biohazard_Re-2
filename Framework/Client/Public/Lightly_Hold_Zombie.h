#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Zombie.h"
#include "Body_Zombie.h"

BEGIN(Client)

class CLightly_Hold_Zombie : public CTask_Node
{
public:
	enum class LIGHTLY_HOLD_ANIM_TYPE{ _LIGHTLY_HOLD, _PUSH_DOWN, _END };
	enum class LIGHTLY_HOLD_ANIM_STATE { _START, _MIDDLE, _FINISH, _END };

private:
	CLightly_Hold_Zombie();
	CLightly_Hold_Zombie(const CLightly_Hold_Zombie& rhs);
	virtual ~CLightly_Hold_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

private:
	_bool							Condition_Check();

private:
	void							Change_Animation();

	void							Change_Animation_Lightly_Hold();
	void							Change_Animation_Bite_PushDown();

private:
	LIGHTLY_HOLD_ANIM_STATE			Compute_Current_AnimState_Lightly_Hold();

	_bool							Is_CurrentAnim_StartAnim();
	_bool							Is_CurrentAnim_MiddleAnim();
	_bool							Is_CurrentAnim_FinishAnim();

private:
	void							Set_Lightly_Hold_LinearStart_HalfMatrix();
	_bool							Is_StateFinished(LIGHTLY_HOLD_ANIM_STATE eState);

private:
	void							Update_PoseState_FaceState();

private:
	void							Apply_HalfMatrix(_float fTimeDelta);

private:
	HRESULT							SetUp_AnimBranches();

public:
	void							SetBlackBoard(class CBlackBoard_Zombie* pBlackBoard) { m_pBlackBoard = pBlackBoard; }
private:
	class CBlackBoard_Zombie*		m_pBlackBoard = { nullptr };
	LIGHTLY_HOLD_ANIM_TYPE			m_eAnimType = { LIGHTLY_HOLD_ANIM_TYPE::_END };
	LIGHTLY_HOLD_ANIM_STATE			m_eAnimState = { LIGHTLY_HOLD_ANIM_STATE::_END };

	_bool							m_isEntry = { false };
	_bool							m_isHoldTarget = { false };

	DIRECTION						m_eDirectionToPlayer = { DIRECTION::_END };

	const wstring					m_strPushDownAnimLayerTag = { TEXT("Bite_Push_Down") };
	const wstring					m_strLightlyHoldAnimLayerTag = { TEXT("Bite_Lightly_Hold") };

	const wstring					m_strBoneLayerTag = { BONE_LAYER_DEFAULT_TAG };

	const PLAYING_INDEX				m_ePlayingIndex = { PLAYING_INDEX::INDEX_0 };

	unordered_map<wstring, unordered_set<_uint>>		m_StartAnims;
	unordered_map<wstring, unordered_set<_uint>>		m_MiddleAnims;
	unordered_map<wstring, unordered_set<_uint>>		m_FinishAnims;

	_bool							m_isSendMSG_To_Player = { false };

	_float4x4						m_Delta_Matrix_To_HalfMatrix;

	_float							m_fAccLinearTime_HalfMatrix = { 0.f };
	_float							m_fTotalLinearTime_HalfMatrix = { 0.f };

public:
	static CLightly_Hold_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END