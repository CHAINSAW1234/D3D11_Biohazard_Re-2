#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Gimmick final : public CCamera
{
public:
	typedef struct tagCameraGimmickDesc: public CCamera::CAMERA_DESC
	{
		_float		fMouseSensor = { 0.0f };
	}CAMERA_GIMMICK_DESC;

private:
	CCamera_Gimmick(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Gimmick(const CCamera_Gimmick& rhs);
	virtual ~CCamera_Gimmick() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* pArg) override;
	virtual void					Tick(_float fTimeDelta) override;
	virtual void					Late_Tick(_float fTimeDelta) override;
	virtual HRESULT					Render() override;

public :
	void							SetPosition(_float4 vPosition);
	void							SetPosition(_vector vPosition);

	CTransform* Get_Transform()
	{
		return m_pTransformCom;
	}

	_float4							Get_Position_Float4();
	_vector							Get_Position_Vector();
	void							SetPlayer(CGameObject* pPlayer);
	void							LookAt(_float4 vPoint);
	void							Bind_PipeLine();

public:
	_ubyte*							Get_Layout_Type()				{ return &m_eLayout_Type; }
	void							Set_FixedMouse(_bool _Fixed)	{ m_isFixedMouse = _Fixed;  }
	virtual void					Active_Camera(_bool isActive) override;

private:
	_float							m_fMouseSensor	= { 0.0f };
	_float							m_fOriginFovy	= { 0.0f };
	_bool							m_isFixedMouse	= { true };

	class CPlayer*					m_pPlayer		= { nullptr };

	_bool							m_isLockPlayer	= { false };
	_ubyte							m_eLayout_Type	= { };

public:
	static CCamera_Gimmick* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END