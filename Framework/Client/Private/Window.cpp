#include "stdafx.h"
#include "Window.h"
#include"Player.h"
#include"CustomCollider.h"
#include"Body_Window.h"

CWindow::CWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CWindow::CWindow(const CWindow& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CWindow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWindow::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	return S_OK;
}

void CWindow::Start()
{
	list<CGameObject*>* pCollider = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Jombie_Collider"));
	if (pCollider == nullptr)
		return;
	for (auto& iter : *pCollider)
	{
		if (m_pColliderCom[INTERACTPROPS_COL_SPHERE]->Intersect(static_cast<CCollider*>(iter->Get_Component(TEXT("Com_Collider")))))
		{
			// 내 인덱스 넣어주기
			_int* iNum = static_cast<CCustomCollider*>(iter)->Node_InteractProps();
			*iNum = m_tagPropDesc.iIndex;
		}
	}


}

void CWindow::Tick(_float fTimeDelta)
{
	__super::Check_Player();
	m_pColliderCom[INTERACTPROPS_COL_SPHERE]->Tick(m_pTransformCom->Get_WorldMatrix());
	
	if (!m_bVisible)
		return;

	if (m_pPlayer == nullptr)
		return;
	
	if (m_iHP[PART_BODY] <= 0)
		m_eState = WINDOW_BREAK;
	
	__super::Tick(fTimeDelta);
}

void CWindow::Late_Tick(_float fTimeDelta)
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
	//if (m_iHP[PART_PANNEL] <= 0)
	//	m_PartObjects[PART_BODY]->Set_Render(false);
	
	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom[INTERACTPROPS_COL_SPHERE]);
#endif
}

HRESULT CWindow::Render()
{
	return S_OK;
}

HRESULT CWindow::Add_Components()
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

HRESULT CWindow::Add_PartObjects()
{
	m_PartObjects.clear();
	m_PartObjects.resize(CWindow::PART_END);

	/*Part_Body*/
	CPartObject* pBodyObj = { nullptr };
	CBody_Window::PART_INTERACTPROPS_DESC BodyDesc = {};
	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pState = &m_eState;
	BodyDesc.strModelComponentName = m_tagPropDesc.strModelComponent;
	pBodyObj = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(m_tagPropDesc.strObjectPrototype, &BodyDesc));
	if (nullptr == pBodyObj)
		return E_FAIL;

	m_PartObjects[CWindow::PART_BODY] = pBodyObj;

	/*PART_PANNEL*/
	m_PartObjects[CWindow::PART_PANNEL] = nullptr;

	return S_OK;
}

HRESULT CWindow::Initialize_PartObjects()
{

	return S_OK;
}

HRESULT CWindow::Bind_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

void CWindow::Active()
{
	*m_pPlayerInteract = false;
	m_bActive = true;


	if (m_eState == WINDOW_STATIC)
		m_eState = WINDOW_BREAK;
	else
		m_eState = WINDOW_STATIC;

}

_float4 CWindow::Get_Object_Pos()
{
	if (!m_bBarrigateInstallable)
		return _float4(0.f, 0.f, 0.f, 1.f); // 
	return m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION)+_float4(0.f,50.f,0.f,0.f);
}

CWindow* CWindow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWindow* pInstance = new CWindow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CWindow"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CWindow::Clone(void* pArg)
{
	CWindow* pInstance = new CWindow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CWindow"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWindow::Free()
{
	__super::Free();

}
