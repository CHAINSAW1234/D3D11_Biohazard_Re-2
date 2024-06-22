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

void CWait_Zombie::Execute()
{
	if (nullptr == m_pBlackBoard)
		return;

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->GetAI();
	pAI->SetState(MONSTER_STATE::MST_IDLE);

	//cout << "Wait" << endl;
	//	Change_Animation();

	static _int				iAnimIndex = { 0 };
	if (DOWN == m_pGameInstance->Get_KeyState('N'))
	{
		iAnimIndex += 1;
		if (iAnimIndex > 380)
			iAnimIndex = 0;
	}

	if (DOWN == m_pGameInstance->Get_KeyState('M'))
	{
		iAnimIndex -= 1;
		if (iAnimIndex < 0)
			iAnimIndex = 379;
	}
	CModel* pModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	pModel->Change_Animation(0, TEXT("Test"), iAnimIndex);
	pModel->Set_OptimizationCulling(false);
	cout << iAnimIndex << endl;

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

	_uint			iPlayingIndex = { static_cast<_uint>(CBody_Zombie::PLAYING_INDEX::INDEX_0) };
	_int			iCurrentPlayingAnimIndex = { pBodyModel->Get_AnimIndex_PlayingInfo(CBody_Zombie::INDEX_0) };
	_int			iResultAnimationIndex = { -1 };

	_bool			isFaceUp = { false };

	_bool			isLoop = { false };
	wstring			strBoneLayerTag = { BONE_LAYER_DEFAULT_TAG };

	//	list<_uint>		ActiveIndices;
	//	ActiveIndices.push_back(iPlayingIndex);
	//	
	//	m_pBlackBoard->Reset_NonActive_Body(ActiveIndices);

	m_isWake = true;

	//	TODO: 애니메이션 누워있는 모션 추가되면 사용하기 =>
	//	현재는 누운 상태에서 일어나는 모션만있음
	if (false == m_isWake)
	{
		if (false == isFaceUp)
		{
			iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_STANDUP_FACEDOWN_F);
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
		}
	}

	else
	{
		_uint			iCurrentMotionType = { m_pBlackBoard->Get_Current_MotionType_Body() };
		_uint			iPreMotionType = { m_pBlackBoard->Get_Pre_MotionType_Body() };

		if (CBody_Zombie::MOTION_A == iCurrentMotionType)
		{
			iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_IDLE_LOOP_A);
			isLoop = true;
		}

		else if (CBody_Zombie::MOTION_B == iCurrentMotionType)
		{
			iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_IDLE_LOOP_B);
			isLoop = true;
		}

		else if (CBody_Zombie::MOTION_C == iCurrentMotionType)
		{
			iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_IDLE_LOOP_C);
			isLoop = true;
		}

		else if (CBody_Zombie::MOTION_D == iCurrentMotionType)
		{
			iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_IDLE_LOOP_D);
			isLoop = true;
		}

		else if (CBody_Zombie::MOTION_E == iCurrentMotionType)
		{
			iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_IDLE_LOOP_E);
			isLoop = true;
		}

		else if (CBody_Zombie::MOTION_F == iCurrentMotionType)
		{
			iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_IDLE_LOOP_F);
			isLoop = true;
		}

		else
		{
			iResultAnimationIndex = static_cast<_int>(CBody_Zombie::ANIM_IDLE_LOOP_F);
			isLoop = true;
		}
	}

	if (-1 == iResultAnimationIndex)
		return;

	//	루프애님이면서 이전 애니메이션과 같다면... 같은 애니메이션을 지속으로 루프 재생중이므로 보간이 필요없다고 판단
	if (true == isLoop && iCurrentPlayingAnimIndex == iResultAnimationIndex)
	{
		pBodyModel->Set_TotalLinearInterpolation(0.f);
	}

	else
	{
		pBodyModel->Set_TotalLinearInterpolation(0.9f);
	}

	pBodyModel->Set_TotalLinearInterpolation(0.9f);

	pBodyModel->Change_Animation(iPlayingIndex, TEXT("Default"), iResultAnimationIndex);
	pBodyModel->Set_Loop(iPlayingIndex, isLoop);
	pBodyModel->Set_BoneLayer_PlayingInfo(iPlayingIndex, strBoneLayerTag);
	pBodyModel->Set_BlendWeight(iPlayingIndex, 1.f);
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
