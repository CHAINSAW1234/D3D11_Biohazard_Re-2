#include "stdafx.h"
#include "Tool_Collider.h"

#include "ImGuizmo.h"

CTool_Collider::CTool_Collider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTool{ pDevice, pContext }
{
}

HRESULT CTool_Collider::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_strCollasingTag = "Tool_Collider";

	return S_OK;
}

void CTool_Collider::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	static _bool isChanged = { false };

	if (ImGui::CollapsingHeader(m_strCollasingTag.c_str()))
	{

	}
}

CGameObject* CTool_Collider::Get_CurrentSelectCollider()
{
	return m_pCurrentSelectCollider;
}

CTool_Collider* CTool_Collider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTool_Collider* pInatnace = new CTool_Collider(pDevice, pContext);

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
