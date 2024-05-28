#include "..\Public\Cell.h"
#include "VIBuffer_Cell.h"

CCell::CCell(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3 * vPoints, _uint iIndex)
{
	memcpy(m_vPoints, vPoints, sizeof(_float3) * POINT_END);

	m_iIndex = iIndex;

#ifdef _DEBUG

	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

#endif

	return S_OK;
}

_vector CCell::GetPlane()
{
	_vector Point1 = XMLoadFloat3(&m_vPoints[POINT_A]);
	_vector Point2 = XMLoadFloat3(&m_vPoints[POINT_B]);
	_vector Point3 = XMLoadFloat3(&m_vPoints[POINT_C]);
	Point1 = XMVectorSetW(Point1, 1.f);
	Point2 = XMVectorSetW(Point2, 1.f);
	Point3 = XMVectorSetW(Point3, 1.f);
	_vector Plane = XMPlaneFromPoints(Point1, Point2, Point3);

	return Plane;
}

_bool CCell::isIn(_fvector vPosition, _fmatrix TerrainWorldMatrix, _int* pNeighborIndex, _float4* SlidingNormal, _bool* IsIsolated, _float4* LinePoint)
{
	_vector		vPoints[3];

	for (size_t i = 0; i < POINT_END; i++)
		vPoints[i] = XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), TerrainWorldMatrix);

	for (size_t i = 0; i < LINE_END; i++)
	{
		_vector		vSourDir = XMVector3Normalize(vPosition - vPoints[i]);
		//vSourDir = XMVectorSetY(vSourDir, 0.f);
		_int Index = i + 1 >= POINT_END ? 0 : i + 1;
		_vector		vDir = vPoints[Index] - vPoints[i];
		_vector		vDestDir = XMVector3Normalize(XMVectorSet(XMVectorGetZ(vDir) * -1.f, 0.f, XMVectorGetX(vDir), 0.0f));

		if (0.0002f < XMVectorGetX(XMVector3Dot(vSourDir, vDestDir)))
		{
			//vPoints[0] = XMVectorSetY(vPoints[0], 0.f);
			//vPoints[1] = XMVectorSetY(vPoints[1], 0.f);
			//vPoints[2] = XMVectorSetY(vPoints[2], 0.f);
			//_vector Pos = XMVectorSetY(vPosition, 0.f);
			_vector Line1 = vPoints[2] - vPoints[0];
			_vector Line2 = vPoints[1] - vPoints[0];

			_float Dist1 = XMVectorGetX(XMVector4Length(vPoints[0] - vPosition));
			_float Dist2 = XMVectorGetX(XMVector4Length(vPoints[1] - vPosition));
			_float Dist3 = XMVectorGetX(XMVector4Length(vPoints[2] - vPosition));

			_float Min = min(min(Dist1, Dist2), Dist3);

			if (Min == Dist1)
			{
				XMStoreFloat4(LinePoint, vPoints[0]);
			}

			if (Min == Dist2)
			{
				XMStoreFloat4(LinePoint, vPoints[1]);
			}

			if (Min == Dist3)
			{
				XMStoreFloat4(LinePoint, vPoints[2]);
			}
			_vector Normal = XMVector3Cross(Line2, Line1);
			Normal = XMVector4Normalize(Normal);
			Normal = XMVector3Cross(Normal, vDir);
			XMStoreFloat4(&m_SlidingNormal, Normal);
			XMStoreFloat4(&m_SlidingNormal, vDir);
			*SlidingNormal = m_SlidingNormal;

			*pNeighborIndex = m_iNeighborIndices[i];

			_int InvalidNeighborCount = 0;
			for (int i = 0; i < 3; ++i)
			{
				if (m_iNeighborIndices[i] == -1)
					++InvalidNeighborCount;
			}

			if (InvalidNeighborCount > 1)
			{
				*IsIsolated = true;
			}
			else
			{
				*IsIsolated = false;
			}

			return false;
		}
	}

	return true;
}

_bool CCell::isIn(_fvector vPosition, _fmatrix TerrainWorldMatrix, _int* pNeighborIndex)
{
	_vector		vPoints[3];

	for (size_t i = 0; i < POINT_END; i++)	
		vPoints[i] = XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), TerrainWorldMatrix);

	for (size_t i = 0; i < LINE_END; i++)
	{
		_vector		vSourDir = XMVector3Normalize(vPosition - vPoints[i]);

		_vector		vDir = vPoints[i + 1 >= POINT_END ? 0 : i + 1] - vPoints[i];
		_vector		vDestDir = XMVector3Normalize(XMVectorSet(XMVectorGetZ(vDir) * -1.f, 0.f, XMVectorGetX(vDir), 0.0f));

		if (0 < XMVectorGetX(XMVector3Dot(vSourDir, vDestDir)))
		{
			*pNeighborIndex = m_iNeighborIndices[i];
			return false;
		}
	}

	return true;
}

_bool CCell::Compare_Points(_fvector vSourPoint, _fvector vDestPoint)
{	
	if (true == XMVector3Equal(vSourPoint, XMLoadFloat3(&m_vPoints[POINT_A])))
	{
		if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[POINT_B])))
			return true;	

		if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[POINT_C])))
			return true;		
	}

	if (true == XMVector3Equal(vSourPoint, XMLoadFloat3(&m_vPoints[POINT_B])))
	{
		if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[POINT_C])))
			return true;

		if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[POINT_A])))
			return true;
	}

	if (true == XMVector3Equal(vSourPoint, XMLoadFloat3(&m_vPoints[POINT_C])))
	{
		if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[POINT_A])))
			return true;

		if (true == XMVector3Equal(vDestPoint, XMLoadFloat3(&m_vPoints[POINT_B])))
			return true;
	}

	return false;
}

#ifdef _DEBUG

HRESULT CCell::Render()
{
	m_pVIBuffer->Bind_Buffers();

	return m_pVIBuffer->Render();	
}

#endif

CCell * CCell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _float3 * pPoints, _uint iIndex)
{
	CCell*		pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX(TEXT("Failed To Created : CCell"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCell::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
