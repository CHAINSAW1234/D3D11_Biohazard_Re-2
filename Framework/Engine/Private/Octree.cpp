#include "GameInstance.h"
#include "Octree.h"
#include "Model.h"
#include "Mesh.h"
#include "Shader.h"
#include "GameObject.h"

int g_CurrentSubdivision = 0;

COctree::COctree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CGameInstance* pGameInstance, _float4 vTranslation,CGameObject* pPlayer)
{
	m_bSubDivided = false;
	m_Width = 0;
	m_TriangleCount = 0;
	m_vCenter = _float4(0, 0, 0, 1.f);
	m_pWorld = NULL;
	memset(m_pOctreeNodes, 0, sizeof(m_pOctreeNodes));

	m_pDevice = pDevice;
	m_pContext = pContext;
	m_pGameInstance = pGameInstance;
	m_vTranslation = vTranslation;
	m_pPlayer = pPlayer;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

COctree::~COctree()
{

}

int COctree::GetSceneTriangleCount(CModel* pWorld)
{
	int numberOfTriangles = 0;

	auto Meshes = *pWorld->GetMeshes();
	auto NumMesh = pWorld->GetNumMesh();

	for (_uint i = 0; i < NumMesh; i++)
	{
		numberOfTriangles += Meshes[i]->GetNumFaces();
	}

	return numberOfTriangles;
}

void COctree::GetSceneDimensions(CModel* pWorld)
{
	if (!pWorld) return;

	auto Meshes = *pWorld->GetMeshes();
	auto NumMesh = pWorld->GetNumMesh();

	int numberOfVerts = 0;

	for (_uint i = 0; i < NumMesh; i++)
	{
		numberOfVerts += Meshes[i]->GetNumVertices();

		for (_uint n = 0; n < Meshes[i]->GetNumVertices(); n++)
		{
			auto pVertices = Meshes[i]->GetVertices();

			m_vCenter = Add_Float4_With_Float3_Coord(m_vCenter, pVertices[n]);
		}
	}

	m_vCenter.x /= numberOfVerts;
	m_vCenter.y /= numberOfVerts;
	m_vCenter.z /= numberOfVerts;

	float currentWidth = 0.0f, currentHeight = 0.0f, currentDepth = 0.0f;

	float maxWidth = 0.0f, maxHeight = 0.0f, maxDepth = 0.0f;

	for (_uint i = 0; i < NumMesh; i++)
	{
		for (_uint j = 0; j < Meshes[i]->GetNumVertices(); j++)
		{
			auto pVertices = Meshes[i]->GetVertices();

			currentWidth = fabsf(pVertices[j].x - m_vCenter.x);
			currentHeight = fabsf(pVertices[j].y - m_vCenter.y);
			currentDepth = fabsf(pVertices[j].z - m_vCenter.z);

			if (currentWidth > maxWidth)
				maxWidth = currentWidth;

			if (currentHeight > maxHeight)
				maxHeight = currentHeight;

			if (currentDepth > maxDepth)
				maxDepth = currentDepth;
		}
	}

	maxWidth *= 2.0f;		maxHeight *= 2.0f;		maxDepth *= 2.0f;

	if (maxWidth > maxHeight && maxWidth > maxDepth)
		m_Width = maxWidth;

	else if (maxHeight > maxWidth && maxHeight > maxDepth)
		m_Width = maxHeight;

	else
		m_Width = maxDepth;
}

_float4 COctree::GetNewNodeCenter(_float4 vCenter, float width, int nodeID)
{
	_float4 vNodeCenter(0, 0, 0, 1.f);

	_float4 vCtr = vCenter;

	switch (nodeID)
	{
	case TOP_LEFT_FRONT:
		vNodeCenter = _float4(vCtr.x - width / 4, vCtr.y + width / 4, vCtr.z + width / 4, 1.f);
		break;

	case TOP_LEFT_BACK:
		vNodeCenter = _float4(vCtr.x - width / 4, vCtr.y + width / 4, vCtr.z - width / 4, 1.f);
		break;

	case TOP_RIGHT_BACK:
		vNodeCenter = _float4(vCtr.x + width / 4, vCtr.y + width / 4, vCtr.z - width / 4, 1.f);
		break;

	case TOP_RIGHT_FRONT:
		vNodeCenter = _float4(vCtr.x + width / 4, vCtr.y + width / 4, vCtr.z + width / 4, 1.f);
		break;

	case BOTTOM_LEFT_FRONT:
		vNodeCenter = _float4(vCtr.x - width / 4, vCtr.y - width / 4, vCtr.z + width / 4, 1.f);
		break;

	case BOTTOM_LEFT_BACK:
		vNodeCenter = _float4(vCtr.x - width / 4, vCtr.y - width / 4, vCtr.z - width / 4, 1.f);
		break;

	case BOTTOM_RIGHT_BACK:
		vNodeCenter = _float4(vCtr.x + width / 4, vCtr.y - width / 4, vCtr.z - width / 4, 1.f);
		break;

	case BOTTOM_RIGHT_FRONT:
		vNodeCenter = _float4(vCtr.x + width / 4, vCtr.y - width / 4, vCtr.z + width / 4, 1.f);
		break;
	}

	return vNodeCenter;
}

void COctree::CreateNewNode(CModel* pWorld, vector<tFaceList> pList, int triangleCount,
	_float4 vCenter, float width, int nodeID)
{
	if (!triangleCount) return;

	CModel* pTempWorld = new CModel(m_pDevice, m_pContext);

	pTempWorld->SetNumMesh(pWorld->GetNumMesh());
	auto TempMeshes = pTempWorld->GetMeshes();
	auto Meshes = pWorld->GetMeshes();

	for (_uint i = 0; i < pWorld->GetNumMesh(); i++)
	{
		CMesh* pObject = (*Meshes)[i];

		CMesh* newObject = new CMesh(m_pDevice, m_pContext);
		TempMeshes->push_back(newObject);

		(*TempMeshes)[i]->SetNumFaces(pList[i].totalFaceCount);
		(*TempMeshes)[i]->SetNumVertices(pObject->GetNumVertices());
		(*TempMeshes)[i]->SetVertices(pObject->GetVertices());
		//(*TempMeshes)[i]->SetNormals(pObject->GetNormals());
		//(*TempMeshes)[i]->SetTangents(pObject->GetTangents());
		//(*TempMeshes)[i]->SetTexcoords(pObject->GetTexcoords());
		(*TempMeshes)[i]->SetVertexBuffer(pObject->GetVertexBuffer());

		int index = 0;

		auto Faces = pObject->GetFaces();
		auto TempFaces = (*TempMeshes)[i]->GetFaces();
		TempFaces->resize(pObject->GetNumFaces());

		for (_uint j = 0; j < pObject->GetNumFaces(); j++)
		{
			if (pList[i].pFaceList[j])
			{
				(*TempFaces)[index] = new CMesh::tFace();
				(*TempFaces)[index]->VertexIndex[0] = (*Faces)[j]->VertexIndex[0];
				(*TempFaces)[index]->VertexIndex[1] = (*Faces)[j]->VertexIndex[1];
				(*TempFaces)[index]->VertexIndex[2] = (*Faces)[j]->VertexIndex[2];
			/*	(*TempFaces)[index]->TexCoordIndex[0] = (*Faces)[j]->TexCoordIndex[0];
				(*TempFaces)[index]->TexCoordIndex[1] = (*Faces)[j]->TexCoordIndex[1];
				(*TempFaces)[index]->TexCoordIndex[2] = (*Faces)[j]->TexCoordIndex[2];*/
				index++;
			}
		}
	}

	m_pOctreeNodes[nodeID] = new COctree(m_pDevice, m_pContext, m_pGameInstance, m_vTranslation,m_pPlayer);
	m_pOctreeNodes[nodeID]->m_pObjects = m_pObjects;
	m_pOctreeNodes[nodeID]->m_pObjects_Anim = m_pObjects_Anim;
	m_pOctreeNodes[nodeID]->m_pObjects_Door = m_pObjects_Door;
	m_pOctreeNodes[nodeID]->m_pObjects_Window = m_pObjects_Window;
	m_pOctreeNodes[nodeID]->m_pObjects_Shutter = m_pObjects_Shutter;
	_float4 vNodeCenter = GetNewNodeCenter(vCenter, width, nodeID);

	g_CurrentSubdivision++;

	m_pOctreeNodes[nodeID]->CreateNode(pTempWorld, triangleCount, vNodeCenter, width / 2);

	g_CurrentSubdivision--;

	Safe_Release(pTempWorld);
}

void COctree::CreateNode(CModel* pWorld, int numberOfTriangles, _float4 vCenter, float width)
{
	m_vCenter = vCenter;

	m_Width = width;

	auto NumMesh = pWorld->GetNumMesh();
	auto Meshes = pWorld->GetMeshes();

	if (/*(numberOfTriangles > g_MaxTriangles) &&*/ (g_CurrentSubdivision < g_MaxSubdivisions))
	{
		m_bSubDivided = true;

		vector<tFaceList> pList1(NumMesh);
		vector<tFaceList> pList2(NumMesh);
		vector<tFaceList> pList3(NumMesh);
		vector<tFaceList> pList4(NumMesh);
		vector<tFaceList> pList5(NumMesh);
		vector<tFaceList> pList6(NumMesh);
		vector<tFaceList> pList7(NumMesh);
		vector<tFaceList> pList8(NumMesh);

		_float4 vCtr = vCenter;

		for (_uint i = 0; i < NumMesh; i++)
		{
			CMesh* pObject = (*Meshes)[i];

			pList1[i].pFaceList.resize(pObject->GetNumFaces());
			pList2[i].pFaceList.resize(pObject->GetNumFaces());
			pList3[i].pFaceList.resize(pObject->GetNumFaces());
			pList4[i].pFaceList.resize(pObject->GetNumFaces());
			pList5[i].pFaceList.resize(pObject->GetNumFaces());
			pList6[i].pFaceList.resize(pObject->GetNumFaces());
			pList7[i].pFaceList.resize(pObject->GetNumFaces());
			pList8[i].pFaceList.resize(pObject->GetNumFaces());

			auto pVertices = pObject->GetVertices();
			auto pIndices = pObject->GetIndices();
			auto Faces = pObject->GetFaces();

			for (_uint j = 0; j < pObject->GetNumFaces(); j++)
			{
				for (int whichVertex = 0; whichVertex < 3; whichVertex++)
				{
					_float4 vPoint = pVertices[(*Faces)[j]->VertexIndex[whichVertex]];

					if ((vPoint.x <= vCtr.x) && (vPoint.y >= vCtr.y) && (vPoint.z >= vCtr.z))
						pList1[i].pFaceList[j] = true;

					if ((vPoint.x <= vCtr.x) && (vPoint.y >= vCtr.y) && (vPoint.z <= vCtr.z))
						pList2[i].pFaceList[j] = true;

					if ((vPoint.x >= vCtr.x) && (vPoint.y >= vCtr.y) && (vPoint.z <= vCtr.z))
						pList3[i].pFaceList[j] = true;

					if ((vPoint.x >= vCtr.x) && (vPoint.y >= vCtr.y) && (vPoint.z >= vCtr.z))
						pList4[i].pFaceList[j] = true;

					if ((vPoint.x <= vCtr.x) && (vPoint.y <= vCtr.y) && (vPoint.z >= vCtr.z))
						pList5[i].pFaceList[j] = true;

					if ((vPoint.x <= vCtr.x) && (vPoint.y <= vCtr.y) && (vPoint.z <= vCtr.z))
						pList6[i].pFaceList[j] = true;

					if ((vPoint.x >= vCtr.x) && (vPoint.y <= vCtr.y) && (vPoint.z <= vCtr.z))
						pList7[i].pFaceList[j] = true;

					if ((vPoint.x >= vCtr.x) && (vPoint.y <= vCtr.y) && (vPoint.z >= vCtr.z))
						pList8[i].pFaceList[j] = true;
				}
			}

			pList1[i].totalFaceCount = 0;
			pList2[i].totalFaceCount = 0;
			pList3[i].totalFaceCount = 0;
			pList4[i].totalFaceCount = 0;
			pList5[i].totalFaceCount = 0;
			pList6[i].totalFaceCount = 0;
			pList7[i].totalFaceCount = 0;
			pList8[i].totalFaceCount = 0;
		}

		int triCount1 = 0;
		int triCount2 = 0;
		int triCount3 = 0;
		int triCount4 = 0;
		int triCount5 = 0;
		int triCount6 = 0;
		int triCount7 = 0;
		int triCount8 = 0;

		for (_uint i = 0; i < NumMesh; i++)
		{
			for (_uint j = 0; j < (*Meshes)[i]->GetNumFaces(); j++)
			{
				if (pList1[i].pFaceList[j])
				{
					pList1[i].totalFaceCount++;
					triCount1++;
				}
				if (pList2[i].pFaceList[j])
				{
					pList2[i].totalFaceCount++;
					triCount2++;
				}
				if (pList3[i].pFaceList[j])
				{
					pList3[i].totalFaceCount++;
					triCount3++;
				}
				if (pList4[i].pFaceList[j])
				{
					pList4[i].totalFaceCount++;
					triCount4++;
				}
				if (pList5[i].pFaceList[j])
				{
					pList5[i].totalFaceCount++;
					triCount5++;
				}
				if (pList6[i].pFaceList[j])
				{
					pList6[i].totalFaceCount++;
					triCount6++;
				}
				if (pList7[i].pFaceList[j])
				{
					pList7[i].totalFaceCount++;
					triCount7++;
				}
				if (pList8[i].pFaceList[j])
				{
					pList8[i].totalFaceCount++;
					triCount8++;
				}
			}
		}
		CreateNewNode(pWorld, pList1, triCount1, vCenter, width, TOP_LEFT_FRONT);
		CreateNewNode(pWorld, pList2, triCount2, vCenter, width, TOP_LEFT_BACK);
		CreateNewNode(pWorld, pList3, triCount3, vCenter, width, TOP_RIGHT_BACK);
		CreateNewNode(pWorld, pList4, triCount4, vCenter, width, TOP_RIGHT_FRONT);
		CreateNewNode(pWorld, pList5, triCount5, vCenter, width, BOTTOM_LEFT_FRONT);
		CreateNewNode(pWorld, pList6, triCount6, vCenter, width, BOTTOM_LEFT_BACK);
		CreateNewNode(pWorld, pList7, triCount7, vCenter, width, BOTTOM_RIGHT_BACK);
		CreateNewNode(pWorld, pList8, triCount8, vCenter, width, BOTTOM_RIGHT_FRONT);
	}
	else
	{
		AssignTrianglesToNode(pWorld, numberOfTriangles);
	}
}

void COctree::AssignTrianglesToNode(CModel* pWorld, int numberOfTriangles)
{
	m_bSubDivided = false;

	m_TriangleCount = numberOfTriangles;

	m_pWorld = new CModel(m_pDevice, m_pContext);

	m_pWorld->SetNumMesh(pWorld->GetNumMesh());

	auto Meshes = pWorld->GetMeshes();
	auto NewMeshes = m_pWorld->GetMeshes();

	for (_uint i = 0; i < m_pWorld->GetNumMesh(); i++)
	{
		CMesh* pObject = (*Meshes)[i];

		CMesh* newObject = new CMesh(m_pDevice, m_pContext);
		newObject->SetNumVertices(pObject->GetNumVertices());
		//newObject->SetVertices(pObject->GetVertices());

		NewMeshes->push_back(newObject);

		_uint numOfFaces = pObject->GetNumFaces();

		(*NewMeshes)[i]->SetNumFaces(numOfFaces);
		(*NewMeshes)[i]->SetNumVertices(pObject->GetNumVertices());
		//(*NewMeshes)[i]->SetNormals(pObject->GetNormals());
		//(*NewMeshes)[i]->SetTangents(pObject->GetTangents());
		//(*NewMeshes)[i]->SetTexcoords(pObject->GetTexcoords());
		(*NewMeshes)[i]->Init_For_Octree();
		(*NewMeshes)[i]->SetVertexBuffer(pObject->GetVertexBuffer());

		auto pIndices = (*NewMeshes)[i]->GetIndices();
		auto Faces = (*Meshes)[i]->GetFaces();

		if (pIndices)
		{
			for (_uint j = 0; j < numOfFaces * 3; j += 3)
			{
				pIndices[j] = (*Faces)[j / 3]->VertexIndex[0];
				pIndices[j + 1] = (*Faces)[j / 3]->VertexIndex[1];
				pIndices[j + 2] = (*Faces)[j / 3]->VertexIndex[2];
			}
		}

		(*NewMeshes)[i]->Ready_Buffer_For_Octree(m_vTranslation);
		(*NewMeshes)[i]->Release_Dump();
	}

	if (m_pObjects)
	{
		for (auto& it : (*m_pObjects))
		{
			_float4 vPos;

			if (it->Get_Localized() == false)
				vPos = it->GetPosition();
			else
				vPos = it->GetPosition_Local_To_World();

			if (IsPointInsideCube(m_vCenter, m_Width, vPos))
			{
				m_vecProps.push_back(it);
			}
		}
	}

	if (m_pObjects_Anim)
	{
		for (auto& it : (*m_pObjects_Anim))
		{
			_float4 vPos;

			if (it->Get_Localized() == false)
				vPos = it->GetPosition();
			else
				vPos = it->GetPosition_Local_To_World();

			if (IsPointInsideCube(m_vCenter, m_Width, vPos))
			{
				m_vecProps.push_back(it);
			}
		}
	}
	
	if (m_pObjects_Door)
	{
		for (auto& it : (*m_pObjects_Door))
		{
			_float4 vPos;

			if (it->Get_Localized() == false)
				vPos = it->GetPosition();
			else
				vPos = it->GetPosition_Local_To_World();

			if (IsPointInsideCube(m_vCenter, m_Width, vPos))
			{
				m_vecProps.push_back(it);
			}
		}
	}
	
	if (m_pObjects_Window)
	{
		for (auto& it : (*m_pObjects_Window))
		{
			_float4 vPos;

			if (it->Get_Localized() == false)
				vPos = it->GetPosition();
			else
				vPos = it->GetPosition_Local_To_World();

			if (IsPointInsideCube(m_vCenter, m_Width, vPos))
			{
				m_vecProps.push_back(it);
			}
		}
	}
	
	if (m_pObjects_Shutter)
	{
		for (auto& it : (*m_pObjects_Shutter))
		{
			_float4 vPos;

			if (it->Get_Localized() == false)
				vPos = it->GetPosition();
			else
				vPos = it->GetPosition_Local_To_World();

			if (IsPointInsideCube(m_vCenter, m_Width, vPos))
			{
				m_vecProps.push_back(it);
			}
		}
	}

	m_DisplayListID = g_EndNodeCount;

	g_EndNodeCount++;
}

void COctree::Free()
{
	for (int i = 0; i < 8; i++)
	{
		if (m_pOctreeNodes[i])
		{
			m_pOctreeNodes[i]->Free();
			delete m_pOctreeNodes[i];
		}
	}

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pWorld);
}

void COctree::DrawOctree(COctree* pNode, CModel* pRootWorld, CShader* pShader)
{
	if (!pNode) return;

	auto vCenter = pNode->m_vCenter;
	vCenter.x += m_vTranslation.x;
	vCenter.y += m_vTranslation.y;
	vCenter.z += m_vTranslation.z;

	if (!m_pGameInstance->isInFrustum_WorldSpace_Cube(XMLoadFloat4(&vCenter), pNode->m_Width * 0.5f, 0.f))
	{
		return;
	}

	if (pNode->IsSubDivided())
	{
		DrawOctree(pNode->m_pOctreeNodes[TOP_LEFT_FRONT], pRootWorld, pShader);
		DrawOctree(pNode->m_pOctreeNodes[TOP_LEFT_BACK], pRootWorld, pShader);
		DrawOctree(pNode->m_pOctreeNodes[TOP_RIGHT_BACK], pRootWorld, pShader);
		DrawOctree(pNode->m_pOctreeNodes[TOP_RIGHT_FRONT], pRootWorld, pShader);
		DrawOctree(pNode->m_pOctreeNodes[BOTTOM_LEFT_FRONT], pRootWorld, pShader);
		DrawOctree(pNode->m_pOctreeNodes[BOTTOM_LEFT_BACK], pRootWorld, pShader);
		DrawOctree(pNode->m_pOctreeNodes[BOTTOM_RIGHT_BACK], pRootWorld, pShader);
		DrawOctree(pNode->m_pOctreeNodes[BOTTOM_RIGHT_FRONT], pRootWorld, pShader);
	}
	else
	{
		if (!pNode->m_pWorld) return;

		auto Meshes = pNode->m_pWorld->GetMeshes();

		for (_uint i = 0; i < pNode->m_pWorld->GetNumMesh(); ++i)
		{
			if ((*Meshes)[i]->GetNumIndices() != 0)
			{
				if (FAILED(pRootWorld->Bind_ShaderResource_Texture(pShader, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
					return;
				if (FAILED(pRootWorld->Bind_ShaderResource_Texture(pShader, "g_NormalTexture", static_cast<_uint>(i), aiTextureType_NORMALS)))
					return;

				if (FAILED(pRootWorld->Bind_ShaderResource_Texture(pShader, "g_ATOSTexture", static_cast<_uint>(i), aiTextureType_METALNESS)))
				{
					if (FAILED(pShader->Begin(0)))
						return;
				}
				else
				{
					if (FAILED(pShader->Begin(1)))
						return;
				}

				pNode->m_pWorld->Render(i);
			}
		}
	}
}

void COctree::DrawOctree_Thread(COctree* pNode)
{
	if (!pNode) return;

	auto vCenter = pNode->m_vCenter;
	vCenter.x += m_vTranslation.x;
	vCenter.y += m_vTranslation.y;
	vCenter.z += m_vTranslation.z;

	if (!m_pGameInstance->isInFrustum_WorldSpace_Cube(XMLoadFloat4(&vCenter), pNode->m_Width * 0.5f, 0.f))
	{
		return;
	}

	if (pNode->IsSubDivided())
	{
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_LEFT_FRONT], &m_vecEntryNode);
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_LEFT_BACK], &m_vecEntryNode);
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_RIGHT_BACK], &m_vecEntryNode);
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_RIGHT_FRONT], &m_vecEntryNode);
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_LEFT_FRONT], &m_vecEntryNode);
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_LEFT_BACK], &m_vecEntryNode);
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_RIGHT_BACK], &m_vecEntryNode);
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_RIGHT_FRONT], &m_vecEntryNode);
	}
	else
	{
		if (!pNode->m_pWorld) return;

		m_vecEntryNode.push_back(pNode->m_pWorld);
	}
}

void COctree::DrawOctree_Thread_Internal(COctree* pNode, vector<class CModel*>* vecModel)
{
	if (!pNode) return;

	auto vCenter = pNode->m_vCenter;
	vCenter.x += m_vTranslation.x;
	vCenter.y += m_vTranslation.y;
	vCenter.z += m_vTranslation.z;

	if (!m_pGameInstance->isInFrustum_WorldSpace_Cube(XMLoadFloat4(&vCenter), pNode->m_Width * 0.5f, 0.f))
	{
		return;
	}

	if (pNode->IsSubDivided())
	{
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_LEFT_FRONT], vecModel);
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_LEFT_BACK], vecModel);
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_RIGHT_BACK], vecModel);
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_RIGHT_FRONT], vecModel);
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_LEFT_FRONT], vecModel);
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_LEFT_BACK], vecModel);
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_RIGHT_BACK], vecModel);
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_RIGHT_FRONT], vecModel);
	}
	else
	{
		if (!pNode->m_pWorld) return;

		if (m_pPlayer)
		{
			auto vPlayerPos = m_pPlayer->GetPosition();
			auto vDelta = vPlayerPos - pNode->m_vCenter;
			auto fDelta = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vDelta)));

			if (fDelta > CULLING_DISTANCE)
				return;
		}

		(*vecModel).push_back(pNode->m_pWorld);

		pNode->m_bRender = true;

		for (int i = 0; i < pNode->m_vecProps.size(); ++i)
		{
			pNode->m_vecProps[i]->Set_Render(true);
		}
	}
}

void COctree::DrawOctree_1()
{
	auto pNode = m_pOctreeNodes[TOP_LEFT_FRONT];
	auto vCenter = pNode->m_vCenter;
	vCenter.x += m_vTranslation.x;
	vCenter.y += m_vTranslation.y;
	vCenter.z += m_vTranslation.z;

	if (!m_pGameInstance->isInFrustum_WorldSpace_Cube(XMLoadFloat4(&vCenter), pNode->m_Width * 0.5f, 0.f))
	{
		return;
	}

	if (pNode->IsSubDivided())
	{
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_LEFT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_LEFT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_RIGHT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_RIGHT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_LEFT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_LEFT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_RIGHT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_RIGHT_FRONT], &(pNode->m_vecEntryNode));
	}
	else
	{
		if (!pNode->m_pWorld) return;

		m_vecEntryNode.push_back(pNode->m_pWorld);
		pNode->m_bRender = true;

		for (int i = 0; i < pNode->m_vecProps.size(); ++i)
		{
			pNode->m_vecProps[i]->Set_Render(true);
		}
	}
}

void COctree::DrawOctree_2()
{
	auto pNode = m_pOctreeNodes[TOP_LEFT_BACK];
	auto vCenter = pNode->m_vCenter;
	vCenter.x += m_vTranslation.x;
	vCenter.y += m_vTranslation.y;
	vCenter.z += m_vTranslation.z;

	if (!m_pGameInstance->isInFrustum_WorldSpace_Cube(XMLoadFloat4(&vCenter), pNode->m_Width * 0.5f, 0.f))
	{
		return;
	}

	if (pNode->IsSubDivided())
	{
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_LEFT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_LEFT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_RIGHT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_RIGHT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_LEFT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_LEFT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_RIGHT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_RIGHT_FRONT], &(pNode->m_vecEntryNode));
	}
	else
	{
		if (!pNode->m_pWorld) return;

		m_vecEntryNode.push_back(pNode->m_pWorld);
		pNode->m_bRender = true;

		for (int i = 0; i < pNode->m_vecProps.size(); ++i)
		{
			pNode->m_vecProps[i]->Set_Render(true);
		}
	}
}

void COctree::DrawOctree_3()
{
	auto pNode = m_pOctreeNodes[TOP_RIGHT_BACK];
	auto vCenter = pNode->m_vCenter;
	vCenter.x += m_vTranslation.x;
	vCenter.y += m_vTranslation.y;
	vCenter.z += m_vTranslation.z;

	if (!m_pGameInstance->isInFrustum_WorldSpace_Cube(XMLoadFloat4(&vCenter), pNode->m_Width * 0.5f, 0.f))
	{
		return;
	}

	if (pNode->IsSubDivided())
	{
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_LEFT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_LEFT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_RIGHT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_RIGHT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_LEFT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_LEFT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_RIGHT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_RIGHT_FRONT], &(pNode->m_vecEntryNode));
	}
	else
	{
		if (!pNode->m_pWorld) return;

		m_vecEntryNode.push_back(pNode->m_pWorld);
		pNode->m_bRender = true;

		for (int i = 0; i < pNode->m_vecProps.size(); ++i)
		{
			pNode->m_vecProps[i]->Set_Render(true);
		}
	}
}

void COctree::DrawOctree_4()
{
	auto pNode = m_pOctreeNodes[TOP_RIGHT_FRONT];
	auto vCenter = pNode->m_vCenter;
	vCenter.x += m_vTranslation.x;
	vCenter.y += m_vTranslation.y;
	vCenter.z += m_vTranslation.z;

	if (!m_pGameInstance->isInFrustum_WorldSpace_Cube(XMLoadFloat4(&vCenter), pNode->m_Width * 0.5f, 0.f))
	{
		return;
	}

	if (pNode->IsSubDivided())
	{
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_LEFT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_LEFT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_RIGHT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_RIGHT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_LEFT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_LEFT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_RIGHT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_RIGHT_FRONT], &(pNode->m_vecEntryNode));
	}
	else
	{
		if (!pNode->m_pWorld) return;

		m_vecEntryNode.push_back(pNode->m_pWorld);
		pNode->m_bRender = true;

		for (int i = 0; i < pNode->m_vecProps.size(); ++i)
		{
			pNode->m_vecProps[i]->Set_Render(true);
		}
	}
}

void COctree::DrawOctree_5()
{
	auto pNode = m_pOctreeNodes[BOTTOM_LEFT_FRONT];
	auto vCenter = pNode->m_vCenter;
	vCenter.x += m_vTranslation.x;
	vCenter.y += m_vTranslation.y;
	vCenter.z += m_vTranslation.z;

	if (!m_pGameInstance->isInFrustum_WorldSpace_Cube(XMLoadFloat4(&vCenter), pNode->m_Width * 0.5f, 0.f))
	{
		return;
	}

	if (pNode->IsSubDivided())
	{
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_LEFT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_LEFT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_RIGHT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_RIGHT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_LEFT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_LEFT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_RIGHT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_RIGHT_FRONT], &(pNode->m_vecEntryNode));
	}
	else
	{
		if (!pNode->m_pWorld) return;

		m_vecEntryNode.push_back(pNode->m_pWorld);
		pNode->m_bRender = true;

		for (int i = 0; i < pNode->m_vecProps.size(); ++i)
		{
			pNode->m_vecProps[i]->Set_Render(true);
		}
	}
}

void COctree::DrawOctree_6()
{
	auto pNode = m_pOctreeNodes[BOTTOM_LEFT_BACK];
	auto vCenter = pNode->m_vCenter;
	vCenter.x += m_vTranslation.x;
	vCenter.y += m_vTranslation.y;
	vCenter.z += m_vTranslation.z;

	if (!m_pGameInstance->isInFrustum_WorldSpace_Cube(XMLoadFloat4(&vCenter), pNode->m_Width * 0.5f, 0.f))
	{
		return;
	}

	if (pNode->IsSubDivided())
	{
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_LEFT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_LEFT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_RIGHT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_RIGHT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_LEFT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_LEFT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_RIGHT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_RIGHT_FRONT], &(pNode->m_vecEntryNode));
	}
	else
	{
		if (!pNode->m_pWorld) return;

		m_vecEntryNode.push_back(pNode->m_pWorld);
		pNode->m_bRender = true;

		for (int i = 0; i < pNode->m_vecProps.size(); ++i)
		{
			pNode->m_vecProps[i]->Set_Render(true);
		}
	}
}

void COctree::DrawOctree_7()
{
	auto pNode = m_pOctreeNodes[BOTTOM_RIGHT_BACK];
	auto vCenter = pNode->m_vCenter;
	vCenter.x += m_vTranslation.x;
	vCenter.y += m_vTranslation.y;
	vCenter.z += m_vTranslation.z;

	if (!m_pGameInstance->isInFrustum_WorldSpace_Cube(XMLoadFloat4(&vCenter), pNode->m_Width * 0.5f, 0.f))
	{
		return;
	}

	if (pNode->IsSubDivided())
	{
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_LEFT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_LEFT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_RIGHT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_RIGHT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_LEFT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_LEFT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_RIGHT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_RIGHT_FRONT], &(pNode->m_vecEntryNode));
	}
	else
	{
		if (!pNode->m_pWorld) return;

		m_vecEntryNode.push_back(pNode->m_pWorld);
		pNode->m_bRender = true;

		for (int i = 0; i < pNode->m_vecProps.size(); ++i)
		{
			pNode->m_vecProps[i]->Set_Render(true);
		}
	}
}

void COctree::DrawOctree_8()
{
	auto pNode = m_pOctreeNodes[BOTTOM_RIGHT_FRONT];
	auto vCenter = pNode->m_vCenter;
	vCenter.x += m_vTranslation.x;
	vCenter.y += m_vTranslation.y;
	vCenter.z += m_vTranslation.z;

	if (!m_pGameInstance->isInFrustum_WorldSpace_Cube(XMLoadFloat4(&vCenter), pNode->m_Width * 0.5f, 0.f))
	{
		return;
	}

	if (pNode->IsSubDivided())
	{
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_LEFT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_LEFT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_RIGHT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[TOP_RIGHT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_LEFT_FRONT], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_LEFT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_RIGHT_BACK], &(pNode->m_vecEntryNode));
		DrawOctree_Thread_Internal(pNode->m_pOctreeNodes[BOTTOM_RIGHT_FRONT], &(pNode->m_vecEntryNode));
	}
	else
	{
		if (!pNode->m_pWorld) return;

		m_vecEntryNode.push_back(pNode->m_pWorld);
		pNode->m_bRender = true;

		for (int i = 0; i < pNode->m_vecProps.size(); ++i)
		{
			pNode->m_vecProps[i]->Set_Render(true);
		}
	}
}

void COctree::Render_Node(CModel* pRootWorld, CShader* pShader)
{
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < m_pOctreeNodes[i]->m_vecEntryNode.size(); ++j)
		{
			auto Meshes = m_pOctreeNodes[i]->m_vecEntryNode[j]->GetMeshes();

			for (_uint k = 0; k < m_pOctreeNodes[i]->m_vecEntryNode[j]->GetNumMesh(); ++k)
			{
				if ((*Meshes)[k]->GetNumIndices() != 0)
				{
					if (FAILED(pRootWorld->Bind_ShaderResource_Texture(pShader, "g_AlphaTexture", static_cast<_uint>(k), aiTextureType_METALNESS)))
					{
						_bool isAlphaTexture = false;
						if (FAILED(pShader->Bind_RawValue("g_isAlphaTexture", &isAlphaTexture, sizeof(_bool))))
							return;
					}
					else
					{
						continue;
					}

					if (FAILED(pRootWorld->Bind_ShaderResource_Texture(pShader, "g_AOTexture", static_cast<_uint>(k), aiTextureType_SHININESS)))
					{
						_bool isAOTexture = false;
						if (FAILED(pShader->Bind_RawValue("g_isAOTexture", &isAOTexture, sizeof(_bool))))
							return;
					}
					else
					{
						continue;
					}

					if (FAILED(pRootWorld->Bind_ShaderResource_Texture(pShader, "g_DiffuseTexture", static_cast<_uint>(k), aiTextureType_DIFFUSE)))
						return;
					if (FAILED(pRootWorld->Bind_ShaderResource_Texture(pShader, "g_NormalTexture", static_cast<_uint>(k), aiTextureType_NORMALS)))
						return;

					if (FAILED(pRootWorld->Bind_ShaderResource_Texture(pShader, "g_EmissiveTexture", static_cast<_uint>(k), aiTextureType_EMISSIVE)))
					{
						_bool isEmissive = false;
						if (FAILED(pShader->Bind_RawValue("g_isEmissiveTexture", &isEmissive, sizeof(_bool))))
							return;
					}
					else
					{
						_bool isEmissive = true;
						if (FAILED(pShader->Bind_RawValue("g_isEmissiveTexture", &isEmissive, sizeof(_bool))))
							return;
					}

					if (FAILED(pShader->Begin(0)))
						return;

					m_pOctreeNodes[i]->m_vecEntryNode[j]->Render(k);
				}
			}
		}
	}
}

void COctree::Render_Node_Blend(CModel* pRootWorld, CShader* pShader)
{
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < m_pOctreeNodes[i]->m_vecEntryNode.size(); ++j)
		{
			auto Meshes = m_pOctreeNodes[i]->m_vecEntryNode[j]->GetMeshes();

			for (_uint k = 0; k < m_pOctreeNodes[i]->m_vecEntryNode[j]->GetNumMesh(); ++k)
			{
				if ((*Meshes)[k]->GetNumIndices() != 0)
				{
					if (FAILED(pRootWorld->Bind_ShaderResource_Texture(pShader, "g_AlphaTexture", static_cast<_uint>(k), aiTextureType_METALNESS)))
					{
						continue;
					}
					else
					{
						_bool isAlphaTexture = true;
						if (FAILED(pShader->Bind_RawValue("g_isAlphaTexture", &isAlphaTexture, sizeof(_bool))))
							return;
					}

					if (FAILED(pRootWorld->Bind_ShaderResource_Texture(pShader, "g_AOTexture", static_cast<_uint>(k), aiTextureType_SHININESS)))
					{
						continue;
					}
					else
					{
						_bool isAOTexture = true;
						if (FAILED(pShader->Bind_RawValue("g_isAOTexture", &isAOTexture, sizeof(_bool))))
							return;
					}

					if (FAILED(pRootWorld->Bind_ShaderResource_Texture(pShader, "g_DiffuseTexture", static_cast<_uint>(k), aiTextureType_DIFFUSE)))
						return;
					if (FAILED(pRootWorld->Bind_ShaderResource_Texture(pShader, "g_NormalTexture", static_cast<_uint>(k), aiTextureType_NORMALS)))
						return;

					if (FAILED(pRootWorld->Bind_ShaderResource_Texture(pShader, "g_EmissiveTexture", static_cast<_uint>(k), aiTextureType_EMISSIVE)))
					{
						_bool isEmissive = false;
						if (FAILED(pShader->Bind_RawValue("g_isEmissiveTexture", &isEmissive, sizeof(_bool))))
							return;
					}
					else
					{
						_bool isEmissive = true;
						if (FAILED(pShader->Bind_RawValue("g_isEmissiveTexture", &isEmissive, sizeof(_bool))))
							return;
					}

					if (FAILED(pShader->Begin(1)))
						return;

					m_pOctreeNodes[i]->m_vecEntryNode[j]->Render(k);
				}
			}
		}

		m_pOctreeNodes[i]->m_bRender = false;
		m_pOctreeNodes[i]->m_vecEntryNode.clear();
	}
}

void COctree::Render_Node_LightDepth_Dir(CModel* pRootWorld, CShader* pShader)
{
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < m_pOctreeNodes[i]->m_vecEntryNode.size(); ++j)
		{
			auto Meshes = m_pOctreeNodes[i]->m_vecEntryNode[j]->GetMeshes();

			for (_uint k = 0; k < m_pOctreeNodes[i]->m_vecEntryNode[j]->GetNumMesh(); ++k)
			{
				if ((*Meshes)[k]->GetNumIndices() != 0)
				{
					if (FAILED(pRootWorld->Bind_ShaderResource_Texture(pShader, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
						return;

					if (FAILED(pShader->Begin(2)))
						return;

					m_pOctreeNodes[i]->m_vecEntryNode[j]->Render(k);
				}
			}
		}
	}
}

void COctree::Render_Node_LightDepth_Spot(CModel* pRootWorld, CShader* pShader)
{
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < m_pOctreeNodes[i]->m_vecEntryNode.size(); ++j)
		{
			auto Meshes = m_pOctreeNodes[i]->m_vecEntryNode[j]->GetMeshes();

			for (_uint k = 0; k < m_pOctreeNodes[i]->m_vecEntryNode[j]->GetNumMesh(); ++k)
			{
				if ((*Meshes)[k]->GetNumIndices() != 0)
				{
					if (FAILED(pRootWorld->Bind_ShaderResource_Texture(pShader, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
						return;

					if (FAILED(pShader->Begin(2)))
						return;

					m_pOctreeNodes[i]->m_vecEntryNode[j]->Render(k);
				}
			}
		}
	}
}

void COctree::Render_Node_LightDepth_Point(CModel* pRootWorld, CShader* pShader)
{
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < m_pOctreeNodes[i]->m_vecEntryNode.size(); ++j)
		{
			auto Meshes = m_pOctreeNodes[i]->m_vecEntryNode[j]->GetMeshes();

			for (_uint k = 0; k < m_pOctreeNodes[i]->m_vecEntryNode[j]->GetNumMesh(); ++k)
			{
				if ((*Meshes)[k]->GetNumIndices() != 0)
				{
					if (FAILED(pRootWorld->Bind_ShaderResource_Texture(pShader, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
						return;

					if (FAILED(pShader->Begin(3)))
						return;

					m_pOctreeNodes[i]->m_vecEntryNode[j]->Render(k);
				}
			}
		}
	}
}

void COctree::Set_Props_Layer(_int iLevel)
{
	m_pObjects = m_pGameInstance->Find_Layer(iLevel, L"Layer_Obj");
	m_pObjects_Anim = m_pGameInstance->Find_Layer(iLevel, L"Layer_InteractObj");
	m_pObjects_Door = m_pGameInstance->Find_Layer(iLevel, L"Layer_Door");
	m_pObjects_Window = m_pGameInstance->Find_Layer(iLevel, L"Layer_Window");
	m_pObjects_Shutter = m_pGameInstance->Find_Layer(iLevel, L"Layer_HShutter");
}
