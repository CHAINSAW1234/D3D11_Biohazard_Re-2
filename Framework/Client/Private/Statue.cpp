#include "stdafx.h"
#include "Statue.h"
#include"Player.h"

#include "Body_Statue.h"
#include "Item_Statue.h"
#include "Camera_Gimmick.h"

CStatue::CStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CStatue::CStatue(const CStatue& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CStatue::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStatue::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (m_tagPropDesc.strGamePrototypeName.find("109") != string::npos)
		m_eStatue_ItemType = (_ubyte)CItem_Statue::STATUE_ITEM::KINGSCEPTER_ITEM;

	else if (m_tagPropDesc.strGamePrototypeName.find("136") != string::npos)
		m_eStatue_ItemType = (_ubyte)CItem_Statue::STATUE_ITEM::STATUEHAND_ITEM;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Initialize_PartObjects()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Object_Sound(m_pTransformCom, 1)))
		return E_FAIL;

	m_iNeedItem = statuebookhand;
	m_iItemIndex = kingscepter01a;
	return S_OK;
}

void CStatue::Tick(_float fTimeDelta)
{
	__super::Tick_Col();

	if (m_bDead)
	{
		m_bItemDead = true;
		m_PartObjects[PART_ITEM_SCEPTER]->Set_Dead(true);
	}

	if (!m_bVisible)
		return;
	
	_bool bCam = { false };

	if(m_bCamera)
	{
		if (DOWN == m_pGameInstance->Get_KeyState(VK_RBUTTON))
			bCam = true;
	}
	if (m_isCamera_Reset)
		bCam = true;
	if (m_bCamera && (bCam || m_isCamera_Reset))
	{
		Reset_Camera();
		m_bCamera = false;
		m_isCamera_Reset = false;
	}


	if (m_bCamera)
	{
		//CPart_InteractProps* pPartLock = static_cast<CPart_InteractProps*>(m_PartObjects[PART_ITEM_SCEPTER]);
		//m_pCameraGimmick->SetPosition(pPartLock->Get_Pos_vector() + pPartLock->GetLookDir_Vector() * 0.15f + _vector{ 0.05f,0.1f,0.f,0.f });
		//m_pCameraGimmick->LookAt(pPartLock->Get_Pos());

		//m_pCameraGimmick->Active_Camera(true);
		Camera_Active(PART_BODY, _float3(-0.5f, -1.f, -1.5f),_float4(0.f,1.5f,0.f,0.f));

	}

	/* 예시 코드 아이템 먹었을 때 연결할 거임 */
	/* 예시 코드 아이템 먹었을 때 연결할 거임 */
	if (true == m_isPut_HandItem)
	{
		m_eState = static_cast<_ubyte>(CStatue::STATE_PLAY);
		m_bObtain = true;
	}
	
	if (m_bCol[INTER_COL_NORMAL][COL_STEP1]&&!m_bItemDead)
	{
		if (*m_pPlayerInteract)
			Active();
	}

	__super::Tick(fTimeDelta);

}

void CStatue::Late_Tick(_float fTimeDelta)
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
	
	if (Activate_Col(Get_Collider_World_Pos(_float4(0.f, 0.f, 0.f, 1.f))))
	{
		if (Check_Col_Player(INTER_COL_NORMAL, COL_STEP0))
			Check_Col_Player(INTER_COL_NORMAL, COL_STEP1);
		else
			m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
	}
	else
	{
		m_bCol[INTER_COL_NORMAL][COL_STEP0] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;

	}
	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUG
	__super::Add_Col_DebugCom();
#endif
}

HRESULT CStatue::Render()
{
	return S_OK;
}

void CStatue::Do_Interact_Props()
{
	m_isPut_HandItem = true;
}

HRESULT CStatue::Add_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(150.f);
	ColliderDesc.vCenter = _float3(-10.f, 1.f, 0.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Normal_Step0"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP0], &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.fRadius = _float(100.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Normal_Step1"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP1], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStatue::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CStatue::PART_END);

	/*Part_Body*/
	CPartObject* pBodyObj = { nullptr };
	CBody_Statue::STATUE_BODY_DESC BodyDesc = {};
	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pSoundCueSign = &m_bSoundCueSign;
	BodyDesc.pState = &m_eState;
	BodyDesc.pResetCamera = &m_isCamera_Reset;
	BodyDesc.strModelComponentName = m_tagPropDesc.strModelComponent;
	pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(m_tagPropDesc.strObjectPrototype, &BodyDesc));
	if (nullptr == pBodyObj)
		return E_FAIL;

	m_PartObjects[CStatue::PART_BODY] = pBodyObj;

	/* 1. Statue */
	CItem_Statue::BODY_ITEM_STATUE ItemDesc = {};
	CPartObject* pItemScepter = { nullptr };

	ItemDesc.pParentsTransform = m_pTransformCom;
	ItemDesc.pState = &m_eState;
	ItemDesc.pSoundCueSign = &m_bSoundCueSign;
	ItemDesc.isPut_HandItem = &m_isPut_HandItem;
	ItemDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm73_109_kingscepter01a_Anim");
	ItemDesc.eItemType = static_cast<_ubyte>(CItem_Statue::STATUE_ITEM::KINGSCEPTER_ITEM);
	pItemScepter = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Medal_Body_ItemStatue"), &ItemDesc));

	if (nullptr == pItemScepter)
		return E_FAIL;
	else
		m_PartObjects[PART_ITEM_SCEPTER] = pItemScepter;

	/* 2. Hand */
	CPartObject* pItemHand = { nullptr };

	ItemDesc.pParentsTransform = m_pTransformCom;
	ItemDesc.pState = &m_eState;
	ItemDesc.pSoundCueSign = &m_bSoundCueSign;
	ItemDesc.isPut_HandItem = &m_isPut_HandItem;
	ItemDesc.strModelComponentName = TEXT("Prototype_Component_Model_sm73_136_statuehand01a_Anim");
	ItemDesc.eItemType = static_cast<_ubyte>(CItem_Statue::STATUE_ITEM::STATUEHAND_ITEM);
	pItemHand = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Medal_Body_ItemStatue"), &ItemDesc));

	if (nullptr == pItemHand)
		return E_FAIL;
	else
		m_PartObjects[PART_ITEM_HAND] = pItemHand;

	return S_OK;
}

HRESULT CStatue::Initialize_PartObjects()
{
	if (m_PartObjects[PART_BODY] != nullptr)
	{
		CModel* pBodyModel = { dynamic_cast<CModel*>(m_PartObjects[PART_BODY]->Get_Component(TEXT("Com_Body_Model"))) };

		CItem_Statue* pItem1 = dynamic_cast<CItem_Statue*>(m_PartObjects[PART_ITEM_SCEPTER]);
		_float4x4* pCombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("ItemSet02")) };
		pItem1->Set_Socket(pCombinedMatrix);

		CItem_Statue* pItem2 = dynamic_cast<CItem_Statue*>(m_PartObjects[PART_ITEM_HAND]);
		pCombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("ItemSet01")) };
		pItem2->Set_Socket(pCombinedMatrix);
	}

	return S_OK;
}

HRESULT CStatue::Bind_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

void CStatue::Active()
{
	*m_pPlayerInteract = false;
	m_bActivity = true;
	m_bCamera = true;
	m_pGameInstance->Active_Camera(g_Level, m_pCameraGimmick);

	if(m_eState != static_cast<_ubyte>(CStatue::STATE_PLAY))
	{
		if (false == m_pGameInstance->IsPaused())
			m_pPlayer->Interact_Props(this);
	}
	else
	{
		if (m_bObtain&&!m_bItemDead)
			m_pPlayer->PickUp_Item(this);
	}
}

_float4 CStatue::Get_Object_Pos()
{
	return _float4();
}

CStatue* CStatue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CStatue* pInstance = new CStatue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStatue"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CStatue::Clone(void* pArg)
{
	CStatue* pInstance = new CStatue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CStatue"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStatue::Free()
{
	__super::Free();

}
