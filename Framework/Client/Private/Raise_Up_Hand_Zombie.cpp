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
#pragma region Default Function
	if (nullptr == m_pBlackBoard)
		return false;

	if (Check_Permition_To_Execute() == false)
		return false;
#pragma endregion

	if (false == m_pBlackBoard->Is_LookTarget())
	{
		m_fAccActiveTime -= fTimeDelta;
		if (0.f > m_fAccActiveTime)
		{
			m_fAccActiveTime = 0.f;
			return true;
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

	return true;
}

void CRaise_Up_Hand_Zombie::Exit()
{
}

void CRaise_Up_Hand_Zombie::Set_Hand_AdditionalMatrices()
{
	CTransform*			pZombie_Transform = { m_pBlackBoard->Get_AI()->Get_Transform() };
	CTransform*			pPlayer_Transform = { m_pBlackBoard->Get_Player()->Get_Transform() };
	CModel* pBody_Model = { m_pBlackBoard->Get_PartModel(CMonster::PART_BODY) };

	if (nullptr == pZombie_Transform || nullptr == pPlayer_Transform || nullptr == pBody_Model)
		return;

	_matrix				PlayerWorldMatrix = { pPlayer_Transform->Get_WorldMatrix() };
	_matrix				PlayerHeadCombinedMatrix = { XMLoadFloat4x4(m_pBlackBoard->Get_Player()->Get_Body_Model()->Get_CombinedMatrix("head")) };
	
	_matrix				PlayerHeadWorldMatrix = { PlayerHeadCombinedMatrix * PlayerWorldMatrix };

	_vector				vPlayerHeadWorldPosition = { PlayerHeadWorldMatrix.r[CTransform::STATE_POSITION] };

	_float				fRatio = { m_fAccActiveTime / ZOMBIE_RAISE_UP_HAND_MAX_TIME };

	pBody_Model->Set_TargetPosition_IK(ZOMBIE_IK_L_HUMEROUS_WRIST_TAG, vPlayerHeadWorldPosition);
	pBody_Model->Set_Blend_IK(ZOMBIE_IK_L_HUMEROUS_WRIST_TAG, fRatio);
	pBody_Model->Set_TargetPosition_IK(ZOMBIE_IK_R_HUMEROUS_WRIST_TAG, vPlayerHeadWorldPosition);
	pBody_Model->Set_Blend_IK(ZOMBIE_IK_R_HUMEROUS_WRIST_TAG, fRatio);

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
