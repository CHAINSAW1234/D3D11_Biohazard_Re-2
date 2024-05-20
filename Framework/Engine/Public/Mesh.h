#pragma once

#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
public:
	typedef struct tagMeshDesc
	{
		string				strName = { "" };
		_uint				iMaterialIndex = { 0 };

		_uint				iNumVertices = { 0 };
		vector<VTXANIMMESH> Vertices;

		_uint				iNumIndices = { 0 };
		vector<_uint>		Indices;

		_uint				iNumBones = { 0 };
		vector<_uint>		Bones;

		_uint				iNumOffsetMatrices = { 0 };
		vector<_float4x4>	OffsetMatrices;
	}MESH_DESC;

private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& rhs);
	virtual ~CMesh() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

	const string Get_MeshName() { return m_szName; }

public:
	/* For.FBXLoad*/
	virtual HRESULT Initialize_Prototype(CModel::MODEL_TYPE eModelType, const aiMesh* pAIMesh, const map<string, _uint>& BoneIndices, _fmatrix TransformationMatrix);
	/* For.BinaryLoad */
	virtual HRESULT Initialize_Prototype(CModel::MODEL_TYPE eType, const MESH_DESC& MeshDesc);
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Set_MatreialDesc(const MATERIAL_DESC& MaterialDesc);
	MATERIAL_DESC Get_MaterialDesc();

public:
	HRESULT Stock_Matrices(const vector<CBone*>& Bones, _float4x4* pMeshBoneMatrices);

private:
	_char					m_szName[MAX_PATH] = { "" };

	/* 이 메시는 모델에서 로드해놓은 머테리얼들 중 몇번째 머테리얼을 이용하는가? */
	_uint					m_iMaterialIndex = { 0 };

	_uint					m_iNumBones = { 0 };
	vector<_uint>			m_Bones;

	vector<_float4x4>		m_OffsetMatrices;

	MATERIAL_DESC			m_MaterialDesc = {};

private:/* For.FBXLoad */
	HRESULT Ready_Vertices_For_NonAnimModel(const aiMesh* pAIMesh, _fmatrix TransformationMatrix);
	HRESULT Ready_Vertices_For_AnimModel(const aiMesh* pAIMesh, const map<string, _uint>& BoneIndices);

private:/* For.BinaryLoad */
	HRESULT Ready_Vertices_For_NonAnimModel(const vector<VTXANIMMESH>& Vertices);
	HRESULT Ready_Vertices_For_AnimModel(const vector<VTXANIMMESH>& Vertices, const vector<_uint>& Indices, const vector<_uint>& Bones, const vector<_float4x4>& OffsetMatrices);

public:
	/* For.FBXLoad*/
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::MODEL_TYPE eModelType, const aiMesh* pAIMesh, const map<string, _uint>& BoneIndices, _fmatrix TransformationMatrix);
	/* For.BinaryLoad */
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::MODEL_TYPE eType, const MESH_DESC& MeshDesc);
	virtual CMesh* Clone(void* pArg);
	virtual void Free() override;

#pragma region TEST
	//public: /* For.Test */
	//	_bool Compare(CMesh* pMesh) {
	//		if (string(pMesh->m_szName) != string(m_szName))
	//		{
	//			int iA = 0;
	//		}
	//
	//		if (pMesh->m_iMaterialIndex != m_iMaterialIndex)
	//		{
	//			int iA = 0;
	//		}
	//
	//		if (pMesh->m_iNumBones != m_iNumBones)
	//		{
	//			int iA = 0;
	//		}
	//
	//		if (pMesh->m_iNumIndices != m_iNumIndices)
	//		{
	//			int iA = 0;
	//		}
	//
	//		if (pMesh->m_iNumVertices != m_iNumVertices)
	//		{
	//			int iA = 0;
	//		}
	//
	//		for (size_t i = 0; i < m_iNumBones; ++i)
	//		{
	//			if (pMesh->m_Bones[i] != m_Bones[i])
	//				int iA = 0;
	//		}
	//
	//		for (_uint i = 0; i < 4; ++i)
	//		{
	//			for (_uint j = 0; j < 4; ++j)
	//			{
	//				for (size_t k = 0; k < m_iNumBones; ++k)
	//				{
	//					if (m_OffsetMatrices[k].m[i][j] != pMesh->m_OffsetMatrices[k].m[i][j])
	//						int iA = 0;
	//				}
	//			}
	//		}
	//
	//		return true;
	//	}
#pragma endregion
};

END