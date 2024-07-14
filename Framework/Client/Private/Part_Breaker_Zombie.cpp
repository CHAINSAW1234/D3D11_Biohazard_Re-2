#include "stdafx.h"
#include "Part_Breaker_Zombie.h"
#include "Part_Mesh_Info_Zombie.h"
#include "Zombie.h"

CPart_Breaker_Zombie::CPart_Breaker_Zombie()
{
}

CPart_Breaker_Zombie::CPart_Breaker_Zombie(const CPart_Breaker_Zombie& rhs)
{
}

HRESULT CPart_Breaker_Zombie::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	PART_BREAKER_DESC*			pDesc = { static_cast<PART_BREAKER_DESC*>(pArg) };
	if (-1 == pDesc->iBodyType ||
		pDesc->iBodyType >= static_cast<_int>(ZOMBIE_BODY_TYPE::_END))
		return E_FAIL;

	m_iBodyType = { static_cast<_uint>(pDesc->iBodyType) };
	m_pBody_Model = { pDesc->pBodyModel };
	m_pFace_Model = { pDesc->pFaceModel };
	m_pShirts_Model = { pDesc->pShirts_Model };
	m_pPants_Model = { pDesc->pPants_Model };

	if (nullptr == m_pBody_Model || nullptr == m_pFace_Model)
		return E_FAIL;

	Safe_AddRef(m_pBody_Model);
	Safe_AddRef(m_pFace_Model);
	Safe_AddRef(m_pShirts_Model);
	Safe_AddRef(m_pPants_Model);

	m_HPs.resize(static_cast<_uint>(BREAK_PART::_END));
	for (auto& iHP : m_HPs)
		iHP = 20;

	m_isBreakParts.resize(static_cast<_uint>(BREAK_PART::_END));

	for (auto& isBreak : m_isBreakParts)
	{
		isBreak = false;
	}

	//	파트별 타입별 인덱스들
	vector<vector<_int>>	MeshPartsTypesIndices;
	MeshPartsTypesIndices.resize(static_cast<_uint>(MESH_PART::_END));
	for (auto& MeshTypesIndices : MeshPartsTypesIndices)
	{
		MeshTypesIndices.resize(static_cast<_uint>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_END));
		for (auto& iIndex : MeshTypesIndices)
			iIndex = -1;
	}

	vector<string>			BodyMeshTags = { m_pBody_Model->Get_MeshTags() };

	if (ZOMBIE_BODY_TYPE::_MALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBodyType))
	{
		if (FAILED(Compute_MeshParts_Types_Indices_Male(MeshPartsTypesIndices, BodyMeshTags)))
			return E_FAIL;
	}

	else if (ZOMBIE_BODY_TYPE::_FEMALE == static_cast<ZOMBIE_BODY_TYPE>(m_iBodyType) ||
		ZOMBIE_BODY_TYPE::_MALE_BIG == static_cast<ZOMBIE_BODY_TYPE>(m_iBodyType))
	{
		if (FAILED(Compute_MeshParts_Types_Indices_Female_Male_Big(MeshPartsTypesIndices, BodyMeshTags)))
			return E_FAIL;
	}

	else
		return E_FAIL;

	m_PartMeshInfos.resize(static_cast<_uint>(MESH_PART::_END));
	for (_int i = static_cast<_int>(MESH_PART::_END) - 2; 0 <= i; --i)
	{
		MESH_PART			ePart = { static_cast<MESH_PART>(i) };
		CPart_Mesh_Info_Zombie::PART_MESH_INFO_DESC		PartMeshInfoDsec;		

		PartMeshInfoDsec.pMeshTypeIndices = &MeshPartsTypesIndices[i];

		if (ePart != MESH_PART::_R_LOWER_RADIUS &&
			ePart != MESH_PART::_L_LOWER_RADIUS &&
			ePart != MESH_PART::_R_LOWER_TABIA &&
			ePart != MESH_PART::_L_LOWER_TABIA)
		{
			PartMeshInfoDsec.pChild_Part_Mesh_Info = m_PartMeshInfos[static_cast<_uint>(i) + 1];
		}

		else
		{
			PartMeshInfoDsec.pChild_Part_Mesh_Info = nullptr;
		}

		CPart_Mesh_Info_Zombie* pPartMeshInfo = { CPart_Mesh_Info_Zombie::Create(&PartMeshInfoDsec) };
		if (nullptr == pPartMeshInfo)
			return E_FAIL;

		m_PartMeshInfos[static_cast<_uint>(i)] = pPartMeshInfo;
	}

	return S_OK;
}

HRESULT CPart_Breaker_Zombie::Compute_MeshParts_Types_Indices_Male(vector<vector<_int>>& MeshPartsTypesIndices, const vector<string>& MeshTags)
{
	_uint					iMeshIndex = { 0 };
	for (auto& strMeshTag : MeshTags)
	{
		_int				iMeshPart = { -1 };
		_int				iMeshType = { -1 };

#pragma region Right Arm

#pragma region Right Upper Humerous

		if (strMeshTag.find("0003") != string::npos)			//	Upper_Humerous	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0032") != string::npos)		//	Upper_Humerous	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0040") != string::npos)		//	Upper_Humerous	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0041") != string::npos)		//	Upper_Humerous	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0066") != string::npos)		//	Upper_Humerous	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0067") != string::npos)		//	Upper_Humerous	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
#pragma endregion

#pragma region Right Lower Humerous

		if (strMeshTag.find("0004") != string::npos)			//	Lower_Humerous	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0033") != string::npos)		//	Lower_Humerous	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0042") != string::npos)		//	Lower_Humerous	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0043") != string::npos)		//	Lower_Humerous	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0068") != string::npos)		//	Lower_Humerous	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0069") != string::npos)		//	Lower_Humerous	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

#pragma endregion

#pragma region Right Upper Radius

		if (strMeshTag.find("0005") != string::npos)			//	Upper_Radius	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0034") != string::npos)		//	Upper_Radius	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0044") != string::npos)		//	Upper_Radius	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0045") != string::npos)		//	Upper_Radius	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0070") != string::npos)		//	Upper_Radius	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0071") != string::npos)		//	Upper_Radius	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
#pragma endregion

#pragma region Right Lower Radius

		if (strMeshTag.find("0006") != string::npos)			//	Lower_Radius	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0035") != string::npos)		//	Lower_Radius	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

#pragma endregion

#pragma endregion //	Right Arm


#pragma region Left Arm

#pragma region Left Upper Humerous

		if (strMeshTag.find("0007") != string::npos)			//	Upper_Humerous	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0036") != string::npos)		//	Upper_Humerous	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0046") != string::npos)		//	Upper_Humerous	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0047") != string::npos)		//	Upper_Humerous	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0072") != string::npos)		//	Upper_Humerous	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0073") != string::npos)		//	Upper_Humerous	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
#pragma endregion

#pragma region Left Lower Humerous

		if (strMeshTag.find("0008") != string::npos)			//	Lower_Humerous	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0037") != string::npos)		//	Lower_Humerous	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0048") != string::npos)		//	Lower_Humerous	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0049") != string::npos)		//	Lower_Humerous	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0074") != string::npos)		//	Lower_Humerous	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0075") != string::npos)		//	Lower_Humerous	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

#pragma endregion

#pragma region Left Upper Radius

		if (strMeshTag.find("0009") != string::npos)			//	Upper_Radius	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0038") != string::npos)		//	Upper_Radius	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0050") != string::npos)		//	Upper_Radius	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0051") != string::npos)		//	Upper_Radius	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0076") != string::npos)		//	Upper_Radius	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0077") != string::npos)		//	Upper_Radius	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
#pragma endregion

#pragma region Left Lower Radius

		if (strMeshTag.find("0010") != string::npos)			//	Lower_Radius	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0039") != string::npos)		//	Lower_Radius	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

#pragma endregion

#pragma endregion //	Left Arm


#pragma region Right Leg

#pragma region Right Upper Femur

		if (strMeshTag.find("0011") != string::npos)			//	Upper_Femur	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0024") != string::npos)		//	Upper_Femur	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0052") != string::npos)		//	Upper_Femur	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0053") != string::npos)		//	Upper_Femur	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0078") != string::npos)		//	Upper_Femur	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0079") != string::npos)		//	Upper_Femur	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
#pragma endregion

#pragma region Right Lower Femur

		if (strMeshTag.find("0012") != string::npos)			//	Lower_Femur	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0025") != string::npos)		//	Lower_Femur	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0054") != string::npos)		//	Lower_Femur	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0055") != string::npos)		//	Lower_Femur	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0080") != string::npos)		//	Lower_Femur	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0081") != string::npos)		//	Lower_Femur	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

#pragma endregion

#pragma region Right Upper Tabia

		if (strMeshTag.find("0013") != string::npos)			//	Upper_Tabia	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0026") != string::npos)		//	Upper_Tabia	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0056") != string::npos)		//	Upper_Tabia	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0057") != string::npos)		//	Upper_Tabia	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0082") != string::npos)		//	Upper_Tabia	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0083") != string::npos)		//	Upper_Tabia	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
#pragma endregion

#pragma region Right Lower Tabia

		if (strMeshTag.find("0014") != string::npos)			//	Lower_Tabia	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0027") != string::npos)		//	Lower_Tabia	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0058") != string::npos)		//	Lower_Tabia	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0084") != string::npos)		//	Lower_Tabia	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

#pragma endregion

#pragma endregion //	Right Leg


#pragma region Left Leg

#pragma region Left Upper Femur

		if (strMeshTag.find("0015") != string::npos)			//	Upper_Femur	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0028") != string::npos)		//	Upper_Femur	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0059") != string::npos)		//	Upper_Femur	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0060") != string::npos)		//	Upper_Femur	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0085") != string::npos)		//	Upper_Femur	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0086") != string::npos)		//	Upper_Femur	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
#pragma endregion

#pragma region Left Lower Femur

		if (strMeshTag.find("0016") != string::npos)			//	Lower_Femur	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0029") != string::npos)		//	Lower_Femur	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0061") != string::npos)		//	Lower_Femur	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0062") != string::npos)		//	Lower_Femur	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0087") != string::npos)		//	Lower_Femur	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0088") != string::npos)		//	Lower_Femur	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

#pragma endregion

#pragma region Left Upper Tabia

		if (strMeshTag.find("0017") != string::npos)			//	Upper_Tabia	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0030") != string::npos)		//	Upper_Tabia	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0063") != string::npos)		//	Upper_Tabia	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0064") != string::npos)		//	Upper_Tabia	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0089") != string::npos)		//	Upper_Tabia	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0090") != string::npos)		//	Upper_Tabia	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
#pragma endregion

#pragma region Left Lower Tabia

		if (strMeshTag.find("0018") != string::npos)			//	Lower_Tabia	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0031") != string::npos)		//	Lower_Tabia	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0065") != string::npos)		//	Lower_Tabia	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0091") != string::npos)		//	Lower_Tabia	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

#pragma endregion

#pragma endregion //	Left Leg

		++iMeshIndex;
	}

	return S_OK;
}

HRESULT CPart_Breaker_Zombie::Compute_MeshParts_Types_Indices_Female_Male_Big(vector<vector<_int>>& MeshPartsTypesIndices, const vector<string>& MeshTags)
{

	_uint					iMeshIndex = { 0 };
	for (auto& strMeshTag : MeshTags)
	{
		_int				iMeshPart = { -1 };
		_int				iMeshType = { -1 };

#pragma region Right Arm

#pragma region Right Upper Humerous

		if (strMeshTag.find("0022") != string::npos)			//	Upper_Humerous	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0011") != string::npos)		//	Upper_Humerous	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0040") != string::npos)		//	Upper_Humerous	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0041") != string::npos)		//	Upper_Humerous	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0066") != string::npos)		//	Upper_Humerous	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0067") != string::npos)		//	Upper_Humerous	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
#pragma endregion

#pragma region Right Lower Humerous

		if (strMeshTag.find("0023") != string::npos)			//	Lower_Humerous	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0012") != string::npos)		//	Lower_Humerous	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0042") != string::npos)		//	Lower_Humerous	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0043") != string::npos)		//	Lower_Humerous	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0068") != string::npos)		//	Lower_Humerous	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0069") != string::npos)		//	Lower_Humerous	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

#pragma endregion

#pragma region Right Upper Radius

		if (strMeshTag.find("0024") != string::npos)			//	Upper_Radius	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0013") != string::npos)		//	Upper_Radius	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0044") != string::npos)		//	Upper_Radius	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0045") != string::npos)		//	Upper_Radius	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0070") != string::npos)		//	Upper_Radius	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0071") != string::npos)		//	Upper_Radius	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
#pragma endregion

#pragma region Right Lower Radius

		if (strMeshTag.find("0025") != string::npos)			//	Lower_Radius	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0014") != string::npos)		//	Lower_Radius	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

#pragma endregion

#pragma endregion //	Right Arm


#pragma region Left Arm

#pragma region Left Upper Humerous

		if (strMeshTag.find("0026") != string::npos)			//	Upper_Humerous	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0015") != string::npos)		//	Upper_Humerous	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0046") != string::npos)		//	Upper_Humerous	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0047") != string::npos)		//	Upper_Humerous	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0072") != string::npos)		//	Upper_Humerous	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0073") != string::npos)		//	Upper_Humerous	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
#pragma endregion

#pragma region Left Lower Humerous

		if (strMeshTag.find("0027") != string::npos)			//	Lower_Humerous	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0016") != string::npos)		//	Lower_Humerous	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0048") != string::npos)		//	Lower_Humerous	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0049") != string::npos)		//	Lower_Humerous	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0074") != string::npos)		//	Lower_Humerous	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0075") != string::npos)		//	Lower_Humerous	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_HUMEROUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

#pragma endregion

#pragma region Left Upper Radius

		if (strMeshTag.find("0028") != string::npos)			//	Upper_Radius	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0017") != string::npos)		//	Upper_Radius	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0050") != string::npos)		//	Upper_Radius	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0051") != string::npos)		//	Upper_Radius	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0076") != string::npos)		//	Upper_Radius	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0077") != string::npos)		//	Upper_Radius	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
#pragma endregion

#pragma region Left Lower Radius

		if (strMeshTag.find("0029") != string::npos)			//	Lower_Radius	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0018") != string::npos)		//	Lower_Radius	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_RADIUS);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

#pragma endregion

#pragma endregion //	Left Arm


#pragma region Right Leg

#pragma region Right Upper Femur

		if (strMeshTag.find("0030") != string::npos)			//	Upper_Femur	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0003") != string::npos)		//	Upper_Femur	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0052") != string::npos)		//	Upper_Femur	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0053") != string::npos)		//	Upper_Femur	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0078") != string::npos)		//	Upper_Femur	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0079") != string::npos)		//	Upper_Femur	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
#pragma endregion

#pragma region Right Lower Femur

		if (strMeshTag.find("0031") != string::npos)			//	Lower_Femur	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0004") != string::npos)		//	Lower_Femur	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0054") != string::npos)		//	Lower_Femur	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0055") != string::npos)		//	Lower_Femur	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0080") != string::npos)		//	Lower_Femur	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0081") != string::npos)		//	Lower_Femur	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

#pragma endregion

#pragma region Right Upper Tabia

		if (strMeshTag.find("0032") != string::npos)			//	Upper_Tabia	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0005") != string::npos)		//	Upper_Tabia	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0056") != string::npos)		//	Upper_Tabia	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0057") != string::npos)		//	Upper_Tabia	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0082") != string::npos)		//	Upper_Tabia	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0083") != string::npos)		//	Upper_Tabia	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
#pragma endregion

#pragma region Right Lower Tabia

		if (strMeshTag.find("0033") != string::npos)			//	Lower_Tabia	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0006") != string::npos)		//	Lower_Tabia	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0058") != string::npos)		//	Lower_Tabia	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0084") != string::npos)		//	Lower_Tabia	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_R_LOWER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

#pragma endregion

#pragma endregion //	Right Leg


#pragma region Left Leg

#pragma region Left Upper Femur

		if (strMeshTag.find("0034") != string::npos)			//	Upper_Femur	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0007") != string::npos)		//	Upper_Femur	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0059") != string::npos)		//	Upper_Femur	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0060") != string::npos)		//	Upper_Femur	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0085") != string::npos)		//	Upper_Femur	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0086") != string::npos)		//	Upper_Femur	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
#pragma endregion

#pragma region Left Lower Femur

		if (strMeshTag.find("0035") != string::npos)			//	Lower_Femur	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0008") != string::npos)		//	Lower_Femur	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0061") != string::npos)		//	Lower_Femur	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0062") != string::npos)		//	Lower_Femur	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0087") != string::npos)		//	Lower_Femur	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0088") != string::npos)		//	Lower_Femur	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_FEMUR);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

#pragma endregion

#pragma region Left Upper Tabia

		if (strMeshTag.find("0036") != string::npos)			//	Upper_Tabia	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0009") != string::npos)		//	Upper_Tabia	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0063") != string::npos)		//	Upper_Tabia	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0064") != string::npos)		//	Upper_Tabia	(Deficit From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0089") != string::npos)		//	Upper_Tabia	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0090") != string::npos)		//	Upper_Tabia	(Damaged From Child)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_UPPER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_CHILD_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
#pragma endregion

#pragma region Left Lower Tabia

		if (strMeshTag.find("0037") != string::npos)			//	Lower_Tabia	(Body)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_BODY);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}
		else if (strMeshTag.find("0010") != string::npos)		//	Lower_Tabia	(Inside)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_INSIDE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0065") != string::npos)		//	Lower_Tabia	(Deficit From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DEFICIT);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

		else if (strMeshTag.find("0091") != string::npos)		//	Lower_Tabia	(Damaged From Parent)
		{
			iMeshPart = static_cast<_int>(MESH_PART::_L_LOWER_TABIA);
			iMeshType = static_cast<_int>(CPart_Mesh_Info_Zombie::PART_MESH_TYPE::_FROM_PARENT_DAMAGE);

			MeshPartsTypesIndices[iMeshPart][iMeshType] = static_cast<_int>(iMeshIndex);
		}

#pragma endregion

#pragma endregion //	Left Leg

		++iMeshIndex;
	}

	return S_OK;
}
_bool CPart_Breaker_Zombie::Is_BreaKPart(BREAK_PART ePart)
{
	if (ePart >= BREAK_PART::_END)
		return false;

	return m_isBreakParts[static_cast<_uint>(ePart)];
}

_bool CPart_Breaker_Zombie::Is_RagDoll_Mesh(_uint iMeshIndex)
{
	_bool			isRagDollMesh = { false };

	unordered_set<_uint>::iterator			iter = { m_RagDollMeshIndices.find(iMeshIndex) };
	if (iter != m_RagDollMeshIndices.end())
	{
		isRagDollMesh = true;
	}

	return isRagDollMesh;
}

_bool CPart_Breaker_Zombie::Attack(BREAK_PART ePart)
{
	if (ePart >= BREAK_PART::_END)
		return false;

	if (1 == m_HPs[static_cast<_uint>(ePart)])
	{
		m_HPs[static_cast<_uint>(ePart)] -= 1;
		return Break(ePart);
	}

	else if (1 < m_HPs[static_cast<_uint>(ePart)])
	{
		m_HPs[static_cast<_uint>(ePart)] -= 1;
	}

	return false;
}

_bool CPart_Breaker_Zombie::Break(BREAK_PART ePart)
{
	if (ePart >= BREAK_PART::_END)
		return false;

	if (true == Is_BreaKPart(ePart))
		return false;

	if (static_cast<_int>(ePart) >= static_cast<_int>(BREAK_PART::_R_UPPER_HUMEROUS) &&
		static_cast<_int>(ePart) <= static_cast<_int>(BREAK_PART::_R_UPPER_RADIUS))
	{
		for (_uint i = static_cast<_uint>(BREAK_PART::_R_UPPER_HUMEROUS); i < static_cast<_uint>(BREAK_PART::_R_UPPER_RADIUS); ++i)
		{
			if (true == Is_BreaKPart(static_cast<BREAK_PART>(i)))
				return false;
		}
	}

	else if (static_cast<_int>(ePart) >= static_cast<_int>(BREAK_PART::_L_UPPER_HUMEROUS) &&
		static_cast<_int>(ePart) <= static_cast<_int>(BREAK_PART::_L_UPPER_RADIUS))
	{
		for (_uint i = static_cast<_uint>(BREAK_PART::_L_UPPER_HUMEROUS); i < static_cast<_uint>(BREAK_PART::_L_UPPER_RADIUS); ++i)
		{
			if (true == Is_BreaKPart(static_cast<BREAK_PART>(i)))
				return false;
		}
	}

	else if (static_cast<_int>(ePart) >= static_cast<_int>(BREAK_PART::_R_UPPER_FEMUR) &&
		static_cast<_int>(ePart) <= static_cast<_int>(BREAK_PART::_R_LOWER_TABIA))
	{
		for (_uint i = static_cast<_uint>(BREAK_PART::_R_UPPER_FEMUR); i < static_cast<_uint>(BREAK_PART::_R_LOWER_TABIA); ++i)
		{
			if (true == Is_BreaKPart(static_cast<BREAK_PART>(i)))
				return false;
		}
	}

	else if (static_cast<_int>(ePart) >= static_cast<_int>(BREAK_PART::_L_UPPER_FEMUR) &&
		static_cast<_int>(ePart) <= static_cast<_int>(BREAK_PART::_L_LOWER_TABIA))
	{
		for (_uint i = static_cast<_uint>(BREAK_PART::_L_UPPER_FEMUR); i < static_cast<_uint>(BREAK_PART::_L_LOWER_TABIA); ++i)
		{
			if (true == Is_BreaKPart(static_cast<BREAK_PART>(i)))
				return false;
		}
	}

	m_isBreakParts[static_cast<_uint>(ePart)] = true;

	if (BREAK_PART::_HEAD == ePart)
	{
		vector<string>				FaceMeshTags = { m_pFace_Model->Get_MeshTags() };
		for (auto& strMeshTag : FaceMeshTags)
		{
			m_pFace_Model->Hide_Mesh(strMeshTag, true);
		}

		vector<string>				BodyMeshTags = { m_pBody_Model->Get_MeshTags() };
		vector<_int>				BrokenHeadMeshIndices; 

		_int						iIndex = { 0 };
		for (auto& strMeshTag : BodyMeshTags)
		{
			if (strMeshTag.find("Broken") != string::npos)
			{
				BrokenHeadMeshIndices.push_back(iIndex);
			}
			++iIndex;
		}

		_int			iRandomHeadIndex = { CGameInstance::Get_Instance()->GetRandom_Int(0, static_cast<_int>(BrokenHeadMeshIndices.size() - 1)) };
		for (auto& iIndex : BrokenHeadMeshIndices)
		{
			if(BrokenHeadMeshIndices[iRandomHeadIndex] == iIndex)
				m_pBody_Model->Hide_Mesh(iIndex, false);
			else
				m_pBody_Model->Hide_Mesh(iIndex, true);
		}
	}

	else
	{
		m_PartMeshInfos[static_cast<_uint>(ePart)]->Break(m_pBody_Model);
		list<_int>			RagDollMeshIndices = { m_PartMeshInfos[static_cast<_uint>(ePart)]->Get_Child_MeshIndices() };
		for (auto& iRagDollMeshIndex : RagDollMeshIndices)
		{
			unordered_set<_uint>::iterator			iterRagDoll = { m_RagDollMeshIndices.find(iRagDollMeshIndex) };
			if (m_RagDollMeshIndices.end() == iterRagDoll)
			{
				m_RagDollMeshIndices.emplace(static_cast<_uint>(iRagDollMeshIndex));
			}

			unordered_set<_uint>::iterator			iterAnim = { m_AnimMeshIndices.find(iRagDollMeshIndex) };
			if (m_AnimMeshIndices.end() != iterAnim)
			{
				m_AnimMeshIndices.erase(iterAnim);
			}
		}
	}

	return true;
}

CPart_Breaker_Zombie* CPart_Breaker_Zombie::Create(void* pArg)
{
	CPart_Breaker_Zombie*			pInstance = { new CPart_Breaker_Zombie() };
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPart_Breaker_Zombie"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPart_Breaker_Zombie::Free()
{
	__super::Free();

	for (auto& pMeshInfo : m_PartMeshInfos)
	{
		Safe_Release(pMeshInfo);
		pMeshInfo = nullptr;
	}
	
	m_PartMeshInfos.clear();

	Safe_Release(m_pBody_Model);
	Safe_Release(m_pFace_Model);
	Safe_Release(m_pPants_Model);
	Safe_Release(m_pShirts_Model);
}


