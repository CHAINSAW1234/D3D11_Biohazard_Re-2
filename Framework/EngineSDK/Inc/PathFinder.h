#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CPathFinder : public CBase
{
protected:
	CPathFinder();
	CPathFinder(const CPathFinder& rhs);
	virtual ~CPathFinder() = default;

public:
	virtual HRESULT					Initialize_Prototype();
	virtual HRESULT					Initialize(void* pArg);

public:
	void							Init_PathFinder();
	void							Init_Visibility_Optimization();
	void							Initiate_PathFinding(_uint StartCell,_uint EndCell,_float4 vStartPos);
	bool							IsValid(_uint Index);
	bool							IsUnBlocked(_uint Index)
	{
		if (Index < 0 || Index > m_iCellCount)
			return false;
		else
			return true;
	}
	void							TracePath(_uint Start, _uint End);
	_float							CalculateHValue(_uint Start, _uint End);
	_float							Calc_Dist(_float4 vSrc, _float4 vDst);
	void							Path_Optimization();
	void							SetStartPos(_float4 vStartPos)
	{
		m_vStartPos = vStartPos;
	}
	_float							VectorSign(_float4 vector1, _float4 vector2)
	{
		vector1 = Float4_Normalize(vector1);
		vector2 = Float4_Normalize(vector2);
		
		return vector1.x * vector2.z - vector2.z * vector1.x;
	}
	float calculateAngle(const _float4& vec) 
	{
		float angle = std::atan2(vec.z, vec.x);

		if (angle < 0) {
			angle += XM_2PI;
		}
		return angle;
	}
	_float							VectorSign_Global(_float4 vector1, _float4 vector2)
	{
		float angle1 = calculateAngle(vector1);
		float angle2 = calculateAngle(vector2);

		if (angle1 >= angle2) {
			return 1;
		}
		else {
			return -1;
		}
	}
	void							Reset();
	stack<_uint>					GetPath()
	{
		return m_Path;
	}
	_float4							GetNextTarget();
	_float4							GetNextTarget_Opt();
	FORCEINLINE _float triarea2(const _float* a, const _float* b, const _float* c)
	{
		const _float ax = b[0] - a[0];
		const _float az = b[1] - a[1];
		const _float bx = c[0] - a[0];
		const _float bz = c[1] - a[1];
		return bx * az - ax * bz;
	}
	FORCEINLINE _float vdistsqr(const _float* a, const _float* b)
	{
		_float dx = a[0] - b[0];
		_float dz = a[1] - b[1];
		return dx * dx + dz * dz;
	}
	FORCEINLINE _bool vequal(const _float* a, const _float* b)
	{
		static const float eq = 0.001f * 0.001f;
		return vdistsqr(a, b) < eq;
	}
	FORCEINLINE void vcpy(float* dst, const float* src) {
		dst[0] = src[0];
		dst[1] = src[2];
	}
protected:
	class CGameInstance*			m_pGameInstance = { nullptr };

private:
	bool*							m_closedList = { nullptr };
	CELL*							m_cellDetails = { nullptr };
	stack<_uint>					m_Path;
	vector<_uint>					m_Path_Optimization;
	_uint							m_iCellCount = { 0 };

	vector<_float4>*				m_vecNavCell_Point;
	vector<class CCell*>*			m_Cells;

	vector<_float4>					m_vecLeft_Vertices;
	vector<_float4>					m_vecRight_Vertices;
	_float4							m_vStartPos;
	vector<_float4>					m_vecPath_Optimization;
	vector<_float4>					m_vecPortals;
public:
	static CPathFinder* Create();

public:
	virtual void Free() override;
};

END