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
	if (true == m_bDead)
		return;

	//__super::Tick(fTimeDelta);

	//if (true == IsMouseHover())
	//{
	//	if(nullptr != m_vecChildUI[0])
	//		m_vecChildUI[0]->Set_Dead(false);
	//}
	//else
	//{
	//	if (nullptr != m_vecChildUI[0])
	//		m_vecChildUI[0]->Set_Dead(true);
	//}
}

void CButton_UI::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CButton_UI::Render()
{
	__super::Render();

	return S_OK;
}

void CButton_UI::FirstTick_Seting()
{

}

_bool CButton_UI::IsMouseHover()
{
	if (true == m_bClickable)
		return __super::IsMouseHover();

	return false;
}

_bool CButton_UI::IsMouseHover(_float& fPosZ)
{
	if (true == m_bClickable)
		return __super::IsMouseHover(fPosZ);

	return false;
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
	__super::Free();
}
