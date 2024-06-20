#include "Bone_Layer.h"

CBone_Layer::CBone_Layer()
{
}

HRESULT CBone_Layer::Initialize(const unordered_set<_uint>& BoneIndices)
{
	if (true == BoneIndices.empty())
		return E_FAIL;

	m_IncludedBoneIndices = BoneIndices;

	return S_OK;
}

void CBone_Layer::Add_BoneIndex(_uint iBoneIndex)
{
	m_IncludedBoneIndices.emplace(iBoneIndex);
}

_bool CBone_Layer::Is_Included(_uint iBoneIndex)
{
	_bool		isIncluded = { false };

	
	unordered_set<_uint>::iterator		iter = { m_IncludedBoneIndices.find(iBoneIndex) };
	//	set<_uint>::iterator		iter = { find(m_IncludedBoneIndices.begin(), m_IncludedBoneIndices.end(), iBoneIndex) };
	if (iter != m_IncludedBoneIndices.end())
		isIncluded = true;

	return isIncluded;
}

CBone_Layer* CBone_Layer::Create(const unordered_set<_uint>& BoneIndices)
{
	CBone_Layer* pInstance = { new CBone_Layer() };

	if (FAILED(pInstance->Initialize(BoneIndices)))
	{
		MSG_BOX(TEXT("Failed To Created : CBone_Layer"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBone_Layer::Free()
{
	__super::Free();
}
