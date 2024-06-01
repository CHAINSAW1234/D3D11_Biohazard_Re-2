#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CBone final : public CBase
{
public:
	typedef struct tagBoneDesc
	{
		string				strName = { "" };
		_int				iParentIndex = { 0 };
		_float4x4			TransformationMatrix;
	}BONE_DESC;

private:
	CBone();
	virtual ~CBone() = default;

public:
	void					Set_TransformationMatrix(_fmatrix TransformationMatrix) 
	{
		XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	}
	void					Set_TransformationMatrix(_float4x4 TransformationMatrix) 
	{
		XMStoreFloat4x4(&m_TransformationMatrix, XMLoadFloat4x4(&TransformationMatrix));
	}

	_float4x4				Get_TrasformationFloat4x4()
	{
		return m_TransformationMatrix;
	}

	_matrix					Get_TrasformationMatrix() 
	{
		return XMLoadFloat4x4(&m_TransformationMatrix);
	}

public:
	const _float4x4*		Get_CombinedTransformationMatrix() const 
	{
		return &m_CombinedTransformationMatrix;
	}

	const _float4x4*		Get_PrevCombinedTransformationMatrix() const {
		return &m_PrevCombinedTransformationMatrix;
	}

	void Set_Pre_CombinedMatrix(_fmatrix PreCombiendMatrix) {
		XMStoreFloat4x4(&m_PrevCombinedTransformationMatrix, PreCombiendMatrix);
	}

	const _float4			Get_Translation() 
	{
		_vector vScale, vRotation, vTrans;
		XMMatrixDecompose(&vScale, &vRotation, &vTrans, XMLoadFloat4x4(&m_TransformationMatrix));

		_float4 vTranslation;
		XMStoreFloat4(&vTranslation, vTrans);
		return vTranslation;
	}

public:
	HRESULT					Initialize(const aiNode* pAiNode, _int iParentIndex);
	HRESULT					Initialize(const BONE_DESC& BoneDesc);
	void					Invalidate_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix ParentsTransformatrixMatrix);
	//	이동성분만 반환
	_float3					Invalidate_CombinedTransformationMatrix_RootMotion_Translation(const vector<CBone*>& Bones, _fmatrix ParentsTransformatrixMatrix);
	//	합행렬 전부 반환
	_float4x4				Invalidate_CombinedTransformationMatrix_RootMotion_WorldMatrix(const vector<CBone*>& Bones, _fmatrix ParentsTransformatrixMatrix);

	void					Invalidate_CombinedTransformationMatrix_RootMotion(const vector<CBone*>& Bones, _float4x4 TransformationMatrix, _bool isActiveXZ, _bool isActiveY, _bool isActiveRotation, _float4* pTranslation, _float4* pQuaternion);

private:
	_vector					Decompose_Translation(_bool isActiveXZ, _bool isActiveY, _fvector vTranslation, _float4* pTranslation);
	_vector					Decompose_Quaternion(_fvector vQuaternion, _float4* pQuaternion);

public:
	_bool					Compare_Name(const _char* pBoneName) 
	{
		return !strcmp(m_szName, pBoneName);
	}

	_char*					Get_Name() { return m_szName; }

	_int					Get_ParentIndex() { return m_iParentBoneIndex; }

	void					Set_Combined_Matrix(_fmatrix CombinedMatrix);
	void					Set_Parent_CombinedMatrix_Ptr(_float4x4* pParentMatrix);
	void					Set_Surbodinate(_bool isSurbodinate) { m_isSurbordinate = isSurbodinate; }
	void					Set_RootBone(_bool isRootBone);

	_bool					Is_RootBone() { return m_isRootBone; }

private:
	_char					m_szName[MAX_PATH] = { "" };

	_float4x4				m_TransformationMatrix;

	_float4x4				m_CombinedTransformationMatrix;
	_float4x4				m_PrevCombinedTransformationMatrix;

	_int					m_iParentBoneIndex = { -1 };
	_bool					m_isRootBone = { false };

	_bool					m_isSurbordinate = { false };
	_float4x4*				m_pParentCombinedMatrix = { nullptr };

public:
	static CBone* Create(const aiNode* pAINode, _int iParentIndex);
	static CBone* Create(const BONE_DESC& BoneDesc);
	CBone* Clone();
	virtual void Free() override;
};

END