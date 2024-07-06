#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Free final : public CCamera
{
public:
	typedef struct tagCameraFreeDesc: public CCamera::CAMERA_DESC
	{
		_float		fMouseSensor = { 0.0f };
	}CAMERA_FREE_DESC;

private:
	CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Free(const CCamera_Free& rhs);
	virtual ~CCamera_Free() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				Tick(_float fTimeDelta) override;
	virtual void				Late_Tick(_float fTimeDelta) override;
	virtual HRESULT				Render() override;
	void						SetPosition(_float4 vPosition);
	void						SetPosition(_vector vPosition);
	CTransform* Get_Transform()
	{
		return m_pTransformCom;
	}
	_float4						Get_Position_Float4();
	_vector						Get_Position_Vector();
	void						SetPlayer(CGameObject* pPlayer);
	void						LookAt(_float4 vPoint);

	void						Bind_PipeLine();
public:
	void	Set_Crosshair_Aiming(_bool _aiming) {
		m_isCrosshair_Aiming = _aiming;
	}
	void	Set_FixedMouse(_bool _Fixed) { m_isFixedMouse = _Fixed;  }
	virtual void	Active_Camera(_bool isActive) override;

private:
	_float						m_fMouseSensor	= { 0.0f };
	_float						m_fOriginFovy	= { 0.0f };
	_bool						m_isFixedMouse	= { true };

	class CPlayer*				m_pPlayer		= { nullptr };

	_bool						m_isLockPlayer = { false };


private:
	_float EaseInQuint(_float start, _float end, _float value);

private: /* UI */
	_bool			m_isCrosshair_Aiming = { false };


public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END