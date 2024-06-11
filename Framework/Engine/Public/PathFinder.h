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
	void							Initiate_PathFinding(_uint StartCell,_uint EndCell);
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
		return vector1.x * vector2.z - vector1.z * vector2.x;
	}
	void							Reset();
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
public:
	static CPathFinder* Create();

public:
	virtual void Free() override;
};

END