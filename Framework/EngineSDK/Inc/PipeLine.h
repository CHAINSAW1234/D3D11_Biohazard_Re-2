#pragma once

#include "Base.h"

BEGIN(Engine)
class CLight;
class CTexture;
class CShader;
class CComputeShader;
class CRenderTarget;
class CGameInstance;

class CPipeLine final : public CBase
{
public:
	enum TRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };
	enum SHADOWLIGHT { DIRECTION, POINT, SPOT, SHADOWLIGHT_END };

	enum CASCADE { CASCADE_NEAR, CASCADE_MIDDLE, CASCADE_FAR, CASCADE_END };

	typedef struct tagFrustumDesc
	{
		_float	fFovy = { 0.0f };
		_float	fAspect = { 0.0f };
		_float	fNear = { 0.0f };
		_float	fFar = { 0.0f };
	}FRUSTUM_DESC;

private:
	CPipeLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPipeLine() = default;

public:
	void				Set_Transform(TRANSFORMSTATE eState, _fmatrix TransformMatrix)
	{
		XMStoreFloat4x4(&m_TransformMatrices[eState], TransformMatrix);
	}
	void				Add_ShadowLight(SHADOWLIGHT eShadowLight, CLight* pLight);
	void				Set_CubeMap(CTexture* m_pTexture, _uint iIndex);

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

	_matrix				Get_PrevTransform_Matrix(TRANSFORMSTATE eState) const
	{
		return XMLoadFloat4x4(&m_PrevTransformMatrices[eState]);
	}
	_float4x4			Get_PrevTransform_Float4x4(TRANSFORMSTATE eState) const
	{
		return m_PrevTransformMatrices[eState];
	}
	_matrix				Get_PrevTransform_Matrix_Inverse(TRANSFORMSTATE eState) const
	{
		return XMLoadFloat4x4(&m_PrevTransformInverseMatrices[eState]);
	}
	_float4x4			Get_PrevTransform_Float4x4_Inverse(TRANSFORMSTATE eState) const
	{
		return m_PrevTransformInverseMatrices[eState];
	}
	_vector				Get_PrevCamPosition_Vector() const
	{
		return XMLoadFloat4(&m_vPrevCamPosition);
	}
	_float4				Get_PrevCamPosition_Float4() const
	{
		return m_vPrevCamPosition;
	}

	_uint				Get_NumShadowLight()
	{
		return m_iNumLight;
	}
	CLight*				Get_ShadowLight(SHADOWLIGHT eShadowLight, _uint iLightIndex) {
		switch (eShadowLight) {
		case DIRECTION:
			return m_pDirectionLight;
			break;
		case POINT: {
			if (iLightIndex >= m_iNumLight)
				return nullptr;

			auto iter = m_Lights.begin();
			advance(iter, iLightIndex);
			return *iter;
		}
				  break;
		case SPOT:
			return m_pSpotLight;
			break;
		}

		return nullptr;
	}
	list<LIGHT_DESC*>	Get_ShadowPointLightDesc_List();

	HRESULT				Bind_PrevIrradianceTexture(CShader* pShader, const _char* pConstantName);
	HRESULT				Bind_CurIrradianceTexture(CShader* pShader, const _char* pConstantName);
	HRESULT				Bind_PrevCubeMapTexture(CShader* pShader, const _char* pConstantName);
	HRESULT				Bind_CurCubeMapTexture(CShader* pShader, const _char* pConstantName);

	_float*				Get_PBRLerpTime() { return &m_fLerpTimeDelta; }

public:
	HRESULT				Initialize();
	void				Tick(_float fTimeDelta);
	HRESULT				Render();
	void				Reset();		// 렌더 이후에 그림자 연산에 사용한 빛을 리스트에서 제거

#ifdef _DEBUG
	class CVIBuffer_Rect* m_pVIBuffer = { nullptr };

	void				Render_Debug();
#endif // _DEBUG


private:
	HRESULT				Set_Components();
	HRESULT				Create_Texture();

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };


	_float4x4			m_TransformMatrices[D3DTS_END];
	_float4x4			m_TransformInverseMatrices[D3DTS_END];
	_float4				m_vCamPosition;

	_float4x4			m_PrevTransformMatrices[D3DTS_END];
	_float4x4			m_PrevTransformInverseMatrices[D3DTS_END];
	_float4				m_vPrevCamPosition;

	// 그림자 계산에 필요한 변수
	
	// 점광원용
	_uint				m_iMaxLight = { 2 };
	_uint				m_iNumLight = { 0 };
	list<CLight*>		m_Lights;

	CLight*				m_pDirectionLight = { nullptr };
	CLight*				m_pSpotLight = { nullptr };

	_bool					m_isRender = { false };
	CComputeShader*			m_pShaderCom = { nullptr };

	_float					m_fLerpTimeDelta = { 0.f };
	_uint					m_iPrevCubeMapIndex = { 0 };
	_uint					m_iCurCubeMapIndex = { 0 };
	CTexture*				m_pPrevCubeMapTexture = { nullptr };
	CTexture*				m_pCurCubeMapTexture = { nullptr };

	CRenderTarget*			m_pPrevIrradialTexture = { nullptr };
	CRenderTarget*			m_pHDRTexture = { nullptr };
	CRenderTarget*			m_pCurIrradialTexture = { nullptr };

public:
	static CPipeLine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;	
};

END