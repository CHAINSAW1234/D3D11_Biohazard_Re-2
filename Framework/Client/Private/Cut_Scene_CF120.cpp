#include "stdafx.h"
#include "Cut_Scene.h"

#include "Actor.h"
#include "Prop_Controller.h"
#include "Cut_Scene_CF120.h"
#include "Cut_Scene_CF94.h"
#include "Cut_Scene_Manager.h"

#include "Actor_PL00.h"
#include "Actor_PL57.h"
#include "Actor_SM42_198.h"
#include "Player.h"
#include "Call_Center.h"
#include "Actor_PartObject.h"
#include "Camera_Event.h"

CCut_Scene_CF120::CCut_Scene_CF120(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCut_Scene{ pDevice, pContext }
{
}

CCut_Scene_CF120::CCut_Scene_CF120(const CCut_Scene_CF120& rhs)
	: CCut_Scene{ rhs }
{
}

HRESULT CCut_Scene_CF120::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCut_Scene_CF120::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pGameInstance->Add_Object_Sound(m_pTransformCom, 11);
	CCut_Scene_Manager::Get_Instance()->Add_CutScene(CCut_Scene_Manager::CF_ID::_120, this);

	return S_OK;
}

void CCut_Scene_CF120::Priority_Tick(_float fTimeDelta)
{
	if (DOWN == m_pGameInstance->Get_KeyState('O'))
	{
		m_isPlaying = true;

		for (auto& pActor : m_Actors)
		{
			if (nullptr == pActor)
				continue;

			pActor->Reset_Animations();
		}

		for (auto& pProp : m_PropControllers)
		{
			if (nullptr == pProp)
				continue;

			pProp->Reset_Animations();
		}

		Start_CutScene();
	}

	__super::Priority_Tick(fTimeDelta);
}

void CCut_Scene_CF120::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CCut_Scene_CF120::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CCut_Scene_CF120::SetUp_Animation_Layer()
{
	//	For.PL0000
	if (FAILED(m_Actors[static_cast<_uint>(CF120_ACTOR_TYPE::_PL_0000)]->Set_Animation(static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_BODY), TEXT("CF120_PL0000"), 0)))
		return E_FAIL;
	if (FAILED(m_Actors[static_cast<_uint>(CF120_ACTOR_TYPE::_PL_0000)]->Set_Animation(static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_HEAD), TEXT("CF120_PL0050"), 0)))
		return E_FAIL;

	//	For.PL5700
	if (FAILED(m_Actors[static_cast<_uint>(CF120_ACTOR_TYPE::_PL_5700)]->Set_Animation(static_cast<_uint>(CActor_PL57::ACTOR_PL57_PART::_BODY), TEXT("CF120_PL5700"), 0)))
		return E_FAIL;
	if (FAILED(m_Actors[static_cast<_uint>(CF120_ACTOR_TYPE::_PL_5700)]->Set_Animation(static_cast<_uint>(CActor_PL57::ACTOR_PL57_PART::_HEAD), TEXT("CF120_PL5750"), 0)))
		return E_FAIL;

	//	For.SM42_198
	if (FAILED(m_Actors[static_cast<_uint>(CF120_ACTOR_TYPE::_SM42_198)]->Set_Animation(static_cast<_uint>(CActor_SM42_198::ACTOR_SM42_198_PART::_BODY), TEXT("CF120_SM42_198_00"), 0)))
		return E_FAIL;
	

	return S_OK;
}

void CCut_Scene_CF120::Start_CutScene()
{
	__super::Start_CutScene();

	CGameObject* pGameObject = { CCall_Center::Get_Instance()->Get_Caller(CCall_Center::CALLER::_PL00) };
	if (nullptr == pGameObject)
		return;

	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf120_dialogue.bnk.2.stm_1.mp3"), 0);
	//m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf120_dialogue.bnk.2.stm_1.mp3"), 1);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf120_dialogue.bnk.2.stm_3.mp3"), 2);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf120_se_en.bnk.2_1.mp3"), 3);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf120_se_en.bnk.2_2.mp3"), 4);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf120_se_en.bnk.2_3.mp3"), 5);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf120_se_en.bnk.2_4.mp3"), 6);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf120_se_en.bnk.2_5.mp3"), 7);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf120_se_en.bnk.2_6.mp3"), 8);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf120_se_en.bnk.2_7.mp3"), 9);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf120_se_en.bnk.2_8.mp3"), 10);

	static_cast<CCut_Scene_CF94*>(CCut_Scene_Manager::Get_Instance()->Get_CutScene(CCut_Scene_Manager::CF_ID::_94))->Finish_Marvine();

	CPlayer* pPlayer = { static_cast<CPlayer*>(pGameObject) };
	pPlayer->Set_Render(false);
	pPlayer->Set_Spotlight(false);
}

void CCut_Scene_CF120::Finish_CutScene()
{
	__super::Finish_CutScene();

	CGameObject* pGameObject = { CCall_Center::Get_Instance()->Get_Caller(CCall_Center::CALLER::_PL00) };
	if (nullptr == pGameObject)
		return;

	CPlayer* pPlayer = { static_cast<CPlayer*>(pGameObject) };
	pPlayer->Set_Render(true);
	m_Actors[static_cast<_uint>(CF120_ACTOR_TYPE::_PL_0000)]->Set_Render_All_Part(false);
	m_Actors[static_cast<_uint>(CF120_ACTOR_TYPE::_PL_5700)]->Set_Render_All_Part(true);
	m_Actors[static_cast<_uint>(CF120_ACTOR_TYPE::_SM42_198)]->Set_Render_All_Part(true);

	m_Actors[static_cast<_uint>(CF120_ACTOR_TYPE::_PL_5700)]->Set_Pause_Anim_All_Part(false);

	static_cast<CActor_PL57*>(m_Actors[static_cast<_uint>(CF120_ACTOR_TYPE::_PL_5700)])->Set_Idle_Loop();
}

void CCut_Scene_CF120::Finish_Marvine()
{
	static_cast<CActor_PL57*>(m_Actors[static_cast<_uint>(CF120_ACTOR_TYPE::_PL_5700)])->Set_Render_All_Part(false);
	static_cast<CActor_PL57*>(m_Actors[static_cast<_uint>(CF120_ACTOR_TYPE::_PL_5700)])->Set_Pause_Anim_All_Part(true);
	static_cast<CActor_PL57*>(m_Actors[static_cast<_uint>(CF120_ACTOR_TYPE::_SM42_198)])->Set_Render_All_Part(false);
	static_cast<CActor_PL57*>(m_Actors[static_cast<_uint>(CF120_ACTOR_TYPE::_SM42_198)])->Set_Pause_Anim_All_Part(true);
}

HRESULT CCut_Scene_CF120::Add_Actors()
{
	m_Actors.resize(static_cast<size_t>(CF120_ACTOR_TYPE::_END));

	CActor::ACTOR_DESC						Actor_PL0000_Desc;
	XMStoreFloat4x4(&Actor_PL0000_Desc.worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	Actor_PL0000_Desc.iBasePartIndex = static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_BODY);
	Actor_PL0000_Desc.iNumParts = static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_END);

	if (FAILED(Add_Actor(TEXT("Prototype_GameObject_Actor_PL0000"), static_cast<_uint>(CF120_ACTOR_TYPE::_PL_0000), &Actor_PL0000_Desc)))
		return E_FAIL;

	CActor::ACTOR_DESC						Actor_PL5700_Desc;
	XMStoreFloat4x4(&Actor_PL5700_Desc.worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	Actor_PL5700_Desc.iBasePartIndex = static_cast<_uint>(CActor_PL57::ACTOR_PL57_PART::_BODY);
	Actor_PL5700_Desc.iNumParts = static_cast<_uint>(CActor_PL57::ACTOR_PL57_PART::_END);

	if (FAILED(Add_Actor(TEXT("Prototype_GameObject_Actor_PL5700"), static_cast<_uint>(CF120_ACTOR_TYPE::_PL_5700), &Actor_PL5700_Desc)))
		return E_FAIL;

	CActor::ACTOR_DESC						Actor_SM42_198_Desc;
	XMStoreFloat4x4(&Actor_SM42_198_Desc.worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	Actor_SM42_198_Desc.iBasePartIndex = static_cast<_uint>(CActor_SM42_198::ACTOR_SM42_198_PART::_BODY);
	Actor_SM42_198_Desc.iNumParts = static_cast<_uint>(CActor_SM42_198::ACTOR_SM42_198_PART::_END);

	if (FAILED(Add_Actor(TEXT("Prototype_GameObject_Actor_SM42_198"), static_cast<_uint>(CF120_ACTOR_TYPE::_SM42_198), &Actor_SM42_198_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCut_Scene_CF120::Add_Props()
{
	return S_OK;
}

HRESULT CCut_Scene_CF120::Add_Camera_Event()
{
	m_strCamera_Event_Tag = TEXT("cf120");

	m_pEvent_Camera = (CCamera_Event*)m_pGameInstance->Get_GameObject(g_Level, g_strCameraTag, 1);
	if (nullptr == m_pEvent_Camera)
		return E_FAIL;

	if (FAILED(m_pEvent_Camera->Add_CamList(m_strCamera_Event_Tag, TEXT("../Bin/DataFiles/mcamlist/cf120.mcamlist.13"))))
		return E_FAIL;

	Safe_AddRef(m_pEvent_Camera);

	return S_OK;
}

CCut_Scene_CF120* CCut_Scene_CF120::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCut_Scene_CF120* pInstance = new CCut_Scene_CF120(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCut_Scene_CF120"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCut_Scene_CF120::Clone(void* pArg)
{
	CCut_Scene_CF120* pInstance = new CCut_Scene_CF120(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCut_Scene_CF120"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCut_Scene_CF120::Free()
{
	__super::Free();
}
