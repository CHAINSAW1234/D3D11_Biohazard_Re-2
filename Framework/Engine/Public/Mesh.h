#pragma once

#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class CMesh final : public CVIBuffer
{
public:
	struct tFace
	{
		int VertexIndex[3];
		int TexCoordIndex[3];
	};
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

public:
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
	HRESULT Stock_PrevMatrices(const vector<CBone*>& Bones, _float4x4* pMeshBoneMatrices);

private:
	_char					m_szName[MAX_PATH] = { "" };

	_uint					m_iMaterialIndex = { 0 };

	_uint					m_iNumBones = { 0 };
	vector<_uint>			m_Bones;

	vector<_float4x4>		m_OffsetMatrices;

	MATERIAL_DESC			m_MaterialDesc = {};

	//For Mesh Cooking
	_float3* m_pVertices_Cooking = { nullptr };
	_uint* m_pIndices_Cooking = { nullptr };

private:/* For.FBXLoad */
	HRESULT					Ready_Vertices_For_NonAnimModel(const aiMesh* pAIMesh, _fmatrix TransformationMatrix);
	HRESULT					Ready_Vertices_For_AnimModel(const aiMesh* pAIMesh, const map<string, _uint>& BoneIndices);

private:/* For.BinaryLoad */
	HRESULT					Ready_Vertices_For_NonAnimModel(const vector<VTXANIMMESH>& Vertices);
	HRESULT					Ready_Vertices_For_AnimModel(const vector<VTXANIMMESH>& Vertices, const vector<_uint>& Indices, const vector<_uint>& Bones, const vector<_float4x4>& OffsetMatrices);

public:/*For Cooking*/
	void	Static_Mesh_Cooking(class CTransform* pTransform = nullptr);

public: /* For Octree Culling*/
	_uint					GetNumFaces()
	{
		return m_iNumIndices / 3;
	}
	_uint					GetNumVertices()
	{
		return m_iNumVertices;
	}
	_uint					GetNumIndices()
	{
		return m_iNumIndices;
	}
	_float3* GetVertices()
	{
		return m_pVertices_Cooking;
	}
	_uint* GetIndices()
	{
		return m_pIndices_Cooking;
	}
	vector<tFace*>* GetFaces()
	{
		return &m_vecFaces;
	}
	void					SetNumFaces(_int Count)
	{
		m_iNumIndices = Count * 3;
	}
	void					SetVertices(_float3* pVertices);
	void					Init_For_Octree();
	void					Ready_Buffer_For_Octree(_float4 vTranslation);
	void					SetNumVertices(_uint Num)
	{
		m_iNumVertices = Num;
	}
	void					Octree_Mesh_Cooking_For_PX();
	ID3D11Buffer* GetVertexBuffer()
	{
		return m_pVB;
	}
	void					SetVertexBuffer(ID3D11Buffer* pBuffer)
	{
		m_pVB = pBuffer;
		Safe_AddRef(pBuffer);
	}
	ID3D11Buffer* GetIndexBuffer()
	{
		return m_pIB;
	}
	void					SetIndexBuffer(ID3D11Buffer* pBuffer)
	{
		m_pIB = pBuffer;
	}
	_float3* GetNormals()
	{
		return m_pNormals;
	}
	_float3* GetTangents()
	{
		return m_pTangents;
	}
	_float2* GetTexcoords()
	{
		return m_pTexcoords;
	}
	void					SetNormals(_float3* pNormals);
	void					SetTangents(_float3* pTangents);
	void					SetTexcoords(_float2* pTexcoords);
	void					Release_IndexBuffer();
	void					Release_Dump();

public:
	_bool					Is_Hide() { return m_isHide; }
	void					Set_Hide(_bool isHide) { m_isHide = isHide; }

	_float3					Get_CenterPoint()
	{
		return m_vCenterPoint;
	}
private:
	vector<tFace*>			m_vecFaces;
	_float3*				m_pNormals = { nullptr };
	_float2*				m_pTexcoords = { nullptr };
	_float3*				m_pTangents = { nullptr };
	
	_bool					m_isHide = { false };

	_float3	 m_vCenterPoint;
public:
	/* For.FBXLoad*/
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::MODEL_TYPE eModelType, const aiMesh* pAIMesh, const map<string, _uint>& BoneIndices, _fmatrix TransformationMatrix);
	/* For.BinaryLoad */
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::MODEL_TYPE eType, const MESH_DESC& MeshDesc);
	virtual CMesh* Clone(void* pArg);
	virtual void Free() override;
};

END