#include "stdafx.h"
#include "GameInstance.h"
#include "MoveTo_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"

#include "Body_Zombie.h"

CMoveTo_Zombie::CMoveTo_Zombie()
	: CTask_Node()
{
}

CMoveTo_Zombie::CMoveTo_Zombie(const CMoveTo_Zombie& rhs)
{
}

HRESULT CMoveTo_Zombie::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMoveTo_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CMoveTo_Zombie::Enter()
{
}

void CMoveTo_Zombie::Execute()
{
	auto pAI = m_pBlackBoard->GetAI();

	pAI->SetState(MONSTER_STATE::MST_WALK);

	cout << "Move" << endl;

	Change_Animation();
}

void CMoveTo_Zombie::Exit()
{
}

void CMoveTo_Zombie::Change_Animation()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_float3			vDirectionToPlayerLocalFloat3;
	if (false == m_pBlackBoard->Compute_Direction_To_Player_Local(&vDirectionToPlayerLocalFloat3))
		return;

	_vector			vDirectionToPlayerLocal = { XMLoadFloat3(&vDirectionToPlayerLocalFloat3) };
	_vector			vDirectionToPlayerLocalXZPlane = { XMVector3Normalize(XMVectorSetY(vDirectionToPlayerLocal, 0.f)) };		//	ȸ������ xz���󿡼��� ����ϱ�����
	_vector			vAILookLocal = { XMVectorSet(0.f, 0.f, 1.f, 0.f) };

	_bool			isRight = { XMVectorGetX(vDirectionToPlayerLocalXZPlane) > 0.f };
	_bool			isFront = { XMVectorGetZ(vDirectionToPlayerLocalXZPlane) > 0.f };

	_float			fDot = { XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDirectionToPlayerLocal), XMVector3Normalize(vAILookLocal))) };
	_float			fAngleToTarget = { acosf(fDot) };

	/* �⺻ �̵� �ִϸ��̼� */
	_uint			iBasePlayingIndex = { static_cast<_uint>(CBody_Zombie::PLAYING_INDEX::INDEX_0) };
	_int			iCurrentBasegAnimIndex = { pBodyModel->Get_AnimIndex_PlayingInfo(CBody_Zombie::INDEX_0) };
	_int			iResultAnimationIndex = { -1 };
	_bool			isLoop = { false };
	wstring			strBaseBoneLayerTag = { BONE_LAYER_DEFAULT_TAG };

	/* ���� �ʿ� ����*/
	_bool			isNeedBlend = { false };

	/* ȸ�� �������ϴ°�� �ʹ׸�� ���� ���� */
	_int			iBlendPlayingIndex = { static_cast<_uint>(CBody_Zombie::PLAYING_INDEX::INDEX_1) };
	_int			iCurrentBlendAnimIndex = { pBodyModel->Get_AnimIndex_PlayingInfo(CBody_Zombie::INDEX_1) };
	_float			fTurnBlendWeight = { 0.f };
	_int			iBlendAnimIndex = { -1 };
	wstring			strBlendBoneLayerTag = { BONE_LAYER_DEFAULT_TAG };

	_uint			iCurrentMotionType = { m_pBlackBoard->Get_Current_MotionType_Body() };
	_uint			iPreMotionType = { m_pBlackBoard->Get_Pre_MotionType_Body() };

	//	�⺻ �̵� => ����Ʈ ���̴�.
	//	���� �ִϸ��̼��� �̵��ִϸ��̼��̾��ٸ� => �ǴϽ� üũ���� ���� �ִϸ��̼����� �Ѿ����

	_bool			isStartAnim = { m_pBlackBoard->Is_Start_Anim(CMonster::PART_BODY, iCurrentBasegAnimIndex) };
	_bool			isMoveAnim = { m_pBlackBoard->Is_Move_Anim(CMonster::PART_BODY, iCurrentBasegAnimIndex) };
	_bool			isTurnAnim = { m_pBlackBoard->Is_Turn_Anim(CMonster::PART_BODY, iCurrentBasegAnimIndex) };


	//	90�� �ʰ� => �ǹ� ��
	//if (XMConvertToRadians(90.f) < fAngleToTarget)
	//{
	//	_bool		isFinished = { false };

	//	if (true == isTurnAnim)
	//	{
	//		//	���� ��ŸƮ ��� ������
	//		if (false == pBodyModel->isFinished(iBasePlayingIndex))
	//		{
	//			iResultAnimationIndex = iCurrentBasegAnimIndex;
	//		}

	//		else
	//		{
	//			isFinished = true;
	//		}
	//	}

	//	if (true == isFinished || false == isTurnAnim)
	//	{
	//		if (XMConvertToRadians(135.f) > fAngleToTarget)
	//		{
	//			if (CBody_Zombie::MOTION_A == iCurrentMotionType)
	//			{
	//				if (false == isRight)
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_L90_A);
	//				}

	//				else
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_R90_A);
	//				}

	//				isLoop = false;
	//			}

	//			else if (CBody_Zombie::MOTION_B == iCurrentMotionType)
	//			{
	//				if (false == isRight)
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_L90_B);
	//				}

	//				else
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_R90_B);
	//				}

	//				isLoop = false;
	//			}

	//			else if (CBody_Zombie::MOTION_C == iCurrentMotionType)
	//			{
	//				if (false == isRight)
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_L90_C);
	//				}

	//				else
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_R90_C);
	//				}

	//				isLoop = false;
	//			}

	//			else if (CBody_Zombie::MOTION_D == iCurrentMotionType)
	//			{
	//				if (false == isRight)
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_L90_D);
	//				}

	//				else
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_R90_D);
	//				}

	//				isLoop = false;
	//			}

	//			else if (CBody_Zombie::MOTION_E == iCurrentMotionType)
	//			{
	//				if (false == isRight)
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_L90_E);
	//				}

	//				else
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_R90_E);
	//				}

	//				isLoop = false;
	//			}

	//			else if (CBody_Zombie::MOTION_F == iCurrentMotionType)
	//			{
	//				if (false == isRight)
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_L90_F);
	//				}

	//				else
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_R90_F);
	//				}

	//				isLoop = false;
	//			}
	//		}

	//		else
	//		{
	//			if (CBody_Zombie::MOTION_A == iCurrentMotionType)
	//			{
	//				if (false == isRight)
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_L180_A);
	//				}

	//				else
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_R180_A);
	//				}

	//				isLoop = false;
	//			}

	//			else if (CBody_Zombie::MOTION_B == iCurrentMotionType)
	//			{
	//				if (false == isRight)
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_L180_B);
	//				}

	//				else
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_R180_B);
	//				}

	//				isLoop = false;
	//			}

	//			else if (CBody_Zombie::MOTION_C == iCurrentMotionType)
	//			{
	//				if (false == isRight)
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_L180_C);
	//				}

	//				else
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_R180_C);
	//				}

	//				isLoop = false;
	//			}

	//			else if (CBody_Zombie::MOTION_D == iCurrentMotionType)
	//			{
	//				if (false == isRight)
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_L180_D);
	//				}

	//				else
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_R180_D);
	//				}

	//				isLoop = false;
	//			}

	//			else if (CBody_Zombie::MOTION_E == iCurrentMotionType)
	//			{
	//				if (false == isRight)
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_L180_E);
	//				}

	//				else
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_R180_E);
	//				}

	//				isLoop = false;
	//			}

	//			else if (CBody_Zombie::MOTION_F == iCurrentMotionType)
	//			{
	//				if (false == isRight)
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_L180_F);
	//				}

	//				else
	//				{
	//					iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_PIVOT_TURN_R180_F);
	//				}

	//				isLoop = false;
	//			}
	//		}
	//	}
	//	

	//	if (iResultAnimationIndex == iCurrentBasegAnimIndex && true == isFinished)
	//	{
	//		pBodyModel->Set_TrackPosition(iBasePlayingIndex, 0.f);
	//	}

	//}

	//	else
	if (true == isMoveAnim)
	{
		_bool		isFinished = { false };
		//	���� ù ��� ���̾������ => ���� üũ�� �ʿ��� => ��ŸƮ����� ��� ������ ������ �Ѿ����
		if (true == isStartAnim)
		{
			//	���� ��ŸƮ ��� ������
			if (false == pBodyModel->isFinished(iBasePlayingIndex))
			{
				iResultAnimationIndex = iCurrentBasegAnimIndex;
			}

			else
			{
				isFinished = true;
			}
		}

		if (true == isFinished || (true == isMoveAnim && false == isStartAnim))
		{
			if (CBody_Zombie::MOTION_A == iCurrentMotionType)
			{
				iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_WALK_LOOP_A);
				isLoop = true;
			}

			else if (CBody_Zombie::MOTION_B == iCurrentMotionType)
			{
				iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_WALK_LOOP_B);
				isLoop = true;
			}

			else if (CBody_Zombie::MOTION_C == iCurrentMotionType)
			{
				iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_WALK_LOOP_C);
				isLoop = true;
			}

			else if (CBody_Zombie::MOTION_D == iCurrentMotionType)
			{
				iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_WALK_LOOP_D);
				isLoop = true;
			}

			else if (CBody_Zombie::MOTION_E == iCurrentMotionType)
			{
				iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_WALK_LOOP_E);
				isLoop = true;
			}

			else if (CBody_Zombie::MOTION_F == iCurrentMotionType)
			{
				iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_WALK_LOOP_F);
				isLoop = true;
			}
		}
	}

	//	�̵��ִϸ��̼����� �ٲ����ϴ°�� => ������̴� �ִϸ��̼��� �̵������̾ƴ�
	else
	{
		if (CBody_Zombie::MOTION_A == iCurrentMotionType)
		{
			iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_WALK_START_A);
			isLoop = false;
		}

		else if (CBody_Zombie::MOTION_B == iCurrentMotionType)
		{
			iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_WALK_START_B);
			isLoop = false;
		}

		else if (CBody_Zombie::MOTION_C == iCurrentMotionType)
		{
			iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_WALK_START_C);
			isLoop = false;
		}

		else if (CBody_Zombie::MOTION_D == iCurrentMotionType)
		{
			iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_WALK_START_D);
			isLoop = false;
		}

		else if (CBody_Zombie::MOTION_E == iCurrentMotionType)
		{
			iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_WALK_START_E);
			isLoop = false;
		}

		else if (CBody_Zombie::MOTION_E == iCurrentMotionType)
		{
			iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_WALK_START_F);
			isLoop = false;
		}
	}

	//	ȸ���� �����Ͽ� ���� ���� ���ϱ�
	//	10�� �̸��� �׳������ϱ�.
	if (XMConvertToRadians(10.f) > fAngleToTarget)
	{
		isNeedBlend = false;
	}

	else
	{
		isNeedBlend = true;

		if (true == isRight)
		{
			if (CBody_Zombie::MOTION_A == iCurrentMotionType)
			{
				iBlendAnimIndex = static_cast<_int>(CBody_Zombie::ANIM_TURNING_LOOP_FR_A);
			}

			if (CBody_Zombie::MOTION_B == iCurrentMotionType)
			{
				iBlendAnimIndex = static_cast<_int>(CBody_Zombie::ANIM_TURNING_LOOP_FR_B);
			}

			if (CBody_Zombie::MOTION_C == iCurrentMotionType)
			{
				iBlendAnimIndex = static_cast<_int>(CBody_Zombie::ANIM_TURNING_LOOP_FR_C);
			}

			if (CBody_Zombie::MOTION_D == iCurrentMotionType)
			{
				iBlendAnimIndex = static_cast<_int>(CBody_Zombie::ANIM_TURNING_LOOP_FR_D);
			}

			if (CBody_Zombie::MOTION_E == iCurrentMotionType)
			{
				iBlendAnimIndex = static_cast<_int>(CBody_Zombie::ANIM_TURNING_LOOP_FR_E);
			}

			if (CBody_Zombie::MOTION_F == iCurrentMotionType)
			{
				iBlendAnimIndex = static_cast<_int>(CBody_Zombie::ANIM_TURNING_LOOP_FR_F);
			}
		}

		else
		{
			if (CBody_Zombie::MOTION_A == iCurrentMotionType)
			{
				iBlendAnimIndex = static_cast<_int>(CBody_Zombie::ANIM_TURNING_LOOP_FL_A);
			}

			if (CBody_Zombie::MOTION_B == iCurrentMotionType)
			{
				iBlendAnimIndex = static_cast<_int>(CBody_Zombie::ANIM_TURNING_LOOP_FL_B);
			}

			if (CBody_Zombie::MOTION_C == iCurrentMotionType)
			{
				iBlendAnimIndex = static_cast<_int>(CBody_Zombie::ANIM_TURNING_LOOP_FL_C);
			}

			if (CBody_Zombie::MOTION_D == iCurrentMotionType)
			{
				iBlendAnimIndex = static_cast<_int>(CBody_Zombie::ANIM_TURNING_LOOP_FL_D);
			}

			if (CBody_Zombie::MOTION_E == iCurrentMotionType)
			{
				iBlendAnimIndex = static_cast<_int>(CBody_Zombie::ANIM_TURNING_LOOP_FL_E);
			}

			if (CBody_Zombie::MOTION_F == iCurrentMotionType)
			{
				iBlendAnimIndex = static_cast<_int>(CBody_Zombie::ANIM_TURNING_LOOP_FL_F);
			}
		}

		fTurnBlendWeight = fminf(fAngleToTarget / XMConvertToRadians(180.f) * 2.f, 1.f);
	}

	if (-1 == iResultAnimationIndex)
		return;

	//	�����ִ��̸鼭 ���� �ִϸ��̼ǰ� ���ٸ�... ���� �ִϸ��̼��� �������� ���� ������̹Ƿ� ������ �ʿ���ٰ� �Ǵ�
	if (true == isLoop && iBasePlayingIndex == iResultAnimationIndex)
	{
		pBodyModel->Set_TotalLinearInterpolation(0.f);
	}

	else
	{
		pBodyModel->Set_TotalLinearInterpolation(0.8f);
	}


	pBodyModel->Change_Animation(iBasePlayingIndex, iResultAnimationIndex);
	pBodyModel->Set_Loop(iBasePlayingIndex, isLoop);
	pBodyModel->Set_BoneLayer_PlayingInfo(iBasePlayingIndex, strBaseBoneLayerTag);
	pBodyModel->Set_BlendWeight(iBasePlayingIndex, 1.f - fTurnBlendWeight);

	if (true == isNeedBlend)
	{
		pBodyModel->Change_Animation(iBlendPlayingIndex, iBlendAnimIndex);
		pBodyModel->Set_Loop(iBlendPlayingIndex, true);
		pBodyModel->Set_BoneLayer_PlayingInfo(iBlendPlayingIndex, strBlendBoneLayerTag);
		pBodyModel->Set_BlendWeight(iBlendPlayingIndex, fTurnBlendWeight);

		_float			fTrackPosition = { pBodyModel->Get_TrackPosition(iBasePlayingIndex) };
		_float			fDuration = { pBodyModel->Get_Duration_From_PlayingInfo(iBlendPlayingIndex) };

		string			strBlendWeight = { to_string(fTurnBlendWeight) };
		cout << strBlendWeight.c_str() << endl;
		cout << "isBlend" << endl;

		while (fTrackPosition > fDuration)
		{
			fTrackPosition -= fDuration;
		}

		pBodyModel->Set_TrackPosition(iBlendPlayingIndex, fTrackPosition);
	}

	else
	{
		pBodyModel->Set_BlendWeight(iBlendPlayingIndex, 0.f);
		pBodyModel->Reset_PreAnimation(iBlendPlayingIndex);

		cout << "isNonBlend" << endl;
	}
}

CMoveTo_Zombie* CMoveTo_Zombie::Create()
{
	CMoveTo_Zombie* pInstance = new CMoveTo_Zombie();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMoveTo_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoveTo_Zombie::Free()
{
	__super::Free();
}
