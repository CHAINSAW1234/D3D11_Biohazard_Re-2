#include "stdafx.h"

#include "Inventory_Slot.h"

CInventory_Slot::CInventory_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCustomize_UI{ pDevice , pContext }
{
}

CInventory_Slot::CInventory_Slot(const CInventory_Slot& rhs)
	: CCustomize_UI{ rhs }
{
}

HRESULT CInventory_Slot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInventory_Slot::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;

		/* For.Com_Texture */
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Filled"),
			TEXT("Com_FilledTexture"), (CComponent**)&m_pFilledTexture)))
			return E_FAIL;

		m_pEmptyTexture = m_pTextureCom;

	}

	return S_OK;
}

void CInventory_Slot::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	if (m_PriorisFilled != m_isFilled)
	{
		m_PriorisFilled = m_isFilled;

		if (false == m_isFilled)
		{
			m_pTextureCom = m_pEmptyTexture;
		}

		else
		{
			m_pTextureCom = m_pFilledTexture;
		}
	}

	__super::Tick(fTimeDelta);
}

void CInventory_Slot::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CInventory_Slot::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInventory_Slot::Change_Tool()
{
	return S_OK;
}

CInventory_Slot* CInventory_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInventory_Slot* pInstance = new CInventory_Slot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CInventory_Slot"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CInventory_Slot::Clone(void* pArg)
{
	CInventory_Slot* pInstance = new CInventory_Slot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CInventory_Slot"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInventory_Slot::Free()
{
	m_pTextureCom = m_pEmptyTexture;
	__super::Free();

	Safe_Release(m_pFilledTexture);
}
