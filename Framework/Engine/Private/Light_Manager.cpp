#include "Light_Manager.h"

#include "Light.h"

CLight_Manager::CLight_Manager()
{

}

const LIGHT_DESC* CLight_Manager::Get_LightDesc(const wstring& strLightTag, _uint iIndex)
{
	auto& iter = m_Lights.find(strLightTag);
	if (iter == m_Lights.end())
		return nullptr;

	return iter->second->Get_LightDesc(iIndex);
}

CLight* CLight_Manager::Get_Light(const wstring& strLightTag)
{
	auto& iter = m_Lights.find(strLightTag);
	if (iter == m_Lights.end())
		return nullptr;

	return iter->second;
}

HRESULT CLight_Manager::Initialize()
{
	return S_OK;
}

HRESULT CLight_Manager::Add_Light_Layer(const wstring& strLightTag)
{
	CLight* pLight = Find_Light(strLightTag);
	if (pLight == nullptr)
	{
		pLight = CLight::Create();
		m_Lights.emplace(strLightTag, pLight);
	}

	return S_OK;
}

HRESULT CLight_Manager::Add_Light(const wstring& strLightTag, const LIGHT_DESC& LightDesc, _float fFovY, _float fAspect, _float fNearZ, _float fFarZ)
{
	CLight* pLight = Find_Light(strLightTag);
	if (pLight == nullptr)
	{
		pLight = CLight::Create();
		m_Lights.emplace(strLightTag, pLight);
	}

	return pLight->Add_LightDesc(LightDesc, fFovY, fAspect, fNearZ, fFarZ);
}

HRESULT CLight_Manager::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	for (auto& pLight : m_Lights)
		pLight.second->Render(pShader, pVIBuffer);

	return S_OK;
}

HRESULT CLight_Manager::Update_Light(const wstring& strLightTag, const LIGHT_DESC& LightDesc, _uint iIndex,_float fLerp)
{
	CLight* pLight = Find_Light(strLightTag);
	if (pLight == nullptr)
		return E_FAIL;

	return pLight->Update(LightDesc, iIndex, fLerp);
}

list<LIGHT_DESC*>* CLight_Manager::Get_Light_List(const wstring& strLightTag)
{
	CLight* pLight = Find_Light(strLightTag);
	if (pLight == nullptr)
		return nullptr;

	return pLight->Get_Light_List();
}

CLight* CLight_Manager::Find_Light(const wstring& strLightTag)
{
	auto		iter = m_Lights.find(strLightTag);

	if (iter == m_Lights.end())
		return nullptr;

	return iter->second;
}

CLight_Manager* CLight_Manager::Create()
{
	CLight_Manager* pInstance = new CLight_Manager();

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



