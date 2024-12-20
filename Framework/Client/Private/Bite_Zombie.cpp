#include "stdafx.h"
#include "GameInstance.h"
#include "Bite_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"

#define	CREEP_FACE_DOWN_L_TRACKPOSITION	85.f
#define CREEP_FACE_DOWN_R_TRACKPOSITION 90.f
#define CREEP_FACE_UP_L_TRACKPOSITION 95.f
#define CREEP_FACE_UP_R_TRACKPOSITION 85.f
#define BACK_TRACKPOSITION 25.f
#define FRONT_TRACKPOSITION 60.f

CBite_Zombie::CBite_Zombie()
	: CTask_Node()
{
}

CBite_Zombie::CBite_Zombie(const CBite_Zombie& rhs)
{
}

HRESULT CBite_Zombie::Initialize(void* pArg)
{
	if (FAILED(SetUp_AnimBranches()))
		return E_FAIL;

	m_fTotalLinearTime_HalfMatrix = 0.2f;

	return S_OK;
}

void CBite_Zombie::Enter()
{
	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	pBodyModel->Set_TotalLinearInterpolation(0.3f);
	pBodyModel->Set_Loop(static_cast<_uint>(PLAYING_INDEX::INDEX_0), false);

	pBodyModel->Set_BlendWeight(static_cast<_uint>(PLAYING_INDEX::INDEX_1), 0.f, 0.f);
	pBodyModel->Reset_PreAnim_CurrentAnim(static_cast<_uint>(PLAYING_INDEX::INDEX_1));

	m_pBlackBoard->Get_AI()->Set_ManualMove(true);

	m_isSendMSG_To_Player = false;
	m_eAnimType = BITE_ANIM_TYPE::_END;

	m_eStartPoseState = m_pBlackBoard->Get_AI()->Get_PoseState();
	m_eStartFaceState = m_pBlackBoard->Get_AI()->Get_FaceState();

	DIRECTION		eDirection = { DIRECTION::_END };
	_float3			vDirectionFromPlayerLocal;
	m_pBlackBoard->Compute_Direction_From_Player_Local(&vDirectionFromPlayerLocal);

	m_isFrontFromPlayer = { vDirectionFromPlayerLocal.z > 0.f };
	XMStoreFloat4x4(&m_Delta_Matrix_To_HalfMatrix, XMMatrixIdentity());

	m_fAccLinearTime_HalfMatrix = 0.f;

#ifdef _DEBUG
	cout << "Enter Bite " << endl;
#endif 
}

_bool CBite_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	MONSTER_STATE			eMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };
	_bool					isEntry = { eMonsterState != MONSTER_STATE::MST_BITE };
	if (true == isEntry)
	{
		if (false == Is_Can_Start_Bite())
			return false;
	}

	BITE_ANIM_STATE			eAnimState = { Compute_Current_AnimState_Bite() };
	if (true == Is_StateFinished(eAnimState))
		return false;

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_BITE);

#pragma region Effect
	Initiate_Effect();
#pragma endregion

	Change_Animation(eAnimState);

	if (false == m_isSendMSG_To_Player)
	{
		Set_Bite_LinearStart_HalfMatrix();
		m_isSendMSG_To_Player = true;
	}
	Apply_HalfMatrix(fTimeDelta);
	Update_PoseState_FaceState();

	if (BITE_ANIM_STATE::_FINISH == eAnimState)
	{
		CModel* pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
		if (nullptr == pBody_Model)
			return false;

		_float			fTrackPosition = { pBody_Model->Get_Duration_From_PlayingInfo(static_cast<_uint>(m_ePlayingIndex)) };
		_float			fDuration = { pBody_Model->Get_Duration_From_PlayingInfo(static_cast<_uint>(m_ePlayingIndex)) };

		_float			fRatio = { fTrackPosition / fDuration };
		if (fRatio > 0.9f)
		{
			m_pBlackBoard->Get_AI()->Set_ManualMove(false);
		}
	}

	return true;
}

void CBite_Zombie::Exit()
{
	m_pBlackBoard->Get_AI()->Set_ManualMove(false);
	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	m_eStartPoseState = CZombie::POSE_STATE::_END;
}

void CBite_Zombie::Change_Animation_Default_Front(BITE_ANIM_STATE eState)
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_int			iResultAnimationIndex = { -1 };

	//	Change StartAnim
	if (BITE_ANIM_STATE::_END == eState)
	{
		iResultAnimationIndex = static_cast<_int>(ANIM_BITE_DEFAULT_FRONT::_START_F);
		m_eAnimType = BITE_ANIM_TYPE::_DEFAULT_F;

		m_pBlackBoard->Get_AI()->Play_Random_Bite_Sound();
	}

	//	Change MiddleAnim
	else if (BITE_ANIM_STATE::_START == eState)
	{
		if (false == pBodyModel->isFinished(static_cast<_int>(m_ePlayingIndex)))
			return;

		iResultAnimationIndex = static_cast<_int>(ANIM_BITE_DEFAULT_FRONT::_DEFAULT);

	}

	//	Change FinishAnim
	else if (BITE_ANIM_STATE::_MIDDLE == eState)
	{
		if (false == pBodyModel->isFinished(static_cast<_int>(m_ePlayingIndex)))
			return;

		_float			fPlayerHP = { static_cast<_float>(m_pBlackBoard->Get_Player()->Get_Hp()) };
		_float			fZombieAttack = { m_pBlackBoard->Get_AI()->Get_Status_Ptr()->fAttack };
		_bool			isCanKillPlayer = { fPlayerHP <= fZombieAttack };

		if (false == m_pBlackBoard->Hit_Player())
			return;

		if (true == isCanKillPlayer)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_BITE_DEFAULT_FRONT::_KILL_F);
			m_pBlackBoard->Get_AI()->Play_Random_Bite_Kill_Sound();
		}

		else
		{
			_int				iRandRejectAnimIndex = { m_pGameInstance->GetRandom_Int(static_cast<_int>(ANIM_BITE_DEFAULT_FRONT::_REJECT1), static_cast<_int>(ANIM_BITE_DEFAULT_FRONT::_REJECT3)) };
			iResultAnimationIndex = iRandRejectAnimIndex;

			m_eFront_Bite_Type = static_cast<ANIM_BITE_DEFAULT_FRONT>(iRandRejectAnimIndex);

			m_pBlackBoard->Get_AI()->Play_Random_Bite_Reject_Sound();
		}
	}

	_int			iPreAnimIndex = { pBodyModel->Get_CurrentAnimIndex(static_cast<_uint>(m_ePlayingIndex)) };
	if (iPreAnimIndex != iResultAnimationIndex)
		m_pBlackBoard->Get_Player()->Request_NextBiteAnimation(iResultAnimationIndex);

	pBodyModel->Change_Animation(static_cast<_uint>(m_ePlayingIndex), m_strDefaultFrontAnimLayerTag, iResultAnimationIndex);
	pBodyModel->Set_BoneLayer_PlayingInfo(static_cast<_uint>(m_ePlayingIndex), m_strBoneLayerTag);
}

void CBite_Zombie::Change_Animation_Default_Back(BITE_ANIM_STATE eState)
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_int			iResultAnimationIndex = { -1 };

	//	Change StartAnim
	if (BITE_ANIM_STATE::_END == eState)
	{
		iResultAnimationIndex = static_cast<_int>(ANIM_BITE_DEFAULT_BACK::_START_B);
		m_eAnimType = BITE_ANIM_TYPE::_DEFAULT_B;
		m_pBlackBoard->Get_AI()->Play_Random_Bite_Sound();
	}

	//	Change MiddleAnim
	else if (BITE_ANIM_STATE::_START == eState)
	{
		if (false == pBodyModel->isFinished(static_cast<_int>(m_ePlayingIndex)))
			return;

		_float			fPlayerHP = { static_cast<_float>(m_pBlackBoard->Get_Player()->Get_Hp()) };
		_float			fZombieAttack = { m_pBlackBoard->Get_AI()->Get_Status_Ptr()->fAttack };
		_bool			isCanKillPlayer = { fPlayerHP <= fZombieAttack };

		if (false == m_pBlackBoard->Hit_Player())
			return;

		if (true == isCanKillPlayer)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_BITE_DEFAULT_BACK::_KILL_B);
			m_pBlackBoard->Get_AI()->Play_Random_Bite_Kill_Sound();
		}

		else
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_BITE_DEFAULT_BACK::_REJECT_B);
			m_pBlackBoard->Get_AI()->Play_Random_Bite_Reject_Sound();
		}
	}

	_int			iPreAnimIndex = { pBodyModel->Get_CurrentAnimIndex(static_cast<_uint>(m_ePlayingIndex)) };
	if (iPreAnimIndex != iResultAnimationIndex)
		m_pBlackBoard->Get_Player()->Request_NextBiteAnimation(iResultAnimationIndex);

	pBodyModel->Change_Animation(static_cast<_uint>(m_ePlayingIndex), m_strDefaultBackAnimLayerTag, iResultAnimationIndex);
	pBodyModel->Set_BoneLayer_PlayingInfo(static_cast<_uint>(m_ePlayingIndex), m_strBoneLayerTag);
}

void CBite_Zombie::Change_Animation_Creep(BITE_ANIM_STATE eState)
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_int			iResultAnimationIndex = { -1 };

	//	Change StartAnim
	if (BITE_ANIM_STATE::_END == eState)
	{
		_float3			vDirectionFromPlayerLocalFloat3 = {};
		if (false == m_pBlackBoard->Compute_Direction_From_Player_Local(&vDirectionFromPlayerLocalFloat3))
		{
			MSG_BOX(TEXT("Called : void CBite_Zombie::Change_Animation_Creep(BITE_ANIM_STATE eState) 좀비 담당자 호출"));
		}
		wstring				strPreAnimLayerTag = { m_pBlackBoard->Get_Current_AnimLayerTag(CMonster::PART_BODY, m_ePlayingIndex) };
		if (strPreAnimLayerTag != TEXT("Lost_Hold"))
			return;

		_int				iAnimLostHold = { m_pBlackBoard->Get_Current_AnimIndex(CMonster::PART_BODY, m_ePlayingIndex) };
		if (-1 == iAnimLostHold)
			return;

		_bool			isRight = { ANIM_LOST_HOLD::_1 == static_cast<ANIM_LOST_HOLD>(iAnimLostHold) || ANIM_LOST_HOLD::_FACEUP2 == static_cast<ANIM_LOST_HOLD>(iAnimLostHold) };

		if (CZombie::FACE_STATE::_DOWN == m_eStartFaceState)
		{
			if (true == isRight)
				iResultAnimationIndex = static_cast<_int>(ANIM_BITE_CREEP::_FACE_DOWN_R);

			else
				iResultAnimationIndex = static_cast<_int>(ANIM_BITE_CREEP::_FACE_DOWN_L);
		}

		else if (CZombie::FACE_STATE::_UP == m_eStartFaceState)
		{
			if (true == isRight)
				iResultAnimationIndex = static_cast<_int>(ANIM_BITE_CREEP::_FACE_UP_R);
			else
				iResultAnimationIndex = static_cast<_int>(ANIM_BITE_CREEP::_FACE_UP_L);
		}

#ifdef _DEBUG
		else
		{
			MSG_BOX(TEXT("Called : void CBite_Zombie::Change_Animation_Creep(BITE_ANIM_STATE eState) 좀비 담당자 호출"));
		}
#endif
		m_eAnimType = BITE_ANIM_TYPE::_CREEP;
		m_pBlackBoard->Get_AI()->Play_Random_Bite_Creep_Sound();
	}

	//	Change FinishAnim
	else if (BITE_ANIM_STATE::_START == eState)
	{
		if (false == pBodyModel->isFinished(static_cast<_int>(m_ePlayingIndex)))
			return;

		_float			fPlayerHP = { static_cast<_float>(m_pBlackBoard->Get_Player()->Get_Hp()) };
		_float			fZombieAttack = { m_pBlackBoard->Get_AI()->Get_Status_Ptr()->fAttack };
		_bool			isCanKillPlayer = { fPlayerHP <= fZombieAttack };

		if (false == m_pBlackBoard->Hit_Player())
			return;

		_int			iAnimIndex = { pBodyModel->Get_CurrentAnimIndex(static_cast<_uint>(m_ePlayingIndex)) };

#ifdef _DEBUG
		if (-1 == iAnimIndex)
			MSG_BOX(TEXT("Called : void CBite_Zombie::Change_Animation_Creep(BITE_ANIM_STATE eState) 좀비 담당자 호출"));
#endif

		_bool			isRight = { iAnimIndex == static_cast<_int>(ANIM_BITE_CREEP::_FACE_DOWN_R) || iAnimIndex == static_cast<_int>(ANIM_BITE_CREEP::_FACE_UP_R) };
		if (true == isCanKillPlayer)
		{
			if (true == isRight)
				iResultAnimationIndex = static_cast<_int>(ANIM_BITE_CREEP::_CREEP_KILL_R);
			else
				iResultAnimationIndex = static_cast<_int>(ANIM_BITE_CREEP::_CREEP_KILL_L);

			m_pBlackBoard->Get_AI()->Play_Random_Bite_Creep_Kill_Sound();
		}

		else
		{
			if (true == isRight)
				iResultAnimationIndex = static_cast<_int>(ANIM_BITE_CREEP::_CREEP_REJECT_R);
			else
				iResultAnimationIndex = static_cast<_int>(ANIM_BITE_CREEP::_CREEP_REJECT_L);

			m_pBlackBoard->Get_AI()->Play_Random_Bite_Creep_Reject_Sound();
		}
	}

	_int				iPreAnimIndex = { pBodyModel->Get_CurrentAnimIndex(static_cast<_uint>(m_ePlayingIndex)) };
	if (iPreAnimIndex != iResultAnimationIndex)
		m_pBlackBoard->Get_Player()->Request_NextBiteAnimation(iResultAnimationIndex);

	pBodyModel->Change_Animation(static_cast<_uint>(m_ePlayingIndex), m_strCreepAnimLayerTag, iResultAnimationIndex);
	pBodyModel->Set_BoneLayer_PlayingInfo(static_cast<_uint>(m_ePlayingIndex), m_strBoneLayerTag);
}

void CBite_Zombie::Change_Animation_Push_Down(BITE_ANIM_STATE eState)
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_int			iResultAnimationIndex = { -1 };

	//	Change StartAnim
	if (BITE_ANIM_STATE::_END == eState)
	{
		_float3			vDirectionFromPlayerLocalFloat3 = {};
		if (false == m_pBlackBoard->Compute_Direction_From_Player_Local(&vDirectionFromPlayerLocalFloat3))
		{
			MSG_BOX(TEXT("Called : void CBite_Zombie::Change_Animation_Creep(BITE_ANIM_STATE eState) 좀비 담당자 호출"));
		}

		_vector			vDirectionFromPlayerLocal = { XMLoadFloat3(&vDirectionFromPlayerLocalFloat3) };
		_bool			isRight = { XMVectorGetX(vDirectionFromPlayerLocal) > 0.f };
		if (true == isRight)
			iResultAnimationIndex = static_cast<_int>(ANIM_BITE_PUSH_DOWN::_DOWN_START_L);

		else
			iResultAnimationIndex = static_cast<_int>(ANIM_BITE_PUSH_DOWN::_DOWN_START_R);

		m_eAnimType = BITE_ANIM_TYPE::_PUSH_DOWN;
	}

	//	Change MiddleAnim
	else if (BITE_ANIM_STATE::_START == eState)
	{
		if (false == pBodyModel->isFinished(static_cast<_int>(m_ePlayingIndex)))
			return;

		_int			iAnimIndex = { pBodyModel->Get_CurrentAnimIndex(static_cast<_uint>(m_ePlayingIndex)) };
		_bool			isRight = { iAnimIndex == static_cast<_int>(ANIM_BITE_PUSH_DOWN::_DOWN_START_R) };

		if (true == isRight)
		{
			_int			iRandomType = { m_pGameInstance->GetRandom_Int(1, 2) };

			if (1 == iRandomType)
			{
				iResultAnimationIndex = static_cast<_int>(ANIM_BITE_PUSH_DOWN::_PUSH_DOWN_R1);
			}

			else if (2 == iRandomType)
			{
				iResultAnimationIndex = static_cast<_int>(ANIM_BITE_PUSH_DOWN::_PUSH_DOWN_R2);
			}
		}

		else
		{
			_int			iRandomType = { m_pGameInstance->GetRandom_Int(1, 2) };

			if (1 == iRandomType)
			{
				iResultAnimationIndex = static_cast<_int>(ANIM_BITE_PUSH_DOWN::_PUSH_DOWN_L1);
			}

			else if (2 == iRandomType)
			{
				iResultAnimationIndex = static_cast<_int>(ANIM_BITE_PUSH_DOWN::_PUSH_DOWN_L2);
			}
		}
	}

	//	Change FinishAnim
	else if (BITE_ANIM_STATE::_MIDDLE == eState)
	{
		if (false == pBodyModel->isFinished(static_cast<_int>(m_ePlayingIndex)))
			return;

		_float			fPlayerHP = { static_cast<_float>(m_pBlackBoard->Get_Player()->Get_Hp()) };
		_float			fZombieAttack = { m_pBlackBoard->Get_AI()->Get_Status_Ptr()->fAttack };
		_bool			isCanKillPlayer = { fPlayerHP <= fZombieAttack };

		if (false == m_pBlackBoard->Hit_Player())
			return;

		_int			iAnimIndex = { pBodyModel->Get_CurrentAnimIndex(static_cast<_uint>(m_ePlayingIndex)) };

#ifdef _DEBUG
		if (-1 == iAnimIndex)
			MSG_BOX(TEXT("Called : void CBite_Zombie::Change_Animation_Creep(BITE_ANIM_STATE eState) 좀비 담당자 호출"));
#endif

		if (true == isCanKillPlayer)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_BITE_PUSH_DOWN::_DOWN_KILL_R);
		}

		else
		{
			_bool			isRight = { iAnimIndex == static_cast<_int>(ANIM_BITE_PUSH_DOWN::_PUSH_DOWN_R1) || iAnimIndex == static_cast<_int>(ANIM_BITE_PUSH_DOWN::_PUSH_DOWN_R2) };
			if (true == isRight)
				iResultAnimationIndex = static_cast<_int>(ANIM_BITE_PUSH_DOWN::_DOWN_REJECT_R);
			else
				iResultAnimationIndex = static_cast<_int>(ANIM_BITE_PUSH_DOWN::_DOWN_REJECT_L);
		}
	}

	_int				iPreAnimIndex = { pBodyModel->Get_CurrentAnimIndex(static_cast<_uint>(m_ePlayingIndex)) };
	if (iPreAnimIndex != iResultAnimationIndex)
		m_pBlackBoard->Get_Player()->Request_NextBiteAnimation(iResultAnimationIndex);

	pBodyModel->Change_Animation(static_cast<_uint>(m_ePlayingIndex), m_strCreepAnimLayerTag, iResultAnimationIndex);
	pBodyModel->Set_BoneLayer_PlayingInfo(static_cast<_uint>(m_ePlayingIndex), m_strBoneLayerTag);
}

void CBite_Zombie::Change_Animation_Lost(BITE_ANIM_STATE eState)
{
}

CBite_Zombie::BITE_ANIM_STATE CBite_Zombie::Compute_Current_AnimState_Bite()
{
	BITE_ANIM_STATE			eAnimState = { BITE_ANIM_STATE::_END };
	if (nullptr == m_pBlackBoard)
		return eAnimState;

	CModel* pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBody_Model)
		return eAnimState;

	_bool			isStartAnim = { Is_CurrentAnim_StartAnim() };
	_bool			isMiddleAnim = { Is_CurrentAnim_MiddleAnim() };
	_bool			isFinishAnim = { Is_CurrentAnim_FinishAnim() };

	_bool			isNonIncludedAnim = { !isStartAnim && !isMiddleAnim && !isFinishAnim };

	if (true == isNonIncludedAnim)
		return eAnimState;

	_bool			isFinished = { pBody_Model->isFinished(static_cast<_uint>(m_ePlayingIndex)) };
	_bool			isCanChange = { isFinishAnim && !isNonIncludedAnim };

	if (true == isStartAnim)
	{
		eAnimState = BITE_ANIM_STATE::_START;
	}

	else if (true == isMiddleAnim)
	{
		eAnimState = BITE_ANIM_STATE::_MIDDLE;
	}

	else if (true == isFinishAnim)
	{
		eAnimState = BITE_ANIM_STATE::_FINISH;
	}

	return eAnimState;
}

_bool CBite_Zombie::Is_CurrentAnim_StartAnim()
{
	_bool				isStartAnim = { false };
	if (nullptr == m_pBlackBoard)
		return isStartAnim;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBodyModel)
		return isStartAnim;

	wstring				strCurrentAnimLayerTag = { pBodyModel->Get_CurrentAnimLayerTag(static_cast<_uint>(m_ePlayingIndex)) };
	_int				iCurrentAnimIndex = { pBodyModel->Get_CurrentAnimIndex(static_cast<_uint>(m_ePlayingIndex)) };

	unordered_map<wstring, unordered_set<_uint>>::iterator			iterLayerTag = { m_StartAnims.find(strCurrentAnimLayerTag) };
	if (iterLayerTag == m_StartAnims.end())
		return isStartAnim;

	unordered_set<_uint >::iterator									iterIndex = { m_StartAnims[strCurrentAnimLayerTag].find(iCurrentAnimIndex) };
	if (iterIndex == m_StartAnims[strCurrentAnimLayerTag].end())
		return isStartAnim;

	isStartAnim = true;
	return isStartAnim;
}

_bool CBite_Zombie::Is_CurrentAnim_MiddleAnim()
{
	_bool				isMiddleAnim = { false };
	if (nullptr == m_pBlackBoard)
		return isMiddleAnim;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBodyModel)
		return isMiddleAnim;

	wstring				strCurrentAnimLayerTag = { pBodyModel->Get_CurrentAnimLayerTag(static_cast<_uint>(m_ePlayingIndex)) };
	_int				iCurrentAnimIndex = { pBodyModel->Get_CurrentAnimIndex(static_cast<_uint>(m_ePlayingIndex)) };

	unordered_map<wstring, unordered_set<_uint>>::iterator			iterLayerTag = { m_MiddleAnims.find(strCurrentAnimLayerTag) };
	if (iterLayerTag == m_MiddleAnims.end())
		return isMiddleAnim;

	unordered_set<_uint >::iterator									iterIndex = { m_MiddleAnims[strCurrentAnimLayerTag].find(iCurrentAnimIndex) };
	if (iterIndex == m_MiddleAnims[strCurrentAnimLayerTag].end())
		return isMiddleAnim;

	isMiddleAnim = true;
	return isMiddleAnim;
}

_bool CBite_Zombie::Is_CurrentAnim_FinishAnim()
{
	_bool				isFInishAnim = { false };
	if (nullptr == m_pBlackBoard)
		return isFInishAnim;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBodyModel)
		return isFInishAnim;

	wstring				strCurrentAnimLayerTag = { pBodyModel->Get_CurrentAnimLayerTag(static_cast<_uint>(m_ePlayingIndex)) };
	_int				iCurrentAnimIndex = { pBodyModel->Get_CurrentAnimIndex(static_cast<_uint>(m_ePlayingIndex)) };

	unordered_map<wstring, unordered_set<_uint>>::iterator			iterLayerTag = { m_FinishAnims.find(strCurrentAnimLayerTag) };
	if (iterLayerTag == m_FinishAnims.end())
		return isFInishAnim;

	unordered_set<_uint >::iterator									iterIndex = { m_FinishAnims[strCurrentAnimLayerTag].find(iCurrentAnimIndex) };
	if (iterIndex == m_FinishAnims[strCurrentAnimLayerTag].end())
		return isFInishAnim;

	isFInishAnim = true;
	return isFInishAnim;
}

_bool CBite_Zombie::Is_Can_Start_Bite()
{
	_float				fDistanceToPlayer = { 0.f };
	if (false == m_pBlackBoard->Compute_Distance_To_Player(&fDistanceToPlayer))
		return false;

	MONSTER_STATE					eMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };
	CMonster::MONSTER_STATUS*		pMonster_Status = { m_pBlackBoard->Get_ZombieStatus_Ptr() };
	CPlayer::STATE					ePlayerState = { static_cast<CPlayer::STATE>(m_pBlackBoard->Get_Player()->Get_Player_State()) };

	if (CPlayer::STATE::BITE == ePlayerState)
		return false;
	
	if (MONSTER_STATE::MST_HOLD != eMonsterState)
		return false;

	if (nullptr == pMonster_Status)
		return false;

	if (false == pMonster_Status->fBiteRange >= fDistanceToPlayer)
		return false;	

	return true;
}

void CBite_Zombie::Change_Animation(BITE_ANIM_STATE eState)
{
	if (CZombie::POSE_STATE::_CREEP == m_eStartPoseState)
	{
		Change_Animation_Creep(eState);
	}

	/*
	else if ()
		Change_Animation_Lost();
	*/

	else
	{
		/*DIRECTION		eDirection = { DIRECTION::_END };
		if (m_eAnimType == BITE_ANIM_TYPE::_END)
		{
			_float3			vDirectionFromPlayerLocal;
			m_pBlackBoard->Compute_Direction_From_Player_Local(&vDirectionFromPlayerLocal);

			_bool			isPlayersFront = { vDirectionFromPlayerLocal.z > 0.f };
			eDirection = { isPlayersFront ? DIRECTION::_F : DIRECTION::_B };
		}

		else
		{
			if (BITE_ANIM_TYPE::_DEFAULT_F == m_eAnimType)
			{
				eDirection = { DIRECTION::_F };
			}

			else
			{
				eDirection = { DIRECTION::_B };
			}
		}

		if (eDirection == DIRECTION::_F)
		{
			Change_Animation_Default_Front(eState);
		}

		else
		{
			Change_Animation_Default_Back(eState);
		}*/

		if (true == m_isFrontFromPlayer)
		{

			Change_Animation_Default_Front(eState);
		}
		else
			Change_Animation_Default_Back(eState);

	}

}

void CBite_Zombie::Set_Bite_LinearStart_HalfMatrix()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pZombieBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	CModel* pPlayerBody_Model = { m_pBlackBoard->Get_Player()->Get_Body_Model() };
	if (nullptr == pZombieBody_Model || nullptr == pPlayerBody_Model)
		return;

	_int					iAnimIndex = { pZombieBody_Model->Get_CurrentAnimIndex(static_cast<_uint>(m_ePlayingIndex)) };
	wstring					strAnimLayerTag = { pZombieBody_Model->Get_CurrentAnimLayerTag(static_cast<_uint>(m_ePlayingIndex)) };

	_float4x4				ResultMatrixFloat4x4;
	//	if (false == m_pBlackBoard->Compute_HalfMatrix_Current_BiteAnim(strAnimLayerTag, iAnimIndex, &ResultMatrixFloat4x4))
	//		return;

	if (CZombie::POSE_STATE::_UP == m_eStartPoseState)
		XMStoreFloat4x4(&ResultMatrixFloat4x4, m_pBlackBoard->Get_Player()->Get_Transform()->Get_WorldMatrix());
	else if (CZombie::POSE_STATE::_CREEP == m_eStartPoseState)
		XMStoreFloat4x4(&ResultMatrixFloat4x4, m_pBlackBoard->Get_AI()->Get_Transform()->Get_WorldMatrix());


	_matrix					PlayerWorldMatrix = { m_pBlackBoard->Get_Player()->Get_Transform()->Get_WorldMatrix() };
	_matrix					ZombieWorldMatrix = { m_pBlackBoard->Get_AI()->Get_Transform()->Get_WorldMatrix() };

	_vector					vPlayerWorldScale, vPlayerWorldQuaternion, vPlayerWorldTranslation;
	_vector					vZombieWorldScale, vZombieWorldQuaternion, vZombieWorldTranslation;
	XMMatrixDecompose(&vPlayerWorldScale, &vPlayerWorldQuaternion, &vPlayerWorldTranslation, PlayerWorldMatrix);
	XMMatrixDecompose(&vZombieWorldScale, &vZombieWorldQuaternion, &vZombieWorldTranslation, ZombieWorldMatrix);

	//	하프 매트릭스를 분해
	_matrix					ResultMatrix = { XMLoadFloat4x4(&ResultMatrixFloat4x4) };
	_vector					vResultScale, vResultQuaternion, vResultTranslation;
	XMMatrixDecompose(&vResultScale, &vResultQuaternion, &vResultTranslation, ResultMatrix);

	//	분해한 하프매트릭스를 각 플레이어와 좀비에게 각자의 스케일을 적용
	_matrix					PlayerHalfMatrix = { XMMatrixAffineTransformation(vPlayerWorldScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vResultQuaternion, vResultTranslation) };
	_matrix					ZombieHalfMatrix = { XMMatrixAffineTransformation(vZombieWorldScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vResultQuaternion, vResultTranslation) };

	////	루트본에 대한 해당 애니메이션의 첫 키프레임의 트랜스폼 매트릭스를 가져옴
	_matrix					RootFirstKeyFramePlayer = { pPlayerBody_Model->Get_FirstKeyFrame_Root_TransformationMatrix(strAnimLayerTag, iAnimIndex) };
	_matrix					RootFirstKeyFrameZombie = { pZombieBody_Model->Get_FirstKeyFrame_Root_TransformationMatrix(strAnimLayerTag, iAnimIndex) };
	_matrix					ModelTransformPlayer = { XMLoadFloat4x4(&pPlayerBody_Model->Get_TransformationMatrix()) };
	_matrix					ModelTransformZombie = { XMLoadFloat4x4(&pZombieBody_Model->Get_TransformationMatrix()) };

	_vector					vPlayerRootScale, vPlayerRootQuaternion, vPlayerRootTranslation;
	_vector					vZombieRootScale, vZombieRootQuaternion, vZombieRootTranslation;

	XMMatrixDecompose(&vPlayerRootScale, &vPlayerRootQuaternion, &vPlayerRootTranslation, RootFirstKeyFramePlayer);
	XMMatrixDecompose(&vZombieRootScale, &vZombieRootQuaternion, &vZombieRootTranslation, RootFirstKeyFrameZombie);

	vPlayerRootTranslation = XMVector3TransformNormal(vPlayerRootTranslation, ModelTransformPlayer);
	vZombieRootTranslation = XMVector3TransformNormal(vZombieRootTranslation, ModelTransformZombie);

	_vector					vRootRotateAxisPlayer = { XMVectorSetW(vPlayerRootQuaternion, 0.f) };
	_vector					vRootRotateAxisZombie = { XMVectorSetW(vZombieRootQuaternion, 0.f) };

	vRootRotateAxisPlayer = XMVector3TransformNormal(vRootRotateAxisPlayer, ModelTransformPlayer);
	vRootRotateAxisZombie = XMVector3TransformNormal(vRootRotateAxisZombie, ModelTransformZombie);

	vPlayerRootQuaternion = XMVectorSetW(vRootRotateAxisPlayer, XMVectorGetW(vPlayerRootQuaternion));
	vZombieRootQuaternion = XMVectorSetW(vRootRotateAxisZombie, XMVectorGetW(vZombieRootQuaternion));

	RootFirstKeyFramePlayer = XMMatrixAffineTransformation(vPlayerRootScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vPlayerRootQuaternion, vPlayerRootTranslation);
	RootFirstKeyFrameZombie = XMMatrixAffineTransformation(vZombieRootScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vZombieRootQuaternion, vZombieRootTranslation);

	PlayerHalfMatrix = RootFirstKeyFramePlayer * PlayerHalfMatrix;
	ZombieHalfMatrix = RootFirstKeyFrameZombie * ZombieHalfMatrix;


	_vector					vZombieHalfScale, vZombieHalfQuaternion, vZombieHalfTranslation;
	XMMatrixDecompose(&vZombieHalfScale, &vZombieHalfQuaternion, &vZombieHalfTranslation, ZombieHalfMatrix);
	_vector					vZombieResultDeltaQuaternion = { XMQuaternionMultiply(XMQuaternionInverse(vZombieWorldQuaternion), XMQuaternionNormalize(vZombieHalfQuaternion)) };
	_vector					vZombieResultDeltaTranslation = { vZombieHalfTranslation - vZombieWorldTranslation };
	_matrix					ZombieDeltaMatrix = { XMMatrixAffineTransformation(vZombieWorldScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vZombieResultDeltaQuaternion, vZombieResultDeltaTranslation) };

	_vector					vPlayerHalfScale, vPlayerHalfQuaternion, vPlayerHalfTranslation;
	XMMatrixDecompose(&vPlayerHalfScale, &vPlayerHalfQuaternion, &vPlayerHalfTranslation, PlayerHalfMatrix);
	_vector					vPlayerResultDeltaQuaternion = { XMQuaternionMultiply(XMQuaternionInverse(vPlayerWorldQuaternion), XMQuaternionNormalize(vPlayerHalfQuaternion)) };
	_vector					vPlayerResultDeltaTranslation = { vPlayerHalfTranslation - vPlayerWorldTranslation };
	_matrix					PlayerDeltaMatrix = { XMMatrixAffineTransformation(vPlayerWorldScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vPlayerResultDeltaQuaternion, vPlayerResultDeltaTranslation) };

	XMStoreFloat4x4(&m_Delta_Matrix_To_HalfMatrix, ZombieDeltaMatrix);
	m_pBlackBoard->Get_Player()->Change_Player_State_Bite(iAnimIndex, strAnimLayerTag, PlayerDeltaMatrix, m_fTotalLinearTime_HalfMatrix);
}

_bool CBite_Zombie::Is_StateFinished(BITE_ANIM_STATE eState)
{
	_bool					isFinished = { false };

	if (BITE_ANIM_STATE::_FINISH == eState)
	{
		CModel* pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
		if (nullptr == pBody_Model)
			return isFinished;

		isFinished = pBody_Model->isFinished(static_cast<_uint>(m_ePlayingIndex));
	}

	return isFinished;
}

void CBite_Zombie::Update_PoseState_FaceState()
{
	if (true == Is_CurrentAnim_FinishAnim())
	{
		if (nullptr == m_pBlackBoard)
			return;

		CModel* pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
		if (nullptr == pBody_Model)
			return;

		_int			iAnimIndex = { pBody_Model->Get_CurrentAnimIndex(static_cast<_uint>(m_ePlayingIndex)) };
		wstring			strAnimLayerTag = { pBody_Model->Get_CurrentAnimLayerTag(static_cast<_uint>(m_ePlayingIndex)) };

		if (-1 == iAnimIndex || TEXT("") == strAnimLayerTag)
			return;

		CZombie::FACE_STATE			eFaceState = { CZombie::FACE_STATE::_END };
		CZombie::POSE_STATE			ePoseState = { CZombie::POSE_STATE::_END };
		if (strAnimLayerTag == m_strDefaultFrontAnimLayerTag)
		{
			if (iAnimIndex == static_cast<_int>(ANIM_BITE_DEFAULT_FRONT::_KILL_F) ||
				iAnimIndex == static_cast<_int>(ANIM_BITE_DEFAULT_FRONT::_REJECT1) ||
				iAnimIndex == static_cast<_int>(ANIM_BITE_DEFAULT_FRONT::_REJECT2) ||
				iAnimIndex == static_cast<_int>(ANIM_BITE_DEFAULT_FRONT::_REJECT3))
			{
				eFaceState = CZombie::FACE_STATE::_UP;
				ePoseState = CZombie::POSE_STATE::_CREEP;
			}
			else
				return;
		}

		else if (strAnimLayerTag == m_strDefaultBackAnimLayerTag)
		{
			if (iAnimIndex == static_cast<_int>(ANIM_BITE_DEFAULT_BACK::_KILL_B) ||
				iAnimIndex == static_cast<_int>(ANIM_BITE_DEFAULT_BACK::_REJECT_B))
			{
				eFaceState = CZombie::FACE_STATE::_UP;
				ePoseState = CZombie::POSE_STATE::_CREEP;
			}
			else
				return;
		}

		else if (strAnimLayerTag == m_strCreepAnimLayerTag)
		{
			if (iAnimIndex == static_cast<_int>(ANIM_BITE_CREEP::_CREEP_KILL_L) ||
				iAnimIndex == static_cast<_int>(ANIM_BITE_CREEP::_CREEP_KILL_R) ||
				iAnimIndex == static_cast<_int>(ANIM_BITE_CREEP::_CREEP_REJECT_L) ||
				iAnimIndex == static_cast<_int>(ANIM_BITE_CREEP::_CREEP_REJECT_R))
			{
				eFaceState = CZombie::FACE_STATE::_DOWN;
				ePoseState = CZombie::POSE_STATE::_CREEP;
			}
		}

		else if (strAnimLayerTag == m_strETCAnimLayerTag)
		{
			//		TODO:			ETC 애님 추가
		}

		if (CZombie::FACE_STATE::_END != eFaceState)
			m_pBlackBoard->Get_AI()->Set_FaceState(eFaceState);

		if (CZombie::POSE_STATE::_END != ePoseState)
			m_pBlackBoard->Get_AI()->Set_PoseState(ePoseState);
	}
}

void CBite_Zombie::Apply_HalfMatrix(_float fTimeDelta)
{
	if (m_fAccLinearTime_HalfMatrix >= m_fTotalLinearTime_HalfMatrix)
		return;

	if (nullptr == m_pBlackBoard)
		return;

	CTransform* pAITransform = { m_pBlackBoard->Get_AI()->Get_Transform() };
	if (nullptr == pAITransform)
		return;

	m_fAccLinearTime_HalfMatrix += fTimeDelta;
	if (m_fAccLinearTime_HalfMatrix > m_fTotalLinearTime_HalfMatrix)
	{
		fTimeDelta += m_fTotalLinearTime_HalfMatrix - m_fAccLinearTime_HalfMatrix;
		m_fAccLinearTime_HalfMatrix = m_fTotalLinearTime_HalfMatrix;
	}

	_float				fRatio = { fTimeDelta / m_fTotalLinearTime_HalfMatrix };

	_vector				vScale, vTranslation, vQuaternion;

	XMMatrixDecompose(&vScale, &vQuaternion, &vTranslation, XMLoadFloat4x4(&m_Delta_Matrix_To_HalfMatrix));

	_vector				vCurrentTranslation = { vTranslation * fRatio };
	_vector				vCurrentQuaternion = { XMQuaternionSlerp(XMQuaternionIdentity(), XMQuaternionNormalize(vQuaternion), fRatio) };

	//	_vector				vCurrentQuaternionInv = { XMQuaternionInverse(XMQuaternionNormalize(vCurrentQuaternion)) };
	//	_matrix				vCurrentRotationInverse = { XMMatrixRotationQuaternion(vCurrentQuaternionInv) };
	//	vCurrentTranslation = XMVector3TransformNormal(vCurrentTranslation, vCurrentRotationInverse);

	_matrix				AIWorldMatrix = { pAITransform->Get_WorldMatrix() };

	_vector				vWorldScale, vWorldQuaternion, vWorldTranslation;
	XMMatrixDecompose(&vWorldScale, &vWorldQuaternion, &vWorldTranslation, AIWorldMatrix);

	_vector				vResultQuaternion = { XMQuaternionMultiply(XMQuaternionNormalize(vWorldQuaternion), XMQuaternionNormalize(vCurrentQuaternion)) };
	_vector				vResultTranslation = { XMVectorSetW(vWorldTranslation + vCurrentTranslation, 1.f) };

	_matrix				AplliedMatrix = { XMMatrixAffineTransformation(vWorldScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vResultQuaternion, vResultTranslation) };
	m_pBlackBoard->Get_AI()->Get_Transform()->Set_WorldMatrix(AplliedMatrix);
}

HRESULT CBite_Zombie::SetUp_AnimBranches()
{
	/* For.Start Anims */
	m_StartAnims[m_strDefaultFrontAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_DEFAULT_FRONT::_START_F));
	m_StartAnims[m_strDefaultFrontAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_DEFAULT_FRONT::_CREEP_START));

	m_StartAnims[m_strDefaultBackAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_DEFAULT_BACK::_START_B));

	m_StartAnims[m_strCreepAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_CREEP::_FACE_UP_L));
	m_StartAnims[m_strCreepAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_CREEP::_FACE_UP_R));
	m_StartAnims[m_strCreepAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_CREEP::_FACE_DOWN_L));
	m_StartAnims[m_strCreepAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_CREEP::_FACE_DOWN_R));

	/* For.Middle Anims */
	m_MiddleAnims[m_strDefaultFrontAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_DEFAULT_FRONT::_DEFAULT));

	/* For.Finish Anims */
	m_FinishAnims[m_strDefaultFrontAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_DEFAULT_FRONT::_REJECT1));
	m_FinishAnims[m_strDefaultFrontAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_DEFAULT_FRONT::_REJECT2));
	m_FinishAnims[m_strDefaultFrontAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_DEFAULT_FRONT::_REJECT3));
	m_FinishAnims[m_strDefaultFrontAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_DEFAULT_FRONT::_KILL_F));

	m_FinishAnims[m_strDefaultBackAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_DEFAULT_BACK::_REJECT_B));
	m_FinishAnims[m_strDefaultBackAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_DEFAULT_BACK::_KILL_B));

	m_FinishAnims[m_strCreepAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_CREEP::_CREEP_REJECT_L));
	m_FinishAnims[m_strCreepAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_CREEP::_CREEP_REJECT_R));
	m_FinishAnims[m_strCreepAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_CREEP::_CREEP_KILL_L));
	m_FinishAnims[m_strCreepAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_CREEP::_CREEP_KILL_R));

	return S_OK;
}

void CBite_Zombie::Initiate_Effect()
{
	switch (m_eStartPoseState)
	{
	case CZombie::POSE_STATE::_UP:
	{
		if(m_eAnimType == BITE_ANIM_TYPE::_DEFAULT_F)
		{
			CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
			if (nullptr == pBodyModel)
				return;

			if (Is_CurrentAnim_MiddleAnim())
			{
				_float fTrackPosition = pBodyModel->Get_TrackPosition(static_cast<_uint>(m_ePlayingIndex));

				if (abs(fTrackPosition - 40.f) < 1.3f)
				{
					m_pBlackBoard->Get_AI()->ResetBiteEffect();
					m_pBlackBoard->Get_Player()->SetCalcDecalMap(true);
				}
			}

			if (Is_CurrentAnim_FinishAnim())
			{
				auto eBiteType = static_cast<ANIM_BITE_DEFAULT_FRONT>(m_eFront_Bite_Type);
				switch (eBiteType)
				{
				case ANIM_BITE_DEFAULT_FRONT::_REJECT1:
				{
					_float fTrackPosition = pBodyModel->Get_TrackPosition(static_cast<_uint>(m_ePlayingIndex));

					if (abs(fTrackPosition - 100.f) < 1.3f)
					{
						const wchar_t* str = L"Break_Drop_";
						wchar_t result[32];
						_int inum = m_pGameInstance->GetRandom_Int(10, 12);

						std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

						m_pGameInstance->Change_Sound_3D(m_pBlackBoard->Get_AI()->Get_Transform(), result, (_uint)ZOMBIE_SOUND_CH::_BITE_DROP);

						if (inum == 11)
						{
							m_pGameInstance->Set_Volume_3D(m_pBlackBoard->Get_AI()->Get_Transform(), (_uint)ZOMBIE_SOUND_CH::_BITE_DROP, 0.4f);
						}
					}
					break;
				}
				case ANIM_BITE_DEFAULT_FRONT::_REJECT2:
				{
					_float fTrackPosition = pBodyModel->Get_TrackPosition(static_cast<_uint>(m_ePlayingIndex));

					if (abs(fTrackPosition - 100.f) < 1.3f)
					{
						const wchar_t* str = L"Break_Drop_";
						wchar_t result[32];
						_int inum = m_pGameInstance->GetRandom_Int(10, 12);

						std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

						m_pGameInstance->Change_Sound_3D(m_pBlackBoard->Get_AI()->Get_Transform(), result, (_uint)ZOMBIE_SOUND_CH::_BITE_DROP);

						if (inum == 11)
						{
							m_pGameInstance->Set_Volume_3D(m_pBlackBoard->Get_AI()->Get_Transform(), (_uint)ZOMBIE_SOUND_CH::_BITE_DROP, 0.4f);
						}
					}
					break;
				}
				case ANIM_BITE_DEFAULT_FRONT::_REJECT3:
				{
					_float fTrackPosition = pBodyModel->Get_TrackPosition(static_cast<_uint>(m_ePlayingIndex));

					if (abs(fTrackPosition - 70.f) < 1.3f)
					{
						const wchar_t* str = L"Break_Drop_";
						wchar_t result[32];
						_int inum = m_pGameInstance->GetRandom_Int(10, 12);

						std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

						m_pGameInstance->Change_Sound_3D(m_pBlackBoard->Get_AI()->Get_Transform(), result, (_uint)ZOMBIE_SOUND_CH::_BITE_DROP);

						if (inum == 11)
						{
							m_pGameInstance->Set_Volume_3D(m_pBlackBoard->Get_AI()->Get_Transform(), (_uint)ZOMBIE_SOUND_CH::_BITE_DROP, 0.4f);
						}
					}
					break;
				}
				}
			}

			m_pBlackBoard->Get_AI()->SetBiteBlood();
			m_pBlackBoard->Get_Player()->SetBiteType(BITE_TYPE_FOR_EFFECT::STAND_FRONT);
		}
		else
		{
			CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
			if (nullptr == pBodyModel)
				return;

			if (Is_CurrentAnim_StartAnim())
			{
				_float fTrackPosition = pBodyModel->Get_TrackPosition(static_cast<_uint>(m_ePlayingIndex));

				if (abs(fTrackPosition - 40.f) < 1.3f)
				{
					m_pBlackBoard->Get_AI()->ResetBiteEffect();
					m_pBlackBoard->Get_Player()->SetCalcDecalMap(true);
				}
			}

			if (Is_CurrentAnim_FinishAnim())
			{
				_float fTrackPosition = pBodyModel->Get_TrackPosition(static_cast<_uint>(m_ePlayingIndex));

				if (abs(fTrackPosition - 90.f) < 1.3f)
				{
					const wchar_t* str = L"Break_Drop_";
					wchar_t result[32];
					_int inum = m_pGameInstance->GetRandom_Int(10, 12);

					std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

					m_pGameInstance->Change_Sound_3D(m_pBlackBoard->Get_AI()->Get_Transform(), result, (_uint)ZOMBIE_SOUND_CH::_BITE_DROP);

					if (inum == 11)
					{
						m_pGameInstance->Set_Volume_3D(m_pBlackBoard->Get_AI()->Get_Transform(), (_uint)ZOMBIE_SOUND_CH::_BITE_DROP, 0.2f);
					}
				}
			}

			m_pBlackBoard->Get_AI()->SetBiteBlood();
			m_pBlackBoard->Get_Player()->SetBiteType(BITE_TYPE_FOR_EFFECT::STAND_BACK);
		}

		break;
	}
	case CZombie::POSE_STATE::_CREEP:
	{
		switch (m_eStartFaceState)
		{
		case CZombie::FACE_STATE::_UP:
		{
			CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
			if (nullptr == pBodyModel)
				return;

			if (Is_CurrentAnim_StartAnim())
			{
				_float fTrackPosition = pBodyModel->Get_TrackPosition(static_cast<_uint>(m_ePlayingIndex));

				if (abs(fTrackPosition - 40.f) < 1.3f)
				{
					m_pBlackBoard->Get_AI()->ResetBiteEffect();
				}
			}

			if (Is_CurrentAnim_FinishAnim())
			{
				_float fTrackPosition = pBodyModel->Get_TrackPosition(static_cast<_uint>(m_ePlayingIndex));

				if (abs(fTrackPosition - 100.f) < 1.3f)
				{
					m_pBlackBoard->Get_AI()->ResetBiteEffect();
				}

				if (abs(fTrackPosition - 140.f) < 1.3f)
				{
					const wchar_t* str = L"Break_Drop_";
					wchar_t result[32];
					_int inum = m_pGameInstance->GetRandom_Int(10, 12);

					std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

					m_pGameInstance->Change_Sound_3D(m_pBlackBoard->Get_AI()->Get_Transform(), result, (_uint)ZOMBIE_SOUND_CH::_BITE_DROP);

					if (inum == 11)
					{
						m_pGameInstance->Set_Volume_3D(m_pBlackBoard->Get_AI()->Get_Transform(), (_uint)ZOMBIE_SOUND_CH::_BITE_DROP, 0.2f);
					}
				}
			}

			m_pBlackBoard->Get_AI()->SetBiteBlood();
			break;
		}
		case CZombie::FACE_STATE::_DOWN:
		{
			CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
			if (nullptr == pBodyModel)
				return;

			if (Is_CurrentAnim_StartAnim())
			{
				_float fTrackPosition = pBodyModel->Get_TrackPosition(static_cast<_uint>(m_ePlayingIndex));

				if (abs(fTrackPosition - 40.f) < 1.3f)
				{
					m_pBlackBoard->Get_AI()->ResetBiteEffect();
				}
			}

			if (Is_CurrentAnim_FinishAnim())
			{
				_float fTrackPosition = pBodyModel->Get_TrackPosition(static_cast<_uint>(m_ePlayingIndex));

				if (abs(fTrackPosition - 100.f) < 1.3f)
				{
					m_pBlackBoard->Get_AI()->ResetBiteEffect();
				}

				if (abs(fTrackPosition - 140.f) < 1.3f)
				{
					const wchar_t* str = L"Break_Drop_";
					wchar_t result[32];
					_int inum = m_pGameInstance->GetRandom_Int(10, 12);

					std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

					m_pGameInstance->Change_Sound_3D(m_pBlackBoard->Get_AI()->Get_Transform(), result, (_uint)ZOMBIE_SOUND_CH::_BITE_DROP);

					if (inum == 11)
					{
						m_pGameInstance->Set_Volume_3D(m_pBlackBoard->Get_AI()->Get_Transform(), (_uint)ZOMBIE_SOUND_CH::_BITE_DROP, 0.2f);
					}
				}
			}

			m_pBlackBoard->Get_AI()->SetBiteBlood();

			break;
		}
		}
		break;
	}
	}
}

CBite_Zombie* CBite_Zombie::Create(void* pArg)
{
	CBite_Zombie* pInstance = { new CBite_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBite_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBite_Zombie::Free()
{
	__super::Free();
}