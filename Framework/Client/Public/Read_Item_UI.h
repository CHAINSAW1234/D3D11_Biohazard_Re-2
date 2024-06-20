#pragma once
#include "Customize_UI.h"

BEGIN(Client)

class CRead_Item_UI final : public CCustomize_UI
{
private :
	enum class READ_UI_TYPE { INTRODUCE_READ, MAIN_READ, ARROW_READ, END_READ };
	enum class READ_ARROW_TYPE { LEFT_ARROW, RIGHT_ARROW, END_ARROW };
	enum class BOOK_TYPE { OFFICER_NOTE_BOOK, END_BOOK };

private:
	CRead_Item_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRead_Item_UI(const CRead_Item_UI& rhs);
	virtual ~CRead_Item_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private :
	void				Introduce_Read(_float fTimeDelta);
	void				Main_Read();
	void				Arrow_Read();

	CRead_Item_UI*		Find_ReadUI(READ_UI_TYPE _readType, _bool _child);
	void				Render_Condition();
	void				Reset();

private :
	READ_UI_TYPE		m_eRead_type		= { READ_UI_TYPE::END_READ };
	READ_ARROW_TYPE		m_eRead_Arrow_Type	= { READ_ARROW_TYPE::END_ARROW };
	BOOK_TYPE			m_eBook_Type		= { BOOK_TYPE::END_BOOK };

private :
	CRead_Item_UI*		m_pIntro_UI = { nullptr };
	CRead_Item_UI*		m_pMain_UI = { nullptr };

	_float				m_fIntro_Timer = { 0.0f };
	_bool				m_isRead_Start = { false };

private : 
	vector<vector<wstring>> m_vecBookTexture;
	_int				m_iBookCnt = { 0 };
	_int				m_iBook_PrevCnt = { 0 };

	_float4				m_vFont_Position = {};

public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END