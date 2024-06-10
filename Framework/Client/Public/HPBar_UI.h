#pragma once
#include "Customize_UI.h"

class CHPBar_UI final : public CCustomize_UI
{
public : 
	enum class HP_TYPE { NORMAL_HP, BRUISE_HP, WARING_HP, DANGER_HP, END_HP };
	enum class HPBAR_TYPE { MAIN_BAR, BACKGROUND_BAR, END_BAR };

private:
	CHPBar_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHPBar_UI(const CHPBar_UI& rhs);
	virtual ~CHPBar_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private :
	void				MainBar(_float fTimeDelta);
	void				MainBar_MaskDown(_float fTimeDelta);

private :
	void				Change_HP(HP_TYPE _type);

private : /* Mask ฐüทร*/
	_float				m_isMaskStop_Timer = {};
	_uint				m_isDarkMaskCnt = {};

	_float4				m_vDefaultColor_Origin = {};
	_float2				m_fOrigin_MaskControl = {};

	_bool				m_isMaskDown = { false };

private :
	HP_TYPE				m_eCurrentHP = { HP_TYPE::NORMAL_HP };
	wstring				m_wstrNormal;
	wstring				m_wstrWaring;
	wstring				m_wstrDanger;

public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

