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

#pragma region 나옹
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


#pragma region 예은 추가
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

	//플레이어가 아이템을 집었을때 호출해주는 함수
	void			PickUp_Item(CGameObject* pPickedUp_Item);

	//플레이어가 상호작용 가능한 Prop과 접촉했을때 호춣하는 함수
	void			Interact_Props(CGameObject* pInteractedProps);

	//아이탬 인벤토리에 넣기
	void			AddItem_ToInven(ITEM_NUMBER eAcquiredItem, _int iItemQuantity = 1);

	//만약 아이템을 넣을수 없는 상황이라면 false를 반환함
	_bool			IsCan_AddItem_ToInven();

	//원하는 아이템의 수량 or 장탄을 반환함
	_int			Get_Search_Item_Quantity(ITEM_NUMBER eItemNum);

	//몇번 단축키에 몇번 아이템이 있는가
	ITEM_NUMBER		Get_Item_On_HotKey(_uint iHotKeyNum);

	//인벤토리 밖에서 아이템을 사용하게 되었을때 쓰는 함수(ex 총알)
	void			UseItem(ITEM_NUMBER eTargetItemNum, _int iUsage);

public://하드용
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
#pragma region 나영

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
	CGameObject*		m_pPickedUp_Item = { nullptr };//PickUp때는 주운 아이템 Interact때는 소통한 아이템
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

	//나중에 지울것
	CItem_Mesh_Viewer* GetMeshViewer() { return m_pItem_Mesh_Viewer; }
};

END