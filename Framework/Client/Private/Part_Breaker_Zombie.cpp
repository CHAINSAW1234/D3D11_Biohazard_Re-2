#include "stdafx.h"
#include "Part_Breaker_Zombie.h"

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
	m_pBody_Model = { pDesc->pBodyModel };

	if (nullptr == m_pBody_Model)
		return E_FAIL;

	Safe_AddRef(m_pBody_Model);

	m_isBreakParts.resize(static_cast<_uint>(BREAK_PART::_END));
	m_RagDollMeshIndices.resize(static_cast<_uint>(BREAK_PART::_END));
	m_AnimMeshIndices.resize(static_cast<_uint>(BREAK_PART::_END));

	for (auto& isBreak : m_isBreakParts)
	{
		isBreak = false;
	}

	_uint					iMeshIndex = { 0 };
	vector<string>			BodyMeshTags = { m_pBody_Model->Get_MeshTags() };
	for (auto& strMeshTag : BodyMeshTags)
	{
#pragma region Right Arm
		//	Hide
		if (strMeshTag.find("0004") != string::npos ||			//	¿À¸¥ÆÈ ÆÈ²ÞÄ¡ »ó´Ü ¹Ùµð		( Body ))
			strMeshTag.find("0005") != string::npos)			//	¿À¸¥ÆÈ ÆÈ²ÞÄ¡ ÇÏ´Ü ¹Ùµð		( Body ))		
		{
			m_HideMeshIndices[static_cast<_uint>(BREAK_PART::_R_ARM)].emplace_back(iMeshIndex);
		}

		//	Anim
		else if (strMeshTag.find("0042") != string::npos ||		//	¿À¸¥ÆÈ ÆÈ²ÞÄ¡ »ó´Ü ¸ß±¸¶óÀÌ	( Deficit )
			strMeshTag.find("0068") != string::npos)			//	¿À¸¥ÆÈ ÆÈ²ÞÄ¡ »ó´Ü ¼Ó»À		( Damage ) 
		{
			m_AppearMeshIndices[static_cast<_uint>(BREAK_PART::_R_ARM)].emplace_back(iMeshIndex);
			m_AnimMeshIndices[static_cast<_uint>(BREAK_PART::_R_ARM)].emplace(iMeshIndex);
		}

		//	RagDoll
		else if (strMeshTag.find("0045") != string::npos ||		//	¿À¸¥ÆÈ ÆÈ²ÞÄ¡ ÇÏ´Ü ¸ß±¸¶óÀÌ	( Deficit )
			strMeshTag.find("0071") != string::npos)			//	¿À¸¥ÆÈ ÆÈ²ÞÄ¡ ÇÏ´Ü ¼Ó»À		( Damage ) 
		{
			m_AppearMeshIndices[static_cast<_uint>(BREAK_PART::_R_ARM)].emplace_back(iMeshIndex);
			m_RagDollMeshIndices[static_cast<_uint>(BREAK_PART::_R_ARM)].emplace(iMeshIndex);
		}
#pragma endregion

#pragma region Left Arm

		//	Hide
		else if (strMeshTag.find("0008") != string::npos ||		//	¿ÞÆÈ ÆÈ²ÞÄ¡ »ó´Ü ¹Ùµð		( Body )
			strMeshTag.find("0009") != string::npos)			//	¿ÞÆÈ ÆÈ²ÞÄ¡ ÇÏ´Ü ¹Ùµð		( Body )
		{
			m_HideMeshIndices[static_cast<_uint>(BREAK_PART::_L_ARM)].emplace_back(iMeshIndex);
		}

		//	Anim
		else if (
			strMeshTag.find("0048") != string::npos ||			//	¿ÞÆÈ ÆÈ²ÞÄ¡ »ó´Ü ¸ß±¸¶óÀÌ	( Deficit )
			strMeshTag.find("0074") != string::npos)			//	¿ÞÆÈ ÆÈ²ÞÄ¡ »ó´Ü ¼Ó»À		( Damage ) 
		{
			m_AppearMeshIndices[static_cast<_uint>(BREAK_PART::_L_ARM)].emplace_back(iMeshIndex);
			m_AnimMeshIndices[static_cast<_uint>(BREAK_PART::_L_ARM)].emplace(iMeshIndex);
		}

		//	RagDoll
		else if (
			strMeshTag.find("0051") != string::npos ||			//	¿ÞÆÈ ÆÈ²ÞÄ¡ ÇÏ´Ü ¸ß±¸¶óÀÌ	( Deficit )
			strMeshTag.find("0077") != string::npos)			//	¿ÞÆÈ ÆÈ²ÞÄ¡ ÇÏ´Ü ¼Ó»À		( Damage ) 
		{
			m_AppearMeshIndices[static_cast<_uint>(BREAK_PART::_L_ARM)].emplace_back(iMeshIndex);
			m_RagDollMeshIndices[static_cast<_uint>(BREAK_PART::_L_ARM)].emplace(iMeshIndex);
		}
#pragma endregion

#pragma region Right Leg

		//	Hide
		else if (strMeshTag.find("0012") != string::npos ||		//	¿À¸¥´Ù¸® ¹«¸­ »ó´Ü ¹Ùµð		( Body )
			strMeshTag.find("0013") != string::npos)			//	¿À¸¥´Ù¸® ¹«¸­ ÇÏ´Ü ¹Ùµð		( Body )
		{
			m_HideMeshIndices[static_cast<_uint>(BREAK_PART::_R_LEG)].emplace_back(iMeshIndex);
		}

		//	Anim
		else if (
			strMeshTag.find("0054") != string::npos ||			//	¿À¸¥´Ù¸® ¹«¸­ »ó´Ü ¸ß±¸¶óÀÌ	( Deficit )
			strMeshTag.find("0080") != string::npos)			//	¿À¸¥´Ù¸® ¹«¸­ »ó´Ü ¼Ó»À		( Damage ) 
		{
			m_AppearMeshIndices[static_cast<_uint>(BREAK_PART::_R_LEG)].emplace_back(iMeshIndex);
			m_AnimMeshIndices[static_cast<_uint>(BREAK_PART::_R_LEG)].emplace(iMeshIndex);
		}

		//	RagDoll
		else if (strMeshTag.find("0057") != string::npos ||		//	¿À¸¥´Ù¸® ¹«¸­ ÇÏ´Ü ¸ß±¸¶óÀÌ	( Deficit )
			strMeshTag.find("0083") != string::npos)			//	¿À¸¥´Ù¸® ¹«¸­ ÇÏ´Ü ¼Ó»À		( Damage ) 
		{
			m_AppearMeshIndices[static_cast<_uint>(BREAK_PART::_R_LEG)].emplace_back(iMeshIndex);
			m_RagDollMeshIndices[static_cast<_uint>(BREAK_PART::_R_LEG)].emplace(iMeshIndex);
		}
#pragma endregion

#pragma region Left Leg

		//	Hide
		else if (strMeshTag.find("0016") != string::npos ||		//	¿Þ´Ù¸® ¹«¸­ »ó´Ü ¹Ùµð		( Body )
			strMeshTag.find("0017") != string::npos)			//	¿Þ´Ù¸® ¹«¸­ ÇÏ´Ü ¹Ùµð		( Body )
		{
			m_HideMeshIndices[static_cast<_uint>(BREAK_PART::_L_LEG)].emplace_back(iMeshIndex);
		}

		//	Anim
		else if (strMeshTag.find("0061") != string::npos ||		//	¿Þ´Ù¸® ¹«¸­ »ó´Ü ¸ß±¸¶óÀÌ		( Deficit )
			strMeshTag.find("0087") != string::npos)			//	¿Þ¸¥´Ù¸® ¹«¸­ »ó´Ü ¼Ó»À		( Damage ) 
		{
			m_AppearMeshIndices[static_cast<_uint>(BREAK_PART::_L_LEG)].emplace_back(iMeshIndex);
			m_AnimMeshIndices[static_cast<_uint>(BREAK_PART::_L_LEG)].emplace(iMeshIndex);
		}

		//	RagDoll
		else if (strMeshTag.find("0064") != string::npos ||		//	¿Þ´Ù¸® ¹«¸­ ÇÏ´Ü ¸ß±¸¶óÀÌ		( Deficit )
			strMeshTag.find("0090") != string::npos)			//	¿Þ´Ù¸® ¹«¸­ ÇÏ´Ü ¼Ó»À			( Damage ) 
		{
			m_AppearMeshIndices[static_cast<_uint>(BREAK_PART::_L_LEG)].emplace_back(iMeshIndex);
			m_RagDollMeshIndices[static_cast<_uint>(BREAK_PART::_L_LEG)].emplace(iMeshIndex);
		}
#pragma endregion

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
	for (_uint i = 0; i < static_cast<_uint>(BREAK_PART::_END); ++i)
	{
		if (true == m_isBreakParts[i])
		{
			unordered_set<_uint>::iterator			iter = { m_RagDollMeshIndices[i].find(iMeshIndex) };
			if (iter != m_RagDollMeshIndices[i].end())
			{
				isRagDollMesh = true;
				break;
			}
		}
	}

	return isRagDollMesh;
}

void CPart_Breaker_Zombie::Break(BREAK_PART ePart)
{
	if (ePart >= BREAK_PART::_END)
		return;

	if (true == Is_BreaKPart(ePart))
		return;

	m_isBreakParts[static_cast<_uint>(ePart)] = true;
	

	for (auto& iMeshIndex : m_AppearMeshIndices[static_cast<_uint>(ePart)])
	{
		m_pBody_Model->Hide_Mesh(iMeshIndex, false);
	}

	for (auto& iMeshIndex : m_HideMeshIndices[static_cast<_uint>(ePart)])
	{
		m_pBody_Model->Hide_Mesh(iMeshIndex, true);
	}
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

	Safe_Release(m_pBody_Model);
}


