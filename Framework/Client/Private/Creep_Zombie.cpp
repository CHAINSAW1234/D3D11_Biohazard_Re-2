#include "stdafx.h"
#include "GameInstance.h"
#include "Creep_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Body_Zombie.h"

CCreep_Zombie::CCreep_Zombie()
	: CTask_Node()
{
}

CCreep_Zombie::CCreep_Zombie(const CCreep_Zombie& rhs)
{
}

HRESULT CCreep_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CCreep_Zombie::Enter()
{
	if (nullptr == m_pBlackBoard)
		return;

	m_eStartFaceState = m_pBlackBoard->Get_AI()->Get_FaceState() ;

#ifdef _DEBUG
	cout << "Enter Creep" << endl;
#endif 
}

_bool CCreep_Zombie::Execute(_float fTimeDelta)
{
	return false;

#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	if (CZombie::POSE_STATE::_CREEP != m_pBlackBoard->Get_AI()->Get_PoseState())
		return false;

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_CREEP);

	Change_Animation();

	return true;
}

void CCreep_Zombie::Exit()
{
}

void CCreep_Zombie::Change_Animation()
{
	//	if (nullptr == m_pBlackBoard)
	//	return;

	//CModel*			pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	//if (nullptr == pBodyModel)
	//	return;

	//_uint			iPlayingIndex = { static_cast<_uint>(PLAYING_INDEX::INDEX_0) };
	//_int			iCurrentPlayingAnimIndex = { pBodyModel->Get_AnimIndex_PlayingInfo(static_cast<_uint>(PLAYING_INDEX::INDEX_0)) };
	//_int			iResultAnimationIndex = { -1 };

	//_bool			isFaceUp = { false };

	//_bool			isLoop = { false };
	//wstring			strBoneLayerTag = { BONE_LAYER_DEFAULT_TAG };

	////	list<_uint>		ActiveIndices;
	////	ActiveIndices.push_back(iPlayingIndex);
	////	
	////	m_pBlackBoard->Reset_NonActive_Body(ActiveIndices);
	//MOTION_TYPE		eCurrentMotionType = { static_cast<MOTION_TYPE>(m_pBlackBoard->Get_Current_MotionType_Body()) };
	//MOTION_TYPE		ePreMotionType = { static_cast<MOTION_TYPE>(m_pBlackBoard->Get_Pre_MotionType_Body()) };

	//if (MOTION_TYPE::MOTION_A == eCurrentMotionType)
	//{
	//	iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_IDLE::_LOOP_A);
	//	isLoop = true;
	//}

	//else if (MOTION_TYPE::MOTION_B == eCurrentMotionType)
	//{
	//	iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_IDLE::_LOOP_B);
	//	isLoop = true;
	//}

	//else if (MOTION_TYPE::MOTION_C == eCurrentMotionType)
	//{
	//	iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_IDLE::_LOOP_C);
	//	isLoop = true;
	//}

	//else if (MOTION_TYPE::MOTION_D == eCurrentMotionType)
	//{
	//	iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_IDLE::_LOOP_D);
	//	isLoop = true;
	//}

	//else if (MOTION_TYPE::MOTION_E == eCurrentMotionType)
	//{
	//	iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_IDLE::_LOOP_E);
	//	isLoop = true;
	//}

	//else if (MOTION_TYPE::MOTION_F == eCurrentMotionType)
	//{
	//	iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_IDLE::_LOOP_F);
	//	isLoop = true;
	//}

	//else
	//{
	//	iResultAnimationIndex = static_cast<_int>(ANIM_ORDINARY_IDLE::_LOOP_F);
	//	isLoop = true;
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

	//pBodyModel->Set_TotalLinearInterpolation(0.9f);

	//pBodyModel->Change_Animation(iPlayingIndex, TEXT("Ordinary_Idle"), iResultAnimationIndex);
	//pBodyModel->Set_Loop(iPlayingIndex, isLoop);
	//pBodyModel->Set_BoneLayer_PlayingInfo(iPlayingIndex, strBoneLayerTag);
	//pBodyModel->Set_BlendWeight(iPlayingIndex, 1.f);
}

CCreep_Zombie* CCreep_Zombie::Create(void* pArg)
{
	CCreep_Zombie* pInstance = { new CCreep_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CCreep_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCreep_Zombie::Free()
{
	__super::Free();
}
