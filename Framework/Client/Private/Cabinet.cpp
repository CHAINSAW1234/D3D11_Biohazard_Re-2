#include "stdafx.h"
#include "Cabinet.h"
#include"Player.h"
#include "PxCollider.h"
#include "Bone.h"

//part-obj
#include"Body_Cabinet.h"
#include"Body_ItemProp.h"
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

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;	
	
	if (FAILED(Initialize_PartObjects()))
		return E_FAIL;	



	return S_OK;
}

void CCabinet::Tick(_float fTimeDelta)
{
	__super::Tick_Col();
	if (!m_bVisible)
		return;
	if (m_PartObjects[PART_ITEM] != nullptr)
		if (m_PartObjects[PART_ITEM]->Get_Dead() == true)
			Set_Region(-1);
	
//#ifdef _DEBUG
//#ifdef UI_POS
//	Get_Object_Pos();
//#endif
//#endif
	if (m_bActivity)
		m_fTimeDelay += fTimeDelta;
	if (m_fTimeDelay > 1.f)
	{
		m_bActivity = false;
		m_fTimeDelay = 0.f;
	}


	if ((m_bCol[INTER_COL_NORMAL][COL_STEP1]|| m_bCol[INTER_COL_DOUBLE][COL_STEP1]) && !m_bActivity)
	{
		if (*m_pPlayerInteract)
			Active();
	}	
	if (m_eState == CABINET_OPEN)
	{
		m_bObtain = true;
		return;
	}
	__super::Tick(fTimeDelta);
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
			if(it != nullptr)
				it->Set_Render(true);
		}

		m_bRender = false;
	}
	if (Activate_Col(Get_Collider_World_Pos(_float4(50.f, 1.f, 50.f,1.f)))|| Activate_Col(Get_Collider_World_Pos(_float4(-50.f, 1.f, 50.f, 1.f))))
	{
		if (Check_Col_Player(INTER_COL_NORMAL, COL_STEP0))
			Check_Col_Player(INTER_COL_NORMAL, COL_STEP1);
		else
			m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
		
		if(m_bReonDesk)
			if (Check_Col_Player(INTER_COL_DOUBLE, COL_STEP0))
			{
				Check_Col_Player(INTER_COL_DOUBLE, COL_STEP1);
			}
			else
				m_bCol[INTER_COL_DOUBLE][COL_STEP1] = false;

	}
	else
	{
		m_bCol[INTER_COL_NORMAL][COL_STEP0] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
		m_bCol[INTER_COL_NORMAL][COL_STEP2] = false;
		if (m_bReonDesk)
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

	ColliderDesc.fRadius = _float(80.f);
	ColliderDesc.vCenter = _float3(50.f, 1.f, 50.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Normal_Step0"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP0], &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.fRadius = _float(50.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Normal_Step1"), (CComponent**)&m_pColliderCom[INTER_COL_NORMAL][COL_STEP1], &ColliderDesc)))
		return E_FAIL;

	if (m_tagPropDesc.strGamePrototypeName.find("reon") != string::npos)
	{
		ColliderDesc.fRadius = _float(80.f);
		ColliderDesc.vCenter = _float3(-50.f, 1.f, 50.f);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Double_Step0"), (CComponent**)&m_pColliderCom[INTER_COL_DOUBLE][COL_STEP0], &ColliderDesc)))
			return E_FAIL;

		ColliderDesc.fRadius = _float(50.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider_Double_Step1"), (CComponent**)&m_pColliderCom[INTER_COL_DOUBLE][COL_STEP1], &ColliderDesc)))
			return E_FAIL;

		m_bReonDesk = true;
	}


	return S_OK;
}

HRESULT CCabinet::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CCabinet::PART_END);

	/*Part_Body*/
	CPartObject* pBodyObj = { nullptr };
	CBody_Cabinet::PART_INTERACTPROPS_DESC BodyDesc = {};
	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pState = &m_eState;
	BodyDesc.strModelComponentName = m_tagPropDesc.strModelComponent;
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
		ItemDesc.pState = &m_eState;
		ItemDesc.pObtain = &m_bObtain;
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
	m_PartObjects[CCabinet::PART_LOCK] = nullptr;



	return S_OK;
}

HRESULT CCabinet::Initialize_PartObjects()
{
	if (m_PartObjects[PART_ITEM] == nullptr)
		return S_OK;

	CModel* pBodyModel = { dynamic_cast<CModel*>(m_PartObjects[PART_BODY]->Get_Component(TEXT("Com_Body_Model"))) };
	//CModel* pItemModel = { dynamic_cast<CModel*>(m_PartObjects[PART_ITEM]->Get_Component(TEXT("Com_Body_Model"))) };

	CBody_ItemProp* pItem = dynamic_cast<CBody_ItemProp*>(m_PartObjects[PART_ITEM]);
	_float4x4* pCombinedMatrix = { const_cast<_float4x4*>(pBodyModel->Get_CombinedMatrix("ItemSet01")) };
	pItem->Set_Socket(pCombinedMatrix);



	return S_OK;
}

HRESULT CCabinet::Bind_ShaderResources()
{
	return S_OK;
}

void CCabinet::Active()
{
	*m_pPlayerInteract = false;
	m_bActivity = true;
	
	m_eState = CABINET_OPEN;
	if (m_bObtain)
		if (nullptr != m_PartObjects[PART_ITEM]&&!m_bItemDead)
			m_pPlayer->PickUp_Item(this);
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

	if(m_PartObjects[PART_LOCK] == nullptr)
		return static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos();
	else
		return static_cast<CPart_InteractProps*>(m_PartObjects[PART_LOCK])->Get_Pos();

}
