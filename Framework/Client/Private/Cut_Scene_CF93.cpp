#include "stdafx.h"
#include "Cut_Scene.h"

#include "Actor.h"
#include "Cut_Scene_CF93.h"

#include "Actor_PL78.h"
#include "Actor_PL00.h"
#include "Actor_EM00.h"
#include "Actor_SM60_035.h"
#include "Prop_SM60_034_00.h"

#include "Call_Center.h"
#include "Player.h"
#include "Actor_PartObject.h"
#include "Camera_Event.h"
#include "Shutter.h"

CCut_Scene_CF93::CCut_Scene_CF93(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCut_Scene{ pDevice, pContext }
{
}

CCut_Scene_CF93::CCut_Scene_CF93(const CCut_Scene_CF93& rhs)
	: CCut_Scene{ rhs }
{
}

HRESULT CCut_Scene_CF93::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCut_Scene_CF93::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pGameInstance->Add_Object_Sound(m_pTransformCom, 19);

	return S_OK;
}

void CCut_Scene_CF93::Priority_Tick(_float fTimeDelta)
{
	if (DOWN == m_pGameInstance->Get_KeyState('Y'))
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

void CCut_Scene_CF93::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CCut_Scene_CF93::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CCut_Scene_CF93::Start_CutScene()
{
	__super::Start_CutScene();

	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf093_dialogue.bnk.2.stm_1.mp3"), 0);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf093_dialogue.bnk.2.stm_3.mp3"), 1);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf093_dialogue.bnk.2.stm_4.mp3"), 2);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf093_music_en.bnk.2_1.mp3"), 3);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf093_se_en.bnk.2_1.mp3"), 4);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf093_se_en.bnk.2_2.mp3"), 5);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf093_se_en.bnk.2_3.mp3"), 6);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf093_se_en.bnk.2_4.mp3"), 7);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf093_se_en.bnk.2_5.mp3"), 8);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf093_se_en.bnk.2_6.mp3"), 9);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf093_se_en.bnk.2_7.mp3"), 10);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf093_se_en.bnk.2_8.mp3"), 11);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf093_se_en.bnk.2_9.mp3"), 12);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf093_se_en.bnk.2_10.mp3"), 13);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf093_se_en.bnk.2_11.mp3"), 14);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf093_se_en.bnk.2_12.mp3"), 15);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf093_se_en.bnk.2_13.mp3"), 16);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf093_se_en.bnk.2_14.mp3"), 17);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf093_se_en.bnk.2_15.mp3"), 18);

	CProp_Controller*		pProp_Controller = { m_PropControllers[static_cast<_uint>(CF93_PROP_TYPE::_SM_60_034)] };
	CShutter*				pShutter = { static_cast<CShutter*>(pProp_Controller->Get_PropObject()) };

	if (nullptr == pShutter)
		return;

	CGameObject*			pGameObject = { CCall_Center::Get_Instance()->Get_Caller(CCall_Center::CALLER::_PL00) };
	if (nullptr == pGameObject)
		return;

	CPlayer* pPlayer = { static_cast<CPlayer*>(pGameObject) };
	pPlayer->Set_Render(false);
	pPlayer->Set_Spotlight(false);

	pShutter->Set_OutOfControll(true);
}

void CCut_Scene_CF93::Finish_CutScene()
{
	__super::Finish_CutScene();

	CGameObject*			pGameObject = { CCall_Center::Get_Instance()->Get_Caller(CCall_Center::CALLER::_PL00) };
	if (nullptr == pGameObject)
		return;

	CPlayer*				pPlayer = { static_cast<CPlayer*>(pGameObject) };
	pPlayer->Set_Render(true);

	m_Actors[static_cast<_uint>(CF93_ACTOR_TYPE::_PL_7800)]->Set_Render_All_Part(true);
	m_Actors[static_cast<_uint>(CF93_ACTOR_TYPE::_PL_0000)]->Set_Render_All_Part(false);


	CProp_Controller*		pProp_Controller = { m_PropControllers[static_cast<_uint>(CF93_PROP_TYPE::_SM_60_034)] };
	CShutter*				pShutter = { static_cast<CShutter*>(pProp_Controller->Get_PropObject()) };

	if (nullptr == pShutter)
		return;

	pShutter->Set_OutOfControll(false);
}

HRESULT CCut_Scene_CF93::SetUp_Animation_Layer()
{
	//	For.EM0000
	if (FAILED(m_Actors[static_cast<_uint>(CF93_ACTOR_TYPE::_EM_0000)]->Set_Animation(static_cast<_uint>(CActor_EM00::ACTOR_EM00_PART::_BODY), TEXT("CF93_EM0000"), 0)))
		return E_FAIL;

	//	For.PL0000
	if (FAILED(m_Actors[static_cast<_uint>(CF93_ACTOR_TYPE::_PL_0000)]->Set_Animation(static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_BODY), TEXT("CF93_PL0000"), 0)))
		return E_FAIL;
	if (FAILED(m_Actors[static_cast<_uint>(CF93_ACTOR_TYPE::_PL_0000)]->Set_Animation(static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_HEAD), TEXT("CF93_PL0050"), 0)))
		return E_FAIL;

	//	For.PL7800
	if (FAILED(m_Actors[static_cast<_uint>(CF93_ACTOR_TYPE::_PL_7800)]->Set_Animation(static_cast<_uint>(CActor_PL78::ACTOR_PL78_PART::_BODY), TEXT("CF93_PL7800"), 0)))
		return E_FAIL;
	if (FAILED(m_Actors[static_cast<_uint>(CF93_ACTOR_TYPE::_PL_7800)]->Set_Animation(static_cast<_uint>(CActor_PL78::ACTOR_PL78_PART::_HEAD), TEXT("CF93_PL7850"), 0)))
		return E_FAIL;
	if (FAILED(m_Actors[static_cast<_uint>(CF93_ACTOR_TYPE::_PL_7800)]->Set_Animation(static_cast<_uint>(CActor_PL78::ACTOR_PL78_PART::_GUTS), TEXT("CF93_PL7880"), 0)))
		return E_FAIL;	

	//	For.SM60_035
	if (FAILED(m_Actors[static_cast<_uint>(CF93_ACTOR_TYPE::_SM_60_035)]->Set_Animation(static_cast<_uint>(CActor_SM60_035::ACTOR_SM60_035_PART::_BODY), TEXT("CF93_SM60_035_00"), 0)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCut_Scene_CF93::Add_Actors()
{
	m_Actors.resize(static_cast<size_t>(CF93_ACTOR_TYPE::_END));

	CActor::ACTOR_DESC			Actor_PL7800_Desc;
	XMStoreFloat4x4(&Actor_PL7800_Desc.worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	Actor_PL7800_Desc.iBasePartIndex = static_cast<_uint>(CActor_PL78::ACTOR_PL78_PART::_BODY);
	Actor_PL7800_Desc.iNumParts = static_cast<_uint>(CActor_PL78::ACTOR_PL78_PART::_END);
	
	if (FAILED(Add_Actor(TEXT("Prototype_GameObject_Actor_PL7800"), static_cast<_uint>(CF93_ACTOR_TYPE::_PL_7800), &Actor_PL7800_Desc)))
		return E_FAIL;

	CActor::ACTOR_DESC			Actor_PL0000_Desc;
	XMStoreFloat4x4(&Actor_PL0000_Desc.worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	Actor_PL0000_Desc.iBasePartIndex = static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_BODY);
	Actor_PL0000_Desc.iNumParts = static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_END);

	if (FAILED(Add_Actor(TEXT("Prototype_GameObject_Actor_PL0000"), static_cast<_uint>(CF93_ACTOR_TYPE::_PL_0000), &Actor_PL0000_Desc)))
		return E_FAIL;

	CActor_EM00::ACTOR_EM00_DESC			Actor_EM0000_Desc;
	XMStoreFloat4x4(&Actor_EM0000_Desc.worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	Actor_EM0000_Desc.iBasePartIndex = static_cast<_uint>(CActor_EM00::ACTOR_EM00_PART::_BODY);
	Actor_EM0000_Desc.iNumParts = static_cast<_uint>(CActor_EM00::ACTOR_EM00_PART::_END);
	Actor_EM0000_Desc.eFaceType = CActor_EM00::ACTOR_EM00_FACE_TYPE::_DEFAULT;

	if (FAILED(Add_Actor(TEXT("Prototype_GameObject_Actor_EM0000"), static_cast<_uint>(CF93_ACTOR_TYPE::_EM_0000), &Actor_EM0000_Desc)))
		return E_FAIL;


	CActor::ACTOR_DESC			Actor_SM60_035_Desc;
	XMStoreFloat4x4(&Actor_SM60_035_Desc.worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	Actor_SM60_035_Desc.iBasePartIndex = static_cast<_uint>(CActor_SM60_035::ACTOR_SM60_035_PART::_BODY);
	Actor_SM60_035_Desc.iNumParts = static_cast<_uint>(CActor_SM60_035::ACTOR_SM60_035_PART::_END);

	if (FAILED(Add_Actor(TEXT("Prototype_GameObject_Actor_SM60_035"), static_cast<_uint>(CF93_ACTOR_TYPE::_SM_60_035), &Actor_SM60_035_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCut_Scene_CF93::Add_Props()
{
	m_PropControllers.resize(static_cast<size_t>(CF93_PROP_TYPE::_END));

	CProp_Controller::PROP_CONTROLL_DESC			Prop_SM60_034_Desc;
	Prop_SM60_034_Desc.strAnimLayerTag = TEXT("CF93_SM60_034_00");

	if (FAILED(Add_PropController(TEXT("Prototype_GameObject_Prop_SM60_034"), static_cast<_uint>(CF93_PROP_TYPE::_SM_60_034), &Prop_SM60_034_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCut_Scene_CF93::Add_Camera_Event()
{
	m_strCamera_Event_Tag = TEXT("cf93");

	m_pEvent_Camera = (CCamera_Event*)m_pGameInstance->Get_GameObject(g_Level, g_strCameraTag, 1);
	if (nullptr == m_pEvent_Camera)
		return E_FAIL;

	if (FAILED(m_pEvent_Camera->Add_CamList(m_strCamera_Event_Tag, TEXT("../Bin/DataFiles/mcamlist/cf093.mcamlist.13"))))
		return E_FAIL;

	Safe_AddRef(m_pEvent_Camera);

	return S_OK;
}

CCut_Scene_CF93* CCut_Scene_CF93::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCut_Scene_CF93* pInstance = new CCut_Scene_CF93(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCut_Scene_CF93"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCut_Scene_CF93::Clone(void* pArg)
{
	CCut_Scene_CF93*			pInstance = new CCut_Scene_CF93(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCut_Scene_CF93"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCut_Scene_CF93::Free()
{
	__super::Free();
}
