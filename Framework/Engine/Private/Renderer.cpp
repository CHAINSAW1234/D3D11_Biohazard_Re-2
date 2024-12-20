#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "GameInstance.h"

#include "ComputeShader.h"
#include "Light.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
	Set_Shadow_Resolution(m_eShadowResolution);

	if (FAILED(SetUp_RenderTargets()))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (FAILED(SetUp_Matrices()))
		return E_FAIL;

	/* For.Shadow */
	if (FAILED(SetUp_LightDSV()))
		return E_FAIL;

	if (FAILED(SetUp_LightDSV_Point()))
		return E_FAIL;

	if (FAILED(Render_LUT()))
		return E_FAIL;

	m_ShaderOptions[SSAO] = true;
	m_ShaderOptions[SSR] = true;
	m_ShaderOptions[VOLUMETRIC] = false;
	m_ShaderOptions[FXAA] = true;

#ifdef _DEBUG
	if (FAILED(SetUp_Debug()))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pRenderObject)
{
	if (eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].push_back(pRenderObject);

	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CRenderer::Render()
{
	if (m_pGameInstance->Get_KeyState('5')) {
		m_ShaderOptions[VOLUMETRIC] = true;
	}
	if(FAILED(m_pGameInstance->Clear_RenderTarget_All()))
		return E_FAIL;

	if (FAILED(Render_Priority()))
		return E_FAIL;

	if (FAILED(Render_Field_Shadow_Direction()))
		return E_FAIL;

	if (FAILED(Render_Field_Shadow_Point()))
		return E_FAIL;

	if (FAILED(Render_Shadow_Direction()))
		return E_FAIL;
		
	if (FAILED(Render_Shadow_Point()))
		return E_FAIL;

	if (FAILED(Render_Shadow_Spot()))
		return E_FAIL;

	if (FAILED(Render_Field()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Copy_Resource(TEXT("Target_Field_Depth"), TEXT("Target_Depth"))))
		return E_FAIL;

	if (FAILED(Render_NonBlend()))
		return E_FAIL;

	if (FAILED(Render_Blend()))
		return E_FAIL;

	if (FAILED(Render_SSAO()))
		return E_FAIL;

	if (FAILED(Render_Lights()))
		return E_FAIL;

	if (FAILED(Render_Light_Result()))
		return E_FAIL;

	if (FAILED(Render_SSR()))
		return E_FAIL;

	if (FAILED(Render_DOF()))
		return E_FAIL;

	if (FAILED(Render_Volumetric()))
		return E_FAIL;

	if (FAILED(Render_NonLight()))
		return E_FAIL;

	//if (FAILED(Render_Distortion()))
	//	return E_FAIL;

	if (FAILED(Render_FXAA()))
		return E_FAIL;

	if (FAILED(Render_PostProcessing_Result()))
		return E_FAIL;

	if (FAILED(Render_Effect_Bloom()))
		return E_FAIL;

	if (FAILED(Render_Effect()))
		return E_FAIL;


	if (FAILED(Render_UI_Blur()))
		return E_FAIL;
	// Pause시 블러

	if (FAILED(Render_UI()))
		return E_FAIL;

	if (FAILED(Render_Examine()))
		return E_FAIL;

	if (FAILED(Render_Font()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Render_Debug()))
		return E_FAIL;
#endif

	if (m_isRenderFieldShadow) {
		m_isRenderFieldShadow = false;
	}

	return S_OK;
}

void CRenderer::Set_RadialBlur(_float fBlurAmount, _float2 BlurUV)
{
	m_fRadialBlurAmount = fBlurAmount;
	m_vRadialBlurUV = BlurUV;
}

void CRenderer::On_RadialBlur()
{
	m_isRadialBlurActive = true;
}

void CRenderer::Off_RadialBlur()
{
	m_isRadialBlurActive = false;
}

_bool CRenderer::Get_ShaderState(SHADER_STATE eState)
{
	if (eState >= SHADER_STATE_END)
		return false;

	return m_ShaderOptions[eState];
}

void CRenderer::Set_ShaderState(SHADER_STATE eState, _bool isState)
{
	if (eState >= SHADER_STATE_END)
		return;

	m_ShaderOptions[eState] = isState;
}

HRESULT CRenderer::SetUp_RenderTargets()
{
	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	if (FAILED(SetUp_RenderTargets_GameObjects(ViewportDesc)))
		return E_FAIL;
	if (FAILED(SetUp_RenderTargets_SSAO(ViewportDesc)))
		return E_FAIL;
	if (FAILED(SetUp_RenderTargets_LightAcc(ViewportDesc)))
		return E_FAIL;
	if (FAILED(SetUp_RenderTargets_Shadow(ViewportDesc)))
		return E_FAIL;
	if (FAILED(SetUp_RenderTargets_Pre_PostProcessing(ViewportDesc)))
		return E_FAIL;
	if (FAILED(SetUp_RenderTargets_SSR(ViewportDesc)))
		return E_FAIL;
	if (FAILED(SetUp_RenderTargets_DOF(ViewportDesc)))
		return E_FAIL;
	if (FAILED(SetUp_RenderTargets_Volumetric(ViewportDesc)))
		return E_FAIL;
	if (FAILED(SetUp_RenderTargets_FXAA(ViewportDesc)))
		return E_FAIL;
	if (FAILED(SetUp_RenderTargets_UI_Blur(ViewportDesc)))
		return E_FAIL;
	if (FAILED(SetUp_RenderTargets_Distortion(ViewportDesc)))
		return E_FAIL;
	if (FAILED(SetUp_RenderTargets_PostProcessing_Result(ViewportDesc)))
		return E_FAIL;
	if (FAILED(SetUp_RenderTarget_SubResult(ViewportDesc)))
		return E_FAIL;

#pragma region Effect
	if (FAILED(SetUp_RenderTargets_Effect_Bloom(ViewportDesc)))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CRenderer::SetUp_LightDSV()
{
	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = static_cast<_uint>(m_fLightDepthTargetViewWidth);
	TextureDesc.Height = static_cast<_uint>(m_fLightDepthTargetViewHeight);
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
		/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	/* RenderTarget */
	/* ShaderResource */
	/* DepthStencil */

	ID3D11DepthStencilView* pLightDepthDSV = { nullptr };
	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pLightDepthDSV)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

HRESULT CRenderer::SetUp_LightDSV_Point()
{
	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_fLightDepthTargetViewCubeWidth = ViewportDesc.Width;

	// For. Shadow_Points
	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = static_cast<_uint>(ViewportDesc.Width);
	TextureDesc.Height = static_cast<_uint>(ViewportDesc.Width);
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = m_iArraySize * 6;							// 최대 깊이 버퍼 개수 : 광원 개수에 따라 다름 여기서는 4, 큐브 맵의 경우 6의 배수로 지정
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
		/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	/* RenderTarget */
	/* ShaderResource */
	/* DepthStencil */
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc = {};
	DepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	DepthStencilViewDesc.Texture2DArray.ArraySize = 6 * m_iArraySize;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, &DepthStencilViewDesc, &m_pLightDepthDSV_Point)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

HRESULT CRenderer::SetUp_Components()
{
	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pComputeShader = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_LUT.hlsl"), "CS_LUT");
	if (nullptr == m_pComputeShader)
		return E_FAIL;


	m_pRandomTexture = CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/RandomNormal.png"));
	if (nullptr == m_pShader)
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::SetUp_Matrices()
{
	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;
	m_WorldMatrix._41 = 0.f;
	m_WorldMatrix._42 = 0.f;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	return S_OK;
}

HRESULT CRenderer::SetUp_RenderTargets_GameObjects(const D3D11_VIEWPORT& ViewportDesc)
{
	/* For.Target_Diffuse */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	/* For.Target_Normal */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	/* For.Target_Depth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 1.f, 0.f, 1.f))))
		return E_FAIL;
	/* For.Target_Material */			//	금속성, 거칠기, 반사율, 이미시브
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Material"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	/* For.Target_Emissive */			//	이미시브텍스처 저장
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Emissive"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	/* For.Target_Depth_Decal */   //데칼을 위한 깊이 텍스쳐
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth_Decal"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 1.f, 0.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Skybox"), TEXT("Target_Diffuse"))))
		return E_FAIL;

	/* MRT_GameObjects : 객체들의 특정 정보를 받아오기위한 렌더타겟들이다. */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Material"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Emissive"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Emissive"), TEXT("Target_Emissive"))))
		return E_FAIL;

	/* For.Target_Depth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Field_Depth"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 1.f, 0.f, 1.f))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::SetUp_RenderTargets_LightAcc(const D3D11_VIEWPORT& ViewportDesc)
{
	/* For.Target_Shade */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_PBR"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* For.Target_Shade */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LUT"), static_cast<_uint>(256), static_cast<_uint>(256), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f), false)))
		return E_FAIL;
	
	/* MRT_LightAcc : 빛들의 연산결과 정보를 받아오기위한 렌더타겟들이다. */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_PBR"), TEXT("Target_PBR"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LUT"), TEXT("Target_LUT"))))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::SetUp_RenderTargets_SSAO(const D3D11_VIEWPORT& ViewportDesc)
{
	/* For.Target_SSAO */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_SSAO"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	/* For.Target_SSAO_BlurX */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_SSAO_BlurX"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	/* For.Target_SSAO_Blur_Fin */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_SSAO_Blur_Fin"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* MRT_SSAO : SSAO적용시 블러를 먹이기 위해 생성. */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_SSAO"), TEXT("Target_SSAO"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_SSAO_BlurX"), TEXT("Target_SSAO_BlurX"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_SSAO_Blur_Fin"), TEXT("Target_SSAO_Blur_Fin"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::SetUp_RenderTargets_Shadow(const D3D11_VIEWPORT& ViewportDesc)
{
	/* For.Target_LightDepth_Field_Dir */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth_Field_Dir"), static_cast<_uint>(m_fLightDepthTargetViewWidth), static_cast<_uint>(m_fLightDepthTargetViewHeight), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f), false)))
		return E_FAIL;
	/* For.Target_LightDepth_Field_Point */
	if (FAILED(m_pGameInstance->Add_RenderTarget_Cube(TEXT("Target_LightDepth_Field_Point"), static_cast<_uint>(ViewportDesc.Width), m_iArraySize, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f), false)))
		return E_FAIL;

	/* For.Target_LightDepth_Dir */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth_Dir"), static_cast<_uint>(m_fLightDepthTargetViewWidth), static_cast<_uint>(m_fLightDepthTargetViewHeight), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	/* For.Target_LightDepth_Point */
	if (FAILED(m_pGameInstance->Add_RenderTarget_Cube(TEXT("Target_LightDepth_Point"), static_cast<_uint>(ViewportDesc.Width), m_iArraySize, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	/* For.Target_LightDepth_Spot */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth_Spot"), static_cast<_uint>(m_fLightDepthTargetViewWidth), static_cast<_uint>(m_fLightDepthTargetViewHeight), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* MRT_Shadow_Field_Dir : Directional Light 그림자 연산을 위한 렌더 타겟. */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow_Field_Dir"), TEXT("Target_LightDepth_Field_Dir"))))
		return E_FAIL;
	/* MRT_Shadow_Field_Point : Point Light 그림자 연산을 위한 렌더 타겟. */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow_Field_Point"), TEXT("Target_LightDepth_Field_Point"))))
		return E_FAIL;

	/* MRT_Shadow_Dir : Directional Light 그림자 연산을 위한 렌더 타겟. */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow_Dir"), TEXT("Target_LightDepth_Dir"))))
		return E_FAIL;
	/* MRT_Shadow_Point : Point Light 그림자 연산을 위한 렌더 타겟. */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow_Point"), TEXT("Target_LightDepth_Point"))))
		return E_FAIL;
	/* MRT_Shadow_Spot : Spot Light 그림자 연산을 위한 렌더 타겟. */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow_Spot"), TEXT("Target_LightDepth_Spot"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::SetUp_RenderTargets_Pre_PostProcessing(const D3D11_VIEWPORT& ViewportDesc)
{
	/* For.Target_Pre_Post_Diffuse */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Pre_Post_Diffuse"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* MRT_Pre_PostProcessing : 그림자, 빛연산을 통해 PreProcess의 최종 연산 */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Pre_PostProcessing"), TEXT("Target_Pre_Post_Diffuse"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::SetUp_RenderTargets_UI_Blur(const D3D11_VIEWPORT& ViewportDesc)
{
	/* For.Target_Pre_Post_Diffuse */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_UI_Blur"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_UI_Blur_X"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_UI_Blur_Fin"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* MRT_Pre_PostProcessing : 그림자, 빛연산을 통해 PreProcess의 최종 연산 */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_UI_Blur"), TEXT("Target_UI_Blur"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_UI_Blur_X"), TEXT("Target_UI_Blur_X"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_UI_Blur_Fin"), TEXT("Target_UI_Blur_Fin"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::SetUp_RenderTargets_Distortion(const D3D11_VIEWPORT& ViewportDesc)
{
	/* For.Target_Distortion */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Distortion"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.5f, 0.5f, 0.5f, 0.f))))
		return E_FAIL;

	/* MRT_Distortion */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Distortion"), TEXT("Target_Distortion"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::SetUp_RenderTargets_Effect_Bloom(const D3D11_VIEWPORT& ViewportDesc)
{
	/* For.Target_Emissive */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_Blur_X"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom_Blur_Y"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* MRT_Emissive */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom"), TEXT("Target_Bloom"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_Blur_X"), TEXT("Target_Bloom_Blur_X"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Bloom_Blur_Y"), TEXT("Target_Bloom_Blur_Y"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Bind_DepthTarget(CShader* pShader)
{
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Decal_Deferred()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_CamViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_CamProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(static_cast<_uint>(SHADER_PASS_DEFERRED::PASS_SSD))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::SetUp_RenderTargets_PostProcessing_Result(const D3D11_VIEWPORT& ViewportDesc)
{
	/* For.Target_PostProcessing_Shade */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_PostProcessing_Result"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* MRT_PostProcessing_Result */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_PostProcessing_Result"), TEXT("Target_PostProcessing_Result"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::SetUp_RenderTargets_SSR(const D3D11_VIEWPORT& ViewportDesc)
{
	/* For.Target_PostProcessing_Shade */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_SSR"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* MRT_SSR : SSR 적용을 위한 렌더 타겟 */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_SSR"), TEXT("Target_SSR"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::SetUp_RenderTargets_DOF(const D3D11_VIEWPORT& ViewportDesc)
{
	/* For.Target_DOF */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_DOF"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	/* For.Target_DOF_BlurX */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_DOF_BlurX"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	/* For.Target_DOF_Blur_Fin */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_DOF_Blur_Fin"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	/* MRT_DOF : DOF 적용을 위한 렌더 타겟 */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DOF"), TEXT("Target_DOF"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DOF_BlurX"), TEXT("Target_DOF_BlurX"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_DOF_Blur_Fin"), TEXT("Target_DOF_Blur_Fin"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::SetUp_RenderTargets_Volumetric(const D3D11_VIEWPORT& ViewportDesc)
{
	/* For.Target_DOF_Blur_Fin */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Volumetric"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	/* For.Target_Volumetric_Blur_X */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Volumetric_Blur_X"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	/* For.Target_Volumetric_Blur_Fin */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Volumetric_Blur_Fin"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* MRT_DOF : DOF 적용을 위한 렌더 타겟 */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Volumetric"), TEXT("Target_Volumetric"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Volumetric_Blur_X"), TEXT("Target_Volumetric_Blur_X"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Volumetric_Blur_Fin"), TEXT("Target_Volumetric_Blur_Fin"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::SetUp_RenderTargets_FXAA(const D3D11_VIEWPORT& ViewportDesc)
{
	/* For.Target_DOF_Blur_Fin */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_FXAA"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f,0.f,0.f,0.f))))
		return E_FAIL;
	
	/* MRT_FXAA : 안티 앨리어싱 적용을 위한 렌더 타겟 */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_FXAA"), TEXT("Target_FXAA"))))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::SetUp_RenderTarget_SubResult(const D3D11_VIEWPORT& ViewportDesc)
{
	/* For.Target_DOF_Blur_Fin */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_SubResult"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	/* MRT_FXAA : 안티 앨리어싱 적용을 위한 렌더 타겟 */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_SubResult"), TEXT("Target_SubResult"))))
		return E_FAIL;

	return S_OK;
}

#ifdef _DEBUG

HRESULT CRenderer::SetUp_Debug()
{
	//return S_OK;

	_float fSize = 200.f;

	//_float WinSizeX = 1600.f + fSize / 2.f;
	//_float WinSizeY = 900.f;

	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_Diffuse"), 
		-fSize / 2 + fSize, -fSize / 2 + fSize, fSize, fSize)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_Normal"), 
		-fSize / 2 + fSize, -fSize / 2 + fSize * 2, fSize, fSize)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_Depth"), 
		-fSize / 2 + fSize, -fSize / 2 + fSize * 3, fSize, fSize)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_Material"), 
		-fSize / 2 + fSize, -fSize / 2 + fSize * 4, fSize, fSize)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_Emissive"), 
		-fSize / 2 + fSize, -fSize / 2 + fSize * 5, fSize, fSize)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_PBR"), 
		-fSize / 2 + fSize * 2, -fSize / 2 + fSize, fSize, fSize)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_SSAO_Blur_Fin"), 
		-fSize / 2 + fSize * 2, -fSize / 2 + fSize * 2, fSize, fSize)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_LUT"), 
		-fSize / 2 + fSize * 2, -fSize / 2 + fSize * 3, fSize, fSize)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_LightDepth_Field_Dir"), 
		-fSize / 2 + fSize * 3, -fSize / 2 + fSize, fSize, fSize)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_LightDepth_Field_Point"),
		-fSize / 2 + fSize * 3, -fSize / 2 + fSize * 2, fSize, fSize)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_LightDepth_Dir"), 
		-fSize / 2 + fSize * 4, -fSize / 2 + fSize, fSize, fSize)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_LightDepth_Point"), 
		-fSize / 2 + fSize * 4, -fSize / 2 + fSize * 2, fSize, fSize)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_LightDepth_Spot"), 
		-fSize / 2 + fSize * 4, -fSize / 2 + fSize * 3, fSize, fSize)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_DOF"), 
		-fSize / 2 + fSize * 5, -fSize / 2 + fSize, fSize, fSize)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_DOF_Blur_Fin"), 
		-fSize / 2 + fSize * 5, -fSize / 2 + fSize * 2, fSize, fSize)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_SSR"), 
		-fSize / 2 + fSize * 6, -fSize / 2 + fSize, fSize, fSize)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_Volumetric"),
		-fSize / 2 + fSize * 6, -fSize / 2 + fSize * 2, fSize, fSize)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_FXAA"),
		-fSize / 2 + fSize * 6, -fSize / 2 + fSize * 3, fSize, fSize)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_Bloom"), 
		-fSize / 2 + fSize * 7, -fSize / 2 + fSize, fSize, fSize)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_RTVDebug(TEXT("Target_Bloom_Blur_Y"), 
		-fSize / 2 + fSize * 7, -fSize / 2 + fSize * 2, fSize, fSize)))
		return E_FAIL;

	return S_OK;
}

#endif

#ifdef _DEBUG

HRESULT CRenderer::Add_DebugComponents(CComponent* pRenderComponent)
{
	m_DebugComponents.emplace_back(pRenderComponent);

	Safe_AddRef(pRenderComponent);

	return S_OK;
}

void CRenderer::On_Off_DebugRender()
{
	m_isRenderDebug = !m_isRenderDebug;
}

#endif

void CRenderer::Set_Shadow_Resolution(SHADOW_RESOLUTION eResolution)
{
	if (SHADOW_RESOLUTION::RES_END <= eResolution)
		return;

	m_eShadowResolution = eResolution;

	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	_float		fViewPortWidth = { ViewportDesc.Width };
	_float		fViewPortHeight = { ViewportDesc.Height };

	_uint		iScale = { static_cast<_uint>(powf(2.f, static_cast<_float>(m_eShadowResolution))) };

	m_fLightDepthTargetViewWidth = fViewPortWidth * static_cast<_float>(iScale);
	m_fLightDepthTargetViewHeight = fViewPortHeight * static_cast<_float>(iScale);
}

void CRenderer::Copy_Depth(ID3D11DepthStencilView* pDestStencilView, ID3D11DepthStencilView* pSrcStencilView)
{
	ID3D11Resource* pSrcResource;
	pSrcStencilView->GetResource(&pSrcResource);

	D3D11_TEXTURE2D_DESC SrcDesc;
	ID3D11Texture2D* SrcTexture = { nullptr };
	pSrcResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&SrcTexture);
	SrcTexture->GetDesc(&SrcDesc);

	SrcDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	if (SrcDesc.ArraySize != 1) {
		SrcDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
	}

	ID3D11Texture2D* DestTexture = nullptr;
	if(FAILED(m_pDevice->CreateTexture2D(&SrcDesc, nullptr, &DestTexture)))
		return ;
	m_pContext->CopyResource(DestTexture, SrcTexture);

	//Safe_Release(pDestStencilView);

	ID3D11DepthStencilView* destDSV = nullptr;
	if (SrcDesc.MiscFlags == D3D11_RESOURCE_MISC_TEXTURECUBE) {
		D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc = {};
		DepthStencilViewDesc.Format = SrcDesc.Format;
		DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		DepthStencilViewDesc.Texture2DArray.ArraySize = SrcDesc.ArraySize;
		m_pDevice->CreateDepthStencilView(DestTexture, &DepthStencilViewDesc, &pDestStencilView);
	}
	else {

		m_pDevice->CreateDepthStencilView(DestTexture, nullptr, &pDestStencilView);
	}
	

	//Safe_Release(SrcDesc);
	Safe_Release(SrcTexture);
	Safe_Release(DestTexture);


}

HRESULT CRenderer::Render_LUT()
{
	if (FAILED(m_pGameInstance->Bind_OutputShaderResource(m_pComputeShader, TEXT("Target_LUT"), "LUT")))
		return E_FAIL;

	if (FAILED(m_pComputeShader->Render(0, 8, 8, 1)))
		return E_FAIL;
	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Skybox"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

	}

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;
	

	//if (m_ShaderOptions[FXAA]) {
	//	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_FXAA"))))
	//		return E_FAIL;
	//}

	for (auto& pRenderObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_PRIORITY].clear();

	//if (m_ShaderOptions[FXAA]) {
	//	if (FAILED(m_pGameInstance->End_MRT()))
	//		return E_FAIL;
	//}

	return S_OK;
}

HRESULT CRenderer::Render_Field()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_FIELD])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_FIELD].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
		return E_FAIL;

	if (FAILED(Render_Decal()))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_NONBLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_NONBLEND].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}

	m_RenderObjects[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
		return E_FAIL;

	if (FAILED(Render_Decal()))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_BLEND])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_Blend();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_BLEND].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	//m_RenderObjects[RENDER_UI].sort([](CGameObject* pFirst, CGameObject* pSecond) {
	//	return pFirst->GetPosition().z > pSecond->GetPosition().z;
	//	});

	for (auto& pRenderObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Font()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_FONT])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_FONT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Examine()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_EXAMINE])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_EXAMINE].clear();

	return S_OK;
}

HRESULT CRenderer::Render_SSAO()
{
	if (!m_ShaderOptions[SSAO])
		return S_OK;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_CamViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_CamProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	if (FAILED(m_pRandomTexture->Bind_ShaderResource(m_pShader, "g_RandomNormalTexture")))
		return E_FAIL;

#pragma region SSAO
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_SSAO"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(static_cast<_uint>(SHADER_PASS_DEFERRED::PASS_SSAO))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;
#pragma endregion

#pragma region BlurX
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_SSAO"), "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_SSAO_BlurX"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(static_cast<_uint>(SHADER_PASS_DEFERRED::PASS_BLURX))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;
#pragma endregion

#pragma region BlurY
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_SSAO_Blur_Fin"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_SSAO_BlurX"), "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(static_cast<_uint>(SHADER_PASS_DEFERRED::PASS_BLURY))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CRenderer::Render_Field_Shadow_Direction()
{
	if (!m_isRenderFieldShadow) {
		for (auto& pRenderObject : m_RenderObjects[RENDER_FIELD_SHADOW_DIR])
		{
			Safe_Release(pRenderObject);
		}
		m_RenderObjects[RENDER_FIELD_SHADOW_DIR].clear();
		return S_OK;
	}

	if(FAILED(m_pGameInstance->Clear_RenderTarget(TEXT("Target_LightDepth_Field_Dir"))))
		return E_FAIL;

	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);
	_float		fOriginViewPortWidth = { ViewportDesc.Width };
	_float		fOriginViewPortHeight = { ViewportDesc.Height };

	Set_ViewPort_Size(m_fLightDepthTargetViewWidth, m_fLightDepthTargetViewHeight);

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow_Field_Dir"), m_pLightDepthDSV)))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_FIELD_SHADOW_DIR])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_LightDepth_Dir();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_FIELD_SHADOW_DIR].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	Set_ViewPort_Size(fOriginViewPortWidth, fOriginViewPortHeight);

	return S_OK;
}

HRESULT CRenderer::Render_Field_Shadow_Point()
{
	if (!m_isRenderFieldShadow) {
		for (auto& pRenderObject : m_RenderObjects[RENDER_FIELD_SHADOW_POINT])
		{
			Safe_Release(pRenderObject);
		}
		m_RenderObjects[RENDER_FIELD_SHADOW_POINT].clear();
		return S_OK;
	}

	if (FAILED(m_pGameInstance->Clear_RenderTarget(TEXT("Target_LightDepth_Field_Point"))))
		return E_FAIL;

	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);
	_float		fOriginViewPortWidth = { ViewportDesc.Width };
	_float		fOriginViewPortHeight = { ViewportDesc.Height };

	Set_ViewPort_Size(m_fLightDepthTargetViewCubeWidth, m_fLightDepthTargetViewCubeWidth, 6 * m_iArraySize);

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow_Field_Point"), m_pLightDepthDSV_Point)))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_FIELD_SHADOW_POINT])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_LightDepth_Point();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_FIELD_SHADOW_POINT].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	Set_ViewPort_Size(fOriginViewPortWidth, fOriginViewPortHeight);

	return S_OK;
}

HRESULT CRenderer::Render_Shadow_Direction()
{
	//if (FAILED(m_pGameInstance->Copy_Resource(TEXT("Target_LightDepth_Dir"), TEXT("Target_LightDepth_Field_Dir"))))
	//	return E_FAIL;

	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);
	_float		fOriginViewPortWidth = { ViewportDesc.Width };
	_float		fOriginViewPortHeight = { ViewportDesc.Height };

	Set_ViewPort_Size(m_fLightDepthTargetViewWidth, m_fLightDepthTargetViewHeight);

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow_Dir"), m_pLightDepthDSV)))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_SHADOW_DIR])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_LightDepth_Dir();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_SHADOW_DIR].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	Set_ViewPort_Size(fOriginViewPortWidth, fOriginViewPortHeight);

	return S_OK;
}

HRESULT CRenderer::Render_Shadow_Point()
{
	//if (FAILED(m_pGameInstance->Copy_Resource(TEXT("Target_LightDepth_Point"), TEXT("Target_LightDepth_Field_Point"))))
	//	return E_FAIL;

	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);
	_float		fOriginViewPortWidth = { ViewportDesc.Width };
	_float		fOriginViewPortHeight = { ViewportDesc.Height };

	Set_ViewPort_Size(m_fLightDepthTargetViewCubeWidth, m_fLightDepthTargetViewCubeWidth, 6 * m_iArraySize);

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow_Point"), m_pLightDepthDSV_Point)))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_SHADOW_POINT])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_LightDepth_Point();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_SHADOW_POINT].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	Set_ViewPort_Size(fOriginViewPortWidth, fOriginViewPortHeight);

	return S_OK;
}

HRESULT CRenderer::Render_Shadow_Spot()
{
	_uint				iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);
	_float		fOriginViewPortWidth = { ViewportDesc.Width };
	_float		fOriginViewPortHeight = { ViewportDesc.Height };

	Set_ViewPort_Size(m_fLightDepthTargetViewWidth, m_fLightDepthTargetViewHeight);

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow_Spot"), m_pLightDepthDSV)))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_SHADOW_SPOT])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_LightDepth_Spot();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_SHADOW_SPOT].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	Set_ViewPort_Size(fOriginViewPortWidth, fOriginViewPortHeight);

	return S_OK;
}

HRESULT CRenderer::Render_UI_Blur()
{
	if (!m_pGameInstance->IsPaused())
		return S_OK;

	if (FAILED(m_pGameInstance->Copy_BackBuffer(TEXT("Target_UI_Blur"))))
		return E_FAIL;

#pragma region BlurX
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_UI_Blur_X"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_UI_Blur"), "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(static_cast<_uint>(SHADER_PASS_DEFERRED::PASS_BLURX))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;
#pragma endregion

#pragma region BlurY
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_UI_Blur_Fin"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_UI_Blur_X"), "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(static_cast<_uint>(SHADER_PASS_DEFERRED::PASS_BLURY))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;
#pragma endregion

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_UI_Blur_Fin"), "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(static_cast<_uint>(SHADER_PASS_DEFERRED::PASS_DEBUG))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Distortion()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Distortion"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_DISTORTION])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render_Distortion();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_DISTORTION].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	m_pGameInstance->Bind_PrevCubeMapTexture(m_pShader, "g_PrevEnvironmentTexture");
	m_pGameInstance->Bind_CurCubeMapTexture(m_pShader, "g_CurEnvironmentTexture");

	m_pGameInstance->Bind_PrevIrradianceTexture(m_pShader, "g_PrevIrradianceTexture");
	m_pGameInstance->Bind_CurIrradianceTexture(m_pShader, "g_CurIrradianceTexture");

	if (FAILED(m_pShader->Bind_RawValue("g_PBRLerpTime", m_pGameInstance->Get_PBRLerpTime(), sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_LUT"), "g_SpecularLUTTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Diffuse"), "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Material"), "g_MaterialTexture")))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	/* Shade */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_PBR"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Light_Result()
{
	/* 백버퍼에다가 디퍼드 방식으로 연산된 최종 결과물을 찍어준다. */
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_PBR"), "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_isSSAO", &m_ShaderOptions[SSAO], sizeof(_bool))))
		return E_FAIL;

	if (m_ShaderOptions[SSAO]) {
		if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_SSAO_Blur_Fin"), "g_SSAOTexture")))
			return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_LightDepth_Dir"), "g_DirLightDepthTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_LightDepth_Field_Dir"), "g_DirLightFieldDepthTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_LightDepth_Point"), "g_PointLightDepthTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_LightDepth_Field_Point"), "g_PointLightFieldDepthTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_LightDepth_Spot"), "g_SpotLightDepthTexture")))
		return E_FAIL;

	// 1. DirectionLight존재 여부 체크
	if (m_pGameInstance->Get_ShadowLight(CPipeLine::DIRECTION) != nullptr) {
		_bool isShadowDirLight = true;
		if (FAILED(m_pShader->Bind_RawValue("g_isShadowDirLight", &isShadowDirLight, sizeof(_bool))))
			return E_FAIL;

		const CLight* pLight = m_pGameInstance->Get_ShadowLight(CPipeLine::DIRECTION);
		const LIGHT_DESC* pDesc = pLight->Get_LightDesc(0);
		
		if (FAILED(m_pShader->Bind_Matrix("g_DirLightViewMatrix", &pDesc->ViewMatrix[0])))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_Matrix("g_DirLightProjMatrix", &pDesc->ProjMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_vDirLightDirection", &pDesc->vDirection, sizeof(_float4))))
			return E_FAIL;
	}
	else {
		_bool isShadowDirLight = false;
		if (FAILED(m_pShader->Bind_RawValue("g_isShadowDirLight", &isShadowDirLight, sizeof(_bool))))
			return E_FAIL;
	}

	_float4* pLightPositions = new _float4[m_iArraySize];
	_float4* pLightRanges = new _float4[m_iArraySize];

	_int iNumShadowLight = 0;

	list<LIGHT_DESC*> LightDesc = m_pGameInstance->Get_ShadowPointLightDesc_List();
	for (auto& pDesc : LightDesc) {
		pLightPositions[iNumShadowLight] = pDesc->vPosition;
		pLightRanges[iNumShadowLight] = _float4(pDesc->fRange, 0, 0, 0);
		++iNumShadowLight;
	}

	if (FAILED(m_pShader->Bind_RawValue("g_iNumShadowPointLight", &iNumShadowLight, sizeof(_int))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_vShadowPointLightPos", pLightPositions, m_iArraySize * sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_fShadowPointLightRange", pLightRanges, m_iArraySize * sizeof(_float4))))
		return E_FAIL;

	// 3. Spot
	if (m_pGameInstance->Get_ShadowLight(CPipeLine::SPOT) != nullptr) {
		_bool isShadowSpotLight = true;
		if (FAILED(m_pShader->Bind_RawValue("g_isShadowSpotLight", &isShadowSpotLight, sizeof(_bool))))
			return E_FAIL;

		const CLight* pLight = m_pGameInstance->Get_ShadowLight(CPipeLine::SPOT);
		const LIGHT_DESC* pDesc = pLight->Get_LightDesc(0);

		if (FAILED(m_pShader->Bind_Matrix("g_SpotLightViewMatrix", &pDesc->ViewMatrix[0])))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_Matrix("g_SpotLightProjMatrix", &pDesc->ProjMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_vSpotLightPosition", &pDesc->vPosition, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_vSpotLightDirection", &pDesc->vDirection, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_fSpotLightCutOff", &pDesc->fCutOff, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_fSpotLightOutCutOff", &pDesc->fOutCutOff, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_fSpotLightRange", &pDesc->fRange, sizeof(_float))))
			return E_FAIL;
	}
	else {
		_bool isShadowSpotLight = false;
		if (FAILED(m_pShader->Bind_RawValue("g_isShadowSpotLight", &isShadowSpotLight, sizeof(_bool))))
			return E_FAIL;
	}

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	/* Shade */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Pre_PostProcessing"))))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(static_cast<_uint>(SHADER_PASS_DEFERRED::PASS_LIGHT_RESULT))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	Safe_Delete_Array(pLightPositions);
	Safe_Delete_Array(pLightRanges);

	if (FAILED(m_pGameInstance->Copy_Resource(TEXT("Target_SubResult"), TEXT("Target_Pre_Post_Diffuse"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_SSR()
{
	if (!m_ShaderOptions[SSR])
		return S_OK;

	/* 백버퍼에다가 디퍼드 방식으로 연산된 최종 결과물을 찍어준다. */
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_CamViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_CamProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_SubResult"), "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Normal"), "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Material"), "g_MaterialTexture")))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	/* Shade */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_SSR"))))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(static_cast<_uint>(SHADER_PASS_DEFERRED::PASS_SSR))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Copy_Resource(TEXT("Target_SubResult"), TEXT("Target_SSR"))))
		return S_OK;

	return S_OK;
}

HRESULT CRenderer::Render_DOF()
{
	if (!m_ShaderOptions[DOF])
		return S_OK;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	// 1. 블러를 먹일 대상을 판별 
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DOF"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(static_cast<_uint>(SHADER_PASS_DEFERRED::PASS_DOF))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_SubResult"), "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_DOF"), "g_DOFTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DOF_BlurX"))))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(static_cast<_uint>(SHADER_PASS_DEFERRED::PASS_DOF_BLURX))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	// 3. Y블러
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_DOF_BlurX"), "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_DOF_Blur_Fin"))))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(static_cast<_uint>(SHADER_PASS_DEFERRED::PASS_DOF_BLURY))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Copy_Resource(TEXT("Target_SubResult"), TEXT("Target_DOF_Blur_Fin"))))
		return S_OK;

	return S_OK;
}

HRESULT CRenderer::Render_Volumetric()
{
	if (!m_ShaderOptions[VOLUMETRIC])
		return S_OK;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", &m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", &m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_CamViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_CamProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_LightDepth_Field_Dir"), "g_DirLightFieldDepthTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_LightDepth_Dir"), "g_DirLightDepthTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_LightDepth_Spot"), "g_SpotLightDepthTexture")))
		return E_FAIL;

	// 1. DirectionLight존재 여부 체크
	if (m_pGameInstance->Get_ShadowLight(CPipeLine::DIRECTION) != nullptr) {
		_bool isShadowDirLight = true;
		if (FAILED(m_pShader->Bind_RawValue("g_isShadowDirLight", &isShadowDirLight, sizeof(_bool))))
			return E_FAIL;

		const CLight* pLight = m_pGameInstance->Get_ShadowLight(CPipeLine::DIRECTION);
		const LIGHT_DESC* pDesc = pLight->Get_LightDesc(0);

		if (FAILED(m_pShader->Bind_Matrix("g_DirLightViewMatrix", &pDesc->ViewMatrix[0])))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_Matrix("g_DirLightProjMatrix", &pDesc->ProjMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_vDirLightDirection", &pDesc->vDirection, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_vDirLightDiffuse", &pDesc->vDiffuse, sizeof(_float4))))
			return E_FAIL;
	}
	else {
		_bool isShadowDirLight = false;
		if (FAILED(m_pShader->Bind_RawValue("g_isShadowDirLight", &isShadowDirLight, sizeof(_bool))))
			return E_FAIL;
	}
	
	// 2. SpotLight
	if (m_pGameInstance->Get_ShadowLight(CPipeLine::SPOT) != nullptr) {
		_bool isShadowSpotLight = true;
		if (FAILED(m_pShader->Bind_RawValue("g_isShadowSpotLight", &isShadowSpotLight, sizeof(_bool))))
			return E_FAIL;

		const CLight* pLight = m_pGameInstance->Get_ShadowLight(CPipeLine::SPOT);
		const LIGHT_DESC* pDesc = pLight->Get_LightDesc(0);

		if (FAILED(m_pShader->Bind_Matrix("g_SpotLightViewMatrix", &pDesc->ViewMatrix[0])))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_Matrix("g_SpotLightProjMatrix", &pDesc->ProjMatrix)))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_vSpotLightPosition", &pDesc->vPosition, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_vSpotLightDirection", &pDesc->vDirection, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_fSpotLightCutOff", &pDesc->fCutOff, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_fSpotLightOutCutOff", &pDesc->fOutCutOff, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_fSpotLightRange", &pDesc->fRange, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_vSpotLightDiffuse", &pDesc->vDiffuse, sizeof(_float4))))
			return E_FAIL;
	}
	else {
		_bool isShadowSpotLight = false;
		if (FAILED(m_pShader->Bind_RawValue("g_isShadowSpotLight", &isShadowSpotLight, sizeof(_bool))))
			return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Volumetric"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(static_cast<_uint>(SHADER_PASS_DEFERRED::PASS_VOLUMETRIC))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Volumetric_Blur_X"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Volumetric"), "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(static_cast<_uint>(SHADER_PASS_DEFERRED::PASS_BLURX))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Volumetric_Blur_Fin"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Volumetric_Blur_X"), "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(static_cast<_uint>(SHADER_PASS_DEFERRED::PASS_BLURY))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_SubResult"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Volumetric_Blur_Fin"), "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(static_cast<_uint>(SHADER_PASS_DEFERRED::PASS_MIX))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_FXAA()
{
	if (!m_ShaderOptions[FXAA])
		return S_OK;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_SubResult"), "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_FXAA"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(static_cast<_uint>(SHADER_PASS_DEFERRED::PASS_COPY_DISCARD))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_SubResult"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_FXAA"), "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(static_cast<_uint>(SHADER_PASS_DEFERRED::PASS_FXAA))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_PostProcessing_Result()
{
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_SubResult"), "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Distortion"), "g_DistortionTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(static_cast<_uint>(SHADER_PASS_DEFERRED::PASS_POST_PROCESSING_RESULT))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Decal()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_DECAL])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_DECAL].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Effect_Bloom()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Emissive"))))
		return E_FAIL;

	for (auto& pRenderObject : m_RenderObjects[RENDER_EFFECT_BLOOM])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_EFFECT_BLOOM].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

#pragma region BlurX
	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Emissive"), "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_Blur_X"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(static_cast<_uint>(SHADER_PASS_DEFERRED::PASS_BLURX_EFFECT))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;
#pragma endregion

#pragma region BlurY
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Bloom_Blur_Y"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Bloom_Blur_X"), "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(static_cast<_uint>(SHADER_PASS_DEFERRED::PASS_BLURY_EFFECT))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;
#pragma endregion

#pragma region Result
	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_SubResult"), "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Bloom_Blur_Y"), "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShader, TEXT("Target_Emissive"), "g_BlackTexture")))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(static_cast<_uint>(SHADER_PASS_DEFERRED::PASS_BLOOM))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;
#pragma endregion

	return S_OK;
}

HRESULT CRenderer::Render_Effect()
{
	for (auto& pRenderObject : m_RenderObjects[RENDER_EFFECT])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();
		Safe_Release(pRenderObject);
	}
	m_RenderObjects[RENDER_EFFECT].clear();

	return S_OK;
}

void CRenderer::Set_ViewPort_Size(_float fWidth, _float fHeight, _uint iArraySize)
{
	D3D11_VIEWPORT* ViewPortDesc = new D3D11_VIEWPORT[iArraySize];
	for (_uint i = 0; i < iArraySize; ++i) {
		ZeroMemory(&ViewPortDesc[i], sizeof(D3D11_VIEWPORT));
		ViewPortDesc[i].TopLeftX = 0;
		ViewPortDesc[i].TopLeftY = 0;
		ViewPortDesc[i].Width = fWidth;
		ViewPortDesc[i].Height = fHeight;
		ViewPortDesc[i].MinDepth = 0.f;
		ViewPortDesc[i].MaxDepth = 1.f;
	}

	m_pContext->RSSetViewports(iArraySize, ViewPortDesc);

	Safe_Delete_Array(ViewPortDesc);
}

#ifdef _DEBUG

HRESULT CRenderer::Render_Debug()
{

	for (auto& pDebugCom : m_DebugComponents)
	{
		if (true == m_isRenderDebug) {
			if (nullptr != pDebugCom)
				pDebugCom->Render();
		}

		Safe_Release(pDebugCom);
	}
	m_DebugComponents.clear();

	if (false == m_isRenderDebug)
		return S_OK;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_SSAO"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_SSAO_Blur_Fin"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_PBR"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	//if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_LUT"), m_pShader, m_pVIBuffer)))
	//	return E_FAIL;
	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_SSR"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_Pre_PostProcessing"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_Distortion"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_Shadow_Field_Dir"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_Shadow_Field_Point"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_Shadow_Dir"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_Shadow_Point"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_Shadow_Spot"), m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_DOF"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_DOF_BlurX"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_DOF_Blur_Fin"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_Volumetric"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_FXAA"), m_pShader, m_pVIBuffer)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_Bloom"), m_pShader, m_pVIBuffer)))
	return E_FAIL;
	if (FAILED(m_pGameInstance->Draw_RTVDebug(TEXT("MRT_Bloom_Blur_Y"), m_pShader, m_pVIBuffer)))
		return E_FAIL;

	_float fSize = 200.f;

	m_pGameInstance->Render_Font(TEXT("Font_CG10"), TEXT("Target_Diffuse"),
		_float2(0, 0), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);
	m_pGameInstance->Render_Font(TEXT("Font_CG10"), TEXT("Target_Normal"),
		_float2(0, fSize), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);

	m_pGameInstance->Render_Font(TEXT("Font_CG10"), TEXT("Target_Depth"),
		_float2(0, fSize * 2), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);

	m_pGameInstance->Render_Font(TEXT("Font_CG10"), TEXT("Target_Material"),
		_float2(0, fSize * 3), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);

	m_pGameInstance->Render_Font(TEXT("Font_CG10"), TEXT("Target_Emissive"),
		_float2(0, fSize * 4), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);

	m_pGameInstance->Render_Font(TEXT("Font_CG10"), TEXT("Target_PBR"),
		_float2(fSize, 0), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);

	m_pGameInstance->Render_Font(TEXT("Font_CG10"), TEXT("Target_SSAO_Blur_Fin"),
		_float2(fSize, fSize), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);

	m_pGameInstance->Render_Font(TEXT("Font_CG10"), TEXT("Target_LUT"),
		_float2(fSize, fSize * 2), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);

	m_pGameInstance->Render_Font(TEXT("Font_CG10"), TEXT("Target_LightDepth_Field_Dir"),
		_float2(fSize * 2, 0), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);

	m_pGameInstance->Render_Font(TEXT("Font_CG10"), TEXT("Target_LightDepth_Field_Point"),
		_float2(fSize * 2, fSize), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);

	m_pGameInstance->Render_Font(TEXT("Font_CG10"), TEXT("Target_LightDepth_Dir"),
		_float2(fSize * 3, 0), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);

	m_pGameInstance->Render_Font(TEXT("Font_CG10"), TEXT("Target_LightDepth_Point"),
		_float2(fSize * 3, fSize), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);

	m_pGameInstance->Render_Font(TEXT("Font_CG10"), TEXT("Target_LightDepth_Spot"),
		_float2(fSize * 3, fSize * 2), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);

	m_pGameInstance->Render_Font(TEXT("Font_CG10"), TEXT("Target_DOF"),
		_float2(fSize * 4, 0), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);

	m_pGameInstance->Render_Font(TEXT("Font_CG10"), TEXT("Target_DOF_Blur_Fin"),
		_float2(fSize * 4, fSize), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);

	m_pGameInstance->Render_Font(TEXT("Font_CG10"), TEXT("Target_SSR"),
		_float2(fSize * 5, 0), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);


	m_pGameInstance->Render_Font(TEXT("Font_CG10"), TEXT("Target_Volumetric"),
		_float2(fSize * 5, fSize), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);


	m_pGameInstance->Render_Font(TEXT("Font_CG10"), TEXT("Target_FXAA"),
		_float2(fSize * 5, fSize * 2), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);

	m_pGameInstance->Render_Font(TEXT("Font_CG10"), TEXT("Target_Bloom"),
		_float2(fSize * 6, 0), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);

	m_pGameInstance->Render_Font(TEXT("Font_CG10"), TEXT("Target_Bloom_Blur_Y"),
		_float2(fSize * 6, fSize), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);




	return S_OK;
}

#endif

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CRenderer"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderer::Free()
{
	__super::Free();

	for (auto& RenderList : m_RenderObjects)
	{
		for (auto& pRenderObject : RenderList)
			Safe_Release(pRenderObject);
		RenderList.clear();
	}

	Safe_Release(m_pShader);
	Safe_Release(m_pComputeShader);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pRandomTexture);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pLightDepthDSV);
	Safe_Release(m_pLightDepthDSV_Point);

#ifdef _DEBUG

	for (auto& pDebugComponent : m_DebugComponents)
	{
		Safe_Release(pDebugComponent);
	}
	m_DebugComponents.clear();

#endif
}
