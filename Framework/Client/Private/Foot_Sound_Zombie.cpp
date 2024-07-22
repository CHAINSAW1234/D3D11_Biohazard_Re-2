#include "stdafx.h"
#include "GameInstance.h"
#include "Foot_Sound_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CFoot_Sound_Zombie::CFoot_Sound_Zombie()
	: CTask_Node()
{
}

CFoot_Sound_Zombie::CFoot_Sound_Zombie(const CFoot_Sound_Zombie& rhs)
{
}

HRESULT CFoot_Sound_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CFoot_Sound_Zombie::Enter()
{

}

_bool CFoot_Sound_Zombie::Execute(_float fTimeDelta)
{
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion	

	if (false == m_isStart)
	{
		CModel* pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
		if (nullptr == pBody_Model)
			return false;

		m_pL_Ball_Combiend = pBody_Model->Get_CombinedMatrix("l_leg_ball");
		m_pR_Ball_Combiend = pBody_Model->Get_CombinedMatrix("r_leg_ball");
		m_pRoot_Combiend = pBody_Model->Get_CombinedMatrix("root");

		if (nullptr == m_pL_Ball_Combiend || nullptr == m_pR_Ball_Combiend || nullptr == m_pRoot_Combiend)
			return false;

		_float4				vL_Ball_Position_Local = Get_Current_L_Ball_Position_Local();
		_float4				vR_Ball_Position_Local = Get_Current_R_Ball_Position_Local();

		m_vPre_L_Ball_Position_Local = vL_Ball_Position_Local;
		m_vPre_R_Ball_Position_Local = vR_Ball_Position_Local;

		m_isStart = true;
	}

	if (CZombie::POSE_STATE::_UP != m_pBlackBoard->Get_AI()->Get_PoseState())
	{
		_float4				vL_Ball_Position_Local_Float3 = { Get_Current_L_Ball_Position_Local() };
		_float4				vR_Ball_Position_Local_Float3 = { Get_Current_R_Ball_Position_Local() };
		_float4				vRoot_Position_Local_Float3 = { Get_Current_Root_Position_Local() };

		_float				fL_Ball_Hegiht = { vL_Ball_Position_Local_Float3.y };
		_float				fR_Ball_Hegiht = { vR_Ball_Position_Local_Float3.y };
		_float				fRootHeight = { vRoot_Position_Local_Float3.y };

		_float				fDistanceYToLBall = { fL_Ball_Hegiht - fRootHeight };
		_float				fDistanceYToRBall = { fR_Ball_Hegiht - fRootHeight };

		_float				fRange = { 6.f };

		if (fRange < fDistanceYToLBall)
		{
			m_isUp_L_Leg = true;
		}

		if (fRange < fDistanceYToRBall)
		{
			m_isUp_R_Leg = true;
		}

		if (fRange > fDistanceYToLBall && true == m_isUp_L_Leg)
		{
			m_pBlackBoard->Get_AI()->Play_Random_Foot_Sound();
			m_isUp_L_Leg = false;
		}

		if (fRange > fDistanceYToRBall && true == m_isUp_R_Leg)
		{
			m_pBlackBoard->Get_AI()->Play_Random_Foot_Sound();
			m_isUp_R_Leg = false;
		}

		m_vPre_L_Ball_Position_Local = vL_Ball_Position_Local_Float3;
		m_vPre_R_Ball_Position_Local = vR_Ball_Position_Local_Float3;
	}

	else
	{
		MONSTER_STATE			eMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() };
		if (eMonsterState == MONSTER_STATE::MST_WALK_LOST)
		{
			m_fAccCreepFootSoundCool += fTimeDelta;
			if (m_fAccCreepFootSoundCool >= m_fTotalCreepFootSoundTime)
			{
				m_fAccCreepFootSoundCool = 0.f;
				m_fTotalCreepFootSoundTime = m_pGameInstance->GetRandom_Real(2.f, 3.f);
				m_pBlackBoard->Get_AI()->Play_Random_Foot_Creep_Sound();
			}
		}
		else
		{
			m_fAccCreepFootSoundCool = 0.f;
		}
	}
	

	

	return true;
}

void CFoot_Sound_Zombie::Exit()
{
}

void CFoot_Sound_Zombie::Update_Current_Bone_Positions_Local()
{
}

_float4 CFoot_Sound_Zombie::Get_Current_L_Ball_Position_Local()
{
	return *(_float4*)(&m_pL_Ball_Combiend->m[CTransform::STATE_POSITION][0]);
}

_float4 CFoot_Sound_Zombie::Get_Current_R_Ball_Position_Local()
{
	return *(_float4*)(&m_pR_Ball_Combiend->m[CTransform::STATE_POSITION][0]);
}

_float4 CFoot_Sound_Zombie::Get_Current_Root_Position_Local()
{
	return *(_float4*)(&m_pRoot_Combiend->m[CTransform::STATE_POSITION][0]);
}

CFoot_Sound_Zombie* CFoot_Sound_Zombie::Create(void* pArg)
{
	CFoot_Sound_Zombie* pInstance = { new CFoot_Sound_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CFoot_Sound_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFoot_Sound_Zombie::Free()
{
	__super::Free();
}
