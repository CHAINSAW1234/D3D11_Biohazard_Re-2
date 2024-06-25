#pragma once
#include "Inventory.h"

BEGIN(Client)

class CInventory_Item_UI final : public CInventory
{
private:
	enum class SUB_INVEN_BOX_POSITION { LEFT_INVEN, RIGHT_INVEN, DOWN_INVEN, UP_INVEN, END_INVEN };
	enum class SUB_INVEN_WHICH_CHILD { PARENT, DEFULAT_BOX_CHILD, TRIANGLE_BOX_CHILD, COUNT_BOX_CHILD, END_CHILD };

private:
	CInventory_Item_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInventory_Item_UI(const CInventory_Item_UI& rhs);
	virtual ~CInventory_Item_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void		Change_BoxType(_bool _item);
	void		Find_SelectBox();

private: /* 박스 처리 */
	void		Box_Operater(_float fTimeDelta);

	void		VoidBox();
	void		SelectBox(_float fTimeDelta);
	void		ClickOnBox();

private: /* 인벤토리 종류 */
	void		Item_Inventory();
	void		Sub_Equipment_Inventory(_float fTimeDelta);

	void		Sub_Iventory_Reset();
	void		Sub_SelectBox(SUB_INVEN_BOX_POSITION _eBoxType);

private: /* Box Type 변수*/
	/* 1. Rendering Type */
	_bool*					m_isSubRender = { nullptr };

	/* 2. Having Box Type  */
	CInventory_Item_UI*		m_pSelectBox	= { nullptr };
	SUB_INVEN_WHICH_CHILD	m_eSubInven_Child = { };
		
	/* 3. Select Box : Cursor */
	_float2					m_fDistance_BetweenCursor = {}; /* Select Box와 Cursor 간의 거리*/
	_bool					m_isMouseAcess = { false };


private: /* Sub Equipment Inventory 변수 */
	SUB_INVEN_BOX_POSITION	m_eSubInven_Type = { SUB_INVEN_BOX_POSITION::END_INVEN };

	_float					m_fOpenInven_Timer = {};
	_float					m_fOrigin_Blending = {};
	_bool					m_isSubInven_Open = { false };


public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END