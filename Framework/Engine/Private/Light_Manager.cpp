#include "Light_Manager.h"

#include "Light.h"
#include "GameInstance.h"

CLight_Manager::CLight_Manager()
	: m_pGameInstance{ CGameInstance::Get_Instance()}
{
	Safe_AddRef(m_pGameInstance);
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
	_int iDir = m_pGameInstance->Get_Player_Dir();
	_int iCol = m_pGameInstance->Get_Player_Collider();

	// 1. Frustum으로 처리하기
	for (auto& pLight : m_Lights) {
		list<LIGHT_DESC*>* pList = pLight.second->Get_Light_List();

		// 1. Dir 컬링 : 플레이어의 좌, 우, 중에 따라 분류된 라이트중 일부를 꺼야됨
		// 	enum MAP_DIRECTION { DIRECTION_WEST, DIRECTION_EAST, DIRECTION_MID };
		switch (iDir) {
		case 0:
			if (pLight.first == TEXT("Layer_Light_Right"))
				continue;
			break;
		case 1:
			if (pLight.first == TEXT("Layer_Light_Left"))
				continue;

			if (pLight.first == TEXT("Layer_Light_Library"))
				continue;
			break;
		case 2:
			break;
		}

		for (size_t i = 0; i < pList->size(); i++)
		{
			auto iter = (*pList).begin();
			advance(iter, i);

			LIGHT_DESC eDesc = **iter;

			// 1. DirectionLight의 경우 무조건 통과
			if (eDesc.eType == LIGHT_DESC::TYPE_DIRECTIONAL) {
				pLight.second->Render(pShader, pVIBuffer, (_uint)i);
			}
			else {
				if (iCol >=0 && !eDesc.BelongNum[iCol]) {
					continue;
				}

				if (m_pGameInstance->isInFrustum_WorldSpace(eDesc.vPosition), .1f) {
					pLight.second->Render(pShader, pVIBuffer, (_uint)i);
				}
			}


		}

	}


	//for (auto& pLight : m_Lights)
	//	pLight.second->Render(pShader, pVIBuffer);

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

	Safe_Release(m_pGameInstance);
}



