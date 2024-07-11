#include "stdafx.h"
#include "GameInstance.h"
#include "Lightly_Hold_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CLightly_Hold_Zombie::CLightly_Hold_Zombie()
	: CTask_Node()
{
}

CLightly_Hold_Zombie::CLightly_Hold_Zombie(const CLightly_Hold_Zombie& rhs)
{
}

HRESULT CLightly_Hold_Zombie::Initialize(void* pArg)
{
	if (FAILED(SetUp_AnimBranches()))
		return E_FAIL;

	m_fTotalLinearTime_HalfMatrix = 1.f;

	return S_OK;
}

void CLightly_Hold_Zombie::Enter()
{
	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	pBodyModel->Set_TotalLinearInterpolation(0.2f);
	pBodyModel->Set_Loop(static_cast<_uint>(PLAYING_INDEX::INDEX_0), false);

	pBodyModel->Set_BlendWeight(static_cast<_uint>(PLAYING_INDEX::INDEX_1), 0.f, 0.f);
	pBodyModel->Reset_PreAnim_CurrentAnim(static_cast<_uint>(PLAYING_INDEX::INDEX_1));

	m_pBlackBoard->Get_AI()->Set_ManualMove(true);

	m_isEntry = true;
	m_isHoldTarget = false;

	m_isSendMSG_To_Player = false;
	m_eAnimType = LIGHTLY_HOLD_ANIM_TYPE::_END;

	_float3			vDirectionToPlayerLocalFloat3 = {};
	if (false == m_pBlackBoard->Compute_Direction_To_Player_Local(&vDirectionToPlayerLocalFloat3))
		return;

	//	좀비의 오른쪽일떄 => 왼쪽으로 덮침
	m_eDirectionToPlayer = vDirectionToPlayerLocalFloat3.x > 0.f ? DIRECTION::_L : DIRECTION::_R;
	m_fAccLinearTime_HalfMatrix = 0.f;



#ifdef _DEBUG

	cout << "Enter Lightly Hold " << endl;

#endif 
}

_bool CLightly_Hold_Zombie::Execute(_float fTimeDelta)
{
	return false;

#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	if (false == Condition_Check())
	{
		m_pBlackBoard->Get_Player()->Request_NextBiteAnimation(-1);
		return false;
	}

	m_eAnimState = { Compute_Current_AnimState_Lightly_Hold() };
	_bool							isEntry = { m_eAnimState == LIGHTLY_HOLD_ANIM_STATE::_END };

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	if (false == m_isHoldTarget)
		pAI->Set_State(MONSTER_STATE::MST_LIGHTLY_HOLD);
	else
		pAI->Set_State(MONSTER_STATE::MST_BITE);

	Change_Animation();

	if (false == m_isSendMSG_To_Player)
	{
		Set_Lightly_Hold_LinearStart_HalfMatrix();
		m_isSendMSG_To_Player = true;
	}

	Apply_HalfMatrix(fTimeDelta);

	CModel*			pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBody_Model)
		return false;

	if (LIGHTLY_HOLD_ANIM_STATE::_FINISH == m_eAnimState)
	{
		MONSTER_STATE			eMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };
		_bool					isEndAnimState = { false };

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

void CLightly_Hold_Zombie::Exit()
{
}

_bool CLightly_Hold_Zombie::Condition_Check()
{
	CZombie*					pAI = { m_pBlackBoard->Get_AI() };
	if (nullptr == pAI)
		return false;

	//	Finish Check
	MONSTER_STATE				eMonster_State = { pAI->Get_Current_MonsterState() };
	if (MONSTER_STATE::MST_LIGHTLY_HOLD == eMonster_State ||
		MONSTER_STATE::MST_BITE == eMonster_State)
	{
		if (MONSTER_STATE::MST_BITE == eMonster_State)
		{
			CModel*				pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
			if (nullptr == pBody_Model)
				return false;

			if (true == pBody_Model->isFinished(static_cast<_uint>(m_ePlayingIndex) &
				LIGHTLY_HOLD_ANIM_STATE::_FINISH == m_eAnimState))
				return false;
		}

		else if(MONSTER_STATE::MST_LIGHTLY_HOLD == eMonster_State)
		{
			if (false == m_isHoldTarget)
			{
				_float				fDistanceToPlayer = {};
				if (false == m_pBlackBoard->Compute_Distance_To_Player_World(&fDistanceToPlayer))
					return false;
				_bool				isCanBite = { pAI->Get_Status_Ptr()->fBiteRange >= fDistanceToPlayer };
				if (true == isCanBite)
				{
					//	m_eAnimType = LIGHTLY_HOLD_ANIM_TYPE::_PUSH_DOWN;
					//	m_isHoldTarget = true;
					//	m_eAnimState = LIGHTLY_HOLD_ANIM_STATE::_START;
				}
			}			

			CModel* pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
			if (nullptr == pBody_Model)
				return false;

			if (true == pBody_Model->isFinished(static_cast<_uint>(m_ePlayingIndex) &
				LIGHTLY_HOLD_ANIM_STATE::_FINISH == m_eAnimState))
				return false;
		}
	}

	//	Entry Check
	else
	{
		//	Can Link Pre Pose State
		CZombie::POSE_STATE			ePoseState = { pAI->Get_PoseState() };
		if (CZombie::POSE_STATE::_UP != ePoseState)
			return false;

		//	Can Link Pre Monster State
		MONSTER_STATE				eMonsterState = { pAI->Get_Current_MonsterState() };
		if (MONSTER_STATE::MST_IDLE != eMonsterState &&
			MONSTER_STATE::MST_WALK != eMonsterState)
			return false;

		_float				fDistanceToPlayer = { 0.f };
		if (false == m_pBlackBoard->Compute_Distance_To_Player(&fDistanceToPlayer))
		{
			return false;
		}

		if (1.5f < fDistanceToPlayer)
			return false;

	}	

	return true;
}

void CLightly_Hold_Zombie::Change_Animation()
{
	if (true == m_isHoldTarget)
	{
		Change_Animation_Bite_PushDown();
	}
	else
	{
		Change_Animation_Lightly_Hold();
	}
}

void CLightly_Hold_Zombie::Change_Animation_Lightly_Hold()
{
	if (nullptr == m_pBlackBoard)
		return;
	
	CModel*			pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;	

	_int			iResultAnimationIndex = { -1 };
	//	Enter Start Anim
	if (LIGHTLY_HOLD_ANIM_STATE::_END == m_eAnimState)
	{
		if (DIRECTION::_L == m_eDirectionToPlayer)
		{
			iResultAnimationIndex = static_cast<_uint>(ANIM_BITE_LIGHTLY_HOLD::_LIGHTLY_HOLD_START_L);
		}
		else if (DIRECTION::_R == m_eDirectionToPlayer)
		{
			iResultAnimationIndex = static_cast<_uint>(ANIM_BITE_LIGHTLY_HOLD::_LIGHTLY_HOLD_START_R);
		}
#ifdef _DEBUG 
		else
			MSG_BOX(TEXT("Called : void CLightly_Hold_Zombie::Change_Animation_Lightly_Hold(LIGHTLY_HOLD_ANIM_STATE eState) 좀비 담당자 호출 "));
#endif
		m_eAnimState = LIGHTLY_HOLD_ANIM_STATE::_START;
	}

	else if (LIGHTLY_HOLD_ANIM_STATE::_START == m_eAnimState)
	{
		if (DIRECTION::_L == m_eDirectionToPlayer)
		{
			iResultAnimationIndex = static_cast<_uint>(ANIM_BITE_LIGHTLY_HOLD::_LIGHTLY_HOLD_END_L);
		}
		else if (DIRECTION::_R == m_eDirectionToPlayer)
		{
			iResultAnimationIndex = static_cast<_uint>(ANIM_BITE_LIGHTLY_HOLD::_LIGHTLY_HOLD_END_R);
		}
#ifdef _DEBUG 
		else
			MSG_BOX(TEXT("Called : void CLightly_Hold_Zombie::Change_Animation_Lightly_Hold(LIGHTLY_HOLD_ANIM_STATE eState) 좀비 담당자 호출 "));
#endif
		m_eAnimState = LIGHTLY_HOLD_ANIM_STATE::_FINISH;
	}	

	_int				iPreAnimIndex = { pBodyModel->Get_CurrentAnimIndex(static_cast<_uint>(m_ePlayingIndex)) };
	if (iPreAnimIndex != iResultAnimationIndex)
		m_pBlackBoard->Get_Player()->Request_NextBiteAnimation(iResultAnimationIndex);

	if (-1 == iResultAnimationIndex)
		return;

	pBodyModel->Change_Animation(static_cast<_uint>(m_ePlayingIndex), m_strLightlyHoldAnimLayerTag, iResultAnimationIndex);
	pBodyModel->Set_BoneLayer_PlayingInfo(static_cast<_uint>(m_ePlayingIndex), m_strBoneLayerTag);

	////////////////////TEST///////////////
	pBodyModel->Set_TickPerSec(m_strLightlyHoldAnimLayerTag, iResultAnimationIndex, 10.f);
}

void CLightly_Hold_Zombie::Change_Animation_Bite_PushDown()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*			pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_int			iResultAnimationIndex = { -1 };
	//	Enter Start Anim
	if (LIGHTLY_HOLD_ANIM_STATE::_END == m_eAnimState)
	{
		if (DIRECTION::_L == m_eDirectionToPlayer)
		{
			iResultAnimationIndex = static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_DOWN_START_L);
		}
		else if (DIRECTION::_R == m_eDirectionToPlayer)
		{
			iResultAnimationIndex = static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_DOWN_START_R);
		}
#ifdef _DEBUG 
		else
			MSG_BOX(TEXT("Called : void CLightly_Hold_Zombie::Change_Animation_Lightly_Hold(LIGHTLY_HOLD_ANIM_STATE eState) 좀비 담당자 호출 "));
#endif
		m_eAnimState = LIGHTLY_HOLD_ANIM_STATE::_MIDDLE;
	}

	else if (LIGHTLY_HOLD_ANIM_STATE::_START == m_eAnimState)
	{
		if (DIRECTION::_L == m_eDirectionToPlayer)
		{
			iResultAnimationIndex = static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_PUSH_DOWN_L1);
			iResultAnimationIndex = static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_PUSH_DOWN_L2);
		}
		else if (DIRECTION::_R == m_eDirectionToPlayer)
		{
			iResultAnimationIndex = static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_PUSH_DOWN_R1);
			iResultAnimationIndex = static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_PUSH_DOWN_R2);
		}
#ifdef _DEBUG 
		else
			MSG_BOX(TEXT("Called : void CLightly_Hold_Zombie::Change_Animation_Lightly_Hold(LIGHTLY_HOLD_ANIM_STATE eState) 좀비 담당자 호출 "));
#endif
		m_eAnimState = LIGHTLY_HOLD_ANIM_STATE::_FINISH;
	}

	else if (LIGHTLY_HOLD_ANIM_STATE::_MIDDLE == m_eAnimState)
	{
		_float			fPlayerHP = { static_cast<_float>(m_pBlackBoard->Get_Player()->Get_Hp()) };
		_float			fZombieAttack = { m_pBlackBoard->Get_AI()->Get_Status_Ptr()->fAttack };
		_bool			isCanKillPlayer = { fPlayerHP <= fZombieAttack };

		if (false == m_pBlackBoard->Hit_Player())
			return;

		if (true == isCanKillPlayer)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_BITE_PUSH_DOWN::_DOWN_KILL_R);
		}

		else
		{
			if (DIRECTION::_L == m_eDirectionToPlayer)
			{
				iResultAnimationIndex = static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_DOWN_REJECT_L);
			}
			else if (DIRECTION::_R == m_eDirectionToPlayer)
			{
				iResultAnimationIndex = static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_DOWN_REJECT_R);
			}
		}
		m_eAnimState = LIGHTLY_HOLD_ANIM_STATE::_END;
	}

	if (-1 == iResultAnimationIndex)
		return;

	pBodyModel->Change_Animation(static_cast<_uint>(m_ePlayingIndex), m_strPushDownAnimLayerTag, iResultAnimationIndex);
	pBodyModel->Set_BoneLayer_PlayingInfo(static_cast<_uint>(m_ePlayingIndex), m_strBoneLayerTag);
}

CLightly_Hold_Zombie::LIGHTLY_HOLD_ANIM_STATE CLightly_Hold_Zombie::Compute_Current_AnimState_Lightly_Hold()
{
	LIGHTLY_HOLD_ANIM_STATE			eAnimState = { LIGHTLY_HOLD_ANIM_STATE::_END };
	if (nullptr == m_pBlackBoard)
		return eAnimState;

	CModel*							pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
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
		eAnimState = LIGHTLY_HOLD_ANIM_STATE::_START;
	}

	else if (true == isMiddleAnim)
	{
		eAnimState = LIGHTLY_HOLD_ANIM_STATE::_MIDDLE;
	}

	else if (true == isFinishAnim)
	{
		eAnimState = LIGHTLY_HOLD_ANIM_STATE::_FINISH;
	}

	return eAnimState;
}

_bool CLightly_Hold_Zombie::Is_CurrentAnim_StartAnim()
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

_bool CLightly_Hold_Zombie::Is_CurrentAnim_MiddleAnim()
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

_bool CLightly_Hold_Zombie::Is_CurrentAnim_FinishAnim()
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

void CLightly_Hold_Zombie::Set_Lightly_Hold_LinearStart_HalfMatrix()
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
	if (false == m_pBlackBoard->Compute_HalfMatrix_Current_BiteAnim(strAnimLayerTag, iAnimIndex, &ResultMatrixFloat4x4))
		return;

	//	XMStoreFloat4x4(&ResultMatrixFloat4x4, m_pBlackBoard->Get_Player()->Get_Transform()->Get_WorldMatrix());

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

	list<string>			PlayerAnimTags = pPlayerBody_Model->Get_Animation_Tags(strAnimLayerTag);
	list<string>			ZombieAnimTags = pZombieBody_Model->Get_Animation_Tags(strAnimLayerTag);

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

_bool CLightly_Hold_Zombie::Is_StateFinished(LIGHTLY_HOLD_ANIM_STATE eState)
{
	return _bool();
}

void CLightly_Hold_Zombie::Update_PoseState_FaceState()
{
}

void CLightly_Hold_Zombie::Apply_HalfMatrix(_float fTimeDelta)
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

	_vector				vCurrentTranslation = { XMVectorSetW(vTranslation * fRatio, 0.f) };
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

HRESULT CLightly_Hold_Zombie::SetUp_AnimBranches()
{
	/* For.Start Anims */
	m_StartAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_DOWN_START_L));
	m_StartAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_DOWN_START_R));

	m_StartAnims[m_strLightlyHoldAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_LIGHTLY_HOLD::_LIGHTLY_HOLD_START_L));
	m_StartAnims[m_strLightlyHoldAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_LIGHTLY_HOLD::_LIGHTLY_HOLD_START_R));

	/* For.Middle Anims */
	m_MiddleAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_PUSH_DOWN_L1));
	m_MiddleAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_PUSH_DOWN_R1));
	m_MiddleAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_PUSH_DOWN_L2));
	m_MiddleAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_PUSH_DOWN_R2));

	/* For.Finish Anims */
	m_FinishAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_DOWN_REJECT_L));
	m_FinishAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_DOWN_REJECT_R));
	m_FinishAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_DOWN_KILL_R));

	m_StartAnims[m_strLightlyHoldAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_LIGHTLY_HOLD::_LIGHTLY_HOLD_END_L));
	m_StartAnims[m_strLightlyHoldAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_LIGHTLY_HOLD::_LIGHTLY_HOLD_END_R));

	return S_OK;
}

CLightly_Hold_Zombie* CLightly_Hold_Zombie::Create(void* pArg)
{
	CLightly_Hold_Zombie* pInstance = { new CLightly_Hold_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CLightly_Hold_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLightly_Hold_Zombie::Free()
{
	__super::Free();
}
