#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight final : public CBase
{
private:
	CLight();
	virtual ~CLight() = default;

public:
	const LIGHT_DESC* Get_LightDesc() const {
		return &m_LightDesc;
	}
	const _float4x4* Get_LightViewMatrix() const {
		return m_LightViewMatrix;
	}
	const _float4x4 Get_LightProjMatrix() const {
		return m_LightProjMatrix;
	}


public:
	HRESULT Initialize(const LIGHT_DESC& LightDesc, _float fFovY, _float fAspect, _float fNearZ, _float fFarZ);
	HRESULT Tick(const LIGHT_DESC& Light_Desc);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

private:
	LIGHT_DESC		m_LightDesc = {};
	_float4x4*		m_LightViewMatrix = {};
	_float4x4		m_LightProjMatrix = {};

public:
	static CLight* Create(const LIGHT_DESC& LightDesc, _float fFovY, _float fAspect, _float fNearZ, _float fFarZ);
	virtual void Free() override;
};

END