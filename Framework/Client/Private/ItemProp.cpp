#include "stdafx.h"
#include "ItemProp.h"
#include"Player.h"

#include "Selector_UI.h"
#include"Body_ItemProp.h"


CItemProp::CItemProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CItemProp::CItemProp(const CItemProp& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CItemProp::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItemProp::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (!m_tagPropDesc.bAnim)
		m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);

	return S_OK;
}

void CItemProp::Tick(_float fTimeDelta)
{
	for (size_t i = 0; i < INTER_COL_END; i++)
	{
		for (size_t j = 0; j < COL_STEP_END; j++)
		{
			if (m_pColliderCom[i][j] == nullptr)
				continue;
			_float4x4 Translation = m_pTransformCom->Get_WorldFloat4x4();
			if (m_pTransformCom->Get_WorldFloat4x4().Translation().y > -1.5f && m_pTransformCom->Get_WorldFloat4x4().Translation().y < 3.2f)
				Translation.m[3][1] = 0.f;
			else if(m_pTransformCom->Get_WorldFloat4x4().Translation().y < 8.f)
				Translation.m[3][1] = 5.f;
			else if(m_pTransformCom->Get_WorldFloat4x4().Translation().y < 15.f)
				Translation.m[3][1] = 10.f;
			m_pColliderCom[i][j]->Tick(Translation);
		}
	}
	if (m_bDead)
	{
		m_bItemDead = true;

		return;
	}

	if (!m_bVisible)
	{

		return;
	}
//
//#ifdef _DEBUG
//#ifdef UI_POS
//	Get_Object_Pos();
//#endif
//#endif

	if (m_bCol[INTER_COL_NORMAL][COL_STEP1])
	{
		//UI����
		if (*m_pPlayerInteract && false == m_pGameInstance->IsPaused())
		{
			Active();
		}
	}
	
	__super::Tick(fTimeDelta);
}

void CItemProp::Late_Tick(_float fTimeDelta)
{
	if (m_pPlayer == nullptr)
		return;

	if (m_bDead)
	{
		if (nullptr != m_pSelector)
		{
			m_pSelector = static_cast<CSelector_UI*>(m_pSelector->Destroy_Selector());

			m_pSelector = nullptr;
		}

		return;
	}

	if (!Visible())
	{
		Select_UI();

		return;
	}

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
	if (!m_bItemDead)
	{
		if (Activate_Col(Get_Collider_World_Pos(_float4(0.f, -50.f, 0.f, 1.f))))
			if (Check_Col_Player(INTER_COL_NORMAL, COL_STEP0))
			{
				m_pPlayer->Set_Tutorial_Start(UI_TUTORIAL_TYPE::INVENTORY_OPEN);

				Check_Col_Player(INTER_COL_NORMAL, COL_STEP1);

			}
			else
			{
				m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;
			

			}
		else
		{
			m_bCol[INTER_COL_NORMAL][COL_STEP0] = false;
			m_bCol[INTER_COL_NORMAL][COL_STEP1] = false;

		}
	}
	
	Select_UI();

	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUG
	__super::Add_Col_DebugCom();
#endif
}

HRESULT CItemProp::Render()
{
	return S_OK;
}

HRESULT CItemProp::Add_Components()
{


	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(120.f);
	

	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);

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

HRESULT CItemProp::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CItemProp::PART_END);

	/*Part_Body*/
	CPartObject* pBodyObj = { nullptr };
	CBody_ItemProp::BODY_ITEMPROPS_DESC BodyDesc = {};
	BodyDesc.pSoundCueSign = &m_bSoundCueSign;
	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pState = &m_eState;
	BodyDesc.pItemDead = &m_bItemDead;
	m_iItemIndex  = BodyDesc.iItemIndex = m_tagPropDesc.tagItemDesc.iItemIndex;
	BodyDesc.strModelComponentName = TEXT("Prototype_Component_Model_") + m_tagPropDesc.tagItemDesc.Name;
	pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_") + m_tagPropDesc.tagItemDesc.Name, &BodyDesc));
	if (nullptr == pBodyObj)
		return E_FAIL;

	m_PartObjects[CItemProp::PART_BODY] = pBodyObj;

	return S_OK;
}

HRESULT CItemProp::Initialize_PartObjects()
{

	return S_OK;
}

HRESULT CItemProp::Bind_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

_float4 CItemProp::Get_Object_Pos()
{
	//if (!m_bDead)
	//	return static_cast<CPart_InteractProps*>(m_PartObjects[PART_BODY])->Get_Pos();
	//else
	//	return _float4();

	if (!m_bDead)
		return m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
	else
		return  _float4();
}

void CItemProp::Active()
{
	*m_pPlayerInteract = false;
	m_pPlayer->PickUp_Item(this);
}

CItemProp* CItemProp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItemProp* pInstance = new CItemProp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CItemProp"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CItemProp::Clone(void* pArg)
{
	CItemProp* pInstance = new CItemProp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CItemProp"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItemProp::Free()
{
	__super::Free();

}
