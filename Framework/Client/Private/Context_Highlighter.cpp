#include "stdafx.h"

#include "Context_Highlighter.h"

CContext_Highlighter::CContext_Highlighter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCustomize_UI{ pDevice , pContext }
{
}

CContext_Highlighter::CContext_Highlighter(const CContext_Highlighter& rhs)
	: CCustomize_UI{ rhs }
{
}

HRESULT CContext_Highlighter::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CContext_Highlighter::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;
	}

	_float4 fMyPos = GetPosition();
	fMyPos.z = 0.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, fMyPos);

	return S_OK;
}

void CContext_Highlighter::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

}

void CContext_Highlighter::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CContext_Highlighter::Render()
{
	__super::Render();

	return S_OK;
}

CContext_Highlighter* CContext_Highlighter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CContext_Highlighter* pInstance = new CContext_Highlighter(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CContext_Highlighter"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CContext_Highlighter::Clone(void* pArg)
{
	CContext_Highlighter* pInstance = new CContext_Highlighter(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CContext_Highlighter"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CContext_Highlighter::Free()
{
	__super::Free();
}
