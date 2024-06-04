#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderer final : public CBase
{
public:
	enum RENDERGROUP {
		RENDER_PRIORITY,
		RENDER_FIELD_SHADOW_DIR, RENDER_FIELD_SHADOW_POINT,
		RENDER_SSAO,
		RENDER_NONBLEND, RENDER_NONLIGHT, RENDER_NON_POSTPROCESSING,
		RENDER_SHADOW_DIR, RENDER_SHADOW_POINT, RENDER_SHADOW_SPOT,
		RENDER_DISTORTION, RENDER_EMISSIVE,
		RENDER_BLEND,
		RENDER_UI,
		RENDER_FONT,
		RENDER_END
	};

	enum SHADOW_RESOLUTION { RES_1X, RES_2X, RES_4X, RES_8X, RES_END };

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderer() = default;

public:
	HRESULT						Initialize();
	HRESULT						Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
	HRESULT						Render();

public:
	void						Set_RadialBlur(_float fBlurAmount, _float2 BlurUV);
	void						On_RadialBlur();
	void						Off_RadialBlur();
	_bool						Get_ShaderState(SHADER_STATE eState);
	void						Set_ShaderState(SHADER_STATE eState, _bool isState);
	void						Set_RenderFieldShadow(_bool isRenderFieldShadow) { m_isRenderFieldShadow = isRenderFieldShadow; }
private:
	HRESULT						SetUp_RenderTargets();
	HRESULT						SetUp_LightDSV();
	HRESULT						SetUp_LightDSV_Point();
	HRESULT						SetUp_Components();
	HRESULT						SetUp_Matrices();

private:		/* For.SetUp_RenderTarget */
	HRESULT						SetUp_RenderTargets_GameObjects(const D3D11_VIEWPORT& ViewportDesc);
	HRESULT						SetUp_RenderTargets_LightAcc(const D3D11_VIEWPORT& ViewportDesc);
	HRESULT						SetUp_RenderTargets_SSAO(const D3D11_VIEWPORT& ViewportDesc);
	HRESULT						SetUp_RenderTargets_Shadow(const D3D11_VIEWPORT& ViewportDesc);
	HRESULT						SetUp_RenderTargets_Pre_PostProcessing(const D3D11_VIEWPORT& ViewportDesc);
	HRESULT						SetUp_RenderTargets_PostProcessing(const D3D11_VIEWPORT& ViewportDesc);
	HRESULT						SetUp_RenderTargets_PostProcessing_Result(const D3D11_VIEWPORT& ViewportDesc);
	HRESULT						SetUp_RenderTargets_MotionBlur(const D3D11_VIEWPORT& ViewportDesc);
	HRESULT						SetUp_RenderTargets_SSR(const D3D11_VIEWPORT& ViewportDesc);
	HRESULT						SetUp_RenderTargets_DOF(const D3D11_VIEWPORT& ViewportDesc);
	HRESULT						SetUp_RenderTargets_FXAA(const D3D11_VIEWPORT& ViewportDesc);
	HRESULT						SetUp_RenderTarget_SubResult(const D3D11_VIEWPORT& ViewportDesc);
	// 잘 모름 
	HRESULT						SetUp_RenderTargets_Distortion(const D3D11_VIEWPORT& ViewportDesc);
	HRESULT						SetUp_RenderTargets_Emissive(const D3D11_VIEWPORT& ViewportDesc);


	HRESULT						SetUp_Test();
	HRESULT						Render_Test();

#ifdef _DEBUG
	HRESULT						SetUp_Debug();
#endif

#ifdef _DEBUG
public:
	HRESULT						Add_DebugComponents(class CComponent* pRenderObject);
	void						On_Off_DebugRender();

#endif

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };
	list<class CGameObject*>	m_RenderObjects[RENDER_END];

private:
	class CVIBuffer_Rect*		m_pVIBuffer = { nullptr };
	class CShader*				m_pShader = { nullptr };
	class CTexture*				m_pRandomTexture = { nullptr };
	_float4x4					m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};

private:
	ID3D11DepthStencilView*		m_pLightDepthDSVs[RES_END] = {};
	SHADOW_RESOLUTION			m_eShadowResolution = { SHADOW_RESOLUTION::RES_4X };
	_float						m_fLightDepthTargetViewWidth = { 0.f };
	_float						m_fLightDepthTargetViewHeight = { 0.f };

private:
	_uint						m_iArraySize = { 2 };		// 동시에 적용하는 최대 점 광원 개수 지정
	ID3D11DepthStencilView*		m_pLightDepthDSV_Point = { nullptr };		// 점 광원용
	_float						m_fLightDepthTargetViewCubeWidth = { 0.f };

private:
	_bool						m_isRenderFieldShadow = { true };
	_bool						m_ShaderOptions[SHADER_STATE_END] = { true };

private:
	_bool						m_isRadialBlurActive = { false };
	_float2						m_vRadialBlurUV = { 0.f, 0.f };
	_float						m_fRadialBlurAmount = { 0.f };

#ifdef _DEBUG
private:
	list<class CComponent*>		m_DebugComponents;

	_bool						m_isRenderDebug = { true };
#endif

public:
	void						Set_Shadow_Resolution(SHADOW_RESOLUTION eResolution);

public:
	HRESULT						Clear();

private:
	HRESULT						Render_Priority();
	HRESULT						Render_NonBlend();
	HRESULT						Render_NonLight();
	HRESULT						Render_Blend();
	HRESULT						Render_Non_PostProcessing();
	HRESULT						Render_UI();
	HRESULT						Render_Font();

private:
	HRESULT						Render_SSAO_Blur();

private:
	HRESULT						Render_Field_Shadow_Direction();
	HRESULT						Render_Field_Shadow_Point();

private:
	HRESULT						Render_Shadow_Direction();
	HRESULT						Render_Shadow_Point();
	HRESULT						Render_Shadow_Spot();

private:
	// 잘 모름
	HRESULT						Render_Distortion();
	HRESULT						Render_Emissive();

private:
	HRESULT						Render_Lights();
	HRESULT						Render_Light_Result();

private:
	HRESULT						Render_MotionBlur();
	HRESULT						Render_SSR();
	HRESULT						Render_DOF();
	HRESULT						Render_FXAA();
	HRESULT						Render_SubResult(const wstring& strRenderTargetTag);	// 후처리 관리용 
private:
	HRESULT						Render_PostProcessing();
	HRESULT						Render_PostProcessing_Result();

private:
	void						Set_ViewPort_Size(_float fWidth, _float fHeight, _uint iArraySize = 1);

#ifdef _DEBUG
private:
	HRESULT						Render_Debug();
#endif

public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END