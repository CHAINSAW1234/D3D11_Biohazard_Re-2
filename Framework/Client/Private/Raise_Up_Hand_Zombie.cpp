#include "stdafx.h"
#include "GameInstance.h"
#include "Raise_Up_Hand_Zombie.h"
#include "Player.h"
#include "BlackBoard_Zombie.h"
#include "Zombie.h"
#include "Body_Zombie.h"

CRaise_Up_Hand_Zombie::CRaise_Up_Hand_Zombie()
	: CTask_Node()
{
}

CRaise_Up_Hand_Zombie::CRaise_Up_Hand_Zombie(const CRaise_Up_Hand_Zombie& rhs)
{
}

HRESULT CRaise_Up_Hand_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CRaise_Up_Hand_Zombie::Enter()
{
	
}

_bool CRaise_Up_Hand_Zombie::Execute(_float fTimeDelta)
{
	return true;

#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion	

	if (false == m_pBlackBoard->Is_LookTarget())
	{
		if (0.f >= m_fAccActiveTime)
			return true;

		m_fAccActiveTime -= fTimeDelta;
		if (0.f > m_fAccActiveTime)
		{
			m_fAccActiveTime = 0.f;
		}
	}

	else
	{
		m_fAccActiveTime += fTimeDelta;
		if (m_fAccActiveTime > ZOMBIE_RAISE_UP_HAND_MAX_TIME)
			m_fAccActiveTime = ZOMBIE_RAISE_UP_HAND_MAX_TIME;
	}	

	Set_Hand_AdditionalMatrices();

	m_pBlackBoard->Get_AI()->Active_IK_Body(true);

	CModel* pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBody_Model)
		return false;

	MONSTER_STATE			eMonsterState = { m_pBlackBoard->Get_AI()->Get_Current_MonsterState() }; 
		
	if (MONSTER_STATE::MST_IDLE != eMonsterState &&
		MONSTER_STATE::MST_WALK != eMonsterState &&
		MONSTER_STATE::MST_TURN != eMonsterState)
	{
		CModel* pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };
		pBody_Model->Set_Blend_IK(ZOMBIE_IK_L_HUMEROUS_RADIUS_TAG, 0.f);
		pBody_Model->Set_Blend_IK(ZOMBIE_IK_R_HUMEROUS_RADIUS_TAG, 0.f);
	}

	return true;
}

void CRaise_Up_Hand_Zombie::Exit()
{
}

void CRaise_Up_Hand_Zombie::Set_Hand_AdditionalMatrices()
{
	CTransform*			pZombie_Transform = { m_pBlackBoard->Get_AI()->Get_Transform() };
	CTransform*			pPlayer_Transform = { m_pBlackBoard->Get_Player()->Get_Transform() };
	CModel*				pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };

	if (nullptr == pZombie_Transform || nullptr == pPlayer_Transform || nullptr == pBody_Model)
		return;

	_matrix				ZombieCombiend_L_Humerous_Matrix = { XMLoadFloat4x4(pBody_Model->Get_CombinedMatrix(ZOMBIE_LEFT_HUMEROUS_BONE_TAG)) };
	_matrix				ZombieCombiend_R_Humerous_Matrix = { XMLoadFloat4x4(pBody_Model->Get_CombinedMatrix(ZOMBIE_RIGHT_HUMEROUS_BONE_TAG)) };

	_matrix				ZombieWorldMatrix = { pZombie_Transform->Get_WorldMatrix() };
	_matrix				L_Hurmerous_WorldMatrix = { ZombieCombiend_L_Humerous_Matrix * ZombieWorldMatrix };
	_matrix				R_Hurmerous_WorldMatrix = { ZombieCombiend_R_Humerous_Matrix * ZombieWorldMatrix };

	_vector				vL_Humerous_WorldPosition = { L_Hurmerous_WorldMatrix.r[CTransform::STATE_POSITION] };
	_vector				vR_Humerous_WorldPosition = { R_Hurmerous_WorldMatrix.r[CTransform::STATE_POSITION] };
	_vector				vZombie_WorldPosition = { ZombieWorldMatrix.r[CTransform::STATE_POSITION] };

	_vector				vDirection_To_L_Humerous = { vL_Humerous_WorldPosition - vZombie_WorldPosition };
	_vector				vDirection_To_R_Humerous = { vR_Humerous_WorldPosition - vZombie_WorldPosition };

	_float				fDistance = { 3.f };
	_vector				vTargetPosition = { pPlayer_Transform->Get_State_Vector(CTransform::STATE_POSITION) };

	_float				fRatio = { fminf(m_fAccActiveTime / ZOMBIE_RAISE_UP_HAND_MAX_TIME, ZOMBIE_RAISE_UP_HAND_MAX_RATIO) };

	_matrix				ZombieHeadCombinedMatrix = { XMLoadFloat4x4(pBody_Model->Get_CombinedMatrix("head")) };
	_matrix				ZombieHeadWorldMatrix = { ZombieHeadCombinedMatrix * ZombieWorldMatrix };
	_vector				ZombieHeadLookDir = { XMVector3Normalize(ZombieHeadWorldMatrix.r[CTransform::STATE_LOOK]) * -1.f };
	_vector				ZombieWorldLookDir = { XMVector3Normalize(ZombieWorldMatrix.r[CTransform::STATE_LOOK]) };

	_vector				ResultLookDir = { XMVector3Normalize(ZombieHeadLookDir + ZombieWorldLookDir) };

	_vector				ZombieHeadPosition = { ZombieHeadWorldMatrix.r[CTransform::STATE_POSITION] };

	vTargetPosition = ZombieHeadPosition + ResultLookDir * fDistance;

	pBody_Model->Set_TargetPosition_IK(ZOMBIE_IK_L_HUMEROUS_RADIUS_TAG, vTargetPosition );
	pBody_Model->Set_Blend_IK(ZOMBIE_IK_L_HUMEROUS_RADIUS_TAG, fRatio);
	pBody_Model->Set_TargetPosition_IK(ZOMBIE_IK_R_HUMEROUS_RADIUS_TAG, vTargetPosition );
	pBody_Model->Set_Blend_IK(ZOMBIE_IK_R_HUMEROUS_RADIUS_TAG, fRatio);
}

CRaise_Up_Hand_Zombie* CRaise_Up_Hand_Zombie::Create(void* pArg)
{
	CRaise_Up_Hand_Zombie* pInstance = { new CRaise_Up_Hand_Zombie() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CRaise_Up_Hand_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRaise_Up_Hand_Zombie::Free()
{
	__super::Free();
}
