#pragma once
#include "Customize_UI.h"

BEGIN(Client)

class CCrosshair_UI final : public CCustomize_UI
{
private :
	enum class CROSSHAIR_TYPE { CROSSHAIR_LEFT, CROSSHAIR_RIGHT, CROSSHAIR_UP, CROSSHAIR_DOWN, CROSSHAIR_END };

private:
	CCrosshair_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCrosshair_UI(const CCrosshair_UI& rhs);
	virtual ~CCrosshair_UI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Change_Tool() override;

public :
	_bool					Get_Crosshair_RenderType(_int _type);
	_bool					Render_Type();

private :
	/* Shot Gun */
	void					Operate_ShotGun(_float fTimeDelta);

	/* Default Gun*/
	void					Operate_DefaultGun(_float fTimeDelta);
	void					Aiming(_float fTimeDelta);
	void					Aiming_Return(_float fTimeDelta);


private :
	CCrosshair_UI*			m_pCenterDot = { nullptr };
	class CTab_Window*		m_pTabWindow = { nullptr };

private :
	CROSSHAIR_TYPE			m_eCrosshair_Type					= { CROSSHAIR_TYPE::CROSSHAIR_END };
	_int					m_eGun_Type							= { 0 };

	_float					m_fCrosshair_Timer					= { 0.0f };
	_float					m_fCrosshair_AccTimer				= { 1.f };
	_bool					m_isAiming							= { false };
	_bool					m_isShoot							= { false };

	_float3					m_vCrosshair_OriginScale			= {};
	_float4					m_vCrosshair_OriginPos				= {};
	_float					m_fCrosshair_ControlDistance		= { 12.f }; /* 중심으로부터 조준 거리 */
	_float					m_fCrosshair_Aiming_Ready_Distance	= { 12.f }; /* 조준 완료 전 대기 거리*/
	_float4					m_fCorsshair_AimPoint				= {}; /* 조준 목표점 */

	_float3					m_fFixed_MinScaled					= {}; /* 최소 스케일 고정*/

	_bool					m_isCrosshair_Render				= { false };


public:
	static CCustomize_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END