#pragma once

#include "Base.h"
#include "Client_Defines.h"
#include "Button_UI.h"
#include "Inventory_Slot.h"
#include "InventorySelect.h"
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
	void Set_OnOff_Inven(_bool bInput);

private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };
	CGameInstance*					m_pGameInstance = { nullptr }; 

private:
	vector<CInventory_Slot*>		m_vecInventoryUI;
	vector<CItem_UI*>				m_vecItem_UI;

	CInventorySelect*				m_pSelectBox = { nullptr };
	CTransform*						m_pSelectBoxTransform = { nullptr };

	_uint							m_iInvenNum = { 8 };

	_float2							m_fRectInterval = { 74.f, 76.f };
	_float4							m_fSelectBoxResetPos = {};

public:
	HRESULT Create_InvenUI(vector<CCustomize_UI::CUSTOM_UI_DESC>* vecInvenUI, _float3 fInterval);
	HRESULT Create_SelectUI(vector<CCustomize_UI::CUSTOM_UI_DESC>* vecInvenUI);
	HRESULT Create_ItemUI(vector<CCustomize_UI::CUSTOM_UI_DESC>* vecInvenUI);

	static CInventory_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END