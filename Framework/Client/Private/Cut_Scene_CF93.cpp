#include "stdafx.h"
#include "Cut_Scene.h"

#include "Actor.h"
#include "Cut_Scene_CF93.h"

#include "Actor_PL78.h"

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

HRESULT CCut_Scene_CF93::Add_Actors()
{
	m_Actors.resize(static_cast<size_t>(CF93_ACTOR_TYPE::_END));

	CActor::ACTOR_DESC			ActorDesc;
	XMStoreFloat4x4(&ActorDesc.worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));
	ActorDesc.iBasePartIndex = static_cast<_uint>(CActor_PL78::ACTOR_PL78_PART::_BODY);
	ActorDesc.iNumParts = static_cast<_uint>(CActor_PL78::ACTOR_PL78_PART::_END);
	
	if (FAILED(Add_Actor(TEXT("Prototype_GameObject_Actor_PL7800"), static_cast<_uint>(CF93_ACTOR_TYPE::_PL_7800), &ActorDesc)))
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
