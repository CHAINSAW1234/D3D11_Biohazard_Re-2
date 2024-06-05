#include "GameInstance.h"
#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Object_Manager.h"
#include "Target_Manager.h"
#include "Level_Manager.h"
#include "Sound_Manager.h"
#include "Timer_Manager.h"
#include "Light_Manager.h"
#include "Font_Manager.h"
#include "Extractor.h"
#include "Renderer.h"
#include "Frustum.h"
#include "Physics_Controller.h"
#include "Picking.h"
#include "Layer.h"
#include "Thread_Pool.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()	
{

}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInstance, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc, ppDevice, ppContext);
	if(nullptr == m_pGraphic_Device)
	{
		MSG_BOX(TEXT("Error: CGraphic_Device::Create -> nullptr"));
		return E_FAIL;	
	}

	m_pInput_Device = CInput_Device::Create(EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
	{
		MSG_BOX(TEXT("Error: CInput_Device::Create -> nullptr"));
		return E_FAIL;
	}

	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
	{
		MSG_BOX(TEXT("Error: CPipeLine::Create -> nullptr"));
		return E_FAIL;
	}
	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
	{
		MSG_BOX(TEXT("Error: CLight_Manager::Create -> nullptr"));
		return E_FAIL;
	}

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
	{
		MSG_BOX(TEXT("Error: CTimer_Manager::Create -> nullptr"));
		return E_FAIL;
	}
	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
	{
		MSG_BOX(TEXT("Error: m_pTarget_Manager::Create -> nullptr"));
		return E_FAIL;
	}

	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
	{
		MSG_BOX(TEXT("Error: CRenderer::Create -> nullptr"));

		return E_FAIL;
	}

	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
	{
		MSG_BOX(TEXT("Error: CLevel_Manager::Create -> nullptr"));
		return E_FAIL;
	}
		
	m_pSound_Manager = CSound_Manager::Create();
	if (nullptr == m_pSound_Manager)
	{
		MSG_BOX(TEXT("Error: CSound_Manager::Create -> nullptr"));
		return E_FAIL;
	}

	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
	{
		MSG_BOX(TEXT("Error: m_pObject_Manager::Create -> nullptr"));
		return E_FAIL;
	}

	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
	{
		MSG_BOX(TEXT("Error: CComponent_Manager::Create -> nullptr"));
		return E_FAIL;
	}

	m_pFont_Manager = CFont_Manager::Create();
	if (nullptr == m_pFont_Manager)
	{
		MSG_BOX(TEXT("Error: m_pFont_Manager::Create -> nullptr"));
		return E_FAIL;
	}	

	if (FAILED(Add_Font(*ppDevice, *ppContext, g_strFontTag, TEXT("../../Client/Bin/Resources/Fonts/141ex.spriteFont"))))
		return E_FAIL;

	m_pFrustum = CFrustum::Create();
	if (nullptr == m_pFrustum)
	{
		MSG_BOX(TEXT("Error: m_pFrustum::Create -> nullptr"));
		return E_FAIL;
	}	
	

	m_pExtractor = CExtractor::Create(*ppDevice, *ppContext);
	if (nullptr == m_pExtractor)
	{
		MSG_BOX(TEXT("Error: m_pExtractor::Create -> nullptr"));
		return E_FAIL;
	}
	m_pPhysics_Controller = CPhysics_Controller::Create();
	if (nullptr == m_pPhysics_Controller)
	{
		MSG_BOX(TEXT("Error: m_pPhysics_Controller::Create -> nullptr"));
		return E_FAIL;
	}
	
	m_pPicking = CPicking::Create(*ppDevice, *ppContext, EngineDesc.hWnd, EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);
	if (nullptr == m_pPicking)
	{
		MSG_BOX(TEXT("Error: m_pPicking::Create -> nullptr"));
		return E_FAIL;
	}

	m_pThread_Pool = CThread_Pool::Create(24);
	if (nullptr == m_pThread_Pool)
	{
		MSG_BOX(TEXT("Error: m_pThread_Pool::Create -> nullptr"));
		return E_FAIL;
	}

	//Random Generator
	m_RandomNumber = mt19937_64(m_RandomDevice());

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if(m_pPipeLine)
		m_pPipeLine->Reset();

	if(m_pInput_Device)
		m_pInput_Device->Tick(fTimeDelta);

	if (m_pLevel_Manager)
		m_pLevel_Manager->Open_Level();

	if (m_pObject_Manager)
		m_pObject_Manager->Priority_Tick(fTimeDelta);

	if (m_pLevel_Manager)
		m_pLevel_Manager->Tick(fTimeDelta);

	if(m_pObject_Manager)
		m_pObject_Manager->Tick(fTimeDelta);	

	if(m_pPicking)
		m_pPicking->Update();

	if(m_pPipeLine)
		m_pPipeLine->Tick();

	if (m_pFrustum)
		m_pFrustum->Tick();

	if(m_pObject_Manager)
		m_pObject_Manager->Late_Tick(fTimeDelta);
	
	if (m_pPhysics_Controller)
		m_pPhysics_Controller->Simulate(fTimeDelta);
}

HRESULT CGameInstance::Begin_Draw(const _float4 & vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
	m_pGraphic_Device->Clear_DepthStencil_View();

	return S_OK;
}

HRESULT CGameInstance::End_Draw()
{
	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::Draw()
{
	if (nullptr == m_pGraphic_Device || 
		nullptr == m_pLevel_Manager)
		return E_FAIL;

	m_pRenderer->Render();

	m_pLevel_Manager->Render();	

	return S_OK;
}

HRESULT CGameInstance::Clear(_uint iClearLevelIndex)
{
	if (nullptr == m_pObject_Manager ||
		nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pObject_Manager->Clear(iClearLevelIndex);

	m_pComponent_Manager->Clear(iClearLevelIndex);

	return S_OK;
}

//wstring CGameInstance::UTF8ToUTF16(const string& utf8Str)
//{
//	//wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
//	//return converter.from_bytes(utf8Str);
//
//}

#pragma region Input_Device
_uint CGameInstance::Get_KeyState(_int iKey)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_KeyState(iKey);
}

_float CGameInstance::Get_Key_Press_Time(_int iKey)
{
	if (nullptr == m_pInput_Device)
		return 0.f;

	return m_pInput_Device->Get_Key_Press_Time(iKey);
}

void CGameInstance::Update_WHEEL(_int iDelta)
{
	if (nullptr == m_pInput_Device)
		return;

	return m_pInput_Device->Update_WHEEL(iDelta);
}

_bool CGameInstance::Check_Wheel_Down()
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Check_Wheel_Down();
}

_bool CGameInstance::Check_Wheel_Up()
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Check_Wheel_Up();
}

_int CGameInstance::Check_Wheel_Delta()
{
	if (nullptr == m_pInput_Device)
		return false;

	return m_pInput_Device->Check_Wheel_Delta();
}

void CGameInstance::Wheel_Clear()
{
	if (nullptr == m_pInput_Device)
		return;

	return m_pInput_Device->Wheel_Clear();
}

POINT CGameInstance::Get_MouseCurPos()
{
	if (nullptr == m_pInput_Device)
		return POINT{};

	return m_pInput_Device->Get_MouseCurPos();
}

POINT CGameInstance::Get_MouseDeltaPos()
{
	if (nullptr == m_pInput_Device)
		return POINT{};

	return m_pInput_Device->Get_MouseDeltaPos();
}

void CGameInstance::Set_MouseCurPos(POINT ptPos)
{
	if (nullptr == m_pInput_Device)
		return;

	m_pInput_Device->Set_MouseCurPos(ptPos);
}
#pragma endregion

#pragma region Renderer
HRESULT CGameInstance::Add_RenderGroup(CRenderer::RENDERGROUP eRenderGroup, CGameObject * pRenderObject)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_RenderGroup(eRenderGroup, pRenderObject);	
}

void CGameInstance::Set_Shadow_Resolution(CRenderer::SHADOW_RESOLUTION eResolution)
{
	if (nullptr == m_pRenderer)
		return;

	m_pRenderer->Set_Shadow_Resolution(eResolution);
}

void CGameInstance::Set_RadialBlur(_float fBlurAmount, _float2 BlurUV)
{
	if (nullptr == m_pRenderer)
		return;

	m_pRenderer->Set_RadialBlur(fBlurAmount, BlurUV);
}

void CGameInstance::On_RadialBlur()
{
	if (nullptr == m_pRenderer)
		return;

	m_pRenderer->On_RadialBlur();
}

void CGameInstance::Off_RadialBlur()
{
	if (nullptr == m_pRenderer)
		return;

	m_pRenderer->Off_RadialBlur();
}
_bool CGameInstance::Get_ShaderState(SHADER_STATE eState)
{
	if (nullptr == m_pRenderer)
		return false;

	return m_pRenderer->Get_ShaderState(eState);
}
void CGameInstance::Set_ShaderState(SHADER_STATE eState, _bool isState)
{
	if (nullptr == m_pRenderer)
		return;

	m_pRenderer->Set_ShaderState(eState, isState);
}
void CGameInstance::Set_RenderFieldShadow(_bool isRenderFieldShadow)
{
	if (nullptr == m_pRenderer)
		return;

	m_pRenderer->Set_RenderFieldShadow(isRenderFieldShadow);
}
#ifdef _DEBUG
void CGameInstance::On_Off_DebugRender()
{
	if (nullptr == m_pRenderer)
		return;

	m_pRenderer->On_Off_DebugRender();
}
#endif
#pragma endregion

#pragma region Level_Manager
HRESULT CGameInstance::Open_Level(_uint iNewLevelID, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Request_Open_Level(iNewLevelID, pNewLevel);
}

_uint CGameInstance::Get_CurrentLevel()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Get_CurrentLevel();
}
#pragma endregion

#pragma region Object_Manager
HRESULT CGameInstance::Add_Prototype(const wstring & strPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);	
}

HRESULT CGameInstance::Add_Clone(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Clone(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

CGameObject * CGameInstance::Clone_GameObject(const wstring & strPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject(strPrototypeTag, pArg);
}

const CComponent * CGameInstance::Get_Component(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strComTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag, strComTag, iIndex);
}

list<class CGameObject*>* CGameInstance::Find_Layer(_uint iLevelIndex, const wstring& LayerTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	CLayer*			pLayer = m_pObject_Manager->Find_Layer(iLevelIndex, LayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_ObjectList_Ptr();
}

void CGameInstance::Release_Layer(_uint iLevelIndex, const wstring& LayerTag)
{
	_ASSERT(m_pObject_Manager != nullptr);

	m_pObject_Manager->Release_Layer(iLevelIndex, LayerTag);
}
HRESULT CGameInstance::Add_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	_ASSERT(m_pObject_Manager != nullptr);

	return m_pObject_Manager->Add_Layer(iLevelIndex, strLayerTag);
}
#pragma endregion

#pragma region Component_Manager
HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring & strPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
	{
		MSG_BOX(TEXT("Error: m_pComponent_Manager -> nullptr"));

		return E_FAIL;
	}

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const wstring & strPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
	{
		MSG_BOX(TEXT("Error:m_pComponent_Manager -> nullptr"));

		return nullptr;
	}

	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}
#pragma endregion

#pragma region Timer_Manager
HRESULT CGameInstance::Add_Timer(const wstring & strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
	{
		MSG_BOX(TEXT("Error: m_pTimer_Manager -> nullptr"));

		return E_FAIL;
	}

	return m_pTimer_Manager->Add_Timer(strTimerTag);	
}

_float CGameInstance::Compute_TimeDelta(const wstring & strTimerTag)
{
	if (nullptr == m_pTimer_Manager)
	{
		MSG_BOX(TEXT("Error: m_pTimer_Manager -> nullptr"));

		return 0.0f;
	}

	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}
#pragma endregion

#pragma region PipeLine
void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

HRESULT CGameInstance::Add_ShadowLight(CPipeLine::SHADOWLIGHT eShadowLight, CLight* pLight)
{
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	m_pPipeLine->Add_ShadowLight(eShadowLight, pLight);
	return S_OK;
}

HRESULT CGameInstance::Add_ShadowLight(CPipeLine::SHADOWLIGHT eShadowLight, const wstring& strLightTag)
{
	if (nullptr == m_pPipeLine || nullptr == m_pLight_Manager)
		return E_FAIL;

	if (nullptr == m_pLight_Manager->Get_Light(strLightTag))
		return E_FAIL;

	m_pPipeLine->Add_ShadowLight(eShadowLight, m_pLight_Manager->Get_Light(strLightTag));
	return S_OK;
}

_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_Transform_Matrix(eState);
}

_float4x4 CGameInstance::Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_Transform_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORMSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_Transform_Matrix_Inverse(eState);
}

_float4x4 CGameInstance::Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORMSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_Transform_Float4x4_Inverse(eState);
}

_vector CGameInstance::Get_CamPosition_Vector() const
{
	if (nullptr == m_pPipeLine)
		return XMVectorZero();

	return m_pPipeLine->Get_CamPosition_Vector();
}

_float4 CGameInstance::Get_CamPosition_Float4() const
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_CamPosition_Float4();
}

_matrix CGameInstance::Get_PrevTransform_Matrix(CPipeLine::TRANSFORMSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_PrevTransform_Matrix(eState);
}

_float4x4 CGameInstance::Get_PrevTransform_Float4x4(CPipeLine::TRANSFORMSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_PrevTransform_Float4x4(eState);
}

_matrix CGameInstance::Get_PrevTransform_Matrix_Inverse(CPipeLine::TRANSFORMSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_PrevTransform_Matrix_Inverse(eState);
}

_float4x4 CGameInstance::Get_PrevTransform_Float4x4_Inverse(CPipeLine::TRANSFORMSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_PrevTransform_Float4x4_Inverse(eState);
}

_vector CGameInstance::Get_PrevCamPosition_Vector() const
{
	if (nullptr == m_pPipeLine)
		return _vector();

	return m_pPipeLine->Get_PrevCamPosition_Vector();
}

_float4 CGameInstance::Get_PrevCamPosition_Float4() const
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_PrevCamPosition_Float4();
}

const CLight* CGameInstance::Get_ShadowLight(CPipeLine::SHADOWLIGHT eShadowLight, _uint iIndex)
{
	if (nullptr == m_pPipeLine)
		return nullptr;
	

	return m_pPipeLine->Get_ShadowLight(eShadowLight, iIndex);
}

list<LIGHT_DESC*> CGameInstance::Get_ShadowPointLightDesc_List()
{
	if (nullptr == m_pPipeLine)
		return list<LIGHT_DESC*>();

	return m_pPipeLine->Get_ShadowPointLightDesc_List();
}
#pragma endregion

#pragma region Light_Manager
const LIGHT_DESC* CGameInstance::Get_LightDesc(const wstring& strLightTag, _uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(strLightTag, iIndex);
}

CLight* CGameInstance::Get_Light(const wstring& strLightTag)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_Light(strLightTag);
}

HRESULT CGameInstance::Add_Light(const wstring& strLightTag, const LIGHT_DESC& LightDesc, _float fFovY, _float fAspect, _float fNearZ, _float fFarZ)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(strLightTag, LightDesc, fFovY, fAspect, fNearZ, fFarZ);
}

HRESULT CGameInstance::Add_Light_Layer(const wstring& strLightTag)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light_Layer(strLightTag);
}

list<LIGHT_DESC*>* CGameInstance::Get_Light_List(const wstring& strLightTag)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_Light_List(strLightTag);
}

HRESULT CGameInstance::Update_Light(const wstring& strLightTag, const LIGHT_DESC& LightDesc, _uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Update_Light(strLightTag, LightDesc, iIndex);
}

HRESULT CGameInstance::Render_Lights(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Render(pShader, pVIBuffer);
}
#pragma endregion

#pragma region Font_Manager
HRESULT CGameInstance::Add_Font(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strFontTag, const wstring & strFontFilePath)
{
	return m_pFont_Manager->Add_Font(pDevice, pContext, strFontTag, strFontFilePath);	
}

HRESULT CGameInstance::Render_Font(const wstring & strFontTag, const wstring & strText, const _float2 & vPosition, _fvector vColor, _float fRadian)
{
	return m_pFont_Manager->Render(strFontTag, strText, vPosition, vColor, fRadian);
}
#pragma endregion

#pragma region Target_Manager
HRESULT CGameInstance::Add_RenderTarget(const wstring & strRenderTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4 & vClearColor, _bool isTickClear)
{
	return m_pTarget_Manager->Add_RenderTarget(strRenderTargetTag, iSizeX, iSizeY, ePixelFormat, vClearColor, isTickClear);
}

HRESULT CGameInstance::Add_RenderTarget_Cube(const wstring& strRenderTargetTag, _uint iSize, _uint iArraySize, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _bool isTickClear)
{
	return m_pTarget_Manager->Add_RenderTarget_Cube(strRenderTargetTag, iSize, iArraySize, ePixelFormat, vClearColor, isTickClear);
}

HRESULT CGameInstance::Add_RenderTarget_3D(const wstring& strRenderTargetTag, _uint iWidth, _uint iHeight, _uint iDepth, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _bool isTickClear)
{
 	return m_pTarget_Manager->Add_RenderTarget_3D(strRenderTargetTag, iWidth, iHeight, iDepth, ePixelFormat, vClearColor, isTickClear);
}

HRESULT CGameInstance::Clear_RenderTarget_All()
{
	return m_pTarget_Manager->Clear_RenderTarget_All();
}

HRESULT CGameInstance::Clear_RenderTarget(const wstring& strRenderTargetTag)
{
	return m_pTarget_Manager->Clear_RenderTarget(strRenderTargetTag);
}

HRESULT CGameInstance::Add_MRT(const wstring & strMRTTag, const wstring & strRenderTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strRenderTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const wstring & strMRTTag, ID3D11DepthStencilView* pDSV)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag, pDSV);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}
#pragma endregion

#pragma region Picking
void CGameInstance::Transform_PickingToLocalSpace(CTransform* pTransform, _float3* pRayDir, _float3* pRayPos)
{
	return m_pPicking->Transform_PickingToLocalSpace(pTransform, pRayDir, pRayPos);
}

void CGameInstance::Transform_PickingToWorldSpace(_float4* pRayDir, _float4* pRayPos)
{
	return m_pPicking->Transform_PickingToWorldSpace(pRayDir, pRayPos);
}

void CGameInstance::Get_PickingWordSpace(_float3* pRayDir, _float3* pRayPos)
{
	if (nullptr == m_pPicking)
		return;

	return m_pPicking->Get_PickingWordSpace(pRayDir, pRayPos);
}

void CGameInstance::Get_PickingWordSpace(_vector& pRayDir, _vector& pRayPos)
{
	if (nullptr == m_pPicking)
		return;

	return m_pPicking->Get_PickingWordSpace(pRayDir, pRayPos);
}

void CGameInstance::ClipCursor(HWND hWnd)
{
	if (nullptr == m_pPicking)
		return;

	m_pPicking->ClipCursor(hWnd);
}

_float2 CGameInstance::Get_ProjMousePos()
{
	if (nullptr == m_pPicking)
		return _float2();

	return m_pPicking->Get_ProjMousePos();
}

POINT CGameInstance::Get_ptProjMousePos()
{
	if (nullptr == m_pPicking)
		return POINT();

	return m_pPicking->Get_ptProjMousePos();
}

_float2 CGameInstance::Get_ViewMousePos()
{
	if (nullptr == m_pPicking)
		return _float2();

	return m_pPicking->Get_ViewMousePos();
}

POINT CGameInstance::Get_ptViewMousePos()
{
	if (nullptr == m_pPicking)
		return POINT();

	return m_pPicking->Get_ptViewMousePos();
}

#pragma endregion

#pragma region Random_Value_Generator
uniform_real_distribution<_float> CGameInstance::GetRandomDevice_Real(_float Start, _float End)
{
	return uniform_real_distribution<_float>(Start,End);
}

uniform_int_distribution<_int> CGameInstance::GetRandomDevice_Int(_int Start, _int End)
{
	return uniform_int_distribution<_int>(Start,End);
}
#pragma endregion

#pragma region Target_Manager
HRESULT CGameInstance::Bind_RTShaderResource(CShader * pShader, const wstring & strRenderTargetTag, const _char * pConstantName)
{
	if (nullptr == m_pTarget_Manager)
	{
		MSG_BOX(TEXT("nullptr == m_pTarget_Manager : CGameInstance"));
		return E_FAIL;
	}	
	return m_pTarget_Manager->Bind_ShaderResource(pShader, strRenderTargetTag, pConstantName);
}

HRESULT CGameInstance::Bind_RTShaderResource(CComputeShader* pShader, const wstring& strRenderTargetTag, const _char* pConstantName)
{
	if (nullptr == m_pTarget_Manager)
	{
		MSG_BOX(TEXT("nullptr == m_pTarget_Manager : CGameInstance"));
		return E_FAIL;
	}
	return m_pTarget_Manager->Bind_ShaderResource(pShader, strRenderTargetTag, pConstantName);
}

HRESULT CGameInstance::Bind_OutputShaderResource(CComputeShader* pShader, const wstring& strRenderTargetTag, const _char* pConstantName)
{
	if (nullptr == m_pTarget_Manager)
	{
		MSG_BOX(TEXT("nullptr == m_pTarget_Manager : CGameInstance"));
		return E_FAIL;
	}
	return m_pTarget_Manager->Bind_OutputShaderResource(pShader, strRenderTargetTag, pConstantName);
}

HRESULT CGameInstance::Copy_Resource(const wstring & strRenderTargetTag, ID3D11Texture2D ** ppTextureHub)
{
	return m_pTarget_Manager->Copy_Resource(strRenderTargetTag, ppTextureHub);
}
HRESULT CGameInstance::Copy_Resource(const wstring& strDestRenderTargetTag, const wstring& strSrcRenderTargetTag)
{
	return m_pTarget_Manager->Copy_Resource(strDestRenderTargetTag, strSrcRenderTargetTag);
}
#pragma endregion

#pragma region Frustrum
_bool CGameInstance::isInFrustum_WorldSpace(_fvector vWorldPos, _float fRange)
{
	if (nullptr == m_pFrustum)
	{
		MSG_BOX(TEXT("nullptr == m_pFrustum : CGameInstance"));
		return false;
	}
	return m_pFrustum->isIn_WorldSpace(vWorldPos, fRange);
}

_bool CGameInstance::isInFrustum_LocalSpace(_fvector vLocalPos, _float fRange)
{
	if (nullptr == m_pFrustum)
	{
		MSG_BOX(TEXT("nullptr == m_pFrustum : CGameInstance"));
		return false;
	}
	return m_pFrustum->isIn_LocalSpace(vLocalPos, fRange);
}

void CGameInstance::TransformFrustum_LocalSpace(_fmatrix WorldMatrixInv)
{
	if (nullptr == m_pFrustum)
	{
		MSG_BOX(TEXT("nullptr == m_pFrustum : CGameInstance"));
		return;
	}
	m_pFrustum->Transform_LocalSpace(WorldMatrixInv);
}

_bool CGameInstance::isInFrustum_WorldSpace_Cube(_fvector vWorldPos, _float size, _float fRange)
{
	return m_pFrustum->isIn_WorldSpace_Cube(vWorldPos, size, fRange);
}

#pragma endregion

#pragma region Extractor
_vector CGameInstance::Compute_WorldPos(const _float2 & vViewportPos, const wstring & strZRenderTargetTag, _uint iOffset)
{
	return m_pExtractor->Compute_WorldPos(vViewportPos, strZRenderTargetTag, iOffset);	
}
#pragma endregion

#pragma region Physics_Controller
void CGameInstance::Cook_Mesh(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum)
{
	m_pPhysics_Controller->Cook_Mesh(pVertices, pIndices, VertexNum, IndexNum);
}

_matrix CGameInstance::GetWorldMatrix_Rigid_Dynamic(_int Index)
{
	return m_pPhysics_Controller->GetWorldMatrix_Rigid_Dynamic(Index);
}

CCharacter_Controller* CGameInstance::GetCharacter_Controller(_int Index)
{
	return m_pPhysics_Controller->GetCharacter_Controller(Index);
}

CRigid_Dynamic* CGameInstance::GetRigid_Dynamic(_int Index)
{
	return m_pPhysics_Controller->GetRigid_Dynamic(Index);
}

_float4 CGameInstance::GetTranslation_Rigid_Dynamic(_int Index)
{
	return m_pPhysics_Controller->GetTranslation_Rigid_Dynamic(Index);
}

void CGameInstance::Simulate()
{
	m_pPhysics_Controller->Simulate(1.f / 60.f);
}

void CGameInstance::InitTerrainPhysics()
{
	if(m_pPhysics_Controller)
		m_pPhysics_Controller->InitTerrain();
}

void CGameInstance::SetBone_Ragdoll(vector<class CBone*>* vecBone)
{
	m_pPhysics_Controller->SetBone_Ragdoll(vecBone);
}

void CGameInstance::SetWorldMatrix(_float4x4 WorldMatrix)
{
	m_pPhysics_Controller->SetWorldMatrix(WorldMatrix);
}

void CGameInstance::SetRotationMatrix(_float4x4 RotationMatrix)
{
	m_pPhysics_Controller->SetRotationMatrix(RotationMatrix);
}

CCharacter_Controller* CGameInstance::Create_Controller(_float4 Pos, _int* Index,CGameObject* pCharacter)
{
	return m_pPhysics_Controller->Create_Controller(Pos, Index, pCharacter);
}

void CGameInstance::Cook_Terrain()
{
	m_pPhysics_Controller->InitTerrain();
}
#pragma endregion

#pragma region	Thread_Pool
void CGameInstance::Insert_Job(function<void()> job)
{
	m_pThread_Pool->EnqueueJob(job);
}
_bool CGameInstance::AllJobsFinished()
{
	return m_pThread_Pool->AllJobsFinished();
}
#pragma endregion

#pragma region Graphic Device

#pragma endregion

#pragma region Render_Target_Debugger
#ifdef _DEBUG

HRESULT CGameInstance::Ready_RTVDebug(const wstring & strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strRenderTargetTag, fX, fY, fSizeX, fSizeY);
}

HRESULT CGameInstance::Draw_RTVDebug(const wstring& strMRTTag, CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	return m_pTarget_Manager->Render_Debug(strMRTTag, pShader, pVIBuffer);
}

HRESULT CGameInstance::Add_DebugComponents(CComponent* pRenderComponent)
{

	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_DebugComponents(pRenderComponent);
}
#endif
#pragma endregion

#pragma region Sound_Manager
HRESULT CGameInstance::Update_Listener(FMOD_3D_ATTRIBUTES& Attributes_desc)
{
	if (nullptr == m_pSound_Manager)
	{
		MSG_BOX(TEXT("nullptr == m_pSound_Manager : CGameInstance"));
		return E_FAIL;
	}
	return m_pSound_Manager->Update_Listener(Attributes_desc);
}

void CGameInstance::Update_Sound(const wstring& pSoundKey, SOUND_DESC _SoundTag)
{
	if (nullptr == m_pSound_Manager)
	{
		MSG_BOX(TEXT("nullptr == m_pSound_Manager : CGameInstance"));
		return;
	}
	m_pSound_Manager->Update_Sound(pSoundKey, _SoundTag);
}
HRESULT CGameInstance::Channel_Pause(_uint iID)
{
	if (nullptr == m_pSound_Manager)
	{
		MSG_BOX(TEXT("nullptr == m_pSound_Manager : CGameInstance"));
		return E_FAIL;
	}
	
	return m_pSound_Manager->Channel_Pause(iID);
}
HRESULT CGameInstance::Change_Channel_Sound(const wstring& SoundKeyTag, _uint iID)
{
	if (nullptr == m_pSound_Manager)
	{
		MSG_BOX(TEXT("nullptr == m_pSound_Manager : CGameInstance"));
		return E_FAIL;
	}

	return m_pSound_Manager->Change_Channel_Sound(SoundKeyTag, iID);
}
HRESULT CGameInstance::Play_Sound(const wstring& SoundKeyTag, _uint iID)
{
	if (nullptr == m_pSound_Manager)
	{
		MSG_BOX(TEXT("nullptr == m_pSound_Manager : CGameInstance"));
		return E_FAIL;
	}

 	return m_pSound_Manager->Play_Sound(SoundKeyTag, iID);
}
HRESULT CGameInstance::Play_Sound_Again(const wstring& SoundKeyTag, _uint iID)
{
	if (nullptr == m_pSound_Manager)
	{
		MSG_BOX(TEXT("nullptr == m_pSound_Manager : CGameInstance"));
		return E_FAIL;
	}

	return m_pSound_Manager->Play_Sound_Again(SoundKeyTag, iID);
}
HRESULT CGameInstance::PlayBGM(_uint iID, const wstring& SoundKey)
{
	if (nullptr == m_pSound_Manager)
	{
		MSG_BOX(TEXT("nullptr == m_pSound_Manager : CGameInstance"));
		return E_FAIL;
	}

	return m_pSound_Manager->PlayBGM(iID, SoundKey);
}
HRESULT CGameInstance::Stop_Sound(_uint iID)
{
	if (nullptr == m_pSound_Manager)
	{
		MSG_BOX(TEXT("nullptr == m_pSound_Manager : CGameInstance"));
		return E_FAIL;
	}

	return m_pSound_Manager->Stop_Sound(iID);
}
HRESULT CGameInstance::Stop_All()
{
	if (nullptr == m_pSound_Manager)
	{
		MSG_BOX(TEXT("nullptr == m_pSound_Manager : CGameInstance"));
		return E_FAIL;
	}

	return m_pSound_Manager->Stop_All();
}
#pragma endregion

void CGameInstance::Release_Engine()
{
	CGameInstance::Get_Instance()->Free();

	Destroy_Instance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pPhysics_Controller);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pExtractor);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pRenderer);	
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pPicking);
	Safe_Release(m_pThread_Pool);
}
