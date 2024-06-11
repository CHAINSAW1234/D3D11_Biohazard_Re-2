#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_int			iCurrentIndex = {0};
	}NAVIGATION_DESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT						Initialize_Prototype(const wstring& strDataFile);
	virtual HRESULT						Initialize(void* pArg) override;
	void								Tick(_fmatrix WorldMatrix);
	_bool								isMove(_fvector vPosition);
	_vector								GetPlane();
	_float								GetCellHeight(_float4 Position);
	_float4								GetSlidingNormal();
	_float4								GetSlidingNormal_Perfect()
	{
		return m_SlidingNormal;
	}
	_bool								GetIsIsolated()
	{
		return m_bIsIsolated;
	}
	_bool								GetCurrentSlidingVector(_fvector vPosition, _float4* SlidingVector);
	_float4								GetLinePoint()
	{
		return m_LinePoint;
	}
	_bool								FindCell(_fvector vPosition);
	void								SetCurrentIndex(_int Index)
	{
		m_iCurrentIndex = Index;
	}
	_int								GetCurrentIndex()
	{
		return m_iCurrentIndex;
	}
	_float4								CalculateCentroid(_float4 vertex1, _float4 vertex2, _float4 vertex3)
	{
		float centroidX = (vertex1.x + vertex2.x + vertex3.x) / 3.0f;
		float centroidY = (vertex1.y + vertex2.y + vertex3.y) / 3.0f;
		float centroidZ = (vertex1.z + vertex2.z + vertex3.z) / 3.0f;

		float centroidW = 1.0f;

		return _float4(centroidX, centroidY, centroidZ, centroidW);
	}
#ifdef _DEBUG
public:
	virtual HRESULT						Render() override;
#endif

private:
	vector<class CCell*>				m_Cells;

	static _float4x4					m_WorldMatrix;

	_int								m_iCurrentIndex = { -1 };
	_float4								m_SlidingNormal;
	_bool								m_bIsIsolated = { false };
	_float4								m_LinePoint;
	_int								m_iNumFaces = { 0 };

	vector<_float4>						m_vecNavCell_Point;
#ifdef _DEBUG
private:
	class CShader*						m_pShader = { nullptr };
#endif

private:
	HRESULT SetUp_Neighbors();

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strDataFile);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END