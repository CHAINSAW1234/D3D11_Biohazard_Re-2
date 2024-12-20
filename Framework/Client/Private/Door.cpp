#include "stdafx.h"
#include "Door.h"
#include "Player.h"
#include "Bone.h"
#include "PxCollider.h"

#include "Body_Door.h"
#include "CustomCollider.h"
#include "Room_Finder.h"

#include "Selector_UI.h"
#include "Emblem_Door.h"
#include "Mark_Door.h"
#include "Key_Door.h"
#include "Camera_Gimmick.h"

CDoor::CDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CDoor::CDoor(const CDoor& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CDoor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDoor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_bLock = m_tagPropDesc.tagDoor.bLock;

	m_tagPropDesc.tagDoor.iLockType; //0=>앰블럼 1=> chain 3=>dial
	/* 0 하트 1 스페이스 2 클로버 3 다이아*/
	m_iEmblemType = m_tagPropDesc.tagDoor.iEmblemType;

	if ((m_tagPropDesc.strGamePrototypeName.find("007") != string::npos) || (m_tagPropDesc.strGamePrototypeName.find("038") != string::npos) || (m_tagPropDesc.strGamePrototypeName.find("113") != string::npos))
		m_eType = DOOR_DOUBLE;

	else if (m_tagPropDesc.strGamePrototypeName.find("056") != string::npos)
	{

		m_eType = DOOR_DUMMY;
	}

	else
	{

		if (m_tagPropDesc.strGamePrototypeName.find("iron") != string::npos)
			m_eDoorTexture = IRON;
		else
			m_eDoorTexture = WOOD;

		m_eType = DOOR_ONE;
	}

	if (m_eType == DOOR_DOUBLE)
	{
		if ((m_tagPropDesc.strGamePrototypeName.find("007") != string::npos))
			m_eDoubleDoorType = DOUBLE_DOOR_MODEL_TYPE::FRONT_DOOR;
		else
			m_eDoubleDoorType = DOUBLE_DOOR_MODEL_TYPE::NORMAL_DOOR;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Initialize_PartObjects()))
		return E_FAIL;
	if (m_tagPropDesc.tagDoor.iLockType != 0)
		m_bLock = false;


	if (FAILED(m_pGameInstance->Add_Object_Sound(m_pTransformCom, 2)))
		return E_FAIL;

	return S_OK;
}

void CDoor::Start()
{
	__super::Start();

	list<CGameObject*>* pCollider = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Jombie_Collider"));
	if (pCollider == nullptr)
		return;
	for (auto& iter : *pCollider)
	{
		if (m_eType == CDoor::DOOR_DUMMY)
			int a = 0;
		if (m_pColliderCom[INTER_COL_NORMAL][COL_STEP0]->Intersect(static_cast<CCollider*>(iter->Get_Component(TEXT("Com_Collider")))))
		{
			// 내 인덱스 넣어주기
			_int* iNum = static_cast<CCustomCollider*>(iter)->Node_InteractProps();
			*iNum = m_tagPropDesc.iIndex;

			//	 ++ 참조
			m_pMyCustomCollider = static_cast<CCustomCollider*>(iter);
			Safe_AddRef(m_pMyCustomCollider);

			break;
		}
	}

#ifdef _DEBUG
	if (nullptr == m_pMyCustomCollider)
	{
		MSG_BOX(TEXT("Called : void CDoor::Start() Custom Collider 못 찾 음"));

		return;
	}
#endif

	if (FAILED(Add_Locations()))
	{
		return;
	}
	if (m_eType == CDoor::DOOR_DUMMY)
	{
		m_iHP = 0;
	}
}

void CDoor::Tick(_float fTimeDelta)
{
	__super::Tick_Col();

	if (m_fDelayLockTime > 0.f)
		m_fDelayLockTime -= fTimeDelta;
	if (m_fDelayLockTime < 0.f)
	{
		m_fDelayLockTime = 0.f;
		m_bAutoOpen = true;
		m_bLock = false;
	}

	if (!m_bVisible)
		return;
	//#ifdef _DEBUG
	//#ifdef UI_POS
	//	Get_Object_Pos();
	//#endif
	//#endif
	if (m_eType == CDoor::DOOR_DUMMY)
	{
		m_iHP = 0;
		return;
	}

	
	m_eType == CDoor::DOOR_ONE ? OneDoor_Tick(fTimeDelta) : DoubleDoor_Tick(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CDoor::Late_Tick(_float fTimeDelta)
{
	if (m_pPlayer == nullptr)
		return;

	if (!Visible())
	{
		Select_UI();

		return;
	}

	if (m_eType == CDoor::DOOR_DUMMY)
		return;

	if (m_bRender == false)
		return;
	else
	{
		for (auto& it : m_PartObjects)
		{
			if (it != nullptr)
				it->Set_Render(true);
		}

		m_bRender = false;
	}
	__super::Late_Tick(fTimeDelta);
	m_eType == CDoor::DOOR_ONE ? OneDoor_Late_Tick(fTimeDelta) : DoubleDoor_Late_Tick(fTimeDelta);


#ifdef _DEBUG
	Add_Col_DebugCom();
#endif
}

HRESULT CDoor::Render()
{


	return S_OK;
}

HRESULT CDoor::Add_Components()
{
	if (m_eType == DOOR_DOUBLE)
	{
		CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};
		//	ColliderDesc.fRadius = _float(180.f);
		ColliderDesc.fRadius = _float(300.f);
		ColliderDesc.vCenter = _float3(-30.f, 1.f, 0.f);

		/* For.Com_Collider */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Normal_Step0"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP0], &ColliderDesc)))
			return E_FAIL;

		ColliderDesc.fRadius = _float(150.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Normal_Step1"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP1], &ColliderDesc)))
			return E_FAIL;

		ColliderDesc.fRadius = _float(120.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Normal_Step2"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP2], &ColliderDesc)))
			return E_FAIL;



		ColliderDesc.fRadius = _float(180.f);
		ColliderDesc.vCenter = _float3(-150.f, 1.f, 0.f);
		/* For.Com_Collider */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Double_Step0"), (CComponent**)&m_pColliderCom[INTER_COL_DOUBLE][COL_STEP0], &ColliderDesc)))
			return E_FAIL;
		ColliderDesc.fRadius = _float(150.f);
		/* For.Com_Collider */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Double_Step1"), (CComponent**)&m_pColliderCom[INTER_COL_DOUBLE][COL_STEP1], &ColliderDesc)))
			return E_FAIL;
		ColliderDesc.fRadius = _float(120.f);
		/* For.Com_Collider */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Double_Step2"), (CComponent**)&m_pColliderCom[INTER_COL_DOUBLE][COL_STEP2], &ColliderDesc)))
			return E_FAIL;
	}
	else
	{
		CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

		ColliderDesc.fRadius = _float(150.f);
		ColliderDesc.vCenter = _float3(-60.f, 1.f, 0.f);
		/* For.Com_Collider */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Step0"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP0], &ColliderDesc)))
			return E_FAIL;

		ColliderDesc.fRadius = _float(110.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Step1"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP1], &ColliderDesc)))
			return E_FAIL;

		ColliderDesc.fRadius = _float(90.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Step2"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP2], &ColliderDesc)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CDoor::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CDoor::PART_END);

	/*Part_Body*/
	CPartObject* pBodyObj = { nullptr };
	CBody_Door::BODY_DOOR_DESC BodyDesc = {};
	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pState = &m_eType;
	BodyDesc.pDoubleDoorType = &m_eDoubleDoorType;
	BodyDesc.pDoubleDoorState = &m_eDoubleState;
	BodyDesc.pDoubleDoorState_Prev = &m_eDoubleState_Prev;
	BodyDesc.pOneDoorState = &m_eOneState;
	BodyDesc.pOneDoorState_Prev = &m_eOneState_Prev;
	BodyDesc.pSoundCueSign = &m_bSoundCueSign;
	BodyDesc.pDoorTexture = &m_eDoorTexture;
	BodyDesc.strModelComponentName = m_tagPropDesc.strModelComponent;

	if (true == m_bLock && m_tagPropDesc.tagDoor.iLockType == 0 &&
		(m_iEmblemType == (_int)CDoor::EMBLEM_TYPE::HEART_EMBLEM ||
			m_iEmblemType == (_int)CDoor::EMBLEM_TYPE::SPADE_EMBLEM ||
			m_iEmblemType == (_int)CDoor::EMBLEM_TYPE::CLOVER_EMBLEM ||
			m_iEmblemType == (_int)CDoor::EMBLEM_TYPE::DIA_EMBLEM))
		BodyDesc.isEmblem = true;

	pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(m_tagPropDesc.strObjectPrototype, &BodyDesc));
	if (nullptr == pBodyObj)
		return E_FAIL;

	m_PartObjects[CDoor::PART_BODY] = pBodyObj;

	/*PART_LOCK*/
	m_PartObjects[CDoor::PART_LOCK] = nullptr;

	/*PART_HANDLE*/

	/* 0 하트 1 스페이스 2 클로버 3 다이아*/
	if (true == BodyDesc.isEmblem && m_bLock)
	{

		/* Emblem*/
		CPartObject* pEmblem = { nullptr };
		CEmblem_Door::BODY_EMBLEM_DOOR EmblemDesc;

		EmblemDesc.pParentsTransform = m_pTransformCom;
		EmblemDesc.pSoundCueSign = &m_bSoundCueSign;
		EmblemDesc.EmblemAnim = &m_eEmblemAnim_Type;
		EmblemDesc.eEmblemType = static_cast<_ubyte>(m_iEmblemType);
		EmblemDesc.pDoorState = &m_eOneState;

		/* Mark*/
		CPartObject* pMark = { nullptr };

		CMark_Door::EMBLEMMARK_DOOR_DESC MarkDesc;

		MarkDesc.pSoundCueSign = &m_bSoundCueSign;
		MarkDesc.pParentsTransform = m_pTransformCom;
		MarkDesc.EmblemAnim = &m_eEmblemAnim_Type;
		MarkDesc.eEmblemType = static_cast<_ubyte>(m_iEmblemType);
		MarkDesc.pDoorState = &m_eOneState;

		/* Key*/
		CPartObject* pKey;

		CKey_Door::KEY_DOOR KeyDesc;
		KeyDesc.pSoundCueSign = &m_bSoundCueSign;
		KeyDesc.pParentsTransform = m_pTransformCom;
		KeyDesc.EmblemAnim = &m_eEmblemAnim_Type; /* emblem Anim */

		switch (m_iEmblemType)
		{
		case (_uint)CDoor::EMBLEM_TYPE::HEART_EMBLEM:
			m_iNeedItem = -1;

			EmblemDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm40_500_dooremblem01a_heart_Anim");
			MarkDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm40_504_dooremblemmark01a_heart");

			break;

		case (_uint)CDoor::EMBLEM_TYPE::SPADE_EMBLEM:
			m_iNeedItem = spadekey01a;

			KeyDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm73_103_spadekey01a");
			EmblemDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm40_500_dooremblem01a_00md_Anim");
			MarkDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm40_504_dooremblemmark01a_spade");


			break;

		case (_uint)CDoor::EMBLEM_TYPE::CLOVER_EMBLEM:
			m_iNeedItem = -1;

			EmblemDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm40_500_dooremblem01a_clover_Anim");
			MarkDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm40_504_dooremblemmark01a_clover");



			break;

		case (_uint)CDoor::EMBLEM_TYPE::DIA_EMBLEM:
			m_iNeedItem = diakey01a;

			KeyDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm73_140_diakey01a");
			EmblemDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm40_500_dooremblem01a_dia_Anim");
			MarkDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm40_504_dooremblemmark01a_dia");


			break;
		}
		pEmblem = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Medal_EmblemDoor"), &EmblemDesc));

		if (nullptr == pEmblem)
			return E_FAIL;
		else
			m_PartObjects[PART_EMBLEM] = pEmblem;

		pMark = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Mark_EmblemDoor"), &MarkDesc));

		if (nullptr == pMark)
			return E_FAIL;
		else
			m_PartObjects[PART_EMBLEM_MARK] = pMark;

		if (m_iEmblemType == (_int)CDoor::EMBLEM_TYPE::SPADE_EMBLEM ||
			m_iEmblemType == (_int)CDoor::EMBLEM_TYPE::DIA_EMBLEM)
		{
			CEmblem_Door* pEmblemObj = static_cast<CEmblem_Door*>(m_PartObjects[PART_EMBLEM]);
			KeyDesc.pParentWorldMatrix = pEmblemObj->Get_WorldMatrix();
			pKey = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Medal_KeyDoor"), &KeyDesc));
			if (nullptr == pKey)
				return E_FAIL;
			else
				m_PartObjects[PART_KEY] = pKey;
		}
		else
			m_PartObjects[PART_KEY] = nullptr;
	}


	return S_OK;
}

void CDoor::Do_Interact_Props()
{
	m_eEmblemAnim_Type = (_uint)CEmblem_Door::EMBLEM_ANIM::OPEN_ANIM;
}

HRESULT CDoor::Initialize_PartObjects()
{
	if (m_PartObjects[PART_BODY] != nullptr)
	{
		if (m_bLock == true && m_tagPropDesc.tagDoor.iLockType == Lock_Type::LOCK_EMBLEM)
			if (CDoor::EMBLEM_TYPE::SPADE_EMBLEM == static_cast<CDoor::EMBLEM_TYPE>(m_iEmblemType) ||
				CDoor::EMBLEM_TYPE::CLOVER_EMBLEM == static_cast<CDoor::EMBLEM_TYPE>(m_iEmblemType) ||
				CDoor::EMBLEM_TYPE::HEART_EMBLEM == static_cast<CDoor::EMBLEM_TYPE>(m_iEmblemType) ||
				CDoor::EMBLEM_TYPE::DIA_EMBLEM == static_cast<CDoor::EMBLEM_TYPE>(m_iEmblemType))
			{
				/* 1. Emblem*/
				CModel* pBodyModel = { static_cast<CModel*>(m_PartObjects[PART_BODY]->Get_Component(TEXT("Com_Body_Model"))) };

				CEmblem_Door* pItem1 = static_cast<CEmblem_Door*>(m_PartObjects[PART_EMBLEM]);
				_float4x4* pCombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("_00")) };
				pItem1->Set_Socket(pCombinedMatrix);
				pCombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("_01")) };
				pItem1->Set_Socket01(pCombinedMatrix);

				/* 2. Key */
				if (CDoor::EMBLEM_TYPE::SPADE_EMBLEM == static_cast<CDoor::EMBLEM_TYPE>(m_iEmblemType) || CDoor::EMBLEM_TYPE::DIA_EMBLEM == static_cast<CDoor::EMBLEM_TYPE>(m_iEmblemType))
				{
					CModel* pEmblemModel = { static_cast<CModel*>(m_PartObjects[PART_EMBLEM]->Get_Component(TEXT("Com_Body_Model"))) };

					CKey_Door* pKey = static_cast<CKey_Door*>(m_PartObjects[PART_KEY]);
					_float4x4* pCombinedMatrixKey = { const_cast<_float4x4*>(pEmblemModel->Get_CombinedMatrix("ItemSet")) };
					pKey->Set_Socket(pCombinedMatrixKey);
				}

				/*mark*/
				CMark_Door* pMark = static_cast<CMark_Door*>(m_PartObjects[PART_EMBLEM_MARK]);
				pCombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("_00")) };
				pMark->Set_Socket(pCombinedMatrix);
				pCombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("_01")) };
				pMark->Set_Socket01(pCombinedMatrix);

			}
	}

	return S_OK;
}

HRESULT CDoor::Add_Locations()
{
	list<CGameObject*>* pColliders = { m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Collider")) };
	if (nullptr == pColliders)
		return E_FAIL;

	CCollider* pMyCollider = { m_pColliderCom[INTER_COL_NORMAL][COL_STEP0] };
	if (nullptr == pMyCollider)
		return E_FAIL;

	for (auto& pCollider : *pColliders)
	{
		if (nullptr == pCollider)
			continue;

		if (true == pMyCollider->Intersect(static_cast<CCollider*>(pCollider->Get_Component(TEXT("Com_Collider")))))
		{
			LOCATION_MAP_VISIT				eLocation = { static_cast<LOCATION_MAP_VISIT>(static_cast<CCustomCollider*>(pCollider)->Get_Region()) };
			m_Linked_Locations.emplace_back(eLocation);
		}
	}

	for (auto& eLocation : m_Linked_Locations)
	{
		if (FAILED(CRoom_Finder::Get_Instance()->Add_Door(eLocation, this)))
		{
			MSG_BOX(TEXT("Called : HRESULT CDoor::Add_Locations() Add_Door Failed"));
		}
	}

#ifdef _DEBUG
	_uint			iNumLocation = { static_cast<_uint>(m_Linked_Locations.size()) };
	if (2 != iNumLocation)
	{
		_int iA = 0;

		//_int			iFloor = { m_pMyCustomCollider->Get_Floor() };
		//_int			iRegion = { m_pMyCustomCollider->Get_Region() };
	}
#endif

	return S_OK;
}

void CDoor::DoubleDoor_Tick(_float fTimeDelta)
{
	if (m_bAttack && m_eDoubleState != DOUBLEDOOR_STATIC && static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Is_Finishied_Anim())
		m_iHP = 5;
	if (m_bActivity)
		m_fTime += fTimeDelta;


	if (m_fTime > 2.f)
	{
		if (m_bCol[INTER_COL_NORMAL][COL_STEP0]|| m_bCol[INTER_COL_DOUBLE][COL_STEP0])
			m_fTime = 0.f;
		if (!m_bCol[INTER_COL_NORMAL][COL_STEP0] && !m_bCol[INTER_COL_DOUBLE][COL_STEP0] /*&& !m_bAttack*/)
		{
			m_fTime = 0.f;
			m_bActivity = false;
			m_eDoubleState = DOUBLEDOOR_STATIC;
		}
		
	}
	if (m_pZombieTransform == nullptr)
		m_bAttack = false;
	if (!m_bActivity && (m_bCol[INTER_COL_NORMAL][COL_STEP1] || m_bCol[INTER_COL_DOUBLE][COL_STEP1]))
	{
		if ((*m_pPlayerInteract || m_bCol[INTER_COL_NORMAL][COL_STEP2] || m_bCol[INTER_COL_DOUBLE][COL_STEP2])&&!m_bAttack)
			DoubleDoor_Active();

		m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;

		m_bCol[INTER_COL_DOUBLE][COL_STEP1] = false;
		m_bCol[INTER_COL_DOUBLE][COL_STEP2] = false;
	}

}

void CDoor::DoubleDoor_Late_Tick(_float fTimeDelta)
{
	//if (m_eDoubleDoorType == DOUBLE_DOOR_MODEL_TYPE::FRONT_DOOR)
	//{
	//	switch (m_eDoubleState)
	//	{
	//	case CDoor::LSIDE_DOUBLEDOOR_OPEN_L:
	//	{
	//		m_eDoubleState_Prev = m_eDoubleState;

	//		break;

	//	}
	//	case CDoor::LSIDE_DOUBLEDOOR_OPEN_R:
	//	{
	//		m_eDoubleState_Prev = m_eDoubleState;

	//		break;

	//	}
	//	case CDoor::RSIDE_DOUBLEDOOR_OPEN_L:
	//	{
	//		m_eDoubleState_Prev = m_eDoubleState;

	//		break;

	//	}
	//	case CDoor::RSIDE_DOUBLEDOOR_OPEN_R:
	//	{
	//		m_eDoubleState_Prev = m_eDoubleState;

	//		break;

	//	}
	//	case CDoor::DOUBLEDOOR_STATIC:
	//	{
	//		break;
	//	}
	//	/*case CDoor::L_DOUBLEDOOR_OPEN:
	//	{
	//		m_eDoubleState_Prev = m_eDoubleState;
	//		break;
	//	}
	//	case CDoor::R_DOUBLEDOOR_OPEN:
	//	{
	//		m_eDoubleState_Prev = m_eDoubleState;

	//		break;
	//	}*/
	//	}

	//}
	//else
	{
		switch (m_eDoubleState)
		{
		case CDoor::LSIDE_DOUBLEDOOR_OPEN_L:
		{
			m_eDoubleState_Prev = m_eDoubleState;
			break;
		}
		case CDoor::LSIDE_DOUBLEDOOR_OPEN_R:
		{
			m_eDoubleState_Prev = m_eDoubleState;

			break;
		}
		case CDoor::RSIDE_DOUBLEDOOR_OPEN_L:
		{
			m_eDoubleState_Prev = m_eDoubleState;

			break;
		}
		case CDoor::RSIDE_DOUBLEDOOR_OPEN_R:
		{
			m_eDoubleState_Prev = m_eDoubleState;

			break;
		}
		case CDoor::DOUBLEDOOR_STATIC:
		{
			break;
		}
		case CDoor::L_DOUBLEDOOR_OPEN:
		{
			m_eDoubleState_Prev = m_eDoubleState;
			break;
		}
		case CDoor::R_DOUBLEDOOR_OPEN:
		{
			m_eDoubleState_Prev = m_eDoubleState;

			break;
		}
		}
	}
	if (Activate_Col(Get_Collider_World_Pos(_float4(-90.f, 1.f, 0.f, 1.f))) || Activate_Col(Get_Collider_World_Pos(_float4(-30.f, 1.f, 0.f, 1.f))) 
		|| Activate_Col(Get_Collider_World_Pos(_float4(-150.f, 1.f, 0.f, 1.f))) || m_bCol[INTER_COL_NORMAL][COL_STEP0] || m_bCol[INTER_COL_DOUBLE][COL_STEP0]
		)
	{
		if (Check_Col_Player(INTER_COL_NORMAL, COL_STEP0)) // 인지?
		{
			if (Check_Col_Player(INTER_COL_NORMAL, COL_STEP1)) // 상호작용?
			{
				if (Check_Col_Player(INTER_COL_NORMAL, COL_STEP2) && !m_bActivity)
					m_bOnce = true;

			}
			else
			{
				m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;

			}

		}
		else
		{
			m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
			m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;

			// Destory : 점점 사라진 후에 null 
		}

		if (Check_Col_Player(INTER_COL_DOUBLE, COL_STEP0)) // 인지?
		{
			if (Check_Col_Player(INTER_COL_DOUBLE, COL_STEP1)) // 상호작용?
			{
				if (Check_Col_Player(INTER_COL_DOUBLE, COL_STEP2) && !m_bActivity)
					m_bOnce = true;
		

			}
			else
			{
				m_bCol[INTER_COL_DOUBLE][COL_STEP2] = false;
	

			}
		}
		else
		{
			m_bCol[INTER_COL_DOUBLE][COL_STEP1] = false;
			m_bCol[INTER_COL_DOUBLE][COL_STEP2] = false;
		}


	}
	else
	{
		m_bCol[INTER_COL_NORMAL][COL_STEP0] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;
		m_bCol[INTER_COL_DOUBLE][COL_STEP0] = false;
		m_bCol[INTER_COL_DOUBLE][COL_STEP1] = false;
		m_bCol[INTER_COL_DOUBLE][COL_STEP2] = false;

	}

	if (!m_bBlock && m_bOnce && (m_bCol[INTER_COL_NORMAL][COL_STEP2] || m_bCol[INTER_COL_DOUBLE][COL_STEP2]) && !m_bAttack)
	{
		m_bOnce = false;
		if (m_bLock)
			m_pPlayer->Set_Door_Setting(CPlayer::DOOR_BEHAVE_LOCK, Get_PlayerLook_Degree());
		else
			m_pPlayer->Set_Door_Setting(CPlayer::DOOR_BEHAVE_OPEN);
	}

	Select_UI();

}

void CDoor::DoubleDoor_Active()
{

	*m_pPlayerInteract = false;
	m_bActivity = true;

	if (m_bLock)
		return;

	m_bInteract = true;

	_float fScala = Radian_To_Player();

	if (XMConvertToDegrees(acosf(fScala)) <= 90.f)
	{
		if (m_bCol[INTER_COL_NORMAL][COL_STEP1] && m_bCol[INTER_COL_DOUBLE][COL_STEP1])
			m_eDoubleState = L_DOUBLEDOOR_OPEN;
		else if (m_bCol[INTER_COL_NORMAL][COL_STEP1])
			m_eDoubleState = LSIDE_DOUBLEDOOR_OPEN_L;
		else
			m_eDoubleState = RSIDE_DOUBLEDOOR_OPEN_R;

	}
	else
	{
		if (m_bCol[INTER_COL_NORMAL][COL_STEP1] && m_bCol[INTER_COL_DOUBLE][COL_STEP1])
			m_eDoubleState = R_DOUBLEDOOR_OPEN;
		else if (m_bCol[INTER_COL_NORMAL][COL_STEP1])
			m_eDoubleState = LSIDE_DOUBLEDOOR_OPEN_R;
		else
			m_eDoubleState = RSIDE_DOUBLEDOOR_OPEN_L;
	}


}

_float4 CDoor::Get_Object_Pos()
{
	if (m_eType == DOOR_DUMMY)
		return _float4();
	if (m_fTime > 0.3f)
		return _float4();
	if (m_eType == DOOR_DOUBLE)
	{
		_float fScala = Radian_To_Player();
		if ((!m_bCol[INTER_COL_NORMAL][COL_STEP0] && !m_bCol[INTER_COL_DOUBLE][COL_STEP0])
			|| (m_bCol[INTER_COL_NORMAL][COL_STEP0] && m_bCol[INTER_COL_DOUBLE][COL_STEP0]))
		{
			_float4 vPosR;
			_float4 vPosL;
			if (XMConvertToDegrees(acosf(fScala)) <= 90.f)
			{
				vPosR = static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos(0); // Lside_L
				vPosL = static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos(2); // Rside_L
			}
			else
			{
				vPosR = static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos(1); //Lside_R
				vPosL = static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos(3); //Rside_R
			}
			return XMVectorSetW((vPosR + vPosL) / 2.f, 1.f);
		}
		else if (m_bCol[INTER_COL_NORMAL][COL_STEP0])
		{
			if (XMConvertToDegrees(acosf(fScala)) <= 90.f)
				return static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos(0);
			else
				return static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos(1);
		}
		else
			if (XMConvertToDegrees(acosf(fScala)) <= 90.f)
				return static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos(2);
			else
				return static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos(3);

	}
	else
	{
		_float fScala = Radian_To_Player();
		if (XMConvertToDegrees(acosf(fScala)) <= 90.f)
			return static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos(0);
		else
			return static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos(1);
	}


	return _float4();
}

_bool CDoor::Attack_Prop(CTransform* pTransform)
{
	if (m_iHP <= 0)
	{
		m_bAttack = true;

		_float fScala = Radian_To_Zombie(pTransform);
		if (m_eType == DOOR_DOUBLE)
		{
			if (XMConvertToDegrees(acosf(fScala)) <= 90.f)
				m_eDoubleState = L_DOUBLEDOOR_OPEN;
			else
				m_eDoubleState = R_DOUBLEDOOR_OPEN;
		}
		else if (m_eType == DOOR_ONE)
		{
			if (XMConvertToDegrees(acosf(fScala)) <= 90.f)
				m_eOneState = ONEDOOR_OPEN_L;
			else
				m_eOneState = ONEDOOR_OPEN_R;
		}
		else
		{

		}
		m_pZombieTransform = nullptr;
		m_bActivity = true;


		return true;
	}
	else
	{
		m_bAttack = true;
		m_iHP -= 1;
		static_cast<CBody_Door*>(m_PartObjects[PART_BODY])->Hit(pTransform);
		m_pZombieTransform = pTransform;
		return false;
	}
}

void CDoor::OneDoor_Tick(_float fTimeDelta)
{
	if (m_bAttack && m_eOneState != ONEDOOR_STATIC && static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Is_Finishied_Anim())
		m_iHP = 5;

	if (m_bActivity)
		m_fTime += fTimeDelta;

	if (m_fTime > 2.f)
	{
		if (m_bCol[INTER_COL_NORMAL][COL_STEP0])
			m_fTime = 0.f;
		if (!m_bCol[INTER_COL_NORMAL][COL_STEP0]/* && !m_bAttack*/)
		{
			m_fTime = 0.f;
			m_bActivity = false;
			m_eOneState = ONEDOOR_STATIC;
		}

	}
	if (m_pZombieTransform == nullptr)
		m_bAttack = false;
	
	_bool bCam = { false };


	switch (m_eEmblemAnim_Type)
	{
	case  (_ubyte)CEmblem_Door::EMBLEM_ANIM::START_ANIM:
		if (m_pGameInstance->Get_KeyState(VK_ESCAPE) == DOWN)
		{
			m_eEmblemAnim_Type = (_ubyte)CEmblem_Door::EMBLEM_ANIM::STATIC_ANIM;
			bCam = true;
		}
		break;
	}



	if (m_bCamera && (bCam || static_cast<CEmblem_Door*>(m_PartObjects[PART_EMBLEM])->Get_Clear()))
	{
		if ((!bCam) && m_bLock)
		{
			if ((m_eEmblemAnim_Type == (_ubyte)CEmblem_Door::EMBLEM_ANIM::OPEN_ANIM || m_eEmblemAnim_Type == (_ubyte)CEmblem_Door::EMBLEM_ANIM::OPENED_ANIM) && m_fDelayLockTime == 0.f)
				m_fDelayLockTime = 1.5f;
		}
		else if (bCam || !m_bLock)
		{
			Reset_Camera();
			m_bCamera = false;
		}
	}

	if (m_bLock)
	{
		if (m_bCol[INTER_COL_NORMAL][COL_STEP1])
		{
		/*	if (m_pGameInstance->Get_KeyState(VK_F1) == DOWN)
			{
				m_pGameInstance->Active_Camera(g_Level, m_pCameraGimmick);
				Camera_Active(PART_KEY, _float3(-0.0001f, 0.0001f, -0.8f), CInteractProps::INTERACT_GIMMICK_TYPE::KEY_GIMMICK);



			}*/

			if (m_bCol[INTER_COL_NORMAL][COL_STEP2])
				m_bActivity = true;
			if ((*m_pPlayerInteract && false == m_pGameInstance->IsPaused())&& !m_bAttack)
				OneDoor_Active();
		}
		//m_bActivity = true;

	}
	else if ((m_bCol[INTER_COL_NORMAL][COL_STEP1] && !m_bActivity && (m_fDelayLockTime == 0.f) && m_eOneState == ONEDOOR_STATIC)|| m_bAutoOpen)
	{
		//UI띄우고

		if ((*m_pPlayerInteract || m_bCol[INTER_COL_NORMAL][COL_STEP2] && !m_bAttack)|| m_bAutoOpen)
			OneDoor_Active();



		m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;

	}
	else
	{
		//m_bCol[INTER_COL_NORMAL][COL_STEP0] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;
	}

	if (m_bCamera)
	{
		_float fScala = Radian_To_Player();

		if (XMConvertToDegrees(acosf(fScala)) <= 90.f)
			Camera_Active(PART_EMBLEM, _float3(-0.025f, 0.075f, -0.45f), CInteractProps::INTERACT_GIMMICK_TYPE::KEY_GIMMICK);
		else
			Camera_Active(PART_EMBLEM, _float3(0.025f, 0.075f, 0.45f), CInteractProps::INTERACT_GIMMICK_TYPE::KEY_GIMMICK);

	}
}

void CDoor::OneDoor_Late_Tick(_float fTimeDelta)
{
	switch (m_eOneState)
	{
	case CDoor::ONEDOOR_OPEN_L:
	{
		m_eOneState_Prev = m_eOneState;
		break;
	}
	case CDoor::ONEDOOR_OPEN_R:
		m_eOneState_Prev = m_eOneState;

	case CDoor::ONEDOOR_STATIC:
		break;
	}


	if (Activate_Col(Get_Collider_World_Pos(_float4(-60.f, 1.f, 0.f, 1.f)))|| m_bCol[INTER_COL_NORMAL][COL_STEP0])
	{
		if (Check_Col_Player(INTER_COL_NORMAL, COL_STEP0)) // 인지?
		{
			if (Check_Col_Player(INTER_COL_NORMAL, COL_STEP1)) // 상호작용?
			{
				if (Check_Col_Player(INTER_COL_NORMAL, COL_STEP2) && !m_bActivity)
					m_bOnce = true;


			}
			else
			{
				m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;

			}
		}
		else
		{
			m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
			m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;


		}
	}
	else
	{
		m_bCol[INTER_COL_NORMAL][COL_STEP0] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;



	}

	if (!m_bBlock && m_bOnce && (m_bCol[INTER_COL_NORMAL][COL_STEP2]||m_bAutoOpen)&&!m_bAttack)
	{
		m_bAutoOpen = false;
		Change_Same_Sound(TEXT("sound_Map_sm40_door_m_wood_normal2_12.mp3"), 0);
		m_bOnce = false;
		if (m_bLock)
			m_pPlayer->Set_Door_Setting(CPlayer::DOOR_BEHAVE_LOCK, Get_PlayerLook_Degree());
		else
			m_pPlayer->Set_Door_Setting(CPlayer::DOOR_BEHAVE_OPEN);
	}

	Select_UI();
}

_float CDoor::Radian_To_Player()
{
	_vector vLook = XMVector4Normalize(m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK));
	_vector vDir = XMVector4Normalize(m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION) - m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));
	_float fScala = XMVectorGetX(XMVector4Dot(vLook, vDir));
	if (fScala > 1.f)
		fScala = 1.f;
	else if (fScala < -1.f)
		fScala = -1.f;
	return fScala;
}

_float CDoor::Radian_To_Zombie(class CTransform* pTransform)
{
	_vector vLook = XMVector4Normalize(m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK));
	_vector vDir = XMVector4Normalize(pTransform->Get_State_Vector(CTransform::STATE_POSITION) - m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));
	_float fScala = XMVectorGetX(XMVector4Dot(vLook, vDir));
	if (fScala > 1.f)
		fScala = 1.f;
	else if (fScala < -1.f)
		fScala = -1.f;
	return fScala;
}

_float CDoor::Distance_Zombie(class CTransform* pTransform)
{
	_vector vDist = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION) - m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
	_float fDistance = XMVectorGetX(XMVector4Length(vDist));
	return fDistance;
}

void CDoor::OneDoor_Active()
{
	if (true == m_isKnock)
		return;

	*m_pPlayerInteract = false;
	m_bActivity = true;

	if (m_bLock)
	{
		//m_bInteract = true;

		m_pGameInstance->Active_Camera(g_Level, m_pCameraGimmick);

		m_bCamera = true;

		m_eEmblemAnim_Type = (_uint)CEmblem_Door::EMBLEM_ANIM::START_ANIM;
		m_pPlayer->Interact_Props(this);
	}
	else
	{
		if (m_bAutoOpen)
			m_bOnce = true;
		m_bInteract = true;

		_float fScala = Radian_To_Player();

		if (XMConvertToDegrees(acosf(fScala)) <= 90.f)
			m_eOneState = ONEDOOR_OPEN_L;
		else
			m_eOneState = ONEDOOR_OPEN_R;
	}

}


CDoor* CDoor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDoor* pInstance = new CDoor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDoor"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CDoor::Clone(void* pArg)
{
	CDoor* pInstance = new CDoor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CDoor"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDoor::Free()
{
	__super::Free();

	Safe_Release(m_pMyCustomCollider);
}
