#include "stdafx.h"
#include "Cut_Scene.h"

#include "Actor.h"
#include "Prop_Controller.h"
#include "Camera_Event.h"

#include "Call_Center.h"

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
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Actors()))
		return E_FAIL;

	if (FAILED(Add_Props()))
		return E_FAIL;

	if (FAILED(Add_Camera_Event()))
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
	/*for (auto& pActor : m_Actors)
	{
		if (true == isNonFinishedCurrentAnim)
			break;

		if (false == pActor->Is_Finished_Animation_All_Part())
		{
			isNonFinishedCurrentAnim = true;
		}
	}

	for (auto& pProp : m_PropControllers)
	{
		if (true == isNonFinishedCurrentAnim)
			break;

		if (false == pProp->Is_Finished_Animation())
		{
			isNonFinishedCurrentAnim = true;
		}
	}*/
	
	if (false == m_pEvent_Camera->Is_Finsihed())
	{
		isNonFinishedCurrentAnim = true;
	}

	if (false == isNonFinishedCurrentAnim)
	{
		_bool		isNonFinshedCurrentSeq = { false };
		for (auto& pActor : m_Actors)
		{
			if (true == isNonFinshedCurrentSeq)
				break;

			if (false == pActor->Is_Finished_All_Animation_All())
			{
				isNonFinshedCurrentSeq = true;
			}
		}

		for (auto& pProp : m_PropControllers)
		{
			if (true == isNonFinshedCurrentSeq)
				break;

			if (false == pProp->Is_Finished_Animation_All())
			{
				isNonFinshedCurrentSeq = true;
			}
		}

		if (false == m_pEvent_Camera->Is_All_Finsihed())
		{
			isNonFinshedCurrentSeq = true;
		}

		if (false == isNonFinshedCurrentSeq)
		{
			m_isPlaying = false;

			cout << "Finish" << endl;

			Finish_CutScene();
		}
		else
		{
			cout << "Next" << endl;

			for (auto& pActor : m_Actors)
			{
				pActor->Set_Next_Animation_Sequence();	
			}

			for (auto& pProp : m_PropControllers)
			{
				pProp->Set_Next_Animation_Sequence();
			}

			m_pEvent_Camera->Change_to_Next();
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
	m_pTransformCom->Set_WorldMatrix(CCall_Center::Get_Instance()->Get_Caller(CCall_Center::CALLER::_PL00)->Get_Transform()->Get_WorldMatrix());

	Late_Tick_Actors(fTimeDelta);
}

HRESULT CCut_Scene::Render()
{
	return S_OK;
}

void CCut_Scene::Start()
{
	for (auto& pProp : m_PropControllers)
	{
		pProp->Start();
	}
}

HRESULT CCut_Scene::SetUp_Animation_Layer()
{
	return S_OK;
}

void CCut_Scene::Start_CutScene()
{
	m_pEvent_Camera->Set_PlayCamlist(m_strCamera_Event_Tag);

	for (auto& pActor : m_Actors)
	{
		pActor->Set_Pause_Anim_All_Part(false);
		pActor->Set_Render_All_Part(true);
	}
}

void CCut_Scene::Finish_CutScene()
{
	if(nullptr != m_pEvent_Camera)
		m_pEvent_Camera->Reset();

	for (auto& pActor : m_Actors)
	{
		pActor->Set_Pause_Anim_All_Part(true);
		pActor->Set_Render_All_Part(false);
	}
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

HRESULT CCut_Scene::Add_PropController(const wstring& strPrototypeTag, _uint iPropType, void* pArg)
{
	CGameObject* pGameObject = { m_pGameInstance->Clone_GameObject(strPrototypeTag, pArg) };
	if (nullptr == pGameObject)
		return E_FAIL;

	CProp_Controller* pPropController = { dynamic_cast<CProp_Controller*>(pGameObject) };
	if (nullptr == pPropController)
		return E_FAIL;

	m_PropControllers[iPropType] = pPropController;

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

	for (auto& pPropController : m_PropControllers)
	{
		Safe_Release(pPropController);
		pPropController = nullptr;
	}
	m_PropControllers.clear();

	Safe_Release(m_pEvent_Camera);
}
