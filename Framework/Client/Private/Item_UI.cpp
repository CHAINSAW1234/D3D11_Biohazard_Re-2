#include "stdafx.h"

#include "Item_UI.h"

CItem_UI::CItem_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCustomize_UI{ pDevice, pContext }
{
}

CItem_UI::CItem_UI(const CItem_UI& rhs)
	: CCustomize_UI{ rhs }
{
}

HRESULT CItem_UI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem_UI::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;

		m_isLoad = false;

		m_bDead = true;
	}

	return S_OK;
}

void CItem_UI::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	if (false == m_bCountable)
	{
		for (auto& iter : m_vecChildUI)
			iter->Set_Dead(true);
	}
	else
	{
		for (auto& iter : m_vecChildUI)
		{
			iter->Set_Dead(false);
			static_cast<CCustomize_UI*>(iter)->Set_Text(0, to_wstring(m_iItemCount));
		}
	}


	__super::Tick(fTimeDelta);
}

void CItem_UI::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CItem_UI::Render()
{
	__super::Render();

	return S_OK;
}

void CItem_UI::Reset_Item()
{
	m_bDead = true;

	m_isWorking = false;

	m_eItemNumber = ITEM_NUMBER_END;

	m_iTextureNum = static_cast<_uint>(m_eItemNumber);

	m_eInvenItemType = INVEN_ITEM_TYPE_END;
}

void CItem_UI::Set_Item(ITEM_NUMBER eItmeNum, INVEN_ITEM_TYPE eItmeType)
{
	m_eItemNumber = eItmeNum;
	m_iTextureNum = static_cast<_uint>(m_eItemNumber);

	m_eInvenItemType = eItmeType;
}

CItem_UI* CItem_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem_UI* pInstance = new CItem_UI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CItem_UI"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CItem_UI::Clone(void* pArg)
{
	CItem_UI* pInstance = new CItem_UI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CItem_UI"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_UI::Free()
{
	__super::Free();
}
