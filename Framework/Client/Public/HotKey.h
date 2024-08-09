#pragma once

#include "UI.h"

#include "Inventory_Slot.h"
#include "Item_UI.h"

BEGIN(Client)

class CHotKey final : public CUI
{
public:
	enum WEAPON_CLASSIFY{EQUIPPED, THROWN, WC_END};

protected:
	CHotKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHotKey(const CHotKey& rhs);
	virtual ~CHotKey() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Start() override;
	void Second_Start();
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Set_Dead(_bool bDead) override;
	void PopUp_Call();

	CInventory_Slot* Get_Hoverd_Slot();
	CInventory_Slot* Get_Empty_Slot();
	_uint RegisterHoykey(_float2 RegisterPos, ITEM_NUMBER eItemNum, _int iItemQuantity);
	_uint RegisterHoykey(_uint iHotkeyNum, ITEM_NUMBER eItemNum, _int iItemQuantity);
	void Update_Registed_Item(ITEM_NUMBER eItemNum, _int iItemQuantity);
	//몇번 단축키에 몇번 아이템이 있는가
	ITEM_NUMBER Get_Item_On_HotKey(_uint iHotKeyNum);

private:
	CInventory_Slot*	m_pEQInven_Slots[4] = { nullptr, nullptr, nullptr, nullptr };
	CItem_UI*			m_pEQItemUI[4] = { nullptr, nullptr, nullptr, nullptr };

	CInventory_Slot*	m_pTHInven_Slots[3] = { nullptr, nullptr, nullptr };
	CItem_UI*			m_pTHItemUI[3] = { nullptr, nullptr, nullptr };
	
	_float2				m_fPositions[4] = {};
	_float2				m_fPopUp_Pos[4] = {};

	_bool				m_isPopUp = { false };

	_float				m_fLifeTime = { 0.f };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

	CUI::UI_DESC Read_HotKeyDat();

	HRESULT Init_InvenSlot();
	HRESULT Create_InvenSlot(vector<CCustomize_UI::CUSTOM_UI_DESC>* vecInvenUI, _float2 fSetPos, _uint iCount, WEAPON_CLASSIFY eWC);
	HRESULT Init_ItemUI();

public:
	static CHotKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END