#include "stdafx.h"
#include "GameInstance.h"
#include "Bite_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"

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

	m_fTotalLinearTime_HalfMatrix = 0.01f;

	return S_OK;
}

void CBite_Zombie::Enter()
{
	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	pBodyModel->Set_TotalLinearInterpolation(0.2f);
	pBodyModel->Set_Loop(static_cast<_uint>(PLAYING_INDEX::INDEX_0), false);

	pBodyModel->Set_BlendWeight(static_cast<_uint>(PLAYING_INDEX::INDEX_1), 0.f, 0.f);
	pBodyModel->Reset_PreAnim_CurrentAnim(static_cast<_uint>(PLAYING_INDEX::INDEX_1));

	m_isSendMSG_To_Player = false;
	m_eAnimType = BITE_ANIM_TYPE::_END;
}

_bool CBite_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	BITE_ANIM_STATE			eAnimState = { Compute_Current_AnimState_Bite() };
	_bool					isEntry = { eAnimState == BITE_ANIM_STATE::_END };
	if (true == isEntry)
	{
		if (false == Is_Can_Start_Bite())
			return false;
	}

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->GetAI();
	pAI->SetState(MONSTER_STATE::MST_BITE);

	if (true == Is_StateFinished(eAnimState))
		return false;

	Change_Animation(eAnimState);

	if (false == m_isSendMSG_To_Player)
	{
		Set_Bite_LinearStart_HalfMatrix();
		m_isSendMSG_To_Player = true;
	}
	Apply_HalfMatrix(fTimeDelta);

#ifdef _DEBUG

	cout << "is Bite" << endl;

#endif;

	return true;
}

void CBite_Zombie::Exit()
{
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

		_float			fPlayerHP = { static_cast<_float>(m_pBlackBoard->GetPlayer()->Get_Hp()) };
		_float			fZombieAttack = { m_pBlackBoard->GetAI()->Get_Status_Ptr()->fAttack };
		_bool			isCanKillPlayer = { fPlayerHP <= fZombieAttack };

		if (true == isCanKillPlayer)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_BITE_DEFAULT_FRONT::_KILL_F);
		}

		else
		{
			_int				iRandRejectAnimIndex = { m_pGameInstance->GetRandom_Int(static_cast<_int>(ANIM_BITE_DEFAULT_FRONT::_REJECT1), static_cast<_int>(ANIM_BITE_DEFAULT_FRONT::_REJECT3)) };
			iResultAnimationIndex = iRandRejectAnimIndex;
		}
	}

	_int			iPreAnimIndex = { pBodyModel->Get_CurrentAnimIndex(static_cast<_uint>(m_ePlayingIndex)) };
	if (iPreAnimIndex != iResultAnimationIndex)
		m_pBlackBoard->GetPlayer()->Request_NextBiteAnimation(iResultAnimationIndex);

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
		m_eAnimType = BITE_ANIM_TYPE::_DEFAULT_F;
	}

	//	Change MiddleAnim
	else if (BITE_ANIM_STATE::_START == eState)
	{
		if (false == pBodyModel->isFinished(static_cast<_int>(m_ePlayingIndex)))
			return;

		_float			fPlayerHP = { static_cast<_float>(m_pBlackBoard->GetPlayer()->Get_Hp()) };
		_float			fZombieAttack = { m_pBlackBoard->GetAI()->Get_Status_Ptr()->fAttack };
		_bool			isCanKillPlayer = { fPlayerHP <= fZombieAttack };

		if (true == isCanKillPlayer)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_BITE_DEFAULT_BACK::_KILL_B);
		}

		else
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_BITE_DEFAULT_BACK::_REJECT_B);
		}
	}

	_int			iPreAnimIndex = { pBodyModel->Get_CurrentAnimIndex(static_cast<_uint>(m_ePlayingIndex)) };
	if (iPreAnimIndex != iResultAnimationIndex)
		m_pBlackBoard->GetPlayer()->Request_NextBiteAnimation(iResultAnimationIndex);

	pBodyModel->Change_Animation(static_cast<_uint>(m_ePlayingIndex), m_strDefaultFrontAnimLayerTag, iResultAnimationIndex);
	pBodyModel->Set_BoneLayer_PlayingInfo(static_cast<_uint>(m_ePlayingIndex), m_strBoneLayerTag);
}

void CBite_Zombie::Change_Animation_Creep(BITE_ANIM_STATE eState)
{
}

void CBite_Zombie::Change_Animation_Push_Down(BITE_ANIM_STATE eState)
{
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
	_bool				isCanBite = { false };

	_float			fDistanceToPlayer = { 0.f };
	if (false == m_pBlackBoard->Compute_Distance_To_Player(&fDistanceToPlayer))
		return isCanBite;

	CMonster::MONSTER_STATUS* pMonster_Status = { m_pBlackBoard->Get_ZombieStatus_Ptr() };
	if (nullptr == pMonster_Status)
		return isCanBite;

	isCanBite = pMonster_Status->fBiteRange >= fDistanceToPlayer;
	return isCanBite;
}

void CBite_Zombie::Change_Animation(BITE_ANIM_STATE eState)
{
	/*if ()
		Change_Animation_Creep();

	else if ()
		Change_Animation_Lost();

	else if ()
		Change_Animation_Push_Down();

	else
		Change_Animation_Default();*/

	DIRECTION		eDirection = { DIRECTION::_END };
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
	}
}

void CBite_Zombie::Set_Bite_LinearStart_HalfMatrix()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*					pZombieBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	CModel*					pPlayerBody_Model = { m_pBlackBoard->GetPlayer()->Get_Body_Model() };
	if (nullptr == pZombieBody_Model || nullptr == pPlayerBody_Model)
		return;

	_int					iAnimIndex = { pZombieBody_Model->Get_CurrentAnimIndex(static_cast<_uint>(m_ePlayingIndex)) };
	wstring					strAnimLayerTag = { pZombieBody_Model->Get_CurrentAnimLayerTag(static_cast<_uint>(m_ePlayingIndex)) };

	_float4x4				ResultMatrixFloat4x4;
	if (false == m_pBlackBoard->Compute_HalfMatrix_Current_BiteAnim(strAnimLayerTag, iAnimIndex, &ResultMatrixFloat4x4))
		return;

	_matrix					PlayerWorldMatrix = { m_pBlackBoard->GetPlayer()->Get_Transform()->Get_WorldMatrix() };
	_matrix					ZombieWorldMatrix = { m_pBlackBoard->GetAI()->Get_Transform()->Get_WorldMatrix() };

	_vector					vPlayerWorldScale, vPlayerWorldQuaternion, vPlayerWorldTranslation;
	_vector					vZombieWorldScale, vZombieWorldQutaernion, vZombieWorldTranslation;
	XMMatrixDecompose(&vPlayerWorldScale, &vPlayerWorldQuaternion, &vPlayerWorldTranslation, PlayerWorldMatrix);
	XMMatrixDecompose(&vZombieWorldScale, &vZombieWorldQutaernion, &vZombieWorldTranslation, ZombieWorldMatrix);

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

	_matrix					PlayerModelTransformationMatrix = { XMLoadFloat4x4(&pPlayerBody_Model->Get_TransformationMatrix()) };
	_matrix					ZombieModelTransformationMatrix = { XMLoadFloat4x4(&pZombieBody_Model->Get_TransformationMatrix()) };
	
	///	모델의 트랜스포메이션 매트릭스 ( 루트본의 부모 )를 해당 첫키프레임에 입힘
	_matrix					RootFirstKeyFramePlayerCombined = { RootFirstKeyFramePlayer * PlayerModelTransformationMatrix };
	_matrix					RootFirstKeyFrameZombieCombined = { RootFirstKeyFrameZombie * ZombieModelTransformationMatrix };

	
	//	중간 상태의 월드를 기반으로 첫 키프레임의 컴바인드 매트릭스를 월드화
	_matrix					RootFirstKeyFramePlayerWorld = { RootFirstKeyFramePlayerCombined * PlayerHalfMatrix };
	_matrix					RootFirstKeyFrameZombieWorld = { RootFirstKeyFrameZombieCombined * ZombieHalfMatrix };

	_vector					vPlayerResultScale, vPlayerResultQuaternion, vPlayerResultTranslation;
	_vector					vZombieResultScale, vZombieResultQuaternion, vZombieResultTranslation;
	XMMatrixDecompose(&vPlayerResultScale, &vPlayerResultQuaternion, &vPlayerResultTranslation, RootFirstKeyFramePlayerWorld);
	XMMatrixDecompose(&vZombieResultScale, &vZombieResultQuaternion, &vZombieResultTranslation, RootFirstKeyFrameZombieWorld);


	//	_vector					vZombieDeltaQuaternion = { XMQuaternionMultiply(XMQuaternionInverse(vZombieResultQuaternion), XMQuaternionNormalize(vZombieWorldQutaernion)) };
	vZombieResultTranslation = XMVector3TransformCoord(vZombieResultTranslation, XMMatrixRotationQuaternion(XMQuaternionInverse(vZombieResultQuaternion)));
	_vector					vZombieDeltaQuaternion = { XMQuaternionMultiply(XMQuaternionInverse(vZombieWorldQutaernion), XMQuaternionNormalize(vZombieResultQuaternion)) };
	_vector					vZombieDeltaTranslation = { vZombieResultTranslation - vZombieWorldTranslation };

	//	_vector					vPlayerDeltaQuaternion = { XMQuaternionMultiply(XMQuaternionInverse(vPlayerResultQuaternion), XMQuaternionNormalize(vPlayerWorldQuaternion)) };
	vPlayerResultTranslation = XMVector3TransformCoord(vPlayerResultTranslation, XMMatrixRotationQuaternion(XMQuaternionInverse(vPlayerResultQuaternion)));
	_vector					vPlayerDeltaQuaternion = { XMQuaternionMultiply(XMQuaternionInverse(vPlayerWorldQuaternion), XMQuaternionNormalize(vPlayerResultQuaternion)) };
	_vector					vPlayerDeltaTranslation = { vPlayerResultTranslation - vPlayerWorldTranslation };

	_matrix					PlayerDeltaMatrix = { XMMatrixAffineTransformation(vPlayerWorldScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vPlayerDeltaQuaternion, vPlayerDeltaTranslation) };
	_matrix					ZombieDeltaMatrix = { XMMatrixAffineTransformation(vZombieWorldScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vZombieDeltaQuaternion, vZombieDeltaTranslation) };

	//	XMStoreFloat4x4(&m_Delta_Matrix_To_HalfMatrix, ZombieHalfMatrix);
	XMStoreFloat4x4(&m_Delta_Matrix_To_HalfMatrix, ZombieDeltaMatrix);
	//	XMStoreFloat4x4(&m_Delta_Matrix_To_HalfMatrix, ZombieDeltaMatrix);
	//	m_pBlackBoard->GetPlayer()->Change_Player_State_Bite(iAnimIndex, strAnimLayerTag, PlayerHalfMatrix, m_fTotalLinearTime_HalfMatrix);
	m_pBlackBoard->GetPlayer()->Change_Player_State_Bite(iAnimIndex, strAnimLayerTag, PlayerDeltaMatrix, m_fTotalLinearTime_HalfMatrix);
	//	m_pBlackBoard->GetPlayer()->Change_Player_State_Bite(iAnimIndex, strAnimLayerTag, PlayerDeltaMatrix, m_fTotalLinearTime_HalfMatrix);



	m_fAccLinearTime_HalfMatrix = 0.f;
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

void CBite_Zombie::Apply_HalfMatrix(_float fTimeDelta)
{
	if (m_fAccLinearTime_HalfMatrix >= m_fTotalLinearTime_HalfMatrix)
		return;

	if (nullptr == m_pBlackBoard)
		return;

	CTransform* pAITransform = { m_pBlackBoard->GetAI()->Get_Transform() };
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
		_vector				vCurrentQuaternion = { XMQuaternionSlerp(XMQuaternionIdentity(), vQuaternion, fRatio) };
		
		//	_vector				vCurrentQuaternionInv = { XMQuaternionInverse(XMQuaternionNormalize(vCurrentQuaternion)) };
		//	_matrix				vCurrentRotationInverse = { XMMatrixRotationQuaternion(vCurrentQuaternionInv) };
		//	vCurrentTranslation = XMVector3TransformNormal(vCurrentTranslation, vCurrentRotationInverse);
		
		_matrix				AIWorldMatrix = { pAITransform->Get_WorldMatrix() };
		_matrix				CurrentRotationMatrix = { XMMatrixRotationQuaternion(vCurrentQuaternion) };
		_matrix				CurrentTimesMatrix = { CurrentRotationMatrix };
		CurrentTimesMatrix.r[CTransform::STATE_POSITION].m128_f32[3] = 0.f;
		
		_vector				vPosition = { AIWorldMatrix.r[CTransform::STATE_POSITION] };
		AIWorldMatrix.r[CTransform::STATE_POSITION] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		
		_matrix				TimesCombinedMatrix = { AIWorldMatrix * CurrentTimesMatrix };
		TimesCombinedMatrix.r[CTransform::STATE_POSITION] = vPosition;
		
		pAITransform->Set_WorldMatrix(TimesCombinedMatrix);
		m_pBlackBoard->GetAI()->Add_Root_Translation(vCurrentTranslation);
}

HRESULT CBite_Zombie::SetUp_AnimBranches()
{
	/* For.Start Anims */
	m_StartAnims[m_strDefaultFrontAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_DEFAULT_FRONT::_START_F));
	m_StartAnims[m_strDefaultFrontAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_DEFAULT_FRONT::_CREEP_START));

	m_StartAnims[m_strDefaultBackAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_DEFAULT_BACK::_START_B));

	m_StartAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_DOWN_START_L));
	m_StartAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_DOWN_START_R));

	m_StartAnims[m_strCreepAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_CREEP::_FACE_UP_L));
	m_StartAnims[m_strCreepAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_CREEP::_FACE_UP_R));
	m_StartAnims[m_strCreepAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_CREEP::_FACE_DOWN_L));
	m_StartAnims[m_strCreepAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_CREEP::_FACE_DOWN_R));

	/* For.Middle Anims */
	m_MiddleAnims[m_strDefaultFrontAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_DEFAULT_FRONT::_DEFAULT));

	m_MiddleAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_PUSH_DOWN_L1));
	m_MiddleAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_PUSH_DOWN_R1));
	m_MiddleAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_PUSH_DOWN_L2));
	m_MiddleAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_PUSH_DOWN_R2));

	/* For.Finish Anims */
	m_FinishAnims[m_strDefaultFrontAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_DEFAULT_FRONT::_REJECT1));
	m_FinishAnims[m_strDefaultFrontAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_DEFAULT_FRONT::_REJECT2));
	m_FinishAnims[m_strDefaultFrontAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_DEFAULT_FRONT::_REJECT3));
	m_FinishAnims[m_strDefaultFrontAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_DEFAULT_FRONT::_KILL_F));

	m_FinishAnims[m_strDefaultBackAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_DEFAULT_BACK::_REJECT_B));
	m_FinishAnims[m_strDefaultBackAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_DEFAULT_BACK::_KILL_B));

	m_FinishAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_DOWN_REJECT_L));
	m_FinishAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_DOWN_REJECT_R));
	m_FinishAnims[m_strPushDownAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_PUSH_DOWN::_DOWN_KILL_R));

	m_FinishAnims[m_strCreepAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_CREEP::_CREEP_REJECT_L));
	m_FinishAnims[m_strCreepAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_CREEP::_CREEP_REJECT_R));
	m_FinishAnims[m_strCreepAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_CREEP::_CREEP_KILL_L));
	m_FinishAnims[m_strCreepAnimLayerTag].emplace(static_cast<_uint>(ANIM_BITE_CREEP::_CREEP_KILL_R));

	return S_OK;
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