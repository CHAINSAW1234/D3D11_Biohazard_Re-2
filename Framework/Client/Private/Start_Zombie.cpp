#include "stdafx.h"
#include "GameInstance.h"
#include "Start_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"
#include "Stun_Zombie.h"

CStart_Zombie::CStart_Zombie()
	: CTask_Node()
{
}

CStart_Zombie::CStart_Zombie(const CStart_Zombie& rhs)
{
}

HRESULT CStart_Zombie::Initialize(void* pArg)
{
	m_isStart = true;

	return S_OK;
}

void CStart_Zombie::Enter()
{
}

_bool CStart_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	ZOMBIE_START_TYPE			eStartType = { m_pBlackBoard->Get_AI()->Get_StartType() };
	if (eStartType == ZOMBIE_START_TYPE::_OUT_DOOR)	
	{
		if(false == m_isStart)
			m_pBlackBoard->SetUp_Nearest_Window();

		_float		fDistanceToPlayer = { 0.f };
		if (false == m_pBlackBoard->Compute_Distance_To_Player_World(&fDistanceToPlayer))
			return false;

		if (3.f > fDistanceToPlayer)
		{
			m_pBlackBoard->Get_AI()->Set_Start(false);

#ifdef _DEBUG
			MSG_BOX(TEXT("Start! Out Door"));
#endif
		}
	}

	else if (eStartType == ZOMBIE_START_TYPE::_DOOR_RUB)
	{
		if (false == m_isStart)
			m_pBlackBoard->SetUp_Nearest_Door();

		if (true == m_pBlackBoard->Get_AI()->Is_In_Location(static_cast<LOCATION_MAP_VISIT>(m_pBlackBoard->Get_Player()->Get_Player_Region())))
		{
			m_pBlackBoard->Get_AI()->Set_Start(false);

#ifdef _DEBUG
			MSG_BOX(TEXT("Start! Door Rub"));
#endif
		}
	}

	else if (eStartType == ZOMBIE_START_TYPE::_CREEP)
	{
		if (true == m_pBlackBoard->Get_AI()->Is_In_Location(static_cast<LOCATION_MAP_VISIT>(m_pBlackBoard->Get_Player()->Get_Player_Region())))
		{
			m_pBlackBoard->Get_AI()->Set_Start(false);
			m_pBlackBoard->Get_AI()->Set_State(MONSTER_STATE::MST_IDLE);
			m_pBlackBoard->Get_AI()->Set_PoseState(CZombie::POSE_STATE::_CREEP);
			m_pBlackBoard->Get_AI()->Set_FaceState(CZombie::FACE_STATE::_UP);

#ifdef _DEBUG
			MSG_BOX(TEXT("Start! Creep"));
#endif
		}
	}

	else if (eStartType == ZOMBIE_START_TYPE::_IDLE)
	{
		if (true == m_pBlackBoard->Get_AI()->Is_In_Location(static_cast<LOCATION_MAP_VISIT>(m_pBlackBoard->Get_Player()->Get_Player_Region())))
		{
			m_pBlackBoard->Get_AI()->Set_Start(false);
			m_pBlackBoard->Get_AI()->Set_State(MONSTER_STATE::MST_IDLE);
			m_pBlackBoard->Get_AI()->Set_PoseState(CZombie::POSE_STATE::_UP);

#ifdef _DEBUG
			MSG_BOX(TEXT("Start! Idle"));
#endif
		}
	}

	else if(eStartType == ZOMBIE_START_TYPE::_RAG_DOLL)
	{
		if (true == m_pBlackBoard->Get_AI()->Is_In_Location(static_cast<LOCATION_MAP_VISIT>(m_pBlackBoard->Get_Player()->Get_Player_Region())))
		{
			m_pBlackBoard->Get_AI()->Set_Start(false);
			m_pBlackBoard->Get_AI()->SetRagdoll(0, _float4(), COLLIDER_TYPE(0));

#ifdef _DEBUG
			MSG_BOX(TEXT("Start! Rad Doll"));
#endif
		}
	}	


	m_isStart = false;

	return true;
}

void CStart_Zombie::Exit()
{
}

CStart_Zombie* CStart_Zombie::Create(void* pArg)
{
	CStart_Zombie*			pInstance = { new CStart_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CStart_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStart_Zombie::Free()
{
	__super::Free();
}
