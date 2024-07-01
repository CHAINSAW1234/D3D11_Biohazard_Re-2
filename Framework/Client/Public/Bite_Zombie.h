#pragma once

#include "Client_Defines.h"
#include "Task_Node.h"
#include "Zombie.h"
#include "Body_Zombie.h"

BEGIN(Client)

class CBite_Zombie : public CTask_Node
{
public:
	enum class BITE_ANIM_STATE { _START, _MIDDLE, _FINISH, _END };
	enum class BITE_ANIM_TYPE { _DEFAULT_F, _DEFAULT_B, _CREEP, _PUSH_DOWN, _END };
private:
	CBite_Zombie();
	CBite_Zombie(const CBite_Zombie& rhs);
	virtual ~CBite_Zombie() = default;

public:
	virtual HRESULT					Initialize(void* pArg);

	virtual void					Enter() override;
	virtual _bool					Execute(_float fTimeDelta) override;
	virtual void					Exit() override;

private:
	void							Change_Animation(BITE_ANIM_STATE eState);

	void							Change_Animation_Default_Front(BITE_ANIM_STATE eState);
	void							Change_Animation_Default_Back(BITE_ANIM_STATE eState);
	void							Change_Animation_Creep(BITE_ANIM_STATE eState);
	void							Change_Animation_Push_Down(BITE_ANIM_STATE eState);
	void							Change_Animation_Lost(BITE_ANIM_STATE eState);

private:
	BITE_ANIM_STATE 				Compute_Current_AnimState_Bite();

	_bool							Is_CurrentAnim_StartAnim();
	_bool							Is_CurrentAnim_MiddleAnim();
	_bool							Is_CurrentAnim_FinishAnim();
	_bool							Is_Can_Start_Bite();

private:
	void							Set_Bite_LinearStart_HalfMatrix();
	_bool							Is_StateFinished(BITE_ANIM_STATE eState);

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

	const wstring&					m_strDefaultFrontAnimLayerTag = { TEXT("Bite_Default_Front") };
	const wstring&					m_strDefaultBackAnimLayerTag = { TEXT("Bite_Default_Back") };
	const wstring&					m_strPushDownAnimLayerTag = { TEXT("Bite_Push_Down") };
	const wstring&					m_strCreepAnimLayerTag = { TEXT("Bite_Creep") };
	const wstring&					m_strETCAnimLayerTag = { TEXT("Bite_ETC") };
	const wstring&					m_strBoneLayerTag = { BONE_LAYER_DEFAULT_TAG };
	const PLAYING_INDEX				m_ePlayingIndex = { PLAYING_INDEX::INDEX_0 };

	BITE_ANIM_TYPE					m_eAnimType = { BITE_ANIM_TYPE::_END };

	unordered_map<wstring, unordered_set<_uint>>		m_StartAnims;
	unordered_map<wstring, unordered_set<_uint>>		m_MiddleAnims;
	unordered_map<wstring, unordered_set<_uint>>		m_FinishAnims;

	CZombie::FACE_STATE				m_eStartFaceState = { CZombie::FACE_STATE::_END };
	CZombie::POSE_STATE				m_eStartPoseState = { CZombie::POSE_STATE::_END };

	MONSTER_STATE					m_ePreState = { MONSTER_STATE::MST_END };

	_bool							m_isSendMSG_To_Player = { false };

	_float							m_fAccLinearTime_HalfMatrix = { 0.f };
	_float							m_fTotalLinearTime_HalfMatrix = { 0.f };

	_float4x4						m_Delta_Matrix_To_HalfMatrix;

	_bool							m_isFrontFromPlayer = { false };

public:
	static CBite_Zombie* Create(void* pArg = nullptr);

public:
	virtual void Free() override;
};

END