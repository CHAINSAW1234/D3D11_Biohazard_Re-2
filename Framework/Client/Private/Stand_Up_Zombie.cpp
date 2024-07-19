#include "stdafx.h"
#include "GameInstance.h"
#include "Stand_Up_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CStand_Up_Zombie::CStand_Up_Zombie()
	: CTask_Node()
{
}

CStand_Up_Zombie::CStand_Up_Zombie(const CStand_Up_Zombie& rhs)
{
}

HRESULT CStand_Up_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CStand_Up_Zombie::Enter()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	pBodyModel->Set_TotalLinearInterpolation(0.4f);
	pBodyModel->Set_Loop(static_cast<_uint>(PLAYING_INDEX::INDEX_0), false);

	m_pBlackBoard->Get_AI()->Stop_Sound(0);
	m_pBlackBoard->Get_AI()->Stop_Sound(1);
	_int			iRandom = { m_pGameInstance->GetRandom_Int(0, 3) };
	if (0 == iRandom)
		m_pBlackBoard->Get_AI()->Change_Sound(TEXT("em0_media.bnk.2_22.mp3"), 0);
	if (1 == iRandom)
		m_pBlackBoard->Get_AI()->Change_Sound(TEXT("em0_media.bnk.2_35.mp3"), 0);
	if (2 == iRandom)
		m_pBlackBoard->Get_AI()->Change_Sound(TEXT("em0_media.bnk.2_48.mp3"), 0);
	if (3 == iRandom)
		m_pBlackBoard->Get_AI()->Change_Sound(TEXT("em0_media.bnk.2_33.mp3"), 0);

	m_pBlackBoard->Get_AI()->Change_Sound(TEXT("em0_media.bnk.2_115.mp3"), 0);


#ifdef _DEBUG

	cout << "Enter StandUp " << endl;

#endif 
}

_bool CStand_Up_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	if (m_pBlackBoard->Get_AI()->Get_Current_MonsterState() == MONSTER_STATE::MST_STANDUP)
	{
		_bool			isFinsihed = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY)->isFinished(static_cast<_uint>(m_ePlayingIndex)) };
		if (true == isFinsihed)
		{
			m_pBlackBoard->Get_ZombieStatus_Ptr()->fAccCreepTime = 0.f;
			m_pBlackBoard->Get_AI()->Set_PoseState(CZombie::POSE_STATE::_UP);
			m_eFaceState = CZombie::FACE_STATE::_END;
			return false;
		}
	}

	else
	{
		if (CZombie::POSE_STATE::_CREEP != m_pBlackBoard->Get_AI()->Get_PoseState())
			return false;

		if (true == m_pBlackBoard->Is_Break_L_Leg() ||
			true == m_pBlackBoard->Is_Break_R_Leg())
			return false;

		if (true == m_pBlackBoard->Is_Break_L_Humerous() &&
			true == m_pBlackBoard->Is_Break_R_Humerous())
			return false;

		if (m_pBlackBoard->Get_ZombieStatus_Ptr()->fAccCreepTime < m_pBlackBoard->Get_ZombieStatus_Ptr()->fTryStandUpTime)
			return false;

		if (false == m_pBlackBoard->Get_AI()->Use_Stamina(CZombie::USE_STAMINA::_STAND_UP))
			return false;

		if (false == m_pBlackBoard->Is_LookTarget())
			return false;

		m_eFaceState = m_pBlackBoard->Get_AI()->Get_FaceState();
		Change_Animation();
	}

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_STANDUP);

	return true;
}

void CStand_Up_Zombie::Exit()
{

}

void CStand_Up_Zombie::Change_Animation()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*			pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	wstring			strPreAnimLayerTag = { pBodyModel->Get_CurrentAnimLayerTag(static_cast<_uint>(m_ePlayingIndex)) };
	if (strPreAnimLayerTag == TEXT("Ordinary_StandUp"))
		return;

	_int			iResultAnimationIndex = { -1 };
	wstring			strBoneLayerTag = { BONE_LAYER_DEFAULT_TAG };
	wstring			strAnimLayerTag = { TEXT("Ordinary_StandUp") };

	_float3			vDirectionToPlayerFloat3 = { };

	if (false == m_pBlackBoard->Compute_Direction_To_Player_Local(&vDirectionToPlayerFloat3))
		return;

	_bool			isCanLookPlayer = { false };
	_float			fDistanceToPlayer = {};
	if (true == m_pBlackBoard->Compute_Distance_To_Player_World(&fDistanceToPlayer))
	{
		isCanLookPlayer = STATUS_ZOMBIE_DEFAULT_RECOGNITION_DISTANCE > fDistanceToPlayer;
	}

	if (true == isCanLookPlayer)
	{
		_vector			vDirectionToPlayerLocal = { XMLoadFloat3(&vDirectionToPlayerFloat3) };
		_vector			vDirectionToPlayerLocalXZPlane = { XMVectorSetY(vDirectionToPlayerLocal, 0.f) };

		_bool			isRight = { 0.f < XMVectorGetX(vDirectionToPlayerLocalXZPlane) };
		_bool			isFront = { 0.f < XMVectorGetZ(vDirectionToPlayerLocalXZPlane) };
		_bool			isBigXAxis = { fabsf(XMVectorGetX(vDirectionToPlayerLocalXZPlane)) > fabsf(XMVectorGetZ(vDirectionToPlayerLocalXZPlane)) };

		if (CZombie::FACE_STATE::_UP == m_eFaceState)
		{
			if (true == isBigXAxis)
			{
				if (true == isRight)
				{
					iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_STANDUP::_FACE_UP_R);
				}

				else
				{
					iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_STANDUP::_FACE_UP_L);
				}
			}

			else
			{
				if (true == isFront)
				{
					iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_STANDUP::_FACE_UP_F);
				}

				else
				{
					iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_STANDUP::_FACE_UP_B);
				}
			}
		}

		else if (CZombie::FACE_STATE::_DOWN == m_eFaceState)
		{
			if (true == isBigXAxis)
			{
				if (true == isRight)
				{
					iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_STANDUP::_FACE_DOWN_R);
				}

				else
				{
					iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_STANDUP::_FACE_DOWN_L);
				}
			}

			else
			{
				if (true == isFront)
				{
					iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_STANDUP::_FACE_DOWN_F);
				}

				else
				{
					iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_STANDUP::_FACE_DOWN_B);
				}
			}
		}

#ifdef _DEBUG
		else { MSG_BOX(TEXT("Called : void CStand_Up_Zombie::Change_Animation() 좀비 담당자 호출"));	}
#endif
	}

	else
	{
		if (CZombie::FACE_STATE::_UP== m_eFaceState)
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_STANDUP::_FACE_UP_F);

		else if (CZombie::FACE_STATE::_DOWN == m_eFaceState)
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_STANDUP::_FACE_DOWN_F);

#ifdef _DEBUG
		else { MSG_BOX(TEXT("Called : void CStand_Up_Zombie::Change_Animation() 좀비 담당자 호출"));	}
#endif
	}	

	if (-1 == iResultAnimationIndex)
		return;

	pBodyModel->Change_Animation(static_cast<_uint>(m_ePlayingIndex), strAnimLayerTag, iResultAnimationIndex);
	pBodyModel->Set_BoneLayer_PlayingInfo(static_cast<_uint>(m_ePlayingIndex), strBoneLayerTag);
}

_bool CStand_Up_Zombie::Condition_Check()
{
	return _bool();
}

CStand_Up_Zombie* CStand_Up_Zombie::Create(void* pArg)
{
	CStand_Up_Zombie* pInstance = { new CStand_Up_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CStand_Up_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStand_Up_Zombie::Free()
{
	__super::Free();
}
