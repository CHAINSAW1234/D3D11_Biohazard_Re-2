#include "stdafx.h"
#include "Cut_Scene.h"

#include "Actor.h"
#include "Cut_Scene_CF92.h"

#include "Actor_PL00.h"
#include "Actor_WP4530.h"
#include "Prop_SM60_033_00.h"

#include "Player.h"
#include "Call_Center.h"
#include "Actor_PartObject.h"
#include "Prop_Controller.h"

#include "Shutter.h"
#include "Camera_Event.h"

#include "FlashLight.h"

CCut_Scene_CF92::CCut_Scene_CF92(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCut_Scene{ pDevice, pContext }
{
}

CCut_Scene_CF92::CCut_Scene_CF92(const CCut_Scene_CF92& rhs)
	: CCut_Scene{ rhs }
{
}

HRESULT CCut_Scene_CF92::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCut_Scene_CF92::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	CTransform* pPL00Transform = { m_Actors[static_cast<_uint>(CF92_ACTOR_TYPE::_PL_0000)]->Get_Transform() };
	m_pGameInstance->Add_Object_Sound(pPL00Transform, 5);

	return S_OK;
}

HRESULT CCut_Scene_CF92::SetUp_Animation_Layer()
{
	//	For.PL0000
	if (FAILED(m_Actors[static_cast<_uint>(CF92_ACTOR_TYPE::_PL_0000)]->Set_Animation(static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_BODY), TEXT("CF92_PL0000"), 0)))
		return E_FAIL;
	if (FAILED(m_Actors[static_cast<_uint>(CF92_ACTOR_TYPE::_PL_0000)]->Set_Animation(static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_HEAD), TEXT("CF92_PL0050"), 0)))
		return E_FAIL;

	//	For.WP4530
	if (FAILED(m_Actors[static_cast<_uint>(CF92_ACTOR_TYPE::_WP_4530)]->Set_Animation(static_cast<_uint>(CActor_WP4530::ACTOR_WP4530_PART::_BODY), TEXT("CF92_WP4530_00"), 0)))
		return E_FAIL;

	return S_OK;
}

void CCut_Scene_CF92::Priority_Tick(_float fTimeDelta)
{
	if (DOWN == m_pGameInstance->Get_KeyState('T'))
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

void CCut_Scene_CF92::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CCut_Scene_CF92::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

void CCut_Scene_CF92::Start_CutScene()
{
	__super::Start_CutScene();

	CProp_Controller*		pProp_Controller = { m_PropControllers[static_cast<_uint>(CF92_PROP_TYPE::_SM_60_033)] };
	CShutter*				pShutter = { static_cast<CShutter*>(pProp_Controller->Get_PropObject()) };

	if (nullptr == pShutter)
		return;

	CTransform*				pPL00Transform = { m_Actors[static_cast<_uint>(CF92_ACTOR_TYPE::_PL_0000)]->Get_Transform() };
	m_pGameInstance->Change_Sound_3D(pPL00Transform, TEXT("cf092_dialogue.bnk.2.x64_2_00000001.mp3"), 0);
	m_pGameInstance->Change_Sound_3D(pPL00Transform, TEXT("cf092_se_en.bnk.2_1.mp3"), 1);
	m_pGameInstance->Change_Sound_3D(pPL00Transform, TEXT("cf092_se_en.bnk.2_2.mp3"), 2);
	m_pGameInstance->Change_Sound_3D(pPL00Transform, TEXT("cf092_se_en.bnk.2_3.mp3"), 3);
	m_pGameInstance->Change_Sound_3D(pPL00Transform, TEXT("cf092_se_en.bnk.2_4.mp3"), 4);

	m_pGameInstance->PlaySoundEffect_2D(TEXT("BGM"), TEXT("cf092_music_en.bnk.2_1.mp3"), 0.5f);


	CGameObject*			pGameObject = { CCall_Center::Get_Instance()->Get_Caller(CCall_Center::CALLER::_WP_4530) };
	CFlashLight*			pFlashLight = { static_cast<CFlashLight*>(pGameObject) };

	_float4x4*				pOriginSocketMatrix = { pFlashLight->Get_Socket_Ptr() };
	
	CModel*					pModel = { static_cast<CModel*>(m_Actors[static_cast<_uint>(CF92_ACTOR_TYPE::_PL_0000)]->Get_PartObject(static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_BODY))->Get_Component(TEXT("Com_Model"))) };
	_float4x4*				pNewSocketMatrix = { const_cast<_float4x4*>(pModel->Get_CombinedMatrix("l_weapon")) };

	pFlashLight->Set_Socket_Ptr(pNewSocketMatrix);
	pFlashLight->Set_Origin_Translation(true);

	m_pOrigin_SocketMatrix = pOriginSocketMatrix;

	pGameObject = { CCall_Center::Get_Instance()->Get_Caller(CCall_Center::CALLER::_PL00) };
	if (nullptr == pGameObject)
		return;

	CPlayer* pPlayer = { static_cast<CPlayer*>(pGameObject) };
	pPlayer->Set_Render(false);

	pPlayer->Set_Spotlight(true);

	pShutter->Set_OutOfControll(true);
}

void CCut_Scene_CF92::Finish_CutScene()
{
	__super::Finish_CutScene();

	CGameObject*			pGameObject = { CCall_Center::Get_Instance()->Get_Caller(CCall_Center::CALLER::_PL00) };
	if (nullptr == pGameObject)
		return;

	CPlayer*				pPlayer = { static_cast<CPlayer*>(pGameObject) };
	pPlayer->Set_Render(true);

	CProp_Controller*		pProp_Controller = { m_PropControllers[static_cast<_uint>(CF92_PROP_TYPE::_SM_60_033)] };
	CShutter*				pShutter = { static_cast<CShutter*>(pProp_Controller->Get_PropObject()) };

	if (nullptr == pShutter)
		return;

	pShutter->Set_OutOfControll(false);

	pGameObject = { CCall_Center::Get_Instance()->Get_Caller(CCall_Center::CALLER::_WP_4530) };
	CFlashLight* pFlashLight = { static_cast<CFlashLight*>(pGameObject) };

	pFlashLight->Set_Socket_Ptr(m_pOrigin_SocketMatrix);
	pFlashLight->Set_Origin_Translation(false);

	m_pOrigin_SocketMatrix = nullptr;
}

HRESULT CCut_Scene_CF92::Add_Actors()
{
	m_Actors.resize(static_cast<size_t>(CF92_ACTOR_TYPE::_END));

	CActor::ACTOR_DESC			Actor_PL0000_Desc;
	XMStoreFloat4x4(&Actor_PL0000_Desc.worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	Actor_PL0000_Desc.iBasePartIndex = static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_BODY);
	Actor_PL0000_Desc.iNumParts = static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_END);

	if (FAILED(Add_Actor(TEXT("Prototype_GameObject_Actor_PL0000"), static_cast<_uint>(CF92_ACTOR_TYPE::_PL_0000), &Actor_PL0000_Desc)))
		return E_FAIL;

	CActor::ACTOR_DESC			Actor_WP4530_Desc;
	XMStoreFloat4x4(&Actor_WP4530_Desc.worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	Actor_WP4530_Desc.iBasePartIndex = static_cast<_uint>(CActor_WP4530::ACTOR_WP4530_PART::_BODY);
	Actor_WP4530_Desc.iNumParts = static_cast<_uint>(CActor_WP4530::ACTOR_WP4530_PART::_END);

	if (FAILED(Add_Actor(TEXT("Prototype_GameObject_Actor_WP4530"), static_cast<_uint>(CF92_ACTOR_TYPE::_WP_4530), &Actor_WP4530_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCut_Scene_CF92::Add_Props()
{
	m_PropControllers.resize(static_cast<size_t>(CF92_PROP_TYPE::_END));

	CProp_Controller::PROP_CONTROLL_DESC			Prop_SM60_033_Desc;
	Prop_SM60_033_Desc.strAnimLayerTag = TEXT("CF92_SM60_033_00");

	if (FAILED(Add_PropController(TEXT("Prototype_GameObject_Prop_SM60_033"), static_cast<_uint>(CF92_PROP_TYPE::_SM_60_033), &Prop_SM60_033_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCut_Scene_CF92::Add_Camera_Event()
{
	m_strCamera_Event_Tag = TEXT("cf92");

	m_pEvent_Camera = (CCamera_Event*)m_pGameInstance->Get_GameObject(g_Level, g_strCameraTag, 1);
	if (nullptr == m_pEvent_Camera)
		return E_FAIL;

	if (FAILED(m_pEvent_Camera->Add_CamList(m_strCamera_Event_Tag, TEXT("../Bin/DataFiles/mcamlist/cf092.mcamlist.13"))))
		return E_FAIL;

	Safe_AddRef(m_pEvent_Camera);

	return S_OK;
}

CCut_Scene_CF92* CCut_Scene_CF92::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCut_Scene_CF92* pInstance = new CCut_Scene_CF92(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCut_Scene_CF92"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCut_Scene_CF92::Clone(void* pArg)
{
	CCut_Scene_CF92* pInstance = new CCut_Scene_CF92(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCut_Scene_CF92"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCut_Scene_CF92::Free()
{
	__super::Free();
}

