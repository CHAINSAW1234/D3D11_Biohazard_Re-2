#pragma once

#include "Component.h"
#include "Animation.h"
#include "PlayingInfo.h"

BEGIN(Engine)

#define ANIM_DEFAULT_LINEARTIME		0.2f
#define MAX_COUNT_BONE				256

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
	void									Add_AnimPlayingInfo(_uint iAnimIndex, _bool isLoop, _uint iPlayingIndex, const wstring& strBoneLayerTag, _float fBlendWeight = 1.f);
	void									Erase_AnimPlayingInfo(_uint iPlayingIndex);

	_uint									Get_NumAnims() { return m_iNumAnimations; }
	_int									Get_CurrentAnimIndex(_uint iPlayingIndex);
	string									Get_CurrentAnimTag(_uint iPlayingIndex);

public:		/* For.Controll AnimSpeed */
	void									Set_TickPerSec(_uint iAnimIndex, _float fTickPerSec);

public:		/* For.RootAnimaition ActiveControll */
	_bool									Is_Active_RootMotion_XZ();
	_bool									Is_Active_RootMotion_Y();
	_bool									Is_Active_RootMotion_Rotation();

	void									Active_RootMotion_XZ(_bool isActive);
	void									Active_RootMotion_Y(_bool isActive);
	void									Active_RootMotion_Rotation(_bool isActive);
	void									Set_RootBone(string strBoneTag);			//	��纻�� �ʱ�ȭ => ��Ʈ���� �ϳ��� ������ ã�� ���� ��Ʈ������ ���

	_bool									Is_Set_RootBone();
	void									Update_LastKeyFrames(const vector<_float4x4>& TransformationMatrices, _uint iPlayingIndex);

private:	/* Utillity */
	_vector									Compute_Quaternion_From_TwoDirection(_fvector vSrcDirection, _fvector vDstDirection);

public:		/* For.Bone_Layer */
	void									Add_Bone_Layer_Range(const wstring& strLayerTag, const string& strStartBoneTag, const string& strEndBoneTag);
	void									Add_Bone_Layer_Range(const wstring& strLayerTag, _uint iStartBoneIndex, _uint iEndBoneIndex);

	void									Add_Bone_Layer_ChildIndices(const wstring& strLayerTag, const string& strBoneTag);
	void									Add_Bone_Layer_ChildIndices(const wstring& strLayerTag, _uint iParentBoneIndex);

	void									Add_Bone_Layer_All_Bone(const wstring& strLayerTag);
	void									Erase_Bone_Layer(const wstring& strLayerTag);

public:		/* For.IK Public*/
	void									Add_IK(string strTargetJointTag, string strEndEffectorTag, const wstring& strIKTag, _uint iNumIteration, _float fBlend);
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
	vector<_float4x4>						Initialize_ResultMatrices(const set<_uint> IncludedBoneIndices);
	_float									Compute_Current_TotalWeight(_uint iBoneIndex);
	vector<_float>							Compute_Current_TotalWeights();
	_float4x4								Compute_BlendTransformation_Additional(_fmatrix SrcMatrix, _fmatrix DstMatrix, _float fAdditionalWeight);
	set<_uint>								Compute_IncludedBoneIndices_AllBoneLayer();

public:
	HRESULT									RagDoll();
	vector<class CBone*>* GetBoneVector() { return &m_Bones; }

public:		/*For Physics Collider*/
	_float4x4								GetBoneTransform(string strBoneTag);
	_float4x4								GetBoneTransform(_int Index);

private:
	void									Apply_RootMotion_Rotation(class CTransform* pTransform, _fvector vQuaternion);
	void									Apply_RootMotion_Translation(class CTransform* pTransform, _fvector vTranslation, _fvector vQuaternion, _float3* pMovedDirection);
	void									Apply_Translation_Inverse_Rotation(_fvector vTranslation);

public:
	void									Set_CombinedMatrix(string strBoneTag, _fmatrix CombinedMatrix);
	void									Set_Parent_CombinedMatrix_Ptr(string strBoneTag, _float4x4* pParentMatrix);
	void									Set_Surbodinate(string strBoneTag, _bool isSurbodinate);

public:		/* For. Access */
	vector<string>							Get_BoneNames();
	vector<_float4>							Get_Translations();
	set<string>								Get_MeshTags();
	list<_uint>								Get_MeshIndices(const string& strMeshTag);		//	�̸��� ���� �޽����� ������ ĭ�������������Ƿ� �ε������� �����̳ʿ� ��Ƽ� ��ȯ�Ѵ�.
	vector<CAnimation*>						Get_Animations() { return m_Animations; }
	map<wstring, class CBone_Layer*>		Get_BoneLayers() { return m_BoneLayers; }
	list<wstring>							Get_BoneLayer_Tags();

	_uint									Get_NumBones() { return static_cast<_uint>(m_Bones.size()); }
	_uint									Get_NumMeshes() const { return m_iNumMeshes; }
	_uint									Get_NumPlayingInfos() { return static_cast<_uint>(m_PlayingAnimInfos.size()); }

	_int									Find_AnimIndex(CAnimation* pAnimation);
	_int									Find_AnimIndex(const string& strAnimTag);
	string									Find_RootBoneTag();
	class CPlayingInfo*						Find_PlayingInfo(_uint iPlayingIndex);
	class CBone_Layer*						Find_BoneLayer(const wstring& strBoneLayerTag);

	HRESULT									Link_Bone_Auto(CModel* pTargetModel);

	_int									Get_BoneIndex(const string& strBoneTag);

	_float4									Invalidate_RootNode(const string& strRoot);

	_bool									Is_Surbodinate_Bone(const string& strBoneTag);
	_bool									Is_Root_Bone(const string& strBoneTag);

	_uint									Get_CurrentMaxKeyFrameIndex(_uint iPlayingIndex);
	_float									Get_Duration_From_Anim(_int iAnimIndex);
	_float									Get_Duration_From_Anim(const string& strAnimTag);
	_float									Get_Duration_From_PlayingInfo(_uint iPlayingIndex);
	_float									Get_TrackPosition(_uint iPlayingIndex);
	_float									Get_BlendWeight(_uint iPlayingIndex);
	_int									Get_AnimIndex_PlayingInfo(_uint iPlayingIndex);
	wstring									Get_BoneLayerTag_PlayingInfo(_uint iPlayingIndex);
	_int									Get_PlayingIndex_From_BoneLayerTag(wstring strBoneLayerTag);

	_bool									Is_Loop_PlayingInfo(_uint iPlayingIndex);
	void									Set_Loop(_uint iPlayingIndex, _bool isLoop);

	void									Set_KeyFrameIndex_AllKeyFrame(_uint iPlayingIndex, _uint iKeyFrameIndex);
	void									Set_TrackPosition(_uint iPlayingIndex, _float fTrackPosition);
	void									Set_BlendWeight(_uint iPlayingIndex, _float fBlendWeight);
	void									Change_Animation(_uint iPlayingIndex, _uint iAnimIndex);
	void									Change_Animation(_uint iPlayingIndex, const string& strAnimTag);
	void									Set_BoneLayer_PlayingInfo(_uint iPlayingIndex, const wstring& strBoneLayerTag);

	class CBone* Get_BonePtr(const _char* pBoneName) const;

	_bool									isFinished(_uint iPlayingIndex);
	void									Get_Child_BoneIndices(string strTargetParentsBoneTag, list<_uint>& ChildBoneIndices);

	const _float4x4* Get_CombinedMatrix(const string& strBoneTag);

public:		/* For.FBX */
	virtual HRESULT							Initialize_Prototype(MODEL_TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix);

public:		/* For.Binary */
	virtual HRESULT							Initialize_Prototype(const string& strModelFilePath, _fmatrix TransformMatrix);
	virtual HRESULT							Initialize(void* pArg) override;

public:
	HRESULT									Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT									Bind_PrevBoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT									Bind_ShaderResource_Texture(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType);
	HRESULT									Bind_ShaderResource_MaterialDesc(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);

	HRESULT									Play_Animations(class CTransform* pTransform, _float fTimeDelta, _float3* pMovedDirection);
	HRESULT									Play_IK(class CTransform* pTransform, _float fTimeDelta);
	HRESULT									Render(_uint iMeshIndex);

private:
	vector<_float4x4>						Apply_Animation(_float fTimeDelta, _uint iPlayingAnimIndex);
	void									Apply_Bone_CombinedMatrices(CTransform* pTransform, _float3* pMovedDirection);
	void									Apply_Bone_TransformMatrices(const vector<vector<_float4x4>>& TransformationMatricesLayer);
	vector<_float4x4>						Compute_ResultMatrices(const vector<vector<_float4x4>>& TransformationMatricesLayer);

public:		/* For.Cooking_Mesh */
	void									Static_Mesh_Cooking(class CTransform* pTransform = nullptr);

private:
	_int									Find_BoneIndex(const string& strBoneTag);
	void									Compute_RootParentsIndicies(_uint iRootIndex, vector<_uint>& ParentsIndices);

public:
	const									MODEL_TYPE& Get_ModelType() { return m_eModelType; }

private:	/* For.Linear Interpolation */
	void									Motion_Changed(_uint iPlayingIndex);

private:	/* For.Linear Interpolation */
	void									Update_LinearInterpolation(_float fTimeDelta, _uint iPlayingIndex);

private:
	MODEL_TYPE								m_eModelType = { TYPE_END };
	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer						m_Importer;

private:
	_uint									m_iNumMeshes = { 0 };
	vector<class CMesh*>					m_Meshes;

private:	/* For.RootMotion */
	_bool									m_isRootMotion_XZ = { false };
	_bool									m_isRootMotion_Y = { false };
	_bool									m_isRootMotion_Rotation = { false };

private:
	_uint									m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>					m_Materials;

	_float4x4								m_TransformationMatrix;

	vector<class CBone*>					m_Bones;
	map<wstring, class CBone_Layer*>		m_BoneLayers;			//	���̾�� �и��� ������ �±׺��� �ε������� �����Ѵ�.

	class CIK_Solver* m_pIK_Solver = { nullptr };

	_uint									m_iNumAnimations = { 0 };
	vector<class CAnimation*>				m_Animations;

	_float4x4								m_MeshBoneMatrices[MAX_COUNT_BONE];

private:	/* For.Blend_Animation */
	vector<class CPlayingInfo*>				m_PlayingAnimInfos;

private: /* For.Additional_Input_Forces */
	//	���� �ε��� ���� �ܺ��� ��ȯ�� �߰������� �����Ѵ�.
	//	�Ӹ��� ���ų��� �� ����غ���
	vector<list<_float4x4>>					m_AdditionalForces;

private:	/* For.Linear_Interpolation */
	_float									m_fTotalLinearTime = { 0.f };

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

public:
	/* Create_From_FBX */
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix);
	/* Create_From_Binary */
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string& strModelFilePath, _fmatrix TransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END