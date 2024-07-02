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

	m_fTotalLinearTime_HalfMatrix = 0.2f;

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

	m_isEntry = true;
	m_isHoldTarget = false;

	m_eAnimType = LIGHTLY_HOLD_ANIM_TYPE::_END;

	_float3			vDirectionToPlayerLocalFloat3 = {};
	if (false == m_pBlackBoard->Compute_Direction_To_Player_Local(&vDirectionToPlayerLocalFloat3))
		return;

	//	좀비의 오른쪽일떄 => 왼쪽으로 덮침
	m_eDirectionToPlayer = vDirectionToPlayerLocalFloat3.x > 0.f ? DIRECTION::_L : DIRECTION::_R;
	XMStoreFloat4x4(&m_Delta_Matrix_To_HalfMatrix, XMMatrixIdentity());

#ifdef _DEBUG

	cout << "Enter Lightly Hold " << endl;

#endif 
}

_bool CLightly_Hold_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	if (false == Condition_Check())
		return false;

	LIGHTLY_HOLD_ANIM_STATE			eAnimState = { Compute_Current_AnimState_Lightly_Hold() };
	_bool							isEntry = { eAnimState == LIGHTLY_HOLD_ANIM_STATE::_END };

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	if (false == m_isHoldTarget)
		pAI->Set_State(MONSTER_STATE::MST_LIGHTLY_HOLD);
	else
		pAI->Set_State(MONSTER_STATE::MST_BITE);

	Change_Animation(eAnimState);

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

			if (true == pBody_Model->isFinished(static_cast<_uint>(m_ePlayingIndex)))
				return false;
		}

		else
		{
			_float				fDistanceToPlayer = {};
			if (false == m_pBlackBoard->Compute_Distance_To_Player_World(&fDistanceToPlayer))
				return false;
			_bool				isCanBite = { pAI->Get_Status_Ptr()->fBiteRange >= fDistanceToPlayer};
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
	}	

	return true;
}

void CLightly_Hold_Zombie::Change_Animation(LIGHTLY_HOLD_ANIM_STATE eState)
{
	if (true == m_isHoldTarget)
	{

	}

	else
	{

	}
}

void CLightly_Hold_Zombie::Change_Animation_Lightly_Hold(LIGHTLY_HOLD_ANIM_STATE eState)
{
	if (nullptr == m_pBlackBoard)
		return;
	
	CModel*			pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;	

	_int			iResultAnimationIndex = { -1 };
	//	Enter Start Anim
	if (LIGHTLY_HOLD_ANIM_STATE::_END == eState)
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

	}

	else if (LIGHTLY_HOLD_ANIM_STATE::_START == eState)
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
	}	

	if (-1 == iResultAnimationIndex)
		return;

	pBodyModel->Change_Animation(static_cast<_uint>(m_ePlayingIndex), m_strLightlyHoldAnimLayerTag, iResultAnimationIndex);
	pBodyModel->Set_BoneLayer_PlayingInfo(static_cast<_uint>(m_ePlayingIndex), m_strBoneLayerTag);
}

void CLightly_Hold_Zombie::Change_Animation_Bite_PushDown(LIGHTLY_HOLD_ANIM_STATE eState)
{
	//enum class ANIM_BITE_PUSH_DOWN {
	//	_DOWN_START_L,			//	플레이어가 왼쪽 , 좀비가 오른쪽 ( 다운 상태의 플레이어를 물어 뜯음 )
	//	_DOWN_START_R,			//	플레이어가 오른쪽 , 좀비가 왼쪽 ( 다운 상태의 플레이어를 물어 뜯음 )
	//	_DOWN_REJECT_L,			//	오른쪽에 있는 좀비를 플레이어 우측으로 밀쳐냄
	//	_DOWN_REJECT_R,			//	왼쪽에 있는 좀비를 플레이어 좌측으로 밀쳐냄
	//	_PUSH_DOWN_L1,
	//	_PUSH_DOWN_R1,
	//	_PUSH_DOWN_L2,
	//	_PUSH_DOWN_R2,
	//	_DOWN_KILL_R,
	//	_END
	//};
	if (nullptr == m_pBlackBoard)
		return;

	CModel*			pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_int			iResultAnimationIndex = { -1 };
	//	Enter Start Anim
	if (LIGHTLY_HOLD_ANIM_STATE::_END == eState)
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
	}

	else if (LIGHTLY_HOLD_ANIM_STATE::_START == eState)
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
	}

	else if (LIGHTLY_HOLD_ANIM_STATE::_MIDDLE == eState)
	{
		_float			fPlayerHP = { static_cast<_float>(m_pBlackBoard->GetPlayer()->Get_Hp()) };
		_float			fZombieAttack = { m_pBlackBoard->Get_AI()->Get_Status_Ptr()->fAttack };
		_bool			isCanKillPlayer = { fPlayerHP <= fZombieAttack };

		if (true == isCanKillPlayer)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_BITE_PUSH_DOWN::_DOWN_KILL_R);
		}

		else
		{

		}

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
	}

	if (-1 == iResultAnimationIndex)
		return;

	pBodyModel->Change_Animation(static_cast<_uint>(m_ePlayingIndex), m_strLightlyHoldAnimLayerTag, iResultAnimationIndex);
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
	CModel* pPlayerBody_Model = { m_pBlackBoard->GetPlayer()->Get_Body_Model() };
	if (nullptr == pZombieBody_Model || nullptr == pPlayerBody_Model)
		return;

	_int					iAnimIndex = { pZombieBody_Model->Get_CurrentAnimIndex(static_cast<_uint>(m_ePlayingIndex)) };
	wstring					strAnimLayerTag = { pZombieBody_Model->Get_CurrentAnimLayerTag(static_cast<_uint>(m_ePlayingIndex)) };

	_float4x4				ResultMatrixFloat4x4;
	if (false == m_pBlackBoard->Compute_HalfMatrix_Current_BiteAnim(strAnimLayerTag, iAnimIndex, &ResultMatrixFloat4x4))
		return;

	//	XMStoreFloat4x4(&ResultMatrixFloat4x4, m_pBlackBoard->GetPlayer()->Get_Transform()->Get_WorldMatrix());

	_matrix					PlayerWorldMatrix = { m_pBlackBoard->GetPlayer()->Get_Transform()->Get_WorldMatrix() };
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
	m_pBlackBoard->GetPlayer()->Change_Player_State_Bite(iAnimIndex, strAnimLayerTag, PlayerDeltaMatrix, m_fTotalLinearTime_HalfMatrix);

	m_fAccLinearTime_HalfMatrix = 0.f;
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
}

HRESULT CLightly_Hold_Zombie::SetUp_AnimBranches()
{
	/* For.Start Anims */
	m_StartAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_DOWN_START_L));
	m_StartAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_DOWN_START_R));

	/* For.Middle Anims */
	m_MiddleAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_PUSH_DOWN_L1));
	m_MiddleAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_PUSH_DOWN_R1));
	m_MiddleAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_PUSH_DOWN_L2));
	m_MiddleAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_PUSH_DOWN_R2));

	/* For.Finish Anims */
	m_FinishAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_DOWN_REJECT_L));
	m_FinishAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_DOWN_REJECT_R));
	m_FinishAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_DOWN_KILL_R));

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
