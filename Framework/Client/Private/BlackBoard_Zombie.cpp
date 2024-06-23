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
		return static_cast<_uint>(MOTION_TYPE::MOTION_END);

	CPartObject*		pPartObject = { m_pAI->Get_PartObject(CMonster::PART_BODY) };
	if (nullptr == pPartObject)
		return static_cast<_uint>(MOTION_TYPE::MOTION_END);

	CBody_Zombie*		pBodyObject = { dynamic_cast<CBody_Zombie*>(pPartObject) };
	if (nullptr == pBodyObject)
		return static_cast<_uint>(MOTION_TYPE::MOTION_END);

	return static_cast<_uint>(pBodyObject->Get_Current_MotionType());
}

_uint CBlackBoard_Zombie::Get_Pre_MotionType_Body()
{
	if (nullptr == m_pAI)
		return static_cast<_uint>(MOTION_TYPE::MOTION_END);

	CPartObject* pPartObject = { m_pAI->Get_PartObject(CMonster::PART_BODY) };
	if (nullptr == pPartObject)
		return static_cast<_uint>(MOTION_TYPE::MOTION_END);

	CBody_Zombie* pBodyObject = { dynamic_cast<CBody_Zombie*>(pPartObject) };
	if (nullptr == pBodyObject)
		return static_cast<_uint>(MOTION_TYPE::MOTION_END);

	return static_cast<_uint>(pBodyObject->Get_Pre_MotionType());
}

CModel* CBlackBoard_Zombie::Get_PartModel(_uint iPartType)
{
	if (nullptr == m_pAI)
		return nullptr;

	return m_pAI->Get_Model_From_PartObject(static_cast<CMonster::PART_ID>(iPartType));
}

void CBlackBoard_Zombie::Reset_NonActive_Body(const list<_uint>& ActivePlayingIndices)
{
	CModel*			pBodyModel = { Get_PartModel(CZombie::PART_BODY) };
	if (nullptr == pBodyModel)
		return;

	_uint			iNumInfo = { pBodyModel->Get_NumPlayingInfos() };
	for (_uint i = 0; i < iNumInfo; ++i)
	{
		list<_uint>::const_iterator		iter = { find(ActivePlayingIndices.begin(), ActivePlayingIndices.end(), i) };
		if (iter == ActivePlayingIndices.end())
		{
			pBodyModel->Reset_PreAnimation(i);
			pBodyModel->Set_BlendWeight(i, 0.f, 0.3f);
		}
	}
}

_bool CBlackBoard_Zombie::Compute_Direction_To_Player_World(_float3* pDirection)
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
	if (false == Compute_Direction_To_Player_World(pDirection))
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

_bool CBlackBoard_Zombie::Compute_Player_Angle_XZ_Plane_Local(_float* pAngle)
{
	if (nullptr == pAngle)
		return false;

	_float3				vDirectionToPlayerFloat3 = {};
	if (false == Compute_Direction_To_Player_Local(&vDirectionToPlayerFloat3))
		return false;

	_vector				vDirectionToPlayer = { XMLoadFloat3(&vDirectionToPlayerFloat3) };
	_vector				vAbsoluteLookDirection = { XMVectorSet(0.f, 0.f, 1.f, 0.f) };

	_vector				vDirectionToPlayerXZ = { XMVectorSetY(vDirectionToPlayer, 0.f) };
	_vector				vAbsoluteLookDirectionXZ = { XMVectorSetY(vAbsoluteLookDirection, 0.f) };

	_float				fDot = { XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDirectionToPlayerXZ), XMVector3Normalize(vAbsoluteLookDirectionXZ))) };
	_float				fResultAngle = { acosf(fDot) };

	*pAngle = fResultAngle;

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

CModel* CBlackBoard_Zombie::Find_PartModel(_uint iPartID)
{
	if (nullptr == m_pAI)
		return nullptr;

	CPartObject*		pPartObject = { m_pAI->Get_PartObject(static_cast<CMonster::PART_ID>(iPartID)) };
	if (nullptr == pPartObject)
		return nullptr;

	CModel*				pModel = { dynamic_cast<CModel*>(pPartObject->Get_Component(TEXT("Com_Model"))) };
	
	return pModel;
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

_bool CBlackBoard_Zombie::Is_Move_CurrentAnim(_uint iPartID, _uint iAnimIndex)
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

vector<_float> CBlackBoard_Zombie::Get_BlendWeights(_uint iPartID)
{
	vector<_float>			BlendWeights;
	CModel*			pModel = { Find_PartModel(iPartID) };
	if (nullptr == pModel)
		return BlendWeights;

	_uint			iNumPlayingInfo = { pModel->Get_NumPlayingInfos() };
	BlendWeights.resize(iNumPlayingInfo);

	for (_uint i = 0; i < iNumPlayingInfo; ++i)
	{
		BlendWeights[i] = pModel->Get_BlendWeight(i);		
	}

	return BlendWeights;
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

