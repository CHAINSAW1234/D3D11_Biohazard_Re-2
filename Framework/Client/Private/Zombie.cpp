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
#define HEADBLOW_BLOOD_SIZE 8.f
#define HEADBLOW_BLOOD_SIZE_DROP 8.f
#define BLOOD_SOUND_COUNT 4
#define BLOOD_SOUND_STARTINDEX 8
CZombie::CZombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster{ pDevice, pContext }
{
}

CZombie::CZombie(const CZombie& rhs)
	: CMonster{ rhs }
{
	for (auto& Pair : rhs.m_SoundTags)
	{
		m_SoundTags.emplace(Pair.first, Pair.second);
	}
}

HRESULT CZombie::Initialize_Prototype()
{
	if (FAILED(Add_SoundTags()))
		return E_FAIL;

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

	if (FAILED(Add_RagDoll_OtherParts()))
		return E_FAIL;

	if (FAILED(Initialize_Sounds()))
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

	CBlackBoard_Zombie::BLACKBOARD_ZOMBIE_DESC			Desc;
	Desc.pPart_Breaker = m_pPart_Breaker;
	Desc.pAI = this;
	m_pBlackBoard = CBlackBoard_Zombie::Create(&Desc);

	Init_BehaviorTree_Zombie();
#pragma endregion

#pragma region Effect
	m_BloodDelay = 20;
	m_HeadBlowEffectDelay = 80;
	Ready_Effect();
#pragma endregion

	Perform_Skinning();

#pragma region For SSD

	//m_pDecal_Layer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, L"Layer_Decal");

	for (size_t i = 0; i < DECAL_COUNT; ++i)
	{
		auto pDecal = new CDecal_SSD(m_pDevice, m_pContext);
		pDecal->SetUsingSound(false);
		pDecal->Initialize(nullptr);
		m_vecDecal_SSD.push_back(pDecal);
	}

#pragma endregion

	m_WindowDropSound_Delay = 20;

#pragma region DEFAULT SETUP

	m_isStart = true;

	m_RagdollWakeUpDelay = 5000;

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

	if (m_eStartType == ZOMBIE_START_TYPE::_RAG_DOLL)
	{
		if (m_bRagdollWakeUp)
		{
			if (m_RagdollWakeUpDelay + m_RagdollWakeUpTime < GetTickCount64())
			{
				m_bRagdollWakeUp = false;
				auto pBody = static_cast<CBody_Zombie*>(m_PartObjects[CZombie::PART_BODY]);
				pBody->WakeUp_Ragdoll();
			}
		}
	}

	if (m_bEvent)
		m_eBeHavior_Col;

#pragma region 절두체 컬링
	if (m_bRagdoll == false && m_bPartial_Ragdoll == false)
	{
		if (!m_pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION), 2.f))
		{
			for (auto& it : m_PartObjects)
			{
				if (it)
					it->Set_Render(false);
			}
			m_bRender = false;
		}
		else
		{
			for (auto& it : m_PartObjects)
			{
				if (it)
					it->Set_Render(true);
			}
			m_bRender = true;
		}
	}
#pragma endregion

	if(m_bDistance_Culling == true)
	{
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

#pragma region Ragdoll 피격
	if (m_bRagdoll)
	{
		if (m_pController)
		{
			if (m_pController->Is_Hit())
			{
				m_bDecal_Hit = true;

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

	//	현재 부러진 파트 타입 -1로 초기화
	m_iNew_Break_PartType = -1;

	if (m_pController && m_bRagdoll == false)
	{
		if (m_pController->Is_Hit())
		{
			m_bDecal_Hit = true;

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
			else
			{
				if (bDecalRay)
				{
					auto Type = m_pController->Get_Hit_Collider_Type();


					if (Type != COLLIDER_TYPE::CHEST && Type != COLLIDER_TYPE::PELVIS)
					{
						BREAK_PART eBreakType = BREAK_PART::_END;

						switch (eType)
						{
						case COLLIDER_TYPE::HEAD:
							eBreakType = BREAK_PART::_HEAD;
							break;

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
							if (m_pPart_Breaker->Is_BreaKPart(BREAK_PART::_R_LOWER_TABIA))
							{
								eBreakType = BREAK_PART::_R_UPPER_TABIA;
							}
							else
							{
								eBreakType = BREAK_PART::_R_LOWER_TABIA;
								eType = COLLIDER_TYPE::FOOT_R;
							}

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

						if (true == m_pPart_Breaker->Attack(eBreakType))
						{
							if (BREAK_PART::_END != eBreakType)
							{
								m_iNew_Break_PartType = static_cast<_int>(eBreakType);
								if (nullptr != m_PartObjects[CMonster::PART_BODY])
									m_PartObjects[CMonster::PART_BODY]->SetPartialRagdoll(m_iIndex_CCT, vForce, eType);

								m_bPartial_Ragdoll = true;

								if (eType != COLLIDER_TYPE::HEAD)
								{
									auto pBody = static_cast<CBody_Zombie*>(m_PartObjects[CZombie::PART_BODY]);
									m_pController->SetHitPart(pBody->Get_Ragdoll_RigidBody(Type));
									m_pGameInstance->Change_Sound_3D(m_pTransformCom, L"Break_0.mp3", (_uint)ZOMBIE_SOUND_CH::_HEAD_BREAK);
									m_pGameInstance->Set_Volume_3D(m_pTransformCom, (_uint)ZOMBIE_SOUND_CH::_HEAD_BREAK, 0.6f);
								}

								if (eType == COLLIDER_TYPE::HEAD)
								{
									m_bHeadBlow = true;
									m_bRagdoll = true;

									Play_Random_Broken_Head_Sound();

									SetRagdoll_StartPose();

									auto pBody = static_cast<CBody_Zombie*>(m_PartObjects[CZombie::PART_BODY]);
									m_pController->SetHitPart(pBody->Get_Ragdoll_RigidBody(Type));
								}
							}
						}

						if (eType == COLLIDER_TYPE::HEAD)
						{
							if (m_bBigAttack && IsPlayerNearBy())
							{
								m_pPart_Breaker->Break(BREAK_PART::_HEAD);

								m_iNew_Break_PartType = static_cast<_int>(eBreakType);
								if (nullptr != m_PartObjects[CMonster::PART_BODY])
									m_PartObjects[CMonster::PART_BODY]->SetPartialRagdoll(m_iIndex_CCT, vForce, eType);

								m_bPartial_Ragdoll = true;

								m_bHeadBlow = true;
								m_bRagdoll = true;

								Play_Random_Broken_Head_Sound();

								SetRagdoll_StartPose();

								auto pBody = static_cast<CBody_Zombie*>(m_PartObjects[CZombie::PART_BODY]);
								m_pController->SetHitPart(pBody->Get_Ragdoll_RigidBody(Type));
							}
						}
						else
						{
							if (m_bBigAttack && IsPlayerNearBy())
							{
								auto iProb = m_pGameInstance->GetRandom_Int(0, 100);

								if(iProb > 40)
								{
									if(m_pPart_Breaker->Attack_STG(eBreakType))
									{
										m_iNew_Break_PartType = static_cast<_int>(eBreakType);
										if (nullptr != m_PartObjects[CMonster::PART_BODY])
											m_PartObjects[CMonster::PART_BODY]->SetPartialRagdoll(m_iIndex_CCT, vForce, eType);

										m_bPartial_Ragdoll = true;

										auto pBody = static_cast<CBody_Zombie*>(m_PartObjects[CZombie::PART_BODY]);
										m_pController->SetHitPart(pBody->Get_Ragdoll_RigidBody(Type));
										m_pGameInstance->Change_Sound_3D(m_pTransformCom, L"Break_0.mp3", (_uint)ZOMBIE_SOUND_CH::_HEAD_BREAK);
										m_pGameInstance->Set_Volume_3D(m_pTransformCom, (_uint)ZOMBIE_SOUND_CH::_HEAD_BREAK, 0.6f);
									}
								}
							}
						}
					}
				}
			}

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

	if(m_bDistance_Culling)
	{
		if (!Distance_Culling())
			return;
	}
	else
	{
		m_bDistance_Culling = true;
	}

	__super::Late_Tick(fTimeDelta);

	if (m_pController && m_pController->GetDead() == false && m_bRender == true)
		m_pController->Update_Collider();

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom_Bounding);
#endif

#pragma region Effect

	if (m_bSetBlood)
	{
		/*For Decal*/
#ifdef DECAL
		if (m_bDecal_Hit)
			Ready_Decal();

		SetBlood();
#endif
	}

	if (m_bHeadBlow)
	{
		SetBlood_HeadBlow();
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

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_EmissiveTexture", static_cast<_uint>(i), aiTextureType_EMISSIVE)))
		{
			_bool isEmissive = false;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isEmissiveTexture", &isEmissive, sizeof(_bool))))
				return E_FAIL;
		}
		else
		{
			_bool isEmissive = true;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isEmissiveTexture", &isEmissive, sizeof(_bool))))
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

#pragma region		Selector Start Action

	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SELECTOR;
	CComposite_Node* pSelectorNode_Start_Action = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSelectorNode_Root->Insert_Child_Node(pSelectorNode_Start_Action);

#pragma region		Start Tasks

	CHide_Locker_Zombie* pTask_Hide_Locker = { CHide_Locker_Zombie::Create() };
	pTask_Hide_Locker->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Start_Action->Insert_Child_Node(pTask_Hide_Locker);

	CDead_Zombie* pTask_Dead_Zombie = { CDead_Zombie::Create() };
	pTask_Dead_Zombie->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Start_Action->Insert_Child_Node(pTask_Dead_Zombie);

#pragma endregion

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

	//	Add Task Node		=> Damage Stun Hold
	CStun_Hold_Zombie* pTask_Stun_Hold = { CStun_Hold_Zombie::Create() };
	pTask_Stun_Hold->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_Hit->Insert_Child_Node(pTask_Stun_Hold);

	//	Add Task Node		=> Damage Lost
	CDamage_Lost_Zombie* pTask_Damage_Lost = { CDamage_Lost_Zombie::Create() };
	pTask_Damage_Lost->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_Hit->Insert_Child_Node(pTask_Damage_Lost);

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

	//	Add_Task Node (Turn Lost)
	CTurn_Lost_Zombie* pTask_Turn_Lost = { CTurn_Lost_Zombie::Create() };
	pTask_Turn_Lost->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Same_Region_Player->Insert_Child_Node(pTask_Turn_Lost);

	//	Add Task Node (Move Lost)
	CMove_To_Target_Lost_Zombie* pTask_Move_Lost = { CMove_To_Target_Lost_Zombie::Create() };
	pTask_Move_Lost->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Same_Region_Player->Insert_Child_Node(pTask_Move_Lost);

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
	*/	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SELECTOR;
	CComposite_Node* pSelectorNode_RootChild_Idle = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSelectorNode_Root->Insert_Child_Node(pSelectorNode_RootChild_Idle);

	//	Need Creep Pose
	//	Add Task Node		( Creep )
	CCreep_Zombie* pTask_Creep = { CCreep_Zombie::Create() };
	pTask_Creep->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_Idle->Insert_Child_Node(pTask_Creep);

	CWait_Lost_Zombie* pTask_Wait_Lost = { CWait_Lost_Zombie::Create() };
	pTask_Wait_Lost->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_Idle->Insert_Child_Node(pTask_Wait_Lost);

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

#pragma region		Task Set Focus Player

	CSet_Focus_Player* pTask_Set_Focus_Player = { CSet_Focus_Player::Create() };
	pTask_Set_Focus_Player->SetBlackBoard(m_pBlackBoard);
	pSequenceNode_Additional_Blend_Anim->Insert_Child_Node(pTask_Set_Focus_Player);

#pragma endregion

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

#pragma region		Task Foot Sound 

	CFoot_Sound_Zombie* pTask_Foot_Sound = { CFoot_Sound_Zombie::Create() };
	pTask_Foot_Sound->SetBlackBoard(m_pBlackBoard);
	pSequenceNode_Additional_Blend_Anim->Insert_Child_Node(pTask_Foot_Sound);

#pragma endregion		//	Sequence Root Child

#pragma region		Task Break Sound 

	CBreak_Sound_Zombie* pTask_Break_Sound = { CBreak_Sound_Zombie::Create() };
	pTask_Break_Sound->SetBlackBoard(m_pBlackBoard);
	pSequenceNode_Additional_Blend_Anim->Insert_Child_Node(pTask_Break_Sound);

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
	ClothesShirtsDesc.ppPart_Breaker = &m_pPart_Breaker;

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
	ClothesPantsDesc.ppPart_Breaker = &m_pPart_Breaker;

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
	PartBreakerDesc.pFaceModel = static_cast<CModel*>(m_PartObjects[CMonster::PART_FACE]->Get_Component(TEXT("Com_Model")));
	PartBreakerDesc.pPants_Model = static_cast<CModel*>(m_PartObjects[CMonster::PART_PANTS]->Get_Component(TEXT("Com_Model")));
	PartBreakerDesc.pShirts_Model = static_cast<CModel*>(m_PartObjects[CMonster::PART_SHIRTS]->Get_Component(TEXT("Com_Model")));
	PartBreakerDesc.iBodyType = m_iBody_ID;
	PartBreakerDesc.iClothesModelID_Shirts = m_iShirts_ID;
	PartBreakerDesc.iClothesModelID_Pants = m_iPants_ID;

	CPart_Breaker_Zombie* pPart_Breaker = { CPart_Breaker_Zombie::Create(&PartBreakerDesc) };
	m_pPart_Breaker = pPart_Breaker;

	if (nullptr == m_pPart_Breaker)
		return E_FAIL;

	return S_OK;
}

HRESULT CZombie::Initialize_Sounds()
{
	if (FAILED(m_pGameInstance->Add_Object_Sound(m_pTransformCom, static_cast<_uint>(ZOMBIE_SOUND_CH::_END))))
		return E_FAIL;

	for (_uint i = 0; i < static_cast<_uint>(ZOMBIE_SOUND_CH::_END); ++i)
	{
		m_pGameInstance->Set_Distance_3D(m_pTransformCom, i, 0.f, 7.f);
	}

	return S_OK;
}

HRESULT CZombie::Add_RagDoll_OtherParts()
{
	CRagdoll_Physics* pRagDoll = { static_cast<CBody_Zombie*>(m_PartObjects[static_cast<_uint>(PART_ID::PART_BODY)])->Get_RagDoll_Ptr() };
	if (nullptr == pRagDoll)
		return E_FAIL;

	static_cast<CClothes_Zombie*>(m_PartObjects[static_cast<_uint>(PART_ID::PART_SHIRTS)])->Set_RagDoll_Ptr(pRagDoll);
	static_cast<CClothes_Zombie*>(m_PartObjects[static_cast<_uint>(PART_ID::PART_PANTS)])->Set_RagDoll_Ptr(pRagDoll);

	CModel* pBody_Model = { static_cast<CModel*>(static_cast<CClothes_Zombie*>(m_PartObjects[static_cast<_uint>(PART_ID::PART_BODY)])->Get_Component(TEXT("Com_Model"))) };
	CModel* pShirt_Model = { static_cast<CModel*>(static_cast<CClothes_Zombie*>(m_PartObjects[static_cast<_uint>(PART_ID::PART_SHIRTS)])->Get_Component(TEXT("Com_Model"))) };
	CModel* pPants_Model = { static_cast<CModel*>(static_cast<CClothes_Zombie*>(m_PartObjects[static_cast<_uint>(PART_ID::PART_PANTS)])->Get_Component(TEXT("Com_Model"))) };

	if (nullptr == pBody_Model || nullptr == pShirt_Model || nullptr == pPants_Model)
		return E_FAIL;

	if (FAILED(pShirt_Model->Link_Bone_Auto_RagDoll(pBody_Model, pRagDoll)))
		return E_FAIL;
	if (FAILED(pPants_Model->Link_Bone_Auto_RagDoll(pBody_Model, pRagDoll)))
		return E_FAIL;


	return S_OK;
}

HRESULT CZombie::Add_SoundTags()
{
	if (FAILED(Add_SoundTags_Idle_Voice()))
		return E_FAIL;

	if (FAILED(Add_SoundTags_Move_Voice()))
		return E_FAIL;

	if (FAILED(Add_SoundTags_KnockBack_Voice()))
		return E_FAIL;

	if (FAILED(Add_SoundTags_StandUp_Voice()))
		return E_FAIL;

	if (FAILED(Add_SoundTags_Hit_EF()))
		return E_FAIL;

	if (FAILED(Add_SoundTags_Bite_Voice()))
		return E_FAIL;

	if (FAILED(Add_SoundTags_Bite_Reject_Voice()))
		return E_FAIL;

	if (FAILED(Add_SoundTags_Bite_Kill_Voice()))
		return E_FAIL;

	if (FAILED(Add_SoundTags_Bite_Creep_Voice()))
		return E_FAIL;

	if (FAILED(Add_SoundTags_Bite_Creep_Reject_Voice()))
		return E_FAIL;

	if (FAILED(Add_SoundTags_Bite_Creep_Kill_Voice()))
		return E_FAIL;

	if (FAILED(Add_SoundTags_Hold_Voice()))
		return E_FAIL;

	if (FAILED(Add_SoundTags_Foot_Foot()))
		return E_FAIL;

	if (FAILED(Add_SoundTags_Foot_Creep_Foot()))
		return E_FAIL;

	if (FAILED(Add_SoundTags_Drop_Body_Foot()))
		return E_FAIL;

	if (FAILED(Add_SoundTags_ETC()))
		return E_FAIL;

	if (FAILED(Add_SoundTags_Interact_Interact()))
		return E_FAIL;

	if (FAILED(Add_SoundTags_BreakHead_Break()))
		return E_FAIL;

	if (FAILED(Add_SoundTags_BreakOthers_Break()))
		return E_FAIL;

	return S_OK;
}

HRESULT CZombie::Add_SoundTags_Idle_Voice()
{
	wstring						strExtTag = { TEXT(".mp3") };


	wstring						strIdleMaleSoundTag = { TEXT("em_Idle_Male_") };
	vector<wstring>				IdleMaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_IDLE_MALE; ++i)
	{
		wstring			strTag = { strIdleMaleSoundTag + to_wstring(i) + strExtTag };
		IdleMaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_IDLE_MALE, IdleMaleSoundTags);


	wstring						strIdleMaleBigSoundTag = { TEXT("em_Idle_Male_Big_") };
	vector<wstring>				IdleMaleBigSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_IDLE_MALE_BIG; ++i)
	{
		wstring			strTag = { strIdleMaleBigSoundTag + to_wstring(i) + strExtTag };
		IdleMaleBigSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_IDLE_MALE_BIG, IdleMaleBigSoundTags);


	wstring						strIdleFemaleSoundTag = { TEXT("em_Idle_Female_") };
	vector<wstring>				IdleFemaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_IDLE_FEMALE; ++i)
	{
		wstring			strTag = { strIdleFemaleSoundTag + to_wstring(i) + strExtTag };
		IdleFemaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_IDLE_FEMALE, IdleFemaleSoundTags);

	return S_OK;
}

HRESULT CZombie::Add_SoundTags_Move_Voice()
{
	wstring						strExtTag = { TEXT(".mp3") };

	wstring						strMoveMaleSoundTag = { TEXT("em_Move_Male_") };
	vector<wstring>				MoveMaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_MOVE_MALE; ++i)
	{
		wstring			strTag = { strMoveMaleSoundTag + to_wstring(i) + strExtTag };
		MoveMaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_MOVE_MALE, MoveMaleSoundTags);


	wstring						strMoveMaleBigSoundTag = { TEXT("em_Move_Male_Big_") };
	vector<wstring>				MoveMaleBigSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_MOVE_MALE_BIG; ++i)
	{
		wstring			strTag = { strMoveMaleBigSoundTag + to_wstring(i) + strExtTag };
		MoveMaleBigSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_MOVE_MALE_BIG, MoveMaleBigSoundTags);


	wstring						strMoveFemaleSoundTag = { TEXT("em_Move_Female_") };
	vector<wstring>				MoveFemaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_MOVE_FEMALE; ++i)
	{
		wstring			strTag = { strMoveFemaleSoundTag + to_wstring(i) + strExtTag };
		MoveFemaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_MOVE_FEMALE, MoveFemaleSoundTags);

	return S_OK;
}

HRESULT CZombie::Add_SoundTags_Bite_Voice()
{
	wstring						strExtTag = { TEXT(".mp3") };

	wstring						strBiteMaleSoundTag = { TEXT("em_Bite_Male_") };
	vector<wstring>				BiteMaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_BITE_MALE; ++i)
	{
		wstring			strTag = { strBiteMaleSoundTag + to_wstring(i) + strExtTag };
		BiteMaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_BITE_MALE, BiteMaleSoundTags);


	wstring						strBiteMaleBigSoundTag = { TEXT("em_Bite_Male_Big_") };
	vector<wstring>				BiteMaleBigSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_BITE_MALE_BIG; ++i)
	{
		wstring			strTag = { strBiteMaleBigSoundTag + to_wstring(i) + strExtTag };
		BiteMaleBigSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_BITE_MALE_BIG, BiteMaleBigSoundTags);


	wstring						strBiteFemaleSoundTag = { TEXT("em_Bite_Female_") };
	vector<wstring>				BiteFemaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_BITE_FEMALE; ++i)
	{
		wstring			strTag = { strBiteFemaleSoundTag + to_wstring(i) + strExtTag };
		BiteFemaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_BITE_FEMALE, BiteFemaleSoundTags);

	return S_OK;
}

HRESULT CZombie::Add_SoundTags_KnockBack_Voice()
{
	wstring						strExtTag = { TEXT(".mp3") };

	wstring						strKnockBackMaleSoundTag = { TEXT("em_KnockBack_Male_") };
	vector<wstring>				KnockBackMaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_KNOCKBACK_MALE; ++i)
	{
		wstring			strTag = { strKnockBackMaleSoundTag + to_wstring(i) + strExtTag };
		KnockBackMaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_KNOCKBACK_MALE, KnockBackMaleSoundTags);


	wstring						strKnockBackMaleBigSoundTag = { TEXT("em_KnockBack_Male_Big_") };
	vector<wstring>				KnockBackMaleBigSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_KNOCKBACK_MALE_BIG; ++i)
	{
		wstring			strTag = { strKnockBackMaleBigSoundTag + to_wstring(i) + strExtTag };
		KnockBackMaleBigSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_KNOCKBACK_MALE_BIG, KnockBackMaleBigSoundTags);


	wstring						strKnockBackFemaleSoundTag = { TEXT("em_KnockBack_Female_") };
	vector<wstring>				KnockBackFemaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_KNOCKBACK_FEMALE; ++i)
	{
		wstring			strTag = { strKnockBackFemaleSoundTag + to_wstring(i) + strExtTag };
		KnockBackFemaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_KNOCKBACK_FEMALE, KnockBackFemaleSoundTags);

	return S_OK;
}

HRESULT CZombie::Add_SoundTags_Stun_Voice()
{
	wstring						strExtTag = { TEXT(".mp3") };

	wstring						strKnockBackMaleSoundTag = { TEXT("em_Stun_Male_") };
	vector<wstring>				KnockBackMaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_STUN_MALE; ++i)
	{
		wstring			strTag = { strKnockBackMaleSoundTag + to_wstring(i) + strExtTag };
		KnockBackMaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_STUN_MALE, KnockBackMaleSoundTags);


	wstring						strKnockBackMaleBigSoundTag = { TEXT("em_Stun_Male_Big_") };
	vector<wstring>				KnockBackMaleBigSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_STUN_MALE_BIG; ++i)
	{
		wstring			strTag = { strKnockBackMaleBigSoundTag + to_wstring(i) + strExtTag };
		KnockBackMaleBigSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_STUN_MALE_BIG, KnockBackMaleBigSoundTags);


	wstring						strKnockBackFemaleSoundTag = { TEXT("em_Stun_Female_") };
	vector<wstring>				KnockBackFemaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_STUN_FEMALE; ++i)
	{
		wstring			strTag = { strKnockBackFemaleSoundTag + to_wstring(i) + strExtTag };
		KnockBackFemaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_STUN_FEMALE, KnockBackFemaleSoundTags);

	return S_OK;
}

HRESULT CZombie::Add_SoundTags_StandUp_Voice()
{
	wstring						strExtTag = { TEXT(".mp3") };

	wstring						strStandUpMaleSoundTag = { TEXT("em_StandUp_Male_") };
	vector<wstring>				StandUpMaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_STAND_UP_MALE; ++i)
	{
		wstring			strTag = { strStandUpMaleSoundTag + to_wstring(i) + strExtTag };
		StandUpMaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_STAND_UP_MALE, StandUpMaleSoundTags);


	wstring						strStandUpMaleBigSoundTag = { TEXT("em_StandUp_Male_Big_") };
	vector<wstring>				StandUpMaleBigSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_STAND_UP_MALE_BIG; ++i)
	{
		wstring			strTag = { strStandUpMaleBigSoundTag + to_wstring(i) + strExtTag };
		StandUpMaleBigSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_STAND_UP_MALE_BIG, StandUpMaleBigSoundTags);


	wstring						strStandUpFemaleSoundTag = { TEXT("em_StandUp_Female_") };
	vector<wstring>				StandUpFemaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_STAND_UP_FEMALE; ++i)
	{
		wstring			strTag = { strStandUpFemaleSoundTag + to_wstring(i) + strExtTag };
		StandUpFemaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_STAND_UP_FEMALE, StandUpFemaleSoundTags);

	return S_OK;
}

HRESULT CZombie::Add_SoundTags_Hit_EF()
{
	wstring						strExtTag = { TEXT(".mp3") };

	wstring						strHitSoundTag = { TEXT("em_common_dmg_hit_media.bnk.2_") };
	vector<wstring>				HitSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_HIT; ++i)
	{
		if (4 == i || 16 == i || 17 == i || 22 == i || 24 == i || 28 == i || 30 == i)
			continue;

		wstring			strTag = { strHitSoundTag + to_wstring(i) + strExtTag };
		HitSoundTags.push_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_HIT, HitSoundTags);

	return S_OK;
}

HRESULT CZombie::Add_SoundTags_Bite_Reject_Voice()
{
	wstring						strExtTag = { TEXT(".mp3") };

	wstring						strBiteRejectMaleSoundTag = { TEXT("em_Bite_Reject_Male_") };
	vector<wstring>				BiteRejectMaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_BITE_REJECT_MALE; ++i)
	{
		wstring			strTag = { strBiteRejectMaleSoundTag + to_wstring(i) + strExtTag };
		BiteRejectMaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_BITE_REJECT_MALE, BiteRejectMaleSoundTags);


	wstring						strBiteRejectMaleBigSoundTag = { TEXT("em_Bite_Reject_Male_Big_") };
	vector<wstring>				BiteRejectMaleBigSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_BITE_REJECT_MALE_BIG; ++i)
	{
		wstring			strTag = { strBiteRejectMaleBigSoundTag + to_wstring(i) + strExtTag };
		BiteRejectMaleBigSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_BITE_REJECT_MALE_BIG, BiteRejectMaleBigSoundTags);


	wstring						strBiteRejectFemaleSoundTag = { TEXT("em_Bite_Reject_Female_") };
	vector<wstring>				BiteRejectFemaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_BITE_REJECT_FEMALE; ++i)
	{
		wstring			strTag = { strBiteRejectFemaleSoundTag + to_wstring(i) + strExtTag };
		BiteRejectFemaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_BITE_REJECT_FEMALE, BiteRejectFemaleSoundTags);

	return S_OK;
}

HRESULT CZombie::Add_SoundTags_Bite_Kill_Voice()
{
	wstring						strExtTag = { TEXT(".mp3") };

	wstring						strBiteKillMaleSoundTag = { TEXT("em_Bite_Kill_Male_") };
	vector<wstring>				BiteKillMaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_BITE_KILL_MALE; ++i)
	{
		wstring			strTag = { strBiteKillMaleSoundTag + to_wstring(i) + strExtTag };
		BiteKillMaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_BITE_KILL_MALE, BiteKillMaleSoundTags);


	wstring						strBiteKillMaleBigSoundTag = { TEXT("em_Bite_Kill_Male_Big_") };
	vector<wstring>				BiteKillMaleBigSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_BITE_KILL_MALE_BIG; ++i)
	{
		wstring			strTag = { strBiteKillMaleBigSoundTag + to_wstring(i) + strExtTag };
		BiteKillMaleBigSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_BITE_KILL_MALE_BIG, BiteKillMaleBigSoundTags);


	wstring						strBiteKillFemaleSoundTag = { TEXT("em_Bite_Kill_Female_") };
	vector<wstring>				BiteKillFemaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_BITE_KILL_FEMALE; ++i)
	{
		wstring			strTag = { strBiteKillFemaleSoundTag + to_wstring(i) + strExtTag };
		BiteKillFemaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_BITE_KILL_FEMALE, BiteKillFemaleSoundTags);

	return S_OK;
}

HRESULT CZombie::Add_SoundTags_Bite_Creep_Voice()
{
	wstring						strExtTag = { TEXT(".mp3") };

	wstring						strBiteCreepMaleSoundTag = { TEXT("em_Bite_Creep_Male_") };
	vector<wstring>				BiteCreepMaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_BITE_CREEP_MALE; ++i)
	{
		wstring			strTag = { strBiteCreepMaleSoundTag + to_wstring(i) + strExtTag };
		BiteCreepMaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_BITE_CREEP_MALE, BiteCreepMaleSoundTags);


	wstring						strBiteCreepMaleBigSoundTag = { TEXT("em_Bite_Creep_Male_Big_") };
	vector<wstring>				BiteCreepMaleBigSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_BITE_CREEP_MALE_BIG; ++i)
	{
		wstring			strTag = { strBiteCreepMaleBigSoundTag + to_wstring(i) + strExtTag };
		BiteCreepMaleBigSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_BITE_CREEP_MALE_BIG, BiteCreepMaleBigSoundTags);


	wstring						strBiteCreepFemaleSoundTag = { TEXT("em_Bite_Creep_Female_") };
	vector<wstring>				BiteCreepFemaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_BITE_CREEP_FEMALE; ++i)
	{
		wstring			strTag = { strBiteCreepFemaleSoundTag + to_wstring(i) + strExtTag };
		BiteCreepFemaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_BITE_CREEP_FEMALE, BiteCreepFemaleSoundTags);

	return S_OK;
}

HRESULT CZombie::Add_SoundTags_Bite_Creep_Reject_Voice()
{
	wstring						strExtTag = { TEXT(".mp3") };

	wstring						strBiteCreepRejectMaleSoundTag = { TEXT("em_Bite_Creep_Reject_Male_") };
	vector<wstring>				BiteCreepRejectMaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_BITE_CREEP_REJECT_MALE; ++i)
	{
		wstring			strTag = { strBiteCreepRejectMaleSoundTag + to_wstring(i) + strExtTag };
		BiteCreepRejectMaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_BITE_CREEP_REJECT_MALE, BiteCreepRejectMaleSoundTags);


	wstring						strBiteCreepRejectMaleBigSoundTag = { TEXT("em_Bite_Creep_Reject_Male_Big_") };
	vector<wstring>				BiteCreepRejectMaleBigSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_BITE_CREEP_REJECT_MALE_BIG; ++i)
	{
		wstring			strTag = { strBiteCreepRejectMaleBigSoundTag + to_wstring(i) + strExtTag };
		BiteCreepRejectMaleBigSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_BITE_CREEP_REJECT_MALE_BIG, BiteCreepRejectMaleBigSoundTags);


	wstring						strBiteCreepRejectFemaleSoundTag = { TEXT("em_Bite_Creep_Reject_Female_") };
	vector<wstring>				BiteCreepRejectFemaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_BITE_CREEP_REJECT_FEMALE; ++i)
	{
		wstring			strTag = { strBiteCreepRejectFemaleSoundTag + to_wstring(i) + strExtTag };
		BiteCreepRejectFemaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_BITE_CREEP_REJECT_FEMALE, BiteCreepRejectFemaleSoundTags);

	return S_OK;
}

HRESULT CZombie::Add_SoundTags_Bite_Creep_Kill_Voice()
{
	wstring						strExtTag = { TEXT(".mp3") };

	wstring						strBiteCreepKillMaleSoundTag = { TEXT("em_Bite_Creep_Kill_Male_") };
	vector<wstring>				BiteCreepKillMaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_BITE_CREEP_KILL_MALE; ++i)
	{
		wstring			strTag = { strBiteCreepKillMaleSoundTag + to_wstring(i) + strExtTag };
		BiteCreepKillMaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_BITE_CREEP_KILL_MALE, BiteCreepKillMaleSoundTags);


	wstring						strBiteCreepKillMaleBigSoundTag = { TEXT("em_Bite_Creep_Kill_Male_Big_") };
	vector<wstring>				BiteCreepKillMaleBigSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_BITE_CREEP_KILL_MALE_BIG; ++i)
	{
		wstring			strTag = { strBiteCreepKillMaleBigSoundTag + to_wstring(i) + strExtTag };
		BiteCreepKillMaleBigSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_BITE_CREEP_KILL_MALE_BIG, BiteCreepKillMaleBigSoundTags);


	wstring						strBiteCreepKillFemaleSoundTag = { TEXT("em_Bite_Creep_Kill_Female_") };
	vector<wstring>				BiteCreepKillFemaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_BITE_CREEP_KILL_FEMALE; ++i)
	{
		wstring			strTag = { strBiteCreepKillFemaleSoundTag + to_wstring(i) + strExtTag };
		BiteCreepKillFemaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_BITE_CREEP_KILL_FEMALE, BiteCreepKillFemaleSoundTags);

	return S_OK;
}

HRESULT CZombie::Add_SoundTags_Hold_Voice()
{
	wstring						strExtTag = { TEXT(".mp3") };

	wstring						strHoldMaleSoundTag = { TEXT("em_Hold_Male_") };
	vector<wstring>				HoldMaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_HOLD_MALE; ++i)
	{
		wstring			strTag = { strHoldMaleSoundTag + to_wstring(i) + strExtTag };
		HoldMaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_HOLD_MALE, HoldMaleSoundTags);


	wstring						strHoldMaleBigSoundTag = { TEXT("em_Hold_Male_Big_") };
	vector<wstring>				HoldMaleBigSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_HOLD_MALE_BIG; ++i)
	{
		wstring			strTag = { strHoldMaleBigSoundTag + to_wstring(i) + strExtTag };
		HoldMaleBigSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_HOLD_MALE_BIG, HoldMaleBigSoundTags);


	wstring						strHoldFemaleSoundTag = { TEXT("em_Hold_Female_") };
	vector<wstring>				HoldFemaleSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_HOLD_FEMALE; ++i)
	{
		wstring			strTag = { strHoldFemaleSoundTag + to_wstring(i) + strExtTag };
		HoldFemaleSoundTags.emplace_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_HOLD_FEMALE, HoldFemaleSoundTags);

	return S_OK;
}

HRESULT CZombie::Add_SoundTags_Foot_Foot()
{
	wstring						strExtTag = { TEXT(".mp3") };
	wstring						strFootSoundTag = { TEXT("em_Foot_") };
	vector<wstring>				FootSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_FOOT; ++i)
	{
		wstring			strTag = { strFootSoundTag + to_wstring(i) + strExtTag };
		FootSoundTags.push_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_FOOT, FootSoundTags);

	return S_OK;
}

HRESULT CZombie::Add_SoundTags_Foot_Creep_Foot()
{
	wstring						strExtTag = { TEXT(".mp3") };
	wstring						strFootSoundTag = { TEXT("em_Foot_Creep_") };
	vector<wstring>				FootSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_FOOT_CREEP; ++i)
	{
		wstring			strTag = { strFootSoundTag + to_wstring(i) + strExtTag };
		FootSoundTags.push_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_FOOT_CREEP, FootSoundTags);

	return S_OK;
}

HRESULT CZombie::Add_SoundTags_Drop_Body_Foot()
{
	wstring						strExtTag = { TEXT(".mp3") };
	wstring						strDropBodySoundTag = { TEXT("em_Drop_Body_") };
	vector<wstring>				DropBodySoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_DROP_BODY; ++i)
	{
		wstring			strTag = { strDropBodySoundTag + to_wstring(i) + strExtTag };
		DropBodySoundTags.push_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_DROP_BODY, DropBodySoundTags);

	return S_OK;
}

HRESULT CZombie::Add_SoundTags_ETC()
{
	return S_OK;
}

HRESULT CZombie::Add_SoundTags_Interact_Interact()
{
	wstring						strExtTag = { TEXT(".mp3") };

	wstring						strKnockWindowSoundTag = { TEXT("em_Knock_Window_") };
	vector<wstring>				KnockWindowSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_KNOCK_WINDOW; ++i)
	{
		wstring			strTag = { strKnockWindowSoundTag + to_wstring(i) + strExtTag };
		KnockWindowSoundTags.push_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_KNOCK_WINDOW, KnockWindowSoundTags);

	wstring						strKnockDoorSoundTag = { TEXT("em_Knock_Door_") };
	vector<wstring>				KnockDoorTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_KNOCK_DOOR; ++i)
	{
		wstring			strTag = { strKnockDoorSoundTag + to_wstring(i) + strExtTag };
		KnockDoorTags.push_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_KNOCK_DOOR, KnockDoorTags);


	wstring						strBreakWindowSoundTag = { TEXT("em_Break_Window_") };
	vector<wstring>				BreakWindowSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_BREAK_WINDOW; ++i)
	{
		wstring			strTag = { strBreakWindowSoundTag + to_wstring(i) + strExtTag };
		BreakWindowSoundTags.push_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_BREAK_WINDOW, BreakWindowSoundTags);

	wstring						strOpenDoorSoundTag = { TEXT("em_Open_Door_") };
	vector<wstring>				OpenDoorSOundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_OPEN_DOOR; ++i)
	{
		wstring			strTag = { strOpenDoorSoundTag + to_wstring(i) + strExtTag };
		OpenDoorSOundTags.push_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_OPEN_DOOR, OpenDoorSOundTags);

	return S_OK;
}

HRESULT CZombie::Add_SoundTags_BreakHead_Break()
{
	wstring						strExtTag = { TEXT(".mp3") };

	wstring						strBreakHeadSoundTag = { TEXT("em_Break_Head_") };
	vector<wstring>				BreakHeadSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_BREAK_HEAD; ++i)
	{
		wstring			strTag = { strBreakHeadSoundTag + to_wstring(i) + strExtTag };
		BreakHeadSoundTags.push_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_BREAK_HEAD, BreakHeadSoundTags);

	return S_OK;
}

HRESULT CZombie::Add_SoundTags_BreakOthers_Break()
{
	wstring						strExtTag = { TEXT(".mp3") };

	wstring						strBreakOtherSoundTag = { TEXT("em_Break_Other_") };
	vector<wstring>				BreakOtherSoundTags;
	for (_uint i = 1; i <= ZOMBIE_NUM_SOUND_BREAK_OTHER; ++i)
	{
		wstring			strTag = { strBreakOtherSoundTag + to_wstring(i) + strExtTag };
		BreakOtherSoundTags.push_back(strTag);
	}
	m_SoundTags.emplace(ZOMBIE_SOUND_TYPE::_BREAK_HEAD, BreakOtherSoundTags);

	return S_OK;
}

void CZombie::Play_Random_Hit_Sound()
{
	unordered_map<ZOMBIE_SOUND_TYPE, vector<wstring>>::iterator			iter = { m_SoundTags.find(ZOMBIE_SOUND_TYPE::_HIT) };
	if (iter == m_SoundTags.end())
		return;

	_uint				iNumHitSound = { static_cast<_uint>(iter->second.size()) };
	if (iNumHitSound == 0)
		return;

	_int				iRandomIndex = { m_pGameInstance->GetRandom_Int(0, static_cast<_int>(iNumHitSound) - 1) };
	wstring				strSoundTag = { iter->second[iRandomIndex] };
	_float				fRandomVolume = { m_pGameInstance->GetRandom_Real(ZOMBIE_MIN_VOLUME_HIT, ZOMBIE_MAX_VOLUME_HIT) };

	Change_Sound(strSoundTag, static_cast<_uint>(ZOMBIE_SOUND_CH::_EF));
	Set_Volume(fRandomVolume, static_cast<_uint>(ZOMBIE_SOUND_CH::_EF));
}

void CZombie::Play_Random_Stun_Sound()
{
	ZOMBIE_SOUND_TYPE			eSoundType = { ZOMBIE_SOUND_TYPE::_END };

	if (ZOMBIE_BODY_TYPE::_MALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_STUN_MALE;
	else if (ZOMBIE_BODY_TYPE::_FEMALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_STUN_FEMALE;
	else if (ZOMBIE_BODY_TYPE::_MALE_BIG == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_STUN_MALE_BIG;

	unordered_map<ZOMBIE_SOUND_TYPE, vector<wstring>>::iterator			iter = { m_SoundTags.find(eSoundType) };
	if (iter == m_SoundTags.end())
		return;

	_uint				iNumStunSound = { static_cast<_uint>(iter->second.size()) };
	if (iNumStunSound == 0)
		return;

	_int				iRandomIndex = { m_pGameInstance->GetRandom_Int(0, static_cast<_int>(iNumStunSound) - 1) };
	wstring				strSoundTag = { iter->second[iRandomIndex] };
	_float				fRandomVolume = { m_pGameInstance->GetRandom_Real(ZOMBIE_MIN_VOLUME_STUN, ZOMBIE_MAX_VOLUME_STUN) };

	Change_Sound(strSoundTag, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
	Set_Volume(fRandomVolume, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
}

void CZombie::Play_Random_KnockBack_Sound()
{
	ZOMBIE_SOUND_TYPE			eSoundType = { ZOMBIE_SOUND_TYPE::_END };

	if (ZOMBIE_BODY_TYPE::_MALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_KNOCKBACK_MALE;
	else if (ZOMBIE_BODY_TYPE::_FEMALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_KNOCKBACK_FEMALE;
	else if (ZOMBIE_BODY_TYPE::_MALE_BIG == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_KNOCKBACK_MALE_BIG;

	unordered_map<ZOMBIE_SOUND_TYPE, vector<wstring>>::iterator			iter = { m_SoundTags.find(eSoundType) };
	if (iter == m_SoundTags.end())
		return;

	_uint				iNumKnockBackSound = { static_cast<_uint>(iter->second.size()) };
	if (iNumKnockBackSound == 0)
		return;

	_int				iRandomIndex = { m_pGameInstance->GetRandom_Int(0, static_cast<_int>(iNumKnockBackSound) - 1) };
	wstring				strSoundTag = { iter->second[iRandomIndex] };
	_float				fRandomVolume = { m_pGameInstance->GetRandom_Real(ZOMBIE_MIN_VOLUME_KNOCKBACK, ZOMBIE_MAX_VOLUME_KNOCKBACK) };

	Change_Sound(strSoundTag, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
	Set_Volume(fRandomVolume, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
}

void CZombie::Play_Random_StandUp_Sound()
{
	ZOMBIE_SOUND_TYPE			eSoundType = { ZOMBIE_SOUND_TYPE::_END };

	if (ZOMBIE_BODY_TYPE::_MALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_STAND_UP_MALE;
	else if (ZOMBIE_BODY_TYPE::_FEMALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_STAND_UP_FEMALE;
	else if (ZOMBIE_BODY_TYPE::_MALE_BIG == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_STAND_UP_MALE_BIG;

	unordered_map<ZOMBIE_SOUND_TYPE, vector<wstring>>::iterator			iter = { m_SoundTags.find(eSoundType) };
	if (iter == m_SoundTags.end())
		return;

	_uint				iNumStandUpSound = { static_cast<_uint>(iter->second.size()) };
	if (iNumStandUpSound == 0)
		return;

	_int				iRandomIndex = { m_pGameInstance->GetRandom_Int(0, static_cast<_int>(iNumStandUpSound) - 1) };
	wstring				strSoundTag = { iter->second[iRandomIndex] };
	_float				fRandomVolume = { m_pGameInstance->GetRandom_Real(ZOMBIE_MIN_VOLUME_STAND_UP, ZOMBIE_MAX_VOLUME_STAND_UP) };

	Change_Sound(strSoundTag, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
	Set_Volume(fRandomVolume, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
}

void CZombie::Play_Random_Idle_Sound()
{
	ZOMBIE_SOUND_TYPE			eSoundType = { ZOMBIE_SOUND_TYPE::_END };

	if (ZOMBIE_BODY_TYPE::_MALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_IDLE_MALE;
	else if (ZOMBIE_BODY_TYPE::_FEMALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_IDLE_FEMALE;
	else if (ZOMBIE_BODY_TYPE::_MALE_BIG == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_IDLE_MALE_BIG;

	unordered_map<ZOMBIE_SOUND_TYPE, vector<wstring>>::iterator			iter = { m_SoundTags.find(eSoundType) };
	if (iter == m_SoundTags.end())
		return;

	_uint				iNumIdleSound = { static_cast<_uint>(iter->second.size()) };
	if (iNumIdleSound == 0)
		return;

	_int				iRandomIndex = { m_pGameInstance->GetRandom_Int(0, static_cast<_int>(iNumIdleSound) - 1) };
	wstring				strSoundTag = { iter->second[iRandomIndex] };
	_float				fRandomVolume = { m_pGameInstance->GetRandom_Real(ZOMBIE_MIN_VOLUME_IDLE, ZOMBIE_MAX_VOLUME_IDLE) };

	Change_Sound(strSoundTag, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
	Set_Volume(fRandomVolume, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
}

void CZombie::Play_Random_Move_Sound()
{
	ZOMBIE_SOUND_TYPE			eSoundType = { ZOMBIE_SOUND_TYPE::_END };

	if (ZOMBIE_BODY_TYPE::_MALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_MOVE_MALE;
	else if (ZOMBIE_BODY_TYPE::_FEMALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_MOVE_FEMALE;
	else if (ZOMBIE_BODY_TYPE::_MALE_BIG == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_MOVE_MALE_BIG;

	unordered_map<ZOMBIE_SOUND_TYPE, vector<wstring>>::iterator			iter = { m_SoundTags.find(eSoundType) };
	if (iter == m_SoundTags.end())
		return;

	_uint				iNumMoveSound = { static_cast<_uint>(iter->second.size()) };
	if (iNumMoveSound == 0)
		return;

	_int				iRandomIndex = { m_pGameInstance->GetRandom_Int(0, static_cast<_int>(iNumMoveSound) - 1) };
	wstring				strSoundTag = { iter->second[iRandomIndex] };
	_float				fRandomVolume = { m_pGameInstance->GetRandom_Real(ZOMBIE_MIN_VOLUME_MOVE, ZOMBIE_MAX_VOLUME_MOVE) };

	Change_Sound(strSoundTag, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
	Set_Volume(fRandomVolume, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
}

void CZombie::Play_Random_Hold_Sound()
{
	ZOMBIE_SOUND_TYPE			eSoundType = { ZOMBIE_SOUND_TYPE::_END };

	if (ZOMBIE_BODY_TYPE::_MALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_HOLD_MALE;
	else if (ZOMBIE_BODY_TYPE::_FEMALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_HOLD_FEMALE;
	else if (ZOMBIE_BODY_TYPE::_MALE_BIG == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_HOLD_MALE_BIG;

	unordered_map<ZOMBIE_SOUND_TYPE, vector<wstring>>::iterator			iter = { m_SoundTags.find(eSoundType) };
	if (iter == m_SoundTags.end())
		return;

	_uint				iNumHoldSound = { static_cast<_uint>(iter->second.size()) };
	if (iNumHoldSound == 0)
		return;

	_int				iRandomIndex = { m_pGameInstance->GetRandom_Int(0, static_cast<_int>(iNumHoldSound) - 1) };
	wstring				strSoundTag = { iter->second[iRandomIndex] };
	_float				fRandomVolume = { m_pGameInstance->GetRandom_Real(ZOMBIE_MIN_VOLUME_HOLD, ZOMBIE_MAX_VOLUME_HOLD) };

	Change_Sound(strSoundTag, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
	Set_Volume(fRandomVolume, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
}

void CZombie::Play_Random_Bite_Sound()
{
	ZOMBIE_SOUND_TYPE			eSoundType = { ZOMBIE_SOUND_TYPE::_END };

	if (ZOMBIE_BODY_TYPE::_MALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_BITE_MALE;
	else if (ZOMBIE_BODY_TYPE::_FEMALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_BITE_FEMALE;
	else if (ZOMBIE_BODY_TYPE::_MALE_BIG == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_BITE_MALE_BIG;

	unordered_map<ZOMBIE_SOUND_TYPE, vector<wstring>>::iterator			iter = { m_SoundTags.find(eSoundType) };
	if (iter == m_SoundTags.end())
		return;

	_uint				iNumBiteSound = { static_cast<_uint>(iter->second.size()) };
	if (iNumBiteSound == 0)
		return;

	_int				iRandomIndex = { m_pGameInstance->GetRandom_Int(0, static_cast<_int>(iNumBiteSound) - 1) };
	wstring				strSoundTag = { iter->second[iRandomIndex] };
	_float				fRandomVolume = { m_pGameInstance->GetRandom_Real(ZOMBIE_MIN_VOLUME_BITE, ZOMBIE_MAX_VOLUME_BITE) };

	Change_Sound(strSoundTag, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
	Set_Volume(fRandomVolume, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
}

void CZombie::Play_Random_Bite_Reject_Sound()
{
	ZOMBIE_SOUND_TYPE			eSoundType = { ZOMBIE_SOUND_TYPE::_END };

	if (ZOMBIE_BODY_TYPE::_MALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_BITE_REJECT_MALE;
	else if (ZOMBIE_BODY_TYPE::_FEMALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_BITE_REJECT_FEMALE;
	else if (ZOMBIE_BODY_TYPE::_MALE_BIG == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_BITE_REJECT_MALE_BIG;

	unordered_map<ZOMBIE_SOUND_TYPE, vector<wstring>>::iterator			iter = { m_SoundTags.find(eSoundType) };
	if (iter == m_SoundTags.end())
		return;

	_uint				iNumBiteRejectSound = { static_cast<_uint>(iter->second.size()) };
	if (iNumBiteRejectSound == 0)
		return;

	_int				iRandomIndex = { m_pGameInstance->GetRandom_Int(0, static_cast<_int>(iNumBiteRejectSound) - 1) };
	wstring				strSoundTag = { iter->second[iRandomIndex] };
	_float				fRandomVolume = { m_pGameInstance->GetRandom_Real(ZOMBIE_MIN_VOLUME_BITE_REJECT, ZOMBIE_MAX_VOLUME_BITE_REJECT) };

	Change_Sound(strSoundTag, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
	Set_Volume(fRandomVolume, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
}

void CZombie::Play_Random_Bite_Kill_Sound()
{
	ZOMBIE_SOUND_TYPE			eSoundType = { ZOMBIE_SOUND_TYPE::_END };

	if (ZOMBIE_BODY_TYPE::_MALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_BITE_KILL_MALE;
	else if (ZOMBIE_BODY_TYPE::_FEMALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_BITE_KILL_FEMALE;
	else if (ZOMBIE_BODY_TYPE::_MALE_BIG == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_BITE_KILL_MALE_BIG;

	unordered_map<ZOMBIE_SOUND_TYPE, vector<wstring>>::iterator			iter = { m_SoundTags.find(eSoundType) };
	if (iter == m_SoundTags.end())
		return;

	_uint				iNumBiteKillSound = { static_cast<_uint>(iter->second.size()) };
	if (iNumBiteKillSound == 0)
		return;

	_int				iRandomIndex = { m_pGameInstance->GetRandom_Int(0, static_cast<_int>(iNumBiteKillSound) - 1) };
	wstring				strSoundTag = { iter->second[iRandomIndex] };
	_float				fRandomVolume = { m_pGameInstance->GetRandom_Real(ZOMBIE_MIN_VOLUME_BITE_KILL, ZOMBIE_MAX_VOLUME_BITE_KILL) };

	Change_Sound(strSoundTag, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
	Set_Volume(fRandomVolume, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
}

void CZombie::Play_Random_Bite_Creep_Sound()
{
	ZOMBIE_SOUND_TYPE			eSoundType = { ZOMBIE_SOUND_TYPE::_END };

	if (ZOMBIE_BODY_TYPE::_MALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_BITE_CREEP_MALE;
	else if (ZOMBIE_BODY_TYPE::_FEMALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_BITE_CREEP_FEMALE;
	else if (ZOMBIE_BODY_TYPE::_MALE_BIG == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_BITE_CREEP_MALE_BIG;

	unordered_map<ZOMBIE_SOUND_TYPE, vector<wstring>>::iterator			iter = { m_SoundTags.find(eSoundType) };
	if (iter == m_SoundTags.end())
		return;

	_uint				iNumBiteCreepSound = { static_cast<_uint>(iter->second.size()) };
	if (iNumBiteCreepSound == 0)
		return;

	_int				iRandomIndex = { m_pGameInstance->GetRandom_Int(0, static_cast<_int>(iNumBiteCreepSound) - 1) };
	wstring				strSoundTag = { iter->second[iRandomIndex] };
	_float				fRandomVolume = { m_pGameInstance->GetRandom_Real(ZOMBIE_MIN_VOLUME_BITE_CREEP, ZOMBIE_MAX_VOLUME_BITE_CREEP) };

	Change_Sound(strSoundTag, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
	Set_Volume(fRandomVolume, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
}

void CZombie::Play_Random_Bite_Creep_Reject_Sound()
{
	ZOMBIE_SOUND_TYPE			eSoundType = { ZOMBIE_SOUND_TYPE::_END };

	if (ZOMBIE_BODY_TYPE::_MALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_BITE_CREEP_REJECT_MALE;
	else if (ZOMBIE_BODY_TYPE::_FEMALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_BITE_CREEP_REJECT_FEMALE;
	else if (ZOMBIE_BODY_TYPE::_MALE_BIG == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_BITE_CREEP_REJECT_MALE_BIG;

	unordered_map<ZOMBIE_SOUND_TYPE, vector<wstring>>::iterator			iter = { m_SoundTags.find(eSoundType) };
	if (iter == m_SoundTags.end())
		return;

	_uint				iNumBiteCreepRejectSound = { static_cast<_uint>(iter->second.size()) };
	if (iNumBiteCreepRejectSound == 0)
		return;

	_int				iRandomIndex = { m_pGameInstance->GetRandom_Int(0, static_cast<_int>(iNumBiteCreepRejectSound) - 1) };
	wstring				strSoundTag = { iter->second[iRandomIndex] };
	_float				fRandomVolume = { m_pGameInstance->GetRandom_Real(ZOMBIE_MIN_VOLUME_BITE_CREEP_REJECT, ZOMBIE_MAX_VOLUME_BITE_CREEP_REJECT) };

	Change_Sound(strSoundTag, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
	Set_Volume(fRandomVolume, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
}

void CZombie::Play_Random_Bite_Creep_Kill_Sound()
{
	ZOMBIE_SOUND_TYPE			eSoundType = { ZOMBIE_SOUND_TYPE::_END };

	if (ZOMBIE_BODY_TYPE::_MALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_BITE_CREEP_KILL_MALE;
	else if (ZOMBIE_BODY_TYPE::_FEMALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_BITE_CREEP_KILL_FEMALE;
	else if (ZOMBIE_BODY_TYPE::_MALE_BIG == static_cast<ZOMBIE_BODY_TYPE>(m_iBody_ID))
		eSoundType = ZOMBIE_SOUND_TYPE::_BITE_CREEP_KILL_MALE_BIG;

	unordered_map<ZOMBIE_SOUND_TYPE, vector<wstring>>::iterator			iter = { m_SoundTags.find(eSoundType) };
	if (iter == m_SoundTags.end())
		return;

	_uint				iNumBiteCreepKillSound = { static_cast<_uint>(iter->second.size()) };
	if (iNumBiteCreepKillSound == 0)
		return;

	_int				iRandomIndex = { m_pGameInstance->GetRandom_Int(0, static_cast<_int>(iNumBiteCreepKillSound) - 1) };
	wstring				strSoundTag = { iter->second[iRandomIndex] };
	_float				fRandomVolume = { m_pGameInstance->GetRandom_Real(ZOMBIE_MIN_VOLUME_BITE_CREEP_KILL, ZOMBIE_MAX_VOLUME_BITE_CREEP_KILL) };

	Change_Sound(strSoundTag, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
	Set_Volume(fRandomVolume, static_cast<_uint>(ZOMBIE_SOUND_CH::_VOICE));
}

void CZombie::Play_Random_Foot_Sound()
{
	unordered_map<ZOMBIE_SOUND_TYPE, vector<wstring>>::iterator			iter = { m_SoundTags.find(ZOMBIE_SOUND_TYPE::_FOOT) };
	if (iter == m_SoundTags.end())
		return;

	_uint				iNumFootSound = { static_cast<_uint>(iter->second.size()) };
	if (iNumFootSound == 0)
		return;

	_int				iRandomIndex = { m_pGameInstance->GetRandom_Int(0, static_cast<_int>(iNumFootSound) - 1) };
	wstring				strSoundTag = { iter->second[iRandomIndex] };
	_float				fRandomVolume = { m_pGameInstance->GetRandom_Real(ZOMBIE_MIN_VOLUME_FOOT, ZOMBIE_MAX_VOLUME_FOOT) };

	Change_Sound(strSoundTag, static_cast<_uint>(ZOMBIE_SOUND_CH::_FOOT));
	Set_Volume(fRandomVolume, static_cast<_uint>(ZOMBIE_SOUND_CH::_FOOT));
}

void CZombie::Play_Random_Foot_Creep_Sound()
{
	unordered_map<ZOMBIE_SOUND_TYPE, vector<wstring>>::iterator			iter = { m_SoundTags.find(ZOMBIE_SOUND_TYPE::_FOOT_CREEP) };
	if (iter == m_SoundTags.end())
		return;

	_uint				iNumFootSound = { static_cast<_uint>(iter->second.size()) };
	if (iNumFootSound == 0)
		return;

	_int				iRandomIndex = { m_pGameInstance->GetRandom_Int(0, static_cast<_int>(iNumFootSound) - 1) };
	wstring				strSoundTag = { iter->second[iRandomIndex] };
	_float				fRandomVolume = { m_pGameInstance->GetRandom_Real(ZOMBIE_MIN_VOLUME_FOOT, ZOMBIE_MAX_VOLUME_FOOT) };

	Change_Sound(strSoundTag, static_cast<_uint>(ZOMBIE_SOUND_CH::_FOOT));
	Set_Volume(fRandomVolume, static_cast<_uint>(ZOMBIE_SOUND_CH::_FOOT));
}

void CZombie::Play_Random_Drop_Body_Sound()
{
	unordered_map<ZOMBIE_SOUND_TYPE, vector<wstring>>::iterator			iter = { m_SoundTags.find(ZOMBIE_SOUND_TYPE::_DROP_BODY) };
	if (iter == m_SoundTags.end())
		return;

	_uint				iNumDropBodySound = { static_cast<_uint>(iter->second.size()) };
	if (iNumDropBodySound == 0)
		return;

	_int				iRandomIndex = { m_pGameInstance->GetRandom_Int(0, static_cast<_int>(iNumDropBodySound) - 1) };
	wstring				strSoundTag = { iter->second[iRandomIndex] };
	_float				fRandomVolume = { m_pGameInstance->GetRandom_Real(ZOMBIE_MIN_VOLUME_DROP_BODY, ZOMBIE_MAX_VOLUME_DROP_BODY) };

	Change_Sound(strSoundTag, static_cast<_uint>(ZOMBIE_SOUND_CH::_FOOT));
	Set_Volume(fRandomVolume, static_cast<_uint>(ZOMBIE_SOUND_CH::_FOOT));
}

void CZombie::Play_Random_Broken_Head_Sound()
{
	unordered_map<ZOMBIE_SOUND_TYPE, vector<wstring>>::iterator			iter = { m_SoundTags.find(ZOMBIE_SOUND_TYPE::_BREAK_HEAD) };
	if (iter == m_SoundTags.end())
		return;

	_uint				iNumBreakHeadSound = { static_cast<_uint>(iter->second.size()) };
	if (iNumBreakHeadSound == 0)
		return;

	_int				iRandomIndex = { m_pGameInstance->GetRandom_Int(0, static_cast<_int>(iNumBreakHeadSound) - 1) };
	wstring				strSoundTag = { iter->second[iRandomIndex] };
	_float				fVolume = { ZOMBIE_VOLUME_BREAK_HEAD };

	Change_Sound(strSoundTag, static_cast<_uint>(ZOMBIE_SOUND_CH::_BREAK));
	Set_Volume(fVolume, static_cast<_uint>(ZOMBIE_SOUND_CH::_BREAK));
}

void CZombie::Play_Random_Broken_Part_Sound()
{
	unordered_map<ZOMBIE_SOUND_TYPE, vector<wstring>>::iterator			iter = { m_SoundTags.find(ZOMBIE_SOUND_TYPE::_BREAK_OTHER) };
	if (iter == m_SoundTags.end())
		return;

	_uint				iNumBreakOthersSound = { static_cast<_uint>(iter->second.size()) };
	if (iNumBreakOthersSound == 0)
		return;

	_int				iRandomIndex = { m_pGameInstance->GetRandom_Int(0, static_cast<_int>(iNumBreakOthersSound) - 1) };
	wstring				strSoundTag = { iter->second[iRandomIndex] };
	_float				fVolume = { ZOMBIE_VOLUME_BREAK_OTHER };

	Change_Sound(strSoundTag, static_cast<_uint>(ZOMBIE_SOUND_CH::_BREAK));
	Set_Volume(fVolume, static_cast<_uint>(ZOMBIE_SOUND_CH::_BREAK));
}

void CZombie::Play_Random_Knock_Window_Sound()
{
	unordered_map<ZOMBIE_SOUND_TYPE, vector<wstring>>::iterator			iter = { m_SoundTags.find(ZOMBIE_SOUND_TYPE::_KNOCK_WINDOW) };
	if (iter == m_SoundTags.end())
		return;

	_uint				iNumKnockWindowSound = { static_cast<_uint>(iter->second.size()) };
	if (iNumKnockWindowSound == 0)
		return;

	_int				iRandomIndex = { m_pGameInstance->GetRandom_Int(0, static_cast<_int>(iNumKnockWindowSound) - 1) };
	wstring				strSoundTag = { iter->second[iRandomIndex] };
	_float				fVolume = { ZOMBIE_VOLUME_INTERACT };

	Change_Sound(strSoundTag, static_cast<_uint>(ZOMBIE_SOUND_CH::_INTERACT));
	Set_Volume(fVolume, static_cast<_uint>(ZOMBIE_SOUND_CH::_INTERACT));
}

void CZombie::Play_Random_Break_Window_Sound()
{
	unordered_map<ZOMBIE_SOUND_TYPE, vector<wstring>>::iterator			iter = { m_SoundTags.find(ZOMBIE_SOUND_TYPE::_BREAK_WINDOW) };
	if (iter == m_SoundTags.end())
		return;

	_uint				iNumBreakWindowSound = { static_cast<_uint>(iter->second.size()) };
	if (iNumBreakWindowSound == 0)
		return;

	_int				iRandomIndex = { m_pGameInstance->GetRandom_Int(0, static_cast<_int>(iNumBreakWindowSound) - 1) };
	wstring				strSoundTag = { iter->second[iRandomIndex] };
	_float				fVolume = { ZOMBIE_VOLUME_INTERACT };

	Change_Sound(strSoundTag, static_cast<_uint>(ZOMBIE_SOUND_CH::_INTERACT));
	Set_Volume(fVolume, static_cast<_uint>(ZOMBIE_SOUND_CH::_INTERACT));
}

void CZombie::Play_Random_Knock_Door_Sound()
{
	unordered_map<ZOMBIE_SOUND_TYPE, vector<wstring>>::iterator			iter = { m_SoundTags.find(ZOMBIE_SOUND_TYPE::_KNOCK_DOOR) };
	if (iter == m_SoundTags.end())
		return;

	_uint				iNumKnockDoorSound = { static_cast<_uint>(iter->second.size()) };
	if (iNumKnockDoorSound == 0)
		return;

	_int				iRandomIndex = { m_pGameInstance->GetRandom_Int(0, static_cast<_int>(iNumKnockDoorSound) - 1) };
	wstring				strSoundTag = { iter->second[iRandomIndex] };
	_float				fVolume = { ZOMBIE_VOLUME_INTERACT };

	Change_Sound(strSoundTag, static_cast<_uint>(ZOMBIE_SOUND_CH::_INTERACT));
	Set_Volume(fVolume, static_cast<_uint>(ZOMBIE_SOUND_CH::_INTERACT));
}

void CZombie::Play_Random_Open_Door_Sound()
{
	unordered_map<ZOMBIE_SOUND_TYPE, vector<wstring>>::iterator			iter = { m_SoundTags.find(ZOMBIE_SOUND_TYPE::_OPEN_DOOR) };
	if (iter == m_SoundTags.end())
		return;

	_uint				iNumOpenDoorSound = { static_cast<_uint>(iter->second.size()) };
	if (iNumOpenDoorSound == 0)
		return;

	_int				iRandomIndex = { m_pGameInstance->GetRandom_Int(0, static_cast<_int>(iNumOpenDoorSound) - 1) };
	wstring				strSoundTag = { iter->second[iRandomIndex] };
	_float				fVolume = { ZOMBIE_VOLUME_INTERACT };

	Change_Sound(strSoundTag, static_cast<_uint>(ZOMBIE_SOUND_CH::_INTERACT));
	Set_Volume(fVolume, static_cast<_uint>(ZOMBIE_SOUND_CH::_INTERACT));
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

void CZombie::Change_Sound(const wstring& strSoundTag, _uint iSoundIndex)
{
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, strSoundTag, iSoundIndex);
}

void CZombie::Set_Volume(_float fVolume, _uint iSoundIndex)
{
	m_pGameInstance->Set_Volume_3D(m_pTransformCom, iSoundIndex, fVolume);
}

void CZombie::Stop_Sound(_uint iSoundIndex)
{
	m_pGameInstance->Stop_Sound_3D(m_pTransformCom, iSoundIndex);
}

void CZombie::SetRagdoll_StartPose()
{
	m_RagdollWakeUpTime = GetTickCount64();

	m_pController->SetReleased(true);
	m_pController->SetDead(true);
	m_pController->Set_Force(_float4(0.f, 0.f, 0.f, 0.f), COLLIDER_TYPE::CHEST);

	m_bRagdoll = true;

	for (auto& pPartObject : m_PartObjects)
	{
		if (nullptr != pPartObject)
			pPartObject->SetRagdoll(m_iIndex_CCT, _float4(0.f, 0.f, 0.f, 0.f), COLLIDER_TYPE::CHEST);
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
	m_bDecal_Hit = false;

	auto vRayOrigin = m_pGameInstance->Get_RayOrigin_Aim();
	auto vRayDir = m_pGameInstance->Get_RayDir_Aim();
	_float	fMinDist = 0.f;
	_float	fMaxDist = 0.f;

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
	m_vecBlood.clear();
	m_vecBlood_Drop.clear();
	m_vecHit.clear();

	for (size_t i = 0; i < BLOOD_COUNT; ++i)
	{
		auto pBlood = CBlood::Create(m_pDevice, m_pContext);
		pBlood->SetSize(NORMAL_ATTACK_BLOOD_SIZE, NORMAL_ATTACK_BLOOD_SIZE, NORMAL_ATTACK_BLOOD_SIZE);
		m_vecBlood.push_back(pBlood);
		pBlood->Start();
	}

	for (size_t i = 0; i < BLOOD_DROP_COUNT; ++i)
	{
		auto pBlood_Drop = CBlood_Drop::Create(m_pDevice, m_pContext);
		pBlood_Drop->SetSize(NORMAL_ATTACK_BLOOD_SIZE_DROP, NORMAL_ATTACK_BLOOD_SIZE_DROP, NORMAL_ATTACK_BLOOD_SIZE_DROP);
		m_vecBlood_Drop.push_back(pBlood_Drop);
		pBlood_Drop->Start();
	}

	m_pImpact = CImpact::Create(m_pDevice, m_pContext);
	m_pImpact->SetSize(1.f, 1.f);

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

	for (size_t i = 0; i < m_vecDecal_SSD.size(); ++i)
	{
		Safe_Release(m_vecDecal_SSD[i]);
	}

	for (size_t i = 0; i < m_vecBlood_Drop.size(); ++i)
	{
		Safe_Release(m_vecBlood_Drop[i]);
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

	for (size_t i = 0; i < m_vecDecal_SSD.size(); ++i)
	{
		m_vecDecal_SSD[i]->Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecBlood_Drop.size(); ++i)
	{
		m_vecBlood_Drop[i]->Tick(fTimeDelta);

		if (m_vecBlood_Drop[i]->GetbDecalSound())
		{
			PlayBloodSound(static_cast<_uint>(i));
		}
	}

	for (size_t i = 0; i < m_vecHit.size(); ++i)
	{
		m_vecHit[i]->Tick(fTimeDelta);
	}

	if (m_pImpact)
		m_pImpact->Tick(fTimeDelta);
}

void CZombie::Late_Tick_Effect(_float fTimeDelta)
{
	for (size_t i = 0; i < m_vecBlood.size(); ++i)
	{
		m_vecBlood[i]->Late_Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecDecal_SSD.size(); ++i)
	{
		m_vecDecal_SSD[i]->Late_Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecBlood_Drop.size(); ++i)
	{
		m_vecBlood_Drop[i]->Late_Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecHit.size(); ++i)
	{
		m_vecHit[i]->Late_Tick(fTimeDelta);
	}

	if (m_pImpact)
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
		m_vecBlood[m_iBloodCount]->SetBiteBlood(false);
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

				m_vecBlood_Drop[m_iBloodCount]->SetWorldMatrix_With_HitNormal(m_vHitNormal);
				m_vecBlood_Drop[m_iBloodCount]->Set_Render(true);
				m_vecBlood_Drop[m_iBloodCount]->SetPosition(m_vHitPosition);
				m_vecBlood_Drop[m_iBloodCount]->SetType(m_pGameInstance->GetRandom_Int(1, 7));
				m_vecBlood_Drop[m_iBloodCount]->SetSize(BIG_ATTACK_BLOOD_SIZE_DROP, BIG_ATTACK_BLOOD_SIZE_DROP, BIG_ATTACK_BLOOD_SIZE_DROP);

			}
			else
			{
				m_vecBlood[m_iBloodCount]->SetSize(NORMAL_ATTACK_BLOOD_SIZE, NORMAL_ATTACK_BLOOD_SIZE, NORMAL_ATTACK_BLOOD_SIZE);

				m_vecBlood_Drop[m_iBloodCount]->SetWorldMatrix_With_HitNormal(m_vHitNormal);
				m_vecBlood_Drop[m_iBloodCount]->Set_Render(true);
				m_vecBlood_Drop[m_iBloodCount]->SetPosition(m_vHitPosition);
				m_vecBlood_Drop[m_iBloodCount]->SetType(m_pGameInstance->GetRandom_Int(0, 7));
				m_vecBlood_Drop[m_iBloodCount]->SetSize(NORMAL_ATTACK_BLOOD_SIZE_DROP, NORMAL_ATTACK_BLOOD_SIZE_DROP, NORMAL_ATTACK_BLOOD_SIZE_DROP);
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
					m_vecBlood_Drop[m_iBloodCount]->SetWorldMatrix_With_HitNormal(m_vHitNormal);
					m_vecBlood_Drop[m_iBloodCount]->Set_Render(true);
					m_vecBlood_Drop[m_iBloodCount]->SetPosition(m_vecBlood[m_iBloodCount - 1]->GetPosition());
					m_vecBlood_Drop[m_iBloodCount]->SetType(m_pGameInstance->GetRandom_Int(1, 7));
					m_vecBlood_Drop[m_iBloodCount]->SetSize(BIG_ATTACK_BLOOD_SIZE_DROP, BIG_ATTACK_BLOOD_SIZE_DROP, BIG_ATTACK_BLOOD_SIZE_DROP);
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
					m_vecBlood_Drop[m_iBloodCount]->SetSize(NORMAL_ATTACK_BLOOD_SIZE_DROP, NORMAL_ATTACK_BLOOD_SIZE_DROP, NORMAL_ATTACK_BLOOD_SIZE_DROP);
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

void CZombie::SetBiteBlood()
{
	if (m_bBiteBlood == false)
	{
		return;
	}

	XMStoreFloat4(&m_vMouthPos, (XMLoadFloat4x4(m_pHeadModel->Get_CombinedMatrix("mouth07")) * m_pTransformCom->Get_WorldMatrix()).r[3]);
	XMStoreFloat4(&m_vMouthLook, (XMLoadFloat4x4(m_pHeadModel->Get_CombinedMatrix("mouth07")) * m_pTransformCom->Get_WorldMatrix()).r[2]);
	m_vMouthLook = Float4_Normalize(m_vMouthLook);

	if (m_iBloodCount >= m_vecBlood.size())
	{
		m_bBiteBlood = false;
		m_iBloodCount = 0;
		return;
	}

	if (m_BloodDelay + m_BloodTime < GetTickCount64())
	{
		m_BloodTime = GetTickCount64();
		m_vecBlood[m_iBloodCount]->Set_Render(true);
		m_vecBlood[m_iBloodCount]->SetWorldMatrix_With_HitNormal(m_vMouthLook);
		m_vecBlood[m_iBloodCount]->SetBiteBlood(true);
		m_vecBlood[m_iBloodCount]->SetPosition(m_vMouthPos);

		if (m_iBloodCount == 0)
		{
			m_iBloodType = m_pGameInstance->GetRandom_Int(0, 10);

			m_vecBlood[m_iBloodCount]->SetType(m_iBloodType);

			if (m_iBloodType >= 10)
			{
				m_iBloodType = 0;
			}

			m_vecBlood[m_iBloodCount]->SetSize(NORMAL_ATTACK_BLOOD_SIZE, NORMAL_ATTACK_BLOOD_SIZE, NORMAL_ATTACK_BLOOD_SIZE);

			m_vecBlood_Drop[m_iBloodCount]->SetWorldMatrix_With_HitNormal(m_vMouthLook);
			m_vecBlood_Drop[m_iBloodCount]->Set_Render(true);
			m_vecBlood_Drop[m_iBloodCount]->SetPosition(m_vMouthPos);
			m_vecBlood_Drop[m_iBloodCount]->SetType(m_pGameInstance->GetRandom_Int(0, 7));
			m_vecBlood_Drop[m_iBloodCount]->SetSize(NORMAL_ATTACK_BLOOD_SIZE_DROP, NORMAL_ATTACK_BLOOD_SIZE_DROP, NORMAL_ATTACK_BLOOD_SIZE_DROP);
		}
		else
		{
			m_vecBlood[m_iBloodCount]->SetType(++m_iBloodType);

			if (m_iBloodType >= 9)
			{
				m_iBloodType = 0;
			}

			m_vecBlood[m_iBloodCount]->SetSize(NORMAL_ATTACK_BLOOD_SIZE, NORMAL_ATTACK_BLOOD_SIZE, NORMAL_ATTACK_BLOOD_SIZE);

			if (m_iBloodCount < BLOOD_DROP_COUNT)
			{
				m_vecBlood_Drop[m_iBloodCount]->SetWorldMatrix_With_HitNormal(m_vMouthLook);
				m_vecBlood_Drop[m_iBloodCount]->Set_Render(true);
				m_vecBlood_Drop[m_iBloodCount]->SetPosition(m_vMouthPos);
				m_vecBlood_Drop[m_iBloodCount]->SetType(m_pGameInstance->GetRandom_Int(0, 7));
				m_vecBlood_Drop[m_iBloodCount]->SetSize(NORMAL_ATTACK_BLOOD_SIZE_DROP, NORMAL_ATTACK_BLOOD_SIZE_DROP, NORMAL_ATTACK_BLOOD_SIZE_DROP);
			}
		}

		++m_iBloodCount;

		if (m_iBloodCount >= m_vecBlood.size())
		{
			m_bBiteBlood = false;
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
				m_vecBlood[i]->SetHitPart((*pHitParts)[i]);
			}
			else
			{
				m_vecBlood[i]->SetHitPart(pBody->Get_Ragdoll_RigidBody((*pColliderTypes)[i]));
			}

			m_vecBlood[i]->Set_Render(true);
			m_vecBlood[i]->SetPosition(vBlockPoint);
			auto iType = m_pGameInstance->GetRandom_Int(1, 10);
			m_vecBlood[i]->SetType(iType);
			m_vecBlood[i]->SetSize(BIG_ATTACK_BLOOD_SIZE_DROP, BIG_ATTACK_BLOOD_SIZE_DROP, BIG_ATTACK_BLOOD_SIZE_DROP);

			m_vecBlood_Drop[i]->Set_Render(true);
			m_vecBlood_Drop[i]->SetPosition(vBlockPoint);
			m_vecBlood_Drop[i]->SetType(m_pGameInstance->GetRandom_Int(1, 7));
			m_vecBlood_Drop[i]->SetSize(BIG_ATTACK_BLOOD_SIZE_DROP, BIG_ATTACK_BLOOD_SIZE_DROP, BIG_ATTACK_BLOOD_SIZE_DROP);

			m_vecHit[i]->Set_Render(true);
			m_vecHit[i]->SetPosition(vBlockPoint);
		}

		pHitPoints->clear();
		pHitNormals->clear();
		pColliderTypes->clear();
		pHitParts->clear();
	}
}

void CZombie::ResetBiteEffect()
{
	m_BloodTime = GetTickCount64();
	m_bBiteBlood = true;
	m_iBloodCount = 0;
}

_float4x4 CZombie::GetDecalWorldMat()
{
	XMStoreFloat4(&m_vMouthPos, (XMLoadFloat4x4(m_pHeadModel->Get_CombinedMatrix("mouth07")) * m_pTransformCom->Get_WorldMatrix()).r[3]);
	XMStoreFloat4(&m_vMouthLook, (XMLoadFloat4x4(m_pHeadModel->Get_CombinedMatrix("mouth07")) * m_pTransformCom->Get_WorldMatrix()).r[2]);
	m_vMouthLook = Float4_Normalize(m_vMouthLook);
	m_vecBlood[m_iBloodCount]->SetWorldMatrix_With_HitNormal(m_vMouthLook);
	m_vecBlood[m_iBloodCount]->SetPosition(m_vMouthPos);

	return	m_vecBlood[m_iBloodCount]->GetWorldMatrix();
}

void CZombie::SetBlood_HeadBlow()
{
	if (m_iBloodCount_HeadBlow >= m_vecBlood.size())
	{
		m_bHeadBlow = false;
		m_iBloodCount_HeadBlow = 0;
		return;
	}

	if (m_HeadBlowEffectTime + m_HeadBlowEffectDelay < GetTickCount64())
	{
		m_HeadBlowEffectTime = GetTickCount64();

		m_vecBlood[m_iBloodCount_HeadBlow]->Set_Render(true);
		m_vecBlood[m_iBloodCount_HeadBlow]->SetWorldMatrix_With_HitNormal(m_vHitNormal);
		m_vecBlood[m_iBloodCount_HeadBlow]->SetHitPart(m_pController->GetHitPart());
		m_vecBlood[m_iBloodCount_HeadBlow]->SetType(m_pGameInstance->GetRandom_Int(1, 7));
		m_vecBlood[m_iBloodCount_HeadBlow]->SetSize(HEADBLOW_BLOOD_SIZE, HEADBLOW_BLOOD_SIZE, HEADBLOW_BLOOD_SIZE);

		auto pBody = static_cast<CBody_Zombie*>(m_PartObjects[CZombie::PART_BODY]);
		_float4 vHeadPos = pBody->GetRigidBodyPos(COLLIDER_TYPE::HEAD);
		_float4 vDelta = _float4(m_pGameInstance->GetRandom_Real(-0.1f, 0.1f),
			m_pGameInstance->GetRandom_Real(-0.1f, 0.1f),
			m_pGameInstance->GetRandom_Real(-0.1f, 0.1f),
			0.f);
		vHeadPos += vDelta;

		m_vecBlood[m_iBloodCount_HeadBlow]->SetPosition(vHeadPos);

		if (m_iBloodCount_HeadBlow < BLOOD_DROP_COUNT)
		{
			m_vecBlood_Drop[m_iBloodCount_HeadBlow]->SetWorldMatrix_With_HitNormal(m_vHitNormal);
			m_vecBlood_Drop[m_iBloodCount_HeadBlow]->Set_Render(true);
			m_vecBlood_Drop[m_iBloodCount_HeadBlow]->SetPosition(m_vecBlood[m_iBloodCount_HeadBlow]->GetPosition());
			m_vecBlood_Drop[m_iBloodCount_HeadBlow]->SetType(m_pGameInstance->GetRandom_Int(1, 7));
			m_vecBlood_Drop[m_iBloodCount_HeadBlow]->SetSize(HEADBLOW_BLOOD_SIZE_DROP, HEADBLOW_BLOOD_SIZE_DROP, HEADBLOW_BLOOD_SIZE_DROP);
		}

		++m_iBloodCount_HeadBlow;

		if (m_iBloodCount_HeadBlow >= m_vecBlood.size())
		{
			m_bHeadBlow = false;
			m_iBloodCount_HeadBlow = 0;
			return;
		}
	}
}

void CZombie::PlayBloodSound(_uint iIndex)
{
	const wchar_t* str = L"Blood_";
	wchar_t result[32];
	_int inum = m_pGameInstance->GetRandom_Int(0, 10);

	std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

	m_pGameInstance->Change_Sound_3D(m_pTransformCom, result, BLOOD_SOUND_STARTINDEX + iIndex);
	m_pGameInstance->Set_Volume_3D(m_pTransformCom, BLOOD_SOUND_STARTINDEX + iIndex, 0.2f);

	m_vecBlood_Drop[iIndex]->SetbDecalSound(false);
}

void CZombie::StartWindowDropSound()
{
	if (m_WindowDropSound_Delay + m_WindowDropSound_Time < GetTickCount64())
	{
		m_bWindowDrop_Sound = false;

		const wchar_t* str = L"Break_Drop_";
		wchar_t result[32];
		_int inum = m_pGameInstance->GetRandom_Int(10, 12);

		std::swprintf(result, sizeof(result) / sizeof(wchar_t), L"%ls%d.mp3", str, inum);

		m_pGameInstance->Change_Sound_3D(m_pTransformCom, result, (_uint)ZOMBIE_SOUND_CH::_BITE_DROP);

		if (inum == 11)
		{
			m_pGameInstance->Set_Volume_3D(m_pTransformCom, (_uint)ZOMBIE_SOUND_CH::_BITE_DROP, 0.2f);
		}
	}
}

void CZombie::Set_ManualMove(_bool isManualMove)
{
	m_isManualMove = isManualMove;

	if (false == m_isManualMove && false == m_bRagdoll)
	{
		_float4				vPosition = { m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION) };
		vPosition.y += CONTROLLER_GROUND_GAP_ZOMBIE;
		m_pController->SetPosition(vPosition);
	}
}

_bool CZombie::IsPlayerNearBy()
{
	auto pPlayer = m_pBlackBoard->Get_Player();
	auto vPlayerPos = pPlayer->GetPositionVector();
	auto vPos = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);

	_vector vDelta = vPlayerPos - vPos;
	if (XMVectorGetX(XMVector3Length(vDelta)) < 2.f)
		return true;
	else
		return false;
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
