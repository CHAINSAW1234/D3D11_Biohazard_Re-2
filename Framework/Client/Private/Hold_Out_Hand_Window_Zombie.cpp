#include "stdafx.h"
#include "GameInstance.h"
#include "Hold_Out_Hand_Window_Zombie.h"
#include "BlackBoard_Zombie.h"

#include "Window.h"
#include "Player.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CHold_Out_Hand_Window_Zombie::CHold_Out_Hand_Window_Zombie()
	: CTask_Node()
{
}

CHold_Out_Hand_Window_Zombie::CHold_Out_Hand_Window_Zombie(const CHold_Out_Hand_Window_Zombie& rhs)
{
}

HRESULT CHold_Out_Hand_Window_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CHold_Out_Hand_Window_Zombie::Enter()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*				pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_int				iRandom = { m_pGameInstance->GetRandom_Int(1, 2) };
	if (1 == iRandom)
	{
		m_eAnimType = ANIM_TYPE::_1;
	}

	else if (2 == iRandom)
	{
		m_eAnimType = ANIM_TYPE::_2;
	}

	CWindow*			pWindow = { m_pBlackBoard->Get_Nearest_Window() };
	if (nullptr == pWindow)
		return;

	_int			iAnimIndex = { -1 };
	if (m_eAnimType == ANIM_TYPE::_1)
	{
		iAnimIndex = static_cast<_int>(ANIM_GIMMICK_WINDOW::_HOLD_OUT_HAND_START1);
	}

	else if (m_eAnimType == ANIM_TYPE::_2)
	{
		iAnimIndex = static_cast<_int>(ANIM_GIMMICK_WINDOW::_HOLD_OUT_HAND_START2);
	}

	if (false == m_pBlackBoard->Compute_DeltaMatrix_AnimFirstKeyFrame_From_Target(pWindow->Get_Transform(), static_cast<_uint>(m_eBasePlayingIndex), iAnimIndex, m_strAnimLayerTag, &m_DeltaInterpolateMatrix))
		return;

	m_pBlackBoard->Get_AI()->Set_ManualMove(true);

#ifdef _DEBUG

	cout << "Enter Hold Window" << endl;

#endif 
}

_bool CHold_Out_Hand_Window_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	CWindow*			pWindow = { m_pBlackBoard->Get_Nearest_Window() };
	if (nullptr == pWindow)
		return false;

	MONSTER_STATE		ePreMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };
	if (ePreMonsterState == MONSTER_STATE::MST_HOLD_WINDOW)
	{
		CModel*			pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
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
			_float			fDistanceToPlayer = {};
			if (false == m_pBlackBoard->Compute_Distance_To_Player(&fDistanceToPlayer))
				return false;

			_bool			isOutRange = { ZOMBIE_HOLD_WINDOW_RECOGNITION_RANGE < fDistanceToPlayer };
			if (true == isOutRange)
				m_eAnimState = ANIM_STATE::_FINISH;
		}

		else if (isSameLayer && isFInishAnimInex)
		{
			if (true == isAnimFinished)
			{
				m_eAnimState = ANIM_STATE::_END;
				return false;
			}
		}
	}

	else
	{
		//	ÇÊ¿ä Á¶°Ç => Ã¢¹® ±úÁü, ¹Ù¸®°ÔÀÌÆ® ÃÄÁü
		_int				iHpWindow = { pWindow->Get_HP_Body() };
		_bool				isWindowBroken = { 0 >= iHpWindow };
		if (false == isWindowBroken)
			return false;

		_bool				isSetBarrigate = { pWindow->Is_Set_Barrigate() };
		if (false == isSetBarrigate)
			return false;

		_float			fDistanceToPlayer = {};
		if (false == m_pBlackBoard->Compute_Distance_To_Player(&fDistanceToPlayer))
			return false;

		_bool			isOutRange = { ZOMBIE_HOLD_WINDOW_RECOGNITION_RANGE < fDistanceToPlayer };
		if (true == isOutRange)
			return false;

		m_eAnimState = ANIM_STATE::_START;
	}

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_HOLD_WINDOW);

	if (m_fAccLinearInterpolateTime < ZOMBIE_HOLD_TOTAL_INTERPOLATE_TO_WINDOW_TIME &&
		true == ZOMBIE_HOLD_TOTAL_INTERPOLATE_TO_WINDOW_TIME)
	{
		_bool				isComplete = { m_fAccLinearInterpolateTime >= ZOMBIE_HOLD_TOTAL_INTERPOLATE_TO_WINDOW_TIME };
		if (false == isComplete)
		{
			_float				fTime = fTimeDelta;
			m_fAccLinearInterpolateTime += fTime;

			if (m_fAccLinearInterpolateTime >= ZOMBIE_HOLD_TOTAL_INTERPOLATE_TO_WINDOW_TIME)
			{
				fTime -= m_fAccLinearInterpolateTime - ZOMBIE_HOLD_TOTAL_INTERPOLATE_TO_WINDOW_TIME;
			}
			_float				fRatio = { fTime / ZOMBIE_HOLD_TOTAL_INTERPOLATE_TO_WINDOW_TIME };

			if (false == isComplete)
			{
				if (false == m_pBlackBoard->Apply_Devide_Delta_Matrix(fRatio, XMLoadFloat4x4(&m_DeltaInterpolateMatrix)))
					return false;
			}
		}
	}

	Change_Animation(fTimeDelta);

	return true;
}

void CHold_Out_Hand_Window_Zombie::Exit()
{
	if (nullptr == m_pBlackBoard)
		return;

	m_pBlackBoard->Get_AI()->Set_ManualMove(false);
}

void CHold_Out_Hand_Window_Zombie::Change_Animation(_float fTimeDelta)
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*			pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_int			iResultAnimationIndex = { -1 };
	_bool			isLoop = { false };


	if (ANIM_STATE::_START == m_eAnimState)
	{
		if (ANIM_TYPE::_1 == m_eAnimType)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_WINDOW::_HOLD_OUT_HAND_START1);
		}

		else if (ANIM_TYPE::_2 == m_eAnimType)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_WINDOW::_HOLD_OUT_HAND_START2);
		}

		isLoop = false;
	}

	else if (ANIM_STATE::_LOOP == m_eAnimState)
	{
		if (ANIM_TYPE::_1 == m_eAnimType)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_WINDOW::_HOLD_OUT_HAND_LOOP1);
		}

		else if (ANIM_TYPE::_2 == m_eAnimType)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_WINDOW::_HOLD_OUT_HAND_LOOP2);
		}

		isLoop = true;
	}

	else if (ANIM_STATE::_FINISH == m_eAnimState)
	{
		if (ANIM_TYPE::_1 == m_eAnimType)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_WINDOW::_HOLD_OUT_HAND_END1);
		}

		else if (ANIM_TYPE::_2 == m_eAnimType)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_GIMMICK_WINDOW::_HOLD_OUT_HAND_END2);
		}

		isLoop = false;
	}

	if (-1 == iResultAnimationIndex)
		return;

	pBodyModel->Change_Animation(static_cast<_uint>(m_eBasePlayingIndex), m_strAnimLayerTag, iResultAnimationIndex);
	pBodyModel->Set_Loop(static_cast<_uint>(m_eBasePlayingIndex), isLoop);

#pragma endregion
}

CHold_Out_Hand_Window_Zombie* CHold_Out_Hand_Window_Zombie::Create(void* pArg)
{
	CHold_Out_Hand_Window_Zombie* pInstance = { new CHold_Out_Hand_Window_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHold_Out_Hand_Window_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHold_Out_Hand_Window_Zombie::Free()
{
	__super::Free();
}

