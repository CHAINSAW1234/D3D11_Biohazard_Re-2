#include "stdafx.h"
#include "Cut_Scene.h"

#include "Actor.h"
#include "Cut_Scene_CF93.h"

#include "Actor_PL78.h"
#include "Actor_PL00.h"
#include "Actor_EM00.h"

#include "Call_Center.h"
#include "Player.h"
#include "Actor_PartObject.h"

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

	return S_OK;
}

void CCut_Scene_CF93::Priority_Tick(_float fTimeDelta)
{
	if (DOWN == m_pGameInstance->Get_KeyState('Y'))
	{
		m_isPlaying = true;

		for (auto& pActor : m_Actors)
		{
			pActor->Reset_Animations();
			Start();
		}
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

void CCut_Scene_CF93::Finish()
{
	return;

	CGameObject* pGameObject = { CCall_Center::Get_Instance()->Get_Caller(CCall_Center::CALLER::_PL00) };
	if (nullptr == pGameObject)
		return;

	CPlayer* pPlayer = { static_cast<CPlayer*>(pGameObject) };
	CTransform* pPlayerTransform = { pPlayer->Get_Transform() };
	if (nullptr == pPlayerTransform)
		return;

	_vector					vWorldScale = { XMLoadFloat3(&pPlayerTransform->Get_Scaled()) };

	pPlayerTransform->Get_WorldMatrix();

	CModel* pPL00_Model = { static_cast<CModel*>(m_Actors[static_cast<_uint>(CF93_ACTOR_TYPE::_PL_0000)]->Get_PartObject(static_cast<_uint>(CActor_PL00::ACTOR_PL00_PART::_BODY))->Get_Component(TEXT("Com_Model"))) };
	if (nullptr == pPL00_Model)
		return;

	_matrix					CurrentCombinedMatrix = { pPL00_Model->Get_CurrentKeyFrame_Root_CombinedMatrix(0) };
	_matrix					ModelTransformationMtarix = { XMLoadFloat4x4(&pPL00_Model->Get_TransformationMatrix()) };
	_vector					vScaleLocal, vQuaternionLocal, vTranslationLocal;
	XMMatrixDecompose(&vScaleLocal, &vQuaternionLocal, &vTranslationLocal, CurrentCombinedMatrix);

	vTranslationLocal = XMVector3TransformCoord(vTranslationLocal, ModelTransformationMtarix);
	_matrix					ResultCombiendMatrix = { XMMatrixAffineTransformation(vScaleLocal, XMVectorSet(0.f, 0.f, 0.f, 1.f), vQuaternionLocal, vTranslationLocal) };
	_matrix					ScaleMatrix = XMMatrixScalingFromVector(vWorldScale);

	_matrix					ResultMatrix = { ResultCombiendMatrix * ScaleMatrix };
	pPlayer->Move_Manual(ResultMatrix);
	pPlayer->Set_Render(true);
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
