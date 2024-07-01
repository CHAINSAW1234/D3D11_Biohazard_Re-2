#pragma once

#include "UI.h"
#include "Inventory_Manager.h"
#include "Item_Mesh_Viewer.h"

BEGIN(Client)

class CTab_Window final : public CUI
{
public:
	enum WINDOW_TYPE{MINIMAP, INVENTORY, HINT, EXAMINE, PICK_UP_ITEM ,WINDOW_TYPE_END };

protected:
	CTab_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTab_Window(const CTab_Window& rhs);
	virtual ~CTab_Window() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void	ItemIven_EventHandle(_float fTimeDelta);
	void	FirstTick_Seting();
	void	Button_Act(_float fTimeDelta);

public:
	void	OnOff_EventHandle();
	void	Set_WindowType(WINDOW_TYPE eWindowType) { m_eWindowType = eWindowType; };

#pragma region 나옹
public:
	_bool				Get_MinMapRender()				{ return m_isMapRender; }
	_bool*				Get_MainRender()				{ return m_pInvenButton->Get_Dead_Ptr(); }
	WINDOW_TYPE*		Get_Window_Render_Type()		{ return &m_eWindowType; }

#pragma endregion
	void	PickUp_Item(CGameObject* pPickedUp_Item);

	//아이탬 인벤토리에 넣기
	void AddItem_ToInven(ITEM_NUMBER eAcquiredItem, _int iItemQuantity = 1);

	//만약 아이템을 넣을수 없는 상황이라면 false를 반환함
	_bool IsCan_AddItem_ToInven();

	//원하는 아이템의 수량 or 장탄을 반환함
	_int Get_Search_Item_Quantity(ITEM_NUMBER eItemNum);

	//몇번 단축키에 몇번 아이템이 있는가
	ITEM_NUMBER Get_Item_On_HotKey(_uint iHotKeyNum);

	void UseItem(ITEM_NUMBER eTargetItemNum, _int iUsage);

private:
	class CButton_UI*	m_pMapButton = { nullptr };
	class CButton_UI*	m_pInvenButton = { nullptr };
	class CButton_UI*	m_pHintButton = { nullptr };

	CInventory_Manager* m_pInventory_Manager = { nullptr };

	class CItem_Discription* m_pItem_Discription = { nullptr };

private : /* NY */
	void				Find_Cursor();
	void				Select_UI();

	class CCursor_UI*	m_pCursor[2] = {nullptr};

private:
	WINDOW_TYPE			m_eWindowType = { INVENTORY };
	_bool				m_isMapRender = { false };

	_bool				m_isFristTick = { true };

	_bool				m_isAlphaControl = { false };

	/*for. Item_Mesh_Viewer*/
	CItem_Mesh_Viewer* m_pItem_Mesh_Viewer = { nullptr };

	_float				m_fCurTime = { 0.f };

	/*for. Picked Up Item*/
	CGameObject*		m_pPickedUp_Item = { nullptr };

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