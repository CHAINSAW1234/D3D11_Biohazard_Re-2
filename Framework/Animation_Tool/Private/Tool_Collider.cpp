#include "stdafx.h"
#include "Tool_Collider.h"

#include "ImGuizmo.h"

CTool_Collider::CTool_Collider()
{
}

HRESULT CTool_Collider::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTool_Collider::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	static _bool isChanged = { false };
}

CGameObject* CTool_Collider::Get_CurrentSelectCollider()
{
	return m_pCurrentSelectCollider;
}

CTool_Collider* CTool_Collider::Create(void* pArg)
{
	CTool_Collider* pInatnace = new CTool_Collider();

	if (FAILED(pInatnace->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTool_Collider"));

		Safe_Release(pInatnace);
	}

	return pInatnace;
}

void CTool_Collider::Free()
{
	__super::Free();
}
