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
		
		//Change_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Items"), TEXT("Com_DefaultTexture"), (CComponent**)&m_pTextureCom);

		Set_IsLoad(false);

		m_bDead = true;

		//m_isResolution = true;

		Convert_Resolution();
	}


	return S_OK;
}

void CItem_UI::Start()
{
	if (false == m_IsChild)
	{
		if (nullptr == m_vecChildUI[0] || nullptr == m_vecChildUI[1])
			return;

		m_mapPartUI.emplace(EQUIP_DISPLAY, static_cast<CCustomize_UI*>(m_vecChildUI[0]));//E써져있는거
		m_mapPartUI.emplace(COUNT_DISPLAY, static_cast<CCustomize_UI*>(m_vecChildUI[1]));//오른쪽아래 아이템 수량
		CGameObject* pHotkeyDisplay = static_cast<CCustomize_UI*>(m_vecChildUI[0])->Get_Child(0);
		m_mapPartUI.emplace(HOTKEY_DISPLAY, static_cast<CCustomize_UI*>(pHotkeyDisplay));//단축키 표시

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

		m_mapPartUI[HOTKEY_DISPLAY]->Get_TexBox(0)->Set_isTransformBase(false);
		m_mapPartUI[HOTKEY_DISPLAY]->Get_TexBox(0)->Set_FontColor(XMVectorSet(1.f, 1.f, 1.f, 1.f));



//#ifdef FHD
//		_float3 fTextConvertPos = m_mapPartUI[COUNT_DISPLAY]->Get_TexBox(0)->Get_Position_UI();
//		
//		_float2 fTextPos = { fTextConvertPos.x / (static_cast<_float>(g_iWinSizeX) / 1600.f) , fTextConvertPos.y / (static_cast<_float>(g_iWinSizeY) / 900.f) };
//
//		_float2 fTextPosDif = { fTextConvertPos.x - fTextPos.x, fTextConvertPos.y - fTextPos.y };
//		
//		m_mapPartUI[COUNT_DISPLAY]->Get_TexBox(0)->Move(_float3{ -fTextPosDif.x , -fTextPosDif.y, 0.f });
//#endif // FHD

	}

	__super::Tick(0.f);
}

void CItem_UI::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	if (false == m_IsChild)
	{
		if (m_iItemQuantity <= 0 && CONSUMABLE == m_eInvenItemType)
		{
			Reset_ItemUI();
			return;
		}

		m_iTextureNum = static_cast<_uint>(m_eItemNumber);

		switch (m_eInvenItemType)
		{
		case Client::HOTKEY:
		case Client::EQUIPABLE: {
			if (m_iItemQuantity < 10)
			{
				wstring Text = TEXT(" ") + to_wstring(m_iItemQuantity);
				static_cast<CCustomize_UI*>(m_mapPartUI[COUNT_DISPLAY])->Set_Text(0, Text);
			}
			else
				static_cast<CCustomize_UI*>(m_mapPartUI[COUNT_DISPLAY])->Set_Text(0, to_wstring(m_iItemQuantity));

			switch (m_eItemNumber)
			{
			case Client::HandGun: {
				if (15 == m_iItemQuantity)
				{
					static_cast<CCustomize_UI*>(m_mapPartUI[COUNT_DISPLAY])->Set_Text_Color(0, XMVectorSet(0.5f, 0.7f, 0.4f, 1.f));
				}
				else
				{
					static_cast<CCustomize_UI*>(m_mapPartUI[COUNT_DISPLAY])->Set_Text_Color(0, XMVectorSet(1.f, 1.f, 1.f, 1.f));
				}
				break;
			}

			case Client::ShotGun: {
				if (7 == m_iItemQuantity)
				{
					static_cast<CCustomize_UI*>(m_mapPartUI[COUNT_DISPLAY])->Set_Text_Color(0, XMVectorSet(0.5f, 0.7f, 0.4f, 1.f));
				}
				else
				{
					static_cast<CCustomize_UI*>(m_mapPartUI[COUNT_DISPLAY])->Set_Text_Color(0, XMVectorSet(1.f, 1.f, 1.f, 1.f));
				}

				break;
			}

			default:
				break;
			}
			break;
		}
			
		case Client::CONSUMABLE_EQUIPABLE: {
			static_cast<CCustomize_UI*>(m_mapPartUI[COUNT_DISPLAY])->Set_Text_Color(0, XMVectorSet(1.f, 1.f, 1.f, 1.f));
			if (m_iItemQuantity < 10)
			{
				wstring Text = TEXT(" ") + to_wstring(m_iItemQuantity);
				static_cast<CCustomize_UI*>(m_mapPartUI[COUNT_DISPLAY])->Set_Text(0, Text);
			}
			else
				static_cast<CCustomize_UI*>(m_mapPartUI[COUNT_DISPLAY])->Set_Text(0, to_wstring(m_iItemQuantity));
			break;
		}
			
		case Client::USEABLE: {
			break;
		}
			
		case Client::CONSUMABLE: {
			static_cast<CCustomize_UI*>(m_mapPartUI[COUNT_DISPLAY])->Set_Text_Color(0, XMVectorSet(1.f, 1.f, 1.f, 1.f));
			if (m_iItemQuantity < 10)
			{
				wstring Text = TEXT(" ") + to_wstring(m_iItemQuantity);
				static_cast<CCustomize_UI*>(m_mapPartUI[COUNT_DISPLAY])->Set_Text(0, Text);
			}
			else
				static_cast<CCustomize_UI*>(m_mapPartUI[COUNT_DISPLAY])->Set_Text(0, to_wstring(m_iItemQuantity));
			break;
		}

		case Client::QUEST: {
			static_cast<CCustomize_UI*>(m_mapPartUI[COUNT_DISPLAY])->Set_Text_Color(0, XMVectorSet(1.f, 1.f, 1.f, 1.f));
			if (m_iItemQuantity < 10)
			{
				wstring Text = TEXT(" ") + to_wstring(m_iItemQuantity);
				static_cast<CCustomize_UI*>(m_mapPartUI[COUNT_DISPLAY])->Set_Text(0, Text);
			}
			else
				static_cast<CCustomize_UI*>(m_mapPartUI[COUNT_DISPLAY])->Set_Text(0, to_wstring(m_iItemQuantity));
			break;
		}
						  
		//case Client::HOTKEY: {
		//	static_cast<CCustomize_UI*>(m_mapPartUI[COUNT_DISPLAY])->Set_Text_Color(0, XMVectorSet(1.f, 1.f, 1.f, 1.f));
		//	if (m_iItemQuantity < 10)
		//	{
		//		wstring Text = TEXT(" ") + to_wstring(m_iItemQuantity);
		//		static_cast<CCustomize_UI*>(m_mapPartUI[COUNT_DISPLAY])->Set_Text(0, Text);
		//	}
		//	else
		//		static_cast<CCustomize_UI*>(m_mapPartUI[COUNT_DISPLAY])->Set_Text(0, to_wstring(m_iItemQuantity));
		//	break;
		//}

			
		default:
			break;
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
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItem_UI::Change_Tool()
{
	return S_OK;
}

void CItem_UI::Set_Dead(_bool bDead)
{
	m_bDead = bDead;

	if (false == m_IsChild)
	{
		switch (m_eInvenItemType)
		{
		case Client::EQUIPABLE: {
			m_mapPartUI[EQUIP_DISPLAY]->CCustomize_UI::Set_Dead(bDead);
			m_mapPartUI[COUNT_DISPLAY]->CCustomize_UI::Set_Dead(bDead);
			if(m_isEquiped == true)
				m_mapPartUI[HOTKEY_DISPLAY]->CCustomize_UI::Set_Dead(true);
			else
				m_mapPartUI[HOTKEY_DISPLAY]->CCustomize_UI::Set_Dead(bDead);
			break;}

		case Client::CONSUMABLE_EQUIPABLE: {
			m_mapPartUI[EQUIP_DISPLAY]->CCustomize_UI::Set_Dead(bDead);
			m_mapPartUI[COUNT_DISPLAY]->CCustomize_UI::Set_Dead(bDead);
			m_mapPartUI[HOTKEY_DISPLAY]->CCustomize_UI::Set_Dead(bDead);
			break;
		}

		case Client::USEABLE: {
			m_mapPartUI[EQUIP_DISPLAY]->CCustomize_UI::Set_Dead(true);
			m_mapPartUI[COUNT_DISPLAY]->CCustomize_UI::Set_Dead(true);
			m_mapPartUI[HOTKEY_DISPLAY]->CCustomize_UI::Set_Dead(true);
			break;}

		case Client::CONSUMABLE: {
			m_mapPartUI[EQUIP_DISPLAY]->CCustomize_UI::Set_Dead(true);
			m_mapPartUI[COUNT_DISPLAY]->CCustomize_UI::Set_Dead(bDead);
			m_mapPartUI[HOTKEY_DISPLAY]->CCustomize_UI::Set_Dead(true);
			break;}

		case Client::QUEST: {
			m_mapPartUI[EQUIP_DISPLAY]->CCustomize_UI::Set_Dead(true);
			m_mapPartUI[COUNT_DISPLAY]->CCustomize_UI::Set_Dead(true);
			m_mapPartUI[HOTKEY_DISPLAY]->CCustomize_UI::Set_Dead(true);
			break;}

		case Client::DRAG_SHADOW: {
			m_mapPartUI[EQUIP_DISPLAY]->CCustomize_UI::Set_Dead(true);
			m_mapPartUI[COUNT_DISPLAY]->CCustomize_UI::Set_Dead(true);
			m_mapPartUI[HOTKEY_DISPLAY]->CCustomize_UI::Set_Dead(true);
			break;}

		case Client::HOTKEY: {
			m_mapPartUI[EQUIP_DISPLAY]->CCustomize_UI::Set_Dead(bDead);
			if (false == m_isHotKeyRegisted)
				m_mapPartUI[COUNT_DISPLAY]->CCustomize_UI::Set_Dead(true);
			else
				m_mapPartUI[COUNT_DISPLAY]->CCustomize_UI::Set_Dead(bDead);
			m_mapPartUI[HOTKEY_DISPLAY]->CCustomize_UI::Set_Dead(bDead);
			break;}

		default:
			break;
		}
	}
	
}

void CItem_UI::Set_Text(wstring Target, wstring strSetText)
{
	if (false == m_IsChild)
	{
		m_mapPartUI[Target]->Set_Text(0, strSetText);
	}
}

//void CItem_UI::Set_Resolution(_bool isResolution)
//{
//	m_isResolution = isResolution;
//
//	Convert_Resolution(true);
//
//	for (auto iter : m_vecChildUI)
//	{
//		static_cast<CItem_UI*>(iter)->Set_Resolution(isResolution);
//	}
//}

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

void CItem_UI::Set_Activ_ItemUI(_bool Active)
{
	if (false == Active)
	{
		m_isActive = false;
		Frame_Change_ValueColor(1);



		//m_mapPartUI[EQUIP_DISPLAY]->Frame_Change_ValueColor(1);
		m_mapPartUI[EQUIP_DISPLAY]->Set_Text_Color(0, XMVectorSet(0.6f, 0.6f, 0.6f, 1.f));
		m_mapPartUI[HOTKEY_DISPLAY]->Set_Text_Color(0, XMVectorSet(0.6f, 0.6f, 0.6f, 1.f));
		m_mapPartUI[COUNT_DISPLAY]->Set_Text_Color(0, XMVectorSet(0.6f, 0.6f, 0.6f, 1.f));
	}

	else
	{
		m_isActive = true;
		Frame_Change_ValueColor(0);

		//m_mapPartUI[EQUIP_DISPLAY]->Frame_Change_ValueColor(0);
		m_mapPartUI[EQUIP_DISPLAY]->Set_Text_Color(0, XMVectorSet(0.f, 0.f, 0.f, 1.f));
		m_mapPartUI[HOTKEY_DISPLAY]->Set_Text_Color(0, XMVectorSet(1.f, 1.f, 1.f, 1.f));
		m_mapPartUI[COUNT_DISPLAY]->Set_Text_Color(0, XMVectorSet(1.f, 1.f, 1.f, 1.f));
	}
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

	if (DRAG_SHADOW == m_eInvenItemType)
		Set_Value_Color(&m_vColor[1]);
}

void CItem_UI::Set_ItemUI_WinSize(ITEM_NUMBER eItmeNum, ITEM_TYPE eItmeType, _vector vSetPos, _int iVariation)
{
	//m_bDead = false;
	m_isWorking = true;
	m_eItemNumber = eItmeNum;
	m_iTextureNum = static_cast<_uint>(m_eItemNumber);
	m_eInvenItemType = eItmeType;

	Set_Position_Winsize(vSetPos);

	m_iItemQuantity = iVariation;

	if (DRAG_SHADOW == m_eInvenItemType)
		Set_Value_Color(&m_vColor[1]);
}

void CItem_UI::Set_PartUI_TextColor(wstring Target, _vector vTextColor)
{
	m_mapPartUI[Target]->Set_Text_Color(0, vTextColor);
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
