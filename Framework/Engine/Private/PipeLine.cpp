#include "..\Public\PipeLine.h"

CPipeLine::CPipeLine()
{
}

HRESULT CPipeLine::Initialize()
{
	for (size_t i = 0; i < PIPELINE_END; ++i) {
		for (size_t j = 0; j < D3DTS_END; j++)
		{
			XMStoreFloat4x4(&m_TransformMatrices[i][j], XMMatrixIdentity());
			XMStoreFloat4x4(&m_TransformInverseMatrices[i][j], XMMatrixIdentity());
		}
		m_vCamPosition[i] = _float4(0.f, 0.f, 0.f, 1.f);
	}
	
	return S_OK;
}

void CPipeLine::Tick()
{
	for (size_t i = 0; i < PIPELINE_END; ++i) {
		for (size_t j = 0; j < D3DTS_END; j++)
		{
			XMStoreFloat4x4(&m_TransformInverseMatrices[i][j], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrices[i][j])));
		}

		memcpy(&m_vCamPosition, &m_TransformInverseMatrices[i][D3DTS_VIEW].m[3][0], sizeof(_float4));
	}
}

CPipeLine * CPipeLine::Create()
{
	CPipeLine*		pInstance = new CPipeLine();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CPipeLine"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPipeLine::Free()
{
}
