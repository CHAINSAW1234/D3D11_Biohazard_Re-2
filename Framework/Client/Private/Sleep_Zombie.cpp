#include "stdafx.h"
#include "GameInstance.h"
#include "Sleep_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CSleep_Zombie::CSleep_Zombie()
	: CTask_Node()
{
}

CSleep_Zombie::CSleep_Zombie(const CSleep_Zombie& rhs)
{
}

HRESULT CSleep_Zombie::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSleep_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CSleep_Zombie::Enter()
{
}

_bool CSleep_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	auto pAI = m_pBlackBoard->GetAI();
	pAI->SetState(MONSTER_STATE::MST_IDLE);

	Change_Animation();

	return true;
}

void CSleep_Zombie::Exit()
{
}

void CSleep_Zombie::Change_Animation()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel* pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

#pragma region 다시 살려야함

	//_uint			iPlayingIndex = { static_cast<_uint>(CBody_Zombie::PLAYING_INDEX::INDEX_0) };
	//_int			iCurrentPlayingAnimIndex = { pBodyModel->Get_AnimIndex_PlayingInfo(CBody_Zombie::INDEX_0) };
	//_int			iResultAnimationIndex = { -1 };

	//_bool			isFaceUp = { false };

	//_bool			isLoop = { false };
	//wstring			strBoneLayerTag = { BONE_LAYER_DEFAULT_TAG };

	//list<_uint>		ActiveIndices;
	//ActiveIndices.push_back(iPlayingIndex);

	//m_pBlackBoard->Reset_NonActive_Body(ActiveIndices);

	//m_isWake = true;

	////	TODO: 애니메이션 누워있는 모션 추가되면 사용하기 =>
	////	현재는 누운 상태에서 일어나는 모션만있음
	//if (false == m_isWake)
	//{
	//	if (false == isFaceUp)
	//	{
	//		iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_STANDUP_FACEDOWN_F);
	//		iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_STANDUP_FACEDOWN_B);
	//		iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_STANDUP_FACEDOWN_L);
	//		iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_STANDUP_FACEDOWN_R);

	//		isLoop = false;
	//	}

	//	else
	//	{
	//		iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_STANDUP_FACEUP_F);
	//		iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_STANDUP_FACEUP_B);
	//		iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_STANDUP_FACEUP_L);
	//		iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_STANDUP_FACEUP_R);

	//		isLoop = false;
	//	}
	//}

	//else
	//{
	//	_uint			iCurrentMotionType = { m_pBlackBoard->Get_Current_MotionType_Body() };
	//	_uint			iPreMotionType = { m_pBlackBoard->Get_Pre_MotionType_Body() };

	//	if (CBody_Zombie::MOTION_A == iCurrentMotionType)
	//	{
	//		iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_IDLE_LOOP_A);
	//		isLoop = true;
	//	}

	//	else if (CBody_Zombie::MOTION_B == iCurrentMotionType)
	//	{
	//		iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_IDLE_LOOP_B);
	//		isLoop = true;
	//	}

	//	else if (CBody_Zombie::MOTION_C == iCurrentMotionType)
	//	{
	//		iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_IDLE_LOOP_C);
	//		isLoop = true;
	//	}

	//	else if (CBody_Zombie::MOTION_D == iCurrentMotionType)
	//	{
	//		iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_IDLE_LOOP_D);
	//		isLoop = true;
	//	}

	//	else if (CBody_Zombie::MOTION_E == iCurrentMotionType)
	//	{
	//		iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_IDLE_LOOP_E);
	//		isLoop = true;
	//	}

	//	else if (CBody_Zombie::MOTION_F == iCurrentMotionType)
	//	{
	//		iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_IDLE_LOOP_F);
	//		isLoop = true;
	//	}

	//	else
	//	{
	//		iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_IDLE_LOOP_F);
	//		isLoop = true;
	//	}
	//}

	//if (-1 == iResultAnimationIndex)
	//	return;

	////	루프애님이면서 이전 애니메이션과 같다면... 같은 애니메이션을 지속으로 루프 재생중이므로 보간이 필요없다고 판단
	//if (true == isLoop && iCurrentPlayingAnimIndex == iResultAnimationIndex)
	//{
	//	pBodyModel->Set_TotalLinearInterpolation(0.f);
	//}

	//else
	//{
	//	pBodyModel->Set_TotalLinearInterpolation(0.9f);
	//}

	//pBodyModel->Change_Animation(iPlayingIndex, TEXT("Default"), iResultAnimationIndex);
	//pBodyModel->Set_Loop(iPlayingIndex, isLoop);
	//pBodyModel->Set_BoneLayer_PlayingInfo(iPlayingIndex, strBoneLayerTag);
	//pBodyModel->Set_BlendWeight(iPlayingIndex, 1.f);

#pragma endregion
}

CSleep_Zombie* CSleep_Zombie::Create()
{
	CSleep_Zombie* pInstance = new CSleep_Zombie();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSleep_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSleep_Zombie::Free()
{
	__super::Free();
}
