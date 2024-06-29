#include "stdafx.h"
#include "GameInstance.h"
#include "BlackBoard_Zombie.h"
#include "Player.h"
#include "Zombie.h"

CBlackBoard_Zombie::CBlackBoard_Zombie()
	: CBlackBoard()
{
}

CBlackBoard_Zombie::CBlackBoard_Zombie(const CBlackBoard_Zombie& rhs)
{
}
HRESULT CBlackBoard_Zombie::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	BLACKBOARD_ZOMBIE_DESC*			pDesc = { static_cast<BLACKBOARD_ZOMBIE_DESC*>(pArg) };

	m_pAI = pDesc->pAI;
	if (nullptr == m_pAI)
		return E_FAIL;

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	auto pPlayerLayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, L"Layer_Player");
	m_pPlayer = dynamic_cast<CPlayer*>(*(*pPlayerLayer).begin());
	return S_OK;
}

void CBlackBoard_Zombie::Priority_Tick(_float fTimeDelta)
{
	Update_Timers(fTimeDelta);
}

void CBlackBoard_Zombie::Late_Tick(_float fTimeDelta)
{
}

void CBlackBoard_Zombie::Update_Timers(_float fTimeDelta)
{
	Update_Recognition_Timer(fTimeDelta);
}

void CBlackBoard_Zombie::Update_Recognition_Timer(_float fTimeDelta)
{
	if (nullptr == m_pAI)
		return;

	MONSTER_STATE					eCurrentState = { m_pAI->Get_Current_MonsterState() };
	CMonster::MONSTER_STATUS*		pMonsterStatus = { m_pAI->Get_Status_Ptr() };
	if (MONSTER_STATE::MST_WALK != eCurrentState)
	{
		pMonsterStatus->fAccRecognitionTime -= fTimeDelta;
		if (pMonsterStatus->fAccRecognitionTime < 0.f)
			pMonsterStatus->fAccRecognitionTime = 0.f;
		return;
	}
		

	_float								fDistanceToPlayer = {};
	if (false == Compute_Distance_To_Player(&fDistanceToPlayer))
		return;

	_bool								isInRange = { fDistanceToPlayer <= pMonsterStatus->fRecognitionRange };
	if (true == isInRange)
	{
		pMonsterStatus->fAccRecognitionTime += fTimeDelta;

		if (pMonsterStatus->fAccRecognitionTime > pMonsterStatus->fMaxRecognitionTime)
			pMonsterStatus->fAccRecognitionTime = pMonsterStatus->fMaxRecognitionTime;
	}

	else
	{
		pMonsterStatus->fAccRecognitionTime -= fTimeDelta;
		if (pMonsterStatus->fAccRecognitionTime < 0.f)
			pMonsterStatus->fAccRecognitionTime = 0.f;
	}

#ifdef			_DEBUG
	
	cout << "Acc Time Recognition : " << to_string(pMonsterStatus->fAccRecognitionTime) << endl;

#endif
}

void CBlackBoard_Zombie::Set_Current_MotionType_Body(MOTION_TYPE eType)
{
	CBody_Zombie*		pBodyZombie = { Get_PartObject_Body() };
	if(nullptr == pBodyZombie)
		return;

	pBodyZombie->Set_MotionType(eType);
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

CZombie::ZOMBIE_STATUS* CBlackBoard_Zombie::Get_ZombieStatus_Ptr()
{
	CZombie::ZOMBIE_STATUS*		pStatus = { static_cast<CZombie::ZOMBIE_STATUS*>(m_pAI->Get_Status_Ptr()) };

	return pStatus;
}

_int CBlackBoard_Zombie::Get_Current_AnimIndex(CMonster::PART_ID ePartID, PLAYING_INDEX eIndex)
{
	CModel*			pModel = { Get_PartModel(ePartID) };
	if (nullptr == pModel)
		return -1;

	return pModel->Get_CurrentAnimIndex(static_cast<_uint>(eIndex));
}

wstring CBlackBoard_Zombie::Get_Current_AnimLayerTag(CMonster::PART_ID ePartID, PLAYING_INDEX eIndex)
{
	CModel* pModel = { Get_PartModel(ePartID) };
	if (nullptr == pModel)
		return TEXT("");

	return pModel->Get_CurrentAnimLayerTag(static_cast<_uint>(eIndex));
}

CModel* CBlackBoard_Zombie::Get_PartModel(_uint iPartType)
{
	if (nullptr == m_pAI)
		return nullptr;

	return m_pAI->Get_Model_From_PartObject(static_cast<CMonster::PART_ID>(iPartType));
}

_matrix CBlackBoard_Zombie::Get_FirstKeyFrame_RootTransformationMatrix(CMonster::PART_ID eID, const wstring& strAnimLayerTag, _int iAnimIndex)
{
	CModel*			pModel = { Get_PartModel(eID) };
	if (nullptr == pModel)
		return XMMatrixIdentity();

	return pModel->Get_FirstKeyFrame_Root_TransformationMatrix(strAnimLayerTag, iAnimIndex);
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

_bool CBlackBoard_Zombie::Compute_Distance_To_Player_World(_float* pDistance)
{
	_bool			isSuccess = { false };

	_float3				vDirectionToPlayer = {};
	if (false == Compute_Direction_To_Player_World(&vDirectionToPlayer))
		return isSuccess;

	*pDistance = XMVectorGetX(XMVector3Length(XMLoadFloat3(&vDirectionToPlayer)));

	isSuccess = true;
	return isSuccess;
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

_bool CBlackBoard_Zombie::Compute_Direction_From_Player_World(_float3* pDirection)
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

	_vector				vDirctionFromPlayer = { vAIPosition - vPlayerPosition };
	XMStoreFloat3(pDirection, vDirctionFromPlayer);

	isSuccess = true;

	return isSuccess;
}

_bool CBlackBoard_Zombie::Compute_Direction_From_Player_Local(_float3* pDirection)
{
	if (false == Compute_Direction_From_Player_World(pDirection))
		return false;

	_vector				vWorldDirection = { XMLoadFloat3(pDirection) };

	CTransform*			pPlayerTransform = { Get_Transform(m_pPlayer) };
	if (nullptr == pPlayerTransform)
		return false;

	_matrix				WorldMatrixInv = { pPlayerTransform->Get_WorldMatrix_Inverse() };
	_vector				vLocalDirection = { XMVector3TransformNormal(vWorldDirection, WorldMatrixInv) };
	XMStoreFloat3(pDirection, vLocalDirection);

	return true;
}

_bool CBlackBoard_Zombie::Compute_Direction_From_Hit_World(_float3* pDirection)
{
	_bool				isSuccess = { false };
	if (nullptr == m_pAI || nullptr == pDirection)
		return isSuccess;

	_vector				vDirectionFromHit = { XMLoadFloat3(&m_pAI->Get_Current_HitDirection()) };

	XMStoreFloat3(pDirection, vDirectionFromHit);
	isSuccess = true;

	return isSuccess;
}

_bool CBlackBoard_Zombie::Compute_Direction_From_Hit_Local(_float3* pDirection)
{
	_bool				isSuccess = { Compute_Direction_From_Hit_World(pDirection) };
	if (nullptr == m_pAI || false == isSuccess)
		return isSuccess;

	CTransform*			pAITransform = { Get_Transform(m_pAI) };
	if (nullptr == pAITransform)
		return isSuccess;

	_matrix				WorldMatrixInv = { pAITransform->Get_WorldMatrix_Inverse() };
	_vector				vDirectionFromHit = { XMLoadFloat3(pDirection) };

	_vector				vDirectionFromHitLocal = { XMVector3TransformNormal(vDirectionFromHit, WorldMatrixInv) };
	XMStoreFloat3(pDirection, vDirectionFromHitLocal);
	isSuccess = true;

	return isSuccess;
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

_bool CBlackBoard_Zombie::Compute_Direction_To_Player_8Direction_Local(DIRECTION* pDiection)
{
	_bool				isSuccess = { false };
	if (nullptr == m_pPlayer)
		return isSuccess;

	_vector				vPlayerPosition = { m_pPlayer->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) };

	return Compute_Direction_To_Target_8Direction_Local(vPlayerPosition, pDiection);
}

_bool CBlackBoard_Zombie::Compute_Direction_To_Player_4Direction_Local(DIRECTION* pDiection)
{
	_bool				isSuccess = { false };
	if (nullptr == m_pPlayer)
		return isSuccess;

	_vector				vPlayerPosition = { m_pPlayer->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) };

	return Compute_Direction_To_Target_4Direction_Local(vPlayerPosition, pDiection);
}

_bool CBlackBoard_Zombie::Compute_Direction_To_Target_8Direction_Local(_fvector vTargetPosition, DIRECTION* pDirection)
{
	_bool				isSuccess = { false };

	_vector				vAIPosition = { m_pAI->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) };
	_vector				vDirectionToTargetWorld = { XMVector3Normalize(vTargetPosition - vAIPosition) };

	_matrix				AIWorldMatrixInv = { m_pAI->Get_Transform()->Get_WorldMatrix_Inverse() };
	_vector				vDirectionToTargetLocal = { XMVector3TransformNormal(vDirectionToTargetWorld, AIWorldMatrixInv) };

	_bool				isRight = { XMVectorGetX(vDirectionToTargetLocal) > 0.f };
	_float				fDot = { XMVectorGetX(XMVector3Dot(XMVectorSet(0.f, 0.f, 1.f, 0.f), vDirectionToTargetLocal)) };
	_float				fAngle = { acosf(fDot) };

	
	if (XMConvertToRadians(22.5f) > fAngle)
	{
		*pDirection = DIRECTION::_F;
	}

	else if (XMConvertToRadians(67.5f) > fAngle)
	{
		if (true == isRight)
			*pDirection = DIRECTION::_FR;

		else
			*pDirection = DIRECTION::_FL;
	}

	else if (XMConvertToRadians(112.5f) > fAngle)
	{
		if (true == isRight)
			*pDirection = DIRECTION::_R;

		else
			*pDirection = DIRECTION::_L;
	}

	else if (XMConvertToRadians(157.5f) > fAngle)
	{
		if (true == isRight)
			*pDirection = DIRECTION::_BR;

		else
			*pDirection = DIRECTION::_BL;
	}

	else
	{
		*pDirection = DIRECTION::_B;
	}

	if (false == isSuccess)
		*pDirection = DIRECTION::_END;

	return isSuccess;
}

_bool CBlackBoard_Zombie::Compute_Direction_To_Target_4Direction_Local(_fvector vTargetPosition, DIRECTION* pDirection)
{
	_vector				vAIPosition = { m_pAI->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) };
	_vector				vDirectionToTargetWorld = { XMVector3Normalize(vTargetPosition - vAIPosition) };

	_matrix				AIWorldMatrixInv = { m_pAI->Get_Transform()->Get_WorldMatrix_Inverse() };
	_vector				vDirectionToTargetLocal = { XMVector3TransformNormal(vDirectionToTargetWorld, AIWorldMatrixInv) };
	_vector				vDirectionToTargetLocalXZPlane = { XMVectorSetY(vDirectionToTargetLocal, 0.f) };

	_bool				isRight = { XMVectorGetX(vDirectionToTargetLocal) > 0.f };
	_float				fDot = { XMVectorGetX(XMVector3Dot(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMVector3Normalize(vDirectionToTargetLocalXZPlane))) };
	_float				fAngle = { acosf(fDot) };


	if (XMConvertToRadians(22.5f) > fAngle)
	{
		*pDirection = DIRECTION::_F;
	}

	else if (XMConvertToRadians(112.5f) > fAngle)
	{
		if (true == isRight)
			*pDirection = DIRECTION::_R;

		else
			*pDirection = DIRECTION::_L;
	}

	else
	{
		*pDirection = DIRECTION::_B;
	}

	return true;
}

_bool CBlackBoard_Zombie::Compute_Distance_To_Player(_float* pDistance)
{
	_bool			isSuccess = { false };

	if (nullptr == m_pPlayer || nullptr == m_pAI)
		return isSuccess;

	_vector			vPlayerPosition = { m_pPlayer->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) };
	_vector			vAIPosition = { m_pAI->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) };

	_vector			vDirectionToPlayer = { vPlayerPosition - vAIPosition };
	_float			fDistance = { XMVectorGetX(XMVector3Length(vDirectionToPlayer)) };

	*pDistance = fDistance;
	isSuccess = true;
	
	return isSuccess;
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

_bool CBlackBoard_Zombie::Compute_HalfMatrix_Current_BiteAnim(const wstring& strBiteAnimLayerTag, _int iAnimIndex, _float4x4* pResultMatrix)
{
	if (nullptr == m_pAI || nullptr == m_pPlayer || nullptr == pResultMatrix)
		return false;

	CTransform*			pPlayerTransform = { m_pPlayer->Get_Transform() };
	CTransform*			pAITransform = { m_pAI->Get_Transform() };

	if (nullptr == pPlayerTransform || nullptr == pAITransform)
		return false;

	_matrix				PlayerWorldMatrix = { pPlayerTransform->Get_WorldMatrix() };
	_matrix				ZombieWorldMatrix = { pAITransform->Get_WorldMatrix() };

	_matrix				ResultMatix;
	_vector				vScalePlayer, vQuaternionPlayer, vTranslationPlayer;
	_vector				vScaleZombie, vQuaternionZombie, vTranslationZombie;
	XMMatrixDecompose(&vScalePlayer, &vQuaternionPlayer, &vTranslationPlayer, PlayerWorldMatrix);
	XMMatrixDecompose(&vScaleZombie, &vQuaternionZombie, &vTranslationZombie, ZombieWorldMatrix);

	//	_vector				vHalfScale = { vScalePlayer };
	_vector				vHalfScale = { XMVectorSet(1.f, 1.f, 1.f, 0.f) };
	_vector				vHalfQuaternion = { XMQuaternionSlerp(vQuaternionPlayer, vQuaternionZombie, 0.5f) };
	_vector				vHalfTranslation = { XMVectorLerp(vTranslationPlayer, vTranslationZombie, 0.5f) };

	ResultMatix = XMMatrixAffineTransformation(vHalfScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vHalfQuaternion, vHalfTranslation);
	XMStoreFloat4x4(pResultMatrix, ResultMatix);

	return true;
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

CBody_Zombie* CBlackBoard_Zombie::Get_PartObject_Body()
{
	if (nullptr == m_pAI)
		return nullptr;

	CPartObject* pPartObject = { m_pAI->Get_PartObject(static_cast<CMonster::PART_ID>(CMonster::PART_BODY)) };
	if (nullptr == pPartObject)
		return nullptr;

	CBody_Zombie* pPartBody = { dynamic_cast<CBody_Zombie*>(pPartObject) };
	if (nullptr == pPartBody)
		return nullptr;

	return pPartBody;
}

ZOMBIE_BODY_ANIM_GROUP CBlackBoard_Zombie::Get_Current_AnimGroup(PLAYING_INDEX eIndex)
{
	CBody_Zombie*			pPartBody = { Get_PartObject_Body() };
	if (nullptr == pPartBody)
		return ZOMBIE_BODY_ANIM_GROUP::_END;

	return pPartBody->Get_Current_AnimGroup(eIndex);
}

ZOMBIE_BODY_ANIM_TYPE CBlackBoard_Zombie::Get_Current_AnimType(PLAYING_INDEX eIndex)
{
	CBody_Zombie* pPartBody = { Get_PartObject_Body() };
	if (nullptr == pPartBody)
		return ZOMBIE_BODY_ANIM_TYPE::_END;

	return pPartBody->Get_Current_AnimType(eIndex);
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

CBlackBoard_Zombie* CBlackBoard_Zombie::Create(void* pArg)
{
	CBlackBoard_Zombie* pInstance = new CBlackBoard_Zombie();

	if (FAILED(pInstance->Initialize(pArg)))
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

