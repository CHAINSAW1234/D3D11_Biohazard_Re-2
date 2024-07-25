#pragma once

#include "Component.h"
#include "Animation.h"
#include "PlayingInfo.h"

BEGIN(Engine)

#define ANIM_DEFAULT_LINEARTIME		0.2f
#define MAX_COUNT_BONE				512

#define DISTANCE_FPSMAX				5.f
#define DISTANCE_FPS45				10.f
#define DISTANCE_FPS30				13.f
#define DISTANCE_FPS20				16.f
#define DISTANCE_FPS10				20.f
#define DISTANCE_FPS5				25.f

#define TIME_FPSMAX					(fTimeDelta)
#define TIME_FPS45					(1.f / 45.f)
#define TIME_FPS30					(1.f / 30.f)
#define TIME_FPS20					(1.f / 20.f)
#define TIME_FPS10					(1.f / 10.f)
#define TIME_FPS5					(1.f / 5.f)
#define TIME_FPS1					(1.f)

class ENGINE_DLL CModel final : public CComponent
{
public:		/* For.Include */
#include "Model_Enums.h"
#include "Model_Struct.h"

public:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:		/* For.Animation */
	void									Add_AnimPlayingInfo(_bool isLoop, _uint iPlayingIndex, const wstring& strBoneLayerTag, _float fBlendWeight = 1.f);
	void									Erase_AnimPlayingInfo(_uint iPlayingIndex);

	_uint									Get_NumAnims(const wstring& strAnimLayerTag);
	_int									Get_CurrentAnimIndex(_uint iPlayingIndex);
	wstring									Get_CurrentAnimLayerTag(_uint iPlayingIndex);
	string									Get_CurrentAnimTag(_uint iPlayingIndex);
	_float4x4								Get_TransformationMatrix();

	_matrix									Get_FirstKeyFrame_Root_TransformationMatrix(const wstring& strAnimLayerTag, _int iAnimIndex);
	_matrix									Get_FirstKeyFrame_Root_CombinedMatrix(const wstring& strAnimLayerTag, _int iAnimIndex);
	_matrix									Get_CurrentKeyFrame_Root_CombinedMatrix(_uint iPlayingIndex);

	void									Reset_PreAnimation(_uint iPlayingIndex);
	void									Reset_PreAnim_CurrentAnim(_uint iPlayingIndex);

	HRESULT									Add_Animations(const wstring& strPrototypeLayerTag, const wstring& strAnimLayerTag);


	HRESULT									Add_Bones(CModel* pModel);

public:		/* Optimization Culling */
	void									Set_OptimizationCulling(_bool isSetOptimization) { m_isOptimization = isSetOptimization; }
	
private:	/* Optimization Culling */
	_float									Compute_NewTimeDelta_Distatnce_Optimization(_float fTimeDelta, class CTransform* pTransform);

public:		/* For.Controll AnimSpeed */
	void									Set_TickPerSec(const wstring& strAnimLayerTag, _uint iAnimIndex, _float fTickPerSec);

public:		/* For.RootAnimaition ActiveControll */
	_bool									Is_Active_RootMotion_XZ();
	_bool									Is_Active_RootMotion_Y();
	_bool									Is_Active_RootMotion_Rotation();

	void									Active_RootMotion_XZ(_bool isActive);
	void									Active_RootMotion_Y(_bool isActive);
	void									Active_RootMotion_Rotation(_bool isActive);
	void									Set_RootBone(string strBoneTag);			//	모든본을 초기화 => 루트본은 하나다 가정후 찾은 본을 루트본으로 등록

	_bool									Is_Set_RootBone();
	void									Update_LastKeyFrames(const vector<_float4x4>& TransformationMatrices, _uint iPlayingIndex);

private:	/* Utillity */
	_vector									Compute_Quaternion_From_TwoDirection(_fvector vSrcDirection, _fvector vDstDirection);

public:		/* For.Bone_Layer */
	void									Add_Bone_Layer_Range(const wstring& strLayerTag, const string& strStartBoneTag, const string& strEndBoneTag);
	void									Add_Bone_Layer_Range(const wstring& strLayerTag, _uint iStartBoneIndex, _uint iEndBoneIndex);

	void									Add_Bone_Layer_ChildIndices(const wstring& strLayerTag, const string& strBoneTag);
	void									Add_Bone_Layer_ChildIndices(const wstring& strLayerTag, _uint iParentBoneIndex);

	void									Add_Bone_Layer_BoneIndices(const wstring& strLayerTag, const list<_uint>& BoneIndices);

	void									Add_Bone_Layer_All_Bone(const wstring& strLayerTag);
	void									Erase_Bone_Layer(const wstring& strLayerTag);

	void									Add_Bone_Layer_Bone(const wstring& strBoneLayerTag, _uint iBoneIndex);
	void									Add_Bone_Layer_Bone(const wstring& strBoneLayerTag, const string& strBoneTag);

public:		/* For.IK Public*/
	void									Add_IK(const string& strTargetJointTag, const string& strEndEffectorTag, const wstring& strIKTag, _uint iNumIteration, _float fBlend);
	void									Erase_IK(const wstring& strIKTag);
	void									Set_TargetPosition_IK(const wstring& strIKTag, _fvector vTargetPosition);
	void									Set_NumIteration_IK(const wstring& strIKTag, _uint iNumIteration);
	void									Set_Blend_IK(const wstring& strIKTag, _float fBlend);
	vector<_float4>							Get_ResultTranslation_IK(const wstring& strIKTag);
	vector<_float4>							Get_OriginTranslation_IK(const wstring& strIKTag);
	vector<_float4x4>						Get_JointCombinedMatrices_IK(const wstring& strIKTag);



public:		/* For.Additioal_Input_Forces */
	void									Add_Additional_Transformation_World(string strBoneTag, _fmatrix AdditionalTransformMatrix);

private:	/* For.Additioal_Input_Forces */
	void									Apply_AdditionalForces(class CTransform* pTransform);
	void									Apply_AdditionalForce(_uint iBoneIndex);
	void									Clear_AdditionalForces();

private:
	_int									Find_RootBoneIndex();

private:
	vector<_float4x4>						Initialize_ResultMatrices_Blend(const unordered_set<_uint>& IncludedBoneIndices);
	vector<_float4x4>						Initialize_ResultMatrices_AdditionalMasking(const unordered_set<_uint>& IncludedBoneIndices);
	_float									Compute_Current_TotalBlendWeight(_uint iBoneIndex);
	void									Compute_Current_TotalBlendWeights();
	_float4x4								Compute_BlendTransformation_Additional(_fmatrix SrcMatrix, _fmatrix DstMatrix, _float fAdditionalWeight);
	unordered_set<_uint>					Compute_IncludedBoneIndices_AllBoneLayer_Blend();
	unordered_set<_uint>					Compute_IncludedBoneIndices_AllBoneLayer_AdditionalMasking();

public:
	HRESULT									RagDoll();
	vector<class CBone*>*					GetBoneVector() { return &m_Bones; }

public:		/*For Physics Collider*/
	_float4x4								GetBoneTransform(string strBoneTag);
	_float4x4								GetBoneTransform(_int Index);

private:
	void									Apply_RootMotion_Rotation(class CTransform* pTransform);
	void									Apply_RootMotion_Translation(class CTransform* pTransform, _float3* pMovedDirection);
	void									Apply_Translation_Inverse_Rotation(_fvector vTranslation);

public:
	void									Set_CombinedMatrix(string strBoneTag, _fmatrix CombinedMatrix);
	void									Set_Parent_CombinedMatrix_Ptr(string strBoneTag, _float4x4* pParentMatrix);
	void									Set_Surbodinate(string strBoneTag, _bool isSurbodinate);

public:		/* For. MeshControll */
	_bool									Is_Hide_Mesh(string strMeshTag);
	_bool									Is_Hide_Mesh(_uint iMeshIndex);

	void									Hide_Mesh_Branch(_uint iMeshBranch, _bool isHide);
	void									Hide_Mesh(_uint iMeshIndex, _bool isHide);
	void									Hide_Mesh(string strMeshTag, _bool isHide);

	_int									Find_Mesh_Index(string strMeshTag);
	string									Find_Mesh_Tag(_uint iMeshIndex);

	list<_uint>								Get_NonHideMeshIndices();

public: /*For. Mesh pos*/
	_float4									Get_Mesh_Local_Pos(string strMeshTag);




public:		/* For. Access */
	vector<string>							Get_BoneNames();
	vector<_float4>							Get_Translations();
	vector<string>							Get_MeshTags();
	list<_uint>								Get_MeshIndices(const string& strMeshTag);		//	이름이 같은 메쉬들이 각각의 칸에있을수있으므로 인덱스들을 컨테이너에 담아서 반환한다.
	vector<CAnimation*>						Get_Animations(const wstring& strAnimLayerTag);
	map<wstring, class CBone_Layer*>		Get_BoneLayers() { return m_BoneLayers; }
	list<wstring>							Get_BoneLayer_Tags();
	list<wstring>							Get_AnimationLayer_Tags();
	list<string>							Get_Animation_Tags(const wstring& strAnimLayerTag);
	_int									Get_Mesh_Branch(_uint iMeshIndex);
	_int									Get_Mesh_Branch(const string& strMeshTag);

	_uint									Get_NumBones() { return static_cast<_uint>(m_Bones.size()); }
	_uint									Get_NumMeshes() const { return m_iNumMeshes; }
	_uint									Get_NumPlayingInfos() { return static_cast<_uint>(m_PlayingAnimInfos.size()); }

	_bool									Find_AnimIndex(_int* pAnimIndex, wstring* pAnimLayerTag, CAnimation* pAnimation);
	_bool									Find_AnimIndex(_int* pAnimIndex, wstring* pAnimLayerTag, const string& strAnimTag);
	string									Find_RootBoneTag();
	class CPlayingInfo*						Find_PlayingInfo(_uint iPlayingIndex);
	class CBone_Layer*						Find_BoneLayer(const wstring& strBoneLayerTag);

	HRESULT									Link_Bone_Auto(CModel* pTargetModel);
	HRESULT									Link_Bone_Auto_RagDoll(CModel* pTargetModel, class CRagdoll_Physics* pRagDoll);

	_int									Get_BoneIndex(const string& strBoneTag);

	_float4									Invalidate_RootNode(const string& strRoot);

	_bool									Is_Surbodinate_Bone(const string& strBoneTag);
	_bool									Is_Root_Bone(const string& strBoneTag);

	_uint									Get_CurrentMaxKeyFrameIndex(_uint iPlayingIndex);
	_float									Get_Duration_From_Anim(const wstring& strAnimLayerTag, _uint iAnimIndex);
	_float									Get_Duration_From_Anim(const wstring& strAnimLayerTag, const string& strAnimTag);
	_float									Get_Duration_From_PlayingInfo(_uint iPlayingIndex);
	_float									Get_TickPerSec_From_Anim(const wstring& strAnimLayerTag, _uint iAnimIndex);
	_float									Get_TickPerSec_From_Anim(const wstring& strAnimLayerTag, const string& strAnimTag);
	_float									Get_TickPerSec_From_PlayingInfo(_uint iPlayingIndex);
	_float									Get_TrackPosition(_uint iPlayingIndex);
	_float									Get_BlendWeight(_uint iPlayingIndex);
	_int									Get_AnimIndex_PlayingInfo(_uint iPlayingIndex);
	wstring									Get_BoneLayerTag_PlayingInfo(_uint iPlayingIndex);
	_int									Get_PlayingIndex_From_BoneLayerTag(wstring strBoneLayerTag);
	list<_uint>								Get_Created_PlayingInfo_Indices();

	_bool									Is_Loop_PlayingInfo(_uint iPlayingIndex);
	void									Set_Loop(_uint iPlayingIndex, _bool isLoop);

	void									Set_TotalLinearInterpolation(_float fTime) { m_fTotalLinearTime = fTime; }
	void									Set_Additional_Masking(_uint iPlayingIndex, _bool isAdditionalMasking, _uint iNumNeedKeyFrame);
	void									Set_KeyFrameIndex_AllKeyFrame(_uint iPlayingIndex, _uint iKeyFrameIndex);
	void									Set_TrackPosition(_uint iPlayingIndex, _float fTrackPosition, _bool isResetRootPre = false);
	void									Set_BlendWeight(_uint iPlayingIndex, _float fBlendWeight, _float fLinearTime = 0.f);
	void									Change_Animation(_uint iPlayingIndex, const wstring& strAnimLayerTag, _uint iAnimIndex);
	void									Change_Animation(_uint iPlayingIndex, const wstring& strAnimLayerTag, const string& strAnimTag);
	void									Set_BoneLayer_PlayingInfo(_uint iPlayingIndex, const wstring& strBoneLayerTag);
	void									Set_Mesh_Branch(const string& strMeshTag, _uint iMeshBranch);
	void									Set_Mesh_Branch(_uint iMeshIndex, _uint iMeshBranch);

	class CBone*							Get_BonePtr(const _char* pBoneName) const;
	class CBone*							Get_BonePtr(_int iIndex);

	_bool									isFinished(_uint iPlayingIndex);
	void									Get_Child_BoneIndices(string strTargetParentsBoneTag, list<_uint>& ChildBoneIndices);
	void									Get_Child_ZointIndices(string strStartBoneTag, const string& strEndBoneTag, list<_uint>& ChildZointIndices);

	const _float4x4*						Get_CombinedMatrix(const string& strBoneTag);

public:		/* For.FBX */
	virtual HRESULT							Initialize_Prototype_TEMP(MODEL_TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix);

public:		/* For.FBX */
	virtual HRESULT							Initialize_Prototype(MODEL_TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix);

public:		/* For.Binary */
	virtual HRESULT							Initialize_Prototype(const string& strModelFilePath, _fmatrix TransformMatrix);
	virtual HRESULT							Initialize(void* pArg) override;

public:
	HRESULT									Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT									Bind_BoneMatrices_Ragdoll(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex,_float4x4* pBoneMatrices);
	HRESULT									Bind_BoneMatrices_Ragdoll_PartObject_L_Arm(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT									Bind_BoneMatrices_Ragdoll_PartObject_R_Arm(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT									Bind_BoneMatrices_Ragdoll_PartObject_L_Leg(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT									Bind_BoneMatrices_Ragdoll_PartObject_R_Leg(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT									Bind_PrevBoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT									Bind_ShaderResource_Texture(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType);
	HRESULT									Bind_ShaderResource_MaterialDesc(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);

	HRESULT									Play_Animations(class CTransform* pTransform, _float fTimeDelta, _float3* pMovedDirection);
	HRESULT									Play_Animation_Light(class CTransform* pTransform, _float fTimeDelta);
	HRESULT									Play_Pose(_uint iPlayingIndex);
	HRESULT									Play_T_Pose();
	HRESULT									Play_IK(class CTransform* pTransform, _float fTimeDelta);
	HRESULT									Render(_uint iMeshIndex);

private:
	vector<_float4x4>						Apply_Animation(_float fTimeDelta, _uint iPlayingAnimIndex);
	void									Apply_Bone_CombinedMatrices(CTransform* pTransform, _float3* pMovedDirection, _uint iStartBoneIndex = 0);
	void									Apply_Bone_TransformMatrices(const vector<vector<_float4x4>>& BlendTransformationMatricesLayer, const vector<vector<_float4x4>>& AdditionalTransformationMatricesLayer);
	vector<_float4x4>						Compute_ResultMatrices_Blend(const vector<vector<_float4x4>>& TransformationMatricesLayer);
	vector<_float4x4>						Compute_ResultMatrices_AdditionalMsking(const vector<vector<_float4x4>>& AdditionalMaskingTransformationMatricesLayer);

public:		/* For.Cooking_Mesh */
	void									Static_Mesh_Cooking(class CTransform* pTransform = nullptr,_int* pIndex = nullptr);
	void									Dynamic_Mesh_Cooking(vector<PxRigidDynamic*>* pColliders, vector<PxTransform>* pTransforms, class CTransform* pTransform = nullptr);
	void									Convex_Mesh_Cooking(vector<PxRigidDynamic*>* pColliders,vector<PxTransform>* pTransforms,class CTransform* pTransform = nullptr);
	void									Convex_Mesh_Cooking_RigidDynamic(PxRigidDynamic** pCollider,_int iId,class CTransform* pTransform = nullptr);
	void									Convex_Mesh_Cooking_RigidDynamic_Grenade(PxRigidDynamic** pCollider,class CTransform* pTransform = nullptr);
	void									Convex_Mesh_Cooking_Convert_Root(vector<PxRigidDynamic*>* pColliders,vector<PxTransform>* pTransforms,class CTransform* pTransform = nullptr);
	void									Convex_Mesh_Cooking_Convert_Root_Double_Door(vector<PxRigidDynamic*>* pColliders,vector<PxTransform>* pTransforms,class CTransform* pTransform = nullptr);
	void									Convex_Mesh_Cooking_Convert_Root_Double_Door_No_Rotate(vector<PxRigidDynamic*>* pColliders,vector<PxTransform>* pTransforms,class CTransform* pTransform = nullptr);
	void									Convex_Mesh_Cooking_Cabinet(vector<PxRigidDynamic*>* pColliders,vector<PxTransform>* pTransforms,class CTransform* pTransform = nullptr);
	void									Convex_Mesh_Cooking_Toilet(vector<PxRigidDynamic*>* pColliders,vector<PxTransform>* pTransforms,class CTransform* pTransform = nullptr);
	void									Create_SoftBody();
	void									Create_Cloth();

private:
	_int									Find_BoneIndex(const string& strBoneTag);
	void									Compute_RootParentsIndicies(_uint iRootIndex, vector<_uint>& ParentsIndices);

public:
	const									MODEL_TYPE& Get_ModelType() { return m_eModelType; }

private:	/* For.Linear Interpolation */
	void									Motion_Changed(_uint iPlayingIndex);

private:	/* For.Linear Interpolation */
	void									Update_LinearInterpolation(_float fTimeDelta, _uint iPlayingIndex);

#pragma region 로컬 트렌스폼 제어
public:		/*For. Local_Control*/
	void									Set_Local_State(LOCAL_STATE eState, const _float4& vState) {
		memcpy(&m_TransformationMatrix.m[eState], &vState, sizeof(_float4));

	}

	void									Set_Local_State(LOCAL_STATE eState, _fvector vState) {
		_float4		vTemp;
		XMStoreFloat4(&vTemp, vState);
		memcpy(&m_TransformationMatrix.m[eState], &vTemp, sizeof(_float4));
	}

	void									Set_Local_Scaled(_float fScaleX, _float fScaleY, _float fScaleZ);

	inline void								Set_LocalMatrix(_float4x4 WorldMatrix) {
		m_TransformationMatrix = WorldMatrix;
	}

	inline void								Set_LocaldMatrix(_matrix WorldMatrix) {
		XMStoreFloat4x4(&m_TransformationMatrix, WorldMatrix);
	}

	_vector									Get_Local_State_Vector(LOCAL_STATE eState) {
		return XMLoadFloat4x4(&m_TransformationMatrix).r[eState];
	}

	_float4									Get_Local_State_Float4(LOCAL_STATE eState) {
		_float4		vTemp;
		XMStoreFloat4(&vTemp, XMLoadFloat4x4(&m_TransformationMatrix).r[eState]);
		return vTemp;
	}

	_float3					Get_Local_Scaled() const {
		_matrix		WorldMatrix = XMLoadFloat4x4(&m_TransformationMatrix);
		return _float3(XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_LOCAL_RIGHT])),
			XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_LOCAL_UP])),
			XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_LOCAL_LOOK])));
	}
#pragma endregion


private:
	MODEL_TYPE								m_eModelType = { TYPE_END };
	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer						m_Importer;

private:
	_uint									m_iNumMeshes = { 0 };
	vector<class CMesh*>					m_Meshes;
	vector<_bool>							m_IsHideMesh;

private:	/* For.RootMotion */
	_bool									m_isRootMotion_XZ = { false };
	_bool									m_isRootMotion_Y = { false };
	_bool									m_isRootMotion_Rotation = { false };

private:
	_uint									m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>					m_Materials;

	_float4x4								m_TransformationMatrix;
	vector<_float4x4>						m_T_Pose_Matrices;

	vector<class CBone*>					m_Bones;
	map<wstring, class CBone_Layer*>		m_BoneLayers;			//	레이어로 분리할 뼈들의 태그별로 인덱스들을 저장한다.

	class CIK_Solver* m_pIK_Solver = { nullptr };

	_uint									m_iNumAnimations = { 0 };
	unordered_map<wstring, class CAnimation_Layer*>			m_AnimationLayers;

	_float4x4								m_MeshBoneMatrices[MAX_COUNT_BONE];

private:	/* For.Blend_Animation */
	vector<class CPlayingInfo*>				m_PlayingAnimInfos;

private: /* For.Additional_Input_Forces */
	//	뼈의 인덱스 별로 외부적 변환을 추가적으로 적용한다.
	//	머리뼈 흔들거나할 떄 사용해보기
	vector<list<_float4x4>>					m_AdditionalForces;

private:	/* For.Linear_Interpolation */
	_float									m_fTotalLinearTime = { 0.f };

private:	/* Distance_Optimization */
	_float									m_fOptimizationFrame = { 60.f };
	_float									m_fAccOptimizationTime = { 0.f };
	_bool									m_isOptimization = { true };

private:
	vector<_float>							m_TotalWeights;

private:
	vector<_int>							m_MeshBranches;

private:	/* For.FBX_Load */
	HRESULT									Ready_Meshes(const map<string, _uint>& BoneIndices);
	HRESULT									Ready_Materials(const _char* pModelFilePath);
	HRESULT									Ready_Bones(aiNode* pAINode, map<string, _uint>& BoneIndices, _int iParentIndex = -1);
	HRESULT									Ready_Animations(const map<string, _uint>& BoneIndices);

private:	/* For.Binary_Load */
	HRESULT									Ready_Meshes(ifstream& ifs);
	HRESULT									Ready_Materials(ifstream& ifs);
	HRESULT									Ready_Bones(ifstream& ifs);
	HRESULT									Ready_Animations(ifstream& ifs);

public:		/* For Octree Culling */
	vector<class CMesh*>* GetMeshes()
	{
		return  &m_Meshes;
	}
	_uint									GetNumMesh()
	{
		return m_iNumMeshes;
	}
	void									SetNumMesh(_uint NumMesh)
	{
		m_iNumMeshes = NumMesh;
	}
	void									Release_IndexBuffer(_uint iNumMesh);
	void									Release_Dump();
	_float4									GetCenterPoint()
	{
		return m_vCenterPoint;
	}
private:
	_float4									m_vCenterPoint;

public:/*For Skinned Mesh Decal*/
	void									Bind_Resource_Skinning(_uint iIndex);
	void									Bind_Essential_Resource_Skinning(_float4x4* pWorldMat);
	void									Staging_Skinning(_uint iIndex);
	void									Perform_Skinning(_uint iIndex);
	void									SetDecalWorldMatrix(_uint iIndex,_float4x4 WorldMatrix,_int iMeshType,_bool bBigAttack);
	void									SetDecalWorldMatrix_Player_Front(_float4x4 WorldMatrix);
	void									SetDecalWorldMatrix_Player_Back(_float4x4 WorldMatrix);
	void									InitDecalWorldMatrix(_float4 vPos,_float4 vNormal);
	void									Init_Decal(_uint iLevel);
	void									Bind_Resource_NonCShader_Decal(_uint iIndex,class CShader* pShader);
	void									Calc_DecalMap_NonCS(class CShader* pShader,_int iMeshType);

	void									Initialize_DecalMap();
public:/*For Mesh RayCasting*/
	_uint									Perform_RayCasting(_uint iIndex, AddDecalInfo Info,_float* pDist);

public:/*For Calc Decal Info*/
	void									Perform_Calc_DecalInfo(_uint iIndex);

public:/*For Decal Map*/
	void									Bind_Resource_DecalMap(_uint iIndex, class CShader* pShader);
	void									Perform_Init_DecalMap(_uint iIndex, class CShader* pShader);


public:/*For Calc Decal Map*/
	void									Perform_Calc_DecalMap(_int iMeshType);
	void									Perform_Calc_DecalMap_Player();
	void									Perform_Calc_DecalMap_StaticModel();
	void									Bind_DecalMap(_uint iIndex,class CShader* pShader);

	
public:/*Release Dump*/
	void									Release_Decal_Dump();

private:/*For Decal Map*/
	vector<ID3D11UnorderedAccessView*>		m_vecUAV_DecalMap;
	vector<_float2*>						m_vecDecal_Map = { nullptr };
	vector<ID3D11Buffer*>					m_vecSB_DecalMap;
	vector<ID3D11ShaderResourceView*>		m_vecSRV_DecalMap;
public:
	/* Create_ */
	static CModel* Create_Temp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix);

	/* Create_From_FBX */
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix);
	/* Create_From_Binary */
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string& strModelFilePath, _fmatrix TransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;


private:
	static unordered_map<wstring, class CTexture*>			m_LoadedTextures;
};

END