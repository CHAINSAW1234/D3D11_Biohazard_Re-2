#include "stdafx.h"
#include "GameInstance.h"
#include "Knock_Door_Zombie.h"
#include "BlackBoard_Zombie.h"

#include "Door.h"
#include "Player.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CKnock_Door_Zombie::CKnock_Door_Zombie()
	: CTask_Node()
{
}

CKnock_Door_Zombie::CKnock_Door_Zombie(const CKnock_Door_Zombie& rhs)
{
}

HRESULT CKnock_Door_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CKnock_Door_Zombie::Enter()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*				pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	CDoor*				pDoor = { m_pBlackBoard->Get_Target_Door() };
	if (nullptr == pDoor)
		return;

	pDoor->Set_Knock(true);

	_float3				vDirectionFromDoorLocalFloat3 = {};
	if (false == m_pBlackBoard->Compute_Direction_From_Target_Local(pDoor, &vDirectionFromDoorLocalFloat3))
		return;

	_bool				isDoorsFront = { vDirectionFromDoorLocalFloat3.z > 0.f };
	if (false == isDoorsFront)
	{
		m_eAnimType = ANIM_TYPE::_A;
	}

	else
	{
		m_eAnimType = ANIM_TYPE::_B;
	}

	_int			iAnimIndex = { -1 };
	if (m_eAnimType == ANIM_TYPE::_A)
	{
		iAnimIndex = static_cast<_int>(ANIM_GIMMICK_DOOR::_KNOCK_FROM_A_START);
	}

	else if (m_eAnimType == ANIM_TYPE::_B)
	{
		iAnimIndex = static_cast<_int>(ANIM_GIMMICK_DOOR::_KNOCK_FROM_B_START);
	}

	if (false == m_pBlackBoard->Compute_DeltaMatrix_AnimFirstKeyFrame_From_Target(pDoor->Get_Transform(), static_cast<_uint>(m_eBasePlayingIndex), iAnimIndex, m_strAnimLayerTag, &m_InterpolateDeltaMatrix))
		return;

	/*_matrix				TargetDoorWorldMatrix = { pDoor->Get_Transform()->Get_WorldMatrix() };
	_matrix				Zombie_WorldMatrix = { m_pBlackBoard->Get_AI()->Get_Transform()->Get_WorldMatrix() };
	
	_vector				vTargetScale, vTargetQuaternion, vTargetTranslation;
	_vector				vZombieScale, vZombieQuaternion, vZombieTranslation;
	
	XMMatrixDecompose(&vTargetScale, &vTargetQuaternion, &vTargetTranslation, TargetDoorWorldMatrix);
	XMMatrixDecompose(&vZombieScale, &vZombieQuaternion, &vZombieTranslation, Zombie_WorldMatrix);

	_matrix				NewWorldMatrix = { XMMatrixAffineTransformation(vZombieScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vTargetQuaternion, vTargetTranslation) };
	m_pBlackBoard->Get_AI()->Get_Transform()->Set_WorldMatrix(NewWorldMatrix);*/

	pBodyModel->Set_TotalLinearInterpolation(0.3f);

	m_pBlackBoard->Get_AI()->Set_ManualMove(true);
	m_fAccLinearInterpolateTime = 0.f;
	m_eAnimState = ANIM_STATE::_START;

#ifdef _DEBUG

	cout << "Enter Knock Door" << endl;

#endif 
}

_bool CKnock_Door_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	CDoor*				pDoor = { m_pBlackBoard->Get_Target_Door() };
	if (nullptr == pDoor)
		return false;

	//	필요 조건 => 문 안열리고 막타이상이어야함 ( HP1 초과... ), 문이 잠기지않음
	_int				iHpDoor = { pDoor->Get_HP() };
	_bool				isDoorCanKnock = { 1 < iHpDoor };
	if (false == isDoorCanKnock)
		return false;

	MONSTER_STATE		ePreMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };
	if (ePreMonsterState == MONSTER_STATE::MST_KNOCK_DOOR)
	{
		if (true == m_pBlackBoard->Get_AI()->Is_In_Location(static_cast<LOCATION_MAP_VISIT>(m_pBlackBoard->Get_Player()->Get_Player_Region())))
			return false;

		CModel* pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
		if (nullptr == pBody_Model)
			return false;

		_int			iCurrentAnimIndex = { pBody_Model->Get_CurrentAnimIndex(static_cast<_uint>(m_eBasePlayingIndex)) };
		wstring			strCurrentAnimLayerTag = { pBody_Model->Get_CurrentAnimLayerTag(static_cast<_uint>(m_eBasePlayingIndex)) };

		_bool			isAnimFinished = { pBody_Model->isFinished(static_cast<_uint>(m_eBasePlayingIndex)) };
		_bool			isSameLayer = { strCurrentAnimLayerTag == m_strAnimLayerTag };
		_bool			isStartAnimINdex = {
		static_cast<_int>(ANIM_GIMMICK_DOOR::_KNOCK_FROM_A_START) == iCurrentAnimIndex ||
		static_cast<_int>(ANIM_GIMMICK_DOOR::_KNOCK_FROM_B_START) == iCurrentAnimIndex
		};
		_bool			isLoopAnimINdex = {
			static_cast<_int>(ANIM_GIMMICK_DOOR::_KNOCK_FROM_A_LOOP) == iCurrentAnimIndex ||
			static_cast<_int>(ANIM_GIMMICK_DOOR::_KNOCK_FROM_B_LOOP) == iCurrentAnimIndex
		};
		_bool			isFInishAnimInex = {
		static_cast<_int>(ANIM_GIMMICK_DOOR::_KNOCK_FROM_A_END) == iCurrentAnimIndex ||
		static_cast<_int>(ANIM_GIMMICK_DOOR::_KNOCK_FROM_B_END) == iCurrentAnimIndex
		};

		if (isSameLayer && isStartAnimINdex)
		{
			if (true == isAnimFinished)
			{
				m_eAnimState = ANIM_STATE::_LOOP;				

				pBody_Model->Set_TotalLinearInterpolation(0.f);
			}
		}

		else if (isSameLayer && isLoopAnimINdex)
		{

			//	해당처리로하면 마지막에 제자리 돌아가는 모션임 지쳤을때로 표현해야할듯?
			/*_int			iDoorHP = { pDoor->Get_HP() };
			if (2 == iDoorHP)
			{
				m_eAnimState = ANIM_STATE::_FINISH;
			}

			else
			{
				m_fAccKnockTime += fTimeDelta;
				if (m_fAccKnockTime >= ZOMBIE_ATTACK_DOOR_NEED_TIME)
				{
					m_fAccKnockTime -= ZOMBIE_ATTACK_DOOR_NEED_TIME;
					pDoor->Attack_Prop();
				}
			}*/

			/*m_fAccKnockTime += fTimeDelta;
			if (m_fAccKnockTime >= ZOMBIE_ATTACK_DOOR_NEED_TIME)
			{
				m_fAccKnockTime -= ZOMBIE_ATTACK_DOOR_NEED_TIME;
				pDoor->Attack_Prop(m_pBlackBoard->Get_AI()->Get_Transform());
			}*/

			_float			fTrackPosition = { pBody_Model->Get_TrackPosition(static_cast<_uint>(m_eBasePlayingIndex)) };
			if (ZOMBIE_DOOR_HIT_TRACK_POSITION_1 > fTrackPosition)
			{
				m_LoopTrack_Event_Level = LOOP_TRACK_EV_LEVEL::_END;
			}

			else if (LOOP_TRACK_EV_LEVEL::_END == m_LoopTrack_Event_Level)
			{
				if (fTrackPosition >= ZOMBIE_DOOR_HIT_TRACK_POSITION_1)
				{
					m_LoopTrack_Event_Level = LOOP_TRACK_EV_LEVEL::_HIT_1;
					pDoor->Attack_Prop(m_pBlackBoard->Get_AI()->Get_Transform());
					m_pBlackBoard->Get_AI()->Play_Random_Knock_Door_Sound();
				}
			}

			else if (LOOP_TRACK_EV_LEVEL::_HIT_1 == m_LoopTrack_Event_Level)
			{
				if (fTrackPosition >= ZOMBIE_DOOR_HIT_TRACK_POSITION_2)
				{
					m_LoopTrack_Event_Level = LOOP_TRACK_EV_LEVEL::_HIT_2;
					pDoor->Attack_Prop(m_pBlackBoard->Get_AI()->Get_Transform());
					m_pBlackBoard->Get_AI()->Play_Random_Knock_Door_Sound();
				}
			}
		}

		else if (isSameLayer && isFInishAnimInex)
		{
			if (true == isAnimFinished)
			{
				m_eAnimState = ANIM_STATE::_END;
				pDoor->Attack_Prop(m_pBlackBoard->Get_AI()->Get_Transform());
				m_pBlackBoard->Get_AI()->Play_Random_Knock_Door_Sound();
				return false;
			}
		}
	}

	else
	{
		if (true == pDoor->Is_Lock())
			return false;
	}

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_KNOCK_DOOR);

	if (m_fAccLinearInterpolateTime < ZOMBIE_DOOR_KNOCK_TOTAL_INTERPOLATE_TO_WINDOW_TIME)
	{
		_bool				isComplete = { m_fAccLinearInterpolateTime >= ZOMBIE_DOOR_KNOCK_TOTAL_INTERPOLATE_TO_WINDOW_TIME };
		if (false == isComplete)
		{
			_float				fTime = fTimeDelta;
			m_fAccLinearInterpolateTime += fTime;

			if (m_fAccLinearInterpolateTime >= ZOMBIE_DOOR_KNOCK_TOTAL_INTERPOLATE_TO_WINDOW_TIME)
			{
				fTime -= m_fAccLinearInterpolateTime - ZOMBIE_DOOR_KNOCK_TOTAL_INTERPOLATE_TO_WINDOW_TIME;
			}
			_float				fRatio = { fTime / ZOMBIE_DOOR_KNOCK_TOTAL_INTERPOLATE_TO_WINDOW_TIME };

			if (false == isComplete)
			{
				if (false == m_pBlackBoard->Apply_Devide_Delta_Matrix(fRatio, XMLoadFloat4x4(&m_InterpolateDeltaMatrix)))
					return false;
			}
		}
	}

	Change_Animation(fTimeDelta);

	return true;
}

void CKnock_Door_Zombie::Exit()
{
	if (nullptr == m_pBlackBoard)
		return;

	CDoor* pDoor = { m_pBlackBoard->Get_Target_Door() };
	if (nullptr != pDoor)
	{
		pDoor->Set_Knock(false);
	}

	CModel*				pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr != pBody_Model)
	{
		pBody_Model->Set_TotalLinearInterpolation(0.3f);
	}
}

void CKnock_Door_Zombie::Change_Animation(_float fTimeDelta)
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*			pBody_Model = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBody_Model)
		return;

	_int			iResultAnimationIndex = { -1 };
	_bool			isLoop = { false };

	if (ANIM_STATE::_START == m_eAnimState)
	{
		if (ANIM_TYPE::_A == m_eAnimType)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_DOOR::_KNOCK_FROM_A_START);
		}

		else if (ANIM_TYPE::_B == m_eAnimType)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_DOOR::_KNOCK_FROM_B_START);
		}

		isLoop = false;
	}

	else if (ANIM_STATE::_LOOP == m_eAnimState)
	{
		if (ANIM_TYPE::_A == m_eAnimType)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_DOOR::_KNOCK_FROM_A_LOOP);
		}

		else if (ANIM_TYPE::_B == m_eAnimType)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_DOOR::_KNOCK_FROM_B_LOOP);
		}

		isLoop = true;
	}

	else if (ANIM_STATE::_FINISH == m_eAnimState)
	{
		if (ANIM_TYPE::_A == m_eAnimType)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_DOOR::_KNOCK_FROM_A_END);
		}

		else if (ANIM_TYPE::_B == m_eAnimType)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_DOOR::_KNOCK_FROM_B_END);
		}

		isLoop = false;
	}

	if (-1 == iResultAnimationIndex)
		return;

	pBody_Model->Change_Animation(static_cast<_uint>(m_eBasePlayingIndex), m_strAnimLayerTag, iResultAnimationIndex);
	pBody_Model->Set_Loop(static_cast<_uint>(m_eBasePlayingIndex), isLoop);

#pragma endregion
}

CKnock_Door_Zombie* CKnock_Door_Zombie::Create(void* pArg)
{
	CKnock_Door_Zombie*				pInstance = { new CKnock_Door_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CKnock_Door_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKnock_Door_Zombie::Free()
{
	__super::Free();
}
