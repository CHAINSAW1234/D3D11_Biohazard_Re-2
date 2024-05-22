#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHT_DESC* Get_LightDesc(const wstring& strLightTag);
	const _float4x4* Get_LightViewMatrix(const wstring& strLightTag);
	const _float4x4 Get_LightProjMatrix(const wstring& strLightTag);

public:
	HRESULT Initialize();
	HRESULT Add_Light(const wstring& strLightTag, const LIGHT_DESC& LightDesc, _float fFovY, _float fAspect, _float fNearZ, _float fFarZ);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT Tick_Light(const wstring& strLightTag, const LIGHT_DESC& LightDesc);
private:
	map<const wstring, class CLight* > m_Lights;

private:
	class CLight*	Find_Light(const wstring& strLightTag);

public:
	static CLight_Manager* Create();
	virtual void Free() override;
};

END