#include "stdafx.h"
#include "Cut_Scene.h"

#include "Actor.h"
#include "Cut_Scene_CF94.h"

#include "Actor_PL00.h"
#include "Actor_PL57.h"
#include "Actor_Em00.h"
#include "Actor_SM69_015.h"
#include "Player.h"
#include "Call_Center.h"
#include "Actor_PartObject.h"

#include "Shutter.h"

#include "Camera_Event.h"
#include "Prop_Controller.h"

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

	_matrix			TransformationMatrix = { XMMatrixRotationY(XMConvertToRadians(90.f)) };
	m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_PL_0000)]->Get_PartObject(static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_BODY))->Set_Local_Transformation(TransformationMatrix);

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

	_uint			iSeqLev = { m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_PL_0000)]->Get_SeqLev() };
	if (1 <= iSeqLev)
	{
		CActor*				pActorPL00 = { m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_PL_0000)] };
		CModel*				pPL00_Body_Model = { static_cast<CModel*>(pActorPL00->Get_PartObject(static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_BODY))->Get_Component(TEXT("Com_Model"))) };

		CActor*				pActorPL57= { m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_PL_5700)] };
		CModel*				pPL57_Body_Model = { static_cast<CModel*>(pActorPL57->Get_PartObject(static_cast<_uint>(CActor_PL57::ACTOR_PL57_PART::_BODY))->Get_Component(TEXT("Com_Model"))) };


		_matrix			PL57CombiendMatrix = { XMLoadFloat4x4(const_cast<_float4x4*>(pPL57_Body_Model->Get_CombinedMatrix("Null_Offset"))) };
		_matrix			PL00CombiendMatrix = { XMLoadFloat4x4(const_cast<_float4x4*>(pPL00_Body_Model->Get_CombinedMatrix("Null_Offset"))) };

		pPL00_Body_Model->Set_CombinedMatrix("Null_Offset", XMMatrixIdentity());

		_vector			vSrcScale, vSrcQuaternion, vSrcTranslation;
		_vector			vDstScale, vDstQuaternion, vDstTranslation;

		/*XMMatrixDecompose(&vSrcScale, &vSrcQuaternion, &vSrcTranslation, PL00CombiendMatrix);
		XMMatrixDecompose(&vDstScale, &vDstQuaternion, &vDstTranslation, PL57CombiendMatrix);

		_vector			vDeltaTranslation = { XMVectorSetW(vDstTranslation - vSrcQuaternion, 0.f) };
		_vector			vDeltaQuaternion = { XMQuaternionMultiply(XMQuaternionInverse(vSrcQuaternion), XMQuaternionNormalize(vDstQuaternion)) };
		
		_matrix			WorldMatrix = { pActorPL00->Get_Transform()->Get_WorldMatrix() };
		_vector			vPostion = { WorldMatrix.r[CTransform::STATE_POSITION] };
		
		_matrix			RotationMatrix = { XMMatrixRotationQuaternion(vDeltaQuaternion) };
		WorldMatrix.r[CTransform::STATE_POSITION] = { XMVectorSet(0.f, 0.f, 0.f, 1.f) };
		WorldMatrix = WorldMatrix * RotationMatrix;

		_matrix			ScaleMatrix = { XMMatrixScaling(0.01f, 0.01f, 0.01f) };
		vPostion = vPostion + XMVector3TransformNormal(vDeltaQuaternion, ScaleMatrix);
		WorldMatrix.r[CTransform::STATE_POSITION] = vPostion;
		pActorPL00->Get_Transform()->Set_WorldMatrix(WorldMatrix);*/

		/*_matrix			WorldMatrix = { pActorPL00->Get_Transform()->Get_WorldMatrix() };

		WorldMatrix.r[CTransform::STATE_POSITION] = PL00CombiendMatrix.r[CTransform::STATE_POSITION] * 0.01f - PL57CombiendMatrix.r[CTransform::STATE_POSITION] * 0.01f;
		pActorPL00->Get_Transform()->Set_WorldMatrix(WorldMatrix);*/
		
	}

	else if (0 == iSeqLev)
	{
		_matrix				RotationMatrix = { XMMatrixIdentity()};
		m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_PL_0000)]->Set_AdditionalRotation_Root(static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_BODY), false, RotationMatrix);

		CActor* pActorPL00 = { m_Actors[static_cast<_uint>(CF94_ACTOR_TYPE::_PL_0000)] };
		CModel* pPL00_Body_Model = { static_cast<CModel*>(pActorPL00->Get_PartObject(static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_BODY))->Get_Component(TEXT("Com_Model"))) };

		pPL00_Body_Model->Set_Surbodinate("COG", false);
	}

	__super::Priority_Tick(fTimeDelta);
}

void CCut_Scene_CF94::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CCut_Scene_CF94::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
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

	return S_OK;
}

void CCut_Scene_CF94::Start_CutScene()
{
	__super::Start_CutScene();

	CGameObject*		pGameObject = { CCall_Center::Get_Instance()->Get_Caller(CCall_Center::CALLER::_PL00) };
	if (nullptr == pGameObject)
		return;

	CPlayer*			pPlayer = { static_cast<CPlayer*>(pGameObject) };
	pPlayer->Set_Render(false);

	CProp_Controller* pProp_Controller = { m_PropControllers[static_cast<_uint>(CF94_PROP_TYPE::_SM_60_033)] };
	CShutter* pShutter = { static_cast<CShutter*>(pProp_Controller->Get_PropObject()) };

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
}
