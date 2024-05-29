#pragma once

#include "Base.h"

BEGIN(Engine)
class CLight;
class CPipeLine final : public CBase
{
public:
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
	void				Set_Transform(TRANSFORMSTATE eState, _fmatrix TransformMatrix) 
	{
		XMStoreFloat4x4(&m_TransformMatrices[eState], TransformMatrix);
	}
	void				Set_ShadowSpotLight(CLight* pLight);
	void				Add_ShadowLight(CLight* pLight);
public:
	_matrix				Get_Transform_Matrix(TRANSFORMSTATE eState) const 
	{
		return XMLoadFloat4x4(&m_TransformMatrices[eState]);
	}
	_float4x4			Get_Transform_Float4x4(TRANSFORMSTATE eState) const 
	{
		return m_TransformMatrices[eState];
	}
	_matrix				Get_Transform_Matrix_Inverse(TRANSFORMSTATE eState) const 
	{
		return XMLoadFloat4x4(&m_TransformInverseMatrices[eState]);
	}
	_float4x4			Get_Transform_Float4x4_Inverse(TRANSFORMSTATE eState) const 
	{
		return m_TransformInverseMatrices[eState];
	}

	_vector				Get_CamPosition_Vector() const 
	{
		return XMLoadFloat4(&m_vCamPosition);
	}

	_float4				Get_CamPosition_Float4() const 
	{
		return m_vCamPosition;
	}

	_uint				Get_NumShadowLight() 
	{
		return m_iNumLight;
	}

	const CLight*		Get_ShadowLight(_uint iIndex) 
	{
		if (iIndex >= m_iNumLight)
			return nullptr;

		auto iter = m_Lights.begin();
		advance(iter, iIndex);
		return *iter;
	}

	CLight*				Get_ShadowSpotLight() 
	{
		return m_pSpotLight;
	}

	list<LIGHT_DESC*>	Get_ShadowLightDesc_List();

public:
	HRESULT				Initialize();
	void				Tick();
	void				Reset();		// 렌더 이후에 그림자 연산에 사용한 빛을 리스트에서 제거

private:
	// cascade를 위한 절두체 분리 및 투영행렬 계산
	void				Update_CascadeFrustum();
	void				Update_CascadeProjMatrices();

private:
	_float4x4			m_TransformMatrices[D3DTS_END];
	_float4x4			m_TransformInverseMatrices[D3DTS_END];
	_float4				m_vCamPosition;
	//FRUSTUM_DESC		m_Frustum;

	// 그림자 계산에 필요한 변수
	// 점광원용
	_uint				m_iMaxLight = { 2 };
	_uint				m_iNumLight = { 0 };
	list<CLight*>		m_Lights;

	// SpotLight
	CLight*				m_pSpotLight = { nullptr };
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