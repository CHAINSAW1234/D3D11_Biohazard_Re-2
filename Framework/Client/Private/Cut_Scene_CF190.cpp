#include "stdafx.h"
#include "Cut_Scene.h"

#include "Actor.h"
#include "Prop_Controller.h"
#include "Cut_Scene_CF190.h"
#include "Cut_Scene_CF120.h"
#include "Cut_Scene_Manager.h"

#include "Actor_PL00.h"
#include "Actor_PL57.h"
#include "Actor_SM40_133.h"
#include "Actor_SM41_024.h"
#include "Actor_WP4530.h"
#include "Prop_SM41_024.h"
#include "Player.h"
#include "Call_Center.h"
#include "Actor_PartObject.h"
#include "Camera_Event.h"

#include "Body_NewpoliceStatue.h"
#include "NewpoliceStatue.h"

CCut_Scene_CF190::CCut_Scene_CF190(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCut_Scene{ pDevice, pContext }
{
}

CCut_Scene_CF190::CCut_Scene_CF190(const CCut_Scene_CF190& rhs)
	: CCut_Scene{ rhs }
{
}

HRESULT CCut_Scene_CF190::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCut_Scene_CF190::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pGameInstance->Add_Object_Sound(m_pTransformCom, 20);
	CCut_Scene_Manager::Get_Instance()->Add_CutScene(CCut_Scene_Manager::CF_ID::_190, this);

	return S_OK;
}

void CCut_Scene_CF190::Priority_Tick(_float fTimeDelta)
{
	if (DOWN == m_pGameInstance->Get_KeyState('P'))
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

	_uint			iSeqLev = { m_Actors[static_cast<_uint>(CF190_ACTOR_TYPE::_PL_0000)]->Get_SeqLev() };
	if (0 == iSeqLev)
	{
		m_Actors[static_cast<_uint>(CF190_ACTOR_TYPE::_WP4530)]->Set_Render_All_Part(false);
		static_cast<CActor_WP4530*>(m_Actors[static_cast<_uint>(CF190_ACTOR_TYPE::_WP4530)])->Set_LightSpot(false);
	}

	if (2 == iSeqLev)
	{
		m_Actors[static_cast<_uint>(CF190_ACTOR_TYPE::_WP4530)]->Set_Render_All_Part(true);
	}

	if (3 == iSeqLev)
	{
		//static_cast<CActor_WP4530*>(m_Actors[static_cast<_uint>(CF190_ACTOR_TYPE::_WP4530)])->Set_LightSpot(false);
	}

	if (4 == iSeqLev)
	{
		m_Actors[static_cast<_uint>(CF190_ACTOR_TYPE::_WP4530)]->Set_Render_All_Part(false);
	}

	__super::Priority_Tick(fTimeDelta);
}

void CCut_Scene_CF190::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CCut_Scene_CF190::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CCut_Scene_CF190::SetUp_Animation_Layer()
{
	//	For.PL0000
	if (FAILED(m_Actors[static_cast<_uint>(CF190_ACTOR_TYPE::_PL_0000)]->Set_Animation(static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_BODY), TEXT("CF190_PL0000"), 0)))
		return E_FAIL;
	if (FAILED(m_Actors[static_cast<_uint>(CF190_ACTOR_TYPE::_PL_0000)]->Set_Animation(static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_HEAD), TEXT("CF190_PL0050"), 0)))
		return E_FAIL;
	if (FAILED(m_Actors[static_cast<_uint>(CF190_ACTOR_TYPE::_PL_0000)]->Set_Animation(static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_HAIR), TEXT("CF190_PL0070"), 0)))
		return E_FAIL;

	//	For.PL5700
	if (FAILED(m_Actors[static_cast<_uint>(CF190_ACTOR_TYPE::_PL_5700)]->Set_Animation(static_cast<_uint>(CActor_PL57::ACTOR_PL57_PART::_BODY), TEXT("CF190_PL5700"), 0)))
		return E_FAIL;
	if (FAILED(m_Actors[static_cast<_uint>(CF190_ACTOR_TYPE::_PL_5700)]->Set_Animation(static_cast<_uint>(CActor_PL57::ACTOR_PL57_PART::_HEAD), TEXT("CF190_PL5750"), 0)))
		return E_FAIL;

	//	For.SM42_198
	if (FAILED(m_Actors[static_cast<_uint>(CF190_ACTOR_TYPE::_SM40_133)]->Set_Animation(static_cast<_uint>(CActor_SM40_133::ACTOR_SM40_133_PART::_BODY), TEXT("CF190_SM40_133"), 0)))
		return E_FAIL;

	////	For.SM41_024
	//if (FAILED(m_Actors[static_cast<_uint>(CF190_ACTOR_TYPE::_SM41_024)]->Set_Animation(static_cast<_uint>(CActor_SM41_024::ACTOR_SM41_024_PART::_BODY), TEXT("CF190_SM41_024"), 0)))
	//	return E_FAIL;

	//	For.WP4530
	if (FAILED(m_Actors[static_cast<_uint>(CF190_ACTOR_TYPE::_WP4530)]->Set_Animation(static_cast<_uint>(CActor_WP4530::ACTOR_WP4530_PART::_BODY), TEXT("CF190_WP4530"), 0)))
		return E_FAIL;


	return S_OK;
}

void CCut_Scene_CF190::Start_CutScene()
{
	__super::Start_CutScene();

	CGameObject* pGameObject = { CCall_Center::Get_Instance()->Get_Caller(CCall_Center::CALLER::_PL00) };
	if (nullptr == pGameObject)
		return;

	/*CProp_Controller* pProp_Controller = { m_PropControllers[static_cast<_uint>(CF190_PROP_TYPE::_SM41_024)] };
	CNewpoliceStatue* pStatue = { static_cast<CNewpoliceStatue*>(pProp_Controller->Get_PropObject()) };
	if (nullptr == pStatue)
		return;

	pStatue->Set_OutOfControll(true);*/
	//	static_cast<CProp_SM41_024*>(pProp_Controller)->Set_Transformation_Origin();

	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf190_dialogue.bnk.2.stm_2.mp3"), 0);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf190_dialogue.bnk.2.stm_4.mp3"), 1);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf190_music_en.bnk.2_2.mp3"), 2);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf190_se_en.bnk.2_1.mp3"), 3);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf190_se_en.bnk.2_2.mp3"), 4);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf190_se_en.bnk.2_3.mp3"), 5);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf190_se_en.bnk.2_4.mp3"), 6);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf190_se_en.bnk.2_5.mp3"), 7);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf190_se_en.bnk.2_6.mp3"), 8);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf190_se_en.bnk.2_7.mp3"), 9);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf190_se_en.bnk.2_8.mp3"), 10);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf190_se_en.bnk.2_9.mp3"), 11);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf190_se_en.bnk.2_10.mp3"), 12);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf190_se_en.bnk.2_11.mp3"), 13);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf190_se_en.bnk.2_12.mp3"), 14);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf190_se_en.bnk.2_13.mp3"), 15);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf190_se_en.bnk.2_14.mp3"), 16);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf190_se_en.bnk.2_15.mp3"), 17);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf190_se_en.bnk.2_16.mp3"), 18);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf190_se_en.bnk.2_17.mp3"), 19);

	CPlayer* pPlayer = { static_cast<CPlayer*>(pGameObject) };
	pPlayer->Set_Render(false);
	pPlayer->Set_Spotlight(false);

	static_cast<CActor_WP4530*>(m_Actors[static_cast<_uint>(CCut_Scene_CF190::CF190_ACTOR_TYPE::_WP4530)])->Set_LightSpot(false);


	static_cast<CCut_Scene_CF120*>(CCut_Scene_Manager::Get_Instance()->Get_CutScene(CCut_Scene_Manager::CF_ID::_120))->Finish_Marvine();
}

void CCut_Scene_CF190::Finish_CutScene()
{
	__super::Finish_CutScene();

	CGameObject* pGameObject = { CCall_Center::Get_Instance()->Get_Caller(CCall_Center::CALLER::_PL00) };
	if (nullptr == pGameObject)
		return;

	CPlayer* pPlayer = { static_cast<CPlayer*>(pGameObject) };
	pPlayer->Set_Render(true);
	m_Actors[static_cast<_uint>(CF190_ACTOR_TYPE::_PL_0000)]->Set_Render_All_Part(false);
	m_Actors[static_cast<_uint>(CF190_ACTOR_TYPE::_PL_5700)]->Set_Render_All_Part(true);
	m_Actors[static_cast<_uint>(CF190_ACTOR_TYPE::_SM40_133)]->Set_Render_All_Part(true);

	m_Actors[static_cast<_uint>(CF190_ACTOR_TYPE::_PL_5700)]->Set_Pause_Anim_All_Part(false);

	static_cast<CActor_PL57*>(m_Actors[static_cast<_uint>(CF190_ACTOR_TYPE::_PL_5700)])->Set_Idle_Loop();

	/*CProp_Controller* pProp_Controller = { m_PropControllers[static_cast<_uint>(CF190_PROP_TYPE::_SM41_024)] };
	CNewpoliceStatue* pStatue = { static_cast<CNewpoliceStatue*>(pProp_Controller->Get_PropObject()) };
	if (nullptr == pStatue)
		return;

	pStatue->Set_OutOfControll(false);*/
	//	static_cast<CProp_SM41_024*>(pProp_Controller)->Reset_Transformation();
}

HRESULT CCut_Scene_CF190::Add_Actors()
{
	m_Actors.resize(static_cast<size_t>(CF190_ACTOR_TYPE::_END));

	CActor::ACTOR_DESC						Actor_PL0000_Desc;
	XMStoreFloat4x4(&Actor_PL0000_Desc.worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	Actor_PL0000_Desc.iBasePartIndex = static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_BODY);
	Actor_PL0000_Desc.iNumParts = static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_END);

	if (FAILED(Add_Actor(TEXT("Prototype_GameObject_Actor_PL0000"), static_cast<_uint>(CF190_ACTOR_TYPE::_PL_0000), &Actor_PL0000_Desc)))
		return E_FAIL;

	CActor::ACTOR_DESC						Actor_PL5700_Desc;
	XMStoreFloat4x4(&Actor_PL5700_Desc.worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	Actor_PL5700_Desc.iBasePartIndex = static_cast<_uint>(CActor_PL57::ACTOR_PL57_PART::_BODY);
	Actor_PL5700_Desc.iNumParts = static_cast<_uint>(CActor_PL57::ACTOR_PL57_PART::_END);

	if (FAILED(Add_Actor(TEXT("Prototype_GameObject_Actor_PL5700"), static_cast<_uint>(CF190_ACTOR_TYPE::_PL_5700), &Actor_PL5700_Desc)))
		return E_FAIL;

	CActor::ACTOR_DESC						Actor_SM40_133_Desc;
	XMStoreFloat4x4(&Actor_SM40_133_Desc.worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	Actor_SM40_133_Desc.iBasePartIndex = static_cast<_uint>(CActor_SM40_133::ACTOR_SM40_133_PART::_BODY);
	Actor_SM40_133_Desc.iNumParts = static_cast<_uint>(CActor_SM40_133::ACTOR_SM40_133_PART::_END);
	
	if (FAILED(Add_Actor(TEXT("Prototype_GameObject_Actor_SM40_133"), static_cast<_uint>(CF190_ACTOR_TYPE::_SM40_133), &Actor_SM40_133_Desc)))
		return E_FAIL;

	/*CActor::ACTOR_DESC						Actor_SM41_024_Desc;
	XMStoreFloat4x4(&Actor_SM41_024_Desc.worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	Actor_SM41_024_Desc.iBasePartIndex = static_cast<_uint>(CActor_SM41_024::ACTOR_SM41_024_PART::_BODY);
	Actor_SM41_024_Desc.iNumParts = static_cast<_uint>(CActor_SM41_024::ACTOR_SM41_024_PART::_END);

	if (FAILED(Add_Actor(TEXT("Prototype_GameObject_Actor_SM41_024"), static_cast<_uint>(CF190_ACTOR_TYPE::_SM41_024), &Actor_SM41_024_Desc)))
		return E_FAIL;*/

	CActor::ACTOR_DESC						Actor_WP4530_Desc;
	XMStoreFloat4x4(&Actor_WP4530_Desc.worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	Actor_WP4530_Desc.iBasePartIndex = static_cast<_uint>(CActor_WP4530::ACTOR_WP4530_PART::_BODY);
	Actor_WP4530_Desc.iNumParts = static_cast<_uint>(CActor_WP4530::ACTOR_WP4530_PART::_END);

	if (FAILED(Add_Actor(TEXT("Prototype_GameObject_Actor_WP4530"), static_cast<_uint>(CF190_ACTOR_TYPE::_WP4530), &Actor_WP4530_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCut_Scene_CF190::Add_Props()
{
	m_PropControllers.resize(static_cast<size_t>(CF190_PROP_TYPE::_END));

	/*CProp_Controller::PROP_CONTROLL_DESC			Prop_SM41_024_Desc;
	Prop_SM41_024_Desc.strAnimLayerTag = TEXT("CF190_SM41_024");

	if (FAILED(Add_PropController(TEXT("Prototype_GameObject_Prop_SM41_024"), static_cast<_uint>(CF190_PROP_TYPE::_SM41_024), &Prop_SM41_024_Desc)))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CCut_Scene_CF190::Add_Camera_Event()
{
	m_strCamera_Event_Tag = TEXT("cf190");

	m_pEvent_Camera = (CCamera_Event*)m_pGameInstance->Get_GameObject(g_Level, g_strCameraTag, 1);
	if (nullptr == m_pEvent_Camera)
		return E_FAIL;

	if (FAILED(m_pEvent_Camera->Add_CamList(m_strCamera_Event_Tag, TEXT("../Bin/DataFiles/mcamlist/cf190.mcamlist.13"))))
		return E_FAIL;

	Safe_AddRef(m_pEvent_Camera);

	return S_OK;
}

CCut_Scene_CF190* CCut_Scene_CF190::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCut_Scene_CF190* pInstance = new CCut_Scene_CF190(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCut_Scene_CF190"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCut_Scene_CF190::Clone(void* pArg)
{
	CCut_Scene_CF190* pInstance = new CCut_Scene_CF190(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCut_Scene_CF190"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCut_Scene_CF190::Free()
{
	__super::Free();
}
