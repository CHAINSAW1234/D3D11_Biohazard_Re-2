#include "Light_Manager.h"

#include "Light.h"

CLight_Manager::CLight_Manager()
{

}

const LIGHT_DESC * CLight_Manager::Get_LightDesc(const wstring & strLightTag)
{
	auto& iter = m_Lights.find(strLightTag);
	if (iter == m_Lights.end())
		return nullptr;

	return iter->second->Get_LightDesc();	
}


HRESULT CLight_Manager::Initialize()
{
	return S_OK;
}

HRESULT CLight_Manager::Add_Light(const wstring& strLightTag, const LIGHT_DESC & LightDesc)
{
	CLight* pLight = Find_Light(strLightTag);
	if (pLight != nullptr)
		return E_FAIL;



	pLight = CLight::Create(LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.emplace(strLightTag, pLight);

	return S_OK;
}

HRESULT CLight_Manager::Render(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	for (auto& pLight : m_Lights)
		pLight.second->Render(pShader, pVIBuffer);

	return S_OK;
}

HRESULT CLight_Manager::Tick_Light(const wstring& strLightTag, const LIGHT_DESC& LightDesc)
{
	CLight* pLight = Find_Light(strLightTag);
	if (pLight == nullptr)
		return E_FAIL;

	return pLight->Tick(LightDesc);
}

CLight* CLight_Manager::Find_Light(const wstring& strLightTag)
{

	auto		iter = m_Lights.find(strLightTag);

	if (iter == m_Lights.end())
		return nullptr;

	return iter->second;
}

CLight_Manager * CLight_Manager::Create()
{
	CLight_Manager*		pInstance = new CLight_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLight_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLight_Manager::Free()
{
	for (auto& pLight : m_Lights)
		Safe_Release(pLight.second);


	m_Lights.clear();
}



