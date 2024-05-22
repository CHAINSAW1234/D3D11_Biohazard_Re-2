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

const _float4x4* CLight_Manager::Get_LightViewMatrix(const wstring& strLightTag)
{
	auto& iter = m_Lights.find(strLightTag);
	if (iter == m_Lights.end())
		return nullptr;

	return iter->second->Get_LightViewMatrix();
}

const _float4x4 CLight_Manager::Get_LightProjMatrix(const wstring& strLightTag)
{
	auto& iter = m_Lights.find(strLightTag);
	if (iter == m_Lights.end())
		return _float4x4();

	return iter->second->Get_LightProjMatrix();
}

HRESULT CLight_Manager::Initialize()
{
	return S_OK;
}

HRESULT CLight_Manager::Add_Light(const wstring& strLightTag, const LIGHT_DESC & LightDesc, _float fFovY, _float fAspect, _float fNearZ, _float fFarZ)
{

	CLight* pLight = Find_Light(strLightTag);
	if (pLight != nullptr)
		return E_FAIL;

	pLight = CLight::Create(LightDesc, fFovY, fAspect, fNearZ, fFarZ);
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



