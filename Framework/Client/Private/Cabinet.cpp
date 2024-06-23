#include "stdafx.h"
#include "Cabinet.h"
#include"Player.h"
#include "PxCollider.h"
#include "Bone.h"

//part-obj
#include"Body_Cabinet.h"
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


	return S_OK;
}

void CCabinet::Tick(_float fTimeDelta)
{
	__super::Check_Player();
	if (!m_bVisible)
	{
		m_pColliderCom[INTERACTPROPS_COL_SPHERE]->Tick(m_pTransformCom->Get_WorldMatrix());
		return;
	}
	if (m_eState == CABINET_OPEN && m_pModelCom->isFinished(0))
		return;

	if (m_bCol && !m_bActive)
	{
		//UI¶ç¿ì°í
		if (*m_pPlayerInteract)
			Active();
		m_bCol = false;
	}
	__super::Tick(fTimeDelta);

	m_pColliderCom[INTERACTPROPS_COL_SPHERE]->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CCabinet::Late_Tick(_float fTimeDelta)
{
	if (!Visible())
		return;
	if (m_bRender == false)
		return;
	__super::Late_Tick(fTimeDelta);
}

HRESULT CCabinet::Render()
{
	return S_OK;
}

HRESULT CCabinet::Add_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(60.f);
	ColliderDesc.vCenter = _float3(50.f, 1.f, 0.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom[INTERACTPROPS_COL_SPHERE], &ColliderDesc)))
		return E_FAIL;

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
	pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(m_tagPropDesc.strObjectPrototype, &BodyDesc));
	if (nullptr == pBodyObj)
		return E_FAIL;

	m_PartObjects[CCabinet::PART_BODY] = pBodyObj;

	/*Part_Item*/
	m_PartObjects[CCabinet::PART_ITEM] = nullptr;

	/*Part_Lock*/
	m_PartObjects[CCabinet::PART_LOCK] = nullptr;



	return S_OK;
}

HRESULT CCabinet::Initialize_PartObjects()
{

	return S_OK;
}

HRESULT CCabinet::Bind_ShaderResources()
{

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
	//	return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
	//	return E_FAIL;

	return S_OK;
}

void CCabinet::Active()
{
	*m_pPlayerInteract = false;
	m_bActive = true;
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
