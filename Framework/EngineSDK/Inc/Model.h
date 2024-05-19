#pragma once

#include "Component.h"
#include "Animation.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum MODEL_TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	class CBone* Get_BonePtr(const _char* pBoneName) const;

	_bool isFinished() {
		return m_Animations[m_iCurrentAnimIndex]->isFinished();
	}

public:	/* For.Animation */
	void Set_Animation(_uint iAnimIndex, _bool isLoop);

	_uint Get_NumAnims() { return m_iNumAnimations; }
	_uint Get_CurrentAnimIndex() { return m_iCurrentAnimIndex; }

	void Set_TickPerSec(_uint iAnimIndex, _float fTickPerSec);

public:
	void Set_CombinedMatrix(string strBoneTag, _fmatrix CombinedMatrix);
	void Set_Parent_CombinedMatrix_Ptr(string strBoneTag, _float4x4* pParentMatrix);
	void Set_Surbodinate(string strBoneTag, _bool isSurbodinate);

public:
	vector<string>				Get_BoneNames();
	vector<_float4>				Get_Translations();
	set<string>					Get_MeshTags();
	list<_uint>					Get_MeshIndices(const string& strMeshTag);		//	이름이 같은 메쉬들이 각각의 칸에있을수있으므로 인덱스들을 컨테이너에 담아서 반환한다.

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


public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	HRESULT Bind_ShaderResource_Texture(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType);
	HRESULT Bind_ShaderResource_MaterialDesc(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	//	루트모션 아닌 경우
	HRESULT Play_Animation(_float fTimeDelta);
	//	루트모션인 경우
	HRESULT Play_Animation_RootMotion_Translation(class CTransform* pTransform, const string& strRootTag, _float fTimeDelta, _bool isAutoRotate = false);
	HRESULT Play_Animation_RootMotion_WorldMatrix(class CTransform* pTransform, const string& strRootTag, _float fTimeDelta);
	HRESULT Render(_uint iMeshIndex);

public:
	const _float4x4* Get_CombinedMatrix(const string& strBoneTag);

private:
	_uint Find_BoneIndex(const string& strBoneTag);
	void Compute_RootParentsIndicies(_uint iRootIndex, vector<_uint>& ParentsIndices);

	void Reset_PreTranslation_Translation(_int iRootIndex);
	void Reset_PreTranslation_WorldMatrix(_int iRootIndex);

public:	/* For.KeyFrame */
	_uint Get_CurrentMaxKeyFrameIndex();
	_float Get_Duration(_int iAnimIndex = -1);
	_float Get_TrackPosition();
	void Set_KeyFrameIndex(_uint iKeyFrameIndex);
	void Set_TrackPosition(_float fTrackPosition);
	const vector<_uint>& Get_CurrentKeyFrameIndices();

	//public:	/* For.Test */
	//	_bool Compare(CModel* pCompareModel);

public:
	const MODEL_TYPE& Get_ModelType() { return m_eModelType; }

private: /* For.Linear Interpolation */
	void Motion_Changed();

private:	/* For.Linear Interpolation */
	void Update_LinearInterpolation(_float fTimeDelta, _int iRootIndex = -1);
	void Reset_LinearInterpolation();

private:	/* For.Linear_Interpolation */
	_bool						m_isLinearInterpolation = { false };
	_float						m_fAccLinearInterpolation = { 0.f };
	vector<KEYFRAME>			m_LastKeyFrames;

private:
	MODEL_TYPE					m_eModelType = { TYPE_END };
	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer			m_Importer;

private:
	_uint						m_iNumMeshes = { 0 };
	vector<class CMesh*>		m_Meshes;

	_uint						m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>		m_Materials;

	_float4x4					m_TransformationMatrix;

	vector<class CBone*>		m_Bones;

	_uint						m_iNumAnimations = { 0 };
	_uint						m_iCurrentAnimIndex = { 0 };
	_uint						m_iPreAnimIndex = { 0 };
	_bool						m_isLoop = { false };
	vector<class CAnimation*>	m_Animations;

	_float4x4					m_MeshBoneMatrices[512];

private:	/* For.Linear_Interpolation */
	_float3						m_vPreTranslationDir = { 0.f, 0.f, 0.f };
	_float4x4					m_PreTranslationMatrix;
	wstring						m_strRootTag = { TEXT("") };
	_float						m_fTotalLinearTime = { 0.2f };

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