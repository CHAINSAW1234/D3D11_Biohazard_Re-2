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

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()	
{

}

HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInstance, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext)
{
	/* 그래픽 디바이스를 초기화한다 .*/
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

	/*m_pFont_Manager = CFont_Manager::Create(*ppGraphic_Device);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;
	*/
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
		

	

	/* 인풋 디바이스를 초기화한다 .*/

	/* 사운드 디바이스를 초기화한다 .*/

	/* 오브젝트 매니져의 공간 예약을 한다. */
	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
	{
		MSG_BOX(TEXT("Error: m_pObject_Manager::Create -> nullptr"));
		return E_FAIL;
	}

	/* 컴포넌트 매니져의 공간 예약을 한다. */
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
		return E_FAIL;
	
	m_pPhysics_Controller->Create_Controller(_float4(0.f,0.f,0.f,1.f));
	m_pPhysics_Controller->Create_Rigid_Dynamic(_float4(0.f, 0.f, 0.f, 1.f));

	m_RandomNumber = mt19937_64(m_RandomDevice());

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (nullptr == m_pLevel_Manager || 
		nullptr == m_pObject_Manager || 
		nullptr == m_pPipeLine||
		nullptr == m_pFrustum )
		return;

	m_pPipeLine->Reset();

	m_pInput_Device->Tick(fTimeDelta);

	m_pObject_Manager->Priority_Tick(fTimeDelta);	

	/* 반복적인 갱신이 필요한 객체들의 Tick함수를 호출한다. */
	m_pLevel_Manager->Tick(fTimeDelta);

	m_pObject_Manager->Tick(fTimeDelta);	

	m_pPicking->Update();

	m_pPipeLine->Tick();

	m_pFrustum->Tick();

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



	/* 화면에 그려져야할 객체들을 그리낟. == 오브젝트 매니져에 들어가있을꺼야 .*/
	/* 오브젝트 매니져에 렌더함수를 만들어서 호출하면 객체들을 다 그린다. */

	/* But. CRenderer객체의 렌더함수를 호출하여 객체를 그리낟. */
	m_pRenderer->Render();

	m_pLevel_Manager->Render();	

	return S_OK;
}

HRESULT CGameInstance::Clear(_uint iClearLevelIndex)
{
	if (nullptr == m_pObject_Manager ||
		nullptr == m_pComponent_Manager)
		return E_FAIL;

	/* 지정된 레벨용 자원(텍스쳐, 사운드, 객체등등) 을 삭제한다. */

	/* 사본 게임오브젝트. */
	m_pObject_Manager->Clear(iClearLevelIndex);

	/* 컴포넌트 원형 */
	m_pComponent_Manager->Clear(iClearLevelIndex);


	return S_OK;
}


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

#ifdef _DEBUG
HRESULT CGameInstance::Add_DebugComponents(CComponent * pRenderComponent)
{

	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_DebugComponents(pRenderComponent);
}
#endif

HRESULT CGameInstance::Open_Level(_uint iNewLevelID, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iNewLevelID, pNewLevel);
}

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

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

void CGameInstance::Set_ShadowSpotLight(const wstring& strLightTag)
{
	if (nullptr == m_pPipeLine || nullptr == m_pLight_Manager)
		return;

	return m_pPipeLine->Set_ShadowSpotLight(m_pLight_Manager->Get_Light(strLightTag));
}

void CGameInstance::Add_ShadowLight(const wstring& strLightTag)
{
	if (nullptr == m_pPipeLine || nullptr == m_pLight_Manager)
		return;

	return m_pPipeLine->Add_ShadowLight(m_pLight_Manager->Get_Light(strLightTag));
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

_uint CGameInstance::Get_NumShadowLight()
{
	if (nullptr == m_pPipeLine)
		return 0;

	return m_pPipeLine->Get_NumShadowLight();
}

const CLight* CGameInstance::Get_ShadowLight(_uint iIndex)
{
	if (nullptr == m_pPipeLine)
		return nullptr;

	return m_pPipeLine->Get_ShadowLight(iIndex);
}

_matrix CGameInstance::Get_SpotLightTransform_Matrix(CPipeLine::TRANSFORMSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_SpotLightTransform_Matrix(eState);
}

_float4x4 CGameInstance::Get_SpotLightTransform_Float4x4(CPipeLine::TRANSFORMSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_SpotLightTransform_Float4x4(eState);
}

_matrix CGameInstance::Get_SpotLightTransform_Matrix_Inverse(CPipeLine::TRANSFORMSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_SpotLightTransform_Matrix_Inverse(eState);
}

_float4x4 CGameInstance::Get_SpotLightTransform_Float4x4_Inverse(CPipeLine::TRANSFORMSTATE eState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_SpotLightTransform_Float4x4_Inverse(eState);
}

const CLight* CGameInstance::Get_ShadowSpotLight()
{
	if (nullptr == m_pPipeLine)
		return nullptr;

	return m_pPipeLine->Get_ShadowSpotLight();
}


const LIGHT_DESC* CGameInstance::Get_LightDesc(const wstring& strLightTag)
{
	if (nullptr == m_pLight_Manager)
	{
		return nullptr;
	}
	return m_pLight_Manager->Get_LightDesc(strLightTag);
}

const _float4x4* CGameInstance::Get_LightViewMatrix(const wstring& strLightTag)
{
	if (nullptr == m_pLight_Manager)
	{
		return nullptr;
	}
	return m_pLight_Manager->Get_LightViewMatrix(strLightTag);
}

const _float4x4 CGameInstance::Get_LightProjMatrix(const wstring& strLightTag)
{
	if (nullptr == m_pLight_Manager)
	{
		return _float4x4();
	}
	return m_pLight_Manager->Get_LightProjMatrix(strLightTag);
}

HRESULT CGameInstance::Add_Light(const wstring& strLightTag, const LIGHT_DESC& LightDesc, _float fFovY, _float fAspect, _float fNearZ, _float fFarZ)
{
	if (nullptr == m_pLight_Manager)
	{
		MSG_BOX(TEXT("nullptr == m_pLight_Manager : CGameInstance"));
		return E_FAIL;
	}
	return m_pLight_Manager->Add_Light(strLightTag, LightDesc, fFovY, fAspect, fNearZ, fFarZ);
}

HRESULT CGameInstance::Render_Lights(CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	if (nullptr == m_pLight_Manager)
	{
		MSG_BOX(TEXT("nullptr == m_pLight_Manager : CGameInstance"));
		return E_FAIL;
	}
	return m_pLight_Manager->Render(pShader, pVIBuffer);
}

HRESULT CGameInstance::Tick_Light(const wstring& strLightTag, const LIGHT_DESC& LightDesc)
{
	if (nullptr == m_pLight_Manager)
	{
		MSG_BOX(TEXT("nullptr == m_pLight_Manager : CGameInstance"));
		return E_FAIL;
	}
	return m_pLight_Manager->Tick_Light(strLightTag, LightDesc);
}
HRESULT CGameInstance::Add_Font(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & strFontTag, const wstring & strFontFilePath)
{
	return m_pFont_Manager->Add_Font(pDevice, pContext, strFontTag, strFontFilePath);	
}

HRESULT CGameInstance::Render_Font(const wstring & strFontTag, const wstring & strText, const _float2 & vPosition, _fvector vColor, _float fRadian)
{
	return m_pFont_Manager->Render(strFontTag, strText, vPosition, vColor, fRadian);
}

HRESULT CGameInstance::Add_RenderTarget(const wstring & strRenderTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4 & vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strRenderTargetTag, iSizeX, iSizeY, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Add_RenderTarget_Cube(const wstring& strRenderTargetTag, _uint iSize, _uint iArraySize, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget_Cube(strRenderTargetTag, iSize, iArraySize, ePixelFormat, vClearColor);
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

_matrix CGameInstance::GetWorldMatrix_Rigid_Dynamic(_int Index)
{
	return m_pPhysics_Controller->GetWorldMatrix_Rigid_Dynamic(Index);
}

void CGameInstance::Cook_Mesh(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum)
{
	m_pPhysics_Controller->Cook_Mesh(pVertices, pIndices, VertexNum, IndexNum);
}

void CGameInstance::Transform_PickingToLocalSpace(CTransform* pTransform, _float3* pRayDir, _float3* pRayPos)
{
	return m_pPicking->Transform_PickingToLocalSpace(pTransform, pRayDir, pRayPos);
}

void CGameInstance::Transform_PickingToWorldSpace(_float4* pRayDir, _float4* pRayPos)
{
	return m_pPicking->Transform_PickingToWorldSpace(pRayDir, pRayPos);
}

uniform_real_distribution<_float> CGameInstance::GetRandomDevice_Real(_float Start, _float End)
{
	return uniform_real_distribution<_float>(Start,End);
}

uniform_int_distribution<_int> CGameInstance::GetRandomDevice_Int(_int Start, _int End)
{
	return uniform_int_distribution<_int>(Start,End);
}

HRESULT CGameInstance::Bind_RTShaderResource(CShader * pShader, const wstring & strRenderTargetTag, const _char * pConstantName)
{
	if (nullptr == m_pTarget_Manager)
	{
		MSG_BOX(TEXT("nullptr == m_pTarget_Manager : CGameInstance"));
		return E_FAIL;
	}	
	return m_pTarget_Manager->Bind_ShaderResource(pShader, strRenderTargetTag, pConstantName);
}

HRESULT CGameInstance::Copy_Resource(const wstring & strRenderTargetTag, ID3D11Texture2D ** ppTextureHub)
{
	return m_pTarget_Manager->Copy_Resource(strRenderTargetTag, ppTextureHub);
}

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

_vector CGameInstance::Compute_WorldPos(const _float2 & vViewportPos, const wstring & strZRenderTargetTag, _uint iOffset)
{
	return m_pExtractor->Compute_WorldPos(vViewportPos, strZRenderTargetTag, iOffset);	
}

_float4 CGameInstance::GetPosition_Physics()
{
	if (m_pPhysics_Controller)
		return m_pPhysics_Controller->GetPosition();

	return _float4(0.f, 0.f, 0.f, 1.f);
}

void CGameInstance::Simulate()
{
	m_pPhysics_Controller->Simulate(1.f / 60.f);
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

void CGameInstance::InitTerrainPhysics()
{
	if(m_pPhysics_Controller)
		m_pPhysics_Controller->InitTerrain();
}

void CGameInstance::Move_CCT(_float4 Dir, _float fTimeDelta, _int Index)
{
	m_pPhysics_Controller->Move_CCT(Dir, fTimeDelta, Index);
}

void CGameInstance::SetColliderTransform(_float4x4 Transform)
{
	m_pPhysics_Controller->SetColliderTransform(Transform);
}

void CGameInstance::SetColliderTransform_Head(_float4x4 Transform)
{
	m_pPhysics_Controller->SetColliderTransform_Head(Transform);
}

void CGameInstance::SetColliderTransform_Left_Arm(_float4x4 Transform)
{
	m_pPhysics_Controller->SetColliderTransform_Left_Arm(Transform);
}

void CGameInstance::SetColliderTransform_Right_Arm(_float4x4 Transform)
{
	m_pPhysics_Controller->SetColliderTransform_Right_Arm(Transform);
}

void CGameInstance::SetColliderTransform_Left_ForeArm(_float4x4 Transform)
{
	m_pPhysics_Controller->SetColliderTransform_Left_ForeArm(Transform);
}

void CGameInstance::SetColliderTransform_Right_ForeArm(_float4x4 Transform)
{
	m_pPhysics_Controller->SetColliderTransform_Right_ForeArm(Transform);
}

void CGameInstance::SetColliderTransform_Pelvis(_float4x4 Transform)
{
	m_pPhysics_Controller->SetColliderTransform_Pelvis(Transform);
}

void CGameInstance::SetColliderTransform_Left_Leg(_float4x4 Transform)
{
	m_pPhysics_Controller->SetColliderTransform_Left_Leg(Transform);
}

void CGameInstance::SetColliderTransform_Right_Leg(_float4x4 Transform)
{
	m_pPhysics_Controller->SetColliderTransform_Right_Leg(Transform);
}

void CGameInstance::SetColliderTransform_Left_Shin(_float4x4 Transform)
{
	m_pPhysics_Controller->SetColliderTransform_Left_Shin(Transform);
}

#ifdef _DEBUG

void CGameInstance::SetColliderTransform_Right_Shin(_float4x4 Transform)
{
	m_pPhysics_Controller->SetColliderTransform_Right_Shin(Transform);
}

HRESULT CGameInstance::Ready_RTVDebug(const wstring & strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strRenderTargetTag, fX, fY, fSizeX, fSizeY);
}
HRESULT CGameInstance::Draw_RTVDebug(const wstring& strMRTTag, CShader * pShader, CVIBuffer_Rect * pVIBuffer)
{
	return m_pTarget_Manager->Render_Debug(strMRTTag, pShader, pVIBuffer);
}
#endif


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

void CGameInstance::Release_Engine()
{
	CGameInstance::Get_Instance()->Free();

	Destroy_Instance();
}

void CGameInstance::Free()
{
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
	Safe_Release(m_pPhysics_Controller);
	Safe_Release(m_pPicking);
}
