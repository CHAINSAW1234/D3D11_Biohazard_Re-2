#include "stdafx.h"
#include "Tool_IK.h"

CTool_IK::CTool_IK(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTool{ pDevice, pContext }
{
}

HRESULT CTool_IK::Initialize(void* pArg)
{
	return S_OK;
}

void CTool_IK::Tick(_float fTimeDelta)
{
}

CTool_IK* CTool_IK::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTool_IK* pInstance = { new CTool_IK(pDevice, pContext) };
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTool_IK"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTool_IK::Free()
{
	__super::Free();
}
