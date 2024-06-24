#pragma once

#include "UI.h"
#include "Inventory_Manager.h"
#include "Item_Mesh_Viewer.h"

BEGIN(Client)

class CTab_Window final : public CUI
{
public:
	enum WINDOW_TYPE{MINIMAP, INVENTORY, HINT, EXAMINE, WINDOW_TYPE_END };

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

public :
	_bool*	Get_MinMapRender()	{ return &m_isMapRender; }
	_bool*	Get_MainRender()	{ return m_pInvenButton->Get_Dead_Ptr(); }


	//아이탬 인벤토리에 넣기
	void AddItem_ToInven(ITEM_NUMBER eAcquiredItem);

	//만약 아이템을 넣을수 없는 상황이라면 false를 반환함
	_bool IsCan_AddItem_ToInven();


	
private:
	class CButton_UI*	m_pMapButton = { nullptr };
	class CButton_UI*	m_pInvenButton = { nullptr };
	class CButton_UI*	m_pHintButton = { nullptr };

	CInventory_Manager* m_pInventory_Manager = { nullptr };

private:
	WINDOW_TYPE			m_eWindowType = { INVENTORY };
	_bool				m_isMapRender = { false };

	_bool				m_isFristTick = { true };

	_bool				m_isAlphaControl = { false };

	/*for. Item_Mesh_Viewer*/
	CItem_Mesh_Viewer* m_pItem_Mesh_Viewer = { nullptr };

	_float				m_fCurTime = { 0.f };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Create_Button();
	HRESULT Create_Inventory();
	HRESULT Creat_MiniMap();
	HRESULT Creat_Hint();
	HRESULT Creat_Item_Mesh_Viewer();


public:
	static CTab_Window* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END