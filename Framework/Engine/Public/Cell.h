#pragma once

#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT {POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE {LINE_AB, LINE_BC, LINE_CA, LINE_END };
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_vector							Get_Point(POINT ePoint) 
	{
		return XMVectorSetW(XMLoadFloat3(&m_vPoints[ePoint]), 1.f);					// ***** 워닝 잡았음 문제 생기면 다시 체크 바람
	}
	_float4							Get_Point_Float4(_int iPoint)
	{
		return _float4(m_vPoints[iPoint].x, m_vPoints[iPoint].y, m_vPoints[iPoint].z, 1.f);		// ***** 워닝 잡았음 문제 생기면 다시 체크 바람
	}
	void							SetUp_Neighbor(LINE eLine, CCell* pNeighbor)
	{
		m_iNeighborIndices[eLine] = pNeighbor->m_iIndex;
	}

public:
	HRESULT							Initialize(const _float3* pPoints, _uint iIndex);
	_bool							isIn(_fvector vPosition, _fmatrix TerrainWorldMatrix, _int* pNeighborIndex, _float4* SlidingNormal, _bool* IsIsolated, _float4* LinePoint = nullptr);
	_bool							isIn(_fvector vPosition, _fmatrix TerrainWorldMatrix, _int* pNeighborIndex);

	_bool							Compare_Points(_fvector vSourPoint, _fvector vDestPoint);
	_float4							GetSlidingNormal()
	{
		return m_SlidingNormal;
	}
	_float4							GetLinePoint()
	{
		return m_LinePoint;
	}
	_vector							GetPlane();
	_int*							GetNeighborIndices()
	{
		return m_iNeighborIndices;
	}
#ifdef _DEBUG
public:
	virtual HRESULT					Render();
#endif

private:
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };

private:
	_float3							m_vPoints[POINT_END];
	_uint							m_iIndex = { 0 };

	_int							m_iNeighborIndices[LINE_END] = { -1, -1, -1 };
	_float4							m_SlidingNormal;
	_float4							m_LinePoint;
#ifdef _DEBUG

private:
	class CVIBuffer_Cell*			m_pVIBuffer = { nullptr };
#endif

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* vPoints, _uint iIndex);
	virtual void Free() override;
};

END