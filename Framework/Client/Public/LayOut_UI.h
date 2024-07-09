#pragma once
#include "Customize_UI.h"

BEGIN(Client)

class CLayOut_UI final : public CCustomize_UI
{
public :
	enum class MENU_HOVER_TYPE { UNMOUNTED_MENU, INSPECTION_MENU, COMBINATION_MENU, SHORTCUTS_MENU, END_MENU };

private:
	enum class LAYOUT_UI_TYPE { MINMAP_UI_TYPE, INVEN_UI_TYPE, HINT_UI_TYPE, FREE_TYPE, BACKGROUND_UI_TYPE, END_UI_TYPE };
	enum class LAYOUT_TEXT { CLOSE_TEXT, TEXT_END };
	
private:
	CLayOut_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLayOut_UI(const CLayOut_UI& rhs);
	virtual ~CLayOut_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public :
	void							Set_Typing_LayOut(_bool _isHover, _int _hoverType)
	{
		m_iMenu_HoverType = _hoverType;
		m_isMenu_Hovering = _isHover;
	}

	_bool							Get_IsCheckTyping_Type() { return m_isInvenCheck_Typing;  } /* Inven이 사용할 typing type인가? */

private:
	void							Find_TabWindow();
	void							Typing_Menu_LayOut();

private:
	LAYOUT_UI_TYPE					m_eLayout_Type = { LAYOUT_UI_TYPE::END_UI_TYPE };

	CLayOut_UI*						m_pLayout_BackGround = { nullptr }; /* 자식에서 사용할 것 : 부모 안에 Render Type을 가져올 것이다. */
	class CTab_Window*				m_pTab_Window = { nullptr };


private: /* 부모에서만 사용할 것 */
	LAYOUT_UI_TYPE					m_eRenderLayout_Type = { LAYOUT_UI_TYPE::END_UI_TYPE }; /* 어떤 Layout을 렌더할 것인가? */
	LAYOUT_TEXT						m_eText_Type = { LAYOUT_TEXT::TEXT_END };
	_bool							m_isPrevRender = { false };
	_bool*							m_isMainRender = { false };


private :
	_bool							m_isInvenCheck_Typing = { false };

	_bool							m_isMenu_Hovering = { false };
	_int							m_iMenu_HoverType = { (_int)MENU_HOVER_TYPE::END_MENU };
	_float4							m_vOriginTextPos = { };

public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END