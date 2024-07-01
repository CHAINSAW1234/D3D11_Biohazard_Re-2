#include "stdafx.h"
#include "Interact_UI.h"
#include"Prop_Manager.h"

CInteract_UI::CInteract_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCustomize_UI{ pDevice, pContext }
{
}

CInteract_UI::CInteract_UI(const CInteract_UI& rhs)
	:/* m_pPropManager{ CProp_Manager::Get_Instance() },*/
	CCustomize_UI{ rhs }
{
}

HRESULT CInteract_UI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteract_UI::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;
	}

	return S_OK;
}

void CInteract_UI::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CInteract_UI::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CInteract_UI::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CInteract_UI::Free()
{
	__super::Free();

	CProp_Manager::Destroy_Instance();
}
