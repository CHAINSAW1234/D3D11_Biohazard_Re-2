#include "stdafx.h"
#include "GameInstance.h"
#include "Move_To_Target_Zombie.h"
#include "BlackBoard_Zombie.h"

#include "Zombie.h"
#include "Body_Zombie.h"

#include "Player.h"
#include "Window.h"
#include "Door.h"


CMove_To_Target_Zombie::CMove_To_Target_Zombie()
	: CTask_Node()
{
}

CMove_To_Target_Zombie::CMove_To_Target_Zombie(const CMove_To_Target_Zombie& rhs)
{
}

HRESULT CMove_To_Target_Zombie::Initialize(void* pArg)
{

	//	Lost류 애님 처리 필요
	//	Tooter 애님 처리 필요 
	//	Turning 애님 처리 필요

	/* For.Brach Start */
	unordered_set<_uint>			OrdinaryStairWalkStartIndices;
	OrdinaryStairWalkStartIndices.emplace(static_cast<_uint>(ANIM_ORDINARY_STAIRS_WALK::_START));
	m_StartAnimIndicesLayer.emplace(TEXT("Ordinary_Stairs_Walk"), OrdinaryStairWalkStartIndices);

	unordered_set<_uint>			OrdinaryWalkStartIndices;
	OrdinaryWalkStartIndices.emplace(static_cast<_uint>(ANIM_ORDINARY_WALK::_START_A));
	OrdinaryWalkStartIndices.emplace(static_cast<_uint>(ANIM_ORDINARY_WALK::_START_B));
	OrdinaryWalkStartIndices.emplace(static_cast<_uint>(ANIM_ORDINARY_WALK::_START_C));
	OrdinaryWalkStartIndices.emplace(static_cast<_uint>(ANIM_ORDINARY_WALK::_START_D));
	OrdinaryWalkStartIndices.emplace(static_cast<_uint>(ANIM_ORDINARY_WALK::_START_E));
	OrdinaryWalkStartIndices.emplace(static_cast<_uint>(ANIM_ORDINARY_WALK::_START_F));
	m_StartAnimIndicesLayer.emplace(TEXT("Ordinary_Walk"), OrdinaryWalkStartIndices);

	unordered_set<_uint>			OrdinaryWalkLoseStartIndices;
	OrdinaryWalkLoseStartIndices.emplace(static_cast<_uint>(ANIM_ORDINARY_WALK_LOSE::_ONES_FOOTING_START));
	m_StartAnimIndicesLayer.emplace(TEXT("Ordinary_Walk_Lose"), OrdinaryWalkLoseStartIndices);


	/* For.Branch Loop */
	unordered_set<_uint>			OrdinaryWalkLoopIndices;
	OrdinaryWalkLoopIndices.emplace(static_cast<_uint>(ANIM_ORDINARY_WALK::_LOOP_A));
	OrdinaryWalkLoopIndices.emplace(static_cast<_uint>(ANIM_ORDINARY_WALK::_LOOP_B));
	OrdinaryWalkLoopIndices.emplace(static_cast<_uint>(ANIM_ORDINARY_WALK::_LOOP_C));
	OrdinaryWalkLoopIndices.emplace(static_cast<_uint>(ANIM_ORDINARY_WALK::_LOOP_D));
	OrdinaryWalkLoopIndices.emplace(static_cast<_uint>(ANIM_ORDINARY_WALK::_LOOP_E));
	OrdinaryWalkLoopIndices.emplace(static_cast<_uint>(ANIM_ORDINARY_WALK::_LOOP_F));
	m_LoopAnimIndicesLayer.emplace(TEXT("Ordinary_Walk"), OrdinaryWalkLoopIndices);

	unordered_set<_uint>			OrdinaryWalkLoseLoopIndices;
	OrdinaryWalkLoseLoopIndices.emplace(static_cast<_uint>(ANIM_ORDINARY_WALK_LOSE::_ONES_FOOTING_LOOP));
	m_StartAnimIndicesLayer.emplace(TEXT("Ordinary_Walk_Lose"), OrdinaryWalkLoseLoopIndices);

	return S_OK;
}

void CMove_To_Target_Zombie::Enter()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	m_fAccBlendTime = 0.f;
	m_isEnterNextRoom = false;
	m_isMoveToPlayer_LinkedRoom = false; 
	m_eNextRoom = LOCATION_MAP_VISIT_END;

#ifdef _DEBUG

	cout << "Enter Move To Tareget" << endl;

#endif 
}

_bool CMove_To_Target_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	CZombie::POSE_STATE			ePoseState = { m_pBlackBoard->Get_AI()->Get_PoseState() };
	if (CZombie::POSE_STATE::_UP != ePoseState)
		return false;

	if (false == m_pBlackBoard->Get_AI()->Is_OutDoor())
	{
		LOCATION_MAP_VISIT			ePlayerLocation = { static_cast<LOCATION_MAP_VISIT>(m_pBlackBoard->Get_Player()->Get_Player_Region()) };
		_bool						isSameLocation_To_Target = { m_pBlackBoard->Get_AI()->Is_In_Location(ePlayerLocation) };
		_bool						isLinkedLocation_To_Target = { m_pBlackBoard->Get_AI()->Is_In_Linked_Location(ePlayerLocation) };

		_bool						isDoorTarget = { false };
		if (false == isSameLocation_To_Target)
		{
			if (false == isLinkedLocation_To_Target)
				return false;

			if (false == m_pBlackBoard->Is_LookTarget())
				return false;

			CDoor*			pTarget_Door = { m_pBlackBoard->Get_Target_Door() };
			if (nullptr == pTarget_Door)
				return false;
			
			if (false == pTarget_Door->Is_Dummy_Door())
			{
				/*_vector			vTargetPosition = { pTarget_Door->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) };
				_vector			vMyPosition = { m_pBlackBoard->Get_AI()->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) };

				_vector			vDirectionToTarget = { vTargetPosition - vMyPosition };
				_vector			vLook = { m_pBlackBoard->Get_AI()->Get_Transform()->Get_State_Vector(CTransform::STATE_LOOK) };

				_float			fDot = { XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDirectionToTarget), XMVector3Normalize(vLook))) };
				_float			fAngle = { acosf(fDot) };

				if (fAngle > XMConvertToRadians(60.f))
				return false;*/

				Safe_Release(m_pTargetObject);
				m_pTargetObject = pTarget_Door;
				Safe_AddRef(m_pTargetObject);
				m_isIncludeRotation = true;

				isDoorTarget = true;
			}		

			else
			{
				m_isMoveToPlayer_LinkedRoom = true;

				list<LOCATION_MAP_VISIT>				LinkedLocations = { pTarget_Door->Get_LinkedLocations() };
				LOCATION_MAP_VISIT						eCurrentLocation = { m_pBlackBoard->Get_AI()->Get_Location() };
				for (auto& eLocation : LinkedLocations)
				{
					if (eCurrentLocation != eLocation)
						m_eNextRoom = eLocation;
				}

				if (m_eNextRoom == LOCATION_MAP_VISIT_END)
					return false;
			}
		}

		if (false == isDoorTarget)
		{
			if (false == m_pBlackBoard->Is_LookTarget())
				return false;

			_float3			vDirectionToPlayerLocalFloat3;
			if (false == m_pBlackBoard->Compute_Direction_To_Player_Local(&vDirectionToPlayerLocalFloat3))
				return false;

			_vector			vDirectionToPlayerLocal = { XMLoadFloat3(&vDirectionToPlayerLocalFloat3) };
			_vector			vDirectionToPlayerLocalXZPlane = { XMVector3Normalize(XMVectorSetY(vDirectionToPlayerLocal, 0.f)) };		//	회전량을 xz평면상에서만 고려하기위함
			_vector			vAILookLocal = { XMVectorSet(0.f, 0.f, 1.f, 0.f) };

			_bool			isRight = { XMVectorGetX(vDirectionToPlayerLocalXZPlane) > 0.f };

			_float			fDot = { XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDirectionToPlayerLocal), XMVector3Normalize(vAILookLocal))) };
			_float			fAngleToTarget = { acosf(fDot) };

			_float			fMaxMoveAngle = { m_pBlackBoard->Get_AI()->Get_Status_Ptr()->fMaxMoveAngle };
			if (fMaxMoveAngle < fAngleToTarget)
				return false;

			Safe_Release(m_pTargetObject);
			m_pTargetObject = m_pBlackBoard->Get_Player();
			Safe_AddRef(m_pTargetObject);
			m_isIncludeRotation = true;
		}		
	}

	else
	{
		CWindow*		pNearestWindow = { static_cast<CWindow*>(m_pBlackBoard->Get_Nearest_Window()) };
		if (nullptr == pNearestWindow)
			return false;		

		Safe_Release(m_pTargetObject);

		m_pTargetObject = pNearestWindow;
		Safe_AddRef(m_pTargetObject);
		m_isIncludeRotation = false;
	}	

	m_pBlackBoard->Organize_PreState(this);

	m_fRemainSoundTime -= fTimeDelta;
	if (0.f > m_fRemainSoundTime)
	{
		m_fRemainSoundTime = m_pGameInstance->GetRandom_Real(ZOMBIE_MOVE_SOUND_MIN_REMAIN, ZOMBIE_MOVE_SOUND_MAX_REMAIN);
		m_pBlackBoard->Get_AI()->Play_Random_Move_Sound();
	}

	if (true == m_isMoveToPlayer_LinkedRoom)
	{
		CDoor*					pTargetDoor = { m_pBlackBoard->Get_Target_Door() };
		if (nullptr != pTargetDoor)
		{
			_vector				vDoorPosition = { pTargetDoor->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) };
			_vector				vZombiePosition = { m_pBlackBoard->Get_AI()->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) };
			
			_float				fDistance = { XMVectorGetX(XMVector3Length(vDoorPosition - vDoorPosition)) };
			if (fDistance < 1.f)
			{
				if (m_eNextRoom != LOCATION_MAP_VISIT_END)
				{
					m_pBlackBoard->Get_AI()->Set_Location(m_eNextRoom);
					m_isMoveToPlayer_LinkedRoom = false;
				}
			}
		}
	}

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_WALK);

	if (true == m_isIncludeRotation)
		Change_Animation_Include_Rotation(fTimeDelta);
	else
	{
		Change_Animation_Front_Only(fTimeDelta);
		Turn_To_TargetObject_Manual(fTimeDelta);
	}

	return true;
}

void CMove_To_Target_Zombie::Exit()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*			pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_int			iBlendPlayingIndex = { static_cast<_uint>(PLAYING_INDEX::INDEX_1) };

	_float			fPreBlendWeight = { pBodyModel->Get_BlendWeight(static_cast<_uint>(PLAYING_INDEX::INDEX_1)) };
	if (fPreBlendWeight > 0.f)
	{
		_float			fTrackPosition = { pBodyModel->Get_TrackPosition(iBlendPlayingIndex) };
		_float			fDuration = { pBodyModel->Get_Duration_From_PlayingInfo(iBlendPlayingIndex) };
		_float			fTickPerSec = { pBodyModel->Get_TickPerSec_From_PlayingInfo(iBlendPlayingIndex) };

		_float			fDelta = { fDuration - fTrackPosition };

		pBodyModel->Set_BlendWeight(iBlendPlayingIndex, 0.f, fminf(fDelta / fTickPerSec, 10.f));
	}	
}

void CMove_To_Target_Zombie::Change_Animation_Front_Only(_float fTimeDelta)
{
	CModel*			pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;
	PLAYING_INDEX					eBasePlayingIndex = { PLAYING_INDEX::INDEX_0 };
	PLAYING_INDEX					eBlendPlayingIndex = { PLAYING_INDEX::INDEX_1 };

	ZOMBIE_BODY_ANIM_GROUP			eCurrentAnimGroup = { m_pBlackBoard->Get_Current_AnimGroup(eBasePlayingIndex) };
	ZOMBIE_BODY_ANIM_TYPE			eCurrentAnimType = { m_pBlackBoard->Get_Current_AnimType(eBasePlayingIndex) };

	_bool							isMoveEntry = { false };
	if (ZOMBIE_BODY_ANIM_TYPE::_MOVE != eCurrentAnimType)
	{
		isMoveEntry = true;
	}
	_int			iResultAnimationIndex = { -1 };
	_bool			isLoop = { false };
	wstring			strBaseBoneLayerTag = { BONE_LAYER_DEFAULT_TAG };

	MOTION_TYPE		eCurrentMotionType = { static_cast<MOTION_TYPE>(m_pBlackBoard->Get_Current_MotionType_Body()) };
	MOTION_TYPE		ePreMotionType = { static_cast<MOTION_TYPE>(m_pBlackBoard->Get_Pre_MotionType_Body()) };

	//	무브 상태 첫 진입
	if (true == isMoveEntry)
	{
		if (MOTION_TYPE::MOTION_A == eCurrentMotionType)
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_START_A);
		else if (MOTION_TYPE::MOTION_B == eCurrentMotionType)
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_START_B);
		else if (MOTION_TYPE::MOTION_C == eCurrentMotionType)
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_START_C);
		else if (MOTION_TYPE::MOTION_D == eCurrentMotionType)
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_START_D);
		else if (MOTION_TYPE::MOTION_E == eCurrentMotionType)
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_START_E);
		else if (MOTION_TYPE::MOTION_F == eCurrentMotionType)
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_START_F);
		else
			MSG_BOX(TEXT("이럴리없어 좀비 담당자 호출 ㄱ"));

		isLoop = false;
	}

	else
	{
		_bool		isStartAnim = { Is_CurrentAnim_StartAnim(PLAYING_INDEX::INDEX_0) };
		_bool		isFinished = { false };
		//	아직 첫 모션 중이었을경우 => 상태 체크가 필요함 => 스타트모션의 경우 끝나야 루프로 넘어가야함
		if (true == isStartAnim)
		{
			_int	iCurrentAnimIndex = { m_pBlackBoard->Get_Current_AnimIndex(CMonster::PART_BODY, PLAYING_INDEX::INDEX_0) };
			//	아직 스타트 모션 진행중
			if (false == pBodyModel->isFinished(static_cast<_uint>(PLAYING_INDEX::INDEX_0)))
				iResultAnimationIndex = iCurrentAnimIndex;

			else
				isStartAnim = false;
		}

		if (false == isStartAnim)
		{
			if (MOTION_TYPE::MOTION_A == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_LOOP_A);
			else if (MOTION_TYPE::MOTION_B == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_LOOP_B);
			else if (MOTION_TYPE::MOTION_C == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_LOOP_C);
			else if (MOTION_TYPE::MOTION_D == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_LOOP_D);
			else if (MOTION_TYPE::MOTION_E == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_LOOP_E);
			else if (MOTION_TYPE::MOTION_F == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_LOOP_F);

			isLoop = true;
		}
	}

	if (-1 == iResultAnimationIndex)
		return;

	//	루프애님이면서 이전 애니메이션과 같다면... 같은 애니메이션을 지속으로 루프 재생중이므로 보간이 필요없다고 판단
	if (true == isLoop && m_pBlackBoard->Get_Current_AnimIndex(CMonster::PART_BODY, PLAYING_INDEX::INDEX_0) == iResultAnimationIndex)
	{
		pBodyModel->Set_TotalLinearInterpolation(0.f);
	}

	//else
	//{
	//	pBodyModel->Set_TotalLinearInterpolation(0.8f);
	//}

	pBodyModel->Change_Animation(static_cast<_uint>(PLAYING_INDEX::INDEX_0), TEXT("Ordinary_Walk"), iResultAnimationIndex);
	pBodyModel->Set_Loop(static_cast<_uint>(PLAYING_INDEX::INDEX_0), isLoop);
	pBodyModel->Set_BoneLayer_PlayingInfo(static_cast<_uint>(PLAYING_INDEX::INDEX_0), strBaseBoneLayerTag);
	pBodyModel->Set_BlendWeight(static_cast<_uint>(PLAYING_INDEX::INDEX_0), 1.f, 0.f);

#pragma endregion
}

void CMove_To_Target_Zombie::Change_Animation_Include_Rotation(_float fTimeDelta)
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*			pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;
	PLAYING_INDEX					eBasePlayingIndex = { PLAYING_INDEX::INDEX_0 };
	PLAYING_INDEX					eBlendPlayingIndex = { PLAYING_INDEX::INDEX_1 };

	ZOMBIE_BODY_ANIM_GROUP			eCurrentAnimGroup = { m_pBlackBoard->Get_Current_AnimGroup(eBasePlayingIndex) };
	ZOMBIE_BODY_ANIM_TYPE			eCurrentAnimType = { m_pBlackBoard->Get_Current_AnimType(eBasePlayingIndex) };

	_bool							isMoveEntry = { false };
	if (ZOMBIE_BODY_ANIM_TYPE::_MOVE != eCurrentAnimType)
	{
		isMoveEntry = true;
	}

	/* 기본 이동 애니메이션 */
	//	_int			iCurrentBasegAnimIndex = { pBodyModel->Get_AnimIndex_PlayingInfo(static_cast<_uint>(PLAYING_INDEX::INDEX_0)) };
	//	wstring			strCurrentBaseAnimLayerTag = { pBodyModel->Get_CurrentAnimLayerTag(static_cast<_uint>(PLAYING_INDEX::INDEX_0)) };
	_int			iResultAnimationIndex = { -1 };
	_bool			isLoop = { false };
	wstring			strBaseBoneLayerTag = { BONE_LAYER_DEFAULT_TAG };

	/* 블렌드 필요 여부*/
	_bool			isNeedBlend = { false };

	/* 회전 섞여야하는경우 터닝모션 블렌드 비율 */
	//	_int			iCurrentBlendAnimIndex = { pBodyModel->Get_AnimIndex_PlayingInfo(static_cast<_uint>(PLAYING_INDEX::INDEX_1)) };
	//	wstring			strCurrentBlendAnimLayerTag = { pBodyModel->Get_CurrentAnimLayerTag(static_cast<_uint>(PLAYING_INDEX::INDEX_1)) };
	_float			fTurnBlendWeight = { 0.f };
	_int			iBlendAnimIndex = { -1 };
	wstring			strBlendBoneLayerTag = { BONE_LAYER_DEFAULT_TAG };

	MOTION_TYPE		eCurrentMotionType = { static_cast<MOTION_TYPE>(m_pBlackBoard->Get_Current_MotionType_Body()) };
	MOTION_TYPE		ePreMotionType = { static_cast<MOTION_TYPE>(m_pBlackBoard->Get_Pre_MotionType_Body()) };

	//	무브 상태 첫 진입
	if (true == isMoveEntry)
	{
		if (MOTION_TYPE::MOTION_A == eCurrentMotionType)
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_START_A);
		else if (MOTION_TYPE::MOTION_B == eCurrentMotionType)
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_START_B);
		else if (MOTION_TYPE::MOTION_C == eCurrentMotionType)
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_START_C);
		else if (MOTION_TYPE::MOTION_D == eCurrentMotionType)
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_START_D);
		else if (MOTION_TYPE::MOTION_E == eCurrentMotionType)
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_START_E);
		else if (MOTION_TYPE::MOTION_F == eCurrentMotionType)
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_START_F);
		else
			MSG_BOX(TEXT("이럴리없어 좀비 담당자 호출 ㄱ"));

		/*_float			fAnimDuration = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY)->Get_Animations(TEXT("Ordinary_Walk"))[iResultAnimationIndex]->Get_Duration() };
		_float			fTickPerSec = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY)->Get_Animations(TEXT("Ordinary_Walk"))[iResultAnimationIndex]->Get_TickPerSec() };
		m_pBlackBoard->Get_PartModel(CMonster::PART_BODY)->Set_TotalLinearInterpolation(fAnimDuration / fTickPerSec);*/

		_int			iBlendPlayingIndex = { static_cast<_uint>(PLAYING_INDEX::INDEX_1) };
		//	pBodyModel->Set_BlendWeight(iBlendPlayingIndex, 0.f, fAnimDuration / fTickPerSec);

		isLoop = false;
	}

	else
	{
		_bool		isStartAnim = { Is_CurrentAnim_StartAnim(PLAYING_INDEX::INDEX_0) };
		_bool		isFinished = { false };
		//	아직 첫 모션 중이었을경우 => 상태 체크가 필요함 => 스타트모션의 경우 끝나야 루프로 넘어가야함
		if (true == isStartAnim)
		{
			_int	iCurrentAnimIndex = { m_pBlackBoard->Get_Current_AnimIndex(CMonster::PART_BODY, PLAYING_INDEX::INDEX_0) };
			//	아직 스타트 모션 진행중
			if (false == pBodyModel->isFinished(static_cast<_uint>(PLAYING_INDEX::INDEX_0)))
				iResultAnimationIndex = iCurrentAnimIndex;

			else
				isStartAnim = false;
		}

		if (false == isStartAnim)
		{
			if (MOTION_TYPE::MOTION_A == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_LOOP_A);
			else if (MOTION_TYPE::MOTION_B == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_LOOP_B);
			else if (MOTION_TYPE::MOTION_C == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_LOOP_C);
			else if (MOTION_TYPE::MOTION_D == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_LOOP_D);
			else if (MOTION_TYPE::MOTION_E == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_LOOP_E);
			else if (MOTION_TYPE::MOTION_F == eCurrentMotionType)
				iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_LOOP_F);

			isLoop = true;

			_float3			vDirectionToPlayerLocalFloat3;
			if (false == m_pBlackBoard->Compute_Direction_To_Player_Local(&vDirectionToPlayerLocalFloat3))
				return;

			_vector			vDirectionToPlayerLocal = { XMLoadFloat3(&vDirectionToPlayerLocalFloat3) };
			_vector			vDirectionToPlayerLocalXZPlane = { XMVector3Normalize(XMVectorSetY(vDirectionToPlayerLocal, 0.f)) };		//	회전량을 xz평면상에서만 고려하기위함
			_vector			vAILookLocal = { XMVectorSet(0.f, 0.f, 1.f, 0.f) };

			_bool			isRight = { XMVectorGetX(vDirectionToPlayerLocalXZPlane) > 0.f };
			_bool			isFront = { XMVectorGetZ(vDirectionToPlayerLocalXZPlane) > 0.f };

			_float			fDot = { XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDirectionToPlayerLocal), XMVector3Normalize(vAILookLocal))) };
			_float			fAngleToTarget = { acosf(fDot) };


			//	회전과 관련하여 블렌드 성분 정하기
			//	10도 미만은 그냥직진하기.
			if (XMConvertToRadians(10.f) > fAngleToTarget)
			{
				isNeedBlend = false;

				m_fAccBlendTime -= fTimeDelta;
			}

			else
			{
				isNeedBlend = true;

				if (true == isRight)
				{
					_int			iPreAnimIndex = { m_pBlackBoard->Get_Current_AnimIndex(CMonster::PART_BODY, PLAYING_INDEX::INDEX_1) };

					if (iPreAnimIndex == static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FL_A) ||
						iPreAnimIndex == static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FL_B) ||
						iPreAnimIndex == static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FL_C) ||
						iPreAnimIndex == static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FL_D) ||
						iPreAnimIndex == static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FL_E) ||
						iPreAnimIndex == static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FL_F))
					{
						CModel* pBody = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
						if (nullptr != pBody)
						{
							pBody->Reset_PreAnim_CurrentAnim(static_cast<_uint>(PLAYING_INDEX::INDEX_1));
						}
					}


					if (MOTION_TYPE::MOTION_A == eCurrentMotionType)
						iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FR_A);
					else if (MOTION_TYPE::MOTION_B == eCurrentMotionType)
						iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FR_B);
					else if (MOTION_TYPE::MOTION_C == eCurrentMotionType)
						iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FR_C);
					else if (MOTION_TYPE::MOTION_D == eCurrentMotionType)
						iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FR_D);
					else if (MOTION_TYPE::MOTION_E == eCurrentMotionType)
						iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FR_E);
					else if (MOTION_TYPE::MOTION_F == eCurrentMotionType)
						iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FR_F);

					_float			fCurrentWeight = { pBodyModel->Get_BlendWeight(static_cast<_uint>(PLAYING_INDEX::INDEX_1)) };
					fTurnBlendWeight = fminf(fAngleToTarget / XMConvertToRadians(180.f) * 2.f, 0.999f);

					m_fAccBlendTime += fTimeDelta;
					_float			fRatio = { fminf(m_fAccBlendTime / 0.2f, 1.f) };
					fTurnBlendWeight = fCurrentWeight + (fTurnBlendWeight - fCurrentWeight) * fRatio;
				}

				else
				{
					_int			iPreAnimIndex = { m_pBlackBoard->Get_Current_AnimIndex(CMonster::PART_BODY, PLAYING_INDEX::INDEX_1) };

					if (iPreAnimIndex == static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FR_A) ||
						iPreAnimIndex == static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FR_B) ||
						iPreAnimIndex == static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FR_C) ||
						iPreAnimIndex == static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FR_D) ||
						iPreAnimIndex == static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FR_E) ||
						iPreAnimIndex == static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FR_F))
					{
						CModel* pBody = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
						if (nullptr != pBody)
						{
							pBody->Reset_PreAnim_CurrentAnim(static_cast<_uint>(PLAYING_INDEX::INDEX_1));
						}
					}


					if (MOTION_TYPE::MOTION_A == eCurrentMotionType)
						iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FL_A);
					else if (MOTION_TYPE::MOTION_B == eCurrentMotionType)
						iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FL_B);
					else if (MOTION_TYPE::MOTION_C == eCurrentMotionType)
						iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FL_C);
					else if (MOTION_TYPE::MOTION_D == eCurrentMotionType)
						iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FL_D);
					else if (MOTION_TYPE::MOTION_E == eCurrentMotionType)
						iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FL_E);
					else if (MOTION_TYPE::MOTION_F == eCurrentMotionType)
						iBlendAnimIndex = static_cast<_int>(ANIM_ORDINARY_WALK::_TURNING_LOOP_FL_F);
				}

				_float			fCurrentWeight = { pBodyModel->Get_BlendWeight(static_cast<_uint>(PLAYING_INDEX::INDEX_1)) };
				fTurnBlendWeight = fminf(fAngleToTarget / XMConvertToRadians(90.f) * 2.f, 0.999f);

				m_fAccBlendTime += fTimeDelta;
				_float			fRatio = { fminf(m_fAccBlendTime / 0.2f, 1.f) };
				fTurnBlendWeight = fCurrentWeight + (fTurnBlendWeight - fCurrentWeight) * fRatio;
			}
		}
	}

	if (-1 == iResultAnimationIndex)
		return;

	//	루프애님이면서 이전 애니메이션과 같다면... 같은 애니메이션을 지속으로 루프 재생중이므로 보간이 필요없다고 판단
	if (true == isLoop && m_pBlackBoard->Get_Current_AnimIndex(CMonster::PART_BODY, PLAYING_INDEX::INDEX_0) == iResultAnimationIndex)
	{
		pBodyModel->Set_TotalLinearInterpolation(0.f);
	}

	//else
	//{
	//	pBodyModel->Set_TotalLinearInterpolation(0.8f);
	//}

	pBodyModel->Change_Animation(static_cast<_uint>(PLAYING_INDEX::INDEX_0), TEXT("Ordinary_Walk"), iResultAnimationIndex);
	pBodyModel->Set_Loop(static_cast<_uint>(PLAYING_INDEX::INDEX_0), isLoop);
	pBodyModel->Set_BoneLayer_PlayingInfo(static_cast<_uint>(PLAYING_INDEX::INDEX_0), strBaseBoneLayerTag);
	pBodyModel->Set_BlendWeight(static_cast<_uint>(PLAYING_INDEX::INDEX_0), 1.f - fTurnBlendWeight, 0.f);

	if (true == isNeedBlend)
	{
		pBodyModel->Change_Animation(static_cast<_uint>(PLAYING_INDEX::INDEX_1), TEXT("Ordinary_Walk"), iBlendAnimIndex);
		pBodyModel->Set_Loop(static_cast<_uint>(PLAYING_INDEX::INDEX_1), true);
		pBodyModel->Set_BoneLayer_PlayingInfo(static_cast<_uint>(PLAYING_INDEX::INDEX_1), strBlendBoneLayerTag);
		pBodyModel->Set_BlendWeight(static_cast<_uint>(PLAYING_INDEX::INDEX_1), fTurnBlendWeight, 0.f);

		if (1.0f != fTurnBlendWeight)
		{
			_float			fBlendTrackPosition = { pBodyModel->Get_TrackPosition(static_cast<_uint>(PLAYING_INDEX::INDEX_1)) };
			_float			fBaseTrackPosition = { pBodyModel->Get_TrackPosition(static_cast<_uint>(PLAYING_INDEX::INDEX_0)) };

			if (fBlendTrackPosition != fBaseTrackPosition)
			{
				fBlendTrackPosition = fBaseTrackPosition;
				pBodyModel->Set_TrackPosition(static_cast<_uint>(PLAYING_INDEX::INDEX_1), fBlendTrackPosition, true);
			}
		}		
			
		_float			fBaseTrackPos = { pBodyModel->Get_TrackPosition(static_cast<_uint>(PLAYING_INDEX::INDEX_0)) };
		_float			fBlendTrackPos = { pBodyModel->Get_TrackPosition(static_cast<_uint>(PLAYING_INDEX::INDEX_1)) };

		string			strBaseTrackPos = { to_string(fBaseTrackPos) + "Base Track"};
		string			strBlendTrackPos = { to_string(fBlendTrackPos) + "Blend Track"};

		m_isPreBlended = true;
	}

	else
	{
		if (true == m_isPreBlended)
		{
			pBodyModel->Set_BlendWeight(static_cast<_uint>(PLAYING_INDEX::INDEX_1), 0.f, 10.f);

			m_isPreBlended = false;
		}		

		else
		{
			_float			fWeight = { pBodyModel->Get_BlendWeight(static_cast<_uint>(PLAYING_INDEX::INDEX_1)) };
			if (fWeight <= 0.f)
				pBodyModel->Reset_PreAnim_CurrentAnim(static_cast<_uint>(PLAYING_INDEX::INDEX_1));
		}
	}

#pragma endregion
}

void CMove_To_Target_Zombie::Turn_To_TargetObject_Manual(_float fTimeDelta)
{
	if (nullptr == m_pTargetObject)
		return;

	_vector				vTargetPosition = { m_pTargetObject->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) };
	_vector				vZombiePosition = { m_pBlackBoard->Get_AI()->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) };

	_vector				vDirectionToTarget = { vTargetPosition - vZombiePosition };
	_vector				vDirectionToTargetXZPlane = { XMVectorSetY(vDirectionToTarget, 0.f) };
	
	_matrix				ZombieWorldMatrixInv = { m_pBlackBoard->Get_AI()->Get_Transform()->Get_WorldMatrix_Inverse() };
	_vector				vDirectionToWindowLocalXZPlane = { XMVector3TransformNormal(vDirectionToTargetXZPlane, ZombieWorldMatrixInv) };
	
	_bool				isRight = { XMVectorGetX(vDirectionToWindowLocalXZPlane) > 0.f };
	_float				fDot = { XMVectorGetX(XMVector3Dot(XMVectorSet(0.f, 0.f, 1.f, 0.f),XMVector3Normalize(vDirectionToWindowLocalXZPlane))) };
	_float				fAngle = { acosf(fDot) };

	if (fAngle > XMConvertToRadians(5.f))
	{
		if (false == isRight)
			fAngle *= -1.f;

		_matrix			ZombieWorldMatrix = { m_pBlackBoard->Get_AI()->Get_Transform()->Get_WorldMatrix() };
		_vector			vPosition = { ZombieWorldMatrix.r[CTransform::STATE_POSITION] };
		ZombieWorldMatrix.r[CTransform::STATE_POSITION] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
		
		_matrix			RotaionMatrix = { XMMatrixRotationY(XMConvertToRadians(180.f) * fTimeDelta) };
		ZombieWorldMatrix = ZombieWorldMatrix * RotaionMatrix;
		ZombieWorldMatrix.r[CTransform::STATE_POSITION] = vPosition;
		m_pBlackBoard->Get_AI()->Get_Transform()->Set_WorldMatrix(ZombieWorldMatrix);
	}
}

_bool CMove_To_Target_Zombie::Is_CurrentAnim_StartAnim(PLAYING_INDEX eIndex)
{
	_bool				isStartAnim = { false };

	CModel*				pBodyModel = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBodyModel)
		return isStartAnim;

	_int				iAnimIndex = { m_pBlackBoard->Get_Current_AnimIndex(CMonster::PART_BODY, eIndex) };
	if (-1 == iAnimIndex)
		return isStartAnim;

	wstring				strAnimLayerTag = { m_pBlackBoard->Get_Current_AnimLayerTag(CMonster::PART_BODY, eIndex) };
	if (TEXT("") == strAnimLayerTag)
		return isStartAnim;

	unordered_map<wstring, unordered_set<_uint>>::iterator			iterLayer = { m_StartAnimIndicesLayer.find(strAnimLayerTag) };
	if (iterLayer != m_StartAnimIndicesLayer.end())
	{
		unordered_set<_uint>::iterator			iterIndices = { iterLayer->second.find(iAnimIndex) };
		if (iterIndices != iterLayer->second.end())
		{
			isStartAnim = true;
		}
	}

	return isStartAnim;
}

_bool CMove_To_Target_Zombie::Is_CurrentAnim_LoopAnim(PLAYING_INDEX eIndex)
{
	_bool				isLoopAnim = { false };

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBodyModel)
		return isLoopAnim;

	_int				iAnimIndex = { m_pBlackBoard->Get_Current_AnimIndex(CMonster::PART_BODY, eIndex) };
	if (-1 == iAnimIndex)
		return isLoopAnim;

	wstring				strAnimLayerTag = { m_pBlackBoard->Get_Current_AnimLayerTag(CMonster::PART_BODY, eIndex) };
	if (TEXT("") == strAnimLayerTag)
		return isLoopAnim;

	unordered_map<wstring, unordered_set<_uint>>::iterator			iterLayer = { m_LoopAnimIndicesLayer.find(strAnimLayerTag) };
	if (iterLayer != m_LoopAnimIndicesLayer.end())
	{
		unordered_set<_uint>::iterator			iterIndices = { iterLayer->second.find(iAnimIndex) };
		if (iterIndices != iterLayer->second.end())
		{
			isLoopAnim = true;
		}
	}

	return isLoopAnim;
}

_int CMove_To_Target_Zombie::Compute_Base_Anim()
{

	return -1;
}

_int CMove_To_Target_Zombie::Compute_Blend_Anim()
{

	return -1;
}

CMove_To_Target_Zombie* CMove_To_Target_Zombie::Create(void* pArg)
{
	CMove_To_Target_Zombie*			pInstance = { new CMove_To_Target_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CMoveTo_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMove_To_Target_Zombie::Free()
{
	__super::Free();

	Safe_Release(m_pTargetObject);
}
