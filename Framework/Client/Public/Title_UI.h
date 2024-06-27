#pragma once
#include "Customize_UI.h"

BEGIN(Client)

class CTitle_UI final : public CCustomize_UI
{

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

private :
	void				TitleGame_Start();
	void				InGame_Start(_float fTimeDelta);
	void				Option_Click(_float fTimeDelta);

	void				Find_Logo();

private :
	CTextBox*			m_pText;
	CTitle_UI*			m_pTitle_BackGround			= { nullptr };
	CTitle_UI*			m_pLogo						= {};

private :
	wstring				m_wstrLogo_Broken;

private :
	_bool				m_isTitleGame_Start			= { false };
	_bool				m_isInGame_Start			= { false };
	_bool				m_isInOption				= { false };

	_bool				m_isGameStart_Text			= { false };
	_float4				m_vOriginTextColor			= {};


private :
	_float4				m_vOriginOption_Pos			= {};


public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END