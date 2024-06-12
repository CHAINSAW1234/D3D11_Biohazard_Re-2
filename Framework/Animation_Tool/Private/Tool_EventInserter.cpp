#include "stdafx.h"
#include "Tool_EventInserter.h"

CTool_EventInserter::CTool_EventInserter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTool{ pDevice, pContext }
{
}

HRESULT CTool_EventInserter::Initialize(void* pArg)
{
	return S_OK;
}

void CTool_EventInserter::Tick(_float fTimeDelta)
{
}

CTool_EventInserter* CTool_EventInserter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTool_EventInserter* pInstance = { new CTool_EventInserter(pDevice, pContext) };
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTool_EventInserter"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTool_EventInserter::Free()
{
	__super::Free();
}
