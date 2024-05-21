#pragma once

#include "Base.h"

/* 렌더링파이프라인에 필요한 ViewMatrix, ProjMatrix보존하낟. */
/* 기타 클라이언트 로직에 필요한 데이터들을 만들고 저장ㅎ나다. (View, Proj Inv, CamPosition */

BEGIN(Engine)

class CPipeLine final : public CBase
{
public:
	enum PIPELINE { CAMERA, SHADOW, PIPELINE_END };
	enum TRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };
private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	void Set_Transform(TRANSFORMSTATE eState, _fmatrix TransformMatrix, PIPELINE ePipeLine) {
		XMStoreFloat4x4(&m_TransformMatrices[eState][ePipeLine], TransformMatrix);
	}
public:
	_matrix Get_Transform_Matrix(TRANSFORMSTATE eState, PIPELINE ePipeLine) const {
		return XMLoadFloat4x4(&m_TransformMatrices[eState][ePipeLine]);
	}
	_float4x4 Get_Transform_Float4x4(TRANSFORMSTATE eState, PIPELINE ePipeLine) const {
		return m_TransformMatrices[eState][ePipeLine];
	}
	_matrix Get_Transform_Matrix_Inverse(TRANSFORMSTATE eState, PIPELINE ePipeLine) const {
		return XMLoadFloat4x4(&m_TransformInverseMatrices[eState][ePipeLine]);
	}
	_float4x4 Get_Transform_Float4x4_Inverse(TRANSFORMSTATE eState, PIPELINE ePipeLine) const {
		return m_TransformInverseMatrices[eState][ePipeLine];
	}

	_vector Get_CamPosition_Vector(PIPELINE ePipeLine) const {
		return XMLoadFloat4(&m_vCamPosition[ePipeLine]);
	}

	_float4 Get_CamPosition_Float4(PIPELINE ePipeLine) const {
		return m_vCamPosition[ePipeLine];
	}

public:
	HRESULT Initialize();
	void Tick();

private:
	_float4x4			m_TransformMatrices[D3DTS_END][PIPELINE_END];
	_float4x4			m_TransformInverseMatrices[D3DTS_END][PIPELINE_END];
	_float4				m_vCamPosition[PIPELINE_END];

public:
	static CPipeLine* Create();
	virtual void Free() override;	
};

END