#include "stdafx.h"
#include "GameInstance.h"
#include "Wait_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CWait_Zombie::CWait_Zombie() 
	: CTask_Node()
{
}

CWait_Zombie::CWait_Zombie(const CWait_Zombie& rhs)
{
}

HRESULT CWait_Zombie::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWait_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CWait_Zombie::Enter()
{
	
}

_bool CWait_Zombie::Execute()
{
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->GetAI();
	pAI->SetState(MONSTER_STATE::MST_IDLE);

	cout << "Wait" << endl;
		Change_Animation();

	return true;
}

void CWait_Zombie::Exit()
{
}

void CWait_Zombie::Change_Animation()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*			pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_uint			iPlayingIndex = { static_cast<_uint>(PLAYING_INDEX::INDEX_0) };
	_int			iCurrentPlayingAnimIndex = { pBodyModel->Get_AnimIndex_PlayingInfo(static_cast<_uint>(PLAYING_INDEX::INDEX_0)) };
	_int			iResultAnimationIndex = { -1 };

	_bool			isFaceUp = { false };

	_bool			isLoop = { false };
	wstring			strBoneLayerTag = { BONE_LAYER_DEFAULT_TAG };

	//	list<_uint>		ActiveIndices;
	//	ActiveIndices.push_back(iPlayingIndex);
	//	
	//	m_pBlackBoard->Reset_NonActive_Body(ActiveIndices);

#pragma region �ٽ� �ؾ���
	m_isWake = true;

	//	TODO: �ִϸ��̼� �����ִ� ��� �߰��Ǹ� ����ϱ� =>
	//	����� ���� ���¿��� �Ͼ�� ��Ǹ�����
	if (false == m_isWake)
	{
		/*if (false == isFaceUp)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_STANDUP_FACEDOWN_F);
			iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_STANDUP_FACEDOWN_B);
			iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_STANDUP_FACEDOWN_L);
			iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_STANDUP_FACEDOWN_R);

			isLoop = false;
		}

		else
		{
			iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_STANDUP_FACEUP_F);
			iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_STANDUP_FACEUP_B);
			iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_STANDUP_FACEUP_L);
			iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_STANDUP_FACEUP_R);

			isLoop = false;
		}*/
	}

	else
	{
		MOTION_TYPE		eCurrentMotionType = { static_cast<MOTION_TYPE>(m_pBlackBoard->Get_Current_MotionType_Body()) };
		MOTION_TYPE		ePreMotionType = { static_cast<MOTION_TYPE>(m_pBlackBoard->Get_Pre_MotionType_Body()) };

		if (MOTION_TYPE::MOTION_A == eCurrentMotionType)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_IDLE::_LOOP_A);
			isLoop = true;
		}

		else if (MOTION_TYPE::MOTION_B == eCurrentMotionType)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_IDLE::_LOOP_B);
			isLoop = true;
		}

		else if (MOTION_TYPE::MOTION_C == eCurrentMotionType)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_IDLE::_LOOP_C);
			isLoop = true;
		}

		else if (MOTION_TYPE::MOTION_D == eCurrentMotionType)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_IDLE::_LOOP_D);
			isLoop = true;
		}

		else if (MOTION_TYPE::MOTION_E == eCurrentMotionType)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_IDLE::_LOOP_E);
			isLoop = true;
		}

		else if (MOTION_TYPE::MOTION_F == eCurrentMotionType)
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_IDLE::_LOOP_F);
			isLoop = true;
		}

		else
		{
			iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_IDLE::_LOOP_F);
			isLoop = true;
		}
	}

	if (-1 == iResultAnimationIndex)
		return;

	//	�����ִ��̸鼭 ���� �ִϸ��̼ǰ� ���ٸ�... ���� �ִϸ��̼��� �������� ���� ������̹Ƿ� ������ �ʿ���ٰ� �Ǵ�
	if (true == isLoop && iCurrentPlayingAnimIndex == iResultAnimationIndex)
	{
		pBodyModel->Set_TotalLinearInterpolation(0.f);
	}

	else
	{
		pBodyModel->Set_TotalLinearInterpolation(0.9f);
	}

	pBodyModel->Set_TotalLinearInterpolation(0.9f);

	pBodyModel->Change_Animation(iPlayingIndex, TEXT("Ordinary_Idle"), iResultAnimationIndex);
	pBodyModel->Set_Loop(iPlayingIndex, isLoop);
	pBodyModel->Set_BoneLayer_PlayingInfo(iPlayingIndex, strBoneLayerTag);
	pBodyModel->Set_BlendWeight(iPlayingIndex, 1.f);

#pragma endregion
}

CWait_Zombie* CWait_Zombie::Create()
{
	CWait_Zombie* pInstance = new CWait_Zombie();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CWait_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWait_Zombie::Free()
{
	__super::Free();
}
