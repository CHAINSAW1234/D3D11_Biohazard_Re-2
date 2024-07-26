#include "stdafx.h"
#include "Cabinet.h"
#include"Player.h"
#include "PxCollider.h"
#include "Bone.h"

#include"Camera_Gimmick.h"

//part-obj
#include"Body_Cabinet.h"
#include"Body_ItemProp.h"
#include"Lock_Cabinet.h"
#include"Card_Cabinet.h"

#include "Call_Center.h"

CCabinet::CCabinet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CCabinet::CCabinet(const CCabinet& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CCabinet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCabinet::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	INTERACTPROPS_DESC* pObj_desc = (INTERACTPROPS_DESC*)pArg;
	m_bLock = pObj_desc->tagCabinet.bLock;
	for (size_t i = 0; i < 10; i++)
		m_iPassWord[i] = pObj_desc->tagCabinet.iLockNum[i];

	if (m_tagPropDesc.strGamePrototypeName.find("006") != string::npos)
	{
		m_eCabinetType = TYPE_LEON;
		m_bLockLeon = true;
	}
	else if (m_tagPropDesc.strGamePrototypeName.find("003") != string::npos)
		m_eCabinetType = TYPE_SAFEBOX;
	else if (m_tagPropDesc.strGamePrototypeName.find("005") != string::npos)
		m_eCabinetType = TYPE_ELECTRIC;
	else if (m_tagPropDesc.strGamePrototypeName.find("020") != string::npos)
	{
		m_eCabinetType = TYPE_WEAPON;
		m_iItemIndex = ShotGun;
	}
	else if (m_tagPropDesc.strGamePrototypeName.find("024") != string::npos)
	{
		if (FAILED(CCall_Center::Get_Instance()->Add_Caller(this, CCall_Center::CALLER::_ZOMBIE_HIDE_LOCKER)))
			return E_FAIL;

		m_eCabinetType = TYPE_ZOMBIE;
	}
	else
		m_eCabinetType = TYPE_NORMAL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Initialize_PartObjects()))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Object_Sound(m_pTransformCom, 3)))
		return E_FAIL;

	if (m_eCabinetType == TYPE_ELECTRIC)
		m_strElectTag = static_cast<CBody_Cabinet*>(m_PartObjects[PART_BODY])->Get_Tag();
	


	return S_OK;
}

void CCabinet::Tick(_float fTimeDelta)
{
	//#ifdef _DEBUG
	//#ifdef UI_POS
	//	Get_Object_Pos();
	//#endif
	//#endif
	__super::Tick_Col();
	if (!m_bVisible)
		return;
	if (m_fDelayLockTime > 0.f)
		m_fDelayLockTime -= fTimeDelta;
	if (m_fDelayLockTime < 0.f)
	{
		m_fDelayLockTime = 0.f;
		m_bLock = false;
		m_bAutoOpen = true;
	}

	//if (m_bActivity)
	//	m_fTimeDelay += fTimeDelta;
	//if (m_fTimeDelay > 1.f)
	//{
	//	m_bActivity = false;
	//	m_fTimeDelay = 0.f;
	//}

	switch (m_eCabinetType)
	{
	case TYPE_NORMAL:
	case TYPE_SAFEBOX:
		Safe_Normal_Tick(fTimeDelta); 
		break;
	case TYPE_ELECTRIC:
		Electric_Tick(fTimeDelta);
		break;
	case TYPE_LEON:
		LeonDesk_Tick(fTimeDelta);
		break;
	case TYPE_WEAPON:
		Weapon_Tick(fTimeDelta);
		break;
	case TYPE_ZOMBIE:
		Zombie_Tick(fTimeDelta);
		break;
	}




}

void CCabinet::Late_Tick(_float fTimeDelta)
{
	if (m_pPlayer == nullptr)
		return;
	if (!Visible())
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
	if (Activate_Col(Get_Collider_World_Pos(_float4(50.f, 1.f, 50.f, 1.f))) || Activate_Col(Get_Collider_World_Pos(_float4(-50.f, 1.f, 50.f, 1.f))))
	{
		if (Check_Col_Player(INTER_COL_NORMAL, COL_STEP0))
			Check_Col_Player(INTER_COL_NORMAL, COL_STEP1);
		else
			m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;

		if (m_bLeonDesk)
			if (Check_Col_Player(INTER_COL_DOUBLE, COL_STEP0))
				Check_Col_Player(INTER_COL_DOUBLE, COL_STEP1);
			else
				m_bCol[INTER_COL_DOUBLE][COL_STEP1] = false;

	}
	else
	{
		m_bCol[INTER_COL_NORMAL][COL_STEP0] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;
		if (m_bLeonDesk)
		{
			m_bCol[INTER_COL_DOUBLE][COL_STEP0] = false;
			m_bCol[INTER_COL_DOUBLE][COL_STEP1] = false;
			m_bCol[INTER_COL_DOUBLE][COL_STEP2] = false;
		}
	}

	__super::Late_Tick(fTimeDelta);


#ifdef _DEBUG
	__super::Add_Col_DebugCom();
#endif
}

HRESULT CCabinet::Render()
{
	return S_OK;
}

HRESULT CCabinet::Add_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(100.f);
	ColliderDesc.vCenter = _float3(50.f, 1.f, 50.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Normal_Step0"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP0], &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.fRadius = _float(65.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Normal_Step1"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP1], &ColliderDesc)))
		return E_FAIL;

	if (m_tagPropDesc.strGamePrototypeName.find("006") != string::npos)
	{
		ColliderDesc.fRadius = _float(100.f);
		ColliderDesc.vCenter = _float3(-50.f, 1.f, 50.f);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Double_Step0"), (CComponent**)&m_pColliderCom[INTER_COL_DOUBLE][COL_STEP0], &ColliderDesc)))
			return E_FAIL;

		ColliderDesc.fRadius = _float(65.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Double_Step1"), (CComponent**)&m_pColliderCom[INTER_COL_DOUBLE][COL_STEP1], &ColliderDesc)))
			return E_FAIL;

		m_bLeonDesk = true;
	}


	return S_OK;
}

HRESULT CCabinet::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CCabinet::PART_END);

	/*Part_Body*/
	CPartObject* pBodyObj = { nullptr };
	CBody_Cabinet::BODY_CABINET_DESC BodyDesc = {};
	BodyDesc.pSoundCueSign = &m_bSoundCueSign;
	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pState = &m_eState;
	BodyDesc.strModelComponentName = m_tagPropDesc.strModelComponent;
	BodyDesc.eCabinetType = m_eCabinetType;
	BodyDesc.pLock = &m_bLock;
	BodyDesc.iPropType = m_tagPropDesc.iPropType;
	pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(m_tagPropDesc.strObjectPrototype, &BodyDesc));
	if (nullptr == pBodyObj)
		return E_FAIL;

	m_PartObjects[CCabinet::PART_BODY] = pBodyObj;

	/*Part_Item*/
	if (m_tagPropDesc.tagCabinet.bItem)
	{
		CPartObject* pItem = { nullptr };
		CBody_ItemProp::BODY_ITEMPROPS_DESC ItemDesc = {};
		ItemDesc.pParentsTransform = m_pTransformCom;
		m_iItemIndex = ItemDesc.iItemIndex = m_tagPropDesc.tagCabinet.iItemIndex;
		ItemDesc.pSoundCueSign = &m_bSoundCueSign;
		ItemDesc.pState = &m_eState;
		ItemDesc.pItemDead = &m_bItemDead; //얻었는가?
		if (m_eCabinetType == TYPE_WEAPON)
			m_tagPropDesc.tagCabinet.Name = TEXT("ShotGun");
		ItemDesc.strModelComponentName = TEXT("Prototype_Component_Model_") + m_tagPropDesc.tagCabinet.Name;
		
		/*if(m_tagPropDesc.tagCabinet.iItemIndex==0)*/
		pItem = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_") + m_tagPropDesc.tagCabinet.Name, &ItemDesc));
		if (nullptr == pItem)
			return E_FAIL;
		m_PartObjects[CCabinet::PART_ITEM] = pItem;
		m_bItemDead = false;
	}
	else
	{
		m_bItemDead = true;
		m_PartObjects[CCabinet::PART_ITEM] = nullptr;
	}



	/*Part_Lock*/
	if (m_bLock)
	{
		switch (m_eCabinetType)
		{
		case TYPE_NORMAL:
		{
			CPartObject* pLock = { nullptr };
			CLock_Cabinet::BODY_LOCK_CABINET_DESC LockDesc = {};
			LockDesc.pParentsTransform = m_pTransformCom;
			LockDesc.pSoundCueSign = &m_bSoundCueSign;
			LockDesc.pState = &m_eState;
			LockDesc.pKeyInput = &m_eKeyInput;
			LockDesc.pLockState = &m_eLockState;
			LockDesc.pPassword = (_int*)m_iPassWord;
			LockDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm42_014_diallock01a_Anim");
			LockDesc.iLockType = CLock_Cabinet::OPENLOCKER_DIAL;
			pLock = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Lock_Cabinet"), &LockDesc));
			if (nullptr == pLock)
				return E_FAIL;
			m_PartObjects[CCabinet::PART_LOCK] = pLock;
		}
		break;
		case TYPE_SAFEBOX:
		{
			CPartObject* pLock = { nullptr };
			CLock_Cabinet::BODY_LOCK_CABINET_DESC LockDesc = {};
			LockDesc.pParentsTransform = m_pTransformCom;
			LockDesc.pState = &m_eState; //현재 캐비넷 본체의 상황을 받는거야
			LockDesc.pPassword = (_int*)m_iPassWord; // 123 
			LockDesc.pSoundCueSign = &m_bSoundCueSign;
			LockDesc.pKeyInput = &m_eKeyInput;
			LockDesc.pCameraControl = &m_isCamera_Reset;
			LockDesc.pLockState = &m_eLockState; //제어당할 스테이트
			LockDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm42_019_safeboxdial01a_Anim");
			LockDesc.iLockType = CLock_Cabinet::SAFEBOX_DIAL;
			pLock = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Lock_Cabinet"), &LockDesc));
			if (nullptr == pLock)
				return E_FAIL;
			m_PartObjects[CCabinet::PART_LOCK] = pLock;
		}
		break;
		case TYPE_ELECTRIC:
			break;
		case TYPE_LEON:
		{
			m_iPassWord[0] = 13;
			m_iPassWord[1] = 4;
			m_iPassWord[2] = 3;
			CPartObject* pLock = { nullptr };
			CLock_Cabinet::BODY_LOCK_CABINET_DESC LockDesc = {};
			LockDesc.pParentsTransform = m_pTransformCom;
			LockDesc.pSoundCueSign = &m_bSoundCueSign;
			LockDesc.pState = &m_eState;
			LockDesc.pKeyInput = &m_eKeyInput;
			LockDesc.pLockState = &m_eLockState;
			LockDesc.pPassword = (_int*)m_iPassWord;
			LockDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm42_014_diallock01a_Anim");
			LockDesc.iLockType = CLock_Cabinet::OPENLOCKER_DIAL;
			/*if(m_tagPropDesc.tagCabinet.iItemIndex==0)*/
			pLock = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Lock_Cabinet"), &LockDesc));
			if (nullptr == pLock)
				return E_FAIL;

			m_iPassWordLeon[0] = 12;
			m_iPassWordLeon[1] = 17;
			m_iPassWordLeon[2] = 6;
			m_PartObjects[CCabinet::PART_LOCK] = pLock;
			pLock = { nullptr };
			LockDesc = {};
			LockDesc.pParentsTransform = m_pTransformCom;
			LockDesc.pSoundCueSign = &m_bSoundCueSign;
			LockDesc.pState = &m_eState;
			LockDesc.pKeyInput = &m_eKeyInput;
			LockDesc.pLockState = &m_eLockLeonState;
			LockDesc.pPassword = (_int*)m_iPassWordLeon;
			LockDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm42_014_diallock01a_Anim");
			LockDesc.iLockType = CLock_Cabinet::OPENLOCKER_DIAL;
			/*if(m_tagPropDesc.tagCabinet.iItemIndex==0)*/
			pLock = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Lock_Cabinet"), &LockDesc));
			if (nullptr == pLock)
				return E_FAIL;
			m_PartObjects[CCabinet::PART_LOCK1] = pLock;
		}
		break;
		case TYPE_WEAPON:
		{
			CPartObject* pLock = { nullptr };
			CLock_Cabinet::BODY_LOCK_CABINET_DESC LockDesc = {};
			LockDesc.pParentsTransform = m_pTransformCom;
			LockDesc.pState = &m_eState;
			LockDesc.pSoundCueSign = &m_bSoundCueSign;
			LockDesc.pKeyInput = &m_eKeyInput;
			LockDesc.pAction = &m_bAction;
			LockDesc.pLockState = &m_eLockState;
			LockDesc.pPassword = (_int*)m_iPassWord;
			LockDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm42_174_cardreader04a_Anim");
			LockDesc.iLockType = CLock_Cabinet::CARD_KEY;
			pLock = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Lock_Cabinet"), &LockDesc));
			if (nullptr == pLock)
				return E_FAIL;
			m_PartObjects[CCabinet::PART_LOCK] = pLock;
		}
		break;
		}
	}
	else
		m_PartObjects[CCabinet::PART_LOCK] = nullptr;

	/*PART_CARD*/
	if (m_eCabinetType == TYPE_WEAPON)
	{
		m_iNeedItem = cardkeylv201a;

		CPartObject* pCard = { nullptr };
		CCard_Cabinet::CARD_CABINET_DESC CardCabinet = {};
		CardCabinet.pParentsTransform = m_pTransformCom;
		CardCabinet.pLock_Cabinet_State = &m_eLockState;
		CardCabinet.pSoundCueSign = &m_bSoundCueSign;
		CardCabinet.pAction = &m_bAction;
		CardCabinet.pLock_WorldMatrix = static_cast<CPart_InteractProps*>(m_PartObjects[CCabinet::PART_LOCK])->Get_WorldMatrix_Ptr();
		CardCabinet.strModelComponentName = TEXT("Prototype_Component_Model_sm73_105_cardkeylv201a");
		pCard = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Card_Cabinet"), &CardCabinet));
		if (nullptr == pCard)
			return E_FAIL;

		m_PartObjects[CCabinet::PART_CARD] = pCard;
	}
	else
		m_PartObjects[CCabinet::PART_CARD] = nullptr;

	return S_OK;
}

HRESULT CCabinet::Initialize_PartObjects()
{
	if (m_PartObjects[PART_ITEM] != nullptr)
	{

		CModel* pBodyModel = { static_cast<CModel*>(m_PartObjects[PART_BODY]->Get_Component(TEXT("Com_Body_Model"))) };

		CBody_ItemProp* pItem = static_cast<CBody_ItemProp*>(m_PartObjects[PART_ITEM]);
		_float4x4* pCombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("ItemSet01")) };
		pItem->Set_Socket(pCombinedMatrix);

	}

	if (m_bLock && m_eCabinetType != TYPE_ELECTRIC&& m_eCabinetType != TYPE_WEAPON)
	{
		CModel* pBodyModel = { static_cast<CModel*>(m_PartObjects[PART_BODY]->Get_Component(TEXT("Com_Body_Model"))) };

		CLock_Cabinet* pLock = static_cast<CLock_Cabinet*>(m_PartObjects[PART_LOCK]);
		_float4x4* pCombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("GimmickSet")) };
		pLock->Set_Socket(pCombinedMatrix);
		if (m_eCabinetType == TYPE_LEON)
		{
			pLock = static_cast<CLock_Cabinet*>(m_PartObjects[PART_LOCK1]);
			_float4x4* pCombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("GimmickSet2")) };
			pLock->Set_Socket(pCombinedMatrix);
		}
	}

	if (m_eCabinetType == TYPE_WEAPON)
	{
		CModel* pBodyModel = { static_cast<CModel*>(m_PartObjects[PART_BODY]->Get_Component(TEXT("Com_Body_Model"))) };

		CLock_Cabinet* pLock = static_cast<CLock_Cabinet*>(m_PartObjects[PART_LOCK]);
		_float4x4* pCombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("ItemSet")) };
		pLock->Set_Socket(pCombinedMatrix);


		CModel* pLockModel = { static_cast<CModel*>(m_PartObjects[PART_LOCK]->Get_Component(TEXT("Com_Body_Model"))) };

		CCard_Cabinet* pCard = static_cast<CCard_Cabinet*>(m_PartObjects[PART_CARD]);
		_float4x4* pLockCombinedMatrix = { const_cast<_float4x4*>(pLockModel->Get_CombinedMatrix("ItemSet")) };
		pCard->Set_Socket(pLockCombinedMatrix);

	}
	return S_OK;
}

HRESULT CCabinet::Bind_ShaderResources()
{
	return S_OK;
}

void CCabinet::Do_Interact_Props()
{
	if (m_eCabinetType == CCabinet::TYPE_WEAPON)
		m_bAction = true;
}

void CCabinet::Safe_Normal_Tick(_float fTimeDelta)
{
	_bool bCam = false;

	if (m_eLockState == CCabinet::LIVE_LOCK)
	{
		if (DOWN == m_pGameInstance->Get_KeyState('D'))
			m_eKeyInput = KEY_D;
		else if (DOWN == m_pGameInstance->Get_KeyState('A'))
			m_eKeyInput = KEY_A;
		else if (DOWN == m_pGameInstance->Get_KeyState('W'))
			m_eKeyInput = KEY_W;
		else if (DOWN == m_pGameInstance->Get_KeyState('S'))
			m_eKeyInput = KEY_S;
		if (DOWN == m_pGameInstance->Get_KeyState(VK_ESCAPE))
		{
			if (m_eLockState != CCabinet::CLEAR_LOCK)
				m_eLockState = CCabinet::STATIC_LOCK;
			bCam = true;
		}

	}
	else
		m_eKeyInput = KEY_NOTHING;
	if (m_isCamera_Reset)
		bCam = true;
	if (m_bCamera && (bCam || static_cast<CLock_Cabinet*>(m_PartObjects[PART_LOCK])->Get_Clear()))
	{
		if (!bCam&&m_bLock)
		{
			if (m_eLockState == CCabinet::CLEAR_LOCK && m_fDelayLockTime == 0.f)
				m_fDelayLockTime = 2.f;
		}
		else if (bCam || !m_bLock)
		{
			Reset_Camera();
			m_bCamera = false;
		}
	}
	
	if (m_bDead)
		m_bItemDead = true;

	if (!m_bDead && m_bCol[INTER_COL_NORMAL][COL_STEP1] /*&& !m_bActivity*/)
	{
		if (*m_pPlayerInteract&& false == m_pGameInstance->IsPaused())
			Safe_Normal_Active();
	}

	if (m_bCamera)
	{
		if (m_eCabinetType == TYPE_NORMAL)
			Camera_Active(PART_LOCK, _float3(0.025f, -0.015f, 0.2f), CInteractProps::INTERACT_GIMMICK_TYPE::LOCK_GIMMICK);
		else
			Camera_Active(PART_LOCK, _float3(0.1f, 0.1f, 0.4f), CInteractProps::INTERACT_GIMMICK_TYPE::LOCK_GIMMICK);
	}

	if (m_eState == CABINET_OPEN)
	{
		m_bObtain = true;
		return;
	}
	__super::Tick(fTimeDelta);
	
}

void CCabinet::LeonDesk_Tick(_float fTimeDelta)
{
	_bool bCam = false;

	if (m_eLockState == CCabinet::LIVE_LOCK || m_eLockLeonState == CCabinet::LIVE_LOCK)
	{
		if (DOWN == m_pGameInstance->Get_KeyState('D'))
			m_eKeyInput = KEY_D;
		else if (DOWN == m_pGameInstance->Get_KeyState('A'))
			m_eKeyInput = KEY_A;
		else if (DOWN == m_pGameInstance->Get_KeyState('W'))
			m_eKeyInput = KEY_W;
		else if (DOWN == m_pGameInstance->Get_KeyState('S'))
			m_eKeyInput = KEY_S;
		if (DOWN == m_pGameInstance->Get_KeyState(VK_ESCAPE))
		{
			if (m_eLockState != CCabinet::CLEAR_LOCK)
				m_eLockState = CCabinet::STATIC_LOCK;
			if (m_eLockLeonState != CCabinet::CLEAR_LOCK)
				m_eLockLeonState = CCabinet::STATIC_LOCK;

			bCam = true;
		}

	}
	else
		m_eKeyInput = KEY_NOTHING;

	if (m_bCamera && (bCam|| static_cast<CLock_Cabinet*>(m_PartObjects[PART_LOCK])->Get_Clear() || static_cast<CLock_Cabinet*>(m_PartObjects[PART_LOCK1])->Get_Clear()))
	{
		_bool bCamera_Reset = { false };
		if (!bCam)
		{
			if (m_bLock && m_eLockState == CCabinet::CLEAR_LOCK&& (bCamera_Reset=true))
				m_bLock = false;
			else if (m_bLockLeon && m_eLockLeonState == CCabinet::CLEAR_LOCK&& (bCamera_Reset=true))
				m_bLockLeon = false;

		}

		 if(bCam || bCamera_Reset)
		{
			 Reset_Camera();
		}
		

	}
	if ((!m_bLock && !m_bLockLeon) && m_bCamera == true)
	 {
		m_fDelayLockTime = 5.f;
		m_bCamera = false;
		Reset_Camera();

	}

	if (m_bDead)
		m_bItemDead = true;

	if (!m_bDead && ((m_bCol[INTER_COL_NORMAL][COL_STEP1] || m_bCol[INTER_COL_DOUBLE][COL_STEP1]) || m_bAutoOpen)/* && !m_bActivity*/)
	{
		if ((*m_pPlayerInteract || m_bAutoOpen) && false == m_pGameInstance->IsPaused())
			LeonDesk_Active();
	}

	if (m_bCamera)
	{
		if(m_bCol[INTER_COL_NORMAL][COL_STEP0]&&m_bActivity&&m_bLock)
		{
			Camera_Active(PART_LOCK, _float3(0.015f, -0.1f, 0.2f), CInteractProps::INTERACT_GIMMICK_TYPE::KEY_GIMMICK);
			m_bActivity = false;
		}
		 if (m_bCol[INTER_COL_DOUBLE][COL_STEP0] && m_bActivity && m_bLockLeon)
		{
			Camera_Active(PART_LOCK1, _float3(-0.015f, -0.1f, 0.2f), CInteractProps::INTERACT_GIMMICK_TYPE::KEY_GIMMICK);
			m_bActivity = false;
		}
	}

	if (m_eState == CABINET_OPEN)
	{
		m_bObtain = true;
		return;
	}
	__super::Tick(fTimeDelta);
	
}

void CCabinet::Electric_Tick(_float fTimeDelta)
{
	_bool bCam = false;
	if (DOWN == m_pGameInstance->Get_KeyState(VK_ESCAPE))
	{
		bCam = true;
	}
	if (m_bCamera)
		Camera_Active(PART_BODY, _float3(-0.001f, 1.5f, -0.4f), CInteractProps::INTERACT_GIMMICK_TYPE::LOCK_GIMMICK);
	if (m_fDelayLockTime > 0.f)
	{
		m_pGameInstance->Active_Camera(g_Level, m_pCameraGimmick);

		m_bCamera = true;
	}

	if (m_bDead)
		m_bItemDead = true;


	if (m_bCamera && (bCam||!m_bLock))
	{
		Reset_Camera();
		m_bCamera = false;
	}

	if (!m_bDead && m_bCol[INTER_COL_NORMAL][COL_STEP1] /*&& !m_bActivity*/)
	{
		if (*m_pPlayerInteract&& false == m_pGameInstance->IsPaused())
			Electric_Active();
	}
	if (m_eState == CABINET_OPEN)
	{
		m_bObtain = true;
		return;
	}
	__super::Tick(fTimeDelta);
}

void CCabinet::Weapon_Tick(_float fTimeDelta)
{
	
	_bool bCam = false;

	if (m_eLockState == CCabinet::LIVE_LOCK)
	{
		if (DOWN == m_pGameInstance->Get_KeyState(VK_ESCAPE))
			bCam = true;
	}
	if (m_bCamera && (bCam || static_cast<CLock_Cabinet*>(m_PartObjects[PART_LOCK])->Get_Clear()))
	{
		if (!bCam&& m_bLock)
		{
			if (m_eLockState == CCabinet::CLEAR_LOCK&& m_fDelayLockTime==0.f)
				m_fDelayLockTime = 4.f;
			//m_bLock = false;
		}
		else if (bCam||!m_bLock)
		{
			Reset_Camera();
			m_bCamera = false;

		}
	}


	if (m_bDead)
		m_bItemDead = true;

	if (m_bCamera)
		Camera_Active(PART_LOCK, _float3(0.05f, 0.15f, 0.3f), CInteractProps::INTERACT_GIMMICK_TYPE::KEY_GIMMICK);
	
	
	if ((!m_bDead && m_bCol[INTER_COL_NORMAL][COL_STEP1]) || m_bAutoOpen)
	{
		if ((*m_pPlayerInteract || m_bAutoOpen) && false == m_pGameInstance->IsPaused())
			Weapon_Active();
	}
	if (m_eState == CABINET_OPEN)
	{
		m_bObtain = true;
		return;
	}
	__super::Tick(fTimeDelta);
}

void CCabinet::Zombie_Tick(_float fTimeDelta)
{
	m_tagPropDesc.iRegionNum = 27;
	if (m_bCol[INTER_COL_NORMAL][COL_STEP1] /*&& !m_bActivity*/)
	{
		if (*m_pPlayerInteract&& false == m_pGameInstance->IsPaused())
			Zombie_Active();
	}
	if (m_eState == CABINET_OPENED)
		m_bOpened = true;
	__super::Tick(fTimeDelta);

}

void CCabinet::Safe_Normal_Active()
{
	*m_pPlayerInteract = false;
	m_bActivity = true;


	if (m_bAutoOpen)
	{
		if (m_bObtain)
			if (nullptr != m_PartObjects[PART_ITEM] && !m_bItemDead)
				m_pPlayer->PickUp_Item(this);
		m_bAutoOpen = false;
	}
	if (!m_bLock)
	{
		m_eState = CABINET_OPEN;
		if (m_bObtain)
			if (nullptr != m_PartObjects[PART_ITEM] && !m_bItemDead)
				m_pPlayer->PickUp_Item(this);
	}
	else
	{
		m_eLockState = LIVE_LOCK;

		m_pGameInstance->Active_Camera(g_Level, m_pCameraGimmick);

		//tabwindow의 힘을 빌려야 할 차례입니다
		m_bCamera = true;

	}

}

void CCabinet::LeonDesk_Active()
{

	*m_pPlayerInteract = false;
	m_bActivity = true;

	if (m_bAutoOpen )
	{
		m_pGameInstance->Active_Camera(g_Level, m_pCameraGimmick);
		m_bCamera = true;
		if (m_bObtain)
			if (nullptr != m_PartObjects[PART_ITEM] && !m_bItemDead)
				m_pPlayer->PickUp_Item(this);
		m_bAutoOpen = false;
	}
	if (!m_bLock&&!m_bLockLeon)
	{
		m_eState = CABINET_OPEN;
		if (m_bObtain)
			if (nullptr != m_PartObjects[PART_ITEM] && !m_bItemDead)
				m_pPlayer->PickUp_Item(this);
	}
	else
	{
		if (m_bCol[INTER_COL_NORMAL][COL_STEP0] && m_bLock)
		{
			m_eLockState = LIVE_LOCK;
			m_pGameInstance->Active_Camera(g_Level, m_pCameraGimmick);
			m_bCamera = true;
		}
		else if (m_bCol[INTER_COL_DOUBLE][COL_STEP0] && m_bLockLeon)
		{
			m_eLockLeonState = LIVE_LOCK;
			m_pGameInstance->Active_Camera(g_Level, m_pCameraGimmick);

			m_bCamera = true;
		}

		//tabwindow의 힘을 빌려야 할 차례입니다

	}
}

void CCabinet::Electric_Active()
{
	*m_pPlayerInteract = false;
	m_bActivity = true;

	if (!m_bLock)
	{
		m_eState = CABINET_OPEN;
		if (m_bObtain)
			if (nullptr != m_PartObjects[PART_ITEM] && !m_bItemDead)
				m_pPlayer->PickUp_Item(this);
	}
}

void CCabinet::Weapon_Active()
{
	*m_pPlayerInteract = false;
	m_bActivity = true;
		
	if (m_bAutoOpen )
	{
		if (m_bObtain)
			if (nullptr != m_PartObjects[PART_ITEM] && !m_bItemDead)
				m_pPlayer->PickUp_Item(this);
		m_bAutoOpen = false;
	}

	if (!m_bLock)
	{
		m_eState = CABINET_OPEN;
		if (m_bObtain)
			if (nullptr != m_PartObjects[PART_ITEM] && !m_bItemDead)
				m_pPlayer->PickUp_Item(this);
	}
	else
	{
		m_eLockState = LIVE_LOCK;
		m_pGameInstance->Set_IsPaused(true);
		m_pGameInstance->Active_Camera(g_Level, m_pCameraGimmick);
		m_bCamera = true;
		m_pPlayer->Interact_Props(this);

	}
}

void CCabinet::Zombie_Active()
{
	*m_pPlayerInteract = false;
	m_bActivity = true;

	m_eState = CABINET_OPEN;
}

CCabinet* CCabinet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCabinet* pInstance = new CCabinet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCabinet"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CCabinet::Clone(void* pArg)
{
	CCabinet* pInstance = new CCabinet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CCabinet"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCabinet::Free()
{
	__super::Free();

}

_float4 CCabinet::Get_Object_Pos()
{
	//m_bObtain => 아이템을 얻을 수 있는 상태
	if (m_bObtain)
		if (m_tagPropDesc.tagCabinet.bItem)
			return static_cast<CPart_InteractProps*>(m_PartObjects[PART_ITEM])->Get_Pos();
		else if (!m_bOpened)
			return static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos();
		else
			return _float4(0.f, 0.f, 0.f, 1.f);

	if (m_PartObjects[PART_LOCK] == nullptr)
		return static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos();
	else
		return static_cast<CPart_InteractProps*>(m_PartObjects[PART_LOCK])->Get_Pos();

}
