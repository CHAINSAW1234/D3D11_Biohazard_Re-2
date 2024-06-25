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

	if (m_iItemQuantity <= 0)
	{
		Reset_ItemUI();
		return;
	}

	switch (m_eInvenItemType)
	{
	case Client::EQUIPABLE:
		break;
	case Client::CONSUMABLE_EQUIPABLE:
		break;
	case Client::USEABLE:
		break;
	case Client::CONSUMABLE: {
		static_cast<CCustomize_UI*>(m_mapPartUI[TEXT("CountDisplay")])->Set_Text(0, to_wstring(m_iItemQuantity));
		break;
	}
		
	case Client::QUEST:
		break;
	case Client::INVEN_ITEM_TYPE_END:
		break;
	default:
		break;
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
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CItem_UI::FirstTick_Seting()
{
	if (false == m_IsChild)
	{
		if (nullptr == m_vecChildUI[0] || nullptr == m_vecChildUI[1])
			return;

		m_mapPartUI.emplace(TEXT("EquipDisplay"), m_vecChildUI[0]);
		m_mapPartUI.emplace(TEXT("CountDisplay"), m_vecChildUI[1]);


		for (auto& iter : *(static_cast<CItem_UI*>(m_vecChildUI[0])->Get_vecTextBoxes()))
		{
			iter->Set_isTransformBase(false);
			iter->Set_isUIRender(true);
		}

		for (auto& iter : *(static_cast<CItem_UI*>(m_vecChildUI[1])->Get_vecTextBoxes()))
		{
			iter->Set_isTransformBase(false);
			iter->Set_isUIRender(true);
		}
	}
}

void CItem_UI::Set_Dead(_bool bDead)
{
	m_bDead = bDead;

	switch (m_eInvenItemType)
	{
	case Client::EQUIPABLE: {
		m_mapPartUI[TEXT("EquipDisplay")]->Set_Dead(bDead);
		m_mapPartUI[TEXT("CountDisplay")]->Set_Dead(bDead);
		break;
	}

	case Client::CONSUMABLE_EQUIPABLE: {
		m_mapPartUI[TEXT("EquipDisplay")]->Set_Dead(bDead);
		m_mapPartUI[TEXT("CountDisplay")]->Set_Dead(bDead);
		break;
	}
				
	case Client::USEABLE: {
		m_mapPartUI[TEXT("EquipDisplay")]->Set_Dead(true);
		m_mapPartUI[TEXT("CountDisplay")]->Set_Dead(true);
		break;
	}
		
	case Client::CONSUMABLE: {
		m_mapPartUI[TEXT("EquipDisplay")]->Set_Dead(true);
		m_mapPartUI[TEXT("CountDisplay")]->Set_Dead(bDead);
		break;
	}
		
	case Client::QUEST: {
		m_mapPartUI[TEXT("EquipDisplay")]->Set_Dead(true);
		m_mapPartUI[TEXT("CountDisplay")]->Set_Dead(true);
		break;
	}
		
	default:
		break;
	}
}

void CItem_UI::Reset_ItemUI()
{
	m_bDead = true;
	m_isWorking = false;
	m_eItemNumber = ITEM_NUMBER_END;
	m_iTextureNum = static_cast<_uint>(m_eItemNumber);
	m_eInvenItemType = INVEN_ITEM_TYPE_END;
	m_iItemQuantity = 0;
	for (auto& iter : m_vecChildUI)
		static_cast<CCustomize_UI*>(iter)->Set_Dead(true);
	
}

void CItem_UI::Set_ItemUI(ITEM_NUMBER eItmeNum, ITEM_TYPE eItmeType, _vector vSetPos, _int iVariation)
{
	//m_bDead = false;
	m_isWorking = true;
	m_eItemNumber = eItmeNum;
	m_iTextureNum = static_cast<_uint>(m_eItemNumber);
	m_eInvenItemType = eItmeType;

	Set_Position(vSetPos);

	m_iItemQuantity = iVariation;
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
	m_mapPartUI.clear();

	__super::Free();
}
