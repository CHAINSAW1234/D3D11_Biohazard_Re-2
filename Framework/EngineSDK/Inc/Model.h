#pragma once

#include "Component.h"
#include "Animation.h"

BEGIN(Engine)

#define ANIM_DEFAULT_LINEARTIME 0.2f

class ENGINE_DLL CModel final : public CComponent
{
public:
#include "Model_Struct.h"

public:
#include "Model_Enums.h"

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const { return m_iNumMeshes; }
	class CBone* Get_BonePtr(const _char* pBoneName) const;

	_bool isFinished(_uint iPlayingIndex);
	void Get_Child_BoneIndices(string strTargetParentsBoneTag, list<_uint>& ChildBoneIndices);

public:	/* For.Animation */
	//	void Set_Animation(_uint iAnimIndex, _bool isLoop);
	void Set_Animation_Blend(ANIM_PLAYING_DESC AnimDesc, _uint iPlayingIndex);

	_uint Get_NumAnims() { return m_iNumAnimations; }
	_uint Get_CurrentAnimIndex(_uint iPlayingIndex);

	/* For.Controll AnimSpeed */
	void Set_TickPerSec(_uint iAnimIndex, _float fTickPerSec);
	void Set_Weight(_uint iPlayingIndex, _float fWeight);

	/* For.RootAnimaition ActiveControll */
	_bool Is_Active_RootMotion_XZ();
	_bool Is_Active_RootMotion_Y();
	_bool Is_Active_RootMotion_Rotation();

	void Active_RootMotion_XZ(_bool isActive);
	void Active_RootMotion_Y(_bool isActive);
	void Active_RootMotion_Rotation(_bool isActive);
	void Set_RootBone(string strBoneTag);			//	모든본을 초기화 => 루트본은 하나다 가정후 찾은 본을 루트본으로 등록

public:
	void Add_IK(string strTargetJointTag, string strEndEffectorTag, wstring strIKTag, _uint iNumIteration, _float fBlend);
	void Set_Direction_IK(wstring strIKTag, _fvector vDirection);
	void Set_NumIteration_IK(wstring strIKTag, _uint iNumIteration);
	void Set_Blend_IK(wstring strIKTag, _float fBlend);
	
private:
	void Apply_IK(class CTransform* pTransform, IK_INFO& IkInfo);
	void Update_Distances_Translations_IK(IK_INFO& IkInfo);
	void Update_Forward_Reaching_IK(IK_INFO& IkInfo);
	void Update_Backward_Reaching_IK(IK_INFO& IkInfo);
	void Update_TransformMatrices_BoneChain(IK_INFO& IkInfo);
	void Update_CombinedMatrices_BoneChain(IK_INFO& IkInfo);

public:		/* For.Bone_Layer */
	void Add_Bone_Layer(const wstring& strLayerTag, list<_uint> BoneIndices);
	void Add_Bone_Layer_All_Bone(const wstring& strLayerTag);
	void Delete_Bone_Layer(const wstring& strLayerTag);

public:		/* For.Additioal_Input_Forces */
	void Add_Additional_Transformation_World(string strBoneTag, _fmatrix AdditionalTransformMatrix);

private:	/* For.Additioal_Input_Forces */
	void Apply_AdditionalForces(class CTransform* pTransform);
	void Apply_AdditionalForce(_uint iBoneIndex);
	void Clear_AdditionalForces();

private:
	_int Find_RootBoneIndex();

private:
	vector<_float4x4> Initialize_ResultMatrices(const set<_uint> IncludedBoneIndices);

	_float Compute_Current_TotalWeight(_uint iBoneIndex);
	_float4x4 Compute_BlendTransformation_Additional(_fmatrix SrcMatrix, _fmatrix DstMatrix, _float fAdditionalWeight);

public:
	//	HRESULT Play_Animation_Separation(class CTransform* pTransform, _float fTimeDelta, _float3* pMovedDirection);
	//	HRESULT RagDoll();
	HRESULT Play_Animation_Separation(class CTransform* pTransform, _float fTimeDelta, _float3* pMovedDirection);
	HRESULT RagDoll();
	vector<class CBone*>* GetBoneVector()
	{
		return &m_Bones;
	}
public:/*For Physics Collider*/
	_float4x4 GetBoneTransform(string strBoneTag);
	_float4x4 GetBoneTransform(_int Index);

private:
	void Apply_RootMotion_Rotation(class CTransform* pTransform, _fvector vQuaternion);
	void Apply_RootMotion_Rotation(class CTransform* pTransform, _fvector vQuaternion, _float4* pTranslation);
	void Apply_RootMotion_Translation(class CTransform* pTransform, _fvector vTranslation, _float3* pMovedDirection, _bool isActiveRotation, _fvector vQuaternion = XMQuaternionIdentity());
	void Apply_Translation_Inverse_Rotation(_fvector vTranslation);

public:
	void Set_CombinedMatrix(string strBoneTag, _fmatrix CombinedMatrix);
	void Set_Parent_CombinedMatrix_Ptr(string strBoneTag, _float4x4* pParentMatrix);
	void Set_Surbodinate(string strBoneTag, _bool isSurbodinate);

public:
	vector<string> Get_BoneNames();
	vector<_float4>	Get_Translations();
	set<string>	Get_MeshTags();
	list<_uint>	Get_MeshIndices(const string& strMeshTag);		//	이름이 같은 메쉬들이 각각의 칸에있을수있으므로 인덱스들을 컨테이너에 담아서 반환한다.

public:
	MATERIAL_DESC Get_Mesh_MaterialDesc(_uint iMeshIndex);
	void Set_Mesh_MaterialDesc(_uint iMeshIndex, const MATERIAL_DESC& MaterialDesc);

	_float4 Invalidate_RootNode(const string& strRoot);

public:
	//	For.FBX
	virtual HRESULT Initialize_Prototype(MODEL_TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix);
	//	For.Binary
	virtual HRESULT Initialize_Prototype(const string& strModelFilePath, _fmatrix TransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

private:
	_float3 Invalidate_BonesCombinedMatix_Translation(_int iRootIndex = -1);
	_float4x4 Invalidate_BonesCombinedMatix_TranslationMatrix(_int iRootIndex = -1);

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT Bind_ShaderResource_Texture(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType);
	HRESULT Bind_ShaderResource_MaterialDesc(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);

	HRESULT Play_Animations(_float fTimeDelta);
	HRESULT Play_Animations_RootMotion(class CTransform* pTransform, _float fTimeDelta, _float3* pMovedDirection);
	HRESULT Render(_uint iMeshIndex);

private:
	vector<_float4x4> Apply_Animation(_float fTimeDelta, set<_uint>& IncludedBoneIndices, _uint iPlayingAnimIndex);
	void Apply_Bone_CombinedMatrices(CTransform* pTransform, _float3* pMovedDirection);
	void Apply_Bone_TransformMatrices(const vector<vector<_float4x4>>& TransformationMatricesLayer, const set<_uint>& IncludedBoneIndices);
	vector<_float4x4> Compute_ResultMatrices(const vector<vector<_float4x4>>& TransformationMatricesLayer, const set<_uint>& IncludedBoneIndices);

public:
	void Static_Mesh_Cooking();

public:
	const _float4x4* Get_CombinedMatrix(const string& strBoneTag);

private:
	_int Find_BoneIndex(const string& strBoneTag);
	void Compute_RootParentsIndicies(_uint iRootIndex, vector<_uint>& ParentsIndices);

public:	/* For.KeyFrame */
	_uint Get_CurrentMaxKeyFrameIndex(_uint iPlayingIndex);
	_float Get_Duration(_uint iPlayingIndex, _int iAnimIndex = -1);
	_float Get_TrackPosition(_uint iPlayingIndex);
	void Set_KeyFrameIndex(_uint iPlayingIndex, _uint iKeyFrameIndex);
	void Set_TrackPosition(_uint iPlayingIndex, _float fTrackPosition);
	const vector<_uint>& Get_CurrentKeyFrameIndices(_uint iPlayingIndex);

public:
	const MODEL_TYPE& Get_ModelType() { return m_eModelType; }

private:	/* For.Linear Interpolation */
	void Motion_Changed(_uint iPlayingIndex);
	void Update_LastKeyFrames(const vector<_float4x4>& TransformationMatrices, vector<KEYFRAME>& LastKeyFrames);
	void Update_LastKeyFrames_Bones(vector<KEYFRAME>& LastKeyFrames);

private:	/* For.Linear Interpolation */
	void Update_LinearInterpolation(_float fTimeDelta, _uint iPlayingIndex);
	void Reset_LinearInterpolation(_uint iPlayingIndex);

private:
	MODEL_TYPE					m_eModelType = { TYPE_END };
	const aiScene*				m_pAIScene = { nullptr };
	Assimp::Importer			m_Importer;

private:
	_uint						m_iNumMeshes = { 0 };
	vector<class CMesh*>		m_Meshes;

private:	/* For.RootMotion */
	_bool						m_isRootMotion_XZ = { false };
	_bool						m_isRootMotion_Y = { false };
	_bool						m_isRootMotion_Rotation = { false };

private:	/* For.Inverse_Kinematic */
	map<wstring, IK_INFO>		m_IKInfos;

private:
	_uint						m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>		m_Materials;

	_float4x4					m_TransformationMatrix;

	vector<class CBone*>				m_Bones;
	map<wstring, class CBone_Layer*>	m_BoneLayers;			//	레이어로 분리할 뼈들의 태그별로 인덱스들을 저장한다.

	_uint						m_iNumAnimations = { 0 };
	vector<class CAnimation*>	m_Animations;

	_float4x4					m_MeshBoneMatrices[512];

private:	/* For.Blend_Animation */
	vector<ANIM_PLAYING_INFO>	m_PlayingAnimInfos;

private: /* For.Additional_Input_Forces */
	//	뼈의 인덱스 별로 외부적 변환을 추가적으로 적용한다.
	//	머리뼈 흔들거나할 떄 사용해보기
	vector<list<_float4x4>>		m_AdditionalForces;

private:	/* For.Linear_Interpolation */
	_float						m_fTotalLinearTime = { 0.f };

private: /* For.FBX_Load */
	HRESULT Ready_Meshes(const map<string, _uint>& BoneIndices);
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(aiNode* pAINode, map<string, _uint>& BoneIndices, _int iParentIndex = -1);
	HRESULT Ready_Animations(const map<string, _uint>& BoneIndices);

private: /* For.Binary_Load */
	HRESULT Ready_Meshes(ifstream& ifs);
	HRESULT Ready_Materials(ifstream& ifs);
	HRESULT Ready_Bones(ifstream& ifs);
	HRESULT Ready_Animations(ifstream& ifs);

public:
	//	fbx읽기
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix);
	//	바이너리 데이터 읽기
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string& strModelFilePath, _fmatrix TransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END