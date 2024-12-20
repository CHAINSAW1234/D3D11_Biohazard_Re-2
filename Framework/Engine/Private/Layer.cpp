#include "..\Public\Layer.h"
#include "GameObject.h"
#include "Camera.h"

CLayer::CLayer()
{
}

const CComponent * CLayer::Get_Component(const wstring & strComTag, _uint iIndex)
{
	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;
	

	return (*iter)->Get_Component(strComTag);
}

HRESULT CLayer::Initialize()
{
	return S_OK;
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);	

	return S_OK;
}

list<class CGameObject*>* CLayer::Get_Layer()
{
	return &m_GameObjects;
}

void CLayer::Release_Layer()
{
	for (auto& pGameObject : m_GameObjects)
	{
		Safe_Release(pGameObject);
		pGameObject = nullptr;
	}
}

void CLayer::Priority_Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Priority_Tick(fTimeDelta);
	}
}

void CLayer::Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Tick(fTimeDelta);
	}
}

void CLayer::Late_Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Late_Tick(fTimeDelta);
	}
}

void CLayer::Start()
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject)
			pGameObject->Start();
	}
}

CGameObject* CLayer::Get_GameObject(_uint iIndex)
{
	if (m_GameObjects.size() <= iIndex)
		return nullptr;

	auto	iter = m_GameObjects.begin();
	std::advance(iter, iIndex);
	//for (size_t i = 0; i < iIndex; i++)
	//	++iter;

	return *iter;
}

list<class CGameObject*>* CLayer::Get_ObjectList_Ptr()
{
	return &m_GameObjects;
}

void CLayer::Active_Camera(CCamera* pCamera)
{
	for (auto& pGameObject : m_GameObjects)
	{
		CCamera* pCurCamera = static_cast<CCamera*>(pGameObject);
		if (pCurCamera == pCamera) {
			pCurCamera->Active_Camera(true);
		}
		else
		{
			pCurCamera->Active_Camera(false);
		}
	}
}

void CLayer::Set_Interpolation_Camera(CCamera* pCamera, _float fStartFovY)
{
	for (auto& pGameObject : m_GameObjects)
	{
		CCamera* pCurCamera = static_cast<CCamera*>(pGameObject);
		if (pCurCamera == pCamera) {
			pCurCamera->Set_Interpolation(fStartFovY);
		}
	}
}

CLayer * CLayer::Create()
{
	CLayer*		pInstance = new CLayer();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLayer"));

		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();
}
