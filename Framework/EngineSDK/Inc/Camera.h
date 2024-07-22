#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4 vEye = {};
		_float4 vAt = {};
		_float	fFovy = { 0.0f };
		_float	fAspect = { 0.0f };
		_float	fNear = { 0.0f };
		_float	fFar = { 0.0f };
	}CAMERA_DESC;


protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& rhs);	
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual void	Late_Tick(_float fTimeDelta) override;

	_bool			Get_IsActive() { return m_isActive; }
	virtual void	Active_Camera(_bool isActive) { 
		m_isActive = isActive; 
		if (m_isActive)
			Bind_PipeLines();
	}

	void			Set_Interpolation(_float fStartFovY);

public:
	_bool			m_isActive = { false };

protected:
	_float			m_fFovy = { 0.0f };
	_float			m_fAspect = { 0.0f };
	_float			m_fNear = { 0.0f };
	_float			m_fFar = { 0.0f };

protected:
	_float4x4		m_ProjMatrix;
	_float4x4		m_ViewMatrix;

protected:
	_float			m_fAccInterpoaltionTime = { 0.f };
	_float4x4		m_PreCamWorldMatrix = {};
	_float			m_fPreFovY = { 0.f };
	_bool			m_isInterpolate = { false };

protected:
	HRESULT Bind_PipeLines();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END