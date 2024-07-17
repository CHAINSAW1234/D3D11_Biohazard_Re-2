#include "stdafx.h"
#include "Cut_Scene.h"

#include "Actor.h"

CCut_Scene::CCut_Scene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CCut_Scene::CCut_Scene(const CCut_Scene& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CCut_Scene::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCut_Scene::Initialize(void* pArg)
{
	if (FAILED(Add_Actors()))
		return E_FAIL;

	if (FAILED(SetUp_Animation_Layer()))
		return E_FAIL;

	return S_OK;
}

void CCut_Scene::Priority_Tick(_float fTimeDelta)
{
	if (false == m_isPlaying)
		return;


	_bool		isNonFinishedCurrentAnim = { false };
	for (auto& pActor : m_Actors)
	{
		if (false == pActor->Is_Finished_Animation_All_Part())
		{
			isNonFinishedCurrentAnim = true;
			break;
		}
	}

	if (false == isNonFinishedCurrentAnim)
	{
		_bool		isNonFinshedCurrentSeq = { false };
		for (auto& pActor : m_Actors)
		{
			if (false == pActor->Is_Finished_All_Animation_All())
			{
				isNonFinshedCurrentSeq = true;
				break;
			}
		}

		if (false == isNonFinshedCurrentSeq)
		{
			m_isPlaying = false;

			cout << "Finish" << endl;

			Finish();
		}
		else
		{
			cout << "Next" << endl;

			for (auto& pActor : m_Actors)
			{
				pActor->Set_Next_Animation_Sequence();	
			}
		}		
	}

	Priority_Tick_Actors(fTimeDelta);
}

void CCut_Scene::Tick(_float fTimeDelta)
{
	if (false == m_isPlaying)
		return;

	Tick_Actors(fTimeDelta);
}

void CCut_Scene::Late_Tick(_float fTimeDelta)
{
	if (false == m_isPlaying)
		return;

	Late_Tick_Actors(fTimeDelta);
}

HRESULT CCut_Scene::Render()
{
	return S_OK;
}

HRESULT CCut_Scene::SetUp_Animation_Layer()
{
	return S_OK;
}

HRESULT CCut_Scene::Add_Actor(const wstring& strPrototypeTag, _uint iActorType, void* pArg)
{
	CGameObject*			pGameObject = { m_pGameInstance->Clone_GameObject(strPrototypeTag, pArg) };
	if (nullptr == pGameObject)
		return E_FAIL;

	CActor*					pActor = { dynamic_cast<CActor*>(pGameObject) };
	if (nullptr == pActor)
		return E_FAIL;

	m_Actors[iActorType] = pActor;

	return S_OK;
}

CActor* CCut_Scene::Get_Actor(_uint iActorType)
{
	_uint				iNumActors = { static_cast<_uint>(m_Actors.size()) };
	if (iActorType >= iNumActors)
		return nullptr;

	return m_Actors[iActorType];
}

void CCut_Scene::Priority_Tick_Actors(_float fTimeDelta)
{
	for (auto& pActor : m_Actors)
	{
		if (nullptr == pActor)
			continue;

		pActor->Priority_Tick(fTimeDelta);
	}
}

void CCut_Scene::Tick_Actors(_float fTimeDelta)
{
	for (auto& pActor : m_Actors)
	{
		if (nullptr == pActor)
			continue;

		pActor->Tick(fTimeDelta);
	}
}

void CCut_Scene::Late_Tick_Actors(_float fTimeDelta)
{
	for (auto& pActor : m_Actors)
	{
		if (nullptr == pActor)
			continue;

		pActor->Late_Tick(fTimeDelta);
	}
}

void CCut_Scene::Free()
{
	__super::Free();

	for (auto& pActor : m_Actors)
	{
		Safe_Release(pActor);
		pActor = nullptr;
	}
	m_Actors.clear();
}
