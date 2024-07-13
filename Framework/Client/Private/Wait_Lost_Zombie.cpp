#include "stdafx.h"
#include "GameInstance.h"
#include "Wait_Lost_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CWait_Lost_Zombie::CWait_Lost_Zombie()
	: CTask_Node()
{
}

CWait_Lost_Zombie::CWait_Lost_Zombie(const CWait_Lost_Zombie& rhs)
{
}

HRESULT CWait_Lost_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CWait_Lost_Zombie::Enter()
{
	Change_Animation();

#ifdef _DEBUG
	cout << "Enter Lost Wait" << endl;
#endif 
}

_bool CWait_Lost_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	MONSTER_STATE			eMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };

	if (MONSTER_STATE::MST_LOST_IDLE != eMonsterState)
	{
		_bool			isBreak_L_Leg = { m_pBlackBoard->Is_Break_L_Leg() };
		_bool			isBreak_R_Leg = { m_pBlackBoard->Is_Break_R_Leg() };
		_bool			isNonBreak_Legs = { !isBreak_L_Leg && !isBreak_R_Leg };

		CZombie::POSE_STATE			ePoseState = { m_pBlackBoard->Get_AI()->Get_PoseState() };
		CZombie::FACE_STATE			eFaceState = { m_pBlackBoard->Get_AI()->Get_FaceState() };

		if (true == isNonBreak_Legs)
		{
			if (CZombie::POSE_STATE::_UP != ePoseState)
				return false;

			_bool			isBreak_L_Ankle = { m_pBlackBoard->Is_Break_L_Foot() };
			_bool			isBreak_R_Ankle = { m_pBlackBoard->Is_Break_R_Foot() };
			if (true == isBreak_L_Ankle)
			{
				m_iAnimationIndex = static_cast<_int>(ANIM_LOST_IDLE::_ANKLE_LOOP1);
			}

			else if (true == isBreak_R_Ankle)
			{
				m_iAnimationIndex = static_cast<_int>(ANIM_LOST_IDLE::_ANKLE_LOOP2);
			}
		}

		else
		{
			if (CZombie::POSE_STATE::_CREEP != ePoseState)
				return false;

			if (CZombie::FACE_STATE::_DOWN != eFaceState)
				return false;

			_bool			isBreak_L_Arm = { m_pBlackBoard->Is_Break_L_Arm() };
			_bool			isBreak_R_Arm = { m_pBlackBoard->Is_Break_R_Arm() };
			_bool			isBreak_Arms = { isBreak_L_Arm && isBreak_R_Arm };

			if (true == isBreak_Arms)
			{
				m_iAnimationIndex = static_cast<_int>(ANIM_LOST_IDLE::_LOOP4);
			}

			else if (true == isBreak_L_Arm)
			{
				m_iAnimationIndex = static_cast<_int>(ANIM_LOST_IDLE::_LOOP2);
			}

			else if (true == isBreak_R_Arm)
			{
				m_iAnimationIndex = static_cast<_int>(ANIM_LOST_IDLE::_LOOP3);
			}

			else
			{
				m_iAnimationIndex = static_cast<_int>(ANIM_LOST_IDLE::_LOOP1);
			}
		}

		if (-1 == m_iAnimationIndex)
			return false;
	}

	m_pBlackBoard->Organize_PreState(this);

	auto pAI = m_pBlackBoard->Get_AI();
	pAI->Set_State(MONSTER_STATE::MST_LOST_IDLE);

	return true;
}

void CWait_Lost_Zombie::Exit()
{
	m_iAnimationIndex = -1;
}

void CWait_Lost_Zombie::Change_Animation()
{
	if (nullptr == m_pBlackBoard)
		return;

	CModel*			pBodyModel = { m_pBlackBoard->Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	pBodyModel->Change_Animation(static_cast<_uint>(m_eBasePlayingIndex), m_strAnimLayerTag, m_iAnimationIndex);
	pBodyModel->Set_Loop(static_cast<_uint>(m_eBasePlayingIndex), true);
}

CWait_Lost_Zombie* CWait_Lost_Zombie::Create(void* pArg)
{
	CWait_Lost_Zombie* pInstance = { new CWait_Lost_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CWait_Lost_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWait_Lost_Zombie::Free()
{
	__super::Free();
}
