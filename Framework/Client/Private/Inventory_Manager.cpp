#include "stdafx.h"

#include "Inventory_Manager.h"
#include "Player.h"
#include "Tab_Window.h"

constexpr _float	Z_POS_SLOT = 0.008f;
constexpr _float	Z_POS_ITEM_UI = 0.007f;
constexpr _float	Z_POS_HIGH_LIGHTER = 0.006f;
constexpr _float	Z_POS_CONTEXT_MENU = 0.005f;

constexpr _float	SLOT_INTERVAL_X = 74.f;
constexpr _float	SLOT_INTERVAL_Y = 76.f;

constexpr _int		SELECTED_NUM = 0;
constexpr _int		COMBINDE_NUM = 1;
constexpr _int		RESULT_NUM = 2;

constexpr _float	PRESSING_TIME = 0.5f;

constexpr _float	SWITCH_SPEED_LIMIT = 10.f;

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

	if (FAILED(Init_ItemUI()))
		return E_FAIL;

	if (FAILED(Init_DragShdow()))
		return E_FAIL;

	if (FAILED(Init_SlotHighlighter()))
		return E_FAIL;

	if (FAILED(Init_ContextMenu()))
		return E_FAIL;

	if (FAILED(Seting_Hotkey()))
		return E_FAIL;

	Set_ItemRecipe();

	return S_OK;
}

void CInventory_Manager::FirstTick_Seting()
{
	AddItem_ToInven(HandGun, 15);
	AddItem_ToInven(ShotGun, 7);
	AddItem_ToInven(handgun_bullet01a, 20);
	AddItem_ToInven(shotgun_bullet01a, 20);
}

void CInventory_Manager::SecondTick_Seting()
{
	for (auto& iter : m_vecItem_UI)
	{
		if (HandGun == iter->Get_ItemNumber())
		{
			_uint iHotKeyNum = m_pHotkey->RegisterHoykey(0, iter->Get_ItemNumber(), iter->Get_ItemQuantity());
			iter->Set_Text(HOTKEY_DISPLAY, to_wstring(iHotKeyNum));
		}

		else if (ShotGun == iter->Get_ItemNumber())
		{
			_uint iHotKeyNum = m_pHotkey->RegisterHoykey(1, iter->Get_ItemNumber(), iter->Get_ItemQuantity());
			iter->Set_Text(HOTKEY_DISPLAY, to_wstring(iHotKeyNum));
		}
	}

	m_pSlotHighlighter->ResetPosition(m_fSlotHighlighterResetPos);
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
	case Client::PICK_UP_ITEM: {
		PICK_UP_ITEM_Operation(fTimeDelta);
		break;
	}
	case Client::INTERACT_PROPS: {
		INTERACT_ITEM_Operation(fTimeDelta);
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
			if (m_pHoverdSlot != m_vecInvenSlot[i])
			{
				m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_InvenSlot_Tick.mp3"), CH_TICK_2D, 0.2f);

				m_pHoverdSlot = m_vecInvenSlot[i];
			}
			m_IsNoOneHover = false;
			pHoveredSlot = m_vecInvenSlot[i];
			
		}
	}

	if (false == m_IsNoOneHover)
	{
		_float4 HoveredPos = static_cast<CTransform*>(pHoveredSlot->Get_Component(g_strTransformTag))->Get_State_Float4(CTransform::STATE_POSITION);
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

		if (nullptr != m_pSelected_ItemUI)
		{
			if (UP == m_pGameInstance->Get_KeyState(VK_LBUTTON))
			{
				if (true == m_pSelected_ItemUI->IsMouseHover() && true == m_pSelected_ItemUI->Get_isWorking())
				{
					m_eInven_Manager_State = CONTEXTUI_SELECT;
					m_eTaskSequence = SETING;
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
						m_eTaskSequence = SETING;
						m_eInven_Manager_State = REARRANGE_ITEM;
						m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_DragUp.mp3"), CH1_2D, 0.5f);
					}
				}
			}
		}
	} 

}

void CInventory_Manager::EQUIP_ITEM_Operation(_float fTimeDelta)
{
	m_pSelected_ItemUI->Set_isEquiped(true);

	ITEM_NUMBER eNumber = m_pSelected_ItemUI->Get_ItemNumber();

	for (_uint i = 0; i < 5; i++)
	{
		if (ITEM_NUMBER_END == m_eEquipedItem[i]) {
			m_eEquipedItem[i] = eNumber;
			break;
		}
	}

	switch (eNumber)
	{
	case Client::HandGun:
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_EquipHandGun.mp3"), CH1_2D, 0.5f);
		break;
	case Client::ShotGun:
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_EquipShotgun.mp3"), CH1_2D, 0.5f);
		break;
	case Client::Flash_Bomb:
		break;
	case Client::Grenade:
		break;


	default:
		break;
	}

	m_pSelected_ItemUI->Set_Dead(false);
	m_pSelected_ItemUI = nullptr;
	m_eInven_Manager_State = EVENT_IDLE;
}

void CInventory_Manager::UNEQUIP_ITEM_Operation(_float fTimeDelta)
{
	m_pSelected_ItemUI->Set_isEquiped(false);

	ITEM_NUMBER eNumber = m_pSelected_ItemUI->Get_ItemNumber();

	for (_uint i = 0; i < 5; i++)
	{
		if (m_eEquipedItem[i] == eNumber)
		{
			m_eEquipedItem[i] = ITEM_NUMBER_END;
			break;
		}
	}

	switch (eNumber)
	{
	case Client::HandGun:
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_EquipShotgun.mp3"), CH1_2D, 0.5f);
		break;
	case Client::ShotGun:
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_EquipShotgun.mp3"), CH1_2D, 0.5f);
		break;
	case Client::Flash_Bomb:
		break;
	case Client::Grenade:
		break;


	default:
		break;
	}

	m_pSelected_ItemUI->Set_Dead(false);
	m_pSelected_ItemUI = nullptr;
	m_eInven_Manager_State = EVENT_IDLE;
}

void CInventory_Manager::PICK_UP_ITEM_Operation(_float fTimeDelta)
{
	_bool isCombind = false;

	switch (m_eTaskSequence)
	{
	case Client::CInventory_Manager::SETING: {
		m_fClickTimer += fTimeDelta;

		if (DOWN == m_pGameInstance->Get_KeyState(VK_RBUTTON) || UP == m_pGameInstance->Get_KeyState(VK_ESCAPE))
		{
			m_eTaskSequence = APPLY;
			m_pContextMenu->Set_Dead(true);
			m_PickResult = -1;
			m_fClickTimer = 0.f;
		}

		m_IsNoOneHover = true;
		_bool isFullSlot = true;
		CInventory_Slot* pHoveredSlot = nullptr;

		for (_uint i = 0; i < m_iInvenCount; i++)
		{
			if (true == m_vecInvenSlot[i]->IsMouseHover())
			{
				if (m_pHoverdSlot != m_vecInvenSlot[i])
				{
					m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_InvenSlot_Tick.mp3"), CH_TICK_2D, 0.2f);
					m_pHoverdSlot = m_vecInvenSlot[i];
				}
				m_IsNoOneHover = false;
				pHoveredSlot = m_vecInvenSlot[i];
			}
			if (false == m_vecInvenSlot[i]->Get_IsFilled())
			{
				isFullSlot = false;
			}
		}

		if (false == m_IsNoOneHover)
		{
			_float4 HoveredPos = static_cast<CTransform*>(pHoveredSlot->Get_Component(g_strTransformTag))->Get_State_Float4(CTransform::STATE_POSITION);
			HoveredPos.z = Z_POS_HIGH_LIGHTER;
			m_pDragShadowTransform->Set_State(CTransform::STATE_POSITION, HoveredPos);
			m_pSlotHighlighterTransform->Set_State(CTransform::STATE_POSITION, HoveredPos);

			if (UP == m_pGameInstance->Get_KeyState(VK_LBUTTON) && m_fClickTimer > 0.8f)
			{
				m_pSelected_ItemUI = nullptr;
				for (auto& iter : m_vecItem_UI)
				{
					if (true == iter->IsMouseHover() && true == iter->Get_isWorking())
					{
						m_pSelected_ItemUI = iter;
					}
				}

				if (nullptr != m_pSelected_ItemUI && true == pHoveredSlot->Get_IsFilled())
				{
					ITEM_NUMBER eResultItem = Find_Recipe(m_pDragShadow->Get_ItemNumber(), m_pSelected_ItemUI->Get_ItemNumber());

					if (false == isFullSlot)
					{
						if (ITEM_NUMBER_END == eResultItem)
						{
							m_eTaskSequence = SELECT;
							_float3 TempTrashCanValue = _float3(HoveredPos.x, HoveredPos.y, Z_POS_CONTEXT_MENU);
							m_pContextMenu->Set_Operation(UNCOMBINABLE_PICKED_UP, false, TempTrashCanValue, TempTrashCanValue);
						}

						else
						{
							m_eTaskSequence = SELECT;
							_float3 TempTrashCanValue = _float3(HoveredPos.x, HoveredPos.y, Z_POS_CONTEXT_MENU);
							m_pContextMenu->Set_Operation(COMBINABLE_PICKED_UP, true, TempTrashCanValue, TempTrashCanValue);
						}
					}
					else
					{
						if (ITEM_NUMBER_END != eResultItem)
						{
							m_eTaskSequence = SELECT;
							_float3 TempTrashCanValue = _float3(HoveredPos.x, HoveredPos.y, Z_POS_CONTEXT_MENU);
							m_pContextMenu->Set_Operation(FULL_SLOT_COMBINABLE_PICKED_UP, true, TempTrashCanValue, TempTrashCanValue);
						}
					}
						
				}

				else if(nullptr == m_pSelected_ItemUI && false == pHoveredSlot->Get_IsFilled())
				{
					AddItem_ToInven(m_pDragShadow->Get_ItemNumber(), m_pDragShadow->Get_ItemQuantity(), _float3(HoveredPos.x, HoveredPos.y, Z_POS_ITEM_UI));
					m_pSlotHighlighter->Set_DragShadow(false);
					m_pDragShadow->Set_Dead(true);
					m_fClickTimer = 0.f;
					m_pSelected_ItemUI = nullptr;
					m_eInven_Manager_State = GET_ITEM;
					pHoveredSlot->Set_IsFilled(true);
				}
			}
		}
		break;
	}
		
	case Client::CInventory_Manager::SELECT: {
		m_pContextMenu->Tick(fTimeDelta);

		INVENTORY_EVENT eInvenEvent = m_pContextMenu->Get_InventoryEvent();

		if (DOWN == m_pGameInstance->Get_KeyState(VK_RBUTTON) || UP == m_pGameInstance->Get_KeyState(VK_ESCAPE))
		{
			m_eTaskSequence = SETING;
			m_pContextMenu->Set_Dead(true);
			m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_Context_Close.mp3"), 0.1f);
		}

		switch (eInvenEvent)
		{
		case Client::COMBINED_ITEM: {
			m_PickResult = 0;
			m_eTaskSequence = APPLY;
			m_pContextMenu->Set_Dead(true);
			break;
		}

		case Client::SWITCH_ITEM: {
			m_PickResult = 1;
			m_eTaskSequence = APPLY;
			m_pContextMenu->Set_Dead(true);
			for (auto& iter : m_vecInvenSlot)
			{
				if (false == iter->Get_IsFilled())
				{
					m_fSwitchTargetPos = _float2{ iter->GetPosition().x, iter->GetPosition().y };
					break;
				}
			}

			break;
		}
		}

		break;
	}
		
	case Client::CInventory_Manager::APPLY: {
		m_fClickTimer = 0.f;
		
		if (m_PickResult == -1)
		{
			m_eInven_Manager_State = EVENT_CANCLE;
		}

		else if (m_PickResult == 0)
		{
			ITEM_NUMBER eResultItem = Find_Recipe(m_pDragShadow->Get_ItemNumber(), m_pSelected_ItemUI->Get_ItemNumber());
			if (handgun_bullet01a == m_pDragShadow->Get_ItemNumber() && handgun_bullet01a == m_pSelected_ItemUI->Get_ItemNumber() && handgun_bullet01a == eResultItem)
			{
				m_pSelected_ItemUI->Set_ItemVariation(m_pDragShadow->Get_ItemQuantity());
			}

			else if (shotgun_bullet01a == m_pDragShadow->Get_ItemNumber() && shotgun_bullet01a == m_pSelected_ItemUI->Get_ItemNumber() && shotgun_bullet01a == eResultItem)
			{
				m_pSelected_ItemUI->Set_ItemVariation(m_pDragShadow->Get_ItemQuantity());
			}

			else
			{
				m_pSelected_ItemUI->Set_ItemUI(eResultItem, ItemType_Classify_ByNumber(eResultItem), m_pSelected_ItemUI->GetPositionVector(),1);
			}

			m_PickResult = -1;
			m_pSlotHighlighter->Set_DragShadow(false);
			m_pDragShadow->Set_Dead(true);
			m_pSelected_ItemUI = nullptr;
			m_eInven_Manager_State = GET_ITEM;
			m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_DragDown.mp3"), CH1_2D, 0.5f);
		}

		else if(m_PickResult == 1)
		{
			Find_Slot(_float2(m_fSwitchTargetPos.x, m_fSwitchTargetPos.y))->Set_IsFilled(true);
			m_pSelected_ItemUI->Set_Position(XMVectorSet(m_fSwitchTargetPos.x, m_fSwitchTargetPos.y, Z_POS_ITEM_UI, 1.f));
			m_fSwitchTargetPos = _float2();
			_float3 fNewItemPos = { m_pDragShadow->GetPositionVector() };
			AddItem_ToInven(m_pDragShadow->Get_ItemNumber(), 10, _float3(fNewItemPos.x, fNewItemPos.y, Z_POS_ITEM_UI));
			m_fItemSwitchTime = 0.f;
			m_PickResult = -1;
			m_pSlotHighlighter->Set_DragShadow(false);
			m_pDragShadow->Set_Dead(true);
			m_pSelected_ItemUI = nullptr;
			m_eInven_Manager_State = GET_ITEM;
			m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_DragDown.mp3"), CH1_2D, 0.5f);

			/*
			if (m_fItemSwitchTime / 0.5f <= 1.f)
			{
				m_fItemSwitchTime += fTimeDelta;
				//_float fSpeed = m_pGameInstance->Get_Ease(Ease_Linear, 0.f, SWITCH_SPEED_LIMIT, m_fItemSwitchTime / 0.5f);
				//m_pSelected_ItemUI->Get_Transform()->Move_toTargetUI(_float4(m_fSwitchTargetPos.x, m_fSwitchTargetPos.y, Z_POS_ITEM_UI, 1.f), fSpeed, 5.f);
				m_pSelected_ItemUI->Move_To_Target(_float4(m_fSwitchTargetPos.x, m_fSwitchTargetPos.y, Z_POS_ITEM_UI, 1.f), fTimeDelta * 10.f, 0.1f);
			}

			else
			{
				Find_Slot(_float2(m_fSwitchTargetPos.x, m_fSwitchTargetPos.y))->Set_IsFilled(true);
				//m_pSelected_ItemUI->Set_Position(_float3(m_fSwitchTargetPos.x, m_fSwitchTargetPos.y, Z_POS_ITEM_UI));
				m_fSwitchTargetPos = _float2();
				_float3 fNewItemPos = { m_pDragShadow->GetPositionVector() };
				AddItem_ToInven(m_pDragShadow->Get_ItemNumber(), 10, _float3(fNewItemPos.x, fNewItemPos.y, Z_POS_ITEM_UI));
				m_fItemSwitchTime = 0.f;
				m_PickResult = -1;
				m_pSlotHighlighter->Set_DragShadow(false);
				m_pDragShadow->Set_Dead(true);
				m_pSelected_ItemUI = nullptr;
				m_eInven_Manager_State = GET_ITEM;
				m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_DragDown.mp3"), CH1_2D, 0.5f);
			}
			*/
		}
		break;
	}
		
	default:
		break;
	}
}



void CInventory_Manager::USE_ITEM_Operation(_float fTimeDelta)
{
	m_pSelected_ItemUI->Set_ItemVariation(-1);

	ITEM_NUMBER eUseItemNum = m_pSelected_ItemUI->Get_ItemNumber();

	CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0));

	switch (eUseItemNum)
	{
	case Client::emergencyspray01a: {
		pPlayer->Set_Hp(5);
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_UseItem_emergencyspray.mp3"), CH1_2D, 0.5f );
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_heal.mp3"), CH2_2D, 0.5f);
		break;
	}

	case Client::greenherb01a: {
		pPlayer->Set_Hp(pPlayer->Get_Hp() + 1);
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_UseItem_herbs.mp3"), CH1_2D, 0.5f);
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_heal.mp3"), CH2_2D, 0.5f);
		break;
	}

	case Client::redherb01a: { //내기억으론 이게 단독사용불가																																											
		break;
	}

	case Client::blueherb01a: {
		break;
	}

	case Client::herbsgg01a: {
		pPlayer->Set_Hp(pPlayer->Get_Hp() + 3);
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_UseItem_herbs.mp3"), CH1_2D, 0.5f);
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_heal.mp3"), CH2_2D, 0.5f);
		break;
	}

	case Client::herbsgr01a: {
		pPlayer->Set_Hp(pPlayer->Get_Hp() + 3);
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_UseItem_herbs.mp3"), CH1_2D, 0.5f);
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_heal.mp3"), CH2_2D, 0.5f);
		break;
	}

	case Client::herbsgb01a: {
		pPlayer->Set_Hp(pPlayer->Get_Hp() + 1);
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_UseItem_herbs.mp3"), CH1_2D, 0.5f);
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_heal.mp3"), CH2_2D, 0.5f);
		break;
	}

	case Client::herbsggb01a: {
		pPlayer->Set_Hp(pPlayer->Get_Hp() + 3);
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_UseItem_herbs.mp3"), CH1_2D, 0.5f);
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_heal.mp3"), CH2_2D, 0.5f);
		break;
	}

	case Client::herbsggg01a: {
		pPlayer->Set_Hp(5);
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_UseItem_herbs.mp3"), CH1_2D, 0.5f);
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_heal.mp3"), CH2_2D, 0.5f);
		break;
	}

	case Client::herbsgrb01a: {
		pPlayer->Set_Hp(5);
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_UseItem_herbs.mp3"), CH1_2D, 0.5f);
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_heal.mp3"), CH2_2D, 0.5f);
		break;
	}

	case Client::herbsrb01a: {
		break;
	}
	//case Client::greenherbitem01a:
	//case Client::redherbitem01a:
	//case Client::blueherbitem01a:
	//	break;

	case Client::clairesbag01a: {
		m_iInvenCount++;
		m_vecInvenSlot[m_iInvenCount - 1]->Set_Dead(false);
		m_iInvenCount++;
		m_vecInvenSlot[m_iInvenCount - 1]->Set_Dead(false);
		break;
	}
	default:
		break;
	}

	m_eInven_Manager_State = EVENT_IDLE;
	m_pContextMenu->Set_Dead(true);
	Find_Slot(_float2(m_pSelected_ItemUI->GetPosition().x, m_pSelected_ItemUI->GetPosition().y))->Set_IsFilled(false);
	m_pSelected_ItemUI->Reset_ItemUI();
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

			_bool isCanCombined = false;

			for (auto& RecipeIter : vecRecipe) //아이탬 레시피에 자신이 있는지 검색
			{
				if (ItemUIiter->Get_ItemNumber() == RecipeIter.eCombination_Item) 
				{
					isCanCombined = true;
				}
			}

			if (false == isCanCombined)
				ItemUIiter->Set_Activ_ItemUI(false);
		}

		m_eTaskSequence = SELECT;

		break;
	}

	case Client::CInventory_Manager::SELECT: {
		
		if (UP == m_pGameInstance->Get_KeyState(VK_RBUTTON) || UP == m_pGameInstance->Get_KeyState(VK_ESCAPE))
		{
			if (INTERACT_PROPS == m_ePre_Inven_Manager_State) {
				m_eInven_Manager_State = INTERACT_PROPS;
				m_eTaskSequence = SETING;
			}
				
			else
				m_eInven_Manager_State = EVENT_IDLE;

			for (auto& iter : m_vecItem_UI)
			{
				if (false == iter->Get_isActive() && true == iter->Get_isWorking())
				{
					iter->Set_Activ_ItemUI(true);
				}
			}
			m_pSelected_ItemUI = nullptr;
		}

		_bool IsNoOneHover = true;
		CInventory_Slot* pHoveredSlot = nullptr;

		for (_uint i = 0; i < m_iInvenCount; i++)
		{
			if (true == m_vecInvenSlot[i]->IsMouseHover())
			{
				if (m_pHoverdSlot != m_vecInvenSlot[i])
				{
					m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_InvenSlot_Tick.mp3"), CH_TICK_2D, 0.2f);
					m_pHoverdSlot = m_vecInvenSlot[i];
				}
				IsNoOneHover = false;
				pHoveredSlot = m_vecInvenSlot[i];
			}
		}

		if (false == IsNoOneHover)
		{
			_float4 HoveredPos = static_cast<CTransform*>(pHoveredSlot->Get_Component(g_strTransformTag))->Get_State_Float4(CTransform::STATE_POSITION);
			HoveredPos.z = Z_POS_ITEM_UI;
			m_pSlotHighlighterTransform->Set_State(CTransform::STATE_POSITION, HoveredPos); //하이라이터 움직임

			for (auto& iter : m_vecItem_UI)
			{
				if (true == iter->IsMouseHover() && true == iter->Get_isWorking())
				{
					m_CombineResources[COMBINDE_NUM] = iter->Get_ItemNumber();
					m_CombineResources[RESULT_NUM] = Find_Recipe(m_CombineResources[SELECTED_NUM], m_CombineResources[COMBINDE_NUM]);

					if (UP == m_pGameInstance->Get_KeyState(VK_LBUTTON) && true == iter->Get_isActive())
					{
						if (iter != m_pSelected_ItemUI)
						{
							Combind_Item(m_pSelected_ItemUI, iter);
							//iter->Reset_ItemUI();
							//pHoveredSlot->Set_IsFilled(false);
							m_eTaskSequence = APPLY;
						}
					}
					else if(UP == m_pGameInstance->Get_KeyState(VK_LBUTTON) && false == iter->Get_isActive())
					{
						m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_Context_Denied.mp3"), CH1_2D, 0.5f);
					}
				}
			}
		}
		break;
	}
		
	case Client::CInventory_Manager::APPLY: {
		//m_pSelected_ItemUI->Set_ItemNumber(m_CombineResources[RESULT_NUM]);
		if(INTERACT_PROPS == m_ePre_Inven_Manager_State)
			m_eTaskSequence = SETING;
		else
			m_eTaskSequence = TS_END;
		m_eInven_Manager_State = EXAMINE_ITEM;
		m_CombineResources[SELECTED_NUM] = { ITEM_NUMBER_END };
		m_CombineResources[COMBINDE_NUM] = { ITEM_NUMBER_END };
		m_CombineResources[RESULT_NUM] = { ITEM_NUMBER_END };
		for (auto& iter : m_vecItem_UI)
		{
			if(false == iter->Get_isActive() && true == iter->Get_isWorking() )
			{
				iter->Set_Activ_ItemUI(true);
			}
		}
		break;
	}

	default:
		break;
	}

}

void CInventory_Manager::HOTKEY_ASSIGNED_ITEM_Operation(_float fTimeDelta)
{
	switch (m_eTaskSequence)
	{
	case Client::CInventory_Manager::SETING: {
		_float4 HighligeterSetingPos = { m_pHotkey->Get_Empty_Slot()->GetPosition().x, m_pHotkey->Get_Empty_Slot()->GetPosition().y, Z_POS_HIGH_LIGHTER, 1.f };
		m_pSlotHighlighterTransform->Set_State(CTransform::STATE_POSITION, HighligeterSetingPos);
		m_eTaskSequence = SELECT;
		break;
	}
		
	case Client::CInventory_Manager::SELECT: {
		CInventory_Slot* pHoverdSlot = m_pHotkey->Get_Hoverd_Slot();
		if (nullptr != pHoverdSlot)
		{
			if (m_pHoverdSlot != pHoverdSlot)
			{
				m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_InvenSlot_Tick.mp3"), CH_TICK_2D, 0.2f);
				m_pHoverdSlot = pHoverdSlot;
			}

			_float4 HighligeterSetingPos = { pHoverdSlot->GetPosition().x, pHoverdSlot->GetPosition().y, Z_POS_HIGH_LIGHTER, 1.f };
			m_pSlotHighlighterTransform->Set_State(CTransform::STATE_POSITION, HighligeterSetingPos);
			
			if (DOWN == m_pGameInstance->Get_KeyState(VK_LBUTTON))
			{
				_uint iHotKeyNum = m_pHotkey->RegisterHoykey(_float2(HighligeterSetingPos.x, HighligeterSetingPos.y), 
					m_pSelected_ItemUI->Get_ItemNumber(), m_pSelected_ItemUI->Get_ItemQuantity());
				_float4 HighligeterSetingPos = { m_vecInvenSlot[0]->GetPosition().x, m_vecInvenSlot[0]->GetPosition().y, Z_POS_HIGH_LIGHTER, 1.0 };
				m_pSlotHighlighterTransform->Set_State(CTransform::STATE_POSITION, HighligeterSetingPos);
				m_pSelected_ItemUI->Set_Text(HOTKEY_DISPLAY, to_wstring(iHotKeyNum));
				
				m_eTaskSequence = TS_END;
				m_eInven_Manager_State = EVENT_IDLE;
				m_pSelected_ItemUI = nullptr;
				m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_DragDown.mp3"), CH1_2D, 0.5f);
			}
		}
		break;
	}
		
	default:
		break;
	}
}

void CInventory_Manager::REARRANGE_ITEM_Operation(_float fTimeDelta)
{
	switch (m_eTaskSequence)
	{
	case Client::CInventory_Manager::SETING: {
		m_IsNoOneHover = true;
		CInventory_Slot* pHoveredSlot = nullptr;

		for (_uint i = 0; i < m_iInvenCount; i++)
		{
			if (true == m_vecInvenSlot[i]->IsMouseHover())
			{
				if (m_pHoverdSlot != m_vecInvenSlot[i])
				{
					m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_InvenSlot_Tick.mp3"), CH_TICK_2D, 0.2f);
					m_pHoverdSlot = m_vecInvenSlot[i];
				}
				m_IsNoOneHover = false;
				pHoveredSlot = m_vecInvenSlot[i];
			}
		}

		if (false == m_IsNoOneHover)
		{
			_float4 HoveredPos = static_cast<CTransform*>(pHoveredSlot->Get_Component(g_strTransformTag))->Get_State_Float4(CTransform::STATE_POSITION);
			HoveredPos.z = Z_POS_HIGH_LIGHTER;

			m_pSlotHighlighterTransform->Set_State(CTransform::STATE_POSITION, HoveredPos);

			if (PRESSING == m_pGameInstance->Get_KeyState(VK_LBUTTON))
			{
				m_pDragShadowTransform->Set_State(CTransform::STATE_POSITION, HoveredPos);
			}

			if (UP == m_pGameInstance->Get_KeyState(VK_LBUTTON))
			{
				CItem_UI* pOverLapItem = nullptr;

				for (auto& iter : m_vecItem_UI)
				{
					if (true == iter->Get_isWorking() && iter != m_pSelected_ItemUI && true == iter->IsMouseHover())
					{
						pOverLapItem = iter;
					}
				}

				if (nullptr == pOverLapItem)
				{
					_float4 fPrePos = m_pSelected_ItemUI->GetPosition();
					_float3 fMove = { HoveredPos.x - fPrePos.x, HoveredPos.y - fPrePos.y, 0.f };

					m_pSelected_ItemUI->Move(fMove);
					Find_Slot(_float2(fPrePos.x, fPrePos.y))->Set_IsFilled(false);
					Find_Slot(_float2( m_pDragShadow->GetPosition().x, m_pDragShadow->GetPosition().y ))->Set_IsFilled(true);
					
					m_pSlotHighlighter->Set_DragShadow(false);
					m_eInven_Manager_State = EVENT_IDLE;
					m_eTaskSequence = TS_END;
					m_pDragShadow->Set_Dead(true);
					m_pSelected_ItemUI = nullptr;
					m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_DragDown.mp3"), CH1_2D, 0.5f);
				}

				else
				{
					ITEM_NUMBER eResult = Find_Recipe(m_pSelected_ItemUI->Get_ItemNumber(), pOverLapItem->Get_ItemNumber());
					if (ITEM_NUMBER_END == eResult)
					{
						Switch_ItemPos(m_pSelected_ItemUI, pOverLapItem);

						m_pSlotHighlighter->Set_DragShadow(false);
						m_eInven_Manager_State = EVENT_IDLE;
						m_eTaskSequence = TS_END;
						m_pDragShadow->Set_Dead(true);
						m_pSelected_ItemUI = nullptr;
					}

					else
					{
						_float3 TempTrashCanValue = _float3(HoveredPos.x, HoveredPos.y, Z_POS_CONTEXT_MENU);
						m_pContextMenu->Set_Operation(COMBINABLE_PICKED_UP, false, TempTrashCanValue, TempTrashCanValue);
						m_pTemp_ItemUI = pOverLapItem;
						m_eTaskSequence = SELECT;
					}
				}
			}
		}
		break;
	}

	case Client::CInventory_Manager::SELECT: {
		m_pContextMenu->Tick(fTimeDelta);

		INVENTORY_EVENT eInvenEvent = m_pContextMenu->Get_InventoryEvent();

		switch (eInvenEvent)
		{
		case Client::COMBINED_ITEM: {
			Combind_Item(m_pSelected_ItemUI, m_pTemp_ItemUI);
			m_pSlotHighlighter->Set_DragShadow(false);
			m_eInven_Manager_State = EVENT_IDLE;
			m_eTaskSequence = TS_END;
			m_pDragShadow->Set_Dead(true);
			m_pSelected_ItemUI = nullptr;
			m_pTemp_ItemUI = nullptr;
			m_pContextMenu->Set_Dead(true);
			m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_DragDown.mp3"), CH1_2D, 0.5f);
			break;
		}
			
		case Client::SWITCH_ITEM: {
			Switch_ItemPos(m_pSelected_ItemUI, m_pTemp_ItemUI);
			m_pSlotHighlighter->Set_DragShadow(false);
			m_eInven_Manager_State = EVENT_IDLE;
			m_eTaskSequence = TS_END;
			m_pDragShadow->Set_Dead(true);
			m_pSelected_ItemUI = nullptr;
			m_pContextMenu->Set_Dead(true);
			m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_DragDown.mp3"), CH1_2D, 0.5f);
			break;
		}

		default:
			break;
		}
		break;
	}

	default:
		break;
	}
}

void CInventory_Manager::DISCARD_ITEM_Operation(_float fTimeDelta)
{
	m_eInven_Manager_State = EVENT_IDLE;
	m_pContextMenu->Set_Dead(true);
	Find_Slot(_float2(m_pSelected_ItemUI->GetPosition().x, m_pSelected_ItemUI->GetPosition().y))->Set_IsFilled(false);
	m_pSelected_ItemUI->Reset_ItemUI();
	m_pSelected_ItemUI = nullptr;
}

void CInventory_Manager::CONTEXTUI_SELECT_Operation(_float fTimeDelta)
{
	m_pContextMenu->Tick(fTimeDelta);

	switch (m_eTaskSequence)
	{
	case Client::CInventory_Manager::SETING: {
		ITEM_NUMBER eSelectedItemNum = m_pSelected_ItemUI->Get_ItemNumber();
		//이징 스타트 도착 지점 제대로 정해주기
		_float3 TempTrashCanValue = _float3(m_pSelected_ItemUI->GetPosition().x, m_pSelected_ItemUI->GetPosition().y, Z_POS_CONTEXT_MENU);

		ITEM_TYPE eItemType = m_pSelected_ItemUI->Get_ItemType();

		switch (eItemType)
		{
		case Client::EQUIPABLE: {
			_bool isEquipd = m_pSelected_ItemUI->Get_isEquiped();
			m_pContextMenu->Set_Operation(eItemType, isEquipd, TempTrashCanValue, TempTrashCanValue);
			break;
		}
			
		case Client::CONSUMABLE_EQUIPABLE: {
			_bool isEquipd = m_pSelected_ItemUI->Get_isEquiped();
			m_pContextMenu->Set_Operation(eItemType, isEquipd, TempTrashCanValue, TempTrashCanValue);

			break;
		}
			
		case Client::USEABLE: {
			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Player"), 0));
			_bool isCanUse = true;
			if (5 == pPlayer->Get_Hp() && true == IsHealItem())
				isCanUse = false;
			m_pContextMenu->Set_Operation(eItemType, isCanUse, TempTrashCanValue, TempTrashCanValue);
			break;
		}
			
		case Client::CONSUMABLE: {
			m_pContextMenu->Set_Operation(eItemType, true, TempTrashCanValue, TempTrashCanValue);
			break;
		}
			
		case Client::QUEST:{
			m_pContextMenu->Set_Operation(eItemType, true, TempTrashCanValue, TempTrashCanValue);
			break;
		}
			
		default:
			break;
		}

		
		m_eTaskSequence = SELECT;
		break;
	}

	case Client::CInventory_Manager::SELECT: {
		INVENTORY_EVENT eInvenEvent = m_pContextMenu->Get_InventoryEvent();
		switch (eInvenEvent)
		{	
		case Client::EVENT_IDLE:
			break;

		case Client::EQUIP_ITEM: {
			m_eInven_Manager_State = EQUIP_ITEM;
			m_pContextMenu->Set_Dead(true);
			m_eTaskSequence = SETING;
			break;
		}

		case Client::UNEQUIP_ITEM: {
			m_eInven_Manager_State = UNEQUIP_ITEM;
			m_pContextMenu->Set_Dead(true);
			m_eTaskSequence = SETING;
			break;
		}

		case Client::USE_ITEM: {
			m_eInven_Manager_State = USE_ITEM;
			m_pContextMenu->Set_Dead(true);
			m_eTaskSequence = SETING;
			break;
		}

		case Client::EXAMINE_ITEM: {
			m_eInven_Manager_State = EXAMINE_ITEM;
			m_pContextMenu->Set_Dead(true);
			m_eTaskSequence = SETING;
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
			m_eTaskSequence = SETING;
			m_pContextMenu->Set_Dead(true);
			break;
		}

		case Client::REARRANGE_ITEM: {
			m_eInven_Manager_State = REARRANGE_ITEM;
			m_pContextMenu->Set_Dead(true);
			m_eTaskSequence = SETING;
			break;
		}

		case Client::DISCARD_ITEM: {
			m_eInven_Manager_State = DISCARD_ITEM;
			m_pContextMenu->Set_Dead(true);
			m_eTaskSequence = SETING;
			break;
		}

		case Client::CONTEXTUI_SELECT: {
			m_eInven_Manager_State = CONTEXTUI_SELECT;
			m_pContextMenu->Set_Dead(true);
			m_eTaskSequence = SETING;
			break;
		}

		default:
			break;
		}
		break;
	}
		
	default:
		break;
	}

	if (DOWN == m_pGameInstance->Get_KeyState(VK_RBUTTON) || UP == m_pGameInstance->Get_KeyState(VK_ESCAPE))
	{
		m_eInven_Manager_State = EVENT_IDLE;
		m_pContextMenu->Set_Dead(true);
		m_pSelected_ItemUI = nullptr;
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_Context_Close.mp3"), 0.1f);
		return;
	}
}

void CInventory_Manager::INTERACT_ITEM_Operation(_float fTimeDelta)
{
	switch (m_eTaskSequence)
	{
	case Client::CInventory_Manager::SETING: {
		if (UP == m_pGameInstance->Get_KeyState(VK_RBUTTON) || UP == m_pGameInstance->Get_KeyState(VK_ESCAPE))
		{
			m_eInven_Manager_State = EVENT_CANCLE;
			m_ePre_Inven_Manager_State = INVEN_EVENT_END;
			m_pSelected_ItemUI = nullptr;
			break;
		}
		
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
			_float4 HoveredPos = static_cast<CTransform*>(pHoveredSlot->Get_Component(g_strTransformTag))->Get_State_Float4(CTransform::STATE_POSITION);
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

			if (nullptr != m_pSelected_ItemUI)
			{
				if (UP == m_pGameInstance->Get_KeyState(VK_LBUTTON))
				{
					if (true == m_pSelected_ItemUI->IsMouseHover() && true == m_pSelected_ItemUI->Get_isWorking())
					{
						m_eTaskSequence = SELECT;
						_float3 TempTrashCanValue = _float3(HoveredPos.x, HoveredPos.y, Z_POS_CONTEXT_MENU);
						if (m_eRequested_Item == m_pSelected_ItemUI->Get_ItemNumber())
							m_pContextMenu->Set_Operation(INTERACT, true, TempTrashCanValue, TempTrashCanValue);
						else
							m_pContextMenu->Set_Operation(INTERACT, false, TempTrashCanValue, TempTrashCanValue);
					}
				}
			}
		}
		break;
	}
		
		
	case Client::CInventory_Manager::SELECT: {
		if (UP == m_pGameInstance->Get_KeyState(VK_RBUTTON) || UP == m_pGameInstance->Get_KeyState(VK_ESCAPE))
		{
			m_eTaskSequence = SETING;
			m_pContextMenu->Set_Dead(true);
			m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_Context_Close.mp3"), 0.1f);
			break;
		}

		m_pContextMenu->Tick(fTimeDelta);

		INVENTORY_EVENT eInvenEvent = m_pContextMenu->Get_InventoryEvent();

		switch (eInvenEvent)
		{
		case Client::USE_ITEM: {
			m_ePre_Inven_Manager_State = INVEN_EVENT_END;
			m_eInven_Manager_State = USE_INTERACT_ITEM;
			m_eTaskSequence = SETING;
			m_pContextMenu->Set_Dead(true);
			InteractUseItem();
			INTERACT_ITEM_SoundPlay();

			break;
		}

		case Client::COMBINED_ITEM: {
			m_ePre_Inven_Manager_State = m_eInven_Manager_State;
			m_eInven_Manager_State = COMBINED_ITEM;
			m_eTaskSequence = SETING; 
			m_pContextMenu->Set_Dead(true);
			break;
		}

		case Client::EXAMINE_ITEM: {
			m_ePre_Inven_Manager_State = m_eInven_Manager_State;
			m_eInven_Manager_State = EXAMINE_ITEM;
			m_eTaskSequence = SETING;
			m_pContextMenu->Set_Dead(true);
			break;
		}

		default:
			break;
		}
		break;


		break;
	}
		
	case Client::CInventory_Manager::APPLY: {
		break;
	}
		

	default:
		break;
	}

	
}

void CInventory_Manager::InteractUseItem()
{
	ITEM_NUMBER iTEM = m_pSelected_ItemUI->Get_ItemNumber();

	switch (iTEM)
	{
	case Client::woodbarricade01a:
		m_pSelected_ItemUI->Set_ItemVariation(-1);
		if (0 == m_pSelected_ItemUI->Get_ItemQuantity())
		{
			Find_Slot(_float2(m_pSelected_ItemUI->GetPosition().x, m_pSelected_ItemUI->GetPosition().y))->Set_IsFilled(false);
		}
		m_pSelected_ItemUI = nullptr;
		break;
	case Client::unicornmedal01a:
		Find_Slot(_float2(m_pSelected_ItemUI->GetPosition().x, m_pSelected_ItemUI->GetPosition().y))->Set_IsFilled(false);
		m_pSelected_ItemUI->Reset_ItemUI();
		m_pSelected_ItemUI = nullptr;
		break;
	case Client::spadekey01a:
		Find_Slot(_float2(m_pSelected_ItemUI->GetPosition().x, m_pSelected_ItemUI->GetPosition().y))->Set_IsFilled(false);
		m_pSelected_ItemUI = nullptr;
		break;
	case Client::cardkeylv101a:
		Find_Slot(_float2(m_pSelected_ItemUI->GetPosition().x, m_pSelected_ItemUI->GetPosition().y))->Set_IsFilled(false);
		m_pSelected_ItemUI->Reset_ItemUI();
		m_pSelected_ItemUI = nullptr;
		break;
	case Client::cardkeylv201a:
		Find_Slot(_float2(m_pSelected_ItemUI->GetPosition().x, m_pSelected_ItemUI->GetPosition().y))->Set_IsFilled(false);
		m_pSelected_ItemUI->Reset_ItemUI();
		m_pSelected_ItemUI = nullptr;
		break;
	case Client::blankkey01a:
		m_pSelected_ItemUI->Set_ItemVariation(-1);
		if (0 == m_pSelected_ItemUI->Get_ItemQuantity())
		{
			Find_Slot(_float2(m_pSelected_ItemUI->GetPosition().x, m_pSelected_ItemUI->GetPosition().y))->Set_IsFilled(false);
		}
		m_pSelected_ItemUI = nullptr;
		break;
	case Client::virginmedal01a:
		Find_Slot(_float2(m_pSelected_ItemUI->GetPosition().x, m_pSelected_ItemUI->GetPosition().y))->Set_IsFilled(false);
		m_pSelected_ItemUI->Reset_ItemUI();
		m_pSelected_ItemUI = nullptr;
		break;
	case Client::diakey01a:
		Find_Slot(_float2(m_pSelected_ItemUI->GetPosition().x, m_pSelected_ItemUI->GetPosition().y))->Set_IsFilled(false);
		m_pSelected_ItemUI = nullptr;
		break;
	case Client::virginmedal02a:
		Find_Slot(_float2(m_pSelected_ItemUI->GetPosition().x, m_pSelected_ItemUI->GetPosition().y))->Set_IsFilled(false);
		m_pSelected_ItemUI->Reset_ItemUI();
		m_pSelected_ItemUI = nullptr;
		break;
	case Client::chaincutter01a:
		Find_Slot(_float2(m_pSelected_ItemUI->GetPosition().x, m_pSelected_ItemUI->GetPosition().y))->Set_IsFilled(false);
		m_pSelected_ItemUI->Reset_ItemUI();
		m_pSelected_ItemUI = nullptr;
		break;
	case Client::statuebookhand:
		Find_Slot(_float2(m_pSelected_ItemUI->GetPosition().x, m_pSelected_ItemUI->GetPosition().y))->Set_IsFilled(false);
		m_pSelected_ItemUI->Reset_ItemUI();
		m_pSelected_ItemUI = nullptr;
		break;

	default:
		break;
	}

	m_pSelected_ItemUI->Set_ItemVariation(-1);
	if (0 == m_pSelected_ItemUI->Get_ItemQuantity())
	{
		Find_Slot(_float2(m_pSelected_ItemUI->GetPosition().x, m_pSelected_ItemUI->GetPosition().y))->Set_IsFilled(false);
	}
	m_pSelected_ItemUI = nullptr;
}

void CInventory_Manager::Switch_ItemPos(CItem_UI* FirstItemUI, CItem_UI* SecondItemUI)
{
	_float4 fFirstPos = FirstItemUI->GetPosition();
	_float4 fSecondPos = SecondItemUI->GetPosition();

	_float3 fFirstMove = { fSecondPos.x - fFirstPos.x, fSecondPos.y - fFirstPos.y , 0.f};
	_float3 fSecondMove = { fFirstPos.x - fSecondPos.x, fFirstPos.y - fSecondPos.y , 0.f };

	FirstItemUI->Move(fFirstMove);
	SecondItemUI->Move(fSecondMove);
}

void CInventory_Manager::Combind_Item(CItem_UI* FirstItemUI, CItem_UI* SecondItemUI)
{
	ITEM_NUMBER eFirstNum = FirstItemUI->Get_ItemNumber();
	ITEM_NUMBER eSecondNum = SecondItemUI->Get_ItemNumber();
	ITEM_NUMBER eItemResult = Find_Recipe(eFirstNum, eSecondNum);

	if (handgun_bullet01a == eFirstNum && handgun_bullet01a == eSecondNum && handgun_bullet01a == eItemResult) {
		FirstItemUI->Set_ItemVariation(SecondItemUI->Get_ItemQuantity());//권총 총알
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_Drop_Bullet.mp3"), CH3_2D, 0.5f);
	}

	else if (shotgun_bullet01a == eFirstNum && shotgun_bullet01a == eSecondNum && shotgun_bullet01a == eItemResult) {
		FirstItemUI->Set_ItemVariation(SecondItemUI->Get_ItemQuantity());//샷건 총알
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_Drop_ShotgunShell.mp3"), CH3_2D, 0.5f);
	}


	else if (HandGun == eFirstNum && vp70powerup == eSecondNum && HandGun == eItemResult){
		CGameObject* pTabWindow = m_pGameInstance->Get_GameObject(g_Level, TEXT("Layer_TabWindow"), 0);
		static_cast<CTab_Window*>(pTabWindow)->Set_Weapon_Accessories(HandGun, 1); //소염기
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_EquipHandGun.mp3"), CH3_2D, 0.5f);
	}

	else if (HandGun == eFirstNum && vp70longmagazine == eSecondNum && HandGun == eItemResult){
		CGameObject* pTabWindow = m_pGameInstance->Get_GameObject(g_Level, TEXT("Layer_TabWindow"), 0);
		static_cast<CTab_Window*>(pTabWindow)->Set_Weapon_Accessories(HandGun, 2); //대용량 탄창
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_EquipHandGun.mp3"), CH3_2D, 0.5f);
	}

	else if (HandGun == eFirstNum && vp70stock == eSecondNum && HandGun == eItemResult){
		CGameObject* pTabWindow = m_pGameInstance->Get_GameObject(g_Level, TEXT("Layer_TabWindow"), 0);
		static_cast<CTab_Window*>(pTabWindow)->Set_Weapon_Accessories(HandGun, 0); //개머리판
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_EquipHandGun.mp3"), CH3_2D, 0.5f);
	}

	else if (ShotGun == eFirstNum && shotgunpartsstock_00 == eSecondNum && ShotGun == eItemResult){
		CGameObject* pTabWindow = m_pGameInstance->Get_GameObject(g_Level, TEXT("Layer_TabWindow"), 0);
		static_cast<CTab_Window*>(pTabWindow)->Set_Weapon_Accessories(ShotGun, 1);
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_EquipShotgun.mp3"), CH3_2D, 0.5f);
	}

	else if (ShotGun == eFirstNum && shotgunpartsbarrel == eSecondNum && ShotGun == eItemResult){
		CGameObject* pTabWindow = m_pGameInstance->Get_GameObject(g_Level, TEXT("Layer_TabWindow"), 0);
		static_cast<CTab_Window*>(pTabWindow)->Set_Weapon_Accessories(ShotGun, 0);
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_EquipShotgun.mp3"), CH3_2D, 0.5f);
	}

	else
		FirstItemUI->Set_ItemNumber(eItemResult);

	switch (eItemResult)
	{
	case Client::herbsgg01a:
	case Client::herbsgr01a:
	case Client::herbsgb01a:
	case Client::herbsggb01a:
	case Client::herbsggg01a:
	case Client::herbsgrb01a:
	case Client::herbsrb01a:
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_UseItem_herbs.mp3"), CH3_2D, 0.5f);
		break;
	case Client::handgun_bullet01a:
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_Drop_Bullet.mp3"), CH3_2D, 0.5f);
		break;
	case Client::shotgun_bullet01a:
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_Drop_ShotgunShell.mp3"), CH3_2D, 0.5f);
		break;

	case Client::statuebookhand:
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_Drop_Rock.mp3"), CH3_2D, 0.5f);
		break;

	default:
		break;
	}

	_float2 fFindPos = { SecondItemUI->GetPosition().x , SecondItemUI->GetPosition().y };
	//_float2 fFindPos = { FirstItemUI->GetPosition().x , FirstItemUI->GetPosition().y };
	Find_Slot(fFindPos)->Set_IsFilled(false);
	//Switch_ItemPos(FirstItemUI, SecondItemUI);
	//Switch_ItemPos(SecondItemUI, FirstItemUI);

	SecondItemUI->Reset_ItemUI();
}


CInventory_Slot* CInventory_Manager::Find_Slot(_float2 FindPos)
{
	for (auto& Slotiter : m_vecInvenSlot)
	{
		if (true == Slotiter->IsPTInRect(_float2(FindPos.x, FindPos.y)))
		{
			Slotiter->Set_IsFilled(true);
			return Slotiter;
		}
	}

	return nullptr;
}

_bool CInventory_Manager::IsHealItem()
{
	ITEM_NUMBER iTemNum = m_pSelected_ItemUI->Get_ItemNumber();

	if (iTemNum < blueherbitem01a)
		return true;

	return false;
}

void CInventory_Manager::INTERACT_ITEM_SoundPlay()
{
	ITEM_NUMBER eItemNum = m_pSelected_ItemUI->Get_ItemNumber();
	switch (eItemNum)
	{
	case Client::woodbarricade01a:
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_Interect_Wood.mp3"), CH1_2D, 0.5f);
		break;
	case Client::blastingfuse01a:
		break;
	case Client::_9vbattery01a:
		break;
	case Client::unicornmedal01a:
		break;
	case Client::spadekey01a:
		break;
	case Client::cardkeylv101a:
		break;
	case Client::cardkeylv201a:
		break;
	case Client::valvehandle01a:
		break;
	case Client::kingscepter01a:
		break;
	case Client::virginheart01a:
		break;
	case Client::blankkey01a:
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_Interact_blankkey.mp3"), CH1_2D, 0.5f);
		break;
	case Client::virginmedal01a:
		break;
	case Client::diakey01a:
		break;
	case Client::virginmedal02a:
		break;
	case Client::statuebookhand:
		break;
	case Client::ITEM_NUMBER_END:
		break;
	default:
		break;
	}
}

void CInventory_Manager::Set_OnOff_Inven(_bool bInput)
{
	if (false == bInput)
	{
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_Inven_Open.mp3"), CH1_2D, 0.2f);
	}
	else
	{
		m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_Inven_Close.mp3"), CH1_2D, 0.2f);
	}


	for (_uint i = 0; i < m_iInvenCount; i++)
	{
		m_vecInvenSlot[i] -> Set_Dead(bInput);
	}

	m_pSlotHighlighter->Set_Dead(bInput);
	m_pSlotHighlighter->Set_DragShadow(false);

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
			m_pSlotHighlighter->ResetPosition(m_fSlotHighlighterResetPos);
		}
	}

	m_pContextMenu->Set_Dead(true);
	m_pDragShadow->Set_Dead(true);

	if(INTERACT_PROPS == m_ePre_Inven_Manager_State)
		m_eInven_Manager_State = INTERACT_PROPS;

	else
		m_eInven_Manager_State = EVENT_IDLE;

	//m_pInven_Item_UI->Reset_Call(!bInput);
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
	Set_OnOff_Inven(false);
	Set_InventoryEvent(PICK_UP_ITEM);

	CInventory_Slot* pEmptydSlot = nullptr;

	for (_uint i = 0; i < m_iInvenCount; i++)
	{
		if (false == m_vecInvenSlot[i]->Get_IsFilled())
		{
			pEmptydSlot = m_vecInvenSlot[i];
			break;
		}
	}

	if (nullptr == pEmptydSlot)
	{
		pEmptydSlot = m_vecInvenSlot[0];
	}

	m_pSlotHighlighter->Set_Dead(false);
	m_pSlotHighlighter->Set_DragShadow(true);

	_float4 EmptyPos = static_cast<CTransform*>(pEmptydSlot->Get_Component(g_strTransformTag))->Get_State_Float4(CTransform::STATE_POSITION);
	EmptyPos.z = Z_POS_HIGH_LIGHTER;
	m_pSlotHighlighterTransform->Set_State(CTransform::STATE_POSITION, EmptyPos);

	_vector TempTrashCanValue = XMVectorSet(EmptyPos.x, EmptyPos.y, Z_POS_ITEM_UI, 1.f);
	m_pDragShadow->Set_ItemUI(eAcquiredItem, DRAG_SHADOW, TempTrashCanValue, iItemQuantity);
	m_pDragShadow->Set_Dead(false);
	m_eTaskSequence = SETING;
}

void CInventory_Manager::IIO_Seting(ITEM_NUMBER eRequiredItem)
{
	Set_OnOff_Inven(false);
	Set_InventoryEvent(INTERACT_PROPS);
	m_eRequested_Item = eRequiredItem;
	m_eTaskSequence = SETING;
}

void CInventory_Manager::UseItem(ITEM_NUMBER eTargetItemNum, _int iUsage)
{
	for (auto& iter : m_vecItem_UI)
	{
		if (eTargetItemNum == iter->Get_ItemNumber() && true == iter->Get_isWorking())
		{
			iter->Set_ItemVariation(-iUsage);
			m_pHotkey->Update_Registed_Item(eTargetItemNum, iter->Get_ItemQuantity());
			if (0 == iter->Get_ItemQuantity() && HandGun != iter->Get_ItemNumber() && ShotGun != iter->Get_ItemNumber())
			{
				Find_Slot(_float2{ iter->GetPosition().x, iter->GetPosition().y })->Set_IsFilled(false);
			}
			
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

void CInventory_Manager::AddItem_ToInven(ITEM_NUMBER eAcquiredItem, _int iItemQuantity, _float3 fItemPos)
{
	for (auto& Itemiter : m_vecItem_UI)
	{
		if (false == Itemiter->Get_isWorking())
		{
			Itemiter->Set_ItemUI(eAcquiredItem, ItemType_Classify_ByNumber(eAcquiredItem), XMVectorSet(fItemPos.x, fItemPos.y, Z_POS_ITEM_UI, 1.f), iItemQuantity);
			Itemiter->Set_Dead(false);
			if (ShotGun == eAcquiredItem)
			{
				_uint iHotKeyNum = m_pHotkey->RegisterHoykey(1, Itemiter->Get_ItemNumber(), Itemiter->Get_ItemQuantity());
				Itemiter->Set_Text(HOTKEY_DISPLAY, to_wstring(iHotKeyNum));
			}

			break;
		}
	}

	for (auto& Slotiter : m_vecInvenSlot)
	{
		if (true == Slotiter->IsPTInRect(_float2(fItemPos.x, fItemPos.y)))
		{
			Slotiter->Set_IsFilled(true);
			break;
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

void CInventory_Manager::Set_Port_To_Blank()
{
	if (m_pSelected_ItemUI->Get_ItemNumber() == portablesafe)
	{
		m_pSelected_ItemUI->Set_ItemUI(blankkey01a, ItemType_Classify_ByNumber(blankkey01a), m_pSelected_ItemUI->GetPositionVector(), 1);
	}
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

	m_pSlotHighlighter->CCustomize_UI::Set_Dead(true);
	m_pSlotHighlighterTransform = dynamic_cast<CTransform*>(m_pSlotHighlighter->Get_Component(g_strTransformTag));

	m_fSlotHighlighterResetPos = m_vecInvenSlot[0]->GetPosition();
	m_fSlotHighlighterResetPos.z = Z_POS_HIGH_LIGHTER;

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

HRESULT CInventory_Manager::Seting_Hotkey()
{
	list<CGameObject*>* pListObj = 	m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));

	for (auto& iter : *pListObj)
	{
		CHotKey* pHotkey = nullptr;

		pHotkey = dynamic_cast<CHotKey*>(iter);

		if (nullptr != pHotkey)
		{
			m_pHotkey = pHotkey;
			Safe_AddRef(m_pHotkey);
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
	Safe_Release(m_pHotkey);
}

ITEM_TYPE CInventory_Manager::ItemType_Classify_ByNumber(ITEM_NUMBER eItemNum)
{
	switch (eItemNum)
	{
	case Client::emergencyspray01a:
	case Client::greenherb01a:
	case Client::redherb01a:
	case Client::blueherb01a:
	case Client::herbsgg01a:
	case Client::herbsgr01a:
	case Client::herbsgb01a:
	case Client::herbsggb01a:
	case Client::herbsggg01a:
	case Client::herbsgrb01a:
	case Client::herbsrb01a:
	case Client::greenherbitem01a:
	case Client::redherbitem01a:
	case Client::blueherbitem01a:
		return USEABLE;
		break;

	case Client::handgun_bullet01a:
		return CONSUMABLE;
		break;
	case Client::shotgun_bullet01a:
		return CONSUMABLE;
		break;
	case Client::woodbarricade01a:
		return CONSUMABLE;
		break;
	case Client::blastingfuse01a:
		return QUEST;
		break;
	case Client::_9vbattery01a:
		return QUEST;
		break;
	case Client::gunpowder01a:
		return CONSUMABLE;
		break;
	case Client::gunpowder01b:
		return CONSUMABLE;
		break;
	case Client::strengtheningyellow01a:
		return CONSUMABLE;
		break;

	case Client::vp70powerup:
	case Client::vp70longmagazine:
	case Client::shotgunpartsstock_00:
	case Client::shotgunpartsbarrel:
	case Client::unicornmedal01a:
	case Client::spadekey01a:
	case Client::cardkeylv101a:
	case Client::cardkeylv201a:
	case Client::valvehandle01a:
	case Client::kingscepter01a:
	case Client::virginheart01a:
	case Client::blankkey01a:
		return QUEST;
		break;

	case Client::statuebook01a:
	case Client::statuehand01a:
		return CONSUMABLE;
		break;

	case Client::virginmedal01a:
	case Client::diakey01a:
	case Client::virginmedal02a:
	case Client::chaincutter01a:
		return QUEST;
		break;

	case Client::rpddocument01a: // DOCUMENTSTART
	case Client::rpddocumentblood01a:
	case Client::diary01a:
	case Client::document01a:
	case Client::pamphlet01a:
	case Client::guidepamphlet01a:
	case Client::memo01a:
	case Client::raccoonmonthly01a:
	case Client::sewercopamphlet01a:
	case Client::report01a:
	case Client::nestlcokout01a:
	case Client::sewerhintposter01a:
	case Client::rpdreport01a:
	case Client::rpdreport01b:
	case Client::chesshints01a:
	case Client::labopc01a:
	case Client::labopc01b:
	case Client::labopc01c:
	case Client::raccoonfigure01a:
	case Client::voicerecorder01a:
	case Client::mappolice01a:
	case Client::mapunderground01a:
	case Client::mapsewer01a:
	case Client::mapraccooncity01a:
	case Client::maplaboratoryhigh01a:
	case Client::maplaboratorymiddle01a:
	case Client::mapupperpolice01a: //DOCUMENTEND
		return DOCUMENT;
		break;

	case Client::clairesbag01a:
		return USEABLE;
		break;
	case Client::HandGun:
		return EQUIPABLE;
		break;
	case Client::ShotGun:
		return EQUIPABLE;
		break;
	case Client::Flash_Bomb:
		return CONSUMABLE_EQUIPABLE;
		break;
	case Client::Grenade:
		return CONSUMABLE_EQUIPABLE;
		break;
	case Client::vp70stock:
		return QUEST;
		break;
	case Client::portablesafe:
		return QUEST;
		break;
	case Client::statuebookhand:
		return QUEST;
		break;

	default:
		break;
	}

	return ITEM_TYPE::INVEN_ITEM_TYPE_END;
}

_uint CInventory_Manager::PickUpItem_Quantity_Classify(ITEM_NUMBER eItemNum)
{
	switch (eItemNum)
	{
	case Client::emergencyspray01a:
	case Client::greenherb01a:
	case Client::redherb01a:
	case Client::blueherb01a:
	case Client::herbsgg01a:
	case Client::herbsgr01a:
	case Client::herbsgb01a:
	case Client::herbsggb01a:
	case Client::herbsggg01a:
	case Client::herbsgrb01a:
	case Client::herbsrb01a:
	case Client::greenherbitem01a:
	case Client::redherbitem01a:
	case Client::blueherbitem01a:
		return 1;
		break;

	case Client::handgun_bullet01a:
		return CGameInstance::Get_Instance()->GetRandom_Int(1, 10);
		break;
	case Client::shotgun_bullet01a:
		return CGameInstance::Get_Instance()->GetRandom_Int(1, 10);
		break;

	case Client::woodbarricade01a:
	case Client::blastingfuse01a:
	case Client::_9vbattery01a:
	case Client::gunpowder01a:
	case Client::gunpowder01b:
	case Client::strengtheningyellow01a:
	case Client::vp70powerup:
	case Client::vp70longmagazine:
	case Client::shotgunpartsstock_00:
	case Client::shotgunpartsbarrel:
	case Client::unicornmedal01a:
	case Client::spadekey01a:
	case Client::cardkeylv101a:
	case Client::cardkeylv201a:
	case Client::valvehandle01a:
	case Client::kingscepter01a:
	case Client::virginheart01a:
	case Client::blankkey01a:
	case Client::statuebook01a:
	case Client::statuehand01a:
	case Client::virginmedal01a:
	case Client::diakey01a:
	case Client::virginmedal02a:
	case Client::chaincutter01a:
	case Client::clairesbag01a:
		return 1;
		break;


	case Client::HandGun:
		return 15;
		break;
	case Client::ShotGun:
		return 7;
		break;

	case Client::Flash_Bomb:
	case Client::Grenade:
	case Client::vp70stock:
	case Client::portablesafe:
	case Client::statuebookhand:
		return 1;
		break;

	default:
		break;
	}



	return _uint();
}

void CInventory_Manager::Set_ItemRecipe()
{
#pragma region 허브 조합식
	Add_Recipe(greenherb01a, greenherb01a,	herbsgg01a);
	Add_Recipe(greenherb01a, redherb01a,	herbsgr01a);
	Add_Recipe(greenherb01a, blueherb01a,	herbsgb01a);
	Add_Recipe(greenherb01a, herbsgg01a,	herbsggg01a);
	Add_Recipe(greenherb01a, herbsgb01a,	herbsggb01a);
	Add_Recipe(greenherb01a, herbsrb01a,    herbsgrb01a);

	Add_Recipe(redherb01a,	 greenherb01a,  herbsgr01a);
	Add_Recipe(redherb01a,   blueherb01a,   herbsrb01a);
	Add_Recipe(redherb01a,   herbsgb01a,    herbsgrb01a);

	Add_Recipe(blueherb01a,	 greenherb01a,  herbsgb01a);
	Add_Recipe(blueherb01a,	 herbsgg01a,    herbsggb01a);
	Add_Recipe(blueherb01a,  herbsgr01a,    herbsgrb01a);

	Add_Recipe(herbsgg01a,   greenherb01a,  herbsggg01a);
	Add_Recipe(herbsgg01a,   blueherb01a,   herbsggb01a);

	Add_Recipe(herbsgb01a,   greenherb01a,  herbsggb01a);
	Add_Recipe(herbsgb01a,   redherb01a,    herbsgrb01a);

	Add_Recipe(herbsrb01a,   greenherb01a,  herbsgrb01a);
#pragma endregion

#pragma region 탄알 조합식
	Add_Recipe(handgun_bullet01a, handgun_bullet01a, handgun_bullet01a);
	Add_Recipe(shotgun_bullet01a, shotgun_bullet01a, shotgun_bullet01a);
#pragma endregion

#pragma region 탄알 조합식
	Add_Recipe(woodbarricade01a, woodbarricade01a, woodbarricade01a);
#pragma endregion

#pragma region 조각상 기믹
	Add_Recipe(statuehand01a, statuebook01a, statuebookhand);
	Add_Recipe(statuebook01a, statuehand01a, statuebookhand);
#pragma endregion

#pragma region 권총 조합식
	Add_Recipe(HandGun, vp70stock, HandGun);
	Add_Recipe(HandGun, vp70powerup, HandGun);
	Add_Recipe(HandGun, vp70longmagazine, HandGun);

	Add_Recipe(vp70stock, HandGun, HandGun);
	Add_Recipe(vp70powerup, HandGun, HandGun);
	Add_Recipe(vp70longmagazine, HandGun, HandGun);
#pragma endregion

#pragma region 샷건 조합식
	Add_Recipe(ShotGun, shotgunpartsstock_00, ShotGun);

	Add_Recipe(ShotGun, shotgunpartsbarrel, ShotGun);

	Add_Recipe(shotgunpartsstock_00, ShotGun, ShotGun);

	Add_Recipe(shotgunpartsbarrel, ShotGun, ShotGun);
#pragma endregion

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

