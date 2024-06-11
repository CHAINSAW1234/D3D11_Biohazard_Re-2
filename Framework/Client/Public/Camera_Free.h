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
private:
	_float						m_fMouseSensor = { 0.0f };
	class CPlayer* m_pPlayer = { nullptr };

public:
	static CCamera_Free* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END