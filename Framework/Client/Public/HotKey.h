#pragma once

#include "UI.h"

#include "Inventory_Slot.h"
#include "Item_UI.h"

BEGIN(Client)

class CHotKey final : public CUI
{



private:
	enum ITEM_DISCRIPTION{NAME, CLASSIFY, DISCRIPTION};

protected:
	CHotKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHotKey(const CHotKey& rhs);
	virtual ~CHotKey() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CInventory_Slot*	m_pInven_Slots[4] = { nullptr,nullptr, nullptr, nullptr };
	CItem_UI*			m_pItemUI[4] = { nullptr,nullptr, nullptr, nullptr };
	_float2				m_fPositions[4] = {};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

	CUI::UI_DESC Read_HotKeyDat();

	HRESULT Init_InvenSlot();
	HRESULT Create_InvenSlot(vector<CCustomize_UI::CUSTOM_UI_DESC>* vecInvenUI, _float3 fSetPos);
	HRESULT Init_ItemUI();



public:
	static CHotKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END