#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHT_DESC* Get_LightDesc(const wstring& strLightTag, _uint iIndex);
	class CLight* Get_Light(const wstring& strLightTag);
	HRESULT Add_Light(const wstring& strLightTag, const LIGHT_DESC& LightDesc, _float fFovY, _float fAspect, _float fNearZ, _float fFarZ);
	HRESULT Add_Light_Layer(const wstring& strLightTag);
	list<LIGHT_DESC*>* Get_Light_List(const wstring& strLightTag);
	HRESULT Update_Light(const wstring& strLightTag, const LIGHT_DESC& LightDesc, _uint iIndex, _float fLerp);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

public:
	HRESULT Initialize();

private:
	list<CLight*>* Culling_RenderringLight();

	map<const wstring, class CLight* > m_Lights;
	class CGameInstance* m_pGameInstance = { nullptr };

private:
	class CLight* Find_Light(const wstring& strLightTag);

public:
	static CLight_Manager* Create();
	virtual void Free() override;
};

END