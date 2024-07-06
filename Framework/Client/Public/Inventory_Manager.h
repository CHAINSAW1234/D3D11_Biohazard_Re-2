#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "Button_UI.h"
#include "Inventory_Slot.h"
#include "Slot_Highlighter.h"
#include "Item_UI.h"
#include "ContextMenu.h"

BEGIN(Client)

class CInventory_Manager final : public CBase
{
	enum TASK_SEQUENCE{ SETING, SELECT, APPLY, TS_END  };

private:
	CInventory_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CInventory_Manager() = default;

public:
	HRESULT Initialize();
	void FirstTick_Seting();
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

private:
	//평상시
	void EVENT_IDLE_Operation(_float fTimeDelta);			
	//장비 장착
	void EQUIP_ITEM_Operation(_float fTimeDelta);			
	//장비 해제
	void UNEQUIP_ITEM_Operation(_float fTimeDelta);
	//아이템 줍기
	void PICK_UP_ITEM_Operation(_float fTimeDelta);
	//아이템 사용
	void USE_ITEM_Operation(_float fTimeDelta);				
	//아이템 조합
	void COMBINED_ITEM_Operation(_float fTimeDelta);		
	//단축키 등록
	void HOTKEY_ASSIGNED_ITEM_Operation(_float fTimeDelta); 
	//아이템 드래그 드랍
	void REARRANGE_ITEM_Operation(_float fTimeDelta);		
	//아이템 삭제
	void DISCARD_ITEM_Operation(_float fTimeDelta);			
	//콘텍스트 UI
	void CONTEXTUI_SELECT_Operation(_float fTimeDelta);


public:
	//Set_Dead호출이라 m_bDead기준으로 변수 줄것
	void Set_OnOff_Inven(_bool bInput);

	void Increase_Slot(_uint iAmount) {
		if (m_iInvenMaxCount <= m_iInvenCount + iAmount)
			m_iInvenCount = m_iInvenMaxCount;
		else
			m_iInvenCount += iAmount;
	}

	INVENTORY_EVENT Get_InventoryEvent() const {
		return m_eInven_Manager_State;
	}

	void Set_InventoryEvent(INVENTORY_EVENT eInvenEvent) {
		m_eInven_Manager_State = eInvenEvent;
	}

	ITEM_NUMBER Get_Selected_ItemNum();

	_bool* Get_NoHover_InvenBox() { return &m_IsNoOneHover; }

public:
	//Pick_Up_Operation 세팅
	void PUO_Seting(ITEM_NUMBER eAcquiredItem, _int iItemQuantity = 1);

	//인벤토리 밖에서 아이템을 사용하게 되었을때 쓰는 함수(ex 총알)
	void UseItem(ITEM_NUMBER eTargetItemNum, _int iUsage);

	//아이탬 인벤토리에 넣기
	void AddItem_ToInven(ITEM_NUMBER eAcquiredItem, _int iItemQuantity = 1);
	void AddItem_ToInven(ITEM_NUMBER eAcquiredItem, _int iItemQuantity, _float3 fItemPos);

	//만약 아이템을 넣을수 없는 상황이라면 false를 반환함
	_bool IsCan_AddItem_ToInven();

	//원하는 아이템의 수량 or 장탄을 반환함
	_int Get_Search_Item_Quantity(ITEM_NUMBER eItemNum);

	//몇번 단축키에 몇번 아이템이 있는가
	ITEM_NUMBER Get_Item_On_HotKey(_uint iHotKeyNum);

	_float4 Get_SelectedUIPos() {
		if (nullptr != m_pSelected_ItemUI)
		{
			CTransform* pTransform = static_cast<CTransform*>(m_pSelected_ItemUI->Get_Component(g_strTransformTag));
			return pTransform->Get_State_Float4(CTransform::STATE_POSITION);
		}
	}


private: 
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };
	CGameInstance*					m_pGameInstance = { nullptr }; 

	INVENTORY_EVENT					m_eInven_Manager_State = { EVENT_IDLE };

	TASK_SEQUENCE					m_eTaskSequence = { TS_END };

private:
	/* for. InvenSlot */
	vector<CInventory_Slot*>		m_vecInvenSlot;
	_uint							m_iInvenCount = { 8 };
	_uint                           m_iInvenMaxCount = { 20 };

	/* for Highlighter */
	CSlot_Highlighter*				m_pSlotHighlighter = { nullptr };
	CTransform*						m_pSlotHighlighterTransform = { nullptr };
	_float4							m_fSlotHighlighterResetPos = {};

	/* for. Item_UI */
	vector<CItem_UI*>				m_vecItem_UI;
	CItem_UI*						m_pSelected_ItemUI = { nullptr };
	
	/*for. Drag_Shadow*/
	CItem_UI*						m_pDragShadow = { nullptr };
	CTransform*						m_pDragShadowTransform = { nullptr };

	/* for. ContextMenu */
	CContextMenu*					m_pContextMenu = { nullptr };

	/* for. SubInven */
	class CInventory_Item_UI*		m_pInven_Item_UI = { nullptr };

	/*for. PickUpItem*/
	_int							m_PickResult = { -1 };//-1이면 없음 0이면 조합 1이면 자리바꾸기
	_float2							m_fSwitchTargetPos = { 0.f, 0.f };
	_float							m_fItemSwitchTime = { 0.f };

private :	
	/*for IDLE_Operation*/
	_bool							m_IsNoOneHover = { true };
	_float							m_fPressingTime = { 0.f };


	/*for. COMBINED_ITEM_Operation*/
	unordered_map<ITEM_NUMBER, vector<ITEM_RECIPE>> m_mapItemRecipe;

	ITEM_NUMBER						m_CombineResources[3] = {ITEM_NUMBER_END, ITEM_NUMBER_END,ITEM_NUMBER_END };


private:
	HRESULT Init_InvenSlot();
	HRESULT Init_SlotHighlighter();
	HRESULT Init_ItemUI();
	HRESULT Init_DragShdow();
	HRESULT Init_ContextMenu();

	HRESULT Seting_SubInven();
	HRESULT Create_InvenSlot(vector<CCustomize_UI::CUSTOM_UI_DESC>* vecInvenUI, _float3 fInterval);



public:
	static CInventory_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
	static ITEM_TYPE ItemType_Classify_ByNumber(ITEM_NUMBER eItemNum);

	void Set_ItemRecipe();
	void Add_Recipe(ITEM_NUMBER eKeyItemNum, ITEM_NUMBER eCombinableItemNum, ITEM_NUMBER eResultItemNum);
	ITEM_NUMBER Find_Recipe(ITEM_NUMBER eKeyItemNum, ITEM_NUMBER eCombinableItemNum);
};

END