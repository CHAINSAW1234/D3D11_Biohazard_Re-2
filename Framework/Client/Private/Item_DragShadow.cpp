#include "stdafx.h"

#include "Item_DragShadow.h"

CItem_DragShadow::CItem_DragShadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCustomize_UI{ pDevice , pContext }
{
}

CItem_DragShadow::CItem_DragShadow(const CItem_DragShadow& rhs)
	: CCustomize_UI{ rhs }
{
}

HRESULT CItem_DragShadow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem_DragShadow::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;
	}

	return S_OK;
}

void CItem_DragShadow::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	__super::Tick(fTimeDelta);
}

void CItem_DragShadow::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CItem_DragShadow::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CItem_DragShadow* CItem_DragShadow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem_DragShadow* pInstance = new CItem_DragShadow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CItem_DragShadow"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CItem_DragShadow::Clone(void* pArg)
{
	CItem_DragShadow* pInstance = new CItem_DragShadow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CItem_DragShadow"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_DragShadow::Free()
{
	__super::Free();
}
