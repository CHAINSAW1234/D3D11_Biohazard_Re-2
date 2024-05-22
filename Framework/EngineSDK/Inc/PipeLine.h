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
	enum CASCADE { CASCADE_NEAR, CASCADE_MIDDLE, CASCADE_FAR, CASCADE_END };

	typedef struct tagFrustumDesc
	{
		_float	fFovy = { 0.0f };
		_float	fAspect = { 0.0f };
		_float	fNear = { 0.0f };
		_float	fFar = { 0.0f };
	}FRUSTUM_DESC;

private:
	CPipeLine();
	virtual ~CPipeLine() = default;

public:
	void Set_Transform(TRANSFORMSTATE eState, _fmatrix TransformMatrix, PIPELINE ePipeLine) {
		XMStoreFloat4x4(&m_TransformMatrices[eState][ePipeLine], TransformMatrix);
	}

	void Set_Frustum(FRUSTUM_DESC FrustumDesc, PIPELINE ePipeLine) {
		memcpy(&m_Frustum[ePipeLine], &FrustumDesc, sizeof(FRUSTUM_DESC));
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

	FRUSTUM_DESC Get_Frustum(PIPELINE ePipeLine) const {
		return m_Frustum[ePipeLine];
	}

public:
	HRESULT Initialize();
	void Tick();

private:
	// cascade를 위한 절두체 분리 및 투영행렬 계산
	void Update_CascadeFrustum();
	void Update_CascadeProjMatrices();
private:
	_float4x4			m_TransformMatrices[D3DTS_END][PIPELINE_END];
	_float4x4			m_TransformInverseMatrices[D3DTS_END][PIPELINE_END];
	_float4				m_vCamPosition[PIPELINE_END];
	FRUSTUM_DESC		m_Frustum[PIPELINE_END];


	// CasCade용 변수 -> 안씀
	_float3				m_vOriginalPoints[8];
	_float				m_fCascadeSplitZ[CASCADE_END + 1];
	_float3				m_vCascadePoints[CASCADE_END][8];
	_float4x4			m_CascadeProjectMatrices[CASCADE_END];

public:
	static CPipeLine* Create();
	virtual void Free() override;	
};

END