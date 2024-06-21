#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "Button_UI.h"
#include "Inventory_Slot.h"
#include "Slot_Highlighter.h"
#include "Item_UI.h"

BEGIN(Client)

class CInventory_Manager final : public CBase
{
public:
	enum INVENTORY_EVENT {  };
private:
	CInventory_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CInventory_Manager() = default;

public:
	HRESULT Initialize();
	void FirstTick_Seting();
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);

public:
	//Set_Dead호출이라 m_bDead기준으로 변수 줄것
	void Set_OnOff_Inven(_bool bInput);

	void Increase_Slot(_uint iAmount) {
		if (m_iInvenMaxCount <= m_iInvenCount + iAmount)
			m_iInvenCount = m_iInvenMaxCount;
		else
			m_iInvenCount += iAmount;
	}

	_bool Get_isItemExamine() const {
		return m_bisItemExamin;
	}
	void Set_isItemExamine(_bool isItemExamin) {
		m_bisItemExamin = isItemExamin;
	}

	//아이탬 인벤토리에 넣기
	_bool AddItem_ToInven(ITEM_NUMBER eAcquiredItem);

	//만약 아이템을 넣을수 없는 상황이라면 false를 반환함
	_bool IsCan_AddItem_ToInven();

private: 
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };
	CGameInstance*					m_pGameInstance = { nullptr }; 

	_bool							m_bisItemExamin = { false };

private:
	/*for. InvenSlot*/
	vector<CInventory_Slot*>		m_vecInvenSlot;
	_uint							m_iInvenCount = { 8 };
	_uint                           m_iInvenMaxCount = { 20 };
	_float2							m_fSlotInterval = { 74.f, 76.f };

	/*for Highlighter*/
	CSlot_Highlighter*				m_pSlotHighlighter = { nullptr };
	CTransform*						m_pSlotHighlighterTransform = { nullptr };
	_float4							m_fSlotHighlighterResetPos = {};

	/*for. Item_UI*/
	vector<CItem_UI*>				m_vecItem_UI;

private:
	HRESULT Init_InvenSlot();
	HRESULT Init_SlotHighlighter();
	HRESULT Init_ItemUI();

	HRESULT Create_InvenSlot(vector<CCustomize_UI::CUSTOM_UI_DESC>* vecInvenUI, _float3 fInterval);
	HRESULT Create_SlotHighlighter(vector<CCustomize_UI::CUSTOM_UI_DESC>* vecInvenUI);
	HRESULT Create_ItemUI(vector<CCustomize_UI::CUSTOM_UI_DESC>* vecInvenUI);

public:
	static CInventory_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END