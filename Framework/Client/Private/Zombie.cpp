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

#define MODEL_SCALE 0.01f

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
	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Initialize_States()))
		return E_FAIL;

	//if (FAILED(Add_Components()))
	//	return E_FAIL;

	//if (FAILED(Add_PartObjects()))
	//	return E_FAIL;

	//if (FAILED(Initialize_PartModels()))
	//	return E_FAIL;

	if (pArg != nullptr)
	{
		MONSTER_DESC* pDesc = (MONSTER_DESC*)pArg;

		m_iIndex = pDesc->Index;
		_float4 vPos = *(_float4*)pDesc->worldMatrix.m[3];
		//	vPos.y += 1.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}
	m_InteractObjVec.resize(JOMBIE_BEHAVIOR_COLLIDER_END);
	//	m_pModelCom->Set_Animation(rand() % 20, true);
	m_pTransformCom->Set_Scaled(MODEL_SCALE, MODEL_SCALE, MODEL_SCALE);

#pragma region AIController Setup

#pragma region Create Controller

	m_pController = m_pGameInstance->Create_Controller(m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION), &m_iIndex_CCT, this, 1.f, 0.35f, m_pTransformCom
		, m_pBodyModel->GetBoneVector(), "../Bin/Resources/Models/Zombie/Body.fbx");

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
	m_BloodDelay = 70;
	Ready_Effect();
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

				/*For Blood Effect*/
#ifndef _DEBUG
				m_bSetBlood = true;
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
#ifndef _DEBUG
			m_bSetBlood = true;
			m_BloodTime = GetTickCount64();
#endif

			m_pController->Set_Hit(false);


			auto vForce = m_pController->Get_Force();
			auto eType = m_pController->Get_Hit_Collider_Type();


			if (m_pController->GetDead())
			{
				CPlayer::EQUIP		eEquip = m_pBlackBoard->GetPlayer()->Get_Equip();

				m_bRagdoll = true;

				if (CPlayer::EQUIP::STG == eEquip)
				{
					vForce = vForce * 2.f;
				}

				for (auto& pPartObject : m_PartObjects)
				{
					if (nullptr != pPartObject)
						pPartObject->SetRagdoll(m_iIndex_CCT, vForce, eType);
				}
			}

		//	For.Anim
			m_eCurrentHitCollider = eType;
			XMStoreFloat3(&m_vHitDirection, XMLoadFloat4(&vForce));

#pragma region HIT TYPE 분할 임시


			CPlayer::EQUIP		eEquip = m_pBlackBoard->GetPlayer()->Get_Equip();
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
#ifndef _DEBUG
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


		if (FAILED(m_pShaderCom->Begin(0)))
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
	CComposite_Node*							pSequenceNode_Root = { CComposite_Node::Create(&CompositeNodeDesc) };
	pNode_Root->Insert_Child_Node(pSequenceNode_Root);

#pragma region Selector Root 
	//		Selector => 성공을 반환 받을 떄 까지
	//		Sequence => 실패를 반환 받을 떄 까지
	/*
	*Root Selector Section
	*/

	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SELECTOR;
	CComposite_Node*							pSelectorNode_Root = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSequenceNode_Root->Insert_Child_Node(pSelectorNode_Root);

	/*
	* Root CHild_Maintain Section ( Maintain_Check )
	*/


#pragma region TASK PRE TASK

	//	Add Task Node		=> Execute Pre Task
	CExecute_PreTask_Zombie* pTask_ExecutePreTask = { CExecute_PreTask_Zombie::Create() };
	pTask_ExecutePreTask->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Root->Insert_Child_Node(pTask_ExecutePreTask);

	//	Add Decorator		=> Is Maintain?
	CIs_Maintain_PreTask_Zombie* pDeco_Maintain_PreTask = { CIs_Maintain_PreTask_Zombie::Create() };
	pDeco_Maintain_PreTask->SetBlackBoard(m_pBlackBoard);
	pTask_ExecutePreTask->Insert_Decorator_Node(pDeco_Maintain_PreTask);

#pragma endregion

#pragma region BITE

	/*
	*Root Child Section ( Bite )
	*/

	/*CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SELECTOR;
	CComposite_Node*			pSelectorNode_RootChild_Bite = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSelectorNode_Root->Insert_Child_Node(pSelectorNode_RootChild_Bite);*/

	//	Add Task Node		=> Bite 
	CBite_Zombie* pTask_Bite_Zombie = { CBite_Zombie::Create() };
	pTask_Bite_Zombie->SetBlackBoard(m_pBlackBoard);
	//	추후 셀렉터로 변경시 셀렉터 자식으로 하기
	//	pSelectorNode_RootChild_Bite->Insert_Child_Node(pTask_Bite_Zombie);
	pSelectorNode_Root->Insert_Child_Node(pTask_Bite_Zombie);

	//	Add Decorator		=> Is Can Link? ( From Hold )
	list<MONSTER_STATE>					CanLinkMonsterStatesBite;
	CanLinkMonsterStatesBite.emplace_back(MONSTER_STATE::MST_HOLD);
	CanLinkMonsterStatesBite.emplace_back(MONSTER_STATE::MST_LIGHTLY_HOLD);
	CIs_Can_Link_Pre_State_Zombie* pDeco_Is_Can_Link_Bite = { CIs_Can_Link_Pre_State_Zombie::Create(CanLinkMonsterStatesBite) };
	pDeco_Is_Can_Link_Bite->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Root->Insert_Decorator_Node(pDeco_Is_Can_Link_Bite);

#pragma endregion

#pragma region STAND_UP || TRUN_OVER

	//	Add RootNode Child Composite Node - Selector Node			(Is Hit?)
	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SELECTOR;
	CComposite_Node* pSelectorNode_RootChild_StandUp_TurnOver = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSelectorNode_Root->Insert_Child_Node(pSelectorNode_RootChild_StandUp_TurnOver);

	//	Add Task Node		=> Stand Up
	CStand_Up_Zombie*			pTask_StandUp = { CStand_Up_Zombie::Create() };
	pTask_StandUp->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_StandUp_TurnOver->Insert_Child_Node(pTask_StandUp);

	//	Add Task Node		=> Turn Over
	/*CTurn_Over_Zombie*			pTask_TurnOver = { CTurn_Over_Zombie::Create() };
	pTask_TurnOver->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_StandUp_TurnOver->Insert_Child_Node(pTask_TurnOver);*/

#pragma endregion

#pragma region SELECTOR HIT
	/*
	*Root Child Section ( Hit )
	*/

	//	Add RootNode Child Composite Node - Selector Node			(Is Hit?)
	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SELECTOR;
	CComposite_Node* pSelectorNode_RootChild_Hit = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSelectorNode_Root->Insert_Child_Node(pSelectorNode_RootChild_Hit);

	//	Add Task Node		=> Damage Stun
	CStun_Zombie* pTask_Stun = { CStun_Zombie::Create() };
	pTask_Stun->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_Hit->Insert_Child_Node(pTask_Stun);

	//	Add Task Node		=> Damage Knock Back
	CKnock_Back_Zombie* pTask_Knockback = { CKnock_Back_Zombie::Create() };
	pTask_Knockback->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_Hit->Insert_Child_Node(pTask_Knockback);

#pragma endregion

#pragma region LIGHTLY HOLD
	/*
	*Root Child Section ( Lightly Hold )
	*/

	//	거리내로 들어오면 시간을 누적
	//	Add Task Node		=> Lightly Hold 
	CLightly_Hold_Zombie* pTask_Lightly_Hold = { CLightly_Hold_Zombie::Create() };
	pTask_Lightly_Hold->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_Root->Insert_Child_Node(pTask_Lightly_Hold);

	//	Add Decorator Node	=> Lightly Hold
	CIs_Character_In_Range_Zombie* pDeco_Charactor_In_Range_Lightly_Hold = { CIs_Character_In_Range_Zombie::Create() };
	pDeco_Charactor_In_Range_Lightly_Hold->SetBlackBoard(m_pBlackBoard);
	pTask_Lightly_Hold->Insert_Decorator_Node(pDeco_Charactor_In_Range_Lightly_Hold);

#pragma endregion

#pragma region MOVE || TURN || HOLD

	/*
	*Root Child Section ( Select Move Or Turn )
	*/

	//	Add RootNode Child Composite Node - Selector Node			(Is Move Or Turn ?)
	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SELECTOR;
	CComposite_Node* pSelectorNode_RootChild_Move = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSelectorNode_Root->Insert_Child_Node(pSelectorNode_RootChild_Move);

	//	Add Decorator Node
	CIs_Character_In_Range_Zombie* pDeco_Charactor_In_Range_Recognition = { CIs_Character_In_Range_Zombie::Create(m_pStatus->fRecognitionRange) };
	pDeco_Charactor_In_Range_Recognition->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_Move->Insert_Decorator_Node(pDeco_Charactor_In_Range_Recognition);

	//	Add Task Node (Hold)
	CHold_Zombie* pTask_Hold = { CHold_Zombie::Create() };
	pTask_Hold->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_Move->Insert_Child_Node(pTask_Hold);

	CIs_Enough_Time_Zombie* pDeco_Enough_Time_For_Hold = { CIs_Enough_Time_Zombie::Create(&m_pStatus->fAccHoldTime, &m_pStatus->fTryHoldTime) };
	pDeco_Enough_Time_For_Hold->SetBlackBoard(m_pBlackBoard);
	pTask_Hold->Insert_Decorator_Node(pDeco_Enough_Time_For_Hold);

	//	Add Task Node (Move)
	CMove_Front_Zombie* pTask_Move = { CMove_Front_Zombie::Create() };
	pTask_Move->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_Move->Insert_Child_Node(pTask_Move);

	//	Add Decorator Node		=>		Task Move, Deco In View
	CIs_Charactor_In_ViewAngle_Zombie* pDeco_Charactor_In_View = { CIs_Charactor_In_ViewAngle_Zombie::Create() };
	pDeco_Charactor_In_View->SetBlackBoard(m_pBlackBoard);
	pTask_Move->Insert_Decorator_Node(pDeco_Charactor_In_View);

	//	Add Decorator Node		=>		Task Move, Deco Can Change State
	/*CIs_Can_Change_State_Zombie::CAN_CHANGE_STATE_ZOMBIE_DESC		CanChangeStateForMoveDesc;
	CanChangeStateForMoveDesc.NonBlockTypes.emplace_back(ZOMBIE_BODY_ANIM_TYPE::_DAMAGE);
	CanChangeStateForMoveDesc.NonBlockTypes.emplace_back(ZOMBIE_BODY_ANIM_TYPE::_TURN);
	CIs_Can_Change_State_Zombie*				pDeco_Can_Change_StateForMove = { CIs_Can_Change_State_Zombie::Create(&CanChangeStateForMoveDesc) };
	pDeco_Can_Change_StateForMove->SetBlackBoard(m_pBlackBoard);
	pTask_Move->Insert_Decorator_Node(pDeco_Can_Change_StateForMove);*/


	////Add Task Node
	//CPivot_Turn_Zombie* pTask_Pivot_Turn = { CPivot_Turn_Zombie::Create() };
	//pTask_Pivot_Turn->SetBlackBoard(m_pBlackBoard);
	//pSelectorNode_RootChild_Move->Insert_Child_Node(pTask_Pivot_Turn);

	//Add Decorator Node		=>		Task Turn, Deco Can Change State
	/*CIs_Can_Change_State_Zombie::CAN_CHANGE_STATE_ZOMBIE_DESC		CanChangeStateForTurnDesc;
	CanChangeStateForTurnDesc.NonBlockTypes.emplace_back(ZOMBIE_BODY_ANIM_TYPE::_DAMAGE);
	CanChangeStateForTurnDesc.NonBlockTypes.emplace_back(ZOMBIE_BODY_ANIM_TYPE::_TURN);
	CIs_Can_Change_State_Zombie*				pDeco_Can_Change_State_ForTurn = { CIs_Can_Change_State_Zombie::Create(&CanChangeStateForTurnDesc) };
	pDeco_Can_Change_State_ForTurn->SetBlackBoard(m_pBlackBoard);
	pTask_Pivot_Turn->Insert_Decorator_Node(pDeco_Can_Change_State_ForTurn);*/

#pragma endregion

#pragma region IDLE

	/*
	*Root Child Section		( Idle )
	*/

	CompositeNodeDesc.eType = COMPOSITE_NODE_TYPE::CNT_SELECTOR;
	CComposite_Node* pSelectorNode_RootChild_Idle = { CComposite_Node::Create(&CompositeNodeDesc) };
	pSelectorNode_Root->Insert_Child_Node(pSelectorNode_RootChild_Idle);


	//	Add Task Node		( Sleep )
	CSleep_Zombie* pTask_Sleep = { CSleep_Zombie::Create() };
	pTask_Sleep->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_Idle->Insert_Child_Node(pTask_Sleep);


	//	Add Task Node		( Creep )
	CCreep_Zombie* pTask_Creep = { CCreep_Zombie::Create() };
	pTask_Creep->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_Idle->Insert_Child_Node(pTask_Creep);

	//	Add Task Node		( Idle )
	CWait_Zombie* pTask_Wait = { CWait_Zombie::Create() };
	pTask_Wait->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_Idle->Insert_Child_Node(pTask_Wait);

	//Add Decorator Node		=>		Task Wait, Deco Can Change State
	/*CIs_Can_Change_State_Zombie::CAN_CHANGE_STATE_ZOMBIE_DESC		CanChangeStateForWaitDesc;
	CanChangeStateForWaitDesc.NonBlockTypes.emplace_back(ZOMBIE_BODY_ANIM_TYPE::_DAMAGE);
	CanChangeStateForWaitDesc.NonBlockTypes.emplace_back(ZOMBIE_BODY_ANIM_TYPE::_TURN);
	CIs_Can_Change_State_Zombie*				pDeco_Can_Change_State_ForWait = { CIs_Can_Change_State_Zombie::Create(&CanChangeStateForWaitDesc) };
	pDeco_Can_Change_State_ForWait->SetBlackBoard(m_pBlackBoard);
	pTask_Wait->Insert_Decorator_Node(pDeco_Can_Change_State_ForWait);*/

#pragma endregion

#pragma endregion		//	Selector Root 

#pragma region		//	Task Shake Skin

	CShake_Skin_Zombie*							pTask_Shake_Skin = { CShake_Skin_Zombie::Create() };
	pTask_Shake_Skin->SetBlackBoard(m_pBlackBoard);
	pSequenceNode_Root->Insert_Child_Node(pTask_Shake_Skin);

#pragma endregion

#pragma region Additional Shake Skin			//	추가해야함


#pragma endregion		//	Additional Shake Skin

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

	ColliderOBBDesc.vRotation = _float3(0.f, XMConvertToRadians(45.0f), 0.f);
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

	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pRootTranslation = &m_vRootTranslation;

	pBodyObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Body_Zombie"), &BodyDesc));
	if (nullptr == pBodyObject)
		return E_FAIL;

	m_PartObjects[CZombie::PART_ID::PART_BODY] = pBodyObject;


	/* For.Part_Face */
	CPartObject* pFaceObject = { nullptr };
	CFace_Zombie::FACE_MONSTER_DESC			FaceDesc;

	FaceDesc.pParentsTransform = m_pTransformCom;
	FaceDesc.eType = CFace_Zombie::TYPE_FACE1;

	pFaceObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Face_Zombie"), &FaceDesc));
	if (nullptr == pFaceObject)
		return E_FAIL;

	m_PartObjects[CZombie::PART_ID::PART_FACE] = pFaceObject;


	CPartObject* pFace2Object = { nullptr };
	CFace_Zombie::FACE_MONSTER_DESC			Face2Desc;

	Face2Desc.pParentsTransform = m_pTransformCom;
	Face2Desc.eType = CFace_Zombie::TYPE_FACE2;

	pFace2Object = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Face_Zombie"), &Face2Desc));
	if (nullptr == pFace2Object)
		return E_FAIL;

	m_PartObjects[CZombie::PART_ID::PART_FACE2] = pFace2Object;

	CPartObject* pFace3Object = { nullptr };
	CFace_Zombie::FACE_MONSTER_DESC			Face3Desc;

	Face3Desc.pParentsTransform = m_pTransformCom;
	Face3Desc.eType = CFace_Zombie::TYPE_FACE3;

	pFace3Object = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Face_Zombie"), &Face3Desc));
	if (nullptr == pFace3Object)
		return E_FAIL;

	m_PartObjects[CZombie::PART_ID::PART_FACE3] = pFace3Object;


	/* For.Part_Hat */
	CPartObject* pHatObject = { nullptr };
	CClothes_Zombie::CLOTHES_MONSTER_DESC		ClothesHatDesc;

	ClothesHatDesc.pParentsTransform = m_pTransformCom;
	ClothesHatDesc.eType = CClothes_Zombie::CLOTHES_TYPE::TYPE_HAT;

	pHatObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Clothes_Zombie"), &ClothesHatDesc));
	if (nullptr == pHatObject)
		return E_FAIL;

	m_PartObjects[CZombie::PART_ID::PART_HAT] = pHatObject;

	/* For.Part_Shirts */
	CPartObject* pShirtsObject = { nullptr };
	CClothes_Zombie::CLOTHES_MONSTER_DESC		ClothesShirtsDesc;

	ClothesShirtsDesc.pParentsTransform = m_pTransformCom;
	ClothesShirtsDesc.eType = CClothes_Zombie::CLOTHES_TYPE::TYPE_SHIRTS;

	pShirtsObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Clothes_Zombie"), &ClothesShirtsDesc));
	if (nullptr == pShirtsObject)
		return E_FAIL;

	m_PartObjects[CZombie::PART_ID::PART_SHIRTS] = pShirtsObject;

	/* For.Part_Shirts2 */
	CPartObject* pShirtsObject2 = { nullptr };
	CClothes_Zombie::CLOTHES_MONSTER_DESC		ClothesShirts2Desc;

	ClothesShirts2Desc.pParentsTransform = m_pTransformCom;
	ClothesShirts2Desc.eType = CClothes_Zombie::CLOTHES_TYPE::TYPE_SHIRTS2;

	pShirtsObject2 = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Clothes_Zombie"), &ClothesShirts2Desc));
	if (nullptr == pShirtsObject2)
		return E_FAIL;

	m_PartObjects[CZombie::PART_ID::PART_SHIRTS2] = pShirtsObject2;

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

	ClothesPantsDesc.pParentsTransform = m_pTransformCom;
	ClothesPantsDesc.eType = CClothes_Zombie::CLOTHES_TYPE::TYPE_PANTS;

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

	m_pStatus->fRecognitionRange = STATUS_ZOMBIE_DEFAULT_RECOGNITION_DISTANCE;
	m_pStatus->fMaxRecognitionTime = STATUS_ZOMBIE_MAX_RECOGNITION_TIME;
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

HRESULT CZombie::Initialize_PartModels()
{
	CModel* pBodyModel = { dynamic_cast<CModel*>(m_PartObjects[PART_BODY]->Get_Component(TEXT("Com_Model"))) };
	CModel* pFaceModel = { dynamic_cast<CModel*>(m_PartObjects[PART_FACE]->Get_Component(TEXT("Com_Model"))) };
	CModel* pFace2Model = { dynamic_cast<CModel*>(m_PartObjects[PART_FACE2]->Get_Component(TEXT("Com_Model"))) };
	CModel* pFace3Model = { dynamic_cast<CModel*>(m_PartObjects[PART_FACE3]->Get_Component(TEXT("Com_Model"))) };
	CModel* pShirtsModel = { dynamic_cast<CModel*>(m_PartObjects[PART_SHIRTS]->Get_Component(TEXT("Com_Model"))) };
	CModel* pShirts2Model = { dynamic_cast<CModel*>(m_PartObjects[PART_SHIRTS2]->Get_Component(TEXT("Com_Model"))) };
	CModel* pPantsModel = { dynamic_cast<CModel*>(m_PartObjects[PART_PANTS]->Get_Component(TEXT("Com_Model"))) };
	CModel* pHatModel = { dynamic_cast<CModel*>(m_PartObjects[PART_HAT]->Get_Component(TEXT("Com_Model"))) };

	m_pBodyModel = pBodyModel;
	m_pHeadModel = pFaceModel;
	m_pHeadModel2 = pFace2Model;
	m_pHeadModel3 = pFace3Model;
	m_pShirtsModel = pShirtsModel;
	m_pShirts2Model = pShirts2Model;
	m_pPantsModel = pPantsModel;
	m_pHatModel = pHatModel;

	if (nullptr == pBodyModel ||
		nullptr == pFaceModel ||
		nullptr == pFace2Model ||
		nullptr == pFace3Model ||
		nullptr == pShirtsModel ||
		nullptr == pShirts2Model ||
		nullptr == pPantsModel ||
		nullptr == pHatModel)
		return E_FAIL;

	if (FAILED(pFaceModel->Link_Bone_Auto(pBodyModel)))
		return E_FAIL;
	if (FAILED(pFace2Model->Link_Bone_Auto(pBodyModel)))
		return E_FAIL;
	if (FAILED(pFace3Model->Link_Bone_Auto(pBodyModel)))
		return E_FAIL;
	if (FAILED(pShirtsModel->Link_Bone_Auto(pBodyModel)))
		return E_FAIL;
	if (FAILED(pShirts2Model->Link_Bone_Auto(pBodyModel)))
		return E_FAIL;
	if (FAILED(pPantsModel->Link_Bone_Auto(pBodyModel)))
		return E_FAIL;
	if (FAILED(pHatModel->Link_Bone_Auto(pBodyModel)))
		return E_FAIL;

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
			m_eBeHavior_Col = (JOMBIE_BEHAVIOR_COLLIDER_TYPE) pColCom->Get_Region(); // 행동enum을 가지고 있음 JOMBIE_BEHAVIOR_COLLIDER_TYPE
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

void CZombie::Perform_Skinning()
{
	//Body Model
	{
		list<_uint> NonHideIndex = m_pBodyModel->Get_NonHideMeshIndices();

		for (auto& i : NonHideIndex)
		{
			m_pBodyModel->Bind_Essential_Resource_Skinning(m_pTransformCom->Get_WorldFloat4x4_Ptr());
			m_pBodyModel->Bind_Resource_Skinning(i);
			m_pGameInstance->Perform_Skinning((*m_pBodyModel->GetMeshes())[i]->GetNumVertices());
			//m_pBodyModel->Staging_Skinning(i);
		}
	}

	//Face Model
	{
		list<_uint> NonHideIndex = m_pHeadModel->Get_NonHideMeshIndices();

		for (auto& i : NonHideIndex)
		{
			m_pHeadModel->Bind_Essential_Resource_Skinning(m_pTransformCom->Get_WorldFloat4x4_Ptr());
			m_pHeadModel->Bind_Resource_Skinning(i);
			m_pGameInstance->Perform_Skinning((*m_pHeadModel->GetMeshes())[i]->GetNumVertices());
			//m_pHeadModel->Staging_Skinning(i);
		}
	}

	//Head2
	{
		list<_uint> NonHideIndex = m_pHeadModel2->Get_NonHideMeshIndices();

		for (auto& i : NonHideIndex)
		{
			m_pHeadModel2->Bind_Essential_Resource_Skinning(m_pTransformCom->Get_WorldFloat4x4_Ptr());
			m_pHeadModel2->Bind_Resource_Skinning(i);
			m_pGameInstance->Perform_Skinning((*m_pHeadModel2->GetMeshes())[i]->GetNumVertices());
			//m_pHeadModel2->Staging_Skinning(i);
		}
	}

	//Head3
	{
		list<_uint> NonHideIndex = m_pHeadModel3->Get_NonHideMeshIndices();

		for (auto& i : NonHideIndex)
		{
			m_pHeadModel3->Bind_Essential_Resource_Skinning(m_pTransformCom->Get_WorldFloat4x4_Ptr());
			m_pHeadModel3->Bind_Resource_Skinning(i);
			m_pGameInstance->Perform_Skinning((*m_pHeadModel3->GetMeshes())[i]->GetNumVertices());
			//m_pHeadModel3->Staging_Skinning(i);
		}
	}

	//Shirts Model
	{
		list<_uint> NonHideIndex = m_pShirtsModel->Get_NonHideMeshIndices();

		for (auto& i : NonHideIndex)
		{
			m_pShirtsModel->Bind_Essential_Resource_Skinning(m_pTransformCom->Get_WorldFloat4x4_Ptr());
			m_pShirtsModel->Bind_Resource_Skinning(i);
			m_pGameInstance->Perform_Skinning((*m_pShirtsModel->GetMeshes())[i]->GetNumVertices());
			//m_pShirtsModel->Staging_Skinning(i);
		}
	}

	//Shirts2 Model
	{
		list<_uint> NonHideIndex = m_pShirts2Model->Get_NonHideMeshIndices();

		for (auto& i : NonHideIndex)
		{
			m_pShirts2Model->Bind_Essential_Resource_Skinning(m_pTransformCom->Get_WorldFloat4x4_Ptr());
			m_pShirts2Model->Bind_Resource_Skinning(i);
			m_pGameInstance->Perform_Skinning((*m_pShirts2Model->GetMeshes())[i]->GetNumVertices());
			//m_pShirts2Model->Staging_Skinning(i);
		}
	}

	//Pants Model
	{
		list<_uint> NonHideIndex = m_pPantsModel->Get_NonHideMeshIndices();

		for (auto& i : NonHideIndex)
		{
			m_pPantsModel->Bind_Essential_Resource_Skinning(m_pTransformCom->Get_WorldFloat4x4_Ptr());
			m_pPantsModel->Bind_Resource_Skinning(i);
			m_pGameInstance->Perform_Skinning((*m_pPantsModel->GetMeshes())[i]->GetNumVertices());
			//m_pPantsModel->Staging_Skinning(i);
		}
	}

	//Hat Model
	{
		list<_uint> NonHideIndex = m_pHatModel->Get_NonHideMeshIndices();

		for (auto& i : NonHideIndex)
		{
			m_pHatModel->Bind_Essential_Resource_Skinning(m_pTransformCom->Get_WorldFloat4x4_Ptr());
			m_pHatModel->Bind_Resource_Skinning(i);
			m_pGameInstance->Perform_Skinning((*m_pHatModel->GetMeshes())[i]->GetNumVertices());
			//m_pHatModel->Staging_Skinning(i);
		}
	}
}

void CZombie::Ready_Decal()
{
	auto vRayOrigin = m_pGameInstance->Get_RayOrigin_Aim();
	auto vRayDir = m_pGameInstance->Get_RayDir_Aim();
	_float	fMinDist = 0.f;
	_float	fMaxDist = 0.f;
	if (m_pColliderCom_Bounding->IntersectRayAABB(XMLoadFloat4(&vRayOrigin), XMLoadFloat4(&vRayDir), fMinDist, fMaxDist))
	{
		Perform_Skinning();

		HitResult hitResult;
		hitResult.hitModel = this;
		hitResult.minHitDistance = fMinDist;
		hitResult.maxHitDistance = fMaxDist;

		_matrix rotMatrix = XMMatrixIdentity();
		//rotMatrix.SetRotation(mainCamera->GetDirection(), decalAngle);

		AddDecalInfo decalInfo;
		decalInfo.rayOrigin = vRayOrigin;
		decalInfo.rayDir = vRayDir;
		XMStoreFloat4(&decalInfo.decalTangent, XMVector4Transform(m_pGameInstance->Get_Camera_Transform()->Get_State_Float4(CTransform::STATE_RIGHT), rotMatrix));
		decalInfo.decalSize = _float3(5.f, 5.f, 5.0f);
		decalInfo.minHitDistance = hitResult.minHitDistance;
		decalInfo.maxHitDistance = hitResult.maxHitDistance;
		decalInfo.decalMaterialIndex = 0;

		////Body Model
		//list<_uint> NonHideIndex = m_pBodyModel->Get_NonHideMeshIndices();
		//for (auto& i : NonHideIndex)
		//{
		//	m_iMeshIndex_Hit = m_pBodyModel->Perform_RayCasting(i, decalInfo, &m_fHitDistance);

		//	if (m_iMeshIndex_Hit != 999)
		//	{
		//		m_iMeshIndex_Hit = i;

		//		_vector CameraLook = XMVectorScale(m_pGameInstance->Get_Camera_Transform()->Get_State_Vector(CTransform::STATE_LOOK), m_fHitDistance);
		//		_vector CameraPos = m_pGameInstance->Get_Camera_Pos_Vector() + CameraLook;
		//		XMStoreFloat4(&m_vHitPosition, CameraPos);

		//		m_vHitNormal = m_pController->GetHitNormal();

		//		break;
		//	}
		//}

		//if(m_iMeshIndex_Hit == 999)
		//{
		//	//Head Model
		//	NonHideIndex = m_pHeadModel->Get_NonHideMeshIndices();
		//	for (auto& i : NonHideIndex)
		//	{
		//		m_iMeshIndex_Hit = m_pHeadModel->Perform_RayCasting(i, decalInfo, &m_fHitDistance);

		//		if (m_iMeshIndex_Hit != 999)
		//		{
		//			m_iMeshIndex_Hit = i;

		//			_vector CameraLook = XMVectorScale(m_pGameInstance->Get_Camera_Transform()->Get_State_Vector(CTransform::STATE_LOOK), m_fHitDistance);
		//			_vector CameraPos = m_pGameInstance->Get_Camera_Pos_Vector() + CameraLook;
		//			XMStoreFloat4(&m_vHitPosition, CameraPos);

		//			m_vHitNormal = m_pController->GetHitNormal();

		//			break;
		//		}
		//	}
		//}

		////Shirt Model
		//if (m_iMeshIndex_Hit == 999)
		//{
		//	NonHideIndex = m_pShirtsModel->Get_NonHideMeshIndices();
		//	for (auto& i : NonHideIndex)
		//	{
		//		m_iMeshIndex_Hit = m_pShirtsModel->Perform_RayCasting(i, decalInfo, &m_fHitDistance);

		//		if (m_iMeshIndex_Hit != 999)
		//		{
		//			m_iMeshIndex_Hit = i;

		//			_vector CameraLook = XMVectorScale(m_pGameInstance->Get_Camera_Transform()->Get_State_Vector(CTransform::STATE_LOOK), m_fHitDistance);
		//			_vector CameraPos = m_pGameInstance->Get_Camera_Pos_Vector() + CameraLook;
		//			XMStoreFloat4(&m_vHitPosition, CameraPos);

		//			m_vHitNormal = m_pController->GetHitNormal();

		//			break;
		//		}
		//	}
		//}

		////Pants Model
		//if (m_iMeshIndex_Hit == 999)
		//{
		//	NonHideIndex = m_pPantsModel->Get_NonHideMeshIndices();
		//	for (auto& i : NonHideIndex)
		//	{
		//		m_iMeshIndex_Hit = m_pPantsModel->Perform_RayCasting(i, decalInfo, &m_fHitDistance);

		//		if (m_iMeshIndex_Hit != 999)
		//		{
		//			m_iMeshIndex_Hit = i;

		//			_vector CameraLook = XMVectorScale(m_pGameInstance->Get_Camera_Transform()->Get_State_Vector(CTransform::STATE_LOOK), m_fHitDistance);
		//			_vector CameraPos = m_pGameInstance->Get_Camera_Pos_Vector() + CameraLook;
		//			XMStoreFloat4(&m_vHitPosition, CameraPos);

		//			m_vHitNormal = m_pController->GetHitNormal();

		//			break;
		//		}
		//	}
		//}


		if (m_iMeshIndex_Hit == 999)
		{
			m_vHitPosition = m_pController->GetBlockPoint();
			m_vHitNormal = m_pController->GetHitNormal();
		}

		/*if (iMeshIndex != -1 && iMeshIndex != 999)
		{
			m_pBodyModel->Perform_Calc_DecalInfo(iMeshIndex);
			m_pBodyModel->Bind_Resource_DecalMap(iMeshIndex, m_pShader_Decal);
			m_pBodyModel->Perform_Init_DecalMap(iMeshIndex, m_pShader_Decal);
		}*/
	}

	m_vHitPosition = m_pController->GetBlockPoint();
	m_vHitNormal = m_pController->GetHitNormal();

	if (m_bRagdoll)
	{
		m_vHitPosition = m_pController->GetBlockPoint();
		m_vHitNormal = m_pController->GetHitNormal();
	}
}

void CZombie::Ready_Effect()
{
	for (size_t i = 0; i < 3; ++i)
	{
		auto pBlood = CBlood::Create(m_pDevice, m_pContext);
		pBlood->SetSize(3.f, 3.f, 3.f);
		m_vecBlood.push_back(pBlood);
	}
}

void CZombie::Release_Effect()
{
	for (size_t i = 0; i < m_vecBlood.size(); ++i)
	{
		Safe_Release(m_vecBlood[i]);
	}
}

void CZombie::Tick_Effect(_float fTimeDelta)
{
	for (size_t i = 0; i < m_vecBlood.size(); ++i)
	{
		m_vecBlood[i]->Tick(fTimeDelta);
	}
}

void CZombie::Late_Tick_Effect(_float fTimeDelta)
{
	for (size_t i = 0; i < m_vecBlood.size(); ++i)
	{
		m_vecBlood[i]->Late_Tick(fTimeDelta);
	}
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
		m_vecBlood[m_iBloodCount]->SetPosition(m_vHitPosition);

		if (m_iBloodCount == 0)
		{
			Calc_Decal_Map();

			m_iBloodType = m_pGameInstance->GetRandom_Int(0, 10);

			m_vecBlood[m_iBloodCount]->SetType(m_iBloodType);

			if (m_iBloodType >= 10)
			{
				m_iBloodType = 0;
			}
		}
		else
		{
			if (m_iBloodType == 1)
			{
				++m_iBloodType;
			}

			m_vecBlood[m_iBloodCount]->SetType(++m_iBloodType);

			if (m_iBloodType >= 10)
			{
				m_iBloodType = 0;
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
	m_pBodyModel->SetDecalWorldMatrix(m_iMeshIndex_Hit, m_vecBlood[m_iBloodCount]->GetWorldMatrix());
	m_pBodyModel->Perform_Calc_DecalMap();

	m_pHeadModel->SetDecalWorldMatrix(m_iMeshIndex_Hit, m_vecBlood[m_iBloodCount]->GetWorldMatrix());
	m_pHeadModel->Perform_Calc_DecalMap();

	m_pHeadModel2->SetDecalWorldMatrix(m_iMeshIndex_Hit, m_vecBlood[m_iBloodCount]->GetWorldMatrix());
	m_pHeadModel2->Perform_Calc_DecalMap();
				
	m_pHeadModel3->SetDecalWorldMatrix(m_iMeshIndex_Hit, m_vecBlood[m_iBloodCount]->GetWorldMatrix());
	m_pHeadModel3->Perform_Calc_DecalMap();

	m_pShirtsModel->SetDecalWorldMatrix(m_iMeshIndex_Hit, m_vecBlood[m_iBloodCount]->GetWorldMatrix());
	m_pShirtsModel->Perform_Calc_DecalMap();

	m_pShirts2Model->SetDecalWorldMatrix(m_iMeshIndex_Hit, m_vecBlood[m_iBloodCount]->GetWorldMatrix());
	m_pShirts2Model->Perform_Calc_DecalMap();

	m_pPantsModel->SetDecalWorldMatrix(m_iMeshIndex_Hit, m_vecBlood[m_iBloodCount]->GetWorldMatrix());
	m_pPantsModel->Perform_Calc_DecalMap();

	m_pHatModel->SetDecalWorldMatrix(m_iMeshIndex_Hit, m_vecBlood[m_iBloodCount]->GetWorldMatrix());
	m_pHatModel->Perform_Calc_DecalMap();
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
	Release_Effect();
}
