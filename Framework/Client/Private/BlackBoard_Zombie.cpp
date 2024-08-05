#include "stdafx.h"
#include "GameInstance.h"
#include "BlackBoard_Zombie.h"
#include "Player.h"
#include "Zombie.h"

#include "Window.h"
#include "Door.h"
#include "Room_Finder.h"

#include "Part_Breaker_Zombie.h"

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

	m_pPart_Breaker = pDesc->pPart_Breaker;
	Safe_AddRef(m_pPart_Breaker);

	if (nullptr == m_pPart_Breaker)
		return E_FAIL;
	
	m_pAI = pDesc->pAI;
	if (nullptr == m_pAI)
		return E_FAIL;

	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	auto pPlayerLayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, L"Layer_Player");
	m_pPlayer = dynamic_cast<CPlayer*>(*(*pPlayerLayer).begin());
	return S_OK;
}

HRESULT CBlackBoard_Zombie::SetUp_Nearest_Window()
{

	if (ZOMBIE_START_TYPE::_OUT_DOOR != m_pAI->Get_StartType())
		return S_OK;

	CWindow*		pWindow = { dynamic_cast<CWindow*>(Find_NearestObejct_In_Layer_SameFloor(TEXT("Layer_Window"))) };
	if (nullptr == pWindow)
#ifdef MAP_NOTHING
		return S_OK;
#else
		return E_FAIL;
#endif

	Release_Nearest_Window();
	m_pNearest_Window = pWindow;
	Safe_AddRef(pWindow);

	return S_OK;
}

HRESULT CBlackBoard_Zombie::SetUp_Nearest_Door()
{


	if (ZOMBIE_START_TYPE::_DOOR_RUB != m_pAI->Get_StartType())
		return S_OK;

	CDoor*		pDoor = { dynamic_cast<CDoor*>(Find_NearestObejct_In_Layer_SameFloor(TEXT("Layer_Door"))) };
	if (nullptr == pDoor)
#ifdef MAP_NOTHING
		return S_OK;
#else
		return E_FAIL;
#endif

	Release_Nearest_Door();
	m_pNearest_Door = pDoor;
	Safe_AddRef(pDoor);

	return S_OK;
}

void CBlackBoard_Zombie::Set_Target_Object(CGameObject* pTargetObject)
{
	Safe_Release(m_pTarget_Object);
	m_pTarget_Object = pTargetObject;
	Safe_AddRef(m_pTarget_Object);
}

void CBlackBoard_Zombie::Priority_Tick(_float fTimeDelta)
{
	Update_Timers(fTimeDelta);
	Update_Status(fTimeDelta);
	Update_Look_Target(fTimeDelta);
	Update_New_Part_Break();

	_bool			isSameLocation = { m_pAI->Is_In_Location(static_cast<LOCATION_MAP_VISIT>(m_pPlayer->Get_Player_Region())) };

	ZOMBIE_START_TYPE				eStart = { m_pAI->Get_StartType() };

	if (eStart != ZOMBIE_START_TYPE::_HIDE_LOCKER)
	{
		if (true == isSameLocation)
		{
			m_pAI->Set_Render_RoomCulling(true);
		}
		else
		{
			m_pAI->Set_Render_RoomCulling(CRoom_Finder::Get_Instance()->Is_Linked_Location_From_Location(m_pAI->Get_Location(), static_cast<LOCATION_MAP_VISIT>(m_pPlayer->Get_Player_Region())));
		}
	}
	
	else
	{
		m_pAI->Set_Render_RoomCulling(true);
	}
}

void CBlackBoard_Zombie::Late_Tick(_float fTimeDelta)
{
}

void CBlackBoard_Zombie::Update_Timers(_float fTimeDelta)
{
	Update_Recognition_Timer(fTimeDelta);
	Update_StandUp_Timer(fTimeDelta);
	Update_Hold_Timer(fTimeDelta);
	Update_LightlyHold_Timer(fTimeDelta);
}

void CBlackBoard_Zombie::Update_Recognition_Timer(_float fTimeDelta)
{
	if (nullptr == m_pAI)
		return;

	MONSTER_STATE					eCurrentState = { m_pAI->Get_Current_MonsterState() };
	CMonster::MONSTER_STATUS*		pMonsterStatus = { m_pAI->Get_Status_Ptr() };

	HIT_TYPE						eHitType = { m_pAI->Get_Current_HitType() };
	if (HIT_TYPE::HIT_END != eHitType)
	{
		pMonsterStatus->fAccRecognitionTime = pMonsterStatus->fMaxRecognitionTime;		
	}

	_float							fDistanceToPlayer = {};
	if (false == Compute_Distance_To_Player(&fDistanceToPlayer))
		return;

	_bool							isLookTarget = { m_pAI->Is_LookTarget() };
	_bool							isInRange = { false};
	if (true == isLookTarget)
	{
		isInRange = fDistanceToPlayer <= pMonsterStatus->fRecognitionRange_LookTarget;
	}
	else
	{
		isInRange = fDistanceToPlayer <= pMonsterStatus->fRecognitionRange;
	}

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
}

void CBlackBoard_Zombie::Update_StandUp_Timer(_float fTimeDelta)
{
	if (nullptr == m_pAI)
		return;

	CZombie::POSE_STATE				ePoseState = { m_pAI->Get_PoseState() };
	CMonster::MONSTER_STATUS*		pMonsterStatus = { m_pAI->Get_Status_Ptr() };

	if(CZombie::POSE_STATE::_CREEP != ePoseState)
	{
		pMonsterStatus->fAccCreepTime -= fTimeDelta;
		if (pMonsterStatus->fAccCreepTime < 0.f)
			pMonsterStatus->fAccCreepTime = 0.f;
		return;
	}

	pMonsterStatus->fAccCreepTime += fTimeDelta;

	if (pMonsterStatus->fAccCreepTime > pMonsterStatus->fTryStandUpTime)
		pMonsterStatus->fAccCreepTime = pMonsterStatus->fTryStandUpTime;
}

void CBlackBoard_Zombie::Update_LightlyHold_Timer(_float fTimeDelta)
{
	/*if (nullptr == m_pAI)
		return;

	MONSTER_STATE					eCurrentState = { m_pAI->Get_Current_MonsterState() };
	CMonster::MONSTER_STATUS*		pMonsterStatus = { m_pAI->Get_Status_Ptr() };
	if (MONSTER_STATE::MST_WALK != eCurrentState ||
		MONSTER_STATE::MST_IDLE != eCurrentState)
	{
		pMonsterStatus->fAccRecognitionTime -= fTimeDelta;
		if (pMonsterStatus->fAccRecognitionTime < 0.f)
			pMonsterStatus->fAccRecognitionTime = 0.f;
		return;
	}

	_float							fDistanceToPlayer = {};
	if (false == Compute_Distance_To_Player(&fDistanceToPlayer))
		return;

	_bool							isInRange = { fDistanceToPlayer <= pMonsterStatus->fRecognitionRange };
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
	}*/
}

void CBlackBoard_Zombie::Update_Hold_Timer(_float fTImeDelta)
{
	if (nullptr == m_pAI)
		return;

	MONSTER_STATE					eCurrentState = { m_pAI->Get_Current_MonsterState() };
	CMonster::MONSTER_STATUS*		pMonsterStatus = { m_pAI->Get_Status_Ptr() };

	_float							fDistanceToPlayer = {};
	if (false == Compute_Distance_To_Player_World(&fDistanceToPlayer))
		return;

	if (pMonsterStatus->fTryHoldRange < fDistanceToPlayer ||
		eCurrentState == MONSTER_STATE::MST_BITE ||
		eCurrentState == MONSTER_STATE::MST_HOLD)
	{
		pMonsterStatus->fAccHoldTime -= fTImeDelta;
		if (0.f > pMonsterStatus->fAccHoldTime)
			pMonsterStatus->fAccHoldTime = 0.f;
	}

	else
	{
		pMonsterStatus->fAccHoldTime += fTImeDelta;
	}
}

CGameObject* CBlackBoard_Zombie::Find_NearestObejct_In_Layer_SameFloor(const wstring& strLayerTag)
{
	if (nullptr == m_pAI)
		return nullptr;

	_float			fNearestDistance = { 100000.f };
	CGameObject*	pNearObject = { nullptr };

	MAP_FLOOR_TYPE			eZombieFloor = { MAP_FLOOR_TYPE::FLOOR_END };

	_matrix			WorldMatrix = { m_pAI->Get_Transform()->Get_WorldMatrix() };
	_float			fWorldPositionY = { WorldMatrix.r[CTransform::STATE_POSITION].m128_f32[1] };
	if (fWorldPositionY >= -1.f && fWorldPositionY < 3.2f)
		eZombieFloor = MAP_FLOOR_TYPE::FLOOR_1;

	else if (fWorldPositionY < 8.5f)
		eZombieFloor = MAP_FLOOR_TYPE::FLOOR_2;

	else if (fWorldPositionY < 11.5f)
		eZombieFloor = MAP_FLOOR_TYPE::FLOOR_3;

	else
		eZombieFloor = MAP_FLOOR_TYPE::FLOOR_FREE;

	list<CGameObject*>* pTargetLayer = { m_pGameInstance->Find_Layer(g_Level, strLayerTag) };
	if (nullptr == pTargetLayer)
		return nullptr;

	for (auto& pGameObject : *pTargetLayer)
	{
		_float			fDistance = {};
		if (false == Compute_Distance_To_Target(pGameObject, &fDistance))
			continue;

		MAP_FLOOR_TYPE			eTargetFloor = { MAP_FLOOR_TYPE::FLOOR_END };

		_matrix			TargetWorldMatrix = { pGameObject->Get_Transform()->Get_WorldMatrix() };
		_float			fTargetWorldPositionY = { TargetWorldMatrix.r[CTransform::STATE_POSITION].m128_f32[1] };
		if (fTargetWorldPositionY >= -1.f && fTargetWorldPositionY < 3.2f)
			eTargetFloor = MAP_FLOOR_TYPE::FLOOR_1;

		else if (fTargetWorldPositionY < 8.5f)
			eTargetFloor = MAP_FLOOR_TYPE::FLOOR_2;

		else if (fTargetWorldPositionY < 11.5f)
			eTargetFloor = MAP_FLOOR_TYPE::FLOOR_3;

		else
			eTargetFloor = MAP_FLOOR_TYPE::FLOOR_FREE;

		if (eTargetFloor != eZombieFloor)
			continue;

		if (fDistance < fNearestDistance)
		{
			fNearestDistance = fDistance;
			pNearObject = pGameObject;
		}
	}

	return pNearObject;
}

void CBlackBoard_Zombie::Release_Nearest_Window()
{
	Safe_Release(m_pNearest_Window); 
	m_pNearest_Window = nullptr;
}

CCustomCollider* CBlackBoard_Zombie::Get_Nearest_Window_CustomCollider()
{
	if (nullptr == m_pNearest_Window)
		return nullptr;

	return m_pNearest_Window->Get_CustomCollider_Ptr();
}

CCustomCollider* CBlackBoard_Zombie::Get_Target_Door_CustomCollider()
{
	if (nullptr == m_pTarget_Door)
		return nullptr;

	return m_pTarget_Door->Get_CustomCollider_Ptr();
}

void CBlackBoard_Zombie::Research_NearestDoor()
{
	Safe_Release(m_pNearest_Door);

	m_pNearest_Door = static_cast<CDoor*>(Find_NearestObejct_In_Layer_SameFloor(TEXT("Layer_Door")));
	Safe_AddRef(m_pNearest_Door);
}

void CBlackBoard_Zombie::Release_Nearest_Door()
{
	Safe_Release(m_pNearest_Door);
	m_pNearest_Door = nullptr;
}

void CBlackBoard_Zombie::Release_Target_Door()
{
	Safe_Release(m_pTarget_Door);
	m_pTarget_Door = nullptr;
}

CCustomCollider* CBlackBoard_Zombie::Get_Nearest_Door_CustomCollider()
{
	if (nullptr == m_pNearest_Door)
		return nullptr;

	return m_pNearest_Door->Get_CustomCollider_Ptr();
}

void CBlackBoard_Zombie::Update_Status(_float fTimeDelta)
{
	Update_Status_Stamina(fTimeDelta);
}

void CBlackBoard_Zombie::Update_Status_Stamina(_float fTimeDelta)
{
	CMonster::MONSTER_STATUS*			pMonsterStatus = { m_pAI->Get_Status_Ptr() };
	
	pMonsterStatus->fStamina += pMonsterStatus->fStaminaChargingPerSec * fTimeDelta;

	if (pMonsterStatus->fStamina > pMonsterStatus->fMaxStamina)
		pMonsterStatus->fStamina = pMonsterStatus->fMaxStamina;
}

void CBlackBoard_Zombie::Update_New_Part_Break()
{
	m_iNewBreakPartType = { m_pAI->Get_New_BreakPartType() };
	if (-1 == m_iNewBreakPartType)
		m_isNewPartBreak = false;

	else
		m_isNewPartBreak = true;
}

void CBlackBoard_Zombie::Update_Look_Target(_float fTImeDelta)
{
	CMonster::MONSTER_STATUS*			pMonsterStatus = { m_pAI->Get_Status_Ptr() };

	if (true == pMonsterStatus->fAccRecognitionTime > pMonsterStatus->fLookTrargetNeedTime)
	{
		m_pAI->Set_LookTarget(true);
	}
	else
	{
		m_pAI->Set_LookTarget(false);
	}
}

_bool CBlackBoard_Zombie::Hit_Player()
{
	if (nullptr == m_pAI || nullptr == m_pPlayer)
		return false;

	CMonster::MONSTER_STATUS*		pStatus = { m_pAI->Get_Status_Ptr() };
	if (nullptr == pStatus)
		return false;

	_int			iPlayerHp = { m_pPlayer->Get_Hp() };
	m_pPlayer->Set_Hp(iPlayerHp - static_cast<_int>(pStatus->fAttack));

	return true;
	
}

void CBlackBoard_Zombie::Set_TargetDoor(CDoor* pTargetDoor)
{
	Safe_Release(m_pTarget_Door);
	m_pTarget_Door = pTargetDoor;
	Safe_AddRef(m_pTarget_Door);
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

	CPartObject*		pPartObject = { m_pAI->Get_PartObject(CMonster::PART_BODY) };
	if (nullptr == pPartObject)
		return static_cast<_uint>(MOTION_TYPE::MOTION_END);

	CBody_Zombie*		pBodyObject = { dynamic_cast<CBody_Zombie*>(pPartObject) };
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

_bool CBlackBoard_Zombie::Is_BreaKPart(BREAK_PART ePart)
{
	if (nullptr == m_pPart_Breaker)
		return false;

	return m_pPart_Breaker->Is_BreaKPart(ePart);
}

_bool CBlackBoard_Zombie::Is_Break_L_Arm()
{
	if (true == Is_BreaKPart(BREAK_PART::_L_UPPER_HUMEROUS))
		return true;
	if (true == Is_BreaKPart(BREAK_PART::_L_LOWER_HUMEROUS))
		return true;
	if (true == Is_BreaKPart(BREAK_PART::_L_UPPER_RADIUS))
		return true;

	return false;
}

_bool CBlackBoard_Zombie::Is_Break_R_Arm()
{
	if (true == Is_BreaKPart(BREAK_PART::_R_UPPER_HUMEROUS))
		return true;
	if (true == Is_BreaKPart(BREAK_PART::_R_LOWER_HUMEROUS))
		return true;
	if (true == Is_BreaKPart(BREAK_PART::_R_UPPER_RADIUS))
		return true;

	return false;
}

_bool CBlackBoard_Zombie::Is_Break_L_Humerous()
{
	if (true == Is_BreaKPart(BREAK_PART::_L_UPPER_HUMEROUS))
		return true;
	if (true == Is_BreaKPart(BREAK_PART::_L_LOWER_HUMEROUS))
		return true;

	return false;
}

_bool CBlackBoard_Zombie::Is_Break_R_Humerous()
{
	if (true == Is_BreaKPart(BREAK_PART::_R_UPPER_HUMEROUS))
		return true;
	if (true == Is_BreaKPart(BREAK_PART::_R_LOWER_HUMEROUS))
		return true;

	return false;
}

_bool CBlackBoard_Zombie::Is_Break_L_Leg()
{
	if (true == Is_BreaKPart(BREAK_PART::_L_UPPER_FEMUR))
		return true;
	if (true == Is_BreaKPart(BREAK_PART::_L_LOWER_FEMUR))
		return true;
	if (true == Is_BreaKPart(BREAK_PART::_L_UPPER_TABIA))
		return true;
	if (true == Is_BreaKPart(BREAK_PART::_L_LOWER_TABIA))
		return true;

	return false;
}

_bool CBlackBoard_Zombie::Is_Break_R_Leg()
{
	if (true == Is_BreaKPart(BREAK_PART::_R_UPPER_FEMUR))
		return true;
	if (true == Is_BreaKPart(BREAK_PART::_R_LOWER_FEMUR))
		return true;
	if (true == Is_BreaKPart(BREAK_PART::_R_UPPER_TABIA))
		return true;
	if (true == Is_BreaKPart(BREAK_PART::_R_LOWER_TABIA))
		return true;

	return false;
}

_bool CBlackBoard_Zombie::Is_Break_L_Foot()
{
	if (true == Is_BreaKPart(BREAK_PART::_L_LOWER_TABIA))
		return true;

	return false;
}

_bool CBlackBoard_Zombie::Is_Break_R_Foot()
{
	if (true == Is_BreaKPart(BREAK_PART::_R_LOWER_TABIA))
		return true;

	return false;
}

_bool CBlackBoard_Zombie::Is_Break_L_Tabia()
{
	if (true == Is_BreaKPart(BREAK_PART::_L_UPPER_TABIA))
		return true;

	return false;
}

_bool CBlackBoard_Zombie::Is_Break_R_Tabia()
{
	if (true == Is_BreaKPart(BREAK_PART::_R_UPPER_TABIA))
		return true;

	return false;
}

_bool CBlackBoard_Zombie::Is_Break_L_Femur()
{
	if (true == Is_BreaKPart(BREAK_PART::_L_UPPER_FEMUR))
		return true;
	if (true == Is_BreaKPart(BREAK_PART::_L_LOWER_FEMUR))
		return true;

	return false;
}

_bool CBlackBoard_Zombie::Is_Break_R_Femur()
{
	if (true == Is_BreaKPart(BREAK_PART::_R_UPPER_FEMUR))
		return true;
	if (true == Is_BreaKPart(BREAK_PART::_R_LOWER_FEMUR))
		return true;

	return false;
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

_bool CBlackBoard_Zombie::Compute_Direction_To_Target_World(CGameObject* pTargetObject, _float3* pDirection)
{
	if (nullptr == pTargetObject || nullptr == pDirection)
		return false;

	_vector				vTargetPosition = { pTargetObject->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) };
	_vector				vZombiePosition = { m_pAI->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) };

	_vector				vDirectionToTarget = { vTargetPosition - vZombiePosition };

	XMStoreFloat3(pDirection, vDirectionToTarget);

	return true;
}

_bool CBlackBoard_Zombie::Compute_Direction_To_Target_Local(CGameObject* pTargetObject, _float3* pDirection)
{
	if (nullptr == pTargetObject || nullptr == pDirection)
		return false;

	if (false == Compute_Direction_To_Target_World(pTargetObject, pDirection))
		return false;

	_matrix				ZombieWorldMatrixInverse = { m_pAI->Get_Transform()->Get_WorldMatrix_Inverse() };
	_vector				vDirectionToTargetLocal = { XMVector3TransformNormal(XMLoadFloat3(pDirection), ZombieWorldMatrixInverse) };
	
	XMStoreFloat3(pDirection, vDirectionToTargetLocal);
	
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

_bool CBlackBoard_Zombie::Compute_Direction_From_Target_World(CGameObject* pTargetObject, _float3* pDirection)
{
	if (nullptr == pTargetObject || nullptr == pDirection)
		return false;

	_vector				vTargetPosition = { pTargetObject->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) };
	_vector				vZombiePosition = { m_pAI->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) };

	_vector				vDirectionFromTarget = { vZombiePosition - vTargetPosition };

	XMStoreFloat3(pDirection, vDirectionFromTarget);

	return true;
}

_bool CBlackBoard_Zombie::Compute_Direction_From_Target_Local(CGameObject* pTargetObject, _float3* pDirection)
{
	if (nullptr == pTargetObject || nullptr == pDirection)
		return false;

	if (false == Compute_Direction_From_Target_World(pTargetObject, pDirection))
		return false;

	_matrix				TargetWorldMatrixInverse = { pTargetObject->Get_Transform()->Get_WorldMatrix_Inverse() };
	_vector				vDirectionFromTargetLocal = { XMVector3TransformNormal(XMLoadFloat3(pDirection), TargetWorldMatrixInverse) };

	XMStoreFloat3(pDirection, vDirectionFromTargetLocal);

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

	if (nullptr == pDirection)
		return isSuccess;

	isSuccess = true;

	_vector				vAIPosition = { m_pAI->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) };
	_vector				vDirectionToTargetWorld = { XMVector3Normalize(vTargetPosition - vAIPosition) };

	_matrix				AIWorldMatrixInv = { m_pAI->Get_Transform()->Get_WorldMatrix_Inverse() };
	_vector				vDirectionToTargetLocal = { XMVector3TransformNormal(vDirectionToTargetWorld, AIWorldMatrixInv) };

	_bool				isRight = { XMVectorGetX(vDirectionToTargetLocal) > 0.f };
	_float				fDot = { XMVectorGetX(XMVector3Dot(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMVector3Normalize(vDirectionToTargetLocal))) };
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

_bool CBlackBoard_Zombie::Compute_Distance_To_Target(CGameObject* pTargetObject, _float* pDistance)
{
	if (nullptr == pTargetObject || nullptr == pDistance)
		return false;

	_vector				vTargetPosition = { pTargetObject->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) };
	_vector				vMyPosition = { m_pAI->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION) };

	_vector				vDirectionToTarget = { vTargetPosition - vMyPosition };
	*pDistance = XMVectorGetX(XMVector3Length(vDirectionToTarget));

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

_bool CBlackBoard_Zombie::Compute_DeltaMatrix_AnimFirstKeyFrame_From_Target(CTransform* pTargetTransform, _uint iPlayingIndex, _int iAnimIndex, const wstring& strAnimLayerTag, _float4x4* pDeltaMatrix)
{
	if (nullptr == pDeltaMatrix || nullptr == pTargetTransform)
		return false;

	if (-1 == iAnimIndex)
		return false;

	if (TEXT("") == strAnimLayerTag)
		return false;

	CModel*			pBody_Model = { Get_PartModel(CMonster::PART_BODY) };
	if (nullptr == pBody_Model)
		return false;

	_uint			iNumAnims = { static_cast<_uint>(pBody_Model->Get_Animation_Tags(strAnimLayerTag).size()) };
	if (static_cast<_int>(iNumAnims) <= iAnimIndex)
		return false;

	_matrix			TargetWorldMatrix = { pTargetTransform->Get_WorldMatrix() };
	_matrix			Zombie_WorldMatrix = { m_pAI->Get_Transform()->Get_WorldMatrix() };

	_vector			vZombieScale, vZombieQuaternion, vZombieTranslation;
	_vector			vTargetScale, vTargetQuaternion, vTargetTranslation;

	XMMatrixDecompose(&vZombieScale, &vZombieQuaternion, &vZombieTranslation, Zombie_WorldMatrix);
	XMMatrixDecompose(&vTargetScale, &vTargetQuaternion, &vTargetTranslation, TargetWorldMatrix);

	TargetWorldMatrix = { XMMatrixAffineTransformation(vZombieScale, XMVectorSet(0.f, 0.f, 0.f ,1.f), vTargetQuaternion, vTargetTranslation) };

	vector<CAnimation*>		Animations = { pBody_Model->Get_Animations(strAnimLayerTag) };

	_matrix			RootFirstKeyFrameMatrix = { pBody_Model->Get_FirstKeyFrame_Root_TransformationMatrix(strAnimLayerTag, iAnimIndex) };
	_matrix			ModelTransformMatrix = { XMLoadFloat4x4(&pBody_Model->Get_TransformationMatrix()) };

	_vector					vRootScale, vRootQuaternion, vRootTranslation;
	XMMatrixDecompose(&vRootScale, &vRootQuaternion, &vRootTranslation, RootFirstKeyFrameMatrix);

	vRootTranslation = XMVector3TransformNormal(vRootTranslation, ModelTransformMatrix);

	_vector			vRootRotateAxis = { XMVectorSetW(vRootQuaternion, 0.f) };
	vRootRotateAxis = XMVector3TransformNormal(vRootRotateAxis, ModelTransformMatrix);
	vRootQuaternion = XMVectorSetW(vRootRotateAxis, XMVectorGetW(vRootQuaternion));
	RootFirstKeyFrameMatrix = XMMatrixAffineTransformation(vRootScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRootQuaternion, vRootTranslation);

	_matrix			InterpolateTargetMatrix = { RootFirstKeyFrameMatrix * TargetWorldMatrix };

	_vector			vInterpolateScale, vInterpolateQuaternion, vInterpolateTranslation;
	XMMatrixDecompose(&vInterpolateScale, &vInterpolateQuaternion, &vInterpolateTranslation, InterpolateTargetMatrix);

	_vector			vDeltaQuaternion = { XMQuaternionMultiply(XMQuaternionNormalize(XMQuaternionInverse(vZombieQuaternion)), XMQuaternionNormalize(vInterpolateQuaternion)) };
	_vector			vDeltaTranslation = { vInterpolateTranslation - vZombieTranslation };

	_matrix			DeltaMatrix = { XMMatrixAffineTransformation(vZombieScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vDeltaQuaternion, vDeltaTranslation) };

	XMStoreFloat4x4(pDeltaMatrix, DeltaMatrix);
	
	return true;
}

_bool CBlackBoard_Zombie::Apply_Devide_Delta_Matrix(_float fRatio, _fmatrix DeltaMatrx)
{
	_vector				vScale, vQuaternion, vTranslation;
	XMMatrixDecompose(&vScale, &vQuaternion, &vTranslation, DeltaMatrx);

	_vector				vDevideQuaternion = { XMQuaternionSlerp(XMQuaternionIdentity(), XMQuaternionNormalize(vQuaternion), fRatio) };
	_vector				vDevideTranslation = { XMVectorSetW(vTranslation * fRatio, 0.f) };

	_matrix				WorldMatrix = { m_pAI->Get_Transform()->Get_WorldMatrix() };
	_vector				vWorldScale, vWorldQuaternion, vWorldTranslation;
	XMMatrixDecompose(&vWorldScale, &vWorldQuaternion, &vWorldTranslation, WorldMatrix);

	_vector				vResultQuaternion = { XMQuaternionMultiply(XMQuaternionNormalize(vWorldQuaternion), XMQuaternionNormalize(vDevideQuaternion)) };
	_vector				vResultTranslation = { XMVectorSetW(vWorldTranslation + vDevideTranslation, 1.f) };

	_matrix				AplliedMatrix = { XMMatrixAffineTransformation(vWorldScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vResultQuaternion, vResultTranslation) };
	m_pAI->Get_Transform()->Set_WorldMatrix(AplliedMatrix);

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

	Safe_Release(m_pNearest_Window);
	Safe_Release(m_pNearest_Door);
	Safe_Release(m_pTarget_Door);
	Safe_Release(m_pPart_Breaker);
	Safe_Release(m_pTarget_Object);
}

