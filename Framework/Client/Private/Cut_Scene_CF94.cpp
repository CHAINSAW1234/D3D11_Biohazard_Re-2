#include "stdafx.h"
#include "Cut_Scene.h"

#include "Actor.h"
#include "Cut_Scene_CF94.h"
#include "Cut_Scene_Manager.h"

#include "Actor_PL00.h"
#include "Actor_PL57.h"
#include "Actor_Em00.h"
#include "Actor_SM69_015.h"
#include "Actor_WP4530.h"
#include "Player.h"
#include "Call_Center.h"
#include "Actor_PartObject.h"

#include "Shutter.h"

#include "Camera_Event.h"
#include "Prop_Controller.h"

#include "Blood.h"
#include "Blood_Drop.h"

#define BLOOD_COUNT 10
#define BIG_ATTACK_BLOOD_SIZE 6.f
#define BIG_ATTACK_BLOOD_SIZE_DROP 5.f
#define NORMAL_ATTACK_BLOOD_SIZE 4.f
#define NORMAL_ATTACK_BLOOD_SIZE_DROP 3.f
#define SHOTGUN_BLOOD_COUNT 8
#define BLOOD_DROP_COUNT 10
#define BLOOD_DROP_COUNT_STG 10
#define HEADBLOW_BLOOD_SIZE 6.f
#define HEADBLOW_BLOOD_SIZE_DROP 6.f

CCut_Scene_CF94::CCut_Scene_CF94(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCut_Scene{ pDevice, pContext }
{
}

CCut_Scene_CF94::CCut_Scene_CF94(const CCut_Scene_CF94& rhs)
	: CCut_Scene{ rhs }
{
}

HRESULT CCut_Scene_CF94::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCut_Scene_CF94::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pGameInstance->Add_Object_Sound(m_pTransformCom, 25);
	CCut_Scene_Manager::Get_Instance()->Add_CutScene(CCut_Scene_Manager::CF_ID::_94, this);

	m_vecBlood.clear();
	m_vecBlood_Drop.clear();

	for (size_t i = 0; i < BLOOD_COUNT; ++i)
	{
		auto pBlood = CBlood::Create(m_pDevice, m_pContext);
		pBlood->SetSize(HEADBLOW_BLOOD_SIZE, HEADBLOW_BLOOD_SIZE, HEADBLOW_BLOOD_SIZE);
		m_vecBlood.push_back(pBlood);
		pBlood->Start();
	}

	for (size_t i = 0; i < BLOOD_DROP_COUNT; ++i)
	{
		auto pBlood_Drop = CBlood_Drop::Create(m_pDevice, m_pContext);
		pBlood_Drop->SetSize(HEADBLOW_BLOOD_SIZE_DROP, HEADBLOW_BLOOD_SIZE_DROP, HEADBLOW_BLOOD_SIZE_DROP);
		m_vecBlood_Drop.push_back(pBlood_Drop);
		pBlood_Drop->SetDropDir_CutScene();
		pBlood_Drop->SetCutScene_Blood(true);
		pBlood_Drop->Start();
	}

	CActor_PartObject* pPartBody = m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_EM_0000)]->Get_PartObject(static_cast<_uint>(CActor_EM00::ACTOR_EM00_PART::_BODY));
	CModel* pBodyModel = { static_cast<CModel*>(pPartBody->Get_Component(TEXT("Com_Model"))) };
	m_SpineCombined = pBodyModel->Get_CombinedMatrix("neck_1");

	m_BloodDelay =20;

	return S_OK;
}

void CCut_Scene_CF94::Priority_Tick(_float fTimeDelta)
{
	if (DOWN == m_pGameInstance->Get_KeyState('U'))
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

	//	1번 시퀀스 240프레임

	_uint			iSeqLev = { m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_PL_0000)]->Get_SeqLev() };
	if (0 == iSeqLev)
	{
		m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_PL_5700)]->Set_Render_All_Part(false);
		m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_WP4530)]->Set_Render_All_Part(false);
	}

	if (1 == iSeqLev)
	{
		CActor_PartObject* pPartBody = m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_EM_0000)]->Get_PartObject(static_cast<_uint>(CActor_EM00::ACTOR_EM00_PART::_BODY));
		CModel* pBodyModel = { static_cast<CModel*>(pPartBody->Get_Component(TEXT("Com_Model"))) };

		m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_WP4530)]->Set_Render_All_Part(false);
		m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_PL_5700)]->Set_Render_All_Part(true);


		_float fTrackPosition = { pBodyModel->Get_TrackPosition(0) };
		if (550.f < fTrackPosition)
		{
			CActor_PartObject* pPartHead = m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_EM_0000)]->Get_PartObject(static_cast<_uint>(CActor_EM00::ACTOR_EM00_PART::_HEAD));
			CActor_PartObject* pPartBrokenHead = m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_SM69_015)]->Get_PartObject(static_cast<_uint>(CActor_SM69_015::ACTOR_SM69_015_PART::_HEAD));

			pPartHead->Set_Render(false);
			pPartBrokenHead->Set_Render(true);
		}

		if (550.f < fTrackPosition && 580.f > fTrackPosition)
		{
			m_bBlood = true;
		}
	}	
	__super::Priority_Tick(fTimeDelta);
}

void CCut_Scene_CF94::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (m_bBlood)
	{
		SetBlood();
	}

	for (size_t i = 0; i < m_vecBlood.size(); ++i)
	{
		m_vecBlood[i]->Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecBlood_Drop.size(); ++i)
	{
		m_vecBlood_Drop[i]->Tick(fTimeDelta);
	}
}

void CCut_Scene_CF94::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	for (size_t i = 0; i < m_vecBlood.size(); ++i)
	{
		m_vecBlood[i]->Late_Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecBlood_Drop.size(); ++i)
	{
		m_vecBlood_Drop[i]->Late_Tick(fTimeDelta);
	}
}

HRESULT CCut_Scene_CF94::SetUp_Animation_Layer()
{
	//	For.PL0000
	if (FAILED(m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_PL_0000)]->Set_Animation(static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_BODY), TEXT("CF94_PL0000"), 0)))
		return E_FAIL;
	if (FAILED(m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_PL_0000)]->Set_Animation(static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_HEAD), TEXT("CF94_PL0050"), 0)))
		return E_FAIL;

	//	For.PL5700
	if (FAILED(m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_PL_5700)]->Set_Animation(static_cast<_uint>(CActor_PL57::ACTOR_PL57_PART::_BODY), TEXT("CF94_PL5700"), 0)))
		return E_FAIL;
	if (FAILED(m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_PL_5700)]->Set_Animation(static_cast<_uint>(CActor_PL57::ACTOR_PL57_PART::_HEAD), TEXT("CF94_PL5750"), 0)))
		return E_FAIL;

	//	For.EM0000
	if (FAILED(m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_EM_0000)]->Set_Animation(static_cast<_uint>(CActor_EM00::ACTOR_EM00_PART::_BODY), TEXT("CF94_EM0000"), 0)))
		return E_FAIL;
	if (FAILED(m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_EM_0000)]->Set_Animation(static_cast<_uint>(CActor_EM00::ACTOR_EM00_PART::_HEAD), TEXT("CF94_EM0050"), 0)))
		return E_FAIL;

	//	For.SM69_015
	if (FAILED(m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_SM69_015)]->Set_Animation(static_cast<_uint>(CActor_SM69_015::ACTOR_SM69_015_PART::_HEAD), TEXT("CF94_SM69_015_00"), 0)))
		return E_FAIL;

	//	For.WP4530
	if (FAILED(m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_WP4530)]->Set_Animation(static_cast<_uint>(CActor_WP4530::ACTOR_WP4530_PART::_BODY), TEXT("CF94_WP4530_00"), 0)))
		return E_FAIL;

	return S_OK;
}

void CCut_Scene_CF94::Start_CutScene()
{
	__super::Start_CutScene();

	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_dailogue.bnk.2.stm_1.mp3"), 0);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_dailogue.bnk.2.stm_3.mp3"), 1);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_music_en.bnk.2_1.mp3"), 2);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_se_en.bnk.2_1.mp3"), 3);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_se_en.bnk.2_2.mp3"), 4);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_se_en.bnk.2_3.mp3"), 5);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_se_en.bnk.2_4.mp3"), 6);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_se_en.bnk.2_5.mp3"), 7);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_se_en.bnk.2_6.mp3"), 8);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_se_en.bnk.2_7.mp3"), 9);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_se_en.bnk.2_8.mp3"), 10);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_se_en.bnk.2_9.mp3"), 11);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_se_en.bnk.2_10.mp3"), 12);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_se_en.bnk.2_11.mp3"), 13);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_se_en.bnk.2_12.mp3"), 14);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_se_en.bnk.2_13.mp3"), 15);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_se_en.bnk.2_14.mp3"), 16);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_se_en.bnk.2_15.mp3"), 17);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_se_en.bnk.2_16.mp3"), 18);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_se_en.bnk.2_17.mp3"), 19);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_se_en.bnk.2_18.mp3"), 20);
	//	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_se_en.bnk.2_19.mp3"), 21);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_se_en.bnk.2_20.mp3"), 22);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_se_en.bnk.2_21.mp3"), 23);
	m_pGameInstance->Change_Sound_3D(m_pTransformCom, TEXT("cf094_se_en.bnk.2_22.mp3"), 24);


	CGameObject*		pGameObject = { CCall_Center::Get_Instance()->Get_Caller(CCall_Center::CALLER::_PL00) };
	if (nullptr == pGameObject)
		return;

	CPlayer*			pPlayer = { static_cast<CPlayer*>(pGameObject) };
	pPlayer->Set_Render(false);
	pPlayer->Set_Spotlight(false);

	CProp_Controller* pProp_Controller = { m_PropControllers[static_cast<_uint>(CF94_PROP_TYPE::_SM_60_033)] };
	CShutter* pShutter = { static_cast<CShutter*>(pProp_Controller->Get_PropObject()) };

	CActor_PartObject*		pPartHead = m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_EM_0000)]->Get_PartObject(static_cast<_uint>(CActor_EM00::ACTOR_EM00_PART::_HEAD));
	CActor_PartObject*		pPartBrokenHead = m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_SM69_015)]->Get_PartObject(static_cast<_uint>(CActor_SM69_015::ACTOR_SM69_015_PART::_HEAD));

	pPartBrokenHead->Set_Render(false);

	CModel*					pHeadModel = { static_cast<CModel*>(pPartHead->Get_Component(TEXT("Com_Model"))) };
	CModel*					pBrokenHeadModel = { static_cast<CModel*>(pPartBrokenHead->Get_Component(TEXT("Com_Model"))) };


	pBrokenHeadModel->Link_Bone_Auto(pHeadModel);

	if (nullptr == pShutter)
		return;
	pShutter->Set_OutOfControll(true);
}

void CCut_Scene_CF94::Finish_CutScene()
{
	__super::Finish_CutScene();

	CGameObject*		pGameObject = { CCall_Center::Get_Instance()->Get_Caller(CCall_Center::CALLER::_PL00) };
	if (nullptr == pGameObject)
		return;

	CPlayer*			pPlayer = { static_cast<CPlayer*>(pGameObject) };
	pPlayer->Set_Render(true);

	CProp_Controller* pProp_Controller = { m_PropControllers[static_cast<_uint>(CF94_PROP_TYPE::_SM_60_033)] };
	CShutter* pShutter = { static_cast<CShutter*>(pProp_Controller->Get_PropObject()) };

	if (nullptr == pShutter)
		return;

	pShutter->Set_OutOfControll(false);

	m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_PL_0000)]->Set_Render_All_Part(false);
	m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_SM69_015)]->Set_Render_All_Part(true);
	m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_EM_0000)]->Set_Render_All_Part(true);
	m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_EM_0000)]->Get_PartObject(static_cast<_uint>(CActor_EM00::ACTOR_EM00_PART::_HEAD))->Set_Render(false);

	static_cast<CActor_PL57*>(m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_PL_5700)])->Set_Idle_Loop();
	static_cast<CActor_PL57*>(m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_PL_5700)])->Set_Render_All_Part(true);
	static_cast<CActor_PL57*>(m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_PL_5700)])->Set_Pause_Anim_All_Part(false);
}

HRESULT CCut_Scene_CF94::Add_Actors()
{
	m_Actors.resize(static_cast<size_t>(CF94_ACTOR_TYPE::_END));

	CActor::ACTOR_DESC			Actor_PL0000_Desc;
	XMStoreFloat4x4(&Actor_PL0000_Desc.worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	Actor_PL0000_Desc.iBasePartIndex = static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_BODY);
	Actor_PL0000_Desc.iNumParts = static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_END);

	if (FAILED(Add_Actor(TEXT("Prototype_GameObject_Actor_PL0000"), static_cast<_uint>(CF94_ACTOR_TYPE::_PL_0000), &Actor_PL0000_Desc)))
		return E_FAIL;

	CActor::ACTOR_DESC			Actor_PL5700_Desc;
	XMStoreFloat4x4(&Actor_PL5700_Desc.worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	Actor_PL5700_Desc.iBasePartIndex = static_cast<_uint>(CActor_PL57::ACTOR_PL57_PART::_BODY);
	Actor_PL5700_Desc.iNumParts = static_cast<_uint>(CActor_PL57::ACTOR_PL57_PART::_END);

	if (FAILED(Add_Actor(TEXT("Prototype_GameObject_Actor_PL5700"), static_cast<_uint>(CF94_ACTOR_TYPE::_PL_5700), &Actor_PL5700_Desc)))
		return E_FAIL;

	CActor_EM00::ACTOR_EM00_DESC			Actor_EM0000_Desc;
	XMStoreFloat4x4(&Actor_EM0000_Desc.worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	Actor_EM0000_Desc.iBasePartIndex = static_cast<_uint>(CActor_EM00::ACTOR_EM00_PART::_BODY);
	Actor_EM0000_Desc.iNumParts = static_cast<_uint>(CActor_EM00::ACTOR_EM00_PART::_END);
	Actor_EM0000_Desc.eFaceType = CActor_EM00::ACTOR_EM00_FACE_TYPE::_DEFAULT;

	if (FAILED(Add_Actor(TEXT("Prototype_GameObject_Actor_EM0000"), static_cast<_uint>(CF94_ACTOR_TYPE::_EM_0000), &Actor_EM0000_Desc)))
		return E_FAIL;


	CActor::ACTOR_DESC			Actor_SM69_015_Desc;
	XMStoreFloat4x4(&Actor_SM69_015_Desc.worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	Actor_SM69_015_Desc.iBasePartIndex = static_cast<_uint>(CActor_SM69_015::ACTOR_SM69_015_PART::_HEAD);
	Actor_SM69_015_Desc.iNumParts = static_cast<_uint>(CActor_SM69_015::ACTOR_SM69_015_PART::_END);

	if (FAILED(Add_Actor(TEXT("Prototype_GameObject_Actor_SM69_015"), static_cast<_uint>(CF94_ACTOR_TYPE::_SM69_015), &Actor_SM69_015_Desc)))
		return E_FAIL;	

	CActor::ACTOR_DESC			Actor_WP4530_Desc;
	XMStoreFloat4x4(&Actor_WP4530_Desc.worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	Actor_WP4530_Desc.iBasePartIndex = static_cast<_uint>(CActor_WP4530::ACTOR_WP4530_PART::_BODY);
	Actor_WP4530_Desc.iNumParts = static_cast<_uint>(CActor_WP4530::ACTOR_WP4530_PART::_END);

	if (FAILED(Add_Actor(TEXT("Prototype_GameObject_Actor_WP4530"), static_cast<_uint>(CF94_ACTOR_TYPE::_WP4530), &Actor_WP4530_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCut_Scene_CF94::Add_Props()
{
	m_PropControllers.resize(static_cast<size_t>(CF94_PROP_TYPE::_END));

	CProp_Controller::PROP_CONTROLL_DESC			Prop_SM60_033_Desc;
	Prop_SM60_033_Desc.strAnimLayerTag = TEXT("CF94_SM60_033_00");

	if (FAILED(Add_PropController(TEXT("Prototype_GameObject_Prop_SM60_033"), static_cast<_uint>(CF94_PROP_TYPE::_SM_60_033), &Prop_SM60_033_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCut_Scene_CF94::Add_Camera_Event()
{
	m_strCamera_Event_Tag = TEXT("cf94");

	m_pEvent_Camera = (CCamera_Event*)m_pGameInstance->Get_GameObject(g_Level, g_strCameraTag, 1);
	if (nullptr == m_pEvent_Camera)
		return E_FAIL;

	if(FAILED(m_pEvent_Camera->Add_CamList(m_strCamera_Event_Tag, TEXT("../Bin/DataFiles/mcamlist/cf094.mcamlist.13"))))
		return E_FAIL;

	Safe_AddRef(m_pEvent_Camera);

	return S_OK;
}

void CCut_Scene_CF94::Finish_Marvine()
{
	static_cast<CActor_PL57*>(m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_PL_5700)])->Set_Render_All_Part(false);
	static_cast<CActor_PL57*>(m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_PL_5700)])->Set_Pause_Anim_All_Part(true);
}

void CCut_Scene_CF94::SetBlood()
{
	if (m_iBloodCount >= m_vecBlood.size())
	{
		m_bBlood = false;
		m_iBloodCount = 0;
		return;
	}

	if (m_BloodDelay + m_BloodTime < GetTickCount64())
	{
		m_BloodTime = GetTickCount64();

		m_vecBlood[m_iBloodCount]->Set_Render(true);
		//m_vecBlood[m_iBloodCount]->SetWorldMatrix_With_HitNormal(m_vHitNormal);
		m_vecBlood[m_iBloodCount]->SetType(m_pGameInstance->GetRandom_Int(0, 10));
		m_vecBlood[m_iBloodCount]->SetSize(HEADBLOW_BLOOD_SIZE, HEADBLOW_BLOOD_SIZE, HEADBLOW_BLOOD_SIZE);

		auto vPos = _float4(m_SpineCombined->_41 * 0.01f+0.15f, m_SpineCombined->_42 * 0.01f, m_SpineCombined->_43 * 0.01f, 1.f);

		_float4 vDelta = _float4(m_pGameInstance->GetRandom_Real(-0.1f, 0.1f),
			m_pGameInstance->GetRandom_Real(-0.1f, 0.f),
			m_pGameInstance->GetRandom_Real(-0.1f, 0.1f),
			0.f);
		vPos += vDelta;

		m_vecBlood[m_iBloodCount]->SetPosition(vPos);

		if (m_iBloodCount < BLOOD_DROP_COUNT)
		{
			//m_vecBlood_Drop[m_iBloodCount]->SetWorldMatrix_With_HitNormal(m_vHitNormal);
			m_vecBlood_Drop[m_iBloodCount]->Set_Render(true);
			m_vecBlood_Drop[m_iBloodCount]->SetPosition(m_vecBlood[m_iBloodCount]->GetPosition());
			m_vecBlood_Drop[m_iBloodCount]->SetType(m_pGameInstance->GetRandom_Int(0, 10));
			m_vecBlood_Drop[m_iBloodCount]->SetSize(HEADBLOW_BLOOD_SIZE_DROP, HEADBLOW_BLOOD_SIZE_DROP, HEADBLOW_BLOOD_SIZE_DROP);
		}

		++m_iBloodCount;

		if (m_iBloodCount >= m_vecBlood.size())
		{
			m_bBlood = false;
			m_iBloodCount = 0;
			return;
		}
	}
}

CCut_Scene_CF94* CCut_Scene_CF94::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCut_Scene_CF94* pInstance = new CCut_Scene_CF94(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCut_Scene_CF94"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCut_Scene_CF94::Clone(void* pArg)
{
	CCut_Scene_CF94* pInstance = new CCut_Scene_CF94(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCut_Scene_CF94"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCut_Scene_CF94::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_vecBlood.size(); ++i)
	{
		Safe_Release(m_vecBlood[i]);
	}

	for (size_t i = 0; i < m_vecBlood_Drop.size(); ++i)
	{
		Safe_Release(m_vecBlood_Drop[i]);
	}
}
