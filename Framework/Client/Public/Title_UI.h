#pragma once
#include "Customize_UI.h"

BEGIN(Client)

class CTitle_UI final : public CCustomize_UI
{
public :
	enum TITLE_TYPE { MAIN_TITLE, SELECT_TITLE, END_TITLE };

private:
	CTitle_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTitle_UI(const CTitle_UI& rhs);
	virtual ~CTitle_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Change_Tool() override;

private :
	void				TitleGame_Start(_float fTimeDelta);
	void				InGame_Start(_float fTimeDelta);
	void				Option_Click(_float fTimeDelta);
	void				Color_Rendering(_float fTimeDetla);
	void				Find_Logo();

private :
	void				MainTitle_Operate(_float fTimeDelta);
	void				SelectTitle_Operate(_float fTimeDelta);

private :
	CTextBox*			m_pText;
	CTitle_UI*			m_pTitle_BackGround			= { nullptr };
	CTitle_UI*			m_pLogo						= { nullptr };

private :
	wstring				m_wstrLogo_Broken;
	_ubyte				m_eTitleType				= {};

private :
	_bool				m_isTitleGame_Start			= { false };
	_bool				m_isInGame_Start			= { false };
	_bool				m_isInOption				= { false };

	_bool				m_isGameStart_Text			= { false };
	_float4				m_vOriginTextColor			= {};

	_float				m_fLogoStart_Timer			= { 0.f };

private :
	_float4				m_vOriginOption_Pos			= {};
	_bool				m_isSelectCharector			= { false };

private :
	_float				m_fColorTimer = { 0.f };
	_bool				m_isColorLight = { false };
	_bool				m_isColorLightType = { false };
	_uint				m_iLightCnt = { 0 };

	_bool				m_isSound = { false };
	_bool				m_isCheckSound = { false };

public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END