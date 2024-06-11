#include "..\Public\Navigation.h"
#include "Cell.h"

#include "Shader.h"
#include "GameInstance.h"
#include "Model.h"
#include "Mesh.h"

_float4x4		CNavigation::m_WorldMatrix{};

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CNavigation::CNavigation(const CNavigation& rhs)
	: CComponent{ rhs }
	, m_Cells{ rhs.m_Cells }
#ifdef _DEBUG
	, m_pShader{ rhs.m_pShader }

#endif
{

#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif

	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);
}

HRESULT CNavigation::Initialize_Prototype(const wstring& strDataFile)
{
	_ulong		dwByte = { 0 };
	HANDLE		hFile = CreateFile(strDataFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		_float3		vPoints[3];

		ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

		if (0 == dwByte)
			break;

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, static_cast<_uint>(m_Cells.size()));
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.emplace_back(pCell);

		auto Point1 = pCell->Get_Point_Float4(0);
		auto Point2 = pCell->Get_Point_Float4(1);
		auto Point3 = pCell->Get_Point_Float4(2);

		auto Centroid = CalculateCentroid(Point1, Point2, Point3);

		m_vecNavCell_Point.push_back(Centroid);
	}

	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;

	m_pGameInstance->SetNavCellPoint(&m_vecNavCell_Point);
	m_pGameInstance->SetCells(&m_Cells);

	/*filesystem::path FullPath("strModelFilePath");

	string strParentsPath = FullPath.parent_path().string();
	string strFileName = FullPath.stem().string();

	string strNewPath = strParentsPath + "/" + strFileName + ".bin";

	ifstream ifs(strNewPath.c_str(), ios::binary);

	if (true == ifs.fail())
	{
		MSG_BOX(TEXT("Failed To OpenFile"));

		return E_FAIL;
	}

	_int iNumBones = 0;
	_int iNumMeshes = 0;
	CModel::MODEL_TYPE eModelType;

	ifs.read(reinterpret_cast<_char*>(&eModelType), sizeof(CModel::MODEL_TYPE));
	ifs.read(reinterpret_cast<_char*>(&iNumBones), sizeof(_uint));
	ifs.read(reinterpret_cast<_char*>(&iNumMeshes), sizeof(_uint));
	ifs.read(reinterpret_cast<_char*>(&eModelType), sizeof(CModel::MODEL_TYPE));

	_char szName[MAX_PATH] = { "" };
	CMesh::MESH_DESC MeshDesc;

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		ifs.read(reinterpret_cast<_char*>(szName), sizeof(_char) * MAX_PATH);
		MeshDesc.strName = szName;
		ifs.read(reinterpret_cast<_char*>(&MeshDesc.iMaterialIndex), sizeof(_uint));

		ifs.read(reinterpret_cast<_char*>(&MeshDesc.iNumVertices), sizeof(_uint));

		VTXANIMMESH Vertex{};
		for (_uint i = 0; i < MeshDesc.iNumVertices; ++i)
		{
			ifs.read(reinterpret_cast<_char*>(&Vertex.vPosition), sizeof(_float3));
			ifs.read(reinterpret_cast<_char*>(&Vertex.vNormal), sizeof(_float3));
			ifs.read(reinterpret_cast<_char*>(&Vertex.vTexcoord), sizeof(_float2));
			ifs.read(reinterpret_cast<_char*>(&Vertex.vTangent), sizeof(_float3));

			if (CModel::MODEL_TYPE::TYPE_ANIM == eModelType)
			{
				ifs.read(reinterpret_cast<_char*>(&Vertex.vBlendIndices), sizeof(XMUINT4));
				ifs.read(reinterpret_cast<_char*>(&Vertex.vBlendWeights), sizeof(_float4));
			}

			MeshDesc.Vertices.push_back(Vertex);
		}

		ifs.read(reinterpret_cast<_char*>(&MeshDesc.iNumIndices), sizeof(_uint));
		_uint iIndex = { 0 };
		for (_uint i = 0; i < MeshDesc.iNumIndices; ++i)
		{
			ifs.read(reinterpret_cast<_char*>(&iIndex), sizeof(_uint));

			MeshDesc.Indices.push_back(iIndex);
		}

		ifs.read(reinterpret_cast<_char*>(&MeshDesc.iNumBones), sizeof(_uint));
		_uint iBoneIndex = { 0 };
		for (_uint i = 0; i < MeshDesc.iNumBones; ++i)
		{
			ifs.read(reinterpret_cast<_char*>(&iBoneIndex), sizeof(_uint));

			MeshDesc.Bones.push_back(iBoneIndex);
		}

		ifs.read(reinterpret_cast<_char*>(&MeshDesc.iNumOffsetMatrices), sizeof(_uint));
		_float4x4 OffsetMatrix;
		for (_uint i = 0; i < MeshDesc.iNumOffsetMatrices; ++i)
		{
			ifs.read(reinterpret_cast<_char*>(&OffsetMatrix), sizeof(_float4x4));

			MeshDesc.OffsetMatrices.push_back(OffsetMatrix);
		}
	}

	_float3		vPoints[3];

	for (int i = 0; i < MeshDesc.iNumIndices; i += 3)
	{
		vPoints[i] = MeshDesc.Vertices[MeshDesc.Indices[i]].vPosition;
		vPoints[i + 1] = MeshDesc.Vertices[MeshDesc.Indices[i + 1]].vPosition;
		vPoints[i + 2] = MeshDesc.Vertices[MeshDesc.Indices[i + 2]].vPosition;

		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, static_cast<_uint>(m_Cells.size()));
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.emplace_back(pCell);

		auto Point1 = pCell->Get_Point_Float4(0);
		auto Point2 = pCell->Get_Point_Float4(1);
		auto Point3 = pCell->Get_Point_Float4(2);

		auto Centroid = CalculateCentroid(Point1, Point2, Point3);

		m_vecNavCell_Point.push_back(Centroid);
	}*/

	//if (FAILED(SetUp_Neighbors()))
	//	return E_FAIL;

	//m_pGameInstance->SetNavCellPoint(&m_vecNavCell_Point);
	//m_pGameInstance->SetCells(&m_Cells);

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		m_iCurrentIndex = ((NAVIGATION_DESC*)pArg)->iCurrentIndex;
	}

	return S_OK;
}

void CNavigation::Tick(_fmatrix WorldMatrix)
{
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

_bool CNavigation::isMove(_fvector vPosition)
{
	if (-1 == m_iCurrentIndex)
		return false;

	_int		iNeighborIndex = { -1 };

	if (true == m_Cells[m_iCurrentIndex]->isIn(vPosition, XMLoadFloat4x4(&m_WorldMatrix), &iNeighborIndex))
		return true;
	else
	{
		if (-1 == iNeighborIndex)
			return false;

		else
		{
			while (true)
			{
				if (-1 == iNeighborIndex)
					return false;

				if (true == m_Cells[iNeighborIndex]->isIn(vPosition, XMLoadFloat4x4(&m_WorldMatrix), &iNeighborIndex))
				{
					m_iCurrentIndex = iNeighborIndex;
					return true;
				}
			}
		}
	}
}

#ifdef _DEBUG

HRESULT CNavigation::Render()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	m_pShader->Begin(0);

	if (-1 == m_iCurrentIndex)
	{

		for (auto& pCell : m_Cells)
		{
			if (nullptr != pCell)
				pCell->Render();
		}
	}
	else
		m_Cells[m_iCurrentIndex]->Render();

	return S_OK;
}

#endif

HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
				pSourCell->SetUp_Neighbor(CCell::LINE_AB, pDestCell);

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
				pSourCell->SetUp_Neighbor(CCell::LINE_BC, pDestCell);


			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
				pSourCell->SetUp_Neighbor(CCell::LINE_CA, pDestCell);
		}
	}

	return S_OK;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strDataFile)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strDataFile)))
	{
		MSG_BOX(TEXT("Failed To Created : CNavigation"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CNavigation"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

_vector CNavigation::GetPlane()
{
	return m_Cells[m_iCurrentIndex]->GetPlane();
}

_float CNavigation::GetCellHeight(_float4 Position)
{
	_vector Plane = m_Cells[m_iCurrentIndex]->GetPlane();
	_float4 PlaneFloat4;
	XMStoreFloat4(&PlaneFloat4, Plane);

	_float YPos = (-PlaneFloat4.x * Position.x - PlaneFloat4.z * Position.z - PlaneFloat4.w) / PlaneFloat4.y;
	return YPos;
}

_float4 CNavigation::GetSlidingNormal()
{
	return m_Cells[m_iCurrentIndex]->GetSlidingNormal();
}

_bool CNavigation::GetCurrentSlidingVector(_fvector vPosition, _float4* SlidingVector)
{
	if (-1 == m_iCurrentIndex)
		return false;

	_int		iNeighborIndex = { -1 };

	m_Cells[m_iCurrentIndex]->isIn(vPosition, XMLoadFloat4x4(&m_WorldMatrix), &iNeighborIndex, &m_SlidingNormal, &m_bIsIsolated);
	*SlidingVector = m_SlidingNormal;
	return true;
}

_bool CNavigation::FindCell(_fvector vPosition)
{
	_int		iNeighborIndex = { -1 };

	for (int i = 0; i < m_iNumFaces; ++i)
	{
		if (true == m_Cells[i]->isIn(vPosition, XMLoadFloat4x4(&m_WorldMatrix), &iNeighborIndex))
		{
			m_iCurrentIndex = i;
			return true;
		}
	}

	return false;
}


void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif
}
