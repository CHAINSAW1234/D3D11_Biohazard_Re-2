#pragma once

#include "Base.h"

BEGIN(Engine)

class CBone_Layer final : public CBase
{
public:
	typedef struct tagBoneDesc
	{
	}BONE_DESC;

private:
	CBone_Layer();
	virtual ~CBone_Layer() = default;

public:
	HRESULT								Initialize(const unordered_set<_uint>& BoneIndices);

public:
	void								Add_BoneIndex(_uint iBoneIndex);
	_bool								Is_Included(_uint iBoneIndex);
	unordered_set<_uint>&				Get_IncludedBoneIndices() { return m_IncludedBoneIndices; }
private:
	unordered_set<_uint>				m_IncludedBoneIndices;

public:
	static CBone_Layer* Create(const unordered_set<_uint>& BoneIndices);
	virtual void Free() override;
};

END