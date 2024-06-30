#include "stdafx.h"
#include "ReaderMachine.h"
#include"Player.h"

#include"Body_Statue.h"

CReaderMachine::CReaderMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CReaderMachine::CReaderMachine(const CReaderMachine& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CReaderMachine::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CReaderMachine::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	return S_OK;
}

void CReaderMachine::Tick(_float fTimeDelta)
{
	__super::Check_Player();
	if (!m_bVisible)
	{
		m_pColliderCom[INTERACTPROPS_COL_SPHERE]->Tick(m_pTransformCom->Get_WorldMatrix());
		return;
	}

	if (m_pPlayer == nullptr)
		return;
	__super::Tick(fTimeDelta);
	m_pColliderCom[INTERACTPROPS_COL_SPHERE]->Tick(m_pTransformCom->Get_WorldMatrix());

}

void CReaderMachine::Late_Tick(_float fTimeDelta)
{
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
	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom[INTERACTPROPS_COL_SPHERE]);
#endif
}

HRESULT CReaderMachine::Render()
{
	return S_OK;
}

HRESULT CReaderMachine::Add_Components()
{
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(100.f);
	ColliderDesc.vCenter = _float3(-10.f, 1.f, 0.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom[INTERACTPROPS_COL_SPHERE], &ColliderDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CReaderMachine::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CReaderMachine::PART_END);

	/*Part_Body*/
	CPartObject* pBodyObj = { nullptr };
	CBody_Statue::PART_INTERACTPROPS_DESC BodyDesc = {};
	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pState = &m_eState;
	BodyDesc.strModelComponentName = m_tagPropDesc.strModelComponent;
	pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(m_tagPropDesc.strObjectPrototype, &BodyDesc));
	if (nullptr == pBodyObj)
		return E_FAIL;

	m_PartObjects[CReaderMachine::PART_BODY] = pBodyObj;

	/*PART_PART*/
	m_PartObjects[CReaderMachine::PART_PART] = nullptr;


	return S_OK;
}

HRESULT CReaderMachine::Initialize_PartObjects()
{

	return S_OK;
}

HRESULT CReaderMachine::Bind_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

void CReaderMachine::Active()
{
	*m_pPlayerInteract = false;
	m_bActive = true;

}

_float4 CReaderMachine::Get_Object_Pos()
{
	return _float4();
}

CReaderMachine* CReaderMachine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CReaderMachine* pInstance = new CReaderMachine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CReaderMachine"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CReaderMachine::Clone(void* pArg)
{
	CReaderMachine* pInstance = new CReaderMachine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CReaderMachine"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CReaderMachine::Free()
{
	__super::Free();

}
