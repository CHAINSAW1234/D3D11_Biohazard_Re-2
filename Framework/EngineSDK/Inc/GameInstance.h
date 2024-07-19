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
	HRESULT									Initialize_Engine(HINSTANCE hInstance, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);
	void									Tick_Engine(_float fTimeDelta);
	HRESULT									Begin_Draw(const _float4& vClearColor);
	HRESULT									End_Draw();
	HRESULT									Draw();
	HRESULT									Clear(_uint iClearLevelIndex);

#pragma region 편의성 함수
public:
	void									SetPlayer(CGameObject* pPlayer)
	{
		m_pPlayer = pPlayer;
	}
	_float4									GetPlayerPos();
	class CGameObject*						GetPlayer()
	{
		return m_pPlayer;
	}
	_bool									IsPaused()
	{
		return m_bPause;
	}
	void									Set_IsPaused(_bool bPause)
	{
		m_bPause = bPause;
	}



	wstring StringToWstring(const std::string& strString);

	string WstringToString(const wstring& wstrString);

	wstring ConvertToWString(const char* str, size_t len);


#pragma region 모델테그 모음
public:
	void Set_ModelTags(wstring keyTags, vector<wstring> Tags) {
		m_mapModelTags.emplace(keyTags, Tags);
	}
	vector<wstring> Get_ModelTags(wstring keyTags) { return m_mapModelTags[keyTags]; }

private:
	map< wstring, vector<wstring> > m_mapModelTags;

#pragma endregion


#pragma region Graphic_Device
public: /* For.Graphic_Device */
	HRESULT									Copy_BackBuffer(ID3D11Texture2D* pTexture);
#pragma endregion

#pragma region Input_Device
public: /* For.Input_Device */
	_uint									Get_KeyState(_int iKey);
	_float									Get_Key_Press_Time(_int iKey);
	void									Update_WHEEL(_int iDelta);
	_bool									Check_Wheel_Down();
	_bool									Check_Wheel_Up();
	_int									Check_Wheel_Delta();
	void									Wheel_Clear();
	POINT									Get_MouseCurPos();
	POINT									Get_MouseDeltaPos();
	void									Set_MouseCurPos(POINT ptPos);
#pragma endregion

#pragma region Renderer
public: /* For.Renderer */
	HRESULT									Add_RenderGroup(CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
	void									Set_RadialBlur(_float fBlurAmount, _float2 BlurUV);
	void									On_RadialBlur();
	void									Off_RadialBlur();
	_bool									Get_ShaderState(SHADER_STATE eState);

	void									Set_ShaderState(SHADER_STATE eState, _bool isState);
	void									Set_RenderFieldShadow(_bool isRenderFieldShadow);
	void									Bind_DepthTarget(class CShader* pShader);
	class CShader*							GetDeferredShader();
	HRESULT									Render_Decal_Deferred();
#ifdef _DEBUG
	void									On_Off_DebugRender();
#endif
#pragma endregion

#pragma region Level_Manager
public: /* For.Level_Manager */
	HRESULT									Open_Level(_uint iNewLevelID, class CLevel* pNewLevel);
	_uint									Get_CurrentLevel();
#pragma endregion

#pragma region Object_Manager
public: /* For.Object_Manager */
	HRESULT									Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	CComponent*								Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);
	HRESULT									Add_Clone(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg = nullptr);
	class CGameObject*						Get_GameObject(_uint iLevelIndex, const wstring& strPrototypeTag, _uint iIndex);
	class CGameObject*						Clone_GameObject(const wstring& strPrototypeTag, void* pArg = nullptr);
	const CComponent*						Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComTag, _uint iIndex = 0);
	list<class CGameObject*>*				Find_Layer(_uint iLevelIndex, const wstring& LayerTag);
	void									Release_Layer(_uint iLevelIndex, const wstring& LayerTag);
	//yeeun	
	HRESULT									Add_Layer(_uint iLevelIndex, const wstring& strLayerTag);
	void									Start();
	void									Active_Camera(_uint iLevelIndex, class CCamera* pCamera);
#pragma endregion

#pragma region Component_Manager
public: /* For.Component_Manager */
	HRESULT									Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, class CComponent* pPrototype);
	class CComponent*						Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);
#pragma endregion 

#pragma region Timer_Manager
public: /* For.Timer_Manager */
	HRESULT									Add_Timer(const wstring& strTimerTag);
	_float									Compute_TimeDelta(const wstring& strTimerTag);
#pragma endregion

#pragma region PipeLine
public: /* For.PipeLine */
	void									Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix);
	HRESULT									Add_ShadowLight(CPipeLine::SHADOWLIGHT eShadowLight, CLight* pLight);
	HRESULT									Add_ShadowLight(CPipeLine::SHADOWLIGHT eShadowLight, const wstring& strLightTag);
	void									Set_CubeMap(CTexture* pTexture, _uint iIndex = 0);
	_matrix									Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE eState) const;
	_float4x4								Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE eState) const;
	_matrix									Get_Transform_Matrix_Inverse(CPipeLine::TRANSFORMSTATE eState) const;
	_float4x4								Get_Transform_Float4x4_Inverse(CPipeLine::TRANSFORMSTATE eState) const;
	_vector									Get_CamPosition_Vector() const;
	_float4									Get_CamPosition_Float4() const;
	_matrix									Get_PrevTransform_Matrix(CPipeLine::TRANSFORMSTATE eState) const;
	_float4x4								Get_PrevTransform_Float4x4(CPipeLine::TRANSFORMSTATE eState) const;
	_matrix									Get_PrevTransform_Matrix_Inverse(CPipeLine::TRANSFORMSTATE eState) const;
	_float4x4								Get_PrevTransform_Float4x4_Inverse(CPipeLine::TRANSFORMSTATE eState) const;
	_vector									Get_PrevCamPosition_Vector() const;
	_float4									Get_PrevCamPosition_Float4() const;
	const CLight*							Get_ShadowLight(CPipeLine::SHADOWLIGHT eShadowLight, _uint iLightIndex = 0); // spotlight는 Light내부의 list에 LIGHT_DESC가 하나만 들어있도록 처리할 것 	
	list<LIGHT_DESC*>						Get_ShadowPointLightDesc_List();
	HRESULT									Bind_PrevIrradianceTexture(CShader* pShader, const _char* pConstantName);
	HRESULT									Bind_CurIrradianceTexture(CShader* pShader, const _char* pConstantName);
	HRESULT									Bind_PrevCubeMapTexture(CShader* pShader, const _char* pConstantName);
	HRESULT									Bind_CurCubeMapTexture(CShader* pShader, const _char* pConstantName);
	_float*									Get_PBRLerpTime();
#pragma endregion

#pragma region Light_Manager
public: /* For.Light_Manager */
	const LIGHT_DESC*						Get_LightDesc(const wstring& strLightTag, _uint iIndex);
	class CLight*							Get_Light(const wstring& strLightTag);
	HRESULT									Add_Light(const wstring& strLightTag, const LIGHT_DESC& LightDesc, _float fFovY = XMConvertToRadians(90.f), _float fAspect = 1, _float fNearZ = 0.01, _float fFarZ = 1000);
	HRESULT									Add_Light_Layer(const wstring& strLightTag);
	list<LIGHT_DESC*>*						Get_Light_List(const wstring& strLightTag);
	HRESULT									Update_Light(const wstring& strLightTag, const LIGHT_DESC& LightDesc, _uint iIndex, _float fLerp = 0.1f);
	HRESULT									Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#pragma endregion

#pragma region Font_Manager
public: /* For.Font_Manager */
	HRESULT									Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strFontTag, const wstring& strFontFilePath);
	HRESULT									Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor, _float fRadian);
	HRESULT									Render_Font_Scaled(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor, _float fRadian, _float fScale);
#pragma endregion

#pragma region Target_Manager
public: /* For.Target_Manager */
	HRESULT									Add_RenderTarget(const wstring& strRenderTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _bool isTickClear = true);
	HRESULT									Add_RenderTarget_Cube(const wstring& strRenderTargetTag, _uint iSize, _uint iArraySize, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _bool isTickClear = true);
	HRESULT									Add_RenderTarget_3D(const wstring& strRenderTargetTag, _uint iWidth, _uint iHeight, _uint iDepth, DXGI_FORMAT ePixelFormat, const _float4& vClearColor, _bool isTickClear = true);
	HRESULT									Clear_RenderTarget_All();
	HRESULT									Clear_RenderTarget(const wstring& strRenderTargetTag);
	HRESULT									Add_MRT(const wstring& strMRTTag, const wstring& strRenderTargetTag);
	HRESULT									Begin_MRT(const wstring& strMRTTag, ID3D11DepthStencilView* pDSV = nullptr);
	HRESULT									End_MRT();
	HRESULT									Bind_RTShaderResource(class CShader* pShader, const wstring& strRenderTargetTag, const _char* pConstantName);
	HRESULT									Bind_RTShaderResource(class CComputeShader* pShader, const wstring& strRenderTargetTag, const _char* pConstantName);
	HRESULT									Bind_OutputShaderResource(class CComputeShader* pShader, const wstring& strRenderTargetTag, const _char* pConstantName);
	HRESULT									Copy_Resource(const wstring& strRenderTargetTag, ID3D11Texture2D** ppTextureHub);
	HRESULT									Copy_Resource(const wstring& strDestRenderTargetTag, const wstring& strSrcRenderTargetTag);
	HRESULT									Copy_BackBuffer(const wstring& strRenderTargetTag);
#pragma endregion

#pragma region Frustrum
public: /* For.Frustum */
	_bool									isInFrustum_WorldSpace(_fvector vWorldPos, _float fRange = 0.f);
	_bool									isInFrustum_LocalSpace(_fvector vLocalPos, _float fRange = 0.f);
	void									TransformFrustum_LocalSpace(_fmatrix WorldMatrixInv);
	_bool									isInFrustum_WorldSpace_Cube(_fvector vWorldPos, _float size, _float fRange = 0.f);
#pragma endregion

#pragma region Extractor
public: /* For.Extractor */
	_vector									Compute_WorldPos(const _float2& vViewportPos, const wstring& strZRenderTargetTag, _uint iOffset = 0);
#pragma endregion

#pragma region Picking
public: /* For.Picking */
	void									Transform_PickingToLocalSpace(class CTransform* pTransform, _Out_ _float3* pRayDir, _Out_ _float3* pRayPos);
	void									Transform_PickingToWorldSpace(_Out_ _float4* pRayDir, _Out_ _float4* pRayPos);
	void									Get_PickingWordSpace(_Out_ _float3* pRayDir, _Out_ _float3* pRayPos);
	void									Get_PickingWordSpace(_Out_ _vector& pRayDir, _Out_ _vector& pRayPos);
	void									ClipCursor(HWND hWnd);
	_float2									Get_ProjMousePos();
	POINT									Get_ptProjMousePos();
	_float2									Get_ViewMousePos();
	POINT									Get_ptViewMousePos();
#pragma endregion

#pragma region Random_Value_Generator
public:
	//Random Number
	uniform_real_distribution<_float>		GetRandomDevice_Real(_float Start, _float End);
	uniform_int_distribution<_int>			GetRandomDevice_Int(_int Start, _int End);
	_int									GetRandom_Int(_int Start, _int End);
	_float									GetRandom_Real(_float Start, _float End);
#pragma endregion

#pragma region Render_Target_Debugger
#ifdef _DEBUG
	HRESULT									Ready_RTVDebug(const wstring& strRenderTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT									Draw_RTVDebug(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT									Add_DebugComponents(class CComponent* pRenderComponent);
#endif
#pragma endregion

#pragma region Sound_Manager
public: /* For.m_pSound_Manager */
	/*HRESULT									Update_Listener(FMOD_3D_ATTRIBUTES& Attributes_desc);
	void									Update_Sound(const wstring& pSoundKey, SOUND_DESC _SoundTag);
	HRESULT									Channel_Pause(_uint iID);
	HRESULT									Change_Channel_Sound(const wstring& SoundKeyTag, _uint iID);
	HRESULT									Play_Sound(const wstring& SoundKeyTag, _uint iID);
	HRESULT									Play_Sound_Again(const wstring& SoundKeyTag, _uint iID);
	HRESULT									Stop_Sound(_uint iID);*/
	HRESULT									Update_Listener(class CTransform* pTransform, _float3 vVelocity);
	HRESULT									Stop_Sound_2D(_uint iChannelIndex);
	HRESULT									Stop_Sound_3D(CTransform* pTransform, _uint iSoundIndex);

	void									Change_Sound_2D(_uint iChannelIndex, const wstring& strSoundTag);
	void									Change_Sound_2D(const wstring& strSoundTag, _uint iChannelIndex);
	void									Set_Volume_2D(_uint iChannelIndex, _float fVolume);
	void									Set_Pause_2D(_uint iChannelIndex, _bool isPause);

	void									Change_Sound_3D(CTransform* pTransform, const wstring& strSoundTag, _uint iSoundIndex);
	void									Set_Volume_3D(CTransform* pTransform, _uint iSoundIndex, _float fVolume);
	void									Set_Pause_3D(CTransform* pTransform, _uint iSoundIndex, _bool isPause);
	void									Set_Distance_3D(CTransform* pTransform, _uint iSoundIndex, _float fMinDistance, _float fMaxDistance);

	HRESULT									Add_Object_Sound(class CTransform* pTransform, _uint iNumChannel);
	HRESULT									PlayBGM(_uint iChannelIndex, const wstring& strSoundTag);
	HRESULT									Stop_All();
#pragma endregion

#pragma region Physics_Controller
public:/*For Physics Controller*/
	FORCEINLINE void						SetSimulate(_bool boolean)
	{
		m_bSimulate = boolean;
	}
	FORCEINLINE _bool						GetSimulate()
	{
		return m_bSimulate;
	}
	FORCEINLINE void						SetTerrainPos(_float3* Pos)
	{
		TerrainPos = Pos;
	}
	FORCEINLINE _float3*					GetTerrainPos()
	{
		return TerrainPos;
	}
	FORCEINLINE void						SetNumVertices(_int Num)
	{
		NumVertices = Num;
	}
	FORCEINLINE _int						GetNumVertices()
	{
		return NumVertices;
	}
	FORCEINLINE void						SetNumIndicies(_int Num)
	{
		NumIndices = Num;
	}
	FORCEINLINE _int						GetNumIndicies()
	{
		return NumIndices;
	}
	FORCEINLINE void						SetIndices(_uint* Pos)
	{
		m_pIndices = Pos;
	}
	FORCEINLINE _uint* GetIndices()
	{
		return m_pIndices;
	}
	class CCharacter_Controller*			GetCharacter_Controller(_int Index);
	class CRigid_Dynamic*					GetRigid_Dynamic(_int Index);
	_float4									GetTranslation_Rigid_Dynamic(_int Index);
	_matrix									GetWorldMatrix_Rigid_Dynamic(_int Index);
	void									InitTerrainPhysics();
	void									SetBone_Ragdoll_Ragdoll(vector<class CBone*>* vecBone);
	void									SetWorldMatrix_Ragdoll(_float4x4 WorldMatrix);
	void									SetRotationMatrix_Ragdoll(_float4x4 RotationMatrix);
	class CCharacter_Controller*			Create_Controller(_float4 Pos, _int* Index, class CGameObject* pCharacter,_float fHeight,_float fRadius, class CTransform* pTransform, vector<class CBone*>* pBones, const std::string& name = "");
	class CRigid_Static*					Create_Rigid_Static(_float4 Pos, _int* Index, class CGameObject* pStaticMesh);
	class CRigid_Dynamic*					Create_Rigid_Dynamic(class CModel* pModel, class CTransform* pTransform, _int* iId, class CGameObject* pObj);
	class CRigid_Dynamic*					Create_Rigid_Dynamic_NoConvex(_float fRadius,_int* iId, class CGameObject* pObj);
	class CRigid_Dynamic*					Create_Rigid_Dynamic_Grenade(class CModel* pModel, class CTransform* pTransform, _int* iId, class CGameObject* pObj);
	void									Cook_Terrain();
	void									Simulate();
	void									Cook_Mesh(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum, class CTransform* pTransform = nullptr, _int* pIndex = nullptr);
	void									Cook_Mesh_NoRotation(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum, class CTransform* pTransform = nullptr);
	void									Cook_Mesh_Dynamic(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum, vector<PxRigidDynamic*>* pColliders, vector<PxTransform>* pTransforms, class CTransform* pTransform = nullptr);
	void									Cook_Mesh_Convex(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum, vector<PxRigidDynamic*>* pColliders, vector<PxTransform>* pTransforms, class CTransform* pTransform = nullptr);
	void									Cook_Mesh_Convex_RigidDynamic(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum,PxRigidDynamic** pCollider, class CTransform* pTransform = nullptr);
	void									Cook_Mesh_Convex_Convert_Root(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum, vector<PxRigidDynamic*>* pColliders, vector<PxTransform>* pTransforms, class CTransform* pTransform ,_float4 vDelta);
	void									Cook_Mesh_Convex_Convert_Root_No_Rotate(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum, vector<PxRigidDynamic*>* pColliders, vector<PxTransform>* pTransforms, class CTransform* pTransform ,_float4 vDelta);
	void									Create_SoftBody(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum);
	void									Create_Cloth(_float3* pVertices, _uint* pIndices, _uint VertexNum, _uint IndexNum);
	_bool									RayCast(_float4 vOrigin, _float4 vDir, _float4* pBlockPoint, _float fMaxDist = 1000.f);
	_bool									RayCast_Shoot(_float4 vOrigin, _float4 vDir, _float4* pBlockPoint,_float4* pBlockNormal, _bool bBigAttack, _bool bDecalRay,_bool* IsHit_Props = nullptr, _float fMaxDist = 1000.f);
	_bool									RayCast_Decal(_float4 vOrigin, _float4 vDir, _float4* pBlockPoint,_float4* pBlockNormal, _float fMaxDist = 1000.f);
	_bool									RayCast_Effect(_float4 vOrigin, _float4 vDir, _float4* pBlockPoint,_float4* pBlockNormal,_bool bBigAttack,_bool* pDynamic, _float fMaxDist = 1000.f);
	_bool									SphereCast_Shoot(_float4 vOrigin, _float4 vDir, _float4* pBlockPoint, _float fMaxDist = 1000.f);
	_bool									SphereCast(_float4 vOrigin, _float4 vDir, _float4* pBlockPoint, _float fMaxDist = 1000.f);
	class CRagdoll_Physics*					Create_Ragdoll(vector<class CBone*>* vecBone,class CTransform* pTransform, const string& name);
	void									Start_Ragdoll(class CRagdoll_Physics* pRagdoll, _uint iId);
	void									Start_PartialRagdoll(class CRagdoll_Physics* pRagdoll, _uint iId, COLLIDER_TYPE eType);
	class CPxCollider*						Create_Px_Collider(class CModel* pModel, class CTransform* pTransform, _int* iId);
	class CPxCollider*						Create_Px_Collider_Convert_Root(class CModel* pModel, class CTransform* pTransform, _int* iId);
	class CPxCollider*						Create_Px_Collider_Convert_Root_Double_Door(class CModel* pModel, class CTransform* pTransform, _int* iId);
	class CPxCollider*						Create_Px_Collider_Cabinet(class CModel* pModel, class CTransform* pTransform, _int* iId);
	class CPxCollider*						Create_Px_Collider_Toilet(class CModel* pModel, class CTransform* pTransform, _int* iId);
	class CGraphic_Device*					GetGraphic_Device()
	{
		return m_pGraphic_Device;
	}
	vector<_float4>*						GetBlockPoints_Props();
	vector<_float4>*						GetBlockNormals_Props();

private:/*For Physics_Controller*/
	_uint*									m_pIndices = { nullptr };
	_int									NumVertices = { 0 };
	_int									NumIndices = { 0 };
	_float3*								TerrainPos = { nullptr };
#pragma endregion

#pragma region For Thread Pool
public:
	void									Insert_Job(function<void()> job);
	_bool									AllJobsFinished();
#pragma endregion

#pragma region For AI Controller
public:
	class CBehaviorTree*					Create_BehaviorTree(_uint* iId);
	class CPathFinder*						Create_PathFinder();
	void									Initialize_BehaviorTree(_uint* iId);
	void									SetNavCellPoint(vector<_float4>* pCellPoint)
	{
		m_vecNavCell_Point = pCellPoint;
	}
	vector<_float4>*						GetNavCellPoint()
	{
		return m_vecNavCell_Point;
	}
	void									SetCells(vector<class CCell*>* pCells)
	{
		m_Cells = pCells;
	}
	vector<class CCell*>* GetCells()
	{
		return m_Cells;
	}
private:
	vector<_float4>*						m_vecNavCell_Point;
	vector<class CCell*>*					m_Cells;

#pragma endregion

#pragma region For Graphic Device

#pragma endregion

#pragma region For Easing
public:
	_float Get_Ease(EASING_TYPE eEase, _float fCurValue, _float fTargetValue, _float fRatio);
#pragma endregion

#pragma region		For Animation Library

	_uint									Get_NumAnim_Prototypes(const wstring& strAnimLayerTag);
	HRESULT									Add_Prototypes_Animation(const wstring& strAnimLayerTag, const string& strDirPath);
	HRESULT									Clone_Animation(const wstring& strAnimLayerTag, _uint iAnimIndex, const vector<class CBone*>& Bones, CAnimation** ppAnimation);

	list<wstring>							Get_AnimationLayer_Tags();
	list<string>							Get_Animation_Tags(const wstring& setrAnimLayerTag);

#pragma endregion

#pragma region For Camera & Picking
public:
	void									Set_RayOrigin_Aim(_float4 vRayOrigin)
	{
		m_vRayOrigin_Aim = vRayOrigin;
	}
	void									Set_RayDir_Aim(_float4 vRayDir)
	{
		m_vRayDir_Aim = vRayDir;
	}
	_float4									Get_RayOrigin_Aim()
	{
		return m_vRayOrigin_Aim;
	}
	_float4									Get_RayDir_Aim()
	{
		return m_vRayDir_Aim;
	}
	void									Set_Camera_Transform(class CTransform* pTransform)
	{
		m_pCameraTransform = pTransform;
	}
	class CTransform*						Get_Camera_Transform()
	{
		return m_pCameraTransform;
	}

	void									Set_Gimmick_Camera_Transform(class CTransform* pTransform)
	{
		m_pGimmickCameraTransform = pTransform;
	}
	class CTransform* Get_Gimmick_Camera_Transform()
	{
		return m_pGimmickCameraTransform;
	}



	_float4									Get_Camera_Pos_Float4();
	_vector									Get_Camera_Pos_Vector();
#pragma endregion

#pragma region For Compute Shader Manager
	void									Bind_Essential_Resource_Skinning(_float4x4* pWorldMat, _float4x4* pBoneMatrices);
	void									Bind_Resource_Skinning(SKINNING_INPUT Input);
	void									Perform_Skinning(_uint iNumVertices);
	void									Bind_Resource_RayCasting(RAYCASTING_INPUT Input);
	void									Perform_RayCasting(_uint iNumTris);
	void									Bind_Resource_Calc_Decal_Info(CALC_DECAL_INPUT Input);
	void									Perform_Calc_Decal_Info();
	void									Bind_Resource_Calc_Decal_Map(CALC_DECAL_MAP_INPUT Input);
	void									Bind_Resource_Calc_Decal_Map_StaticModel(CALC_DECAL_MAP_INPUT_STATIC_MODEL Input);
	void									Perform_Calc_Decal_Map(_uint iNumVertices);
	void									Perform_Calc_Decal_Map_StaticModel(_uint iNumVertices);
#pragma endregion

#pragma region Event_Manager
public:
	HRESULT Add_Event(class CEvent* pNewEvent, const wstring& strEventTag);
	class CEvent* Get_Event(const wstring& strEventTag);

#pragma endregion

private:
	class CGraphic_Device*						m_pGraphic_Device = { nullptr };
	class CInput_Device*						m_pInput_Device = { nullptr };
	class CLevel_Manager*						m_pLevel_Manager = { nullptr };
	class CObject_Manager*						m_pObject_Manager = { nullptr };
	class CComponent_Manager*					m_pComponent_Manager = { nullptr };
	class CRenderer*							m_pRenderer = { nullptr };
	class CTimer_Manager*						m_pTimer_Manager = { nullptr };
	class CPipeLine*							m_pPipeLine = { nullptr };
	class CLight_Manager*						m_pLight_Manager = { nullptr };
	class CFont_Manager*						m_pFont_Manager = { nullptr };
	class CTarget_Manager*						m_pTarget_Manager = { nullptr };
	class CFrustum*								m_pFrustum = { nullptr };
	class CExtractor*							m_pExtractor = { nullptr };
	class CPhysics_Controller*					m_pPhysics_Controller = { nullptr };
	class CSound_Manager*						m_pSound_Manager = { nullptr };
	class CPicking*								m_pPicking = { nullptr };
	class CThread_Pool*							m_pThread_Pool = { nullptr };
	class CAIController*						m_pAIController = { nullptr };
	class CEasing*								m_pEasing = { nullptr };
	class CAnimation_Library*					m_pAnimation_Library = { nullptr };
	class CCompute_Shader_Manager*				m_pCS_Manager = { nullptr };
	class CEvent_Manager*						m_pEvent_Manager = { nullptr };
	/*for physics*/
	_bool										m_bSimulate = { false };

	//Random Number
	random_device							m_RandomDevice;
	mt19937_64								m_RandomNumber;

	class CGameObject*						m_pPlayer = { nullptr };
	_bool									m_bPause = { false };

	_float4									m_vRayOrigin_Aim;
	_float4									m_vRayDir_Aim;
	class CTransform*						m_pCameraTransform = { nullptr };
	class CTransform*						m_pGimmickCameraTransform = { nullptr };


public:
	static void Release_Engine();
	virtual void Free() override;
};

END
