#include "GameInstance.h"
#include "PathFinder.h"
#include "Cell.h"
#include "Navigation.h"

CPathFinder::CPathFinder() : m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

CPathFinder::CPathFinder(const CPathFinder& rhs)
{
}

HRESULT CPathFinder::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPathFinder::Initialize(void* pArg)
{
	return S_OK;
}

void CPathFinder::Init_PathFinder()
{
	m_Cells = m_pGameInstance->GetCells();
	m_vecNavCell_Point = m_pGameInstance->GetNavCellPoint();

	m_iCellCount = m_Cells->size();

	m_closedList = new bool[m_iCellCount];

	memset(m_closedList, false, m_iCellCount);

	m_cellDetails = new CELL[m_iCellCount];
}

void CPathFinder::Init_Visibility_Optimization()
{
	m_vecLeft_Vertices.resize(m_Path.size()+1);
	m_vecRight_Vertices.resize(m_Path.size()+1);

	Path_Optimization();
}

void CPathFinder::Initiate_PathFinding(_uint StartCell, _uint EndCell,_float4 vStartPos)
{
	m_vStartPos = vStartPos;

	typedef pair<_float, _uint > pPair;

	if (StartCell == EndCell)
		return;

	memset(m_closedList, false, m_iCellCount);

	for (int i = 0; i < m_iCellCount; i++)
	{
		m_cellDetails[i].f = FLT_MAX;
		m_cellDetails[i].g = FLT_MAX;
		m_cellDetails[i].h = FLT_MAX;
		m_cellDetails[i].parent = -1;
	}

	m_cellDetails[StartCell].f = 0.0;
	m_cellDetails[StartCell].g = 0.0;
	m_cellDetails[StartCell].h = 0.0;
	m_cellDetails[StartCell].parent = StartCell;

	set<pPair> openList;
	openList.insert(make_pair(0.f, StartCell));

	bool foundDest = false;

	int iIndex;

	while (!openList.empty())
	{
		pPair p = *openList.begin();

		openList.erase(openList.begin());

		iIndex = p.second;
		m_closedList[iIndex] = true;

		_float gNew, hNew, fNew;
		_int iNeighbor_1 = (*m_Cells)[iIndex]->GetNeighborIndices()[0];
		_int iNeighbor_2 = (*m_Cells)[iIndex]->GetNeighborIndices()[1];
		_int iNeighbor_3 = (*m_Cells)[iIndex]->GetNeighborIndices()[2];

		_float fDist_1;
		if (iNeighbor_1 != -1)
			fDist_1 = Calc_Dist((*m_vecNavCell_Point)[iIndex], (*m_vecNavCell_Point)[iNeighbor_1]);
		else
			fDist_1 = 0.f;

		_float fDist_2;
		if (iNeighbor_2 != -1)
			fDist_2 = Calc_Dist((*m_vecNavCell_Point)[iIndex], (*m_vecNavCell_Point)[iNeighbor_2]);
		else
			fDist_2 = 0.f;

		_float fDist_3;
		if (iNeighbor_3 != -1)
			fDist_3 = Calc_Dist((*m_vecNavCell_Point)[iIndex], (*m_vecNavCell_Point)[iNeighbor_3]);
		else
			fDist_3 = 0.f;

		//총 3방향 존재
		//----------- 1st Successor ------------

		if (iNeighbor_1 != -1 && IsValid(iNeighbor_1) == true)
		{

			if (iNeighbor_1 == EndCell)
			{
				m_cellDetails[iNeighbor_1].parent = iIndex;

				TracePath(StartCell, EndCell);
				foundDest = true;

				Init_Visibility_Optimization();

				return;
			}

			else if (IsUnBlocked(iNeighbor_1) == true)
			{
				if (m_closedList[iNeighbor_1] == false)
				{
					gNew = m_cellDetails[iIndex].g + fDist_1;
					hNew = CalculateHValue(iNeighbor_1, EndCell);
					fNew = gNew + hNew;

					if (m_cellDetails[iNeighbor_1].f == FLT_MAX || m_cellDetails[iNeighbor_1].f > fNew)
					{
						openList.insert(make_pair(
							fNew, iNeighbor_1));

						m_cellDetails[iNeighbor_1].f = fNew;
						m_cellDetails[iNeighbor_1].g = gNew;
						m_cellDetails[iNeighbor_1].h = hNew;
						m_cellDetails[iNeighbor_1].parent = iIndex;
					}
				}
			}
		}

		//----------- 2nd Successor ------------

		if (iNeighbor_2 != -1 && IsValid(iNeighbor_2) == true)
		{

			if (iNeighbor_2 == EndCell)
			{
				m_cellDetails[iNeighbor_2].parent = iIndex;

				TracePath(StartCell, EndCell);
				foundDest = true;

				Init_Visibility_Optimization();

				return;
			}

			else if (IsUnBlocked(iNeighbor_2) == true)
			{
				if (m_closedList[iNeighbor_2] == false)
				{
					gNew = m_cellDetails[iIndex].g + fDist_2;
					hNew = CalculateHValue(iNeighbor_2, EndCell);
					fNew = gNew + hNew;

					if (m_cellDetails[iNeighbor_2].f == FLT_MAX || m_cellDetails[iNeighbor_2].f > fNew)
					{
						openList.insert(make_pair(
							fNew, iNeighbor_2));

						m_cellDetails[iNeighbor_2].f = fNew;
						m_cellDetails[iNeighbor_2].g = gNew;
						m_cellDetails[iNeighbor_2].h = hNew;
						m_cellDetails[iNeighbor_2].parent = iIndex;
					}
				}
			}
		}

		//----------- 3rd Successor ------------

		if (iNeighbor_3 != -1 && IsValid(iNeighbor_3) == true)
		{

			if (iNeighbor_3 == EndCell)
			{
				m_cellDetails[iNeighbor_3].parent = iIndex;

				TracePath(StartCell, EndCell);
				foundDest = true;

				Init_Visibility_Optimization();

				return;
			}

			else if (IsUnBlocked(iNeighbor_3) == true)
			{
				if (m_closedList[iNeighbor_3] == false)
				{
					gNew = m_cellDetails[iIndex].g + fDist_3;
					hNew = CalculateHValue(iNeighbor_3, EndCell);
					fNew = gNew + hNew;

					if (m_cellDetails[iNeighbor_3].f == FLT_MAX
						|| m_cellDetails[iNeighbor_3].f > fNew) {
						openList.insert(make_pair(
							fNew, iNeighbor_3));

						m_cellDetails[iNeighbor_3].f = fNew;
						m_cellDetails[iNeighbor_3].g = gNew;
						m_cellDetails[iNeighbor_3].h = hNew;
						m_cellDetails[iNeighbor_3].parent = iIndex;
					}
				}
			}
		}
	}

	return;
}

bool CPathFinder::IsValid(_uint Index)
{
	return true;
}

void CPathFinder::TracePath(_uint Start, _uint End)
{
	if (Start < 0 || End < 0)
		return;

	if (End > m_iCellCount)
		return;

	while (!(m_cellDetails[End].parent == Start))
	{
		m_Path.push(End);
		m_Path_Optimization.push_back(End);
		int temp = m_cellDetails[End].parent;
		End = temp;
	}

	m_Path.push(End);
	m_Path_Optimization.push_back(End);

	reverse(m_Path_Optimization.begin(), m_Path_Optimization.end());

	return;
}

_float CPathFinder::CalculateHValue(_uint Start, _uint End)
{
	_float dx = (*m_vecNavCell_Point)[End].x - (*m_vecNavCell_Point)[Start].x;
	_float dy = (*m_vecNavCell_Point)[End].y - (*m_vecNavCell_Point)[Start].y;
	_float dz = (*m_vecNavCell_Point)[End].z - (*m_vecNavCell_Point)[Start].z;
	return (_float)sqrt(dx * dx + dy * dy + dz * dz);
}

_float CPathFinder::Calc_Dist(_float4 vSrc, _float4 vDst)
{
	auto Delta_float4 = vSrc - vDst;
	
	_vector Delta_vector = XMLoadFloat4(&Delta_float4);
	_float fDist = XMVectorGetX(XMVector3Length(Delta_vector));

	return fDist;
}

void CPathFinder::Path_Optimization()
{
#pragma region 구버전
	_float4 apex = m_vStartPos;
	int left = 1;
	int right = 1;
	int nextleft = 0;
	int nextright = 0;

	for (int i = 0; i < m_Path.size() - 1; i++)
	{
		_uint Cell_Index = m_Path_Optimization[i];
		auto Neighbors = (*m_Cells)[Cell_Index]->GetNeighborIndices();

		for (int j = 0; j < 3; j++)
		{
			if (Neighbors[j] == m_Path_Optimization[i + 1])
			{
				int k = j + 1 >= 3 ? 0 : j + 1;

				auto Point1 = (*m_Cells)[m_Path_Optimization[i]]->Get_Point_Float4(j);
				auto Point2 = (*m_Cells)[m_Path_Optimization[i]]->Get_Point_Float4(k);
				m_vecLeft_Vertices[i + 1] = Point1;
				m_vecRight_Vertices[i + 1] = Point2;

				break;
			}
		}
	}

	// Initialise portal vertices first point.
	auto First_Cell = (*m_Cells)[m_Path_Optimization[0]];
	for (int j = 0; j < 3; j++)
	{
		if ((First_Cell->Get_Point_Float4(j)) != m_vecLeft_Vertices[1]
			&& (First_Cell->Get_Point_Float4(j)) != m_vecRight_Vertices[1])
		{
			m_vecLeft_Vertices[0] = First_Cell->Get_Point_Float4(j);
			m_vecRight_Vertices[0] = First_Cell->Get_Point_Float4(j);
		}
	}

	// Initialise portal vertices last point.
	auto Last_Cell = (*m_Cells)[m_Path_Optimization[m_Path_Optimization.size()-1]];
	for (int j = 0;j < 3; j++)
	{
		if (Last_Cell->Get_Point_Float4(j)
			!= m_vecLeft_Vertices[m_Path.size()-1]
			&& Last_Cell->Get_Point_Float4(j)
			!= m_vecRight_Vertices[m_Path.size() - 1])
		{
			m_vecLeft_Vertices[m_Path.size()] = Last_Cell->Get_Point_Float4(j);
			m_vecRight_Vertices[m_Path.size()] = Last_Cell->Get_Point_Float4(j);
		}
	}

	// Step through channel.
	for (int i = 2; i <= m_Path.size() - 1; i++)
	{
		//If new left vertex is different, process.
		if ((m_vecLeft_Vertices[i] != m_vecLeft_Vertices[left])
			&& i > left)
		{
			_float4 newSide = m_vecLeft_Vertices[i] - apex;
			newSide.w = 1.f;

			// If new side does not widen funnel, update.
			if (VectorSign(newSide,
				m_vecLeft_Vertices[left] - apex) > 0)
			{
				// If new side crosses other side, update apex.
				if (VectorSign(newSide,
					m_vecRight_Vertices[right] - apex) > 0)
				{
					// Find next vertex.
					//auto next = right + 1;
					nextright = right;

					for (int j = nextright; j <= m_Path.size(); j++)
					{
						if (m_vecRight_Vertices[j] != m_vecRight_Vertices[nextright])
						{
							nextright = j;
							break;
						}
					}

					m_vecPath_Optimization.push_back(m_vecRight_Vertices[right]);
					apex = m_vecRight_Vertices[right];
					right = nextright;
					left = nextright;
				}
				else
				{
					left = i;
				}
			}
		}

		//If new right vertex is different, process.
		if ((m_vecRight_Vertices[i] != m_vecRight_Vertices[right])
			&& i > right)
		{
			_float4 newSide = m_vecRight_Vertices[i] - apex;
			newSide.w = 1.f;

			// If new side does not widen funnel, update.
			if (VectorSign(m_vecRight_Vertices[right] - apex,
				newSide) > 0)
			{
				// If new side crosses other side, update apex.
				if (VectorSign(m_vecLeft_Vertices[left] - apex,
					newSide) > 0)
				{
					// Find next vertex.
					nextleft = left;

					for (int j = nextleft; j <= m_Path.size(); j++)
					{
						if (m_vecLeft_Vertices[j] != m_vecLeft_Vertices[nextleft])
						{
							nextleft = j;
							break;
						}
					}

					m_vecPath_Optimization.push_back(m_vecLeft_Vertices[left]);
					apex = m_vecLeft_Vertices[left];
					left = nextleft;
					right = nextleft;
				}
				else
				{
					right = i;
				}
			}
		}

		continue;



#pragma region Steering
		//If new left vertex is different, process.
		if ((m_vecLeft_Vertices[i] != m_vecLeft_Vertices[left])
			&& i > left)
		{
			_float4 newSide = m_vecLeft_Vertices[i] - apex;
			newSide.w = 1.f;

			// If new side does not widen funnel, update.
			if (VectorSign(newSide,
				m_vecLeft_Vertices[left] - apex) >= 0)
			{
				// If new side crosses other side, update apex.
				if (VectorSign(newSide,
					m_vecRight_Vertices[right] - apex) >= 0)
				{
					int next = right;
					int prev = right;

					// Find next vertex.
					for (int j = next; j <=m_Path_Optimization.size(); j++)
					{
						if (m_vecRight_Vertices[j] != m_vecRight_Vertices[next])
						{
							next = j;
							break;
						}
					}

					// Find previous vertex.
					for (int j = prev; j >= 0; j--)
					{
						if (m_vecRight_Vertices[j] != m_vecRight_Vertices[prev])
						{
							prev = j;
							break;
						}
					}

					// Calculate line angles.
					_float nextAngle = (_float)atan2(
						m_vecRight_Vertices[next].z - m_vecRight_Vertices[right].z,
						m_vecRight_Vertices[next].x - m_vecRight_Vertices[right].x);
					_float prevAngle =(_float)atan2(
						m_vecRight_Vertices[right].z - m_vecRight_Vertices[prev].z,
						m_vecRight_Vertices[right].x - m_vecRight_Vertices[prev].x);

					// Calculate minimum distance between line angles.
					float distance = nextAngle - prevAngle;

					if (abs(distance) >XM_PI)
					{
						distance -= distance > 0 ? XM_2PI
							: -XM_PI;
					}

					// Calculate left perpendicular to average angle.
					float angle = prevAngle + (distance / 2)
						+ XM_PIDIV2;
					_float4 normal = Float4_Normalize(_float4((_float)cos(angle),0.f,(_float)sin(angle),0.f));

					// Add new offset apex to list and update right side.
					auto NewPath = m_vecRight_Vertices[right] - (normal * 0.1f);
					NewPath.w = 1.f;
					m_vecPath_Optimization.push_back(NewPath);
					apex = m_vecRight_Vertices[right];
					right = next;
					left = next+1;
				}
				else
				{
					left = i;
				}
			}
		}

		if ((m_vecRight_Vertices[i] != m_vecRight_Vertices[left])
			&& i > left)
		{
			_float4 newSide = m_vecRight_Vertices[i] - apex;
			newSide.w = 1.f;

			// If new side does not widen funnel, update.
			if (VectorSign(m_vecRight_Vertices[right] - apex,
				newSide) >= 0)
			{
				// If new side crosses other side, update apex.
				if (VectorSign(m_vecLeft_Vertices[left] - apex,
					newSide) >= 0)
				{
					int next = left;
					int prev = left;

					// Find next vertex.
					for (int j = next; j <= m_Path_Optimization.size(); j++)
					{
						if (m_vecLeft_Vertices[j] != m_vecLeft_Vertices[next])
						{
							next = j;
							break;
						}
					}

					// Find previous vertex.
					for (int j = prev; j >= 0; j--)
					{
						if (m_vecLeft_Vertices[j] != m_vecLeft_Vertices[prev])
						{
							prev = j;
							break;
						}
					}

					// Calculate line angles.
					_float nextAngle = (_float)atan2(
						m_vecLeft_Vertices[next].z - m_vecLeft_Vertices[left].z,
						m_vecLeft_Vertices[next].x - m_vecLeft_Vertices[left].x);
					_float prevAngle = (_float)atan2(
						m_vecLeft_Vertices[left].z - m_vecLeft_Vertices[prev].z,
						m_vecLeft_Vertices[left].x - m_vecLeft_Vertices[prev].x);

					// Calculate minimum distance between line angles.
					float distance = nextAngle - prevAngle;

					if (abs(distance) > XM_PI)
					{
						distance -= distance > 0 ? XM_2PI
							: -XM_PI;
					}

					// Calculate left perpendicular to average angle.
					float angle = prevAngle + (distance / 2)
						+ XM_PIDIV2;
					_float4 normal = Float4_Normalize(_float4((_float)cos(angle), 0.f, (_float)sin(angle), 0.f));

					// Add new offset apex to list and update right side.
					auto NewPath = m_vecLeft_Vertices[left] + (normal * 0.1f);
					NewPath.w = 1.f;
					m_vecPath_Optimization.push_back(NewPath);
					apex = m_vecLeft_Vertices[left];
					left = next;
					right = next+1;
				}
				else
				{
					right = i;
				}
			}
		}
#pragma endregion
	}

	if(m_vecPath_Optimization.empty() == false && (m_vecPath_Optimization[m_vecPath_Optimization.size() - 1] != m_vecLeft_Vertices[m_vecLeft_Vertices.size() - 1]))
		m_vecPath_Optimization.push_back(m_vecLeft_Vertices[m_vecLeft_Vertices.size() - 1]);
#pragma endregion

return;

#pragma region 신버전
	m_vecPortals.resize(2*(m_Path.size() + 1));
	int nportals = 0;
	_float4 vEndPos = (*m_vecNavCell_Point)[m_Path_Optimization[m_Path_Optimization.size() - 1]];
	// 시작 포탈
	m_vecPortals[nportals * 2 + 0] =  m_vStartPos;
	m_vecPortals[nportals * 2 + 1] = m_vStartPos;
	nportals++;

	for (int i = 0; i < m_Path.size() - 1; i++)
	{
		_uint Cell_Index = m_Path_Optimization[i];
		auto Neighbors = (*m_Cells)[Cell_Index]->GetNeighborIndices();

		for (int j = 0; j < 3; j++)
		{
			if (Neighbors[j] == m_Path_Optimization[i + 1])
			{
				int k = j + 1 >= 3 ? 0 : j + 1;

				auto Point1 = (*m_Cells)[m_Path_Optimization[i]]->Get_Point_Float4(j);
				auto Point2 = (*m_Cells)[m_Path_Optimization[i]]->Get_Point_Float4(k);
				m_vecPortals[nportals * 2 + 0] = Point1;
				m_vecPortals[nportals * 2 + 1] = Point2;

				nportals++;

				break;
			}
		}
	}

	// 끝 포탈
	m_vecPortals[nportals * 2 + 0] = vEndPos;
	m_vecPortals[nportals * 2 + 1] = vEndPos;
	nportals++;

	StringPull(nportals,nportals);

#pragma endregion

}

void CPathFinder::Reset()
{
	m_vecLeft_Vertices.clear();
	m_vecRight_Vertices.clear();
	m_Path_Optimization.clear();
	m_vecPath_Optimization.clear();
}

_float4 CPathFinder::GetNextTarget()
{
	if (m_Path.empty())
		return _float4(0.f, 0.f, 0.f, 1.f);

	auto vNextTarget = m_Path.top();
	m_Path.pop();

	return (*m_vecNavCell_Point)[vNextTarget];
}

_float4 CPathFinder::GetNextTarget_Opt()
{
	if (m_vecPath_Optimization.empty())
		return _float4(0.f, 0.f, 0.f, 1.f);

	auto vNextTarget = *m_vecPath_Optimization.begin();
	m_vecPath_Optimization.erase(m_vecPath_Optimization.begin());
	
	return vNextTarget;
}

int CPathFinder::StringPull(int nportals, const int maxPts)
{
	// Find straight path.
	int npts = 0;
	// Init scan state
	float portalApex[2], portalLeft[2], portalRight[2];
	int apexIndex = 0, leftIndex = 0, rightIndex = 0;
	vcpy(portalApex, &m_vecPortals[0].x);
	vcpy(portalLeft, &m_vecPortals[0].x);
	vcpy(portalRight, &m_vecPortals[0].z);

	// Add start point.
	m_vecPath_Optimization.push_back(m_vecPortals[0]);
	npts++;

	for (int i = 1; i < nportals && npts < maxPts; ++i)
	{
		const float* left = &m_vecPortals[i].x;
		const float* right = &m_vecPortals[i].z;

		// Update right vertex.
		if (triarea2(portalApex, portalRight, right) <= 0.0f)
		{
			if (vequal(portalApex, portalRight) || triarea2(portalApex, portalLeft, right) > 0.0f)
			{
				// Tighten the funnel.
				vcpy(portalRight, right);
				rightIndex = i*2;
			}
			else
			{
				// Right over left, insert left to path and restart scan from portal left point.
				m_vecPath_Optimization.push_back(m_vecPortals[leftIndex]);
				npts++;
				// Make current left the new apex.
				vcpy(portalApex, portalLeft);
				apexIndex = leftIndex;
				// Reset portal
				vcpy(portalLeft, portalApex);
				vcpy(portalRight, portalApex);
				leftIndex = apexIndex;
				rightIndex = apexIndex;
				// Restart scan
				i = apexIndex;
				continue;
			}
		}

		// Update left vertex.
		if (triarea2(portalApex, portalLeft, left) >= 0.0f)
		{
			if (vequal(portalApex, portalLeft) || triarea2(portalApex, portalRight, left) < 0.0f)
			{
				// Tighten the funnel.
				vcpy(portalLeft, left);
				leftIndex = i*2;
			}
			else
			{
				// Left over right, insert right to path and restart scan from portal right point.
				m_vecPath_Optimization.push_back(m_vecPortals[rightIndex]);
				npts++;
				// Make current right the new apex.
				vcpy(portalApex, portalRight);
				apexIndex = rightIndex;
				// Reset portal
				vcpy(portalLeft, portalApex);
				vcpy(portalRight, portalApex);
				leftIndex = apexIndex;
				rightIndex = apexIndex;
				// Restart scan
				i = apexIndex;
				continue;
			}
		}
	}
	// Append last point to path.
	if (npts < maxPts)
	{
		m_vecPath_Optimization.push_back(m_vecPortals[nportals*2-1]);
		npts++;
	}

	return npts;
}

CPathFinder* CPathFinder::Create()
{
	CPathFinder* pInstance = new CPathFinder();

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPathFinder"));

		Safe_Release(pInstance);
	}
	else
	{
		pInstance->Init_PathFinder();
	}

	return pInstance;
}

void CPathFinder::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Delete_Array(m_cellDetails);
	Safe_Delete_Array(m_closedList);
}