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
	_bool			isTopParrentBone = { -1 == m_iParentBoneIndex };

	if (true == m_isSurbordinate)
	{
		if (nullptr == m_pParentCombinedMatrix)
			return;

		m_CombinedTransformationMatrix = *m_pParentCombinedMatrix;
	}

	//	�θ� ���� ���ٸ� => ��Ʈ �����
	else if (true == isTopParrentBone)
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

void CBone::Invalidate_CombinedTransformationMatrix_RootMotion(const vector<CBone*>& Bones, _float4x4 TransformationMatrix, _bool isActiveXZ, _bool isActiveY, _bool isActiveRotation, _float4* pTranslation, _float4* pQuaternion)
{
	_vector			vScale, vQuaternion, vTranslation;

	_bool			isSetTranslation = { isActiveXZ || isActiveY };
	_bool			isSetQuaternion = { isActiveRotation };
	_bool			isTopParrentBone = { -1 == m_iParentBoneIndex };

	if (true == m_isSurbordinate)
	{
		if (nullptr == m_pParentCombinedMatrix)
			return;

		m_CombinedTransformationMatrix = *m_pParentCombinedMatrix;
	}

	//	�θ� ���� ���ٸ� => ��Ʈ �����
	else if (true == isTopParrentBone)
	{
		//	���� ���� ��ȯ���е��� �����Ͽ� Ư�������� ������ ��ȯ
		_matrix			MyTransformationMatrix = { XMLoadFloat4x4(&m_TransformationMatrix) };
		XMMatrixDecompose(&vScale, &vQuaternion, &vTranslation, MyTransformationMatrix);

		if (true == isSetQuaternion)
		{
			vQuaternion = Decompose_Quaternion(vQuaternion, pQuaternion);

			_vector			vInverseQuaternion = { XMQuaternionInverse(vQuaternion) };
			_matrix			InverseRotationMatrix = { XMMatrixRotationQuaternion(vInverseQuaternion) };

			_vector			vResultTranslation = { XMVector3TransformNormal(vTranslation, InverseRotationMatrix) };

			vTranslation = XMVectorSetW(vResultTranslation, 1.f);
		}

		if (true == isSetTranslation)
		{
			vTranslation = Decompose_Translation(isActiveXZ, isActiveY, vTranslation, pTranslation);
		}
		

		MyTransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vQuaternion, vTranslation);
		XMStoreFloat4x4(&m_TransformationMatrix, MyTransformationMatrix);

		// ���� ������� �ִϸ��̼ǿ� ���� Ư�� ���е��� ���ŵ� ��Ʈ������ ����Ŀ� �����ش�.
		_matrix			CombinedMatrix = XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&TransformationMatrix);
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, CombinedMatrix);
	}

	else
	{
		//	���� ���� ��ȯ���е��� �����Ͽ� Ư�������� ������ ��ȯ
		_matrix			MyTransformationMatrix = { XMLoadFloat4x4(&m_TransformationMatrix) };
		XMMatrixDecompose(&vScale, &vQuaternion, &vTranslation, MyTransformationMatrix);

		if (true == isSetQuaternion)
		{
			vQuaternion = Decompose_Quaternion(vQuaternion, pQuaternion);

			_vector			vInverseQuaternion = { XMQuaternionInverse(XMLoadFloat4(pQuaternion)) };
			_matrix			InverseRotationMatrix = { XMMatrixRotationQuaternion(vInverseQuaternion) };

			_vector			vResultTranslation = { XMVector3TransformNormal(vTranslation, InverseRotationMatrix) };

			vTranslation = XMVectorSetW(vResultTranslation, 1.f);
		}

		if (true == isSetTranslation)
		{
			vTranslation = Decompose_Translation(isActiveXZ, isActiveY, vTranslation, pTranslation);
		}		

		MyTransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vQuaternion, vTranslation);
		XMStoreFloat4x4(&m_TransformationMatrix, MyTransformationMatrix);

		//	������ ȸ�����и� ���� ��Ʈ������ ��ȯ ��Ʈ������ ����Ѵ�.
		_matrix			ParrentsCombinedMatrix = XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix);
		_matrix			CombinedMatrix = MyTransformationMatrix * ParrentsCombinedMatrix;
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, CombinedMatrix);
	}
}

_vector CBone::Decompose_Translation(_bool isActiveXZ, _bool isActiveY, _fvector vTranslation, _float4* pTranslation)
{
	_vector		vResultTranslation = { vTranslation };

	if (true == isActiveXZ)
	{
		pTranslation->x = XMVectorGetX(vResultTranslation);
		pTranslation->z = XMVectorGetZ(vResultTranslation);

		vResultTranslation = XMVectorSetX(vResultTranslation, 0.f);
		vResultTranslation = XMVectorSetZ(vResultTranslation, 0.f);
	}

	if (true == isActiveY)
	{
		pTranslation->y = XMVectorGetY(vResultTranslation);

		vResultTranslation = XMVectorSetY(vResultTranslation, 0.f);
	}

	return vResultTranslation;
}

_vector CBone::Decompose_Quaternion(_fvector vQuaternion, _float4* pQuaternion)
{
	_vector		vResultQuaternion = { vQuaternion };

	XMStoreFloat4(pQuaternion, vQuaternion);
	vResultQuaternion = XMQuaternionIdentity();

	return vResultQuaternion;
}

void CBone::Set_Combined_Matrix(_fmatrix CombinedMatrix)
{
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
