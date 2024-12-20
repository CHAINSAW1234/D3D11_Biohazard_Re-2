#include "stdafx.h"
#include "Cut_Scene.h"

#include "Actor.h"
#include "Cut_Scene_CF95.h"
#include "Cut_Scene_Manager.h"

#include "Actor_PL00.h"
#include "Actor_EM00.h"
#include "Actor_WP4530.h"
#include "Player.h"
#include "Call_Center.h"
#include "Actor_PartObject.h"
#include "FlashLight.h"

#include "Camera_Event.h"
#include "Prop_Controller.h"

CCut_Scene_CF95::CCut_Scene_CF95(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCut_Scene{ pDevice, pContext }
{
}

CCut_Scene_CF95::CCut_Scene_CF95(const CCut_Scene_CF95& rhs)
	: CCut_Scene{ rhs }
{
}

HRESULT CCut_Scene_CF95::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCut_Scene_CF95::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pGameInstance->Add_Object_Sound(m_pTransformCom, 11);
	CCut_Scene_Manager::Get_Instance()->Add_CutScene(CCut_Scene_Manager::CF_ID::_95, this);

	m_Actors[static_cast<_uint>(CF95_ACTOR_TYPE::_EM_0000)]->Set_Render_All_Part(true);
	m_Actors[static_cast<_uint>(CF95_ACTOR_TYPE::_EM_0000)]->Play_Pose_FirstTick(static_cast<_uint>(CActor_EM00::ACTOR_EM00_PART::_BODY));
	m_Actors[static_cast<_uint>(CF95_ACTOR_TYPE::_EM_0000)]->Play_Pose_FirstTick(static_cast<_uint>(CActor_EM00::ACTOR_EM00_PART::_HEAD));
	m_Actors[static_cast<_uint>(CF95_ACTOR_TYPE::_EM_0000)]->Play_Animation_PartObject(static_cast<_uint>(CActor_EM00::ACTOR_EM00_PART::_SHIRTS));
	m_Actors[static_cast<_uint>(CF95_ACTOR_TYPE::_EM_0000)]->Play_Animation_PartObject(static_cast<_uint>(CActor_EM00::ACTOR_EM00_PART::_PANTS));

	return S_OK;
}

void CCut_Scene_CF95::Priority_Tick(_float fTimeDelta)
{
	if (DOWN == m_pGameInstance->Get_KeyState('I'))
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

void CCut_Scene_CF95::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CCut_Scene_CF95::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CCut_Scene_CF95::SetUp_Animation_Layer()
{
	//	For.PL0000
	if (FAILED(m_Actors[static_cast<_uint>(CF95_ACTOR_TYPE::_PL_0000)]->Set_Animation(static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_BODY), TEXT("CF95_PL0000"), 0)))
		return E_FAIL;
	if (FAILED(m_Actors[static_cast<_uint>(CF95_ACTOR_TYPE::_PL_0000)]->Set_Animation(static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_HEAD), TEXT("CF95_PL0050"), 0)))
		return E_FAIL;

	//	For.EM0000
	if (FAILED(m_Actors[static_cast<_uint>(CF95_ACTOR_TYPE::_EM_0000)]->Set_Animation(static_cast<_uint>(CActor_EM00::ACTOR_EM00_PART::_BODY), TEXT("CF95_EM0000"), 0)))
		return E_FAIL;
	if (FAILED(m_Actors[static_cast<_uint>(CF95_ACTOR_TYPE::_EM_0000)]->Set_Animation(static_cast<_uint>(CActor_EM00::ACTOR_EM00_PART::_HEAD), TEXT("CF95_EM0050"), 0)))
		return E_FAIL;

	//	For.WP4530
	if (FAILED(m_Actors[static_cast<_uint>(CF95_ACTOR_TYPE::_WP4530)]->Set_Animation(static_cast<_uint>(CActor_WP4530::ACTOR_WP4530_PART::_BODY), TEXT("CF95_WP4530_00"), 0)))
		return E_FAIL;

	return S_OK;
}

void CCut_Scene_CF95::Start_CutScene()
{
	__super::Start_CutScene();

	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf095_dialogue.bnk.2.stm_2.mp3"), 1);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf095_se_en.bnk.2_1.mp3"), 2);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf095_se_en.bnk.2_2.mp3"), 3);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf095_se_en.bnk.2_3.mp3"), 4);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf095_se_en.bnk.2_4.mp3"), 5);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf095_se_en.bnk.2_5.mp3"), 6);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf095_se_en.bnk.2_6.mp3"), 7);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf095_se_en.bnk.2_7.mp3"), 8);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf095_se_en.bnk.2_8.mp3"), 9);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf095_se_en.bnk.2_9.mp3"), 10);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf095_se_en.bnk.2_10.mp3"), 0);

	CGameObject* pGameObject = { CCall_Center::Get_Instance()->Get_Caller(CCall_Center::CALLER::_PL00) };
	if (nullptr == pGameObject)
		return;

	CPlayer* pPlayer = { static_cast<CPlayer*>(pGameObject) };
	pPlayer->Set_Render(false);
	pPlayer->Set_Spotlight(false);

		pGameObject = { CCall_Center::Get_Instance()->Get_Caller(CCall_Center::CALLER::_WP_4530) };
		CFlashLight* pFlashLight = { static_cast<CFlashLight*>(pGameObject) };
		
		_float4x4* pOriginSocketMatrix = { pFlashLight->Get_Socket_Ptr() };
		
		CModel* pModel = { static_cast<CModel*>(m_Actors[static_cast<_uint>(CF95_ACTOR_TYPE::_PL_0000)]->Get_PartObject(static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_BODY))->Get_Component(TEXT("Com_Model"))) };
		_float4x4* pNewSocketMatrix = { const_cast<_float4x4*>(pModel->Get_CombinedMatrix("r_weapon")) };

		pFlashLight->Set_Socket_Ptr(pNewSocketMatrix);
		pFlashLight->Set_Origin_Translation(true);
		pFlashLight->Set_Right_Handed(true);
		pFlashLight->Set_Render(true);
		m_pOrigin_SocketMatrix = pOriginSocketMatrix;

		static_cast<CActor_WP4530*>(m_Actors[static_cast<_uint>(CCut_Scene_CF95::CF95_ACTOR_TYPE::_WP4530)])->Set_LightSpot(false);
		static_cast<CActor_WP4530*>(m_Actors[static_cast<_uint>(CCut_Scene_CF95::CF95_ACTOR_TYPE::_WP4530)])->Set_Render(false);
}

void CCut_Scene_CF95::Finish_CutScene()
{
	__super::Finish_CutScene();

	CGameObject* pGameObject = { CCall_Center::Get_Instance()->Get_Caller(CCall_Center::CALLER::_PL00) };
	if (nullptr == pGameObject)
		return;

	CPlayer* pPlayer = { static_cast<CPlayer*>(pGameObject) };
	pPlayer->Set_Render(true);
	pPlayer->Set_Spotlight(true);
	m_Actors[static_cast<_uint>(CF95_ACTOR_TYPE::_PL_0000)]->Set_Render_All_Part(false);
	m_Actors[static_cast<_uint>(CF95_ACTOR_TYPE::_EM_0000)]->Set_Render_All_Part(true);
	m_Actors[static_cast<_uint>(CF95_ACTOR_TYPE::_WP4530)]->Set_Render_All_Part(false);

		pGameObject = { CCall_Center::Get_Instance()->Get_Caller(CCall_Center::CALLER::_WP_4530) };
		CFlashLight* pFlashLight = { static_cast<CFlashLight*>(pGameObject) };

		pFlashLight->Set_Socket_Ptr(m_pOrigin_SocketMatrix);
		pFlashLight->Set_Origin_Translation(false);
		pFlashLight->Set_Right_Handed(false);
		
		m_pOrigin_SocketMatrix = nullptr;
}

HRESULT CCut_Scene_CF95::Add_Actors()
{
	m_Actors.resize(static_cast<size_t>(CF95_ACTOR_TYPE::_END));

	CActor::ACTOR_DESC						Actor_PL0000_Desc;
	XMStoreFloat4x4(&Actor_PL0000_Desc.worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	Actor_PL0000_Desc.iBasePartIndex = static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_BODY);
	Actor_PL0000_Desc.iNumParts = static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_END);

	if (FAILED(Add_Actor(TEXT("Prototype_GameObject_Actor_PL0000"), static_cast<_uint>(CF95_ACTOR_TYPE::_PL_0000), &Actor_PL0000_Desc)))
		return E_FAIL;

	CActor_EM00::ACTOR_EM00_DESC			Actor_EM0000_Desc;
	XMStoreFloat4x4(&Actor_EM0000_Desc.worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	Actor_EM0000_Desc.iBasePartIndex = static_cast<_uint>(CActor_EM00::ACTOR_EM00_PART::_BODY);
	Actor_EM0000_Desc.iNumParts = static_cast<_uint>(CActor_EM00::ACTOR_EM00_PART::_END);
	Actor_EM0000_Desc.eFaceType = CActor_EM00::ACTOR_EM00_FACE_TYPE::_BROKEN;

	if (FAILED(Add_Actor(TEXT("Prototype_GameObject_Actor_EM0000"), static_cast<_uint>(CF95_ACTOR_TYPE::_EM_0000), &Actor_EM0000_Desc)))
		return E_FAIL;

	CActor::ACTOR_DESC						Actor_WP4530_Desc;
	XMStoreFloat4x4(&Actor_WP4530_Desc.worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	Actor_WP4530_Desc.iBasePartIndex = static_cast<_uint>(CActor_WP4530::ACTOR_WP4530_PART::_BODY);
	Actor_WP4530_Desc.iNumParts = static_cast<_uint>(CActor_WP4530::ACTOR_WP4530_PART::_END);

	if (FAILED(Add_Actor(TEXT("Prototype_GameObject_Actor_WP4530"), static_cast<_uint>(CF95_ACTOR_TYPE::_WP4530), &Actor_WP4530_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCut_Scene_CF95::Add_Props()
{
	return S_OK;
}

HRESULT CCut_Scene_CF95::Add_Camera_Event()
{
	m_strCamera_Event_Tag = TEXT("cf95");

	m_pEvent_Camera = (CCamera_Event*)m_pGameInstance->Get_GameObject(g_Level, g_strCameraTag, 1);
	if (nullptr == m_pEvent_Camera)
		return E_FAIL;

	if (FAILED(m_pEvent_Camera->Add_CamList(m_strCamera_Event_Tag, TEXT("../Bin/DataFiles/mcamlist/cf095.mcamlist.13"))))
		return E_FAIL;

	Safe_AddRef(m_pEvent_Camera);

	return S_OK;
}

CCut_Scene_CF95* CCut_Scene_CF95::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCut_Scene_CF95* pInstance = new CCut_Scene_CF95(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCut_Scene_CF95"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCut_Scene_CF95::Clone(void* pArg)
{
	CCut_Scene_CF95* pInstance = new CCut_Scene_CF95(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCut_Scene_CF95"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCut_Scene_CF95::Free()
{
	__super::Free();
}
