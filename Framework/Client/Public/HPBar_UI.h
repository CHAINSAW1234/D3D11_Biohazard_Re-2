#pragma once
#include "Customize_UI.h"
#include "Observer.h"

BEGIN(Client)

class CHPBar_UI final : public CCustomize_UI , public CObserver
{
public : 
	enum class HP_TYPE {DANGER, CAUTION, FINE, HP_END };

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
	virtual void OnNotify() override;

private :
	void						Operation_HPBar(_float fTimeDelta);


private :
	void						BackGroundBar(_float fTimeDelta);
	void						Change_HP(HP_TYPE _type);

private :
	void						Find_Main_Inventory();
	void						HPBar_Position_Setting();

private :
	void						HP_Condition(_float fTimeDelta);
	void						Reset_HPBar();

private : /* Mask ฐüทร*/
	_float4						m_vDefaultColor_Origin		= {};
	_float2						m_fOrigin_MaskControl		= {};

	_bool						m_isMaskDown				= { false };
	_bool						m_isTimerReturn				= { false };
	_uint						m_iColorMask_Cnt			= { 2 };
	_uint						m_iCurrent_ColorMask_Cnt	= { 0 };


private : /* HP BAR */
	CHPBar_UI*					m_pMainBar				= { nullptr };
	_bool*						m_pMain_Inven_Render	= { nullptr };
	
	_bool						m_isInGame_HPBar_Render = { false };
	_float						m_fHPLifeTimer = { 0.f };
	_float						m_fOriginBlending		= {};

private : /* FONT */
	_float2						m_fDistance_Font		= {};

private :
	CCustomize_UI::HPBAR_TYPE	m_eHPBar		= { HPBAR_TYPE::END_BAR };
	HP_TYPE						m_eCurrentHP	= { HP_TYPE::FINE };
	wstring						m_wstrNormal;
	wstring						m_wstrWaring;
	wstring						m_wstrDanger;

public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END