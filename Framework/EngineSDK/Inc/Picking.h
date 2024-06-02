#pragma once

#include "Base.h"

BEGIN(Engine)

class CPicking final : public CBase
{
private:
	CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPicking() = default;

public:
	HRESULT							Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	void							Update();
	void							Transform_PickingToLocalSpace(class CTransform* pTransform, _float3* pRayDir, _float3* pRayPos);
	void							Transform_PickingToWorldSpace(_float4* pRayDir, _float4* pRayPos);

	void							Get_PickingWordSpace(_Out_ _float3* pRayDir, _Out_ _float3* pRayPos);
	void							Get_PickingWordSpace(_Out_ _vector& pRayDir, _Out_ _vector& pRayPos);
	void							ClipCursor(HWND hWnd);

public:
	_float2 Get_ProjMousePos() const {
		return  m_fProjMousePos;
	}

	POINT Get_ptProjMousePos() const {
		POINT ptPoint = {};
		ptPoint.x = static_cast<long>(m_fProjMousePos.x);
		ptPoint.y = static_cast<long>(m_fProjMousePos.y);
		return ptPoint;
	}

	_float2 Get_ViewMousePos() const {
		return m_fViewMousePos;
	}

	POINT Get_ptViewMousePos() const {
		POINT ptPoint = {};
		ptPoint.x = static_cast<long>(m_fViewMousePos.x);
		ptPoint.y = static_cast<long>(m_fViewMousePos.y);
		return ptPoint;
	}




private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };

	HWND							m_hWnd;
	_uint							m_iWinSizeX, m_iWinSizeY;

	_float3							m_vRayDir, m_vRayPos;
	_float2							m_fProjMousePos = {};
	_float2							m_fViewMousePos = {};

public:
	static CPicking* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	virtual void Free() override;
};

END