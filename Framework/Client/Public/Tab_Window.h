#pragma once

#include "UI.h"
#include "Inventory_Manager.h"
#include "Item_Mesh_Viewer.h"
#include "HotKey.h"
#include "Hint.h"

BEGIN(Client)

class CTab_Window final : public CUI
{
public:
	enum WINDOW_TYPE{MINIMAP, INVENTORY, HINT, EXAMINE, PICK_UP_ITEM_WINDOW, INTERACT_PROPS, WINDOW_TYPE_END };

protected:
	CTab_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTab_Window(const CTab_Window& rhs);
	virtual ~CTab_Window() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Start() override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void MINIMAP_Operation(_float fTimeDelta);
	void INVENTORY_Operation(_float fTimeDelta);
	void HINT_Operation(_float fTimeDelta);
	void EXAMINE_Operation(_float fTimeDelta);
	void PICK_UP_ITEM_WINDOW_Operation(_float fTimeDelta);
	void INTERACT_PROPS_Operation(_float fTimeDelta);

#pragma region ≥™øÀ
public: 
	_bool				Get_MinMapRender()				{ return m_isMapRender; }
	_bool*				Get_MainRender_Ptr()			{ return m_pInvenButton->Get_Dead_Ptr(); }
	WINDOW_TYPE*		Get_Window_RenderType_Ptr()		{ return &m_eWindowType; }
#pragma endregion


#pragma region øπ¿∫ √ﬂ∞°
	class CRead_Item_UI*	m_pRead_Item_UI = { nullptr };
	class CItem_Map_UI*			m_pMap_UI = { nullptr };
#pragma endregion 


private:
	void			ItemIven_EventHandle(_float fTimeDelta);
	void			Button_Act(_float fTimeDelta);

public:
	void			OnOff_EventHandle();
	void			Set_WindowType(WINDOW_TYPE eWindowType) { m_eWindowType = eWindowType; };

	//«√∑π¿ÃæÓ∞° æ∆¿Ã≈€¿ª ¡˝æ˙¿ª∂ß »£√‚«ÿ¡÷¥¬ «‘ºˆ
	void			PickUp_Item(CGameObject* pPickedUp_Item);

	//«√∑π¿ÃæÓ∞° ªÛ»£¿€øÎ ∞°¥…«— Prop∞˙ ¡¢√À«ﬂ¿ª∂ß »£≠Ñ«œ¥¬ «‘ºˆ
	void			interact_Props(CGameObject* pPickedUp_Item);

	//æ∆¿Ã≈∆ ¿Œ∫•≈‰∏Æø° ≥÷±‚
	void			AddItem_ToInven(ITEM_NUMBER eAcquiredItem, _int iItemQuantity = 1);

	//∏∏æ‡ æ∆¿Ã≈€¿ª ≥÷¿ªºˆ æ¯¥¬ ªÛ»≤¿Ã∂Û∏È false∏¶ π›»Ø«‘
	_bool			IsCan_AddItem_ToInven();

	//ø¯«œ¥¬ æ∆¿Ã≈€¿« ºˆ∑Æ or ¿Â≈∫¿ª π›»Ø«‘
	_int			Get_Search_Item_Quantity(ITEM_NUMBER eItemNum);

	//∏Óπ¯ ¥‹√‡≈∞ø° ∏Óπ¯ æ∆¿Ã≈€¿Ã ¿÷¥¬∞°
	ITEM_NUMBER		Get_Item_On_HotKey(_uint iHotKeyNum);

	//¿Œ∫•≈‰∏Æ π€ø°º≠ æ∆¿Ã≈€¿ª ªÁøÎ«œ∞‘ µ«æ˙¿ª∂ß æ≤¥¬ «‘ºˆ(ex √—æÀ)
	void			UseItem(ITEM_NUMBER eTargetItemNum, _int iUsage);

	void			Hotkey_PopUp();

private:
	class CButton_UI*			m_pMapButton = { nullptr };
	class CButton_UI*			m_pInvenButton = { nullptr };
	class CButton_UI*			m_pHintButton = { nullptr };

	CInventory_Manager*			m_pInventory_Manager = { nullptr };

	class CItem_Discription*	m_pItem_Discription = { nullptr };

	CItem_Mesh_Viewer*			m_pItem_Mesh_Viewer = { nullptr };

	CHotKey*					m_pHotKey = { nullptr };

	CHint*						m_pHint = { nullptr };

private : /* NY */
	void				Find_Cursor();
	void				Select_UI();

	class CCursor_UI*	m_pCursor[2] = {nullptr};

private:
	WINDOW_TYPE			m_eWindowType = { INVENTORY };
	_bool				m_isMapRender = { false };

	/*for. Item_Mesh_Viewer*/
	_float				m_fCurTime = { 0.f };

	/*for. Picked Up Item*/
	CGameObject*		m_pPickedUp_Item = { nullptr };
	UI_OPERRATION		m_eSequence = { STATE_END };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Create_Button();
	HRESULT Create_Inventory();
	HRESULT Creat_MiniMap();
	HRESULT Creat_Hint();
	HRESULT Creat_Item_Mesh_Viewer();
	HRESULT Creat_Item_Discription();

public:
	static CTab_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END