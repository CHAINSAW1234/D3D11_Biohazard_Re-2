#pragma once

#include "Renderer.h"
#include "Component_Manager.h"
#include "PipeLine.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(HINSTANCE hInstance, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void Tick_Engine(_float fTimeDelta);
	HRESULT Begin_Draw(const _float4 & vClearColor);
	HRESULT End_Draw();
	HRESULT Draw();
	HRESULT Clear(_uint iClearLevelIndex);

public: /* For.Input_Device */
	_uint	Get_KeyState(_int iKey);
	_float	Get_Key_Press_Time(_int iKey);
	void	Update_WHEEL(_int iDelta);
	_bool	Check_Wheel_Down();
	_bool	Check_Wheel_Up();
	_int	Check_Wheel_Delta();
	void	Wheel_Clear();
	POINT	Get_MouseCurPos();
	POINT	Get_MouseDeltaPos();
	void	Set_MouseCurPos(POINT ptPos);

public: /* For.Renderer */
	HRESULT Add_RenderGroup(CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
	void Set_Shadow_Resolution(CRenderer::SHADOW_RESOLUTION eResolution);
	void Set_RadialBlur(_float fBlurAmount, _float2 BlurUV);
	void On_RadialBlur();
	void Off_RadialBlur();

#ifdef _DEBUG
public:
	HRESULT Add_DebugComponents(class CComponent* pRenderComponent);
#endif

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iNewLevelID, class CLevel* pNewLevel);

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_Clone(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	class CGameObject* Clone_GameObject(const wstring& strPrototypeTag, void* pArg = nullptr);
	const CComponent* Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComTag, _uint iIndex = 0);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);

public: /* For.Timer_Manager */
	HRESULT Add_Timer(const wstring& strTimerTag);
	_float Compute_TimeDelta(const wstring& strTimerTag);

public: /* For.PipeLine */
	void Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix);
	HRESULT Add_ShadowLight(CPipeLine::SHADOWLIGHT eShadowLight, CLight* pLight);
	HRESULT Add_ShadowLight(CPipeLine::SHADOWLIGHT eShadowLight, const wstring& strLightTag);
	_matrix Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE eState) const;
	_float4x4 Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE eState) const;
	_matrix Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORMSTATE eState) const;
	_float4x4 Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORMSTATE eState) const;
	_vector Get_CamPosition_Vector() const;
	_float4 Get_CamPosition_Float4() const;
	_uint Get_NumShadowSpotLight();
	const CLight* Get_ShadowLight(CPipeLine::SHADOWLIGHT eShadowLight, _uint iLightIndex = 0); // spotlight는 Light내부의 list에 LIGHT_DESC가 하나만 들어있도록 처리할 것 	
	list<LIGHT_DESC*> Get_ShadowPointLightDesc_List();

public: /* For.Light_Manager */
	const LIGHT_DESC* Get_LightDesc(const wstring& strLightTag, _uint iIndex);
	class CLight* Get_Light(const wstring& strLightTag);
	HRESULT Add_Light(const wstring& strLightTag, const LIGHT_DESC& LightDesc, _float fFovY = XMConvertToRadians(90.f), _float fAspect = 1, _float fNearZ = 0.01, _float fFarZ = 1000);
	HRESULT Add_Light_Layer(const wstring& strLightTag);
	list<LIGHT_DESC*>* Get_Light_List(const wstring& strLightTag);
	HRESULT Update_Light(const wstring& strLightTag, const LIGHT_DESC& LightDesc, _uint iIndex);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);


public: /* For.Font_Manager */
	HRESULT Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFontTag, const wstring& strFontFilePath);
	HRESULT Render_Font(const wstring& strFontTag, const wstring & strText, const _float2 & vPosition, _fvector vColor, _float fRadian);

public: /* For.Target_Manager */
	HRESULT Add_RenderTarget(const wstring& strRenderTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_RenderTarget_Cube(const wstring& strRenderTargetTag, _uint iSize, _uint iArraySize, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_RenderTarget_3D(const wstring& strRenderTargetTag, _uint iWidth, _uint iHeight, _uint iDepth, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Clear_RenderTarget(const wstring& strRenderTargetTag);

	HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strRenderTargetTag);
	HRESULT Begin_MRT(const wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT End_MRT();
	HRESULT Bind_RTShaderResource(class CShader* pShader, const wstring& strRenderTargetTag, const _char* pConstantName);
	HRESULT Bind_RTShaderResource(class CComputeShader* pShader, const wstring& strRenderTargetTag, const _char* pConstantName);
	HRESULT Bind_OutputShaderResource(class CComputeShader* pShader, const wstring& strRenderTargetTag, const _char* pConstantName);
	HRESULT Copy_Resource(const wstring& strRenderTargetTag, ID3D11Texture2D** ppTextureHub);

public: /* For.Frustum */
	_bool isInFrustum_WorldSpace(_fvector vWorldPos, _float fRange = 0.f);
	_bool isInFrustum_LocalSpace(_fvector vLocalPos, _float fRange = 0.f);
	void TransformFrustum_LocalSpace(_fmatrix WorldMatrixInv);

public: /* For.Extractor */
	_vector Compute_WorldPos(const _float2& vViewportPos, const wstring& strZRenderTargetTag, _uint iOffset = 0);

public: /* For.Picking */
	void Transform_PickingToLocalSpace(class CTransform* pTransform, _Out_ _float3* pRayDir, _Out_ _float3* pRayPos);
	void Transform_PickingToWorldSpace(_Out_ _float4* pRayDir, _Out_ _float4* pRayPos);

public:
	//Random Number
	uniform_real_distribution<_float>	GetRandomDevice_Real(_float Start, _float End);
	uniform_int_distribution<_int>		GetRandomDevice_Int(_int Start, _int End);

public:/*For Physics Controller*/
	_float4 GetPosition_Physics();
	void	Simulate();
	void	SetSimulate(_bool boolean)
	{
		m_bSimulate = boolean;
	}
	_bool	GetSimulate()
	{
		return m_bSimulate;
	}
	class CCharacter_Controller* GetCharacter_Controller(_int Index);
	class CRigid_Dynamic* GetRigid_Dynamic(_int Index);
	_float4	GetTranslation_Rigid_Dynamic(_int Index);
	_matrix	GetWorldMatrix_Rigid_Dynamic(_int Index);
	void	Cook_Mesh(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum);

public://Temp
	_float3* TerrainPos = { nullptr };
	void	SetTerrainPos(_float3 * Pos)
	{
		TerrainPos = Pos;
	}
	_float3* GetTerrainPos()
	{
		return TerrainPos;
	}
	_int		NumVertices = { 0 };
	_int		NumIndices = { 0 };
	void	SetNumVertices(_int Num)
	{
		NumVertices = Num;
	}
	_int	GetNumVertices()
	{
		return NumVertices;
	}
	void	SetNumIndicies(_int Num)
	{
		NumIndices = Num;
	}
	_int	GetNumIndicies()
	{
		return NumIndices;
	}

	_uint* m_pIndices = { nullptr };
	void	SetIndices(_uint* Pos)
	{
		m_pIndices = Pos;
	}
	_uint* GetIndices()
	{
		return m_pIndices;
	}
	void			InitTerrainPhysics();
	void			Move_CCT(_float4 Dir, _float fTimeDelta,_int Index);
	void			SetColliderTransform(_float4x4 Transform);
	void			SetColliderTransform_Head(_float4x4 Transform);
	void			SetColliderTransform_Left_Arm(_float4x4 Transform);
	void			SetColliderTransform_Right_Arm(_float4x4 Transform);
	void			SetColliderTransform_Left_ForeArm(_float4x4 Transform);
	void			SetColliderTransform_Right_ForeArm(_float4x4 Transform);
	void			SetColliderTransform_Pelvis(_float4x4 Transform);
	void			SetColliderTransform_Left_Leg(_float4x4 Transform);
	void			SetColliderTransform_Right_Leg(_float4x4 Transform);
	void			SetColliderTransform_Left_Shin(_float4x4 Transform);
	void			SetColliderTransform_Right_Shin(_float4x4 Transform);
#ifdef _DEBUG
	HRESULT Ready_RTVDebug(const wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Draw_RTVDebug(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

#endif

public: /* For.m_pSound_Manager */
	HRESULT Update_Listener(FMOD_3D_ATTRIBUTES& Attributes_desc);
	void Update_Sound(const wstring& pSoundKey, SOUND_DESC _SoundTag);
	HRESULT  Channel_Pause(_uint iID);
	HRESULT  Change_Channel_Sound(const wstring& SoundKeyTag, _uint iID);
	HRESULT Play_Sound(const wstring& SoundKeyTag, _uint iID);
	HRESULT Play_Sound_Again(const wstring& SoundKeyTag, _uint iID);
	HRESULT PlayBGM(_uint iID, const wstring& SoundKey);
	HRESULT Stop_Sound(_uint iID);
	HRESULT Stop_All();

private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };	
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CFont_Manager*			m_pFont_Manager = { nullptr };
	class CTarget_Manager*			m_pTarget_Manager = { nullptr };
	class CFrustum*					m_pFrustum = { nullptr };
	class CExtractor*				m_pExtractor = { nullptr };
	class CPhysics_Controller*		m_pPhysics_Controller = { nullptr };
	class CSound_Manager* 			m_pSound_Manager = { nullptr };
	class CPicking*					m_pPicking = { nullptr };

	/*for physics*/
	_bool							m_bSimulate = { false };

	//Random Number
	random_device					m_RandomDevice;
	mt19937_64						m_RandomNumber;
public:		
	static void Release_Engine();
	virtual void Free() override;
};

END