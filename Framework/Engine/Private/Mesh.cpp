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

	/* 인덱스 버퍼의 byte크기 */
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

	/* 인덱스 버퍼의 byte크기 */
	m_BufferDesc.ByteWidth = m_iIndexStride * m_iNumIndices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	for (_uint i = 0; i < m_iNumIndices; ++i)
	{
		pIndices[i] = MeshDesc.Indices[i];
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

HRESULT CMesh::Ready_Vertices_For_NonAnimModel(const aiMesh* pAIMesh, _fmatrix TransformationMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	ZeroMemory(&m_BufferDesc, sizeof m_BufferDesc);

	/* 정점버퍼의 byte크기 */
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

		/* 이 뼈는 몇개의 정점들에게 영향을 준다. */
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

	/* 정점버퍼의 byte크기 */
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
		memcpy_s(&pVertices[i].vPosition, sizeof(_float3), &Vertices[i].vPosition, sizeof(_float3));
		memcpy_s(&m_pVerticesPos[i], sizeof(_float3), &pVertices[i].vPosition, sizeof(_float3));
		memcpy_s(&pVertices[i].vNormal, sizeof(_float3), &Vertices[i].vNormal, sizeof(_float3));
		memcpy_s(&pVertices[i].vTexcoord, sizeof(_float2), &Vertices[i].vTexcoord, sizeof(_float2));
		memcpy_s(&pVertices[i].vTangent, sizeof(_float3), &Vertices[i].vTangent, sizeof(_float3));
	}

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

	/* 정점버퍼의 byte크기 */
	m_BufferDesc.ByteWidth = m_iVertexStride * m_iNumVertices;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iVertexStride;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; i++)
	{
		memcpy_s(&pVertices[i].vPosition, sizeof(_float3), &Vertices[i].vPosition, sizeof(_float3));
		memcpy_s(&m_pVerticesPos[i], sizeof(_float3), &pVertices[i].vPosition, sizeof(_float3));
		memcpy_s(&pVertices[i].vNormal, sizeof(_float3), &Vertices[i].vNormal, sizeof(_float3));
		memcpy_s(&pVertices[i].vTexcoord, sizeof(_float2), &Vertices[i].vTexcoord, sizeof(_float2));
		memcpy_s(&pVertices[i].vTangent, sizeof(_float3), &Vertices[i].vTangent, sizeof(_float3));
		memcpy_s(&pVertices[i].vBlendIndices, sizeof(XMUINT4), &Vertices[i].vBlendIndices, sizeof(XMUINT4));
		memcpy_s(&pVertices[i].vBlendWeights, sizeof(_float4), &Vertices[i].vBlendWeights, sizeof(_float4));
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

#pragma region 별도의 노말계산 그러나 들어있던것과 오차 범위이내수준
	//vector<_float3> vNormals;
	//vNormals.resize(m_iNumVertices);

	//for (_uint i = 0; i < m_iNumIndices; )
	//{
	//	_uint iIndices[] = {
	//		Indices[i++],
	//		Indices[i++],
	//		Indices[i++]
	//	};

	//	_vector vSrc = XMLoadFloat3(&Vertices[iIndices[1]].vPosition) - XMLoadFloat3(&Vertices[iIndices[0]].vPosition);
	//	_vector vDest = XMLoadFloat3(&Vertices[iIndices[2]].vPosition) - XMLoadFloat3(&Vertices[iIndices[1]].vPosition);

	//	_vector vNormal = XMVector3Cross(vSrc, vDest);

	//	XMStoreFloat3(&vNormals[iIndices[0]], XMLoadFloat3(&vNormals[iIndices[0]]) + vNormal);
	//	XMStoreFloat3(&vNormals[iIndices[1]], XMLoadFloat3(&vNormals[iIndices[1]]) + vNormal);
	//	XMStoreFloat3(&vNormals[iIndices[2]], XMLoadFloat3(&vNormals[iIndices[2]]) + vNormal);
	//}

	//for (_uint i = 0; i < m_iNumVertices; ++i)
	//	XMStoreFloat3(&vNormals[i], XMVector3Normalize(XMLoadFloat3(&vNormals[i])));

	//for (_uint i = 0; i < m_iNumVertices; i++)
	//{
	//	memcpy_s(&pVertices[i].vNormal, sizeof(_float3), &Vertices[i].vNormal, sizeof(_float3));
	//}

	//for (_uint i = 0; i < m_iNumVertices; ++i)
	//{
	//	if (false == XMVector3Equal(XMLoadFloat3(&pVertices[i].vNormal), XMLoadFloat3(&vNormals[i])))
	//		int iA = 0;
	//}

#pragma endregion

	ZeroMemory(&m_InitialData, sizeof(m_InitialData));
	m_InitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
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
}
