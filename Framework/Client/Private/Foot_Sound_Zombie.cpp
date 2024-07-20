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

	if (CZombie::POSE_STATE::_UP != m_pBlackBoard->Get_AI()->Get_PoseState())
		return true;

	if (false == m_isStart)
	{
		CModel*				pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
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

	_float4				vL_Ball_Position_Local_Float3 = { Get_Current_L_Ball_Position_Local() };
	_float4				vR_Ball_Position_Local_Float3 = { Get_Current_R_Ball_Position_Local() };
	_float4				vRoot_Position_Local_Float3 = { Get_Current_Root_Position_Local()};

	_float				fL_Ball_Hegiht = { vL_Ball_Position_Local_Float3.y };
	_float				fR_Ball_Hegiht = { vR_Ball_Position_Local_Float3.y };
	_float				fRootHeight = { vRoot_Position_Local_Float3.y };

	_float				fDistanceYToLBall = { fL_Ball_Hegiht - fRootHeight };
	_float				fDistanceYToRBall = { fR_Ball_Hegiht - fRootHeight };


	if (true == m_isUp_L_Leg)
		m_fAccIgnoreDownTime_L_Leg += fTimeDelta;
	else
		m_fAccIgnoreUpTime_L_Leg += fTimeDelta;

	if (true == m_isUp_R_Leg)
		m_fAccIgnoreDownTime_R_Leg += fTimeDelta;
	else
		m_fAccIgnoreUpTime_R_Leg += fTimeDelta;


	_float				fRange = { 100.f };
	if (fRange > fDistanceYToLBall)
	{
		_float				fPre_L_Ball_Height = { m_vPre_L_Ball_Position_Local.y };

		//	이전 발 위치보다 현재 발위치가 높은경우
		if (fPre_L_Ball_Height < fL_Ball_Hegiht)
		{
			//	발이 올라간다의 상태가 아니었을 경우
			if (false == m_isUp_L_Leg)
			{
				//	무시하기로한 시간보다 누적시간이 크다면
				if (m_fAccIgnoreUpTime_L_Leg > FOOT_UP_IGNORE_TIME)
				{
					m_isUp_L_Leg = true;
					m_fAccIgnoreUpTime_L_Leg = 0.f;

					m_pBlackBoard->Get_AI()->Play_Random_Foot_Sound();

					cout << "Play L Foot Sound" << endl;
				}
			}			
		}

		//	이전 발 위치보다 현재 발위치가 낮은 경우
		else
		{
			//	발이 내려간다의 상태가 아니었을 경우
			if (true == m_isUp_L_Leg)
			{
				//	무시하기로한 시간보다 누적시간이 크다면
				if (m_fAccIgnoreDownTime_L_Leg > FOOT_DOWN_IGNORE_TIME)
				{
					m_isUp_L_Leg = false;
					m_fAccIgnoreDownTime_L_Leg = 0.f;
				}
			}
		}
	}

	if (fRange > fDistanceYToRBall)
	{
		_float				fPre_R_Ball_Height = { m_vPre_R_Ball_Position_Local.y };

		//	이전 발 위치보다 현재 발위치가 높은경우
		if (fPre_R_Ball_Height < fR_Ball_Hegiht)
		{
			//	발이 올라간다의 상태가 아니었을 경우
			if (false == m_isUp_R_Leg)
			{
				//	무시하기로한 시간보다 누적시간이 크다면
				if (m_fAccIgnoreUpTime_L_Leg > FOOT_UP_IGNORE_TIME)
				{
					m_isUp_R_Leg = true;
					m_fAccIgnoreUpTime_R_Leg = 0.f;

					m_pBlackBoard->Get_AI()->Play_Random_Foot_Sound();

					cout << "Play R Foot Sound" << endl;
				}
			}
		}

		//	이전 발 위치보다 현재 발위치가 낮은 경우
		else
		{
			//	발이 내려간다의 상태가 아니었을 경우
			if (true == m_isUp_R_Leg)
			{
				//	무시하기로한 시간보다 누적시간이 크다면
				if (m_fAccIgnoreDownTime_L_Leg > FOOT_DOWN_IGNORE_TIME)
				{
					m_isUp_R_Leg = false;
					m_fAccIgnoreDownTime_R_Leg = 0.f;
				}
			}
		}
	}

	m_vPre_L_Ball_Position_Local = vL_Ball_Position_Local_Float3;
	m_vPre_R_Ball_Position_Local = vR_Ball_Position_Local_Float3;

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
