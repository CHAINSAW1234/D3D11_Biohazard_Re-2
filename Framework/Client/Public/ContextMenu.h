#pragma once
#include "Customize_UI.h"


BEGIN(Client)

class CContextMenu final : public CCustomize_UI
{
protected:
	CContextMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CContextMenu(const CContextMenu& rhs);
	virtual ~CContextMenu() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Change_Tool() override;

private:
	void PopUp_Operation(_float fTimeDelta);
	void Idle_Operation(_float fTimeDelta);
	void Hide_Operation(_float fTimeDelta);

public:
	void Set_Operation(ITEM_TYPE eItemType, _bool bActive, _float3 isEquiped, _float3 fArrivalPos );

	INVENTORY_EVENT Get_InventoryEvent() const {
		return m_eContextEvent;
	}

private:
	void Set_EventbyTexture(_uint iTextureNum);

private:
	class CContext_Highlighter* m_pContext_Highlighter = { nullptr };
	CTransform*					m_pContext_HighlighterTransform = { nullptr };

private:
	UI_OPERRATION				m_eContext_State = { STATE_END };

	ITEM_TYPE					m_eContextType = { INVEN_ITEM_TYPE_END };

	INVENTORY_EVENT				m_eContextEvent = { INVEN_EVENT_END };

	_uint						m_iContextMenuCount = { 0 };

	_float3						m_fAppearPos = {};

	_float3						m_fArrivalPos = {};

	CGameObject*				m_pHoverdChild = { nullptr };//사운드용

#pragma region 나옹
private:
	/* Fuction */
	HRESULT						Find_LayOut();
	void						HoveringFor_LayOut(_int _hoverType);

	/* variable */
	class CLayOut_UI*			m_pLayOut = { nullptr };

	_int						m_iHoverMenu_Type = { 0 };
	_int						m_iPrev_HoverMenu_Type{ 0 };

	UI_OPERRATION				m_ePrevContext_State = { STATE_END };


#pragma endregion

private:
	HRESULT Init_Context_Highlighter();

public:
	static CContextMenu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END