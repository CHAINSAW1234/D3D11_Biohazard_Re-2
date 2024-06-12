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
	HRESULT					Initialize(const set<_uint>& BoneIndices);

public:
	_bool					Is_Included(_uint iBoneIndex);
	set<_uint>& Get_IncludedBoneIndices() { return m_IncludedBoneIndices; }
private:
	set<_uint>				m_IncludedBoneIndices;

public:
	static CBone_Layer* Create(const set<_uint>& BoneIndices);
	virtual void Free() override;
};

END