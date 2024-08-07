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
	enum WINDOW_TYPE { MINIMAP, INVENTORY, HINT, EXAMINE, PICK_UP_ITEM_WINDOW, INTERACT_PROPS, WINDOW_TYPE_END };

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
	void PICK_UP_ITEM_SoundPlay(ITEM_NUMBER eItemNum);
	void INTERACT_PROPS_Operation(_float fTimeDelta);

#pragma region ³ª¿Ë
public: 
	_bool							Get_MinMapRender()				{ return m_isMapRender; }
	_bool*							Get_MainRender_Ptr()			{ return m_pInvenButton->Get_Dead_Ptr(); }
	_bool*							Get_MapItem_Ptr()				 { return &m_isGetMapItem;  }
	_bool							Get_PickUp_ItemRender()			{ return m_isPickUp_Item; }
	WINDOW_TYPE*					Get_Window_RenderType_Ptr()		{ return &m_eWindowType; }
	void							Destroy_Statue_Item(ITEM_NUMBER _statue);

private:
	class CTargeting_Map_UI* m_pTargetNotify_UI = { nullptr };

private:
	_bool							m_isPickUp_Item = { false };
	_bool							m_isGetMapItem = { false };
	_bool							m_isGetMapItem_Close = { false };

	class CCamera_Free* m_pCamera = { nullptr };
#pragma endregion


#pragma region ¿¹Àº Ãß°¡
private:
	class CRead_Item_UI* m_pRead_Item_UI = { nullptr };
	class CItem_Map_UI* m_pMapItem_UI = { nullptr };
#pragma endregion 


private:
	void			ItemIven_EventHandle(_float fTimeDelta);
	void			Button_Act(_float fTimeDelta);
	_bool			IsInputTab();

public:
	void			OnOff_EventHandle();
	void			Set_WindowType(WINDOW_TYPE eWindowType) { m_eWindowType = eWindowType; };
	void			Hotkey_PopUp();
	void			Set_Weapon_Accessories(ITEM_NUMBER eCallItemType, _uint iAccessories);

	//ÇÃ·¹ÀÌ¾î°¡ ¾ÆÀÌÅÛÀ» Áý¾úÀ»¶§ È£ÃâÇØÁÖ´Â ÇÔ¼ö
	void			PickUp_Item(CGameObject* pPickedUp_Item);

	//ÇÃ·¹ÀÌ¾î°¡ »óÈ£ÀÛ¿ë °¡´ÉÇÑ Prop°ú Á¢ÃËÇßÀ»¶§ È£­„ÇÏ´Â ÇÔ¼ö
	void			Interact_Props(CGameObject* pInteractedProps);

	//¾ÆÀÌÅÆ ÀÎº¥Åä¸®¿¡ ³Ö±â
	void			AddItem_ToInven(ITEM_NUMBER eAcquiredItem, _int iItemQuantity = 1);

	//¸¸¾à ¾ÆÀÌÅÛÀ» ³ÖÀ»¼ö ¾ø´Â »óÈ²ÀÌ¶ó¸é false¸¦ ¹ÝÈ¯ÇÔ
	_bool			IsCan_AddItem_ToInven();

	//¿øÇÏ´Â ¾ÆÀÌÅÛÀÇ ¼ö·® or ÀåÅºÀ» ¹ÝÈ¯ÇÔ
	_int			Get_Search_Item_Quantity(ITEM_NUMBER eItemNum);

	//¸î¹ø ´ÜÃàÅ°¿¡ ¸î¹ø ¾ÆÀÌÅÛÀÌ ÀÖ´Â°¡
	ITEM_NUMBER		Get_Item_On_HotKey(_uint iHotKeyNum);

	//ÀÎº¥Åä¸® ¹Û¿¡¼­ ¾ÆÀÌÅÛÀ» »ç¿ëÇÏ°Ô µÇ¾úÀ»¶§ ¾²´Â ÇÔ¼ö(ex ÃÑ¾Ë)
	void			UseItem(ITEM_NUMBER eTargetItemNum, _int iUsage);

public://ÇÏµå¿ë
	void			Set_Dead_Discription(_bool bDead);
	void			Set_PortabTo_Blank();


private:
	class CButton_UI*			m_pMapButton = { nullptr };
	class CButton_UI*			m_pInvenButton = { nullptr };
	class CButton_UI*			m_pHintButton = { nullptr };

	class CItem_Discription* m_pItem_Discription = { nullptr };

	CInventory_Manager*			m_pInventory_Manager = { nullptr };

	CItem_Mesh_Viewer*			m_pItem_Mesh_Viewer = { nullptr };

	CHotKey*					m_pHotKey = { nullptr };

	CHint*						m_pHint = { nullptr };
#pragma region ³ª¿µ

private: /* NY */
	void				Find_Cursor();
	void				Select_UI();

	class CCursor_UI* m_pCursor[2] = { nullptr };

private:
	WINDOW_TYPE			m_eWindowType = { INVENTORY };
	WINDOW_TYPE			m_ePreWindowType = { WINDOW_TYPE_END };
	_bool				m_isMapRender = { false };

	_bool				m_isSecondTick = { false };
	_bool				m_isMapOpen = { false };
#pragma endregion


	/*for. Item_Mesh_Viewer*/
	_float				m_fCurTime = { 0.f };

	/*for. Picked Up Item*/
	CGameObject*		m_pPickedUp_Item = { nullptr };//PickUp¶§´Â ÁÖ¿î ¾ÆÀÌÅÛ Interact¶§´Â ¼ÒÅëÇÑ ¾ÆÀÌÅÛ
	UI_OPERRATION		m_eSequence = { STATE_END };

	vector<ITEM_NUMBER> m_vecCollect_ITEM;
	

	_bool isFirst = true;

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

	//³ªÁß¿¡ Áö¿ï°Í
	CItem_Mesh_Viewer* GetMeshViewer() { return m_pItem_Mesh_Viewer; }
};

END