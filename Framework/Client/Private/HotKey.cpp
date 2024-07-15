#include "stdafx.h"

#include "Item_Discription.h"
#include "HotKey.h"

constexpr _uint		EQ_SLOT_COUNT = 4;
constexpr _uint		TH_SLOT_COUNT = 3;
constexpr _float	Z_POS_ITEM_UI = 0.007f;
constexpr _float	Z_POS_SLOT = 0.008f;
constexpr _float	Y_DEF = 154.f;
constexpr _float	LIFE_TIME_LIMIT = 0.5f;

CHotKey::CHotKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice , pContext }
{
}

CHotKey::CHotKey(const CHotKey& rhs)
	: CUI{ rhs }
{
}

HRESULT CHotKey::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHotKey::Initialize(void* pArg)
{
	CUI::UI_DESC UIDesc = Read_HotKeyDat();

	if (FAILED(__super::Initialize(&UIDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Init_InvenSlot()))
		return E_FAIL;

	if (FAILED(Init_ItemUI()))
		return E_FAIL;

	m_bDead = true;

	return S_OK;
}

void CHotKey::Start()
{

	for (_uint i = 0; i < EQ_SLOT_COUNT; i++)
	{
		m_fPopUp_Pos[i] = _float2(m_fPositions[i].x, m_fPositions[i].y - Y_DEF);

		m_pEQItemUI[i]->Set_Text(HOTKEY_DISPLAY, to_wstring(i + 1));
		m_pEQItemUI[i]->Set_PartUI_TextColor(HOTKEY_DISPLAY, XMVectorSet(1.f, 1.f, 1.f, 1.f));
		m_pEQItemUI[i]->Set_ItemUI(ITEM_NUMBER_END, HOTKEY, XMVectorSet(m_fPositions[i].x, m_fPositions[i].y, Z_POS_ITEM_UI, 1.f), 0);
	}
}

void CHotKey::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	if (true == m_isPopUp)
	{
		m_isAlphaControl = true;
		m_fLifeTime += fTimeDelta;
		if (m_fLifeTime > LIFE_TIME_LIMIT)
		{
			m_fAlpha -= 0.2f;
			if (m_fAlpha < 0.f)
			{
				Set_Dead(true);
				m_fAlpha = 1.f;
				m_fLifeTime = 0.f;
				m_isAlphaControl = false;
			}
		}
	}


}

void CHotKey::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CHotKey::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CHotKey::Set_Dead(_bool bDead)
{
	m_bDead = bDead;

	for (_uint i = 0; i < EQ_SLOT_COUNT; i++)
	{
		m_pEQInven_Slots[i]->Set_Dead(bDead);
		m_pEQItemUI[i]->Set_Dead(bDead);
	}

	m_isPopUp = false;
}

void CHotKey::PopUp_Call()
{
	Set_Dead(false);
	m_isPopUp = true;
}

CInventory_Slot* CHotKey::Get_Hoverd_Slot()
{
	for (_uint i = 0; i < EQ_SLOT_COUNT; i++)
	{
		if (true == m_pEQInven_Slots[i]->IsMouseHover())
			return m_pEQInven_Slots[i];
	}

	return nullptr;
}

CInventory_Slot* CHotKey::Get_Empty_Slot()
{
	for (_uint i = 0; i < EQ_SLOT_COUNT; i++)
	{
		if (false == m_pEQInven_Slots[i]->Get_IsFilled())
			return m_pEQInven_Slots[i];
	}

	return nullptr;
}

_uint CHotKey::RegisterHoykey(_float2 RegisterPos, ITEM_NUMBER eItemNum, _int iItemQuantity)
{
	for (_uint i = 0; i < EQ_SLOT_COUNT; i++)
	{
		if (eItemNum == m_pEQItemUI[i]->Get_ItemNumber())
		{
			m_pEQItemUI[i]->Set_ItemNumber(ITEM_NUMBER_END);
			m_pEQItemUI[i]->Set_ItemQuantity(0);
			m_pEQItemUI[i]->Set_isHotKeyRegisted(false);
			m_pEQItemUI[i]->Set_Dead(false);

			m_pEQInven_Slots[i]->Set_IsFilled(false);
		}

		if (true == m_pEQItemUI[i]->IsPTInRect(RegisterPos))
		{
			m_pEQItemUI[i]->Set_ItemNumber(eItemNum);
			m_pEQItemUI[i]->Set_ItemQuantity(iItemQuantity);
			m_pEQItemUI[i]->Set_isHotKeyRegisted(true);
			m_pEQItemUI[i]->Set_Dead(false);
		}
	}

	for (_uint i = 0; i < EQ_SLOT_COUNT; i++)
	{
		if (true == m_pEQInven_Slots[i]->IsPTInRect(RegisterPos))
		{
			m_pEQInven_Slots[i]->Set_IsFilled(true);
			return i+1;
		}
	}

	return 0;
}

_uint CHotKey::RegisterHoykey(_uint iHotkeyNum, ITEM_NUMBER eItemNum, _int iItemQuantity)
{
	m_pEQItemUI[iHotkeyNum]->Set_ItemNumber(eItemNum);
	m_pEQItemUI[iHotkeyNum]->Set_ItemQuantity(iItemQuantity);
	m_pEQItemUI[iHotkeyNum]->Set_isHotKeyRegisted(true);
	m_pEQInven_Slots[iHotkeyNum]->Set_IsFilled(true);

	return iHotkeyNum + 1;
}


void CHotKey::Update_Registed_Item(ITEM_NUMBER eItemNum, _int iItemQuantity)
{
	for (_uint i = 0; i < EQ_SLOT_COUNT; i++)
	{
		if (eItemNum == m_pEQItemUI[i]->Get_ItemNumber())
		{
			m_pEQItemUI[i]->Set_ItemQuantity(iItemQuantity);
		}
	}
}

ITEM_NUMBER CHotKey::Get_Item_On_HotKey(_uint iHotKeyNum)
{
	return m_pEQItemUI[iHotKeyNum]->Get_ItemNumber();
}

HRESULT CHotKey::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isAlphaControl", &m_isAlphaControl, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

CUI::UI_DESC CHotKey::Read_HotKeyDat()
{
	ifstream inputFileStream;
	wstring selectedFilePath;
	selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/HotKey.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	vector<CCustomize_UI::CUSTOM_UI_DESC> vecCustomInvenUIDesc;
	CCustomize_UI::ExtractData_FromDat(inputFileStream, &vecCustomInvenUIDesc, true);

	CUI::UI_DESC UIDesc = {};
	//UIDesc.vPos = { vecCustomInvenUIDesc[0].worldMatrix._41, vecCustomInvenUIDesc[0].worldMatrix._42, vecCustomInvenUIDesc[0].worldMatrix._43, 1.f };

	//UIDesc.vSize.x = sqrt(vecCustomInvenUIDesc[0].worldMatrix._11 * vecCustomInvenUIDesc[0].worldMatrix._11 +
	//	vecCustomInvenUIDesc[0].worldMatrix._12 * vecCustomInvenUIDesc[0].worldMatrix._12 + 
	//	vecCustomInvenUIDesc[0].worldMatrix._13 * vecCustomInvenUIDesc[0].worldMatrix._13);

	//UIDesc.vSize.y =sqrt(vecCustomInvenUIDesc[0].worldMatrix._21 * vecCustomInvenUIDesc[0].worldMatrix._21 +
	//	vecCustomInvenUIDesc[0].worldMatrix._22 * vecCustomInvenUIDesc[0].worldMatrix._22 + 
	//	vecCustomInvenUIDesc[0].worldMatrix._23 * vecCustomInvenUIDesc[0].worldMatrix._23);

	UIDesc.worldMatrix = vecCustomInvenUIDesc[0].worldMatrix;

	for (_uint i = 1; i < 5; i++)
	{
		m_fPositions[i - 1] = { vecCustomInvenUIDesc[i].worldMatrix._41, vecCustomInvenUIDesc[i].worldMatrix._42, Z_POS_ITEM_UI };
	}

	return UIDesc;
}

HRESULT CHotKey::Init_InvenSlot()
{
	ifstream inputFileStream;
	wstring selectedFilePath;
	selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/VoidSlot.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	vector<CCustomize_UI::CUSTOM_UI_DESC> vecCustomInvenUIDesc;
	CCustomize_UI::ExtractData_FromDat(inputFileStream, &vecCustomInvenUIDesc, true);

	for (_uint i = 0; i < EQ_SLOT_COUNT; i++)
	{
		if (FAILED(Create_InvenSlot(&vecCustomInvenUIDesc, m_fPositions[i], i, EQUIPPED)))
			return E_FAIL;
	}

	for (_uint i = 0; i < TH_SLOT_COUNT; i++)
	{
		if (FAILED(Create_InvenSlot(&vecCustomInvenUIDesc, m_fPositions[i], i, THROWN)))
			return E_FAIL;
	}

	for (_uint i = 0; i < EQ_SLOT_COUNT; i++)
	{
		Safe_AddRef(m_pEQInven_Slots[i]);
	}

	for (_uint i = 0; i < TH_SLOT_COUNT; i++)
	{
		Safe_AddRef(m_pTHInven_Slots[i]);
	}

	return S_OK;
}

HRESULT CHotKey::Create_InvenSlot(vector<CCustomize_UI::CUSTOM_UI_DESC>* vecInvenUI, _float2 fSetPos, _uint iCount, WEAPON_CLASSIFY eWC)
{
	CInventory_Slot* pParentInvenUI = { nullptr };

	for (auto& iter : *vecInvenUI)
	{
		if (0 == iter.fMaxFrame && TEXT("") != iter.wstrDefaultTexturPath)
		{
			/* For.Prototype_Component_Texture_ */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, iter.wstrDefaultTexturComTag,
				CTexture::Create(m_pDevice, m_pContext, iter.wstrDefaultTexturPath)))) {
				int a = 0;
			}
		}

		else if (0 < iter.fMaxFrame && TEXT("") != iter.wstrDefaultTexturPath)
		{
			/* For.Prototype_Component_Texture_ */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, iter.wstrDefaultTexturComTag,
				CTexture::Create(m_pDevice, m_pContext, iter.wstrDefaultTexturPath, (_uint)iter.fMaxFrame)))) {
				int a = 0;
			}
		}

		if (TEXT("") != iter.wstrMaskPath)
		{
			/* For.Prototype_Component_Texture_ */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, iter.wstrMaskComTag,
				CTexture::Create(m_pDevice, m_pContext, iter.wstrMaskPath)))) {
				int a = 0;
			}
		}

		//CGameObject* pGameOBJ = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Inventory_Item_UI"), &iter);
		//if (nullptr == pGameOBJ)
		//	return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_InventorySlot"), &iter)))
			return E_FAIL;

		CGameObject* pGameObj = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"))->back();

		CInventory_Slot* pInvenUI = dynamic_cast<CInventory_Slot*>(pGameObj);

		pInvenUI->Set_Position(XMVectorSet(fSetPos.x, fSetPos.y, Z_POS_SLOT, 1.f));
		pInvenUI->Set_IsLoad(false);
		pInvenUI->Set_Dead(true);

		if (nullptr != pParentInvenUI)
		{
			pParentInvenUI->PushBack_Child(pGameObj);
			pParentInvenUI = nullptr;
		}

		else
		{
			switch (eWC)
			{
			case Client::CHotKey::EQUIPPED:
				m_pEQInven_Slots[iCount] = pInvenUI;
				break;
			case Client::CHotKey::THROWN:
				m_pTHInven_Slots[iCount] = pInvenUI;
				break;

			default:
				break;
			}

		}

		pParentInvenUI = pInvenUI;
	}

	return S_OK;
}

HRESULT CHotKey::Init_ItemUI()
{
	for (_uint i = 0; i < EQ_SLOT_COUNT; i++)
	{
		ifstream inputFileStream;
		wstring selectedFilePath;
		selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/Item_UI.dat");
		inputFileStream.open(selectedFilePath, ios::binary);
		CItem_UI* pItemUI = { nullptr };
		CCustomize_UI::CreatUI_FromDat(inputFileStream, nullptr, TEXT("Prototype_GameObject_ItemUI"),
			(CGameObject**)&pItemUI, m_pDevice, m_pContext);
		m_pEQItemUI[i] = pItemUI;

		Safe_AddRef(m_pEQItemUI[i]);
	}

	for (_uint i = 0; i < TH_SLOT_COUNT; i++)
	{
		ifstream inputFileStream;
		wstring selectedFilePath;
		selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/Item_UI.dat");
		inputFileStream.open(selectedFilePath, ios::binary);
		CItem_UI* pItemUI = { nullptr };
		CCustomize_UI::CreatUI_FromDat(inputFileStream, nullptr, TEXT("Prototype_GameObject_ItemUI"),
			(CGameObject**)&pItemUI, m_pDevice, m_pContext);
		m_pTHItemUI[i] = pItemUI;

		Safe_AddRef(m_pTHItemUI[i]);
	}

	return S_OK;
}

HRESULT CHotKey::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAlphaSortTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WholeMouse"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

CHotKey* CHotKey::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHotKey* pInstance = new CHotKey(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CHotKey"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHotKey::Clone(void* pArg)
{
	CHotKey* pInstance = new CHotKey(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHotKey"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHotKey::Free()
{
	__super::Free();

	for (_uint i = 0; i < EQ_SLOT_COUNT; i++)
	{
		Safe_Release(m_pEQInven_Slots[i]);
		Safe_Release(m_pEQItemUI[i]);
	}

	for (_uint i = 0; i < TH_SLOT_COUNT; i++)
	{
		Safe_Release(m_pTHInven_Slots[i]);
		Safe_Release(m_pTHItemUI[i]);
	}

}
