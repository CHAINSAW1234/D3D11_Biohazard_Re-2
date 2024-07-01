#include "stdafx.h"

#include "Inventory_Manager.h"
#include "Inventory_Item_UI.h"

constexpr _float	Z_POS_SLOT = 0.8f;
constexpr _float	Z_POS_HIGH_LIGHTER = 0.7f;
constexpr _float	Z_POS_ITEM_UI = 0.6f;
constexpr _float	Z_POS_CONTEXT_MENU = 0.5f;

constexpr _float	SLOT_INTERVAL_X = 74.f;
constexpr _float	SLOT_INTERVAL_Y = 76.f;

constexpr _int		SELECTED_NUM = 0;
constexpr _int		COMBINDE_NUM = 1;
constexpr _int		RESULT_NUM = 2;

constexpr _float	PRESSING_TIME = 0.5f;

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

	if (FAILED(Init_DragShdow()))
		return E_FAIL;

	if (FAILED(Init_ContextMenu()))
		return E_FAIL;

	Set_ItemRecipe();

	return S_OK;
}

void CInventory_Manager::FirstTick_Seting()
{
	m_pSlotHighlighter->FirstTick_Seting();
	
	m_pSlotHighlighter->ResetPosition(m_fSlotHighlighterResetPos);

	for (auto& iter : m_vecItem_UI)
		iter->FirstTick_Seting();

	m_pDragShadow->FirstTick_Seting();

	if (FAILED(Seting_SubInven()))
		MSG_BOX(TEXT("Failed to Find SubInven"));
}

void CInventory_Manager::Tick(_float fTimeDelta)
{
	switch (m_eInven_Manager_State)
	{
	case Client::EVENT_IDLE: {
		EVENT_IDLE_Operation(fTimeDelta);
		break;
	}
		
	case Client::EQUIP_ITEM: {
		EQUIP_ITEM_Operation(fTimeDelta);
		break;
	}
		
	case Client::UNEQUIP_ITEM: {
		UNEQUIP_ITEM_Operation(fTimeDelta);
		break;
	}
		
	case Client::USE_ITEM: {
		USE_ITEM_Operation(fTimeDelta);
		break;
	}
		
	case Client::COMBINED_ITEM: {
		COMBINED_ITEM_Operation(fTimeDelta);
		break;
	}
		
	case Client::HOTKEY_ASSIGNED_ITEM: {
		HOTKEY_ASSIGNED_ITEM_Operation(fTimeDelta);
		break;
	}
		
	case Client::REARRANGE_ITEM: {
		REARRANGE_ITEM_Operation(fTimeDelta);
		break;
	}
		
	case Client::DISCARD_ITEM: {
		DISCARD_ITEM_Operation(fTimeDelta);
		break;
	}
		
	case Client::CONTEXTUI_SELECT: {
		CONTEXTUI_SELECT_Operation(fTimeDelta);
		break;
	}
		
	default:
		break;
	}

}

void CInventory_Manager::Late_Tick(_float fTimeDelta)
{
	m_pContextMenu->Late_Tick(fTimeDelta);
}

void CInventory_Manager::EVENT_IDLE_Operation(_float fTimeDelta)
{
	//_bool IsNoOneHover = true;
	m_IsNoOneHover = true;
	CInventory_Slot* pHoveredSlot = nullptr;

	for (_uint i = 0; i < m_iInvenCount; i++)
	{
		if (true == m_vecInvenSlot[i]->IsMouseHover())
		{
			m_IsNoOneHover = false;
			pHoveredSlot = m_vecInvenSlot[i];
		}
	}

	//m_pSlotHighlighter->Set_Dead(m_IsNoOneHover);

	if (false == m_IsNoOneHover)
	{
		_float4 HoveredPos = dynamic_cast<CTransform*>(pHoveredSlot->Get_Component(g_strTransformTag))->Get_State_Float4(CTransform::STATE_POSITION);
		HoveredPos.z = Z_POS_HIGH_LIGHTER;
		m_pSlotHighlighterTransform->Set_State(CTransform::STATE_POSITION, HoveredPos);

		for (auto& iter : m_vecItem_UI)
		{
			if (true == iter->IsMouseHover() && true == iter->Get_isWorking())
			{
				m_pSelected_ItemUI = iter;
				break;
			}

			else
				m_pSelected_ItemUI = nullptr;
		}

		if (UP == m_pGameInstance->Get_KeyState(VK_LBUTTON))
		{
			if (true == m_pSelected_ItemUI->IsMouseHover() && true == m_pSelected_ItemUI->Get_isWorking())
			{
				//이징 스타트 도착 지점 제대로 정해주기
				_float3 TempTrashCanValue = _float3(HoveredPos.x, HoveredPos.y, Z_POS_CONTEXT_MENU);
				m_pContextMenu->Set_Operation(m_pSelected_ItemUI->Get_ItemType(), true, TempTrashCanValue, TempTrashCanValue);
				m_eInven_Manager_State = CONTEXTUI_SELECT;
			}
		}

		if (PRESSING == m_pGameInstance->Get_KeyState(VK_LBUTTON))
		{
			if (true == m_pSelected_ItemUI->IsMouseHover() && true == m_pSelected_ItemUI->Get_isWorking())
			{
				m_fPressingTime += fTimeDelta;

				if (m_fPressingTime >= PRESSING_TIME)
				{
					_vector TempTrashCanValue = XMVectorSet(HoveredPos.x, HoveredPos.y, Z_POS_ITEM_UI, 1.f);

					m_pDragShadow->Set_ItemUI(m_pSelected_ItemUI->Get_ItemNumber(), DRAG_SHADOW, TempTrashCanValue, m_pSelected_ItemUI->Get_ItemQuantity());
					m_pDragShadow->Set_Dead(false);

					m_fPressingTime = 0.f;

					m_pSlotHighlighter->Set_DragShadow(true);

					m_eInven_Manager_State = REARRANGE_ITEM;
				}
			}
		}
		/*
		if (UP == m_pGameInstance->Get_KeyState(VK_LBUTTON))
		{
			for (auto& iter : m_vecItem_UI)
			{
				if (true == iter->IsMouseHover() && true == iter->Get_isWorking())
				{
					//이징 스타트 도착 지점 제대로 정해주기
					_float3 TempTrashCanValue = _float3(HoveredPos.x, HoveredPos.y, Z_POS_CONTEXT_MENU);
					m_pContextMenu->Set_Operation(iter->Get_ItemType(), true, TempTrashCanValue, TempTrashCanValue);
					m_eInven_Manager_State = CONTEXTUI_SELECT;
					m_pSelected_ItemUI = iter;
				}
			}
		}

		if (PRESSING == m_pGameInstance->Get_KeyState(VK_LBUTTON))
		{
			m_fPressingTime += fTimeDelta;
			if (m_fPressingTime >= PRESSING_TIME)
			{
				for (auto& iter : m_vecItem_UI)
				{
					if (true == iter->IsMouseHover() && true == iter->Get_isWorking())
					{
						_vector TempTrashCanValue = XMVectorSet(HoveredPos.x, HoveredPos.y, Z_POS_ITEM_UI, 1.f);
						m_eInven_Manager_State = REARRANGE_ITEM;
						
						m_pDragShadow->Set_ItemUI(iter->Get_ItemNumber(), DRAG_SHADOW, TempTrashCanValue, iter->Get_ItemQuantity());
						m_pDragShadow->Set_Dead(false);

						m_pSelected_ItemUI = iter;
						
						m_fPressingTime = 0.f;

						m_pSlotHighlighter->Set_DragShadow(true);
						break;
					}
				}
			}
		}
		*/
	} 
}

void CInventory_Manager::EQUIP_ITEM_Operation(_float fTimeDelta)
{

}

void CInventory_Manager::UNEQUIP_ITEM_Operation(_float fTimeDelta)
{

}

void CInventory_Manager::PICK_UP_ITEM_Operation(_float fTimeDelta)
{
	switch (m_eTaskSequence)
	{
	case Client::CInventory_Manager::SETING:
		break;
	case Client::CInventory_Manager::SELECT:
		break;
	case Client::CInventory_Manager::APPLY:
		break;
	default:
		break;
	}
}

void CInventory_Manager::USE_ITEM_Operation(_float fTimeDelta)
{
	m_pSelected_ItemUI->Set_ItemVariation(-1);

	m_eInven_Manager_State = EVENT_IDLE;
	m_pContextMenu->Set_Dead(true);
	m_pSelected_ItemUI = nullptr;

}

void CInventory_Manager::COMBINED_ITEM_Operation(_float fTimeDelta)
{
	switch (m_eTaskSequence)
	{
	case Client::CInventory_Manager::SETING: {
		m_CombineResources[SELECTED_NUM] = m_pSelected_ItemUI->Get_ItemNumber();
		vector<ITEM_RECIPE> vecRecipe = m_mapItemRecipe[m_CombineResources[SELECTED_NUM]];

		for (auto& ItemUIiter : m_vecItem_UI) //모든 아이템 순회
		{
			if (false == ItemUIiter->Get_isWorking() || m_pSelected_ItemUI == ItemUIiter) // 일안하거나 자기 자신 있으면 패스
				continue;

			_bool isCanCombined = false; //todo : 나중에 선택 가능과 불가능으로 바꿔야함

			for (auto& RecipeIter : vecRecipe) //아이탬 레시피에 자신이 있는지 검색
			{
				if (ItemUIiter->Get_ItemNumber() == RecipeIter.eCombination_Item) 
				{
					isCanCombined = true;
				}
			}

			if (false == isCanCombined)
				ItemUIiter->Reset_ItemUI();
		}

		m_eTaskSequence = SELECT;

		break;
	}

	case Client::CInventory_Manager::SELECT: {
		_bool IsNoOneHover = true;
		CInventory_Slot* pHoveredSlot = nullptr;

		for (_uint i = 0; i < m_iInvenCount; i++)
		{
			if (true == m_vecInvenSlot[i]->IsMouseHover())
			{
				IsNoOneHover = false;
				pHoveredSlot = m_vecInvenSlot[i];
			}
		}

		if (false == IsNoOneHover)
		{
			_float4 HoveredPos = dynamic_cast<CTransform*>(pHoveredSlot->Get_Component(g_strTransformTag))->Get_State_Float4(CTransform::STATE_POSITION);
			HoveredPos.z = Z_POS_ITEM_UI;
			m_pSlotHighlighterTransform->Set_State(CTransform::STATE_POSITION, HoveredPos); //하이라이터 움직임

			for (auto& iter : m_vecItem_UI)
			{
				if (true == iter->IsMouseHover() && true == iter->Get_isWorking())
				{
					m_CombineResources[COMBINDE_NUM] = iter->Get_ItemNumber();
					m_CombineResources[RESULT_NUM] = Find_Recipe(m_CombineResources[SELECTED_NUM], m_CombineResources[COMBINDE_NUM]);

					if (UP == m_pGameInstance->Get_KeyState(VK_LBUTTON))
					{
						iter->Reset_ItemUI();
						m_eTaskSequence = APPLY;
					}
				}

				else
				{
					m_CombineResources[COMBINDE_NUM] = ITEM_NUMBER_END;
					m_CombineResources[RESULT_NUM] = ITEM_NUMBER_END;
				}
			}
		}
		break;
	}
		
	case Client::CInventory_Manager::APPLY: {
		m_pSelected_ItemUI->Set_ItemNumber(m_CombineResources[RESULT_NUM]);
		m_eTaskSequence = TS_END;
		m_eInven_Manager_State = EXAMINE_ITEM;
		m_CombineResources[SELECTED_NUM] = { ITEM_NUMBER_END };
		m_CombineResources[COMBINDE_NUM] = { ITEM_NUMBER_END };
		m_CombineResources[RESULT_NUM] = { ITEM_NUMBER_END };
		break;
	}


	default:
		break;
	}

}

void CInventory_Manager::HOTKEY_ASSIGNED_ITEM_Operation(_float fTimeDelta)
{

}

void CInventory_Manager::REARRANGE_ITEM_Operation(_float fTimeDelta)
{
	m_IsNoOneHover = true;
	CInventory_Slot* pHoveredSlot = nullptr;

	for (_uint i = 0; i < m_iInvenCount; i++)
	{
		if (true == m_vecInvenSlot[i]->IsMouseHover())
		{
			m_IsNoOneHover = false;
			pHoveredSlot = m_vecInvenSlot[i];
		}
	}

	if (false == m_IsNoOneHover)
	{
		_float4 HoveredPos = dynamic_cast<CTransform*>(pHoveredSlot->Get_Component(g_strTransformTag))->Get_State_Float4(CTransform::STATE_POSITION);
		HoveredPos.z = Z_POS_HIGH_LIGHTER;
		m_pSlotHighlighterTransform->Set_State(CTransform::STATE_POSITION, HoveredPos);

		if (PRESSING == m_pGameInstance->Get_KeyState(VK_LBUTTON))
		{
			m_pDragShadowTransform->Set_State(CTransform::STATE_POSITION, HoveredPos);
		}

		if (UP == m_pGameInstance->Get_KeyState(VK_LBUTTON))
		{
			CTransform* pSelectedItemTransform = m_pSelected_ItemUI->Get_Transform();
			_float4 fPos = { HoveredPos.x, HoveredPos.y, Z_POS_ITEM_UI, 1.f };
			pSelectedItemTransform->Set_State(CTransform::STATE_POSITION, fPos);
			m_pSlotHighlighter->Set_DragShadow(false);
			m_eInven_Manager_State = EVENT_IDLE;
			m_pDragShadow->Set_Dead(true);
			m_pSelected_ItemUI = nullptr;
		}
	}
}

void CInventory_Manager::DISCARD_ITEM_Operation(_float fTimeDelta)
{
	m_pSelected_ItemUI->Reset_ItemUI();
	m_pSelected_ItemUI = nullptr;

	m_eInven_Manager_State = EVENT_IDLE;
}

void CInventory_Manager::CONTEXTUI_SELECT_Operation(_float fTimeDelta)
{
	if (DOWN == m_pGameInstance->Get_KeyState(VK_RBUTTON))
	{
		m_eInven_Manager_State = EVENT_IDLE;
		m_pContextMenu->Set_Dead(true);
		m_pSelected_ItemUI = nullptr;
		return;
	}

	m_pContextMenu->Tick(fTimeDelta);

	INVENTORY_EVENT eInvenEvent = m_pContextMenu->Get_InventoryEvent();

	switch (eInvenEvent)
	{
	case Client::EVENT_IDLE:
		break;

	case Client::EQUIP_ITEM: {
		m_eInven_Manager_State = EQUIP_ITEM;
		m_pContextMenu->Set_Dead(true);
		break;
	}
		
	case Client::UNEQUIP_ITEM: {
		m_eInven_Manager_State = UNEQUIP_ITEM;
		m_pContextMenu->Set_Dead(true);
		break;
	}
		
	case Client::USE_ITEM: {
		m_eInven_Manager_State = USE_ITEM;
		m_pContextMenu->Set_Dead(true);
		break;
	}
		
	case Client::EXAMINE_ITEM: {
		m_eInven_Manager_State = EXAMINE_ITEM;
		m_pContextMenu->Set_Dead(true);
		break;
	}
		
	case Client::COMBINED_ITEM: {
		m_eInven_Manager_State = COMBINED_ITEM;
		m_eTaskSequence = SETING;
		m_pContextMenu->Set_Dead(true);
		break;
	}
		
	case Client::HOTKEY_ASSIGNED_ITEM: {
		m_eInven_Manager_State = HOTKEY_ASSIGNED_ITEM;
		m_pContextMenu->Set_Dead(true);
		break;
	}
		
	case Client::REARRANGE_ITEM: {
		m_eInven_Manager_State = REARRANGE_ITEM;
		m_pContextMenu->Set_Dead(true);
		break;
	}
		
	case Client::DISCARD_ITEM: {
		m_eInven_Manager_State = DISCARD_ITEM;
		m_pContextMenu->Set_Dead(true);
		break;
	}
		
	case Client::CONTEXTUI_SELECT: {
		m_eInven_Manager_State = CONTEXTUI_SELECT;
		m_pContextMenu->Set_Dead(true);
		break;
	}
	
	default:
		break;
	}
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
		//탭창킬때 일하고 있는놈만 켜줘야함
		if (false == bInput)
		{
			if (true == iter->Get_isWorking())
				iter->Set_Dead(bInput);
		}

		//끌땐 다끄자
		else
		{
			iter->Set_Dead(bInput);
		}
	}

	m_pContextMenu->Set_Dead(true);

	m_eInven_Manager_State = EVENT_IDLE;

	m_pInven_Item_UI->Reset_Call(!bInput);
}

ITEM_NUMBER CInventory_Manager::Get_Selected_ItemNum()
{
	if (COMBINED_ITEM == m_eInven_Manager_State)
		return m_CombineResources[RESULT_NUM];


	if (nullptr != m_pSelected_ItemUI)
	{
		return m_pSelected_ItemUI->Get_ItemNumber();
	}
	else
		return ITEM_NUMBER_END;
}

void CInventory_Manager::PUO_Seting(ITEM_NUMBER eAcquiredItem, _int iItemQuantity)
{
	_vector TempTrashCanValue = XMVectorSet(m_fSlotHighlighterResetPos.x, m_fSlotHighlighterResetPos.y, Z_POS_ITEM_UI, 1.f);

	m_pDragShadow->Set_ItemUI(m_pSelected_ItemUI->Get_ItemNumber(), DRAG_SHADOW, TempTrashCanValue, m_pSelected_ItemUI->Get_ItemQuantity());
	m_pDragShadow->Set_Dead(false);



}

void CInventory_Manager::UseItem(ITEM_NUMBER eTargetItemNum, _int iUsage)
{
	for (auto& iter : m_vecItem_UI)
	{
		if (eTargetItemNum == iter->Get_ItemNumber() && true == iter->Get_isWorking())
		{
			iter->Set_ItemVariation(-iUsage);

			//todo 재귀하게 만들어보면 좋을듯 남은 수량 다 써버리게
			//if (iter->Get_ItemQuantity() >= iUsage)
			//	iter->Set_ItemVariation(-iUsage);
			//else
			//{
			//	_uint iLeftUsage = iUsage - iter->Get_ItemQuantity();
			//}
		}
	}
}

void CInventory_Manager::AddItem_ToInven(ITEM_NUMBER eAcquiredItem, _int iItemQuantity)
{
	for (auto& Itemiter : m_vecItem_UI)
	{
		if (true == Itemiter->Get_isWorking())
			continue;

		for (auto& Slotiter : m_vecInvenSlot)
		{
			if (true == Slotiter->Get_IsFilled())
				continue;

			_vector vSlotPos = Slotiter->GetPositionVector();
			vSlotPos = XMVectorSetZ(vSlotPos, Z_POS_ITEM_UI);
			Slotiter->Set_IsFilled(true);
			Itemiter->Set_ItemUI(eAcquiredItem, ItemType_Classify_ByNumber(eAcquiredItem), vSlotPos, iItemQuantity);
			return;
		}
	}
}

_bool CInventory_Manager::IsCan_AddItem_ToInven()
{
	_bool IsCanAddItem = false;

	for (_uint i = 0; i < m_iInvenCount; i++)
	{
		if (false == m_vecInvenSlot[i]->Get_IsFilled())
		{
			IsCanAddItem = true;
		}
	}

	return IsCanAddItem;
}

_int CInventory_Manager::Get_Search_Item_Quantity(ITEM_NUMBER eItemNum)
{
	_int iItemQuantity = 0;

	for (auto& iter : m_vecItem_UI)
	{
		if (eItemNum == iter->Get_ItemNumber())
		{
			iItemQuantity += iter->Get_ItemQuantity();
		}
	}

	return iItemQuantity;
}

ITEM_NUMBER CInventory_Manager::Get_Item_On_HotKey(_uint iHotKeyNum)
{

	
	return ITEM_NUMBER();
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

#pragma region 인벤토리 슬롯
	selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/VoidSlot.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	vector<CCustomize_UI::CUSTOM_UI_DESC> vecCustomInvenUIDesc;
	CCustomize_UI::ExtractData_FromDat(inputFileStream, &vecCustomInvenUIDesc, true);

	for (_uint i = 0; i < 5; i++)
	{
		for (_uint j = 0; j < 4; j++)
		{
			if (FAILED(Create_InvenSlot(&vecCustomInvenUIDesc, _float3((SLOT_INTERVAL_X * j), -(SLOT_INTERVAL_Y * i), 0.f))))
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
	selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/SlotHighlighter.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	CCustomize_UI::CreatUI_FromDat(inputFileStream, nullptr, TEXT("Prototype_GameObject_SlotHighlighter"),
		(CGameObject**)&m_pSlotHighlighter, m_pDevice, m_pContext);

	m_pSlotHighlighter->Set_Dead(true);
	m_pSlotHighlighterTransform = dynamic_cast<CTransform*>(m_pSlotHighlighter->Get_Component(g_strTransformTag));
	m_fSlotHighlighterResetPos = m_vecInvenSlot[0]->GetPosition();
	m_fSlotHighlighterResetPos.z = 0.7f;

	Safe_AddRef(m_pSlotHighlighter);
	Safe_AddRef(m_pSlotHighlighterTransform);

	return S_OK;
}

HRESULT CInventory_Manager::Init_ItemUI()
{
	for (_uint i = 0; i < 20; i++)
	{
		ifstream inputFileStream;
		wstring selectedFilePath;
		selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/Item_UI.dat");
		inputFileStream.open(selectedFilePath, ios::binary);
		CItem_UI* pItemUI = { nullptr };
		CCustomize_UI::CreatUI_FromDat(inputFileStream, nullptr, TEXT("Prototype_GameObject_ItemUI"), 
			(CGameObject**)&pItemUI, m_pDevice, m_pContext);
		m_vecItem_UI.push_back(pItemUI);
	}

	for (auto& iter : m_vecItem_UI)
	{
		if (nullptr != iter)
		{
			Safe_AddRef(iter);
			iter->Set_Dead(true);
		}
	}

	return S_OK;
}

HRESULT CInventory_Manager::Init_DragShdow()
{
	ifstream inputFileStream;
	wstring selectedFilePath;
	selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/Item_UI.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	CCustomize_UI::CreatUI_FromDat(inputFileStream, nullptr, TEXT("Prototype_GameObject_ItemUI"),
		(CGameObject**)&m_pDragShadow, m_pDevice, m_pContext);
	if (nullptr == m_pDragShadow)
		return E_FAIL;

	m_pDragShadow->Set_Dead(true);
	m_pDragShadow->Set_ItemType(DRAG_SHADOW);

	m_pDragShadowTransform = dynamic_cast<CTransform*>(m_pDragShadow->Get_Component(g_strTransformTag));

	Safe_AddRef(m_pDragShadow);
	Safe_AddRef(m_pDragShadowTransform);
	return S_OK;
}

HRESULT CInventory_Manager::Init_ContextMenu()
{
	ifstream inputFileStream;
	wstring selectedFilePath;
	selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/Context_Menu.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	
	CCustomize_UI::CreatUI_FromDat(inputFileStream, nullptr, TEXT("Prototype_GameObject_ContextMenu"), 
		(CGameObject**)&m_pContextMenu, m_pDevice, m_pContext);

	Safe_AddRef(m_pContextMenu);
	m_pContextMenu->Set_Dead(true);



	return S_OK;
}

HRESULT CInventory_Manager::Seting_SubInven()
{
	list<CGameObject*>* pGameObjList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

	for (auto& iter : *pGameObjList)
	{
		CInventory_Item_UI* pInven_item_UI = dynamic_cast<CInventory_Item_UI*>(iter);

		if (nullptr != pInven_item_UI)
		{
			m_pInven_Item_UI = pInven_item_UI;
			return S_OK;
		}
	}

	return E_FAIL;
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

	Safe_Release(m_pContextMenu);

	Safe_Release(m_pDragShadow);
	Safe_Release(m_pDragShadowTransform);
}

ITEM_TYPE CInventory_Manager::ItemType_Classify_ByNumber(ITEM_NUMBER eItemNum)
{
	switch (eItemNum)
	{
	case Client::emergencyspray01a:
		return USEABLE;
		break;
	case Client::greenherb01a:
		return USEABLE;
		break;
	case Client::redherb01a:
		return USEABLE;
		break;
	case Client::blueherb01a:
		return USEABLE;
		break;
	case Client::herbsgg01a:
		return USEABLE;
		break;
	case Client::herbsgr01a:
		return USEABLE;
		break;
	case Client::herbsgb01a:
		return USEABLE;
		break;
	case Client::herbsggb01a:
		return USEABLE;
		break;
	case Client::herbsggg01a:
		return USEABLE;
		break;
	case Client::herbsgrb01a:
		return USEABLE;
		break;
	case Client::herbsrb01a:
		return USEABLE;
		break;
	case Client::greenherbitem01a:
		return USEABLE;
		break;
	case Client::redherbitem01a:
		return USEABLE;
		break;
	case Client::blueherbitem01a:
		return USEABLE;
		break;
	case Client::handgun_bullet01a:
		return CONSUMABLE;
		break;
	case Client::shotgun_bullet01a:
		return CONSUMABLE;
		break;
	case Client::submachinegun_bullet01a:
		return CONSUMABLE;
		break;
	case Client::magnumbulleta:
		return CONSUMABLE;
		break;
	case Client::biggun_bullet01a:
		return CONSUMABLE;
		break;
	case Client::inkribbon01a:
		return QUEST;
		break;
	case Client::woodbarricade01a:
		return CONSUMABLE;
		break;
	case Client::blastingfuse01a:
		return QUEST;
		break;
	case Client::gunpowder01a:
		return QUEST;
		break;
	case Client::gunpowder01b:
		return QUEST;
		break;
	case Client::strengtheningyellow01a:
		return USEABLE;
		break;
	/*case Client::HandGun:
		return EQUIPABLE;
		break;*/
	case Client::ITEM_NUMBER_END:
		break;
	default:
		break;
	}

	return ITEM_TYPE::INVEN_ITEM_TYPE_END;
}

void CInventory_Manager::Set_ItemRecipe()
{
	Add_Recipe(greenherb01a, greenherb01a,	herbsgg01a);
	Add_Recipe(greenherb01a, redherb01a,	herbsgr01a);
	Add_Recipe(greenherb01a, blueherb01a,	herbsgb01a);
	Add_Recipe(greenherb01a, herbsgg01a,	herbsggg01a);
	Add_Recipe(greenherb01a, herbsgr01a,	herbsgg01a);
	Add_Recipe(greenherb01a, herbsgb01a,	herbsgg01a);






}

void CInventory_Manager::Add_Recipe(ITEM_NUMBER eKeyItemNum, ITEM_NUMBER eCombinableItemNum, ITEM_NUMBER eResultItemNum)
{
	ITEM_RECIPE sRecipe = { eCombinableItemNum , eResultItemNum };
	m_mapItemRecipe[eKeyItemNum].push_back(sRecipe);
}

ITEM_NUMBER CInventory_Manager::Find_Recipe(ITEM_NUMBER eKeyItemNum, ITEM_NUMBER eCombinableItemNum)
{
	vector<ITEM_RECIPE> vecRecipe = m_mapItemRecipe[eKeyItemNum];

	for (auto& iter : vecRecipe)
	{
		if (eCombinableItemNum == iter.eCombination_Item)
			return iter.eResult_Item;
	}

	return ITEM_NUMBER_END;
}

