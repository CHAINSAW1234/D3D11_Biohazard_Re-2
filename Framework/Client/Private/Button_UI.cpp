#include "stdafx.h"

#include "Button_UI.h"

CButton_UI::CButton_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCustomize_UI{ pDevice , pContext }
{
}

CButton_UI::CButton_UI(const CButton_UI& rhs)
	: CCustomize_UI{ rhs }
{
}

HRESULT CButton_UI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CButton_UI::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;
	}

	return S_OK;
}

void CButton_UI::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (UP == m_pGameInstance->Get_KeyState(VK_LBUTTON) )
	{
		if (true == IsMouseHover())
		{
			m_isClicked = true;
		}
	}
}

void CButton_UI::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CButton_UI::Render()
{
	__super::Render();

	return S_OK;
}

CButton_UI* CButton_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CButton_UI* pInstance = new CButton_UI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CButton_UI"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CButton_UI::Clone(void* pArg)
{
	CButton_UI* pInstance = new CButton_UI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CButton_UI"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CButton_UI::Free()
{
}
