#include "stdafx.h"
#include "GameInstance.h"
#include "Rub_Door_Zombie.h"
#include "BlackBoard_Zombie.h"

#include "Door.h"
#include "Player.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CRub_Door_Zombie::CRub_Door_Zombie()
	: CTask_Node()
{
}

CRub_Door_Zombie::CRub_Door_Zombie(const CRub_Door_Zombie& rhs)
{
}

HRESULT CRub_Door_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CRub_Door_Zombie::Enter()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*				pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	CDoor*				pDoor = { m_pBlackBoard->Get_Target_Door() };
	if (nullptr == pDoor)
		return;

	_float3				vDirectionFromDoorLocalFloat3 = {};
	if (false == m_pBlackBoard->Compute_Direction_From_Target_Local(pDoor, &vDirectionFromDoorLocalFloat3))
		return;

	_bool				isDoorsFront = { vDirectionFromDoorLocalFloat3.z > 0.f };
	_int				iAnimIndex = { -1 };
	if (false == isDoorsFront)
	{
		iAnimIndex = static_cast<_int>(ANIM_GIMMICK_DOOR::_WINDOW_TO_RUB_FROM_A_START);
	}

	else
	{
		iAnimIndex = static_cast<_int>(ANIM_GIMMICK_DOOR::_WINDOW_TO_RUB_FROM_B_START);
	}

	if (false == m_pBlackBoard->Compute_DeltaMatrix_AnimFirstKeyFrame_From_Target(pDoor->Get_Transform(), static_cast<_uint>(m_eBasePlayingIndex), iAnimIndex, m_strAnimLayerTag, &m_InterpolateDeltaMatrix))
	{
#ifdef _DEBUG 
		MSG_BOX(TEXT("Called void COpen_Door_Zombie::Enter() Failed Compute Delta Matrix"));
#endif 
		return;
	}

	m_pBlackBoard->Get_AI()->Set_ManualMove(true);
	m_fAccLinearInterpolateTime = 0.f;
	m_eAnimState = ANIM_STATE::_START;

#ifdef _DEBUG

	cout << "Enter Rub Door" << endl;

#endif 
}

_bool CRub_Door_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	//	문이 잠겨있어야함
	CDoor*			pDoor = { m_pBlackBoard->Get_Target_Door() };
	if (nullptr == pDoor)
		return false;

	MONSTER_STATE		ePreMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };
	if (MONSTER_STATE::MST_RUB_DOOR == ePreMonsterState)
	{
		CModel* pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
		if (nullptr == pBody_Model)
			return false;

		_int			iCurrentAnimIndex = { pBody_Model->Get_CurrentAnimIndex(static_cast<_uint>(m_eBasePlayingIndex)) };
		wstring			strCurrentAnimLayerTag = { pBody_Model->Get_CurrentAnimLayerTag(static_cast<_uint>(m_eBasePlayingIndex)) };

		_bool			isAnimFinished = { pBody_Model->isFinished(static_cast<_uint>(m_eBasePlayingIndex)) };
		_bool			isSameLayer = { strCurrentAnimLayerTag == m_strAnimLayerTag };
		_bool			isStartAnimINdex = {
		static_cast<_int>(ANIM_GIMMICK_WINDOW::_HOLD_OUT_HAND_START1) == iCurrentAnimIndex ||
		static_cast<_int>(ANIM_GIMMICK_WINDOW::_HOLD_OUT_HAND_START2) == iCurrentAnimIndex
		};
		_bool			isFInishAnimInex = {
			static_cast<_int>(ANIM_GIMMICK_WINDOW::_HOLD_OUT_HAND_END1) == iCurrentAnimIndex ||
			static_cast<_int>(ANIM_GIMMICK_WINDOW::_HOLD_OUT_HAND_END2) == iCurrentAnimIndex
		};
		_bool			isLoopAnimINdex = {
		static_cast<_int>(ANIM_GIMMICK_WINDOW::_HOLD_OUT_HAND_LOOP1) == iCurrentAnimIndex ||
		static_cast<_int>(ANIM_GIMMICK_WINDOW::_HOLD_OUT_HAND_LOOP2) == iCurrentAnimIndex
		};

		if (isSameLayer && isStartAnimINdex)
		{
			if (true == isAnimFinished)
			{
				m_eAnimState = ANIM_STATE::_LOOP;
			}
		}

		else if (isSameLayer && isLoopAnimINdex)
		{
			_int			iDoorHP = { pDoor->Get_HP() };
			if (2 == iDoorHP)
			{
				m_eAnimState = ANIM_STATE::_FINISH;
			}

			else
			{
				_float			fDistanceToPlayer = {};
				if (false == m_pBlackBoard->Compute_Distance_To_Player(&fDistanceToPlayer))
					return false;

				_bool			isInRange = { ZOMBIE_RUB_DOOR_RECOGNITION_RANGE >= fDistanceToPlayer };
				if (true == isInRange)
					m_eAnimState = ANIM_STATE::_FINISH;
			}
		}

		else if (isSameLayer && isFInishAnimInex)
		{
			if (true == isAnimFinished)
			{
				m_eAnimState = ANIM_STATE::_END;
				pDoor->Attack_Prop();
				return false;
			}
		}
	}

	else
	{
		_float			fDistanceToPlayer = {};
		if (false == m_pBlackBoard->Compute_Distance_To_Player(&fDistanceToPlayer))
			return false;

		_bool			isInRange = { ZOMBIE_RUB_DOOR_RECOGNITION_RANGE >= fDistanceToPlayer };
		if (true == isInRange)
			return false;

		if (false == pDoor->Is_Lock())
			return false;
	}

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_RUB_DOOR);

	if (m_fAccLinearInterpolateTime < ZOMBIE_DOOR_RUB_TOTAL_INTERPOLATE_TO_WINDOW_TIME)
	{
		_bool				isComplete = { m_fAccLinearInterpolateTime >= ZOMBIE_DOOR_RUB_TOTAL_INTERPOLATE_TO_WINDOW_TIME };
		if (false == isComplete)
		{
			_float				fTime = fTimeDelta;
			m_fAccLinearInterpolateTime += fTime;

			if (m_fAccLinearInterpolateTime >= ZOMBIE_DOOR_RUB_TOTAL_INTERPOLATE_TO_WINDOW_TIME)
			{
				fTime -= m_fAccLinearInterpolateTime - ZOMBIE_DOOR_RUB_TOTAL_INTERPOLATE_TO_WINDOW_TIME;
			}
			_float				fRatio = { fTime / ZOMBIE_DOOR_RUB_TOTAL_INTERPOLATE_TO_WINDOW_TIME };

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

void CRub_Door_Zombie::Exit()
{
	if (nullptr == m_pBlackBoard)
		return;

	m_pBlackBoard->Get_AI()->Set_ManualMove(false);
}

void CRub_Door_Zombie::Change_Animation(_float fTimeDelta)
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
			iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_DOOR::_WINDOW_TO_RUB_FROM_A_START);
		}

		else if (ANIM_TYPE::_B == m_eAnimType)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_DOOR::_WINDOW_TO_RUB_FROM_B_START);
		}

		isLoop = false;
	}

	else if (ANIM_STATE::_LOOP == m_eAnimState)
	{
		if (ANIM_TYPE::_A == m_eAnimType)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_DOOR::_WINDOW_TO_RUB_FROM_A_LOOP);
		}

		else if (ANIM_TYPE::_B == m_eAnimType)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_DOOR::_WINDOW_TO_RUB_FROM_B_LOOP);
		}

		isLoop = true;
	}

	else if (ANIM_STATE::_FINISH == m_eAnimState)
	{
		if (ANIM_TYPE::_A == m_eAnimType)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_DOOR::_WINDOW_TO_RUB_FROM_A_END);
		}

		else if (ANIM_TYPE::_B == m_eAnimType)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_DOOR::_WINDOW_TO_RUB_FROM_B_END);
		}

		isLoop = false;
	}

	if (-1 == iResultAnimationIndex)
		return;

	pBody_Model->Change_Animation(static_cast<_uint>(m_eBasePlayingIndex), m_strAnimLayerTag, iResultAnimationIndex);
	pBody_Model->Set_Loop(static_cast<_uint>(m_eBasePlayingIndex), isLoop);

#pragma endregion
}

CRub_Door_Zombie* CRub_Door_Zombie::Create(void* pArg)
{
	CRub_Door_Zombie*			pInstance = { new CRub_Door_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CRub_Door_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRub_Door_Zombie::Free()
{
	__super::Free();
}
