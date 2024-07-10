#include "stdafx.h"
#include "GameInstance.h"
#include "Hold_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CHold_Zombie::CHold_Zombie()
	: CTask_Node()
{
}

CHold_Zombie::CHold_Zombie(const CHold_Zombie& rhs)
{
}

HRESULT CHold_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CHold_Zombie::Enter()
{
	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	pBodyModel->Set_TotalLinearInterpolation(0.2f);
	pBodyModel->Set_Loop(static_cast<_uint>(m_eBasePlayingIndex), false);

	pBodyModel->Set_BlendWeight(static_cast<_uint>(m_eBlendPlayingIndex), 0.f, 0.2f);
	pBodyModel->Reset_PreAnim_CurrentAnim(static_cast<_uint>(m_eBlendPlayingIndex));

#ifdef _DEBUG

	cout << "Enter Hold " << endl;

#endif 
}

_bool CHold_Zombie::Execute(_float fTimeDelta)
{
	return false;
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	MONSTER_STATE			eMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };
	_bool					isEntry = { eMonsterState != MST_HOLD };

	if (true == isEntry)
	{
		DIRECTION			eDirectionToPlayer = { DIRECTION::_END };
		if (false == m_pBlackBoard->Compute_Direction_To_Player_8Direction_Local(&eDirectionToPlayer))
			return false;

		if (DIRECTION::_BL == eDirectionToPlayer ||
			DIRECTION::_BR == eDirectionToPlayer ||
			DIRECTION::_FL == eDirectionToPlayer ||
			DIRECTION::_FR == eDirectionToPlayer)
			return false;

		if (MONSTER_STATE::MST_STANDUP == eMonsterState ||
			MONSTER_STATE::MST_TURNOVER == eMonsterState)
			return false;

		CMonster::MONSTER_STATUS*		pStatus = { m_pBlackBoard->Get_AI()->Get_Status_Ptr() };
		if (nullptr == pStatus)
			return false;

		CZombie::POSE_STATE				ePoseState = { m_pBlackBoard->Get_AI()->Get_PoseState() };
		if (CZombie::POSE_STATE::_CREEP == ePoseState)
		{
			_float3				vDirectionToPlayerLocalFloat3 = {};
			if (false == m_pBlackBoard->Compute_Direction_To_Player_Local(&vDirectionToPlayerLocalFloat3))
				return false;

			_float				fAngleToTarget = { XMVectorGetX(XMVector3Dot(XMVector3Normalize(XMLoadFloat3(&vDirectionToPlayerLocalFloat3)), XMVectorSet(0.f, 0.f, 1.f, 0.f))) };
			if (ZOMBIE_CREEP_HOLD_MAX_ANGLE < fAngleToTarget)
				return false;

			_bool				isFront = { vDirectionToPlayerLocalFloat3.z > 0.f };
			_bool				isRight = { vDirectionToPlayerLocalFloat3.x > 0.f };			

			CZombie::FACE_STATE			eFaceState = { m_pBlackBoard->Get_AI()->Get_FaceState() };
			if (CZombie::FACE_STATE::_DOWN == eFaceState)
			{
				if (false == isFront)
					return false;
			}

			else if (CZombie::FACE_STATE::_UP == eFaceState)
			{
				if (true == isFront)
					return false;
			}
		}

		if (pStatus->fAccHoldTime < pStatus->fTryHoldTime)
			return false;

		else
		{
			Change_Animation();	
			pStatus->fAccHoldTime = 0.f;
		}
	}

	else
	{
		if (true == Is_StateFinished())
			return false;
	}

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_HOLD);

	Apply_Additional_Turn(fTimeDelta);

	return true;
}

void CHold_Zombie::Exit()
{
}

void CHold_Zombie::Change_Animation()
{
	CZombie::POSE_STATE			ePoseState = { m_pBlackBoard->Get_AI()->Get_PoseState() };
	if (CZombie::POSE_STATE::_UP == ePoseState)
	{
		Change_Animation_StandUp();

	}

	else if(CZombie::POSE_STATE::_CREEP == ePoseState)
	{
		Change_Animation_Creep();
	}

#ifdef _DEBUG
	else
	{
		MSG_BOX(TEXT("Called : void CHold_Zombie::Change_Animation() 좀비 담당자 호출"));
	}
#endif
}

void CHold_Zombie::Change_Animation_StandUp()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	DIRECTION		eDirection = { DIRECTION::_END };
	if (false == m_pBlackBoard->Compute_Direction_To_Player_4Direction_Local(&eDirection))
		return;

	_int			iResultAnimationIndex = { -1 };
	wstring			strAnimLayerTag = { TEXT("Ordinary_Hold") };

	if (DIRECTION::_F == eDirection)
	{
		_int			iRandomAnimIndex = { m_pGameInstance->GetRandom_Int(static_cast<_int>(ANIM_ORDINARY_HOLD::_F1), static_cast<_int>(ANIM_ORDINARY_HOLD::_F2)) };

		iResultAnimationIndex = iRandomAnimIndex;
	}

	else if (DIRECTION::_B == eDirection)
	{
		_float3				vDirectionToPlayerLocalFloat3;
		if (false == m_pBlackBoard->Compute_Direction_To_Player_Local(&vDirectionToPlayerLocalFloat3))
		{
			return;
		}
		_vector				vDirectionToPlayerLocal = { XMLoadFloat3(&vDirectionToPlayerLocalFloat3) };
		_bool				isRight = { 0.f < XMVectorGetX(vDirectionToPlayerLocal) };

		if (true == isRight)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_HOLD::_R180);
		}

		else
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_HOLD::_L180);
		}
	}

	else if (DIRECTION::_R == eDirection)
	{
		iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_HOLD::_R90);
	}

	else if (DIRECTION::_L == eDirection)
	{
		iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_HOLD::_L90);
	}

	if (-1 == iResultAnimationIndex)
		return;

	pBodyModel->Change_Animation(static_cast<_uint>(m_eBasePlayingIndex), m_strStandUpAnimLayerTag, iResultAnimationIndex);
	pBodyModel->Set_BoneLayer_PlayingInfo(static_cast<_uint>(m_eBasePlayingIndex), m_strBoneLayerTag);
}

void CHold_Zombie::Change_Animation_Creep()
{
	if (nullptr == m_pBlackBoard)
		return;

	_float3				vDirectionToPlayerLocalFloat3 = {};
	if (false == m_pBlackBoard->Compute_Direction_To_Player_Local(&vDirectionToPlayerLocalFloat3))
		return;

	_bool				isFront = { vDirectionToPlayerLocalFloat3.z > 0.f };
	_bool				isRight = { vDirectionToPlayerLocalFloat3.x > 0.f };

	CZombie::FACE_STATE			eFaceState = { m_pBlackBoard->Get_AI()->Get_FaceState() };

	CModel*			pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_int			iResultAnimationIndex = { -1 };

	if (CZombie::FACE_STATE::_DOWN == eFaceState)
	{
		if (false == isFront)
			return;

		if (true == isRight)
			iResultAnimationIndex = static_cast<_uint>(ANIM_LOST_HOLD::_1);

		else
			iResultAnimationIndex = static_cast<_uint>(ANIM_LOST_HOLD::_0);
	}

	else if (CZombie::FACE_STATE::_UP == eFaceState)
	{
		if (true == isFront)
			return;

		if (true == isRight)
			iResultAnimationIndex = static_cast<_uint>(ANIM_LOST_HOLD::_FACEUP2);

		else
			iResultAnimationIndex = static_cast<_uint>(ANIM_LOST_HOLD::_FACEUP1);

		m_pBlackBoard->Get_AI()->Set_FaceState(CZombie::FACE_STATE::_DOWN);
	}

	if (-1 == iResultAnimationIndex)
		return;

	pBodyModel->Change_Animation(static_cast<_uint>(m_eBasePlayingIndex), m_strCreepAnimLayerTag, iResultAnimationIndex);
	pBodyModel->Set_BoneLayer_PlayingInfo(static_cast<_uint>(m_eBasePlayingIndex), m_strBoneLayerTag);
}

_bool CHold_Zombie::Is_StateFinished()
{
	_bool					isFinished = { false };
	
	CModel*					pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBody_Model)
		return isFinished;

	isFinished = pBody_Model->isFinished(static_cast<_uint>(m_eBasePlayingIndex));

	return isFinished;
}

void CHold_Zombie::Apply_Additional_Turn(_float fTimeDelta)
{
	CTransform*				pZombieTransform = { m_pBlackBoard->Get_AI()->Get_Transform() };
	if (nullptr == pZombieTransform)
		return;

	CModel*					pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBody_Model)
		return;

	_float					fCurrentAnimDuration = { pBody_Model->Get_Duration_From_PlayingInfo(static_cast<_uint>(m_eBasePlayingIndex)) };
	_float					fCurrentTrackPosition = { pBody_Model->Get_TrackPosition(static_cast<_uint>(m_eBasePlayingIndex)) };
	
	if (fCurrentAnimDuration * 0.5f < fCurrentTrackPosition)
		return;
	
	_matrix					ZobmieWorldMatrix = { pZombieTransform->Get_WorldMatrix() };
	_float3					vDirectionToPlayerLocalFloat3 = {};
	if (false == m_pBlackBoard->Compute_Direction_To_Player_Local(&vDirectionToPlayerLocalFloat3))
		return;

	_vector					vDirectionToPlayerLocal = { XMVector3Normalize(XMLoadFloat3(&vDirectionToPlayerLocalFloat3)) };
	_vector					vAbsoluteLookDir = { XMVectorSet(0.f, 0.f, 1.f, 0.f) };

	_float					fDot = { XMVectorGetX(XMVector3Dot(vDirectionToPlayerLocal, vAbsoluteLookDir)) };
	_float					fDeltaAngle = { acosf(fDot) };
	_float					fMaxAngle = { fTimeDelta * ZOMBIE_HOLD_ADDITIONAL_ROTATE_PER_SEC };

	_float					fResultAngle = { fminf(fDeltaAngle, fMaxAngle) };

	if (XMVectorGetX(vDirectionToPlayerLocal) < 0.f)
	{
		fResultAngle *= -1.f;
	}

	_vector					vZombieUpDir = { pZombieTransform->Get_State_Vector(CTransform::STATE_UP) };
	_matrix					RotitonMatrix = { XMMatrixRotationAxis(XMVector3Normalize(vZombieUpDir), fResultAngle) };

	_vector					vZombiePosition = { ZobmieWorldMatrix.r[CTransform::STATE_POSITION] };
	ZobmieWorldMatrix.r[CTransform::STATE_POSITION] = { XMVectorZero() };
	_matrix					ResultWorldMatrix = { ZobmieWorldMatrix * RotitonMatrix };
	ResultWorldMatrix.r[CTransform::STATE_POSITION] = vZombiePosition;
	
	pZombieTransform->Set_WorldMatrix(ResultWorldMatrix);
}

CHold_Zombie* CHold_Zombie::Create(void* pArg)
{
	CHold_Zombie* pInstance = { new CHold_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHold_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHold_Zombie::Free()
{
	__super::Free();
}
