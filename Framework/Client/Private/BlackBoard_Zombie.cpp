#include "stdafx.h"
#include "GameInstance.h"
#include "BlackBoard_Zombie.h"
#include "Player.h"
#include "Zombie.h"

#include "Body_Zombie.h"

CBlackBoard_Zombie::CBlackBoard_Zombie()
	: CBlackBoard()
{
}

CBlackBoard_Zombie::CBlackBoard_Zombie(const CBlackBoard_Zombie& rhs)
{
}

HRESULT CBlackBoard_Zombie::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlackBoard_Zombie::Initialize(void* pArg)
{
	return S_OK;
}

void CBlackBoard_Zombie::Initialize_BlackBoard(CZombie* pAI)
{
	if (nullptr == pAI)
		return;

	m_pAI = pAI;

	auto pPlayerLayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, L"Layer_Player");
	m_pPlayer = dynamic_cast<CPlayer*>(*(*pPlayerLayer).begin());
}

_uint CBlackBoard_Zombie::Get_Current_MotionType_Body()
{
	if (nullptr == m_pAI)
		return CBody_Zombie::MOTION_END;

	CPartObject*		pPartObject = { m_pAI->Get_PartObject(CMonster::PART_BODY) };
	if (nullptr == pPartObject)
		return CBody_Zombie::MOTION_END;

	CBody_Zombie*		pBodyObject = { dynamic_cast<CBody_Zombie*>(pPartObject) };
	if (nullptr == pBodyObject)
		return CBody_Zombie::MOTION_END;

	return pBodyObject->Get_Current_MotionType();
}

_uint CBlackBoard_Zombie::Get_Pre_MotionType_Body()
{
	if (nullptr == m_pAI)
		return CBody_Zombie::MOTION_END;

	CPartObject* pPartObject = { m_pAI->Get_PartObject(CMonster::PART_BODY) };
	if (nullptr == pPartObject)
		return CBody_Zombie::MOTION_END;

	CBody_Zombie* pBodyObject = { dynamic_cast<CBody_Zombie*>(pPartObject) };
	if (nullptr == pBodyObject)
		return CBody_Zombie::MOTION_END;

	return pBodyObject->Get_Pre_MotionType();
}

CModel* CBlackBoard_Zombie::Get_PartModel(_uint iPartType)
{
	if (nullptr == m_pAI)
		return nullptr;

	return m_pAI->Get_Model_From_PartObject(static_cast<CMonster::PART_ID>(iPartType));
}

_bool CBlackBoard_Zombie::Compute_Direction_To_Player(_float3* pDirection)
{
	_bool				isSuccess = { false };
	if (nullptr == m_pAI || nullptr == m_pPlayer || nullptr == pDirection)
		return isSuccess;

	CTransform*			pAITransform = { Get_Transform(m_pAI) };
	CTransform*			pPlayerTransform = { Get_Transform(m_pPlayer) };

	if (nullptr == pAITransform || nullptr == pPlayerTransform)
		return isSuccess;

	_vector				vAIPosition = { pAITransform->Get_State_Vector(CTransform::STATE_POSITION) };
	_vector				vPlayerPosition = { pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION) };

	_vector				vDirctionToPlayer = { vPlayerPosition - vAIPosition };
	XMStoreFloat3(pDirection, vDirctionToPlayer);

	isSuccess = true;

	return isSuccess;
}

_bool CBlackBoard_Zombie::Compute_Direction_To_Player_Local(_float3* pDirection)
{
	if (false == Compute_Direction_To_Player(pDirection))
		return false;

	_vector				vWorldDirection = { XMLoadFloat3(pDirection) };
	
	CTransform*			pAITransform = { Get_Transform(m_pAI) };
	if (nullptr == pAITransform)
		return false;

	_matrix				WorldMatrixInv = { pAITransform->Get_WorldMatrix_Inverse() };
	_vector				vLocalDirection = { XMVector3TransformNormal(vWorldDirection, WorldMatrixInv) };
	XMStoreFloat3(pDirection, vLocalDirection);

	return true;
}

CTransform* CBlackBoard_Zombie::Get_Transform(CGameObject* pObject)
{
	if (nullptr == pObject)
		return nullptr;

	CTransform*			pTransform = { dynamic_cast<CTransform*>(pObject->Get_Component(TEXT("Com_Transform"))) };
	
	return pTransform;
}

CTransform* CBlackBoard_Zombie::Get_Transform_AI()
{
	return Get_Transform(m_pAI);
}

_bool CBlackBoard_Zombie::Is_Start_Anim(_uint iPartID, _uint iAnimIndex)
{
	CPartObject*		pPartObject = { m_pAI->Get_PartObject(static_cast<CMonster::PART_ID>(iPartID)) };

	if (CMonster::PART_ID::PART_BODY == static_cast<CMonster::PART_ID>(iPartID))
	{
		CBody_Zombie*		pBodyZombie = { dynamic_cast<CBody_Zombie*>(pPartObject) };
		if (nullptr == pBodyZombie)
			return false;

		return pBodyZombie->Is_Start_Anim(iAnimIndex);
	}

	return false;
}

_bool CBlackBoard_Zombie::Is_Loop_Anim(_uint iPartID, _uint iAnimIndex)
{
	CPartObject*		pPartObject = { m_pAI->Get_PartObject(static_cast<CMonster::PART_ID>(iPartID)) };

	if (CMonster::PART_ID::PART_BODY == static_cast<CMonster::PART_ID>(iPartID))
	{
		CBody_Zombie*		pBodyZombie = { dynamic_cast<CBody_Zombie*>(pPartObject) };
		if (nullptr == pBodyZombie)
			return false;

		return pBodyZombie->Is_Loop_Anim(iAnimIndex);
	}

	return false;
}

_bool CBlackBoard_Zombie::Is_Move_Anim(_uint iPartID, _uint iAnimIndex)
{
	CPartObject*		pPartObject = { m_pAI->Get_PartObject(static_cast<CMonster::PART_ID>(iPartID)) };

	if (CMonster::PART_ID::PART_BODY == static_cast<CMonster::PART_ID>(iPartID))
	{
		CBody_Zombie*		pBodyZombie = { dynamic_cast<CBody_Zombie*>(pPartObject) };
		if (nullptr == pBodyZombie)
			return false;

		return pBodyZombie->Is_Move_Anim(iAnimIndex);
	}

	return false;
}

_bool CBlackBoard_Zombie::Is_Turn_Anim(_uint iPartID, _uint iAnimIndex)
{
	CPartObject*		pPartObject = { m_pAI->Get_PartObject(static_cast<CMonster::PART_ID>(iPartID)) };

	if (CMonster::PART_ID::PART_BODY == static_cast<CMonster::PART_ID>(iPartID))
	{
		CBody_Zombie*		pBodyZombie = { dynamic_cast<CBody_Zombie*>(pPartObject) };
		if (nullptr == pBodyZombie)
			return false;

		return pBodyZombie->Is_Turn_Anim(iAnimIndex);
	}

	return false;
}

CBlackBoard_Zombie* CBlackBoard_Zombie::Create()
{
	CBlackBoard_Zombie* pInstance = new CBlackBoard_Zombie();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBlackBoard_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlackBoard_Zombie::Free()
{
	__super::Free();
}

