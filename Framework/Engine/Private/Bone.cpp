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
	//	Assimp의 모든 행렬은 전치되어있으므로 다시 전치 하여 저장해줘야한다.
	//	Assimp(Assimp은 오픈 소스 3D 모델 임포트 라이브러리입니다)에서는 모든 행렬을 column-major 형태로 다룹니다
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

	//	부모 뼈가 없다면 => 루트 노드라면
	else if (true == isTopParrentBone)
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * ParentsTransformatrixMatrix);
	}

	else
	{
		//	부모 뼈의 컴바인드 행렬과 행렬곱을 해준다.
		//	크 자 이 공 부 -> 나의 트랜스포메이션 * 부모의 행렬
		_matrix			ParrentsCombinedMatrix = XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix);
		_matrix			CombinedMatrix = XMLoadFloat4x4(&m_TransformationMatrix) * ParrentsCombinedMatrix;

		XMStoreFloat4x4(&m_CombinedTransformationMatrix, CombinedMatrix);
	}
}

void CBone::Invalidate_CombinedTransformationMatrix_RootMotion(const vector<CBone*>& Bones, _float4x4 TransformationMatrix, _bool isActiveXZ, _bool isActiveY, _bool isActiveRotation, _float4* pTranslation, _float4* pQuaternion)
{
	/*_vector			vScale, vQuaternion, vTranslation;

	_bool			isSetTranslation = { isActiveXZ || isActiveY };
	_bool			isSetQuaternion = { isActiveRotation };*/
	_bool			isTopParrentBone = { -1 == m_iParentBoneIndex };

	if (true == m_isSurbordinate)
	{
		if (nullptr == m_pParentCombinedMatrix)
			return;

		m_CombinedTransformationMatrix = *m_pParentCombinedMatrix;
	}

	//	부모 뼈가 없다면 => 루트 노드라면
	else if (true == isTopParrentBone)
	{
		//	현재 뼈의 변환성분들을 분해하여 특정성분을 추출후 반환
		/*_matrix			MyTransformationMatrix = { XMLoadFloat4x4(&m_TransformationMatrix) };
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
		XMStoreFloat4x4(&m_TransformationMatrix, MyTransformationMatrix);*/

		// 현재 결과에서 애니메이션에 따라 특정 성분들이 제거된 매트릭스를 합행렬에 곱해준다.
		_matrix			CombinedMatrix = XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&TransformationMatrix);
		XMStoreFloat4x4(&m_CombinedTransformationMatrix, CombinedMatrix);
	}

	else
	{
		//	현재 뼈의 변환성분들을 분해하여 특정성분을 추출후 반환
		/*_matrix			MyTransformationMatrix = { XMLoadFloat4x4(&m_TransformationMatrix) };
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
		XMStoreFloat4x4(&m_TransformationMatrix, MyTransformationMatrix);*/

		_matrix			ParrentsCombinedMatrix = XMLoadFloat4x4(&Bones[m_iParentBoneIndex]->m_CombinedTransformationMatrix);
		_matrix			CombinedMatrix = XMLoadFloat4x4(&m_TransformationMatrix) * ParrentsCombinedMatrix;
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

_float4x4* CBone::Get_ParentCombinedMatrix_RagDoll_Ptr(_uint iRagDollIndex)
{
	_uint			iNumMatrices = { static_cast<_uint>(m_pParentCombinedMatrices_RagDoll.size()) };
	if (iNumMatrices <= iRagDollIndex)
		return nullptr;

	return m_pParentCombinedMatrices_RagDoll[iRagDollIndex];
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

void CBone::Set_Parent_CombinedMatrix_Ptr_RagDoll(_uint iRagDollIndex, _float4x4* pParentMatrix)
{
	if (nullptr == pParentMatrix)
		return;

	_uint			iNumMatrices = { static_cast<_uint>(m_pParentCombinedMatrices_RagDoll.size()) };
	if (iNumMatrices <= iRagDollIndex)
		return;

	m_pParentCombinedMatrices_RagDoll[iRagDollIndex] = pParentMatrix;
}

void CBone::Set_RootBone(_bool isRootBone)
{
	m_isRootBone = isRootBone;
}

_bool CBone::Is_Set_ParentCombiend_RagDoll(_uint iRagDollIndex)
{
	_uint			iNumMatrices = { static_cast<_uint>(m_pParentCombinedMatrices_RagDoll.size()) };
	if (iNumMatrices <= iRagDollIndex)
		return false;

	return nullptr != m_pParentCombinedMatrices_RagDoll[iRagDollIndex];
}

void CBone::Resize_ParentCombinedMatrices_RagDoll(_uint iNumRagDoll)
{
	m_pParentCombinedMatrices_RagDoll.resize(iNumRagDoll);
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
