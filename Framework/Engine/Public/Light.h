#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight final : public CBase
{
private:
	CLight();
	virtual ~CLight() = default;

public:
	HRESULT						Add_LightDesc(LIGHT_DESC desc, _float fFovY, _float fAspect, _float fNearZ, _float fFarZ);
	LIGHT_DESC*					Get_LightDesc(_uint _iIndex) const
	{
		if (m_Lights.empty() || m_Lights.size() < _iIndex)
			return nullptr;
		auto& iter = m_Lights.begin();
		for (_uint i = 0; i < _iIndex; i++)
			iter++;
		return (*iter);
	}
	HRESULT						Set_Render(_bool bRender);
	list<LIGHT_DESC*>*			Get_Light_List() { return &m_Lights; }

public:
	HRESULT						Initialize();
	HRESULT						Update(const LIGHT_DESC& Light_Desc, _uint iIndex, _float fLerp);
	HRESULT						Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

private:
	bool bUse = { false };			// 구역에 대한 설정용 
	list<LIGHT_DESC*>			m_Lights = {};

public:
	static CLight*				Create();
	virtual void Free() override;
};

END