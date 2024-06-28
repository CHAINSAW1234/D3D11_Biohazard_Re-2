#pragma once
#include "Customize_UI.h"

BEGIN(Client)

class CCursor_UI final : public CCustomize_UI
{
private :
	enum class CURSOR_TYPE { CURSOR, CURSOR_BLUR, CURSOR_END };

private :
	CCursor_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCursor_UI(const CCursor_UI& rhs);
	virtual ~CCursor_UI() = default;

public :
	void Set_Cursor_Blur(_bool _blur)	{ m_isCursor_Blur = _blur; }
	void Set_Inven_Open(_bool _open)	{ m_isInven_Open = _open; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private :
	void			Cursor_Blur(_float fTimeDelta);
	void			Inven_Open(_float fTimeDelta);
	void			BlurCursor_Transform();

private:
	CTransform*		m_pBlurCursor_Trans			= { nullptr };
	_float2			m_fBlur_Distance			= {};

private :
	CURSOR_TYPE		m_eCursor_Type				= { CURSOR_TYPE::CURSOR_END };
	_bool			m_isInven_Open				= { false };
	_bool			m_isCursor_Blur				= { false };
	_bool			m_isCursor_Blur_Blending	= { false };

	_bool			m_isShowCursor				= { false };

public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END