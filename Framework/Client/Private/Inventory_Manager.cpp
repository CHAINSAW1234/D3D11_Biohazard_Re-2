#include "stdafx.h"

#include "Inventory_Manager.h"

CInventory_Manager::CInventory_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CInventory_Manager::Initialize()
{
	if (FAILED(Init_InvenSlot()))
		return E_FAIL;

	if (FAILED(Init_SlotHighlighter()))
		return E_FAIL;

	if (FAILED(Init_ItemUI()))
		return E_FAIL;

	return S_OK;
}

void CInventory_Manager::FirstTick_Seting()
{
	m_pSlotHighlighter->FirstTick_Seting();
	m_pSlotHighlighter->ResetPosition(m_fSlotHighlighterResetPos);
}

void CInventory_Manager::Tick(_float fTimeDelta)
{
	m_IsNoOneHover = true;
	CInventory_Slot* pHoveredObj = nullptr;

	for (_uint i = 0; i < m_iInvenCount; i++)
	{
		if (true == m_vecInvenSlot[i]->IsMouseHover())
		{
			m_IsNoOneHover = false;
			pHoveredObj = m_vecInvenSlot[i];
		}
	}

	m_pSlotHighlighter->Set_Dead(m_IsNoOneHover);

	if (false == m_IsNoOneHover)
	{
		_float4 HoveredPos = dynamic_cast<CTransform*>(pHoveredObj->Get_Component(g_strTransformTag))->Get_State_Float4(CTransform::STATE_POSITION);
		HoveredPos.z = 0.7f;
		m_pSlotHighlighterTransform->Set_State(CTransform::STATE_POSITION, HoveredPos);
	}
}

void CInventory_Manager::Late_Tick(_float fTimeDelta)
{

}

void CInventory_Manager::Set_OnOff_Inven(_bool bInput)
{
	for (_uint i = 0; i < m_iInvenCount; i++)
	{
		m_vecInvenSlot[i] -> Set_Dead(bInput);
	}

	m_pSlotHighlighter->Set_Dead(bInput);

	for (auto& iter : m_vecItem_UI)
	{
		//≈«√¢≈≥∂ß ¿œ«œ∞Ì ¿÷¥¬≥∏∏ ƒ—¡‡æﬂ«‘
		if (false == bInput)
		{
			if (true == iter->Get_isWorking())
				iter->Set_Dead(bInput);
		}

		//≤¯∂© ¥Ÿ≤Ù¿⁄
		else
		{
			iter->Set_Dead(bInput);
		}
	}

	m_bisItemExamin = false;
}

_bool CInventory_Manager::AddItem_ToInven(ITEM_NUMBER eAcquiredItem)
{
	for (auto& iter : m_vecItem_UI)
	{

	}

	return true;
}

_bool CInventory_Manager::IsCan_AddItem_ToInven()
{
	_bool IsCanAddItem = false;

	for (auto& iter : m_vecItem_UI)
	{
		if (false == iter->Get_isWorking())
		{
			IsCanAddItem = true;
		}
	}

	return IsCanAddItem;
}

CInventory_Manager* CInventory_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInventory_Manager* pInstance = new CInventory_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CInventory_Manager"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CInventory_Manager::Init_InvenSlot()
{
	ifstream inputFileStream;
	wstring selectedFilePath;

#pragma region ¿Œ∫•≈‰∏Æ ΩΩ∑‘
	selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/VoidSlot.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	vector<CCustomize_UI::CUSTOM_UI_DESC> vecCustomInvenUIDesc;
	CCustomize_UI::ExtractData_FromDat(inputFileStream, &vecCustomInvenUIDesc, true);

	for (_uint i = 0; i < 5; i++)
	{
		for (_uint j = 0; j < 4; j++)
		{
			_float fPosX = m_fSlotInterval.x * j;
			_float fPosY = -m_fSlotInterval.y * i;
			if (FAILED(Create_InvenSlot(&vecCustomInvenUIDesc, _float3(fPosX, fPosY, 0.f))))
				return E_FAIL;
		}
	}

	for (auto& iter : m_vecInvenSlot)
	{
		if (nullptr != iter)
		{
			Safe_AddRef(iter);
			iter->Set_Dead(true);
		}
	}
#pragma endregion

	return S_OK;
}

HRESULT CInventory_Manager::Init_SlotHighlighter()
{
	ifstream inputFileStream;
	wstring selectedFilePath;

#pragma region ΩΩ∑‘ «œ¿Ã∂Û¿Ã≈Õ
	selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/SlotHighlighter.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	vector<CCustomize_UI::CUSTOM_UI_DESC> vecCustomSelecUIDesc;
	CCustomize_UI::ExtractData_FromDat(inputFileStream, &vecCustomSelecUIDesc, true);
	if (FAILED(Create_SlotHighlighter(&vecCustomSelecUIDesc)))
		return E_FAIL;

	m_pSlotHighlighter->Set_Dead(true);
	m_pSlotHighlighterTransform = dynamic_cast<CTransform*>(m_pSlotHighlighter->Get_Component(g_strTransformTag));
	m_fSlotHighlighterResetPos = m_vecInvenSlot[0]->GetPosition();
	m_fSlotHighlighterResetPos.z = 0.7f;

	Safe_AddRef(m_pSlotHighlighter);
	Safe_AddRef(m_pSlotHighlighterTransform);
#pragma endregion

	return S_OK;
}

HRESULT CInventory_Manager::Init_ItemUI()
{
	ifstream inputFileStream;
	wstring selectedFilePath;

#pragma region ItemUI
	selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/Item_UI.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	vector<CCustomize_UI::CUSTOM_UI_DESC> vecItemUIDesc;
	CCustomize_UI::ExtractData_FromDat(inputFileStream, &vecItemUIDesc, true);

	for (_uint i = 0; i < 4; i++)
	{
		if (FAILED(Create_ItemUI(&vecItemUIDesc)))
			return E_FAIL;
	}

	for (auto& iter : m_vecItem_UI)
	{
		if (nullptr != iter)
		{
			Safe_AddRef(iter);
			iter->Set_Dead(true);
		}
	}
#pragma endregion

	return S_OK;
}

HRESULT CInventory_Manager::Create_InvenSlot(vector<CCustomize_UI::CUSTOM_UI_DESC>* vecInvenUI, _float3 fInterval)
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

		pInvenUI->Move_State(fInterval, 0);
		pInvenUI->Set_IsLoad(false);
		pInvenUI->Set_Dead(true);

		if (nullptr != pParentInvenUI)
		{
			pParentInvenUI->PushBack_Child(pGameObj);
			pParentInvenUI = nullptr;
		}

		else
		{
			m_vecInvenSlot.push_back(pInvenUI);
		}

		pParentInvenUI = pInvenUI;
	}

	return S_OK;
}

HRESULT CInventory_Manager::Create_SlotHighlighter(vector<CCustomize_UI::CUSTOM_UI_DESC>* vecInvenUI)
{
	CSlot_Highlighter* pParentInvenUI = { nullptr };

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

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_SlotHighlighter"), &iter)))
			return E_FAIL;

		CGameObject* pGameObj = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"))->back();

		CSlot_Highlighter* pInvenUI = dynamic_cast<CSlot_Highlighter*>(pGameObj);

		pInvenUI->Set_Dead(true);

		if (nullptr != pParentInvenUI)
		{
			pParentInvenUI->PushBack_Child(pGameObj);
			pParentInvenUI = nullptr;
		}

		else
		{
			m_pSlotHighlighter = pInvenUI;
		}

		pParentInvenUI = pInvenUI;
	}

	return S_OK;
}

HRESULT CInventory_Manager::Create_ItemUI(vector<CCustomize_UI::CUSTOM_UI_DESC>* vecInvenUI)
{
	CItem_UI* pParent_ItemUI = { nullptr };

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

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_ItemUI"), &iter)))
			return E_FAIL;

		CGameObject* pGameObj = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"))->back();

		CItem_UI* pItemUI = dynamic_cast<CItem_UI*>(pGameObj);

		pItemUI->Set_Dead(true);

		if (nullptr != pParent_ItemUI)
		{
			pParent_ItemUI->PushBack_Child(pGameObj);
			pParent_ItemUI = nullptr;
		}

		else
		{
			m_vecItem_UI.push_back(pItemUI);
		}

		pParent_ItemUI = pItemUI;
	}

	return S_OK;
}

void CInventory_Manager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	for (auto& iter : m_vecInvenSlot)
	{
		Safe_Release(iter);
	}
	m_vecInvenSlot.clear();

	Safe_Release(m_pSlotHighlighter);
	Safe_Release(m_pSlotHighlighterTransform);

	for (auto& iter : m_vecItem_UI)
	{
		Safe_Release(iter);
	}
	m_vecItem_UI.clear();
}