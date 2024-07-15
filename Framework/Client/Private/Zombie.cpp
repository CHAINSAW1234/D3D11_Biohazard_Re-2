#include "stdafx.h"
#include "..\Public\Zombie.h"
#include "Character_Controller.h"
#include "BehaviorTree.h"
#include "PathFinder.h"

#include "Body_Zombie.h"
#include "Face_Zombie.h"
#include "Clothes_Zombie.h"

#include "PartObject.h"

#include "Header_Package_Zombie.h"
#include "Player.h"
#include "Decal_Blood.h"
#include "Mesh.h"
#include "Effect_Header_Zombie.h"

#include"CustomCollider.h"
#include"Window.h"
#include"Door.h"

#include "Decal_SSD.h"
#include "Blood_Drop.h"
#include "Impact.h"
#include "Hit.h"

#include "Part_Breaker_Zombie.h"

#include "Room_Finder.h"

#define MODEL_SCALE 0.01f
#define BLOOD_COUNT 10
#define DECAL_COUNT 20
#define BIG_ATTACK_BLOOD_SIZE 6.f
#define BIG_ATTACK_BLOOD_SIZE_DROP 5.f
#define NORMAL_ATTACK_BLOOD_SIZE 4.f
#define NORMAL_ATTACK_BLOOD_SIZE_DROP 3.f
#define SHOTGUN_BLOOD_COUNT 8
#define BLOOD_DROP_COUNT 10
#define BLOOD_DROP_COUNT_STG 10

CZombie::CZombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CZombie::CZombie(const CZombie& rhs)
	: CMonster{ rhs }
{

}

HRESULT CZombie::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CZombie::Initialize(void* pArg)
{
	GAMEOBJECT_DESC			GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (pArg == nullptr)
		return E_FAIL;

	ZOMBIE_DESC* pDesc = (ZOMBIE_DESC*)pArg;

	m_eStartType = pDesc->eStart_Type;
	m_eLocation = pDesc->eLocation;

	if (ZOMBIE_START_TYPE::_OUT_DOOR == m_eStartType)
	{
		m_isOutDoor = true;
	}
	else
	{
		m_isOutDoor = false;
	}

	if (m_eLocation == LOCATION_MAP_VISIT::LOCATION_MAP_VISIT_END)
	{
		//	MSG_BOX(TEXT("Called : HRESULT CZombie::Initialize(void* pArg)Location 미설정"));
		//	return E_FAIL;

		m_eLocation = LOCATION_MAP_VISIT::ART_STORAGE;
	}

	if (ZOMBIE_START_TYPE::_END == m_eStartType)
	{
#ifdef _DEBUG
		MSG_BOX(TEXT("Called : HRESULT CZombie::Initialize(void* pArg)  Zombie Start Type 미설정 "));
#endif
		return E_FAIL;
	}

	ZOMBIE_BODY_TYPE					eBodyType = { pDesc->eBodyModelType };

	if (ZOMBIE_BODY_TYPE::_MALE == eBodyType)
	{
		ZOMBIE_MALE_DESC* pMaleDesc = { static_cast<ZOMBIE_MALE_DESC*>(pArg) };
		m_iFace_ID = static_cast<_int>(pMaleDesc->eFaceType);
		//	m_iHat_Type = static_cast<_int>(pMaleDesc->eHatType);
		m_iShirts_ID = static_cast<_int>(pMaleDesc->eShirtsType);
		m_iPants_ID = static_cast<_int>(pMaleDesc->ePantsType);
		m_iBody_ID = static_cast<_int>(ZOMBIE_BODY_TYPE::_MALE);
	}

	else if (ZOMBIE_BODY_TYPE::_FEMALE == eBodyType)
	{
		ZOMBIE_FEMALE_DESC* pFemaleDesc = { static_cast<ZOMBIE_FEMALE_DESC*>(pArg) };
		m_iFace_ID = static_cast<_int>(pFemaleDesc->eFaceType);
		m_iShirts_ID = static_cast<_int>(pFemaleDesc->eShirtsType);
		m_iPants_ID = static_cast<_int>(pFemaleDesc->ePantsType);
		m_iBody_ID = static_cast<_int>(ZOMBIE_BODY_TYPE::_FEMALE);
	}

	else if (ZOMBIE_BODY_TYPE::_MALE_BIG == eBodyType)
	{
		ZOMBIE_MALE_BIG_DESC* pMaleBigDesc = { static_cast<ZOMBIE_MALE_BIG_DESC*>(pArg) };
		m_iFace_ID = static_cast<_int>(pMaleBigDesc->eFaceType);
		m_iShirts_ID = static_cast<_int>(pMaleBigDesc->eShirtsType);
		m_iPants_ID = static_cast<_int>(pMaleBigDesc->ePantsType);
		m_iBody_ID = static_cast<_int>(ZOMBIE_BODY_TYPE::_MALE_BIG);
	}

	else
		return E_FAIL;

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Initialize_States()))
		return E_FAIL;

	if (FAILED(Initialize_PartBreaker()))
		return E_FAIL;

	//if (FAILED(Add_Components()))
	//	return E_FAIL;

	//if (FAILED(Add_PartObjects()))
	//	return E_FAIL;

	//if (FAILED(Initialize_PartModels()))
	//	return E_FAIL;

	m_iIndex = pDesc->Index;
	_float4 vPos = *(_float4*)pDesc->worldMatrix.m[3];
	vPos.y += 1.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_InteractObjVec.resize(JOMBIE_BEHAVIOR_COLLIDER_END);
	//	m_pModelCom->Set_Animation(rand() % 20, true);
	m_pTransformCom->Set_Scaled(MODEL_SCALE, MODEL_SCALE, MODEL_SCALE);

#pragma region AIController Setup

#pragma region Create Controller

	if (m_iBody_ID == (_int)ZOMBIE_BODY_TYPE::_FEMALE)
	{
		m_pController = m_pGameInstance->Create_Controller(m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION), &m_iIndex_CCT, this, 1.f, 0.35f, m_pTransformCom
			, m_pBodyModel->GetBoneVector(), "../Bin/Resources/Models/Zombie_Female/Body_Female.fbx");
	}

	if (m_iBody_ID == (_int)ZOMBIE_BODY_TYPE::_MALE)
	{
		m_pController = m_pGameInstance->Create_Controller(m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION), &m_iIndex_CCT, this, 1.f, 0.35f, m_pTransformCom
			, m_pBodyModel->GetBoneVector(), "../Bin/Resources/Models/Zombie_Male/Body_Male.fbx");
	}

	if (m_iBody_ID == (_int)ZOMBIE_BODY_TYPE::_MALE_BIG)
	{
		m_pController = m_pGameInstance->Create_Controller(m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION), &m_iIndex_CCT, this, 1.f, 0.35f, m_pTransformCom
			, m_pBodyModel->GetBoneVector(), "../Bin/Resources/Models/Zombie_Male_Big/Body_Male_Big.fbx");
	}

#pragma endregion

	m_pPathFinder = m_pGameInstance->Create_PathFinder();

	m_pNavigationCom->FindCell(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));
	_int iCurrentIndex = m_pNavigationCom->GetCurrentIndex();

	//m_pPathFinder->Initiate_PathFinding(iCurrentIndex, iCurrentIndex + 150, m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));
	m_vNextTarget = m_pPathFinder->GetNextTarget_Opt();

	m_vDir = Float4_Normalize(m_vNextTarget - m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));
	m_vDir.w = 0.f;

	CBlackBoard_Zombie::BLACKBOARD_ZOMBIE_DESC			Desc;
	Desc.pAI = this;
	m_pBlackBoard = CBlackBoard_Zombie::Create(&Desc);

	Init_BehaviorTree_Zombie();
#pragma endregion

#pragma region Effect
	m_BloodDelay = 20;
	Ready_Effect();
#pragma endregion

	Perform_Skinning();

#pragma region For SSD

	//m_pDecal_Layer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, L"Layer_Decal");

	for (size_t i = 0; i < DECAL_COUNT; ++i)
	{
		auto pDecal = new CDecal_SSD(m_pDevice, m_pContext);
		pDecal->Initialize(nullptr);
		m_vecDecal_SSD.push_back(pDecal);
	}

#pragma endregion


#pragma region DEFAULT SETUP

	m_isStart = true;

#pragma endregion
	return S_OK;
}

void CZombie::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	m_pBlackBoard->Priority_Tick(fTimeDelta);
}

void CZombie::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->IsPaused())
	{
		fTimeDelta = 0.f;
	}

	if (m_bEvent)
		m_eBeHavior_Col;

	if (!Distance_Culling())
	{
		for (auto& it : m_PartObjects)
		{
			if (it)
				it->SetCulling(true);
		}

		return;
	}
	else
	{
		for (auto& it : m_PartObjects)
		{
			if (it)
				it->SetCulling(false);
		}
	}

	__super::Tick(fTimeDelta);

	if (nullptr != m_pController && false == m_isManualMove && m_pController->GetDead() == false)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pController->GetPosition_Float4_Zombie());

#pragma region BehaviorTree 코드

	if (false == m_bRagdoll)
		m_pBehaviorTree->Initiate(fTimeDelta);

#pragma endregion

	_float4			vDirection = {};
	_vector			vRootMoveDir = { XMLoadFloat3(&m_vRootTranslation) };
	XMStoreFloat4(&vDirection, vRootMoveDir);

	if (m_pController && m_pController->GetDead() == false)
	{
		if (false == m_isManualMove)
		{
			m_pController->Move(vDirection, fTimeDelta);
		}

		else
		{
			_vector				vCurrentPosition = { m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION) };
			_vector				vResultPosition = { vCurrentPosition + vRootMoveDir };

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vResultPosition);
		}
	}
	XMStoreFloat3(&m_vRootTranslation, XMVectorZero());

#pragma region 예은 추가 - 이벤트
	//if (m_bEvent)
	//{
	//	_bool bBehavior = true;
	//	if (m_InteractObjVec[m_eBeHavior_Col] != nullptr)
	//		bBehavior = static_cast<CInteractProps*>(m_InteractObjVec[m_eBeHavior_Col])->Attack_Prop(m_pTransformCom);
	//	// bBehavior가 true이면 창문이 깨지거나 문이 열리거나 하고 있음
	//	// 

	//	m_fEventCoolTime += fTimeDelta;
	//}
	//if (m_fEventCoolTime > 10.f)
	//	m_bEvent = false; //이벤트 쿨탐 10초가 지나면 다시 이벤트를 할 수 있는 상태(중복 방지)

#pragma endregion


#pragma region 길찾기 임시 코드
	//if (m_bArrived == false)
	//{
	//	m_vDir = Float4_Normalize(m_vNextTarget - m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));
	//	m_vDir.w = 0.f;

	//	_float4 vDelta = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION) - m_vNextTarget;
	//	vDelta.y = 0.f;

	//	if (XMVectorGetX(XMVector3Length(XMLoadFloat4(&vDelta))) < 0.75f)
	//	{
	//		m_bArrived = true;
	//	}
	//	else
	//	{
	//		//if (m_pController)
	//		//	m_pController->Move(m_vDir * 0.015f, fTimeDelta);
	//	}
	//}
	//else
	//{
	//	m_vNextTarget = m_pPathFinder->GetNextTarget_Opt();
	//	m_vDir = Float4_Normalize(m_vNextTarget - m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));
	//	m_vDir.w = 0.f;
	//
	//	if (m_vNextTarget.x == 0 && m_vNextTarget.y == 0 && m_vNextTarget.z == 0)
	//		m_bArrived = true;
	//	else
	//		m_bArrived = false;
	//}
#pragma endregion

#pragma region Ragdoll 피격
	if (m_bRagdoll)
	{
		if (m_pController)
		{
			if (m_pController->Is_Hit())
			{
				m_iBloodCount = 0;

				m_bBigAttack = m_pController->IsBigAttack();

				if (m_bBigAttack)
				{
					SetBlood_STG();
				}

				/*For Blood Effect*/
#ifdef DECAL
				if (m_pController->Is_Hit_Decal_Ray())
				{
					m_bSetBlood = true;

					m_pImpact->Set_Render(true);
					m_pImpact->SetPosition(m_pController->GetBlockPoint());

					m_pController->Set_Hit_Decal_Ray(false);
				}

				m_BloodTime = GetTickCount64();
#endif
				m_pController->Set_Hit(false);
			}
		}
	}
#pragma endregion

	if (m_pController && m_bRagdoll == false)
	{
		if (m_pController->Is_Hit())
		{
			m_iBloodCount = 0;

			/*For Blood Effect*/
#ifdef DECAL
			_bool bDecalRay = false;
			bDecalRay = m_pController->Is_Hit_Decal_Ray();
			if (bDecalRay)
			{
				m_bSetBlood = true;

				m_pImpact->Set_Render(true);
				m_pImpact->SetPosition(m_pController->GetBlockPoint());

				m_pController->Set_Hit_Decal_Ray(false);
			}

			m_BloodTime = GetTickCount64();
#endif

			m_pController->Set_Hit(false);
			m_bBigAttack = m_pController->IsBigAttack();

			auto vForce = m_pController->Get_Force();
			auto eType = m_pController->Get_Hit_Collider_Type();


			if (m_pController->GetDead())
			{
				CPlayer::EQUIP		eEquip = m_pBlackBoard->Get_Player()->Get_Equip();

				m_bRagdoll = true;

				if (CPlayer::EQUIP::STG == eEquip)
				{
					vForce = vForce * 4.f;
				}

				for (auto& pPartObject : m_PartObjects)
				{
					if (nullptr != pPartObject)
						pPartObject->SetRagdoll(m_iIndex_CCT, vForce, eType);
				}
			}
		/*	else
			{
				if(bDecalRay)
				{
					auto Type = m_pController->Get_Hit_Collider_Type();

					if(Type != COLLIDER_TYPE::CHEST && Type != COLLIDER_TYPE::PELVIS && Type != COLLIDER_TYPE::HEAD
						&& Type != COLLIDER_TYPE::HAND_L && Type != COLLIDER_TYPE::HAND_R)
					{
						for (auto& pPartObject : m_PartObjects)
						{
							if (nullptr != pPartObject)
								pPartObject->SetPartialRagdoll(m_iIndex_CCT, vForce, eType);

							m_bPartial_Ragdoll = true;

							BREAK_PART eBreakType = BREAK_PART::_END;
							switch (eType)
							{
							case COLLIDER_TYPE::ARM_R:
								eBreakType = BREAK_PART::_R_UPPER_HUMEROUS;
								break;
							case COLLIDER_TYPE::ARM_L:
								eBreakType = BREAK_PART::_L_UPPER_HUMEROUS;
								break;	


							case COLLIDER_TYPE::FOREARM_R:
								eBreakType = BREAK_PART::_R_UPPER_RADIUS;
								break;
							case COLLIDER_TYPE::FOREARM_L:
								eBreakType = BREAK_PART::_L_UPPER_RADIUS;
								break;


							case COLLIDER_TYPE::LEG_R:
								eBreakType = BREAK_PART::_R_UPPER_FEMUR;
								break;
							case COLLIDER_TYPE::CALF_R:
								eBreakType = BREAK_PART::_R_UPPER_TABIA;
								break;


							case COLLIDER_TYPE::LEG_L:
								eBreakType = BREAK_PART::_L_UPPER_FEMUR;
								break;
							case COLLIDER_TYPE::CALF_L:
								eBreakType = BREAK_PART::_L_UPPER_TABIA;
								break;

							case COLLIDER_TYPE::FOOT_R:
								eBreakType = BREAK_PART::_R_LOWER_TABIA;
								break;
							case COLLIDER_TYPE::FOOT_L:
								eBreakType = BREAK_PART::_L_LOWER_TABIA;
								break;
							}

							m_pPart_Breaker->Break(eBreakType);
						}

						auto pBody = static_cast<CBody_Zombie*>(m_PartObjects[CZombie::PART_BODY]);
						m_pController->SetHitPart(pBody->Get_Ragdoll_RigidBody(Type));
					}
				}
			}*/

			if (m_bBigAttack)
			{
				SetBlood_STG();
			}

			//	For.Anim
			m_eCurrentHitCollider = eType;
			XMStoreFloat3(&m_vHitDirection, XMLoadFloat4(&vForce));

#pragma region HIT TYPE 분할 임시


			CPlayer::EQUIP		eEquip = m_pBlackBoard->Get_Player()->Get_Equip();
			if (CPlayer::EQUIP::HG == eEquip)
			{
				m_eCurrentHitType = HIT_TYPE::HIT_SMALL;
			}

			else if (CPlayer::EQUIP::STG == eEquip)
			{
				m_eCurrentHitType = HIT_TYPE::HIT_BIG;
			}

#pragma endregion
		}
		else
		{
			m_eCurrentHitType = HIT_TYPE::HIT_END;
			m_eCurrentHitCollider = COLLIDER_TYPE::_END;
			XMStoreFloat3(&m_vHitDirection, XMVectorZero());
		}
	}

	//For Decal.
	m_pColliderCom_Bounding->Tick(m_pTransformCom->Get_WorldMatrix_Pure_Mat());

#pragma region Effect

	Tick_Effect(fTimeDelta);

#pragma endregion
}

void CZombie::Late_Tick(_float fTimeDelta)
{
	m_pBlackBoard->Late_Tick(fTimeDelta);

	if (m_pGameInstance->IsPaused())
	{
		fTimeDelta = 0.f;
	}

	if (!Distance_Culling())
		return;

	__super::Late_Tick(fTimeDelta);

	if (m_pController && m_pController->GetDead() == false)
		m_pController->Update_Collider();

#pragma region 예은
	//if(!m_bEvent)
	//	Col_EventCol();

#pragma endregion 

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom_Bounding);
#endif

#pragma region Effect

	if (m_bSetBlood)
	{
		/*For Decal*/
#ifdef DECAL
		Ready_Decal();
		SetBlood();
#endif
	}

	Late_Tick_Effect(fTimeDelta);

#pragma endregion
}

HRESULT CZombie::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_NormalTexture", static_cast<_uint>(i), aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
			return E_FAIL;

		//	if (FAILED(m_pModelCom->Bind_PrevBoneMatrices(m_pShaderCom, "g_PrevBoneMatrices", static_cast<_uint>(i))))
		//		return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_AlphaTexture", static_cast<_uint>(i), aiTextureType_METALNESS)))
		{
			_bool isAlphaTexture = false;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isAlphaTexture", &isAlphaTexture, sizeof(_bool))))
				return E_FAIL;
		}
		else
		{
			_bool isAlphaTexture = true;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isAlphaTexture", &isAlphaTexture, sizeof(_bool))))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_AOTexture", static_cast<_uint>(i), aiTextureType_SHININESS)))
		{
			_bool isAOTexture = false;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isAOTexture", &isAOTexture, sizeof(_bool))))
				return E_FAIL;
		}
		else
		{
			_bool isAOTexture = true;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isAOTexture", &isAOTexture, sizeof(_bool))))
				return E_FAIL;
		}


		if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_DEFAULT)))
			return E_FAIL;

		m_pModelCom->Render(static_cast<_uint>(i));
	}

	return S_OK;
}

void CZombie::Init_BehaviorTree_Zombie()
{
	m_pBehaviorTree = m_pGameInstance->Create_BehaviorTree(&m_iAIController_ID);

	//Root Node
	auto pNode_Root = m_pBehaviorTree->GetRootNode();
	CComposite_Node::COMPOSITE_NODE_DESC		CompositeNodeDesc;

#pragma region RootNode Childe Section

#pragma region Sequence Root

	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SEQUENCE;
	CComposite_Node* pSequenceNode_Root = { CComposite_Node::Create(&CompositeNodeDesc) };
	pNode_Root->Insert_Child_Node(pSequenceNode_Root);

#pragma region Selector Root 
	//		Selector => 성공을 반환 받을 떄 까지
	//		Sequence => 실패를 반환 받을 떄 까지

	/*
	*Root Selector Section
	*/

	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SELECTOR;
	CComposite_Node* pSelectorNode_Root = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSequenceNode_Root->Insert_Child_Node(pSelectorNode_Root);

#pragma region TASK PRE TASK

	/*
	* Root CHild_Maintain Section ( Maintain_Check )
	*/

	//	Add Task Node		=> Execute Pre Task
	CExecute_PreTask_Zombie* pTask_ExecutePreTask = { CExecute_PreTask_Zombie::Create() };
	pTask_ExecutePreTask->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Root->Insert_Child_Node(pTask_ExecutePreTask);

	//	Add Decorator		=> Is Maintain?
	CIs_Maintain_PreTask_Zombie* pDeco_Maintain_PreTask = { CIs_Maintain_PreTask_Zombie::Create() };
	pDeco_Maintain_PreTask->SetBlackBoard(m_pBlackBoard);
	pTask_ExecutePreTask->Insert_Decorator_Node(pDeco_Maintain_PreTask);

#pragma endregion		//	Selector Root Child

#pragma region		Selector Start Set Up

	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SELECTOR;
	CComposite_Node* pSelectorNode_Start_SetUp = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSelectorNode_Root->Insert_Child_Node(pSelectorNode_Start_SetUp);

	CIs_Start_Zombie* pDeco_Is_Start = { CIs_Start_Zombie::Create() };
	pDeco_Is_Start->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Start_SetUp->Insert_Decorator_Node(pDeco_Is_Start);

#pragma region		//	Task Start

	CStart_Zombie* pTask_Start = { CStart_Zombie::Create() };
	pTask_Start->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Start_SetUp->Insert_Child_Node(pTask_Start);

#pragma endregion	//	Selector Start Set Up Child

#pragma endregion	//	Selector Root Child

#pragma region Selector Default

	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SELECTOR;
	CComposite_Node* pSelectorNode_Default = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSelectorNode_Root->Insert_Child_Node(pSelectorNode_Default);

#pragma region Selector Out Door Check

	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SELECTOR;
	CComposite_Node* pSelectorNode_OutDoorCheck = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSelectorNode_Default->Insert_Child_Node(pSelectorNode_OutDoorCheck);

	CIs_Out_Door_Zombie* pDeco_Is_Out_Door_Zombie = { CIs_Out_Door_Zombie::Create() };
	pDeco_Is_Out_Door_Zombie->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_OutDoorCheck->Insert_Decorator_Node(pDeco_Is_Out_Door_Zombie);

	//	TODO:		Is Trigger On 상태 필요함
	//	CIs_Trigger_On* pDeco_Is_Trigger_On = { CIs_Trigger_On::Create() };
	//	pDeco_Is_Trigger_On->SetBlackBoard(m_pBlackBoard);
	//	pSelectorNode_OutDoorCheck->Insert_Decorator_Node(pDeco_Is_Trigger_On);

	//	창문에 미 충돌
#pragma region Move To Window

	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SELECTOR;
	CComposite_Node* pSelectorNode_Move_To_Window = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSelectorNode_OutDoorCheck->Insert_Child_Node(pSelectorNode_Move_To_Window);

	CIs_Collision_Prop_Zombie* pDeco_Is_Non_Collision_WindowTrigger = { CIs_Collision_Prop_Zombie::Create(CIs_Collision_Prop_Zombie::COLL_PROP_TYPE::_WINDOW, CIs_Collision_Prop_Zombie::RETURN_TYPE::_REVERSE) };
	pDeco_Is_Non_Collision_WindowTrigger->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Move_To_Window->Insert_Decorator_Node(pDeco_Is_Non_Collision_WindowTrigger);

	//	 필요조건 => 위치 미 도달
	CMove_To_Target_Zombie* pTask_Move_To_Window = { CMove_To_Target_Zombie::Create() };
	pTask_Move_To_Window->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Move_To_Window->Insert_Child_Node(pTask_Move_To_Window);

#pragma endregion		//	Selector Out Door Chekc Child

#pragma region Selector Is Collision Window Trigger
	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SELECTOR;
	CComposite_Node* pSelectorNode_Interact_Window = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSelectorNode_OutDoorCheck->Insert_Child_Node(pSelectorNode_Interact_Window);

	CIs_Collision_Prop_Zombie* pDeco_Is_Collision_WindowTrigger = { CIs_Collision_Prop_Zombie::Create(CIs_Collision_Prop_Zombie::COLL_PROP_TYPE::_WINDOW, CIs_Collision_Prop_Zombie::RETURN_TYPE::_STARARIGHT) };
	pDeco_Is_Collision_WindowTrigger->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Interact_Window->Insert_Decorator_Node(pDeco_Is_Collision_WindowTrigger);

	//	창문에 도달
#pragma region BREAK_WINDOW || BREAK_IN_WINDOW || HOLD_OUT_HAND_WINDOW || IN_WINDOW || KNOCK_WINDOW


	//	Set_ManualMove true , Release Window	=> Knock Enter
	//	Set_ManualMove false, Release Window	=> In || Break IN Exit 

	//	필요 조건 => 위치 도달, 창문 깨짐, 바리게이트 쳐짐
	CHold_Out_Hand_Window_Zombie* pTask_Hold_Out_Hand_Window = { CHold_Out_Hand_Window_Zombie::Create() };
	pTask_Hold_Out_Hand_Window->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Interact_Window->Insert_Child_Node(pTask_Hold_Out_Hand_Window);

	//	Break와 Break_In은 Break_In 난수 처리후 실패시 Break로 ? 아니면 스테미너여부
	//	필요 조건 => 위치 도달, 창문 안깨졌으나 체력 0되는 순간에 Knock에서 이어지게끔, 바리게이트 안쳐짐
	CBreak_In_Window_Zombie* pTask_Break_In_Window = { CBreak_In_Window_Zombie::Create() };
	pTask_Break_In_Window->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Interact_Window->Insert_Child_Node(pTask_Break_In_Window);

	//	필요 조건 => 위치 도달, 창문 깨짐, 바리게이트 안쳐짐
	CIn_Window_Zombie* pTask_In_Window = { CIn_Window_Zombie::Create() };
	pTask_In_Window->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Interact_Window->Insert_Child_Node(pTask_In_Window);

	//	필요 조건 => 위치 도달, 창문 안깨졌으나 체력 0되는 순간에 Knock에서 이어지게끔, 바리게이트 상관 없음, 
	CBreak_Window_Zombie* pTask_Break_Window = { CBreak_Window_Zombie::Create() };
	pTask_Break_Window->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Interact_Window->Insert_Child_Node(pTask_Break_Window);

	//	필요 조건 => 위치 도달, 창문 안깨짐, 바리게이트 여부(상관없음)
	CKnock_Window_Zombie* pTask_Knock_Window = { CKnock_Window_Zombie::Create() };
	pTask_Knock_Window->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Interact_Window->Insert_Child_Node(pTask_Knock_Window);

#pragma endregion		//	Selector Is Collision Window Trigger

#pragma endregion		//	Selector Out Door Check Child

#pragma endregion		//	Selector Default Child

#pragma region In Door Check	

	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SELECTOR;
	CComposite_Node* pSelectorNode_InDoorCheck = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSelectorNode_Default->Insert_Child_Node(pSelectorNode_InDoorCheck);

	CIs_In_Door_Zombie* pDeco_Is_In_Door_Zombie = { CIs_In_Door_Zombie::Create() };
	pDeco_Is_In_Door_Zombie->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_InDoorCheck->Insert_Decorator_Node(pDeco_Is_In_Door_Zombie);

#pragma region Selector Start SetUp
//
//	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SELECTOR;
//	CComposite_Node* pSelectorNode_StartSetUp = { CComposite_Node::Create(&CompositeNodeDesc) };
//	pSelectorNode_InDoorCheck->Insert_Child_Node(pSelectorNode_StartSetUp);
//
//	CIs_Start_Rub_Door_Zombie* pDeco_Is_Start_Rub_Door = { CIs_Start_Rub_Door_Zombie::Create() };
//	pDeco_Is_Start_Rub_Door->SetBlackBoard(m_pBlackBoard);
//	pSelectorNode_StartSetUp->Insert_Child_Node(pDeco_Is_Start_Rub_Door);
//
#pragma region Rub Door
//
//	CRub_Door_Zombie* pTask_Rub_Door = { CRub_Door_Zombie::Create() };
//	pTask_Rub_Door->SetBlackBoard(m_pBlackBoard);
//	pSelectorNode_StartSetUp->Insert_Child_Node(pTask_Rub_Door);
//
//	CIs_Collision_Prop_Zombie* pDeco_Is_Collision_Door_Trigger = { CIs_Collision_Prop_Zombie::Create(CIs_Collision_Prop_Zombie::COLL_PROP_TYPE::_DOOR, CIs_Collision_Prop_Zombie::RETURN_TYPE::_STARARIGHT) };
//	pDeco_Is_Collision_Door_Trigger->SetBlackBoard(m_pBlackBoard);
//	pTask_Rub_Door->Insert_Decorator_Node(pDeco_Is_Collision_Door_Trigger);
//
#pragma endregion	Child Selector Start SetUp 
//
#pragma region Move To Door
//
//	CMove_To_Target_Zombie* pTask_Move_To_Door = { CMove_To_Target_Zombie::Create() };
//	pTask_Move_To_Door->SetBlackBoard(m_pBlackBoard);
//	pSelectorNode_StartSetUp->Insert_Child_Node(pTask_Move_To_Door);
//
//	//	문과 충돌하지않앗는지
//	CIs_Collision_Prop_Zombie* pDeco_Is_Collision_Door_Inverse_Trigger = { CIs_Collision_Prop_Zombie::Create(CIs_Collision_Prop_Zombie::COLL_PROP_TYPE::_DOOR, CIs_Collision_Prop_Zombie::RETURN_TYPE::_REVERSE) };
//	pDeco_Is_Collision_Door_Inverse_Trigger->SetBlackBoard(m_pBlackBoard);
//	pTask_Move_To_Door->Insert_Decorator_Node(pDeco_Is_Collision_Door_Inverse_Trigger);
//
#pragma endregion	Child Selector Start SetUp 	
#pragma endregion Selector In Door Check Child

#pragma region Sequence Different Location Player
	//	문을 찾는다 => 연결된 방이아닐시 다른 행동으로 넘아가서 Idle상태가됨

	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SEQUENCE;
	CComposite_Node* pSequecne_Different_Region_Player = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSelectorNode_InDoorCheck->Insert_Child_Node(pSequecne_Different_Region_Player);

	CIs_Different_Location_To_Player_Zombie* pDeco_Is_Different_Location_To_Player = { CIs_Different_Location_To_Player_Zombie::Create() };
	pDeco_Is_Different_Location_To_Player->SetBlackBoard(m_pBlackBoard);
	pSequecne_Different_Region_Player->Insert_Decorator_Node(pDeco_Is_Different_Location_To_Player);

#pragma region Find_Door_To_Player

	CFind_Door_To_Target_Zombie* pTask_Find_Door_To_Target = { CFind_Door_To_Target_Zombie::Create() };
	pTask_Find_Door_To_Target->SetBlackBoard(m_pBlackBoard);
	pSequecne_Different_Region_Player->Insert_Child_Node(pTask_Find_Door_To_Target);	

#pragma endregion		//	Sequence Different Location Player Child

#pragma region Selector Move To Door || InteractDoor 

	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SELECTOR;
	CComposite_Node* pSelector_Move_To_Door_Interact_Door = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSequecne_Different_Region_Player->Insert_Child_Node(pSelector_Move_To_Door_Interact_Door);

#pragma region Move_To_Target_Door		Deco Non Colllision Door 

	CMove_To_Target_Zombie* pTask_Move_To_Door = { CMove_To_Target_Zombie::Create() };
	pTask_Move_To_Door->SetBlackBoard(m_pBlackBoard);
	pSelector_Move_To_Door_Interact_Door->Insert_Child_Node(pTask_Move_To_Door);

	//	문과 충돌하지않앗는지
	CIs_Collision_Prop_Zombie* pDeco_Is_Collision_Door_Inverse_Trigger = { CIs_Collision_Prop_Zombie::Create(CIs_Collision_Prop_Zombie::COLL_PROP_TYPE::_DOOR, CIs_Collision_Prop_Zombie::RETURN_TYPE::_REVERSE) };
	pDeco_Is_Collision_Door_Inverse_Trigger->SetBlackBoard(m_pBlackBoard);
	pTask_Move_To_Door->Insert_Decorator_Node(pDeco_Is_Collision_Door_Inverse_Trigger);

#pragma endregion		//	Selector Move To Door || InteractDoor  Child

#pragma region Selector Interact Door 

	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SELECTOR;
	CComposite_Node* pSelectorNode_Interact_Door = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSelector_Move_To_Door_Interact_Door->Insert_Child_Node(pSelectorNode_Interact_Door);

	CIs_Collision_Prop_Zombie* pDeco_Is_Collision_Door_Trigger = { CIs_Collision_Prop_Zombie::Create(CIs_Collision_Prop_Zombie::COLL_PROP_TYPE::_DOOR, CIs_Collision_Prop_Zombie::RETURN_TYPE::_STARARIGHT) };
	pDeco_Is_Collision_Door_Trigger->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Interact_Door->Insert_Decorator_Node(pDeco_Is_Collision_Door_Trigger);

#pragma region Door Interact OPEN || KNOCK || RUB

	//	필요 조건 => 문 닫힘 ( 체력 1 => 1대치면 열림), 문잠기지않음		
	COpen_Door_Zombie* pTask_Open_Door = { COpen_Door_Zombie::Create() };
	pTask_Open_Door->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Interact_Door->Insert_Child_Node(pTask_Open_Door);

	//	필요 조건 => 문 안열리고 막타이상이어야함 ( HP1 초과... ), 문이 잠기지않음
	CKnock_Door_Zombie* pTask_Knock_Door = { CKnock_Door_Zombie::Create() };
	pTask_Knock_Door->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Interact_Door->Insert_Child_Node(pTask_Knock_Door);

	//	문이 잠겨있어야함
	//	pSelectorNode_Interact_Door->Insert_Child_Node(pTask_Rub_Door);

#pragma endregion		//	Selector Interact Door Childs

#pragma endregion		//	Selector Move To Door || InteractDoor  Child

#pragma endregion		//	Sequence Different Location Player Child

#pragma endregion		//	Selector In Door Check Child

#pragma region Selector Same Location To Player

	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SELECTOR;
	CComposite_Node* pSelectorNode_Same_Region_Player = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSelectorNode_InDoorCheck->Insert_Child_Node(pSelectorNode_Same_Region_Player);

	CIs_Same_Location_To_Player_Zombie* pDeco_Is_Same_Location_To_Player = { CIs_Same_Location_To_Player_Zombie::Create() };
	pDeco_Is_Same_Location_To_Player->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Same_Region_Player->Insert_Decorator_Node(pDeco_Is_Same_Location_To_Player);

#pragma region BITE

	/*
	*Root Child Section ( Bite )
	*/

	//	Add Task Node		=> Bite 
	CBite_Zombie* pTask_Bite_Zombie = { CBite_Zombie::Create() };
	pTask_Bite_Zombie->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Same_Region_Player->Insert_Child_Node(pTask_Bite_Zombie);

#pragma endregion		//	Selector Same Region Child

#pragma region		STAND_UP || TRUN_OVER

	//	Add RootNode Child Composite Node - Selector Node			(Is Hit?)
	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SELECTOR;
	CComposite_Node* pSelectorNode_RootChild_StandUp_TurnOver = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSelectorNode_Same_Region_Player->Insert_Child_Node(pSelectorNode_RootChild_StandUp_TurnOver);

	//	Add Task Node		=> Stand Up
	CStand_Up_Zombie* pTask_StandUp = { CStand_Up_Zombie::Create() };
	pTask_StandUp->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_StandUp_TurnOver->Insert_Child_Node(pTask_StandUp);

	//	Add Task Node		=> Turn Over
	/*CTurn_Over_Zombie*			pTask_TurnOver = { CTurn_Over_Zombie::Create() };
	pTask_TurnOver->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_StandUp_TurnOver->Insert_Child_Node(pTask_TurnOver);*/

#pragma endregion		//	Selector Same Region Child

#pragma region		SELECTOR HIT
	/*
	*Root Child Section ( Hit )
	*/

	//	Add RootNode Child Composite Node - Selector Node			(Is Hit?)
	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SELECTOR;
	CComposite_Node* pSelectorNode_RootChild_Hit = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSelectorNode_Same_Region_Player->Insert_Child_Node(pSelectorNode_RootChild_Hit);

	//	Add Task Node		=> Damage Stun
	CStun_Zombie* pTask_Stun = { CStun_Zombie::Create() };
	pTask_Stun->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_Hit->Insert_Child_Node(pTask_Stun);

	//	Add Task Node		=> Damage Knock Back
	CKnock_Back_Zombie* pTask_Knockback = { CKnock_Back_Zombie::Create() };
	pTask_Knockback->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_Hit->Insert_Child_Node(pTask_Knockback);

#pragma endregion		//	Selector Same Region Child

#pragma region		LIGHTLY HOLD
	/*
	*Root Child Section ( Lightly Hold )
	*/

	//	거리내로 들어오면 시간을 누적
	//	Add Task Node		=> Lightly Hold 
	CLightly_Hold_Zombie* pTask_Lightly_Hold = { CLightly_Hold_Zombie::Create() };
	pTask_Lightly_Hold->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Same_Region_Player->Insert_Child_Node(pTask_Lightly_Hold);

#pragma endregion		//	Selector Same Region Child

#pragma region		Selector Move 

	/*
	*Root Child Section ( Select Move Or Turn )
	*/

	//	Add RootNode Child Composite Node - Selector Node			(Is Move Or Turn ?)
	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SELECTOR;
	CComposite_Node* pSelectorNode_RootChild_Move = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSelectorNode_Same_Region_Player->Insert_Child_Node(pSelectorNode_RootChild_Move);

	//	Add Decorator Node
	CIs_Character_In_Range_Zombie* pDeco_Charactor_In_Range_Recognition = { CIs_Character_In_Range_Zombie::Create(m_pStatus->fRecognitionRange) };
	pDeco_Charactor_In_Range_Recognition->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_Move->Insert_Decorator_Node(pDeco_Charactor_In_Range_Recognition);

#pragma region		MOVE || TURN |HOLD

	//	Add Task Node (Hold)
	CHold_Zombie* pTask_Hold = { CHold_Zombie::Create() };
	pTask_Hold->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Same_Region_Player->Insert_Child_Node(pTask_Hold);

	//	Add Task Node (Move)
	CMove_To_Target_Zombie* pTask_Move = { CMove_To_Target_Zombie::Create() };
	pTask_Move->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Same_Region_Player->Insert_Child_Node(pTask_Move);

	//	Add Task Node (Pivot Turn)
	CPivot_Turn_Zombie* pTask_Pivot_Turn = { CPivot_Turn_Zombie::Create() };
	pTask_Pivot_Turn->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Same_Region_Player->Insert_Child_Node(pTask_Pivot_Turn);

#pragma endregion		//	Selector Move Childs

#pragma endregion		//	Selector Same Region Child

#pragma endregion		//	Selector In Door Check Child

#pragma endregion		//	Selector Default Child

#pragma region IDLE

	/*
	*Root Child Section		( Idle )
	*/

	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SELECTOR;
	CComposite_Node* pSelectorNode_RootChild_Idle = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSelectorNode_Root->Insert_Child_Node(pSelectorNode_RootChild_Idle);

	//	Need Creep Pose
	//	Add Task Node		( Creep )
	CCreep_Zombie* pTask_Creep = { CCreep_Zombie::Create() };
	pTask_Creep->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_Idle->Insert_Child_Node(pTask_Creep);

	//	Need Up Pose`
	//	Add Task Node		( Idle )
	CWait_Zombie* pTask_Wait = { CWait_Zombie::Create() };
	pTask_Wait->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_Idle->Insert_Child_Node(pTask_Wait);

	//	Else...
	//	Add Task Node		( Sleep )
	CSleep_Zombie* pTask_Sleep = { CSleep_Zombie::Create() };
	pTask_Sleep->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_Idle->Insert_Child_Node(pTask_Sleep);

#pragma endregion		//	Selector Default Child

#pragma endregion		//	Selector Root Child

#pragma endregion		//	Sequence Root Child

#pragma region Additional Blend Anim Sequence

	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SEQUENCE;
	CComposite_Node* pSequenceNode_Additional_Blend_Anim = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSequenceNode_Root->Insert_Child_Node(pSequenceNode_Additional_Blend_Anim);

#pragma region		Task Shake Skin

	CShake_Skin_Zombie* pTask_Shake_Skin = { CShake_Skin_Zombie::Create() };
	pTask_Shake_Skin->SetBlackBoard(m_pBlackBoard);
	pSequenceNode_Additional_Blend_Anim->Insert_Child_Node(pTask_Shake_Skin);

#pragma endregion		//	Child Sequence Additional Blend Anim

#pragma region		Task Turn Spine

	CTurn_Spine_Head_Zombie* pTask_Turn_Spine_Head = { CTurn_Spine_Head_Zombie::Create() };
	pTask_Turn_Spine_Head->SetBlackBoard(m_pBlackBoard);
	pSequenceNode_Additional_Blend_Anim->Insert_Child_Node(pTask_Turn_Spine_Head);

#pragma endregion		//	Child Sequence Additional Blend Anim

#pragma region		Task Raise Up Hand

	CRaise_Up_Hand_Zombie* pTask_Raise_Up_Hand = { CRaise_Up_Hand_Zombie::Create() };
	pTask_Raise_Up_Hand->SetBlackBoard(m_pBlackBoard);
	pSequenceNode_Additional_Blend_Anim->Insert_Child_Node(pTask_Raise_Up_Hand);

#pragma endregion		//	Sequence Root Child 

#pragma endregion	//	Sequence Root Child

#pragma endregion		//	Sequence Root

#pragma endregion		//	RootNode Childe Section
	return;
}

HRESULT CZombie::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Com_Collider_Head */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = 0.32f;
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.fRadius + 0.6f, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Head"), (CComponent**)&m_pColliderCom[COLLIDER_HEAD], &ColliderDesc)))
		return E_FAIL;

	/* Com_Collider_Body */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderOBBDesc{};

	ColliderOBBDesc.vRotation = _float4(0.f, XMConvertToRadians(45.0f), 0.f, 0.f);
	ColliderOBBDesc.vSize = _float3(0.8f, 0.6f, 0.8f);
	ColliderOBBDesc.vCenter = _float3(0.f, ColliderOBBDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_Body"), (CComponent**)&m_pColliderCom[COLLIDER_BODY], &ColliderOBBDesc)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC			NavigationDesc{};

	NavigationDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NavigationDesc)))
		return E_FAIL;

	/* Com_Collider_Body */
	CBounding_AABB::BOUNDING_AABB_DESC		ColliderAABBDesc{};

	ColliderAABBDesc.vSize = _float3(1.2f, 2.3f, 1.2f);
	ColliderAABBDesc.vCenter = _float3(0.f, ColliderAABBDesc.vSize.y * 0.5f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_Bounding"), (CComponent**)&m_pColliderCom_Bounding, &ColliderAABBDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CZombie::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CZombie::Add_PartObjects()
{
	m_PartObjects.resize(CZombie::PART_ID::PART_END);

	/* For.Part_Body */
	CPartObject* pBodyObject = { nullptr };
	CBody_Zombie::BODY_MONSTER_DESC			BodyDesc;

	BodyDesc.pRender = &m_isRender;
	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pRootTranslation = &m_vRootTranslation;
	BodyDesc.eBodyType = static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID);
	BodyDesc.ppPart_Breaker = &m_pPart_Breaker;

	pBodyObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Body_Zombie"), &BodyDesc));
	if (nullptr == pBodyObject)
		return E_FAIL;

	m_PartObjects[CZombie::PART_ID::PART_BODY] = pBodyObject;


	/* For.Part_Face */
	CPartObject* pFaceObject = { nullptr };
	CFace_Zombie::FACE_MONSTER_DESC			FaceDesc;

	FaceDesc.pRender = &m_isRender;
	FaceDesc.pParentsTransform = m_pTransformCom;
	FaceDesc.eBodyType = static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID);
	FaceDesc.iFaceModelID = m_iFace_ID;

	pFaceObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Face_Zombie"), &FaceDesc));
	if (nullptr == pFaceObject)
		return E_FAIL;

	m_PartObjects[CZombie::PART_ID::PART_FACE] = pFaceObject;


	/* For.Part_Hat */
	/*CPartObject* pHatObject = { nullptr };
	CClothes_Zombie::CLOTHES_MONSTER_DESC		ClothesHatDesc;

	ClothesHatDesc.pParentsTransform = m_pTransformCom;
	ClothesHatDesc.eType = CClothes_Zombie::CLOTHES_TYPE::TYPE_HAT;

	pHatObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Clothes_Zombie"), &ClothesHatDesc));
	if (nullptr == pHatObject)
		return E_FAIL;

	m_PartObjects[CZombie::PART_ID::PART_HAT] = pHatObject;*/

	/* For.Part_Shirts */
	CPartObject* pShirtsObject = { nullptr };
	CClothes_Zombie::CLOTHES_MONSTER_DESC		ClothesShirtsDesc;

	ClothesShirtsDesc.pRender = &m_isRender;
	ClothesShirtsDesc.pParentsTransform = m_pTransformCom;
	ClothesShirtsDesc.eBodyType = static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID);
	ClothesShirtsDesc.eClothesType = ZOMBIE_CLOTHES_TYPE::_SHIRTS;
	ClothesShirtsDesc.iClothesModelID = m_iShirts_ID;

	pShirtsObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Clothes_Zombie"), &ClothesShirtsDesc));
	if (nullptr == pShirtsObject)
		return E_FAIL;

	m_PartObjects[CZombie::PART_ID::PART_SHIRTS] = pShirtsObject;

	/* For.Part_Shirts3 */
	/*CPartObject* pShirtsObject3 = { nullptr };
	CClothes_Zombie::CLOTHES_MONSTER_DESC		ClothesShirts3Desc;

	ClothesShirts3Desc.pParentsTransform = m_pTransformCom;
	ClothesShirtsDesc.eType = CClothes_Zombie::CLOTHES_TYPE::TYPE_SHIRTS3;

	pShirtsObject3 = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Clothes_Zombie"), &ClothesShirts3Desc));
	if (nullptr == pShirtsObject3)
		return E_FAIL;

	m_PartObjects[CZombie::PART_ID::PART_SHIRTS3] = pShirtsObject3;*/


	/* For.Part_Pants */
	CPartObject* pPantsObject = { nullptr };
	CClothes_Zombie::CLOTHES_MONSTER_DESC		ClothesPantsDesc;

	ClothesPantsDesc.pRender = &m_isRender;
	ClothesPantsDesc.pParentsTransform = m_pTransformCom;
	ClothesPantsDesc.eBodyType = static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID);
	ClothesPantsDesc.eClothesType = ZOMBIE_CLOTHES_TYPE::_PANTS;
	ClothesPantsDesc.iClothesModelID = m_iPants_ID;

	pPantsObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Clothes_Zombie"), &ClothesPantsDesc));
	if (nullptr == pPantsObject)
		return E_FAIL;

	m_PartObjects[CZombie::PART_ID::PART_PANTS] = pPantsObject;

	return S_OK;
}

HRESULT CZombie::Initialize_Status()
{
	m_pStatus = new MONSTER_STATUS();

	m_pStatus->fAttack = STATUS_ZOMBIE_ATTACK;
	m_pStatus->fViewAngle = STATUS_ZOMBIE_VIEW_ANGLE;
	m_pStatus->fMaxMoveAngle = STATUS_ZOMBIE_MAX_MOVE_ANGLE;

	m_pStatus->fRecognitionRange = STATUS_ZOMBIE_DEFAULT_RECOGNITION_DISTANCE;
	m_pStatus->fRecognitionRange_LookTarget = STATUS_ZOMBIE_MAX_RECOGNITION_DISTANCE;
	m_pStatus->fMaxRecognitionTime = STATUS_ZOMBIE_MAX_RECOGNITION_TIME;
	m_pStatus->fLookTrargetNeedTime = STATUS_ZOMBIE_NEED_RECOGNITION_TIME;
	m_pStatus->fBiteRange = STATUS_ZOMBIE_BITE_RANGE;

	m_pStatus->fTryHoldTime = STATUS_ZOMBIE_TRY_HOLD_TIME;
	m_pStatus->fTryHoldRange = STATUS_ZOMBIE_TRY_HOLD_RANGE;

	m_pStatus->fLightlyHoldRange = STATUS_ZOMBIE_LIGHTLY_HOLD_RANGE;
	m_pStatus->fTryLightlyHoldTime = STATUS_ZOMBIE_TRY_LIGHTLY_HOLD_TIME;

	m_pStatus->fTryStandUpTime = STATUS_ZOMBIE_TRY_STANDUP_TIME;

	m_pStatus->fHealth = STATUS_ZOMBIE_HEALTH;

	m_pStatus->fStamina = STATUS_ZOMBIE_STAMINA;
	m_pStatus->fMaxStamina = STATUS_ZOMBIE_STAMINA_MAX;
	m_pStatus->fStaminaChargingPerSec = STATUS_ZOMBIE_STAMINA_CHARGING_PER_SEC;

	return S_OK;
}

HRESULT CZombie::Initialize_States()
{
	m_eFaceState = FACE_STATE::_END;
	m_ePoseState = POSE_STATE::_UP;
	m_eState = MONSTER_STATE::MST_IDLE;

	return S_OK;
}

HRESULT CZombie::Initialize_PartBreaker()
{
	CPart_Breaker_Zombie::PART_BREAKER_DESC			PartBreakerDesc;
	PartBreakerDesc.pBodyModel = static_cast<CModel*>(m_PartObjects[CMonster::PART_BODY]->Get_Component(TEXT("Com_Model")));
	PartBreakerDesc.iBodyType = m_iBody_ID;

	CPart_Breaker_Zombie*			pPart_Breaker = { CPart_Breaker_Zombie::Create(&PartBreakerDesc) };
	m_pPart_Breaker = pPart_Breaker;
	

	if (nullptr == m_pPart_Breaker)
		return E_FAIL;

	return S_OK;
}

void CZombie::Play_Animations_Body(_float fTimeDelta)
{
	static_cast<CBody_Zombie*>(m_PartObjects[CMonster::PART_BODY])->Play_Animations(fTimeDelta);
}

void CZombie::Active_IK_Body(_bool isActive)
{
	static_cast<CBody_Zombie*>(m_PartObjects[PART_BODY])->Active_IK(isActive);
}

_bool CZombie::Is_In_Location(LOCATION_MAP_VISIT eLocation)
{
	return eLocation == m_eLocation;
}

_bool CZombie::Is_In_Linked_Location(LOCATION_MAP_VISIT eLocation)
{
	return CRoom_Finder::Get_Instance()->Is_Linked_Location_From_Location(m_eLocation, eLocation);
}

HRESULT CZombie::Initialize_PartModels()
{
	CModel* pBodyModel = { dynamic_cast<CModel*>(m_PartObjects[PART_BODY]->Get_Component(TEXT("Com_Model"))) };
	CModel* pFaceModel = { dynamic_cast<CModel*>(m_PartObjects[PART_FACE]->Get_Component(TEXT("Com_Model"))) };
	CModel* pShirtsModel = { dynamic_cast<CModel*>(m_PartObjects[PART_SHIRTS]->Get_Component(TEXT("Com_Model"))) };
	CModel* pPantsModel = { dynamic_cast<CModel*>(m_PartObjects[PART_PANTS]->Get_Component(TEXT("Com_Model"))) };
	//	CModel* pHatModel = { dynamic_cast<CModel*>(m_PartObjects[PART_HAT]->Get_Component(TEXT("Com_Model"))) };

	m_pBodyModel = pBodyModel;
	m_pHeadModel = pFaceModel;
	m_pShirtsModel = pShirtsModel;
	m_pPantsModel = pPantsModel;
	//	m_pHatModel = pHatModel;

	if (nullptr == pBodyModel ||
		nullptr == pFaceModel ||
		nullptr == pShirtsModel ||
		nullptr == pPantsModel /*||
		nullptr == pHatModel*/
		)
		return E_FAIL;

	if (FAILED(pFaceModel->Link_Bone_Auto(pBodyModel)))
		return E_FAIL;
	if (FAILED(pShirtsModel->Link_Bone_Auto(pBodyModel)))
		return E_FAIL;
	if (FAILED(pPantsModel->Link_Bone_Auto(pBodyModel)))
		return E_FAIL;
	/*if (FAILED(pHatModel->Link_Bone_Auto(pBodyModel)))
		return E_FAIL;*/

	return S_OK;
}

_bool CZombie::Is_Enough_Stamina(USE_STAMINA eAction)
{
	_bool		isEnough = { false };
	if (USE_STAMINA::_BITE == eAction)	
	{
		isEnough = m_pStatus->fStamina > ZOMBIE_NEED_STAMINA_BITE;
	}

	else if (USE_STAMINA::_STAND_UP == eAction)
	{
		isEnough = m_pStatus->fStamina > ZOMBIE_NEED_STAMINA_STANDUP;
	}

	else if (USE_STAMINA::_TURN_OVER == eAction)
	{
		isEnough = m_pStatus->fStamina > ZOMBIE_NEED_STAMINA_TURN_OVER;
	}

	return isEnough;
}

_bool CZombie::Use_Stamina(USE_STAMINA eAction)
{
	if (false == Is_Enough_Stamina(eAction))
		return false;

	if (USE_STAMINA::_BITE == eAction)
	{
		m_pStatus->fStamina -= ZOMBIE_NEED_STAMINA_BITE;
	}

	else if (USE_STAMINA::_STAND_UP == eAction)
	{
		m_pStatus->fStamina -= ZOMBIE_NEED_STAMINA_STANDUP;
	}

	else if (USE_STAMINA::_TURN_OVER == eAction)
	{
		m_pStatus->fStamina -= ZOMBIE_NEED_STAMINA_TURN_OVER;
	}

	return true;
}

void CZombie::Col_EventCol()
{
	list<CGameObject*>* pCollider = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Jombie_Collider"));
	if (pCollider == nullptr)
		return;
	for (auto& iter : *pCollider)
	{
		if (m_pColliderCom_Bounding->Intersect(static_cast<CCollider*>(iter->Get_Component(TEXT("Com_Collider")))))
		{
			CCustomCollider* pColCom = static_cast<CCustomCollider*>(iter);
			m_bEvent = true;
			m_eBeHavior_Col = (JOMBIE_BEHAVIOR_COLLIDER_TYPE)pColCom->Get_Region(); // 행동enum을 가지고 있음 JOMBIE_BEHAVIOR_COLLIDER_TYPE
			m_iPropNum = pColCom->Get_InteractProps();
			wstring strLayer = { L"" };
			switch (m_eBeHavior_Col)
			{
			case JOMBIE_BEHAVIOR_COLLIDER_WINDOW:
				strLayer = TEXT("Layer_Window");
				break;
			case JOMBIE_BEHAVIOR_COLLIDER_DOOR:
				strLayer = TEXT("Layer_Door");
				break;
			}
			m_InteractObjVec[m_eBeHavior_Col] = m_pGameInstance->Get_GameObject(g_Level, strLayer, m_iPropNum);
			//m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_InteractObjVec[m_eBeHavior_Col]->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION)) ;
			break; // 이벤트임을 알림
		}
		else
			m_bEvent = false;
	}
}

void CZombie::Update_Region_Datas()
{
	list<CGameObject*>* pColliderObjects = { m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Collider")) };
	if (nullptr == pColliderObjects)
		return;

	for (auto& pColliderObject : *pColliderObjects)
	{
		if (nullptr == pColliderObject)
			continue;

		if (m_pColliderCom_Bounding->Intersect(static_cast<CCollider*>(pColliderObject->Get_Component(TEXT("Com_Collider")))))
		{
			CCustomCollider* pCustomCollider = { static_cast<CCustomCollider*>(pColliderObject) };

			m_iCurrent_Region = pCustomCollider->Get_Region();
			m_eCurrent_Floor = static_cast<MAP_FLOOR_TYPE>(pCustomCollider->Get_Floor());

			break;
		}
	}
}

void CZombie::SetRagdoll_StartPose()
{
	m_pController->SetReleased(true);
	m_pController->SetDead(true);
	m_pController->Set_Force(_float4(0.f, 0.f, 0.f, 0.f), COLLIDER_TYPE::CHEST);

	m_bRagdoll = true;

	for (auto& pPartObject : m_PartObjects)
	{
		if (nullptr != pPartObject)
			pPartObject->SetRagdoll(m_iIndex_CCT, _float4(0.f,0.f,0.f,0.f), COLLIDER_TYPE::CHEST);
	}
}

void CZombie::Perform_Skinning()
{
	if (m_bRagdoll == false)
	{
		m_pBodyModel->Bind_Essential_Resource_Skinning(m_pTransformCom->Get_WorldFloat4x4_Ptr());

		//Body Model
		{
			if (nullptr != m_pBodyModel)
			{
				list<_uint> NonHideIndex = m_pBodyModel->Get_NonHideMeshIndices();

				for (auto& i : NonHideIndex)
				{
					//if (m_pBodyModel->Get_Mesh_Branch(i) != (_int)CBody_Zombie::BODY_MESH_TYPE::_INNER)
					{
						m_pBodyModel->Bind_Resource_Skinning(i);
						m_pGameInstance->Perform_Skinning((*m_pBodyModel->GetMeshes())[i]->GetNumVertices());
					}
				}
			}
		}

		//Face Model
		{
			if (nullptr != m_pHeadModel)
			{
				m_pHeadModel->Bind_Essential_Resource_Skinning(m_pTransformCom->Get_WorldFloat4x4_Ptr());

				list<_uint> NonHideIndex = m_pHeadModel->Get_NonHideMeshIndices();

				for (auto& i : NonHideIndex)
				{
					if (m_pHeadModel->Get_Mesh_Branch(i) != (_int)CFace_Zombie::FACE_MESH_TYPE::_INNER)
					{
						m_pHeadModel->Bind_Resource_Skinning(i);
						m_pGameInstance->Perform_Skinning((*m_pHeadModel->GetMeshes())[i]->GetNumVertices());
						//m_pHeadModel->Staging_Skinning(i);
					}
				}
			}
		}

		//Shirts Model
		{
			if (nullptr != m_pShirtsModel)
			{
				m_pShirtsModel->Bind_Essential_Resource_Skinning(m_pTransformCom->Get_WorldFloat4x4_Ptr());

				list<_uint> NonHideIndex = m_pShirtsModel->Get_NonHideMeshIndices();

				for (auto& i : NonHideIndex)
				{
					m_pShirtsModel->Bind_Resource_Skinning(i);
					m_pGameInstance->Perform_Skinning((*m_pShirtsModel->GetMeshes())[i]->GetNumVertices());
					//m_pShirtsModel->Staging_Skinning(i);
				}
			}
		}

		//Pants Model
		{
			if (nullptr != m_pPantsModel)
			{
				m_pPantsModel->Bind_Essential_Resource_Skinning(m_pTransformCom->Get_WorldFloat4x4_Ptr());

				list<_uint> NonHideIndex = m_pPantsModel->Get_NonHideMeshIndices();

				for (auto& i : NonHideIndex)
				{
					m_pPantsModel->Bind_Resource_Skinning(i);
					m_pGameInstance->Perform_Skinning((*m_pPantsModel->GetMeshes())[i]->GetNumVertices());
					//m_pPantsModel->Staging_Skinning(i);
				}
			}
		}

		//Hat Model
		{
			if (nullptr != m_pHatModel)
			{
				m_pHatModel->Bind_Essential_Resource_Skinning(m_pTransformCom->Get_WorldFloat4x4_Ptr());

				list<_uint> NonHideIndex = m_pHatModel->Get_NonHideMeshIndices();

				for (auto& i : NonHideIndex)
				{
					m_pHatModel->Bind_Resource_Skinning(i);
					m_pGameInstance->Perform_Skinning((*m_pHatModel->GetMeshes())[i]->GetNumVertices());
					//m_pHatModel->Staging_Skinning(i);
				}
			}

		}
	}
	else
	{
		m_WorldMat_To_Decal = m_pTransformCom->Get_WorldFloat4x4();
		m_WorldMat_To_Decal._41 = 0.f;
		m_WorldMat_To_Decal._42 = 0.f;
		m_WorldMat_To_Decal._43 = 0.f;

		//Body Model
		{
			if (nullptr != m_pBodyModel)
			{
				m_pBodyModel->Bind_Essential_Resource_Skinning(&m_WorldMat_To_Decal);

				list<_uint> NonHideIndex = m_pBodyModel->Get_NonHideMeshIndices();

				for (auto& i : NonHideIndex)
				{
					if (m_pBodyModel->Get_Mesh_Branch(i) != (_int)CBody_Zombie::BODY_MESH_TYPE::_INNER)
					{
						m_pBodyModel->Bind_Resource_Skinning(i);
						m_pGameInstance->Perform_Skinning((*m_pBodyModel->GetMeshes())[i]->GetNumVertices());
					}
				}
			}
		}

		//Face Model
		{
			if (nullptr != m_pHeadModel)
			{
				list<_uint> NonHideIndex = m_pHeadModel->Get_NonHideMeshIndices();

				m_pHeadModel->Bind_Essential_Resource_Skinning(&m_WorldMat_To_Decal);

				for (auto& i : NonHideIndex)
				{
					if (m_pHeadModel->Get_Mesh_Branch(i) != (_int)CFace_Zombie::FACE_MESH_TYPE::_INNER)
					{
						m_pHeadModel->Bind_Resource_Skinning(i);
						m_pGameInstance->Perform_Skinning((*m_pHeadModel->GetMeshes())[i]->GetNumVertices());
						//m_pHeadModel->Staging_Skinning(i);
					}
				}
			}
		}

		//Shirts Model
		{
			if (nullptr != m_pShirtsModel)
			{
				m_pShirtsModel->Bind_Essential_Resource_Skinning(&m_WorldMat_To_Decal);

				list<_uint> NonHideIndex = m_pShirtsModel->Get_NonHideMeshIndices();

				for (auto& i : NonHideIndex)
				{
					m_pShirtsModel->Bind_Resource_Skinning(i);
					m_pGameInstance->Perform_Skinning((*m_pShirtsModel->GetMeshes())[i]->GetNumVertices());
					//m_pShirtsModel->Staging_Skinning(i);
				}
			}
		}

		//Pants Model
		{
			if (nullptr != m_pPantsModel)
			{
				m_pPantsModel->Bind_Essential_Resource_Skinning(&m_WorldMat_To_Decal);

				list<_uint> NonHideIndex = m_pPantsModel->Get_NonHideMeshIndices();

				for (auto& i : NonHideIndex)
				{
					m_pPantsModel->Bind_Resource_Skinning(i);
					m_pGameInstance->Perform_Skinning((*m_pPantsModel->GetMeshes())[i]->GetNumVertices());
					//m_pPantsModel->Staging_Skinning(i);
				}
			}
		}

		//Hat Model
		{
			if (nullptr != m_pHatModel)
			{
				m_pHatModel->Bind_Essential_Resource_Skinning(&m_WorldMat_To_Decal);

				list<_uint> NonHideIndex = m_pHatModel->Get_NonHideMeshIndices();

				for (auto& i : NonHideIndex)
				{
					m_pHatModel->Bind_Resource_Skinning(i);
					m_pGameInstance->Perform_Skinning((*m_pHatModel->GetMeshes())[i]->GetNumVertices());
					//m_pHatModel->Staging_Skinning(i);
				}
			}
		}
	}
}

void CZombie::Ready_Decal()
{
	auto vRayOrigin = m_pGameInstance->Get_RayOrigin_Aim();
	auto vRayDir = m_pGameInstance->Get_RayDir_Aim();
	_float	fMinDist = 0.f;
	_float	fMaxDist = 0.f;
	//	if (m_pColliderCom_Bounding->IntersectRayAABB(XMLoadFloat4(&vRayOrigin), XMLoadFloat4(&vRayDir), fMinDist, fMaxDist))
	//	{
	//		Perform_Skinning();
	//
	//		HitResult hitResult;
	//		hitResult.hitModel = this;
	//		hitResult.minHitDistance = fMinDist;
	//		hitResult.maxHitDistance = fMaxDist;
	//
	//		_matrix rotMatrix = XMMatrixIdentity();
	//		//rotMatrix.SetRotation(mainCamera->GetDirection(), decalAngle);
	//
	//		AddDecalInfo decalInfo;
	//		decalInfo.rayOrigin = vRayOrigin;
	//		decalInfo.rayDir = vRayDir;
	//		XMStoreFloat4(&decalInfo.decalTangent, XMVector4Transform(m_pGameInstance->Get_Camera_Transform()->Get_State_Float4(CTransform::STATE_RIGHT), rotMatrix));
	//		decalInfo.decalSize = _float3(5.f, 5.f, 5.0f);
	//		decalInfo.minHitDistance = hitResult.minHitDistance;
	//		decalInfo.maxHitDistance = hitResult.maxHitDistance;
	//		decalInfo.decalMaterialIndex = 0;
	//
	//#pragma region RayCasting
	//		////Body Model
	//		//list<_uint> NonHideIndex = m_pBodyModel->Get_NonHideMeshIndices();
	//		//for (auto& i : NonHideIndex)
	//		//{
	//		//	m_iMeshIndex_Hit = m_pBodyModel->Perform_RayCasting(i, decalInfo, &m_fHitDistance);
	//
	//		//	if (m_iMeshIndex_Hit != 999)
	//		//	{
	//		//		m_iMeshIndex_Hit = i;
	//
	//		//		_vector CameraLook = XMVectorScale(m_pGameInstance->Get_Camera_Transform()->Get_State_Vector(CTransform::STATE_LOOK), m_fHitDistance);
	//		//		_vector CameraPos = m_pGameInstance->Get_Camera_Pos_Vector() + CameraLook;
	//		//		XMStoreFloat4(&m_vHitPosition, CameraPos);
	//
	//		//		m_vHitNormal = m_pController->GetHitNormal();
	//
	//		//		break;
	//		//	}
	//		//}
	//
	//		//if(m_iMeshIndex_Hit == 999)
	//		//{
	//		//	//Head Model
	//		//	NonHideIndex = m_pHeadModel->Get_NonHideMeshIndices();
	//		//	for (auto& i : NonHideIndex)
	//		//	{
	//		//		m_iMeshIndex_Hit = m_pHeadModel->Perform_RayCasting(i, decalInfo, &m_fHitDistance);
	//
	//		//		if (m_iMeshIndex_Hit != 999)
	//		//		{
	//		//			m_iMeshIndex_Hit = i;
	//
	//		//			_vector CameraLook = XMVectorScale(m_pGameInstance->Get_Camera_Transform()->Get_State_Vector(CTransform::STATE_LOOK), m_fHitDistance);
	//		//			_vector CameraPos = m_pGameInstance->Get_Camera_Pos_Vector() + CameraLook;
	//		//			XMStoreFloat4(&m_vHitPosition, CameraPos);
	//
	//		//			m_vHitNormal = m_pController->GetHitNormal();
	//
	//		//			break;
	//		//		}
	//		//	}
	//		//}
	//
	//		////Shirt Model
	//		//if (m_iMeshIndex_Hit == 999)
	//		//{
	//		//	NonHideIndex = m_pShirtsModel->Get_NonHideMeshIndices();
	//		//	for (auto& i : NonHideIndex)
	//		//	{
	//		//		m_iMeshIndex_Hit = m_pShirtsModel->Perform_RayCasting(i, decalInfo, &m_fHitDistance);
	//
	//		//		if (m_iMeshIndex_Hit != 999)
	//		//		{
	//		//			m_iMeshIndex_Hit = i;
	//
	//		//			_vector CameraLook = XMVectorScale(m_pGameInstance->Get_Camera_Transform()->Get_State_Vector(CTransform::STATE_LOOK), m_fHitDistance);
	//		//			_vector CameraPos = m_pGameInstance->Get_Camera_Pos_Vector() + CameraLook;
	//		//			XMStoreFloat4(&m_vHitPosition, CameraPos);
	//
	//		//			m_vHitNormal = m_pController->GetHitNormal();
	//
	//		//			break;
	//		//		}
	//		//	}
	//		//}
	//
	//		////Pants Model
	//		//if (m_iMeshIndex_Hit == 999)
	//		//{
	//		//	NonHideIndex = m_pPantsModel->Get_NonHideMeshIndices();
	//		//	for (auto& i : NonHideIndex)
	//		//	{
	//		//		m_iMeshIndex_Hit = m_pPantsModel->Perform_RayCasting(i, decalInfo, &m_fHitDistance);
	//
	//		//		if (m_iMeshIndex_Hit != 999)
	//		//		{
	//		//			m_iMeshIndex_Hit = i;
	//
	//		//			_vector CameraLook = XMVectorScale(m_pGameInstance->Get_Camera_Transform()->Get_State_Vector(CTransform::STATE_LOOK), m_fHitDistance);
	//		//			_vector CameraPos = m_pGameInstance->Get_Camera_Pos_Vector() + CameraLook;
	//		//			XMStoreFloat4(&m_vHitPosition, CameraPos);
	//
	//		//			m_vHitNormal = m_pController->GetHitNormal();
	//
	//		//			break;
	//		//		}
	//		//	}
	//		//}
	//#pragma endregion
	//
	//		if (m_iMeshIndex_Hit == 999)
	//		{
	//			m_vHitPosition = m_pController->GetBlockPoint();
	//			m_vHitNormal = m_pController->GetHitNormal();
	//		}
	//	}

	Perform_Skinning();

	m_vHitPosition = m_pController->GetBlockPoint();
	m_vHitNormal = m_pController->GetHitNormal();

	if (m_bRagdoll)
	{
		m_vHitPosition = m_pController->GetBlockPoint();
		m_vHitNormal = m_pController->GetHitNormal();
	}
}

void CZombie::RayCast_Decal()
{
	if (m_pGameInstance->RayCast_Decal(m_vHitPosition, m_pGameInstance->Get_Camera_Transform()->Get_State_Float4(CTransform::STATE_LOOK), &m_vDecalPoint, &m_vDecalNormal, 2.f))
	{
		//auto iNumDecal = m_pDecal_Layer->size();
		//for(auto& it : *m_pDecal_Layer)
		//{
		//	if(it->GetbRender() == false)
		//	{
		//		++m_iDecal_Index;
		//		auto pDecal = static_cast<CDecal_SSD*>(it);
		//		pDecal->Set_Render(true);
		//		//pDecal->SetWorldMatrix_With_HitNormal(m_vDecalNormal);
		//		pDecal->SetPosition(m_vDecalPoint);
		//		pDecal->LookAt(m_vDecalNormal);
		//		break;
		//	}
		//}

		m_vecDecal_SSD[m_iDecal_Index]->Set_Render(true);
		m_vecDecal_SSD[m_iDecal_Index]->SetPosition(m_vDecalPoint);
		m_vecDecal_SSD[m_iDecal_Index]->LookAt(m_vDecalNormal);
		++m_iDecal_Index;

		if (m_iDecal_Index >= m_vecDecal_SSD.size())
		{
			m_iDecal_Index = 0;
		}
	}
}

void CZombie::Ready_Effect()
{
	for (size_t i = 0; i < BLOOD_COUNT; ++i)
	{
		auto pBlood = CBlood::Create(m_pDevice, m_pContext);
		pBlood->SetSize(NORMAL_ATTACK_BLOOD_SIZE, NORMAL_ATTACK_BLOOD_SIZE, NORMAL_ATTACK_BLOOD_SIZE);
		m_vecBlood.push_back(pBlood);
		pBlood->Start();
	}

	for (size_t i = 0; i < SHOTGUN_BLOOD_COUNT; ++i)
	{
		auto pBlood = CBlood::Create(m_pDevice, m_pContext);
		pBlood->SetSize(BIG_ATTACK_BLOOD_SIZE, BIG_ATTACK_BLOOD_SIZE, BIG_ATTACK_BLOOD_SIZE);
		m_vecBlood_STG.push_back(pBlood);
		pBlood->Start();
	}

	for (size_t i = 0; i < BLOOD_DROP_COUNT; ++i)
	{
		auto pBlood_Drop = CBlood_Drop::Create(m_pDevice, m_pContext);
		pBlood_Drop->SetSize(NORMAL_ATTACK_BLOOD_SIZE_DROP, NORMAL_ATTACK_BLOOD_SIZE_DROP, NORMAL_ATTACK_BLOOD_SIZE_DROP);
		m_vecBlood_Drop.push_back(pBlood_Drop);
		pBlood_Drop->Start();
	}

	for (size_t i = 0; i < BLOOD_DROP_COUNT_STG; ++i)
	{
		auto pBlood_Drop = CBlood_Drop::Create(m_pDevice, m_pContext);
		pBlood_Drop->SetSize(BIG_ATTACK_BLOOD_SIZE_DROP, BIG_ATTACK_BLOOD_SIZE_DROP, BIG_ATTACK_BLOOD_SIZE_DROP);
		m_vecBlood_Drop_STG.push_back(pBlood_Drop);
		pBlood_Drop->Start();
	}

	for (size_t i = 0; i < SHOTGUN_BLOOD_COUNT; ++i)
	{
		auto pBlood_Drop = CBlood_Drop::Create(m_pDevice, m_pContext);
		pBlood_Drop->SetSize(BIG_ATTACK_BLOOD_SIZE_DROP, BIG_ATTACK_BLOOD_SIZE_DROP, BIG_ATTACK_BLOOD_SIZE_DROP);
		m_vecBlood_Drop_STG_NoRay.push_back(pBlood_Drop);
		pBlood_Drop->Start();
	}

	m_pImpact = CImpact::Create(m_pDevice, m_pContext);
	m_pImpact->SetSize(1.f, 1.f);

	m_vecHit.clear();
	for (size_t i = 0; i < SHOTGUN_BLOOD_COUNT; ++i)
	{
		auto pHit = CHit::Create(m_pDevice, m_pContext);
		pHit->SetSize(1.5f, 1.5f);
		m_vecHit.push_back(pHit);
	}
}

void CZombie::Release_Effect()
{
	for (size_t i = 0; i < m_vecBlood.size(); ++i)
	{
		Safe_Release(m_vecBlood[i]);
	}

	for (size_t i = 0; i < m_vecBlood_STG.size(); ++i)
	{
		Safe_Release(m_vecBlood_STG[i]);
	}

	for (size_t i = 0; i < m_vecDecal_SSD.size(); ++i)
	{
		Safe_Release(m_vecDecal_SSD[i]);
	}

	for (size_t i = 0; i < m_vecBlood_Drop.size(); ++i)
	{
		Safe_Release(m_vecBlood_Drop[i]);
	}

	for (size_t i = 0; i < m_vecBlood_Drop_STG.size(); ++i)
	{
		Safe_Release(m_vecBlood_Drop_STG[i]);
	}

	for (size_t i = 0; i < m_vecBlood_Drop_STG_NoRay.size(); ++i)
	{
		Safe_Release(m_vecBlood_Drop_STG_NoRay[i]);
	}

	Safe_Release(m_pImpact);

	for (size_t i = 0; i < m_vecHit.size(); ++i)
	{
		Safe_Release(m_vecHit[i]);
	}
}

void CZombie::Tick_Effect(_float fTimeDelta)
{
	for (size_t i = 0; i < m_vecBlood.size(); ++i)
	{
		m_vecBlood[i]->Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecBlood_STG.size(); ++i)
	{
		m_vecBlood_STG[i]->Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecDecal_SSD.size(); ++i)
	{
		m_vecDecal_SSD[i]->Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecBlood_Drop.size(); ++i)
	{
		m_vecBlood_Drop[i]->Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecBlood_Drop_STG_NoRay.size(); ++i)
	{
		m_vecBlood_Drop_STG_NoRay[i]->Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecBlood_Drop_STG.size(); ++i)
	{
		m_vecBlood_Drop_STG[i]->Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecHit.size(); ++i)
	{
		m_vecHit[i]->Tick(fTimeDelta);
	}

	m_pImpact->Tick(fTimeDelta);
}

void CZombie::Late_Tick_Effect(_float fTimeDelta)
{
	for (size_t i = 0; i < m_vecBlood.size(); ++i)
	{
		m_vecBlood[i]->Late_Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecBlood_STG.size(); ++i)
	{
		m_vecBlood_STG[i]->Late_Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecDecal_SSD.size(); ++i)
	{
		m_vecDecal_SSD[i]->Late_Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecBlood_Drop_STG.size(); ++i)
	{
		m_vecBlood_Drop_STG[i]->Late_Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecBlood_Drop_STG_NoRay.size(); ++i)
	{
		m_vecBlood_Drop_STG_NoRay[i]->Late_Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecBlood_Drop.size(); ++i)
	{
		m_vecBlood_Drop[i]->Late_Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecHit.size(); ++i)
	{
		m_vecHit[i]->Late_Tick(fTimeDelta);
	}

	m_pImpact->Late_Tick(fTimeDelta);
}

void CZombie::SetBlood()
{
	if (m_iBloodCount >= m_vecBlood.size())
	{
		m_bSetBlood = false;
		m_iBloodCount = 0;
		return;
	}

	if (m_BloodDelay + m_BloodTime < GetTickCount64())
	{
		m_BloodTime = GetTickCount64();
		m_vecBlood[m_iBloodCount]->Set_Render(true);
		m_vecBlood[m_iBloodCount]->SetWorldMatrix_With_HitNormal(m_vHitNormal);

		if (m_iBloodCount == 0)
		{
			if (m_bRagdoll == false)
			{
				m_vecBlood[m_iBloodCount]->SetHitPart(m_pController->GetHitPart());
			}
			else
			{
				auto pBody = static_cast<CBody_Zombie*>(m_PartObjects[CZombie::PART_BODY]);
				m_vecBlood[m_iBloodCount]->SetHitPart(pBody->Get_Ragdoll_RigidBody(m_pController->Get_Hit_Collider_Type()));
			}

			m_vecBlood[m_iBloodCount]->SetPosition(m_vHitPosition);
		}
		else
		{
			if (m_bRagdoll == false)
			{
				m_vecBlood[m_iBloodCount]->SetHitPart(m_pController->GetHitPart());
			}
			else
			{
				auto pBody = static_cast<CBody_Zombie*>(m_PartObjects[CZombie::PART_BODY]);
				m_vecBlood[m_iBloodCount]->SetHitPart(pBody->Get_Ragdoll_RigidBody(m_pController->Get_Hit_Collider_Type()));
			}

			m_vecBlood[m_iBloodCount]->SetPosition(m_vecBlood[m_iBloodCount - 1]->GetPosition());
		}

		if (m_iBloodCount == 0)
		{
			Calc_Decal_Map();
			RayCast_Decal();

			if (m_bBigAttack)
			{
				m_iBloodType = m_pGameInstance->GetRandom_Int(0, 10);

				m_vecBlood[m_iBloodCount]->SetType(m_iBloodType);
			}
			else
			{
				m_iBloodType = m_pGameInstance->GetRandom_Int(0, 10);

				m_vecBlood[m_iBloodCount]->SetType(m_iBloodType);
			}

			if (m_iBloodType >= 10)
			{
				m_iBloodType = 0;
			}

			if (m_bBigAttack)
			{
				m_vecBlood[m_iBloodCount]->SetSize(BIG_ATTACK_BLOOD_SIZE, BIG_ATTACK_BLOOD_SIZE, BIG_ATTACK_BLOOD_SIZE);

				m_vecBlood_Drop_STG[m_iBloodCount]->SetWorldMatrix_With_HitNormal(m_vHitNormal);
				m_vecBlood_Drop_STG[m_iBloodCount]->Set_Render(true);
				m_vecBlood_Drop_STG[m_iBloodCount]->SetPosition(m_vHitPosition);
				m_vecBlood_Drop_STG[m_iBloodCount]->SetType(m_pGameInstance->GetRandom_Int(1, 7));

			}
			else
			{
				m_vecBlood[m_iBloodCount]->SetSize(NORMAL_ATTACK_BLOOD_SIZE, NORMAL_ATTACK_BLOOD_SIZE, NORMAL_ATTACK_BLOOD_SIZE);

				m_vecBlood_Drop[m_iBloodCount]->SetWorldMatrix_With_HitNormal(m_vHitNormal);
				m_vecBlood_Drop[m_iBloodCount]->Set_Render(true);
				m_vecBlood_Drop[m_iBloodCount]->SetPosition(m_vHitPosition);
				m_vecBlood_Drop[m_iBloodCount]->SetType(m_pGameInstance->GetRandom_Int(0, 7));

			}
		}
		else
		{
			m_vecBlood[m_iBloodCount]->SetType(++m_iBloodType);

			if (m_iBloodType >= 9)
			{
				m_iBloodType = 0;
			}

			if (m_bBigAttack)
			{
				m_vecBlood[m_iBloodCount]->SetSize(BIG_ATTACK_BLOOD_SIZE, BIG_ATTACK_BLOOD_SIZE, BIG_ATTACK_BLOOD_SIZE);

				if (m_iBloodCount < BLOOD_DROP_COUNT_STG)
				{
					m_vecBlood_Drop_STG[m_iBloodCount]->SetWorldMatrix_With_HitNormal(m_vHitNormal);
					m_vecBlood_Drop_STG[m_iBloodCount]->Set_Render(true);
					m_vecBlood_Drop_STG[m_iBloodCount]->SetPosition(m_vecBlood[m_iBloodCount - 1]->GetPosition());
					m_vecBlood_Drop_STG[m_iBloodCount]->SetType(m_pGameInstance->GetRandom_Int(1, 7));
				}
			}
			else
			{
				m_vecBlood[m_iBloodCount]->SetSize(NORMAL_ATTACK_BLOOD_SIZE, NORMAL_ATTACK_BLOOD_SIZE, NORMAL_ATTACK_BLOOD_SIZE);

				if (m_iBloodCount < BLOOD_DROP_COUNT)
				{
					m_vecBlood_Drop[m_iBloodCount]->SetWorldMatrix_With_HitNormal(m_vHitNormal);
					m_vecBlood_Drop[m_iBloodCount]->Set_Render(true);
					m_vecBlood_Drop[m_iBloodCount]->SetPosition(m_vecBlood[m_iBloodCount - 1]->GetPosition());
					m_vecBlood_Drop[m_iBloodCount]->SetType(m_pGameInstance->GetRandom_Int(0, 7));
				}
			}
		}

		++m_iBloodCount;

		if (m_iBloodCount >= m_vecBlood.size())
		{
			m_bSetBlood = false;
			m_iBloodCount = 0;
			return;
		}
	}
}

void CZombie::Calc_Decal_Map()
{
	if (m_bBigAttack)
	{
		if (m_pBodyModel)
		{
			m_pBodyModel->SetDecalWorldMatrix(m_iMeshIndex_Hit, m_vecBlood[m_iBloodCount]->GetWorldMatrix(), (_int)CBody_Zombie::BODY_MESH_TYPE::_INNER, true);
			m_pBodyModel->Perform_Calc_DecalMap((_int)CBody_Zombie::BODY_MESH_TYPE::_INNER);
		}

		if (m_pHeadModel)
		{
			m_pHeadModel->SetDecalWorldMatrix(m_iMeshIndex_Hit, m_vecBlood[m_iBloodCount]->GetWorldMatrix(), (_int)CFace_Zombie::FACE_MESH_TYPE::_INNER, true);
			m_pHeadModel->Perform_Calc_DecalMap((_int)CFace_Zombie::FACE_MESH_TYPE::_INNER);
		}

		if (m_pShirtsModel)
		{
			m_pShirtsModel->SetDecalWorldMatrix(m_iMeshIndex_Hit, m_vecBlood[m_iBloodCount]->GetWorldMatrix(), (_int)CBody_Zombie::BODY_MESH_TYPE::_INNER, true);
			m_pShirtsModel->Perform_Calc_DecalMap((_int)CBody_Zombie::BODY_MESH_TYPE::_INNER);
		}

		if (m_pPantsModel)
		{
			m_pPantsModel->SetDecalWorldMatrix(m_iMeshIndex_Hit, m_vecBlood[m_iBloodCount]->GetWorldMatrix(), (_int)CBody_Zombie::BODY_MESH_TYPE::_INNER, true);
			m_pPantsModel->Perform_Calc_DecalMap((_int)CBody_Zombie::BODY_MESH_TYPE::_INNER);
		}

		if (m_pHatModel)
		{
			m_pHatModel->SetDecalWorldMatrix(m_iMeshIndex_Hit, m_vecBlood[m_iBloodCount]->GetWorldMatrix(), (_int)CBody_Zombie::BODY_MESH_TYPE::_INNER, true);
			m_pHatModel->Perform_Calc_DecalMap((_int)CBody_Zombie::BODY_MESH_TYPE::_INNER);
		}
	}
	else
	{
		if (m_pBodyModel)
		{
			m_pBodyModel->SetDecalWorldMatrix(m_iMeshIndex_Hit, m_vecBlood[m_iBloodCount]->GetWorldMatrix(), (_int)CBody_Zombie::BODY_MESH_TYPE::_INNER, false);
			m_pBodyModel->Perform_Calc_DecalMap((_int)CBody_Zombie::BODY_MESH_TYPE::_INNER);
		}

		if (m_pHeadModel)
		{
			m_pHeadModel->SetDecalWorldMatrix(m_iMeshIndex_Hit, m_vecBlood[m_iBloodCount]->GetWorldMatrix(), (_int)CFace_Zombie::FACE_MESH_TYPE::_INNER, false);
			m_pHeadModel->Perform_Calc_DecalMap((_int)CFace_Zombie::FACE_MESH_TYPE::_INNER);
		}

		if (m_pShirtsModel)
		{
			m_pShirtsModel->SetDecalWorldMatrix(m_iMeshIndex_Hit, m_vecBlood[m_iBloodCount]->GetWorldMatrix(), (_int)CBody_Zombie::BODY_MESH_TYPE::_INNER, false);
			m_pShirtsModel->Perform_Calc_DecalMap((_int)CBody_Zombie::BODY_MESH_TYPE::_INNER);
		}

		if (m_pPantsModel)
		{
			m_pPantsModel->SetDecalWorldMatrix(m_iMeshIndex_Hit, m_vecBlood[m_iBloodCount]->GetWorldMatrix(), (_int)CBody_Zombie::BODY_MESH_TYPE::_INNER, false);
			m_pPantsModel->Perform_Calc_DecalMap((_int)CBody_Zombie::BODY_MESH_TYPE::_INNER);
		}

		if (m_pHatModel)
		{
			m_pHatModel->SetDecalWorldMatrix(m_iMeshIndex_Hit, m_vecBlood[m_iBloodCount]->GetWorldMatrix(), (_int)CBody_Zombie::BODY_MESH_TYPE::_INNER, false);
			m_pHatModel->Perform_Calc_DecalMap((_int)CBody_Zombie::BODY_MESH_TYPE::_INNER);
		}
	}
}

void CZombie::SetBlood_STG()
{
	if (m_bBigAttack)
	{
		auto pBody = static_cast<CBody_Zombie*>(m_PartObjects[CZombie::PART_BODY]);

		auto pHitPoints = m_pController->GetBlockPoint_STG();
		auto pHitNormals = m_pController->GetNormalPoint_STG();
		auto pColliderTypes = m_pController->GetColliderType_STG();
		auto pHitParts = m_pController->GetHitPart_STG();

		for (size_t i = 0; i < (*pHitPoints).size(); ++i)
		{
			auto vBlockPoint = (*pHitPoints)[i];
			auto vBlockNormal = (*pHitNormals)[i];

			if (m_pController->GetDead() == false)
			{
				m_vecBlood_STG[i]->SetHitPart((*pHitParts)[i]);
			}
			else
			{
				m_vecBlood_STG[i]->SetHitPart(pBody->Get_Ragdoll_RigidBody((*pColliderTypes)[i]));
			}

			m_vecBlood_STG[i]->Set_Render(true);
			//m_vecBlood_STG[i]->SetWorldMatrix_With_HitNormal(m_vHitNormal);
			m_vecBlood_STG[i]->SetPosition(vBlockPoint);
			auto iType = m_pGameInstance->GetRandom_Int(1, 10);
			m_vecBlood_STG[i]->SetType(iType);

			//m_vecBlood_Drop_STG_NoRay[i]->SetWorldMatrix_With_HitNormal(m_vHitNormal);
			m_vecBlood_Drop_STG_NoRay[i]->Set_Render(true);
			m_vecBlood_Drop_STG_NoRay[i]->SetPosition(vBlockPoint);
			m_vecBlood_Drop_STG_NoRay[i]->SetType(m_pGameInstance->GetRandom_Int(1, 7));

			m_vecHit[i]->Set_Render(true);
			m_vecHit[i]->SetPosition(vBlockPoint);
		}

		pHitPoints->clear();
		pHitNormals->clear();
		pColliderTypes->clear();
		pHitParts->clear();
	}
}

void CZombie::Set_ManualMove(_bool isManualMove)
{
	m_isManualMove = isManualMove;

	if (false == m_isManualMove)
	{
		_float4				vPosition = { m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION) };
		vPosition.y += CONTROLLER_GROUND_GAP_ZOMBIE;
		m_pController->SetPosition(vPosition);
	}
}

CZombie* CZombie::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CZombie* pInstance = new CZombie(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CZombie"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CZombie::Clone(void* pArg)
{
	CZombie* pInstance = new CZombie(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CZombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CZombie::Free()
{
	__super::Free();

	Safe_Release(m_pBlackBoard);
	Safe_Release(m_pPart_Breaker);
	Release_Effect();
}
