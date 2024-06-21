#include "Mesh.h"
#include "Bone.h"

#include "GameInstance.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CMesh::CMesh(const CMesh& rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CMesh::Initialize_Prototype(CModel::MODEL_TYPE eModelType, const aiMesh* pAIMesh, const map<string, _uint>& BoneIndices, _fmatrix TransformationMatrix)
{
	strcpy_s(m_szName, pAIMesh->mName.data);
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_pVerticesPos = new _float3[m_iNumVertices];
	m_iNumIndices = pAIMesh->mNumFaces * 3;
	m_iIndexStride = sizeof(_uint);
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER

	HRESULT hr =
		CModel::TYPE_NONANIM == eModelType
		? Ready_Vertices_For_NonAnimModel(pAIMesh, TransformationMatrix)
		: Ready_Vertices_For_AnimModel(pAIMesh, BoneIndices);

	if (FAILED(hr))
		return E_FAIL;
#pragma endregion


#pragma region INDEX_BUFFER

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint		iNumIndices = { 0 };

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
		pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize_Prototype(CModel::MODEL_TYPE eType, const MESH_DESC& MeshDesc)
{
	strcpy_s(m_szName, MeshDesc.strName.c_str());
	m_iMaterialIndex = MeshDesc.iMaterialIndex;
	m_iNumVertices = (_uint)MeshDesc.Vertices.size();
	m_pVerticesPos = new _float3[m_iNumVertices];
	m_iNumIndices = (_uint)MeshDesc.Indices.size();
	m_pIndices = new _uint[m_iNumIndices];
	m_iIndexStride = sizeof(_uint);
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	HRESULT hr =
		CModel::TYPE_NONANIM == eType
		? Ready_Vertices_For_NonAnimModel(MeshDesc.Vertices)
		: Ready_Vertices_For_AnimModel(MeshDesc.Vertices, MeshDesc.Indices, MeshDesc.Bones, MeshDesc.OffsetMatrices);

	if (FAILED(hr))
		return E_FAIL;

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_uint* pIndices = new _uint[m_iNumIndices];

	m_pIndices_Cooking = new _uint[m_iNumIndices];

	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	for (_uint i = 0; i < m_iNumIndices; ++i)
	{
		pIndices[i] = MeshDesc.Indices[i];
	}

	for (_uint i = 0; i < m_iNumIndices; ++i)
	{
		m_pIndices_Cooking[i] = pIndices[i];
	}

	for (_uint i = 0; i < m_iNumIndices / 3; ++i)
	{
		auto Face = new tFace();
		//auto Face = tFace();

		for (int j = 0; j < 3; ++j)
		{
			Face->VertexIndex[j] = pIndices[i * 3 + j];
		}

		m_vecFaces.push_back(Face);
	}

	memcpy(m_pIndices, pIndices, sizeof(_uint) * m_iNumIndices);

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

#pragma endregion

	return S_OK;
}

HRESULT CMesh::Initialize(void* pArg)
{
	return S_OK;
}

void CMesh::Set_MatreialDesc(const MATERIAL_DESC& MaterialDesc)
{
	m_MaterialDesc = MaterialDesc;
}

MATERIAL_DESC CMesh::Get_MaterialDesc()
{
	return m_MaterialDesc;
}

HRESULT CMesh::Stock_Matrices(const vector<CBone*>& Bones, _float4x4* pMeshBoneMatrices)
{
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		//	메쉬의 뼈 메트릭스를 전부 순회하며 담아준다.
		//	해당 뼈의 오프셋 매트릭스 * 전체뼈에서의 해당 뼈의 컴바인드 매트릭스를 곱한다.
		_matrix OffsetMatrix = XMLoadFloat4x4(&m_OffsetMatrices[i]);
		_matrix CombinedMatrix = XMLoadFloat4x4(Bones[m_Bones[i]]->Get_CombinedTransformationMatrix());

		_matrix BoneMatrix = OffsetMatrix * CombinedMatrix;

		XMStoreFloat4x4(&pMeshBoneMatrices[i], BoneMatrix);
	}

	return S_OK;
}

HRESULT CMesh::Stock_PrevMatrices(const vector<CBone*>& Bones, _float4x4* pMeshBoneMatrices)
{
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		_matrix OffsetMatrix = XMLoadFloat4x4(&m_OffsetMatrices[i]);
		_matrix CombinedMatrix = XMLoadFloat4x4(Bones[m_Bones[i]]->Get_PrevCombinedTransformationMatrix());

		_matrix BoneMatrix = OffsetMatrix * CombinedMatrix;

		XMStoreFloat4x4(&pMeshBoneMatrices[i], BoneMatrix);
	}

	return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_NonAnimModel(const aiMesh* pAIMesh, _fmatrix TransformationMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy_s(&pVertices[i].vPosition, sizeof(_float3), &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), TransformationMatrix));
		XMStoreFloat3(&m_pVerticesPos[i], XMLoadFloat3(&pVertices[i].vPosition));

		memcpy_s(&pVertices[i].vNormal, sizeof(_float3), &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), TransformationMatrix));

		memcpy_s(&pVertices[i].vTexcoord, sizeof(_float2), &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy_s(&pVertices[i].vTangent, sizeof(_float3), &pAIMesh->mTangents[i], sizeof(_float3));
	}

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_AnimModel(const aiMesh* pAIMesh, const map<string, _uint>& BoneIndices)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼의 byte크기 */
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);


	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy_s(&pVertices[i].vPosition, sizeof(_float3), &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy_s(&m_pVerticesPos[i], sizeof(_float3), &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy_s(&pVertices[i].vNormal, sizeof(_float3), &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy_s(&pVertices[i].vTexcoord, sizeof(_float2), &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy_s(&pVertices[i].vTangent, sizeof(_float3), &pAIMesh->mTangents[i], sizeof(_float3));
	}

	m_iNumBones = pAIMesh->mNumBones;

	for (_uint i = 0; i < m_iNumBones; i++)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];

		_float4x4	OffsetMatrix;
		memcpy_s(&OffsetMatrix, sizeof(_float4x4), &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		//	assimp에서 로드한 행렬은 전부 전치되어있으므로 다시 전치하여 저장해야한다.
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.push_back(OffsetMatrix);

		auto iter = BoneIndices.find(pAIBone->mName.data);

		if (iter == BoneIndices.end())
			return E_FAIL;

		_uint iBoneIndex = iter->second;

		m_Bones.push_back(iBoneIndex);

		_uint		iNumWeights = pAIBone->mNumWeights;

		for (_uint j = 0; j < iNumWeights; j++)
		{
			if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.x = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.x = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.y = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.y = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.z = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.z = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w)
			{
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendIndices.w = i;
				pVertices[pAIBone->mWeights[j].mVertexId].vBlendWeights.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}

	//	뼈가 없던 메쉬라면 강제적으로 뼈를 생성해준다.
	//	보통은 메쉬의 이름과 동일한 이름을 지닌 뼈가 모델 전체의 뼈중에 있으므로 해당뼈를 찾아서 지닌다.
	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		auto iterPair = BoneIndices.find(m_szName);
		if (BoneIndices.end() == iterPair)
			return E_FAIL;

		_uint iBoneIndex = iterPair->second;

		m_Bones.push_back(iBoneIndex);

		_float4x4		OffsetMatrix;

		//	부모단계의 컴바인드 매트릭스 단계에서 이미 해당 정보의 기입이끝났다고 가정
		//	별도의 오프셋을 지니지않게한다. ( 항등행렬 1개 삽입 )
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.push_back(OffsetMatrix);
	}

	ZeroMemory(&m_InitialData, sizeof(m_InitialData));
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_NonAnimModel(const vector<VTXANIMMESH>& Vertices)
{
	m_iVertexStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];

	m_pVertices_Cooking = new _float3[m_iNumVertices];
	m_pNormals = new _float3[m_iNumVertices];
	m_pTexcoords = new _float2[m_iNumVertices];
	m_pTangents = new _float3[m_iNumVertices];

	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	for (size_t i = 0; i < static_cast<size_t>(m_iNumVertices); i++)
	{
		memcpy_s(&pVertices[i].vPosition, sizeof(_float3), &Vertices[i].vPosition, sizeof(_float3));
		memcpy_s(&m_pVerticesPos[i], sizeof(_float3), &pVertices[i].vPosition, sizeof(_float3));
		memcpy_s(&pVertices[i].vNormal, sizeof(_float3), &Vertices[i].vNormal, sizeof(_float3));
		memcpy_s(&pVertices[i].vTexcoord, sizeof(_float2), &Vertices[i].vTexcoord, sizeof(_float2));
		memcpy_s(&pVertices[i].vTangent, sizeof(_float3), &Vertices[i].vTangent, sizeof(_float3));
	}

	_float3 vTotal_Pos = _float3(0.f,0.f,0.f);

	for (int i = 0; i < static_cast<_int>(m_iNumVertices); ++i)
	{
		vTotal_Pos += pVertices[i].vPosition;
		m_pVertices_Cooking[i] = pVertices[i].vPosition;
		m_pNormals[i] = pVertices[i].vNormal;
		m_pTexcoords[i] = pVertices[i].vTexcoord;
		m_pTangents[i] = pVertices[i].vTangent;
	}

	m_vCenterPoint = vTotal_Pos / m_iNumVertices;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_AnimModel(const vector<VTXANIMMESH>& Vertices, const vector<_uint>& Indices, const vector<_uint>& Bones, const vector<_float4x4>& OffsetMatrices)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	m_pVertices_Cooking = new _float3[m_iNumVertices];

	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		memcpy_s(&pVertices[i].vPosition, sizeof(_float3), &Vertices[i].vPosition, sizeof(_float3));
		memcpy_s(&m_pVerticesPos[i], sizeof(_float3), &pVertices[i].vPosition, sizeof(_float3));
		memcpy_s(&pVertices[i].vNormal, sizeof(_float3), &Vertices[i].vNormal, sizeof(_float3));
		memcpy_s(&pVertices[i].vTexcoord, sizeof(_float2), &Vertices[i].vTexcoord, sizeof(_float2));
		memcpy_s(&pVertices[i].vTangent, sizeof(_float3), &Vertices[i].vTangent, sizeof(_float3));
		memcpy_s(&pVertices[i].vBlendIndices, sizeof(XMUINT4), &Vertices[i].vBlendIndices, sizeof(XMUINT4));
		memcpy_s(&pVertices[i].vBlendWeights, sizeof(_float4), &Vertices[i].vBlendWeights, sizeof(_float4));

		_float			fTotalWeights = { Vertices[i].vBlendWeights.x + Vertices[i].vBlendWeights.y + Vertices[i].vBlendWeights.z + Vertices[i].vBlendWeights.w };


		if (Vertices[i].vBlendWeights.w == 0.f)
		{
			_int		iA = 0;

			if (abs(1.f - fTotalWeights) > 0.0001f)
			{
				_int		iA = { 0 };
			}
		}
	}

	for (int i = 0; i < static_cast<_int>(m_iNumVertices); ++i)
	{
		m_pVertices_Cooking[i] = pVertices[i].vPosition;
	}

	m_iNumBones = (_uint)Bones.size();

	for (auto& iBoneIndex : Bones)
	{
		m_Bones.push_back(iBoneIndex);
	}

	for (auto& OffsetMatrix : OffsetMatrices)
	{
		m_OffsetMatrices.push_back(OffsetMatrix);
	}

	ZeroMemory(&m_InitialData, sizeof(m_InitialData));
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

void CMesh::Static_Mesh_Cooking(CTransform* pTransform)
{
	m_pGameInstance->Cook_Mesh(m_pVertices_Cooking, m_pIndices_Cooking, m_iNumVertices, m_iNumIndices,pTransform);
}

void CMesh::Static_Mesh_Cooking_NoRotation(CTransform* pTransform)
{
	m_pGameInstance->Cook_Mesh_NoRotation(m_pVertices_Cooking, m_pIndices_Cooking, m_iNumVertices, m_iNumIndices, pTransform);
}

void CMesh::Dynamic_Mesh_Cooking(vector<PxRigidDynamic*>* pColliders, vector<PxTransform>* pTransforms,CTransform* pTransform)
{
	m_pGameInstance->Cook_Mesh_Dynamic(m_pVertices_Cooking, m_pIndices_Cooking, m_iNumVertices, m_iNumIndices, pColliders, pTransforms, pTransform);
}

void CMesh::Convex_Mesh_Cooking(vector<PxRigidDynamic*>* pColliders, vector<PxTransform>* pTransforms, CTransform* pTransform)
{
	m_pGameInstance->Cook_Mesh_Convex(m_pVertices_Cooking, m_pIndices_Cooking, m_iNumVertices, m_iNumIndices,pColliders,pTransforms, pTransform);
}

void CMesh::Convex_Mesh_Cooking_Convert_Root(vector<PxRigidDynamic*>* pColliders, vector<PxTransform>* pTransforms, CTransform* pTransform, _float4 vDelta)
{
	m_pGameInstance->Cook_Mesh_Convex_Convert_Root(m_pVertices_Cooking, m_pIndices_Cooking, m_iNumVertices, m_iNumIndices, pColliders, pTransforms, pTransform,vDelta);
}

void CMesh::Convex_Mesh_Cooking_Convert_Root_No_Rotate(vector<PxRigidDynamic*>* pColliders, vector<PxTransform>* pTransforms, CTransform* pTransform, _float4 vDelta)
{
	m_pGameInstance->Cook_Mesh_Convex_Convert_Root_No_Rotate(m_pVertices_Cooking, m_pIndices_Cooking, m_iNumVertices, m_iNumIndices, pColliders, pTransforms, pTransform, vDelta);
}

void CMesh::Create_SoftBody()
{
	m_pGameInstance->Create_SoftBody(m_pVertices_Cooking, m_pIndices_Cooking, m_iNumVertices, m_iNumIndices);
}

void CMesh::SetVertices(_float3* pVertices)
{
	m_pVertices_Cooking = new _float3[m_iNumVertices];

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		m_pVertices_Cooking[i] = pVertices[i];
	}
}

void CMesh::Init_For_Octree()
{
	m_pIndices_Cooking = new _uint[m_iNumIndices];
}

void CMesh::Ready_Buffer_For_Octree(_float4 vTranslation)
{
	if (m_iNumIndices == 0)
		return;

	m_iNumVertexBuffers = 1;

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_iVertexStride = sizeof(VTXMESH);

	//VTXMESH* pVertices = new VTXMESH[m_iNumVertices];

	//ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	//for (size_t i = 0; i < static_cast<size_t>(m_iNumVertices); i++)
	//{
	//	pVertices[i].vPosition = m_pVertices_Cooking[i]/* + Convert_Float4_To_Float3(vTranslation)*/;
	//	pVertices[i].vNormal = m_pNormals[i];
	//	pVertices[i].vTexcoord = m_pTexcoords[i];
	//	pVertices[i].vTangent = m_pTangents[i];
	//}

	//ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	//m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	//m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//m_BufferDesc.CPUAccessFlags = 0;
	//m_BufferDesc.MiscFlags = 0;
	//m_BufferDesc.StructureByteStride = m_iVertexStride;

	//ZeroMemory(&m_InitialData, sizeof m_InitialData);
	//m_InitialData.pSysMem = pVertices;

	//__super::Create_Buffer(&m_pVB);

	//Safe_Delete_Array(pVertices);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	m_iIndexStride = sizeof(_uint);
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	ZeroMemory(&m_InitialData, sizeof m_InitialData);
	m_InitialData.pSysMem = m_pIndices_Cooking;

	__super::Create_Buffer(&m_pIB);
}

void CMesh::Octree_Mesh_Cooking_For_PX()
{
	if (m_iNumIndices == 0)
		return;

	m_pGameInstance->Cook_Mesh(m_pVertices_Cooking, m_pIndices_Cooking, m_iNumVertices, m_iNumIndices);
}

void CMesh::SetNormals(_float3* pNormals)
{
	m_pNormals = new _float3[m_iNumVertices];
	for (int i = 0; i < static_cast<_int>(m_iNumVertices); ++i)
	{
		m_pNormals[i] = pNormals[i];
	}
}

void CMesh::SetTangents(_float3* pTangents)
{
	m_pTangents = new _float3[m_iNumVertices];
	for (int i = 0; i < static_cast<_int>(m_iNumVertices); ++i)
	{
		m_pTangents[i] = pTangents[i];
	}
}

void CMesh::SetTexcoords(_float2* pTexcoords)
{
	m_pTexcoords = new _float2[m_iNumVertices];
	for (int i = 0; i < static_cast<_int>(m_iNumVertices); ++i)
	{
		m_pTexcoords[i] = pTexcoords[i];
	}
}

void CMesh::Release_IndexBuffer()
{
	Safe_Release(m_pIB);
	Safe_Delete_Array(m_pIndices_Cooking);
	Safe_Delete_Array(m_pVertices_Cooking);
	Safe_Delete_Array(m_pVerticesPos);
	Safe_Delete_Array(m_pIndices);
}

void CMesh::Release_Dump()
{
	if (m_pVertices_Cooking)
		Safe_Delete_Array(m_pVertices_Cooking);
	if (m_pIndices_Cooking)
		Safe_Delete_Array(m_pIndices_Cooking);
	if (m_pNormals)
		Safe_Delete_Array(m_pNormals);
	if (m_pTangents)
		Safe_Delete_Array(m_pTangents);
	if (m_pTexcoords)
		Safe_Delete_Array(m_pTexcoords);

	if (!m_vecFaces.empty())
	{
		for (int i = 0; i < m_vecFaces.size(); ++i)
		{
			delete m_vecFaces[i];
		}
	}

	m_vecFaces.clear();
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::MODEL_TYPE eModelType, const aiMesh* pAIMesh, const map<string, _uint>& BoneIndices, _fmatrix TransformationMatrix)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pAIMesh, BoneIndices, TransformationMatrix)))
	{
		MSG_BOX(TEXT("Failed To Created : CMesh"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::MODEL_TYPE eType, const MESH_DESC& MeshDesc)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, MeshDesc)))
	{
		MSG_BOX(TEXT("Failed To Created : CMesh"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CMesh* CMesh::Clone(void* pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMesh"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();

	if (m_pVertices_Cooking)
		Safe_Delete_Array(m_pVertices_Cooking);
	if (m_pIndices_Cooking)
		Safe_Delete_Array(m_pIndices_Cooking);
	if (m_pNormals)
		Safe_Delete_Array(m_pNormals);
	if (m_pTangents)
		Safe_Delete_Array(m_pTangents);
	if (m_pTexcoords)
		Safe_Delete_Array(m_pTexcoords);

	if (!m_vecFaces.empty())
	{
		for (int i = 0; i < m_vecFaces.size(); ++i)
		{
			delete m_vecFaces[i];
		}
	}
}
