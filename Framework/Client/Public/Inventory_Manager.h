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
	enum INVEN_STATE{ NON_SELECT, SELECT, STATE_END };

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

private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };
	CGameInstance*					m_pGameInstance = { nullptr }; 

private:
	vector<CInventory_Slot*>		m_vecInvenSlot;
	vector<CItem_UI*>				m_vecItem_UI;

	CSlot_Highlighter*				m_pSlotHighlighter = { nullptr };
	CTransform*						m_pSlotHighlighterTransform = { nullptr };
	_float4							m_fSlotHighlighterResetPos = {};

	_uint							m_iInvenCount = { 8 };

	_float2							m_fSlotInterval = { 74.f, 76.f };

public:
	HRESULT Create_InvenUI(vector<CCustomize_UI::CUSTOM_UI_DESC>* vecInvenUI, _float3 fInterval);
	HRESULT Create_SelectUI(vector<CCustomize_UI::CUSTOM_UI_DESC>* vecInvenUI);
	HRESULT Create_ItemUI(vector<CCustomize_UI::CUSTOM_UI_DESC>* vecInvenUI);

	static CInventory_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END