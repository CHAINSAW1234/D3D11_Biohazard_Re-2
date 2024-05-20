#include "Bone.h"
#include "Transform.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const aiNode* pAiNode, _int iParentIndex)
{
	m_iParentBoneIndex = iParentIndex;

	strcpy_s(m_szName, pAiNode->mName.data);

	memcpy_s(&m_TransformationMatrix, sizeof(_float4x4), &pAiNode->mTransformation, sizeof(_float4x4));
	//	Assimp�� ��� ����� ��ġ�Ǿ������Ƿ� �ٽ� ��ġ �Ͽ� ����������Ѵ�.
	//	Assimp(Assimp�� ���� �ҽ� 3D �� ����Ʈ ���̺귯���Դϴ�)������ ��� ����� column-major ���·� �ٷ�ϴ�
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CBone::Initialize(const BONE_DESC& BoneDesc)
{
	m_iParentBoneIndex = BoneDesc.iParentIndex;

	strcpy_s(m_szName, BoneDesc.strName.c_str());

	memcpy_s(&m_TransformationMatrix, sizeof(_float4x4), &BoneDesc.TransformationMatrix, sizeof(_float4x4));
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	return S_OK;
}

void CBone::Invalidate_CombinedTransformationMatrix(const vector<CBone*>& Bones, _fmatrix ParentsTransformatrixMatrix)
{
	if (true == m_isSurbordinate)
	{
		if (nullptr == m_pParentCombinedMatrix)
			return;

		m_CombinedTransformationMatrix = *m_pParentCombinedMatrix;
	}

	//	�θ� ���� ���ٸ� => ��Ʈ �����
	else if (-1 == m_iParentBoneIndex)
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * ParentsTransformatrixMatrix);
	}

	else
	{
		//	�θ� ���� �Ĺ��ε� ��İ� ��İ��� ���ش�.
		//	ũ �� �� �� �� -> ���� Ʈ���������̼� * �θ��� ���
		_matrix			ParrentsCombinedMatrix = XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix);
		_matrix			CombinedMatrix = XMLoadFloat4x4(&m_TransformationMatrix) * ParrentsCombinedMatrix;

		XMStoreFloat4x4(&m_CombinedTransformationMatrix, CombinedMatrix);
	}
}

_float3 CBone::Invalidate_CombinedTransformationMatrix_RootMotion_Translation(const vector<CBone*>& Bones, _fmatrix ParentsTransformatrixMatrix)
{
	_vector			vScale, vRotation, vTranslation;
	//	�θ� ���� ���ٸ� => ��Ʈ �����
	if (-1 == m_iParentBoneIndex)
	{
		_matrix			CombinedMatrix = XMLoadFloat4x4(&m_TransformationMatrix) * ParentsTransformatrixMatrix;

		XMMatrixDecompose(&vScale, &vRotation, &vTranslation, CombinedMatrix);

		CombinedMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, XMVectorSet(0.f, 0.f, 0.f, 1.f));

		XMStoreFloat4x4(&m_CombinedTransformationMatrix, CombinedMatrix);
	}

	else
	{
		//	������ ȸ�����и� ���� ��Ʈ������ ��ȯ ��Ʈ������ ����Ѵ�.
		_matrix			ParrentsCombinedMatrix = XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix);
		_matrix			CombinedMatrix = XMLoadFloat4x4(&m_TransformationMatrix) * ParrentsCombinedMatrix;

		XMMatrixDecompose(&vScale, &vRotation, &vTranslation, CombinedMatrix);

		vRotation = XMQuaternionIdentity();
		CombinedMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, XMVectorSet(0.f, 0.f, 0.f, 1.f));
		

		XMStoreFloat4x4(&m_CombinedTransformationMatrix, CombinedMatrix);
	}

	//	������ �̵������� ��ȯ
	_float3			vTraslationFloat3;

	XMStoreFloat3(&vTraslationFloat3, vTranslation);

	return vTraslationFloat3;
}

_float4x4 CBone::Invalidate_CombinedTransformationMatrix_RootMotion_WorldMatrix(const vector<CBone*>& Bones, _fmatrix ParentsTransformatrixMatrix)
{
	_float4x4		CombinedFloat4x4;

	if (-1 == m_iParentBoneIndex)
	{
		XMStoreFloat4x4(&CombinedFloat4x4, XMLoadFloat4x4(&m_TransformationMatrix));

		XMStoreFloat4x4(&m_CombinedTransformationMatrix, ParentsTransformatrixMatrix);
	}

	else
	{
		_matrix			ParrentsCombinedMatrix = XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix);

		XMStoreFloat4x4(&CombinedFloat4x4, XMLoadFloat4x4(&m_TransformationMatrix));

		XMStoreFloat4x4(&m_CombinedTransformationMatrix, ParrentsCombinedMatrix);
	}

	return CombinedFloat4x4;
}

void CBone::Invalidate_CombinedTransformationMatrix_RootMotion(const vector<CBone*>& Bones, _fmatrix ParentsCombinedMatrix, _float4* pTranslation, _float4* pQuaternion)
{
	//_vector			vScale, vRotation, vTranslation;

	//_bool			isSetTranslation = { pTranslation != nullptr };
	//_bool			isSetQuaternion = { pQuaternion != nullptr };

	////	�θ� ���� ���ٸ� => ��Ʈ �����
	//if (-1 == m_iParentBoneIndex)
	//{
	//	_matrix			CombinedMatrix = XMLoadFloat4x4(&m_TransformationMatrix) * ParentsCombinedMatrix;

	//	XMMatrixDecompose(&vScale, &vRotation, &vTranslation, CombinedMatrix);

	//	if(true == isSet)

	//	CombinedMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	//	//	�θ��� ������� �״�� ���� => ���� ��ȯ�� ���� �� ( �׵������ �����԰� ����. )
	//	XMStoreFloat4x4(&m_CombinedTransformationMatrix, CombinedMatrix);
	//}

	//else
	//{
	//	//	������ ȸ�����и� ���� ��Ʈ������ ��ȯ ��Ʈ������ ����Ѵ�.
	//	_matrix			ParrentsCombinedMatrix = XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix);
	//	_matrix			CombinedMatrix = XMLoadFloat4x4(&m_TransformationMatrix) * ParrentsCombinedMatrix;

	//	XMMatrixDecompose(&vScale, &vRotation, &vTranslation, CombinedMatrix);

	//	vRotation = XMQuaternionIdentity();
	//	CombinedMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, XMVectorSet(0.f, 0.f, 0.f, 1.f));


	//	XMStoreFloat4x4(&m_CombinedTransformationMatrix, CombinedMatrix);
	//}

	////	������ �̵������� ��ȯ
	//_float3			vTraslationFloat3;

	//XMStoreFloat3(&vTraslationFloat3, vTranslation);
}

void CBone::Set_Combined_Matrix(_fmatrix CombinedMatrix)
{
	_vector		vNewScale, vNewTranslation, vNewQuaternion;
	XMMatrixDecompose(&vNewScale, &vNewQuaternion, &vNewTranslation, CombinedMatrix);



	XMStoreFloat4x4(&m_CombinedTransformationMatrix, CombinedMatrix);
}

void CBone::Set_Parent_CombinedMatrix_Ptr(_float4x4* pParentMatrix)
{
	if (nullptr == pParentMatrix)
		return;

	m_pParentCombinedMatrix = pParentMatrix;
}

void CBone::Set_RootBone(_bool isRootBone)
{
	m_isRootBone = isRootBone;
}

CBone* CBone::Create(const aiNode* pAINode, _int iParentIndex)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAINode, iParentIndex)))
	{
		MSG_BOX(TEXT("Failed To Created : CBone"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Create(const BONE_DESC& BoneDesc)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(BoneDesc)))
	{
		MSG_BOX(TEXT("Failed To Created : CBone"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
	__super::Free();
}
