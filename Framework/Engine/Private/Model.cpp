#include "..\Public\Model.h"

#include "Shader.h"
#include "Texture.h"

#include "GameInstance.h"

#include "Model_Extractor.h"

#include "Animation.h"
#include "Bone.h"
#include "Mesh.h"
#include "Bone_Layer.h"

#include "Transform.h"
#include "RagDoll_Physics.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CModel::CModel(const CModel& rhs)
	: CComponent{ rhs }
	, m_eModelType{ rhs.m_eModelType }
	, m_iNumMeshes{ rhs.m_iNumMeshes }
	, m_Meshes{ rhs.m_Meshes }
	, m_iNumMaterials{ rhs.m_iNumMaterials }
	, m_Materials{ rhs.m_Materials }
	, m_TransformationMatrix{ rhs.m_TransformationMatrix }
	, m_iNumAnimations{ rhs.m_iNumAnimations }
{
	for (auto& pPrototypeAnimation : rhs.m_Animations)
		m_Animations.push_back(pPrototypeAnimation->Clone());

	for (auto& pPrototypeBone : rhs.m_Bones)
		m_Bones.push_back(pPrototypeBone->Clone());

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& Material : m_Materials)
	{
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			Safe_AddRef(Material.MaterialTextures[i]);
	}

	m_PlayingAnimInfos.resize(3);
}

void CModel::Set_Animation_Blend(ANIM_PLAYING_DESC AnimDesc, _uint iPlayingIndex)
{
	if (static_cast<_uint>(AnimDesc.iAnimIndex) >= m_iNumAnimations)
		return;

	if (iPlayingIndex >= static_cast<_uint>(m_PlayingAnimInfos.size()))
		return;

	if (AnimDesc.iAnimIndex == m_PlayingAnimInfos[iPlayingIndex].iAnimIndex)
		return;

	vector<KEYFRAME>			LastKeyFrames;
	LastKeyFrames.resize(m_Animations[AnimDesc.iAnimIndex]->Get_Channels().size());

	_int						iPreAnimIndex = { m_PlayingAnimInfos[iPlayingIndex].iAnimIndex };

	if (-1 != m_PlayingAnimInfos[iPlayingIndex].iAnimIndex)
	{
		m_Animations[m_PlayingAnimInfos[iPlayingIndex].iAnimIndex]->Reset_TrackPostion();
		m_Animations[m_PlayingAnimInfos[iPlayingIndex].iAnimIndex]->Reset_Finish();

		LastKeyFrames = m_PlayingAnimInfos[iPlayingIndex].LastKeyFrames;
	}

	ANIM_PLAYING_INFO		AnimInfo;
	AnimInfo.iAnimIndex = AnimDesc.iAnimIndex;
	AnimInfo.isLoop = AnimDesc.isLoop;
	AnimInfo.fWeight = AnimDesc.fWeight;
	AnimInfo.strBoneLayerTag = AnimDesc.strBoneLayerTag;
	AnimInfo.LastKeyFrames = LastKeyFrames;
	AnimInfo.iPreAnimIndex = iPreAnimIndex;

	m_PlayingAnimInfos[iPlayingIndex] = AnimInfo;

	m_Animations[m_PlayingAnimInfos[iPlayingIndex].iAnimIndex]->Reset_TrackPostion();
}

_uint CModel::Get_CurrentAnimIndex(_uint iPlayingIndex)
{
	if (iPlayingIndex >= static_cast<_uint>(m_PlayingAnimInfos.size()))
		return false;

	_uint			iAnimIndex = { static_cast<_uint>(m_PlayingAnimInfos[iPlayingIndex].iAnimIndex) };
	return iAnimIndex;
}

void CModel::Set_TickPerSec(_uint iAnimIndex, _float fTickPerSec)
{
	if (iAnimIndex >= m_Animations.size())
		return;

	m_Animations[iAnimIndex]->Set_TickPerSec(fTickPerSec);
}

void CModel::Set_Weight(_uint iPlayingIndex, _float fWeight)
{
	if (iPlayingIndex >= static_cast<_uint>(m_PlayingAnimInfos.size()))
		return;

	if (0.f > fWeight)
		fWeight = 0.f;

	m_PlayingAnimInfos[iPlayingIndex].fWeight = fWeight;
}

_bool CModel::Is_Active_RootMotion_XZ()
{
	return m_isRootMotion_XZ;
}

_bool CModel::Is_Active_RootMotion_Y()
{
	return m_isRootMotion_Y;
}

_bool CModel::Is_Active_RootMotion_Rotation()
{
	return m_isRootMotion_Rotation;
}

void CModel::Active_RootMotion_XZ(_bool isActive)
{
	m_isRootMotion_XZ = isActive;
}

void CModel::Active_RootMotion_Y(_bool isActive)
{
	m_isRootMotion_Y = isActive;
}

void CModel::Active_RootMotion_Rotation(_bool isActive)
{
	m_isRootMotion_Rotation = isActive;
}

void CModel::Set_RootBone(string strBoneTag)
{
	_int		iBoneIndex = { Find_BoneIndex(strBoneTag) };
	if (-1 == iBoneIndex)
		return;

	for (auto& pBone : m_Bones)
		pBone->Set_RootBone(false);

	m_Bones[iBoneIndex]->Set_RootBone(true);
}

void CModel::Add_IK(string strTargetJointTag, string strEndEffectorTag, wstring strIKTag, _uint iNumIteration, _float fBlend)
{
	map<wstring, IK_INFO>::iterator		iter = { m_IKInfos.find(strIKTag) };
	if (iter != m_IKInfos.end())
		return;

	_int		iNumBones = { static_cast<_int>(m_Bones.size()) };

	_int		iIKRootBoneIndex = { Find_BoneIndex(strTargetJointTag) };
	_int		iEndEffectorIndex = { Find_BoneIndex(strEndEffectorTag) };

	if (iIKRootBoneIndex >= iNumBones ||
		iIKRootBoneIndex == -1 ||
		iEndEffectorIndex >= iNumBones ||
		iEndEffectorIndex == -1)
		return;

	_bool		isCombined = { false };
	IK_INFO			IkInfo;

	IkInfo.iNumIteration = iNumIteration;
	IkInfo.fBlend = fBlend;

	vector<_uint>			JointIndices;
	_uint					iBoneIndex = { static_cast<_uint>(iEndEffectorIndex) };
	JointIndices.push_back(iBoneIndex);
	while (false == isCombined)
	{
		_int		iParrentIndex = { m_Bones[iBoneIndex]->Get_ParentIndex() };
		if (-1 == iParrentIndex)
		{
			return;
		}

		if (iIKRootBoneIndex == iParrentIndex)
		{
			isCombined = true;
		}

		iBoneIndex = iParrentIndex;
		JointIndices.push_back(iParrentIndex);
	}

	if (true == isCombined)
	{
		IkInfo.iIKRootBoneIndex = iIKRootBoneIndex;
		IkInfo.iEndEffectorIndex = iEndEffectorIndex;

		_uint		iNumIndices = { static_cast<_uint>(JointIndices.size()) };
		IkInfo.JointIndices.resize(iNumIndices);
		for (_int i = iNumIndices - 1; i >= 0; --i)
		{
			IkInfo.JointIndices[(iNumIndices - 1) - i] = JointIndices[i];
		}
	}

	IkInfo.IKIncludedIndices.clear();
	IkInfo.IKIncludedIndices.push_front(IkInfo.iIKRootBoneIndex);

	Get_Child_BoneIndices(strTargetJointTag, IkInfo.IKIncludedIndices);

	//	�ӽ� �߰�
	//	�ӽ� �߰�
	//	�ӽ� �߰�
	//	�ӽ� �߰�

	_uint			iNumJoint = { static_cast<_uint>(IkInfo.JointIndices.size()) };

	IkInfo.BoneThetas.clear();
	//	IkInfo.BoneThetas.push_back(_float4(XMConvertToRadians(10.f), XMConvertToRadians(10.f), XMConvertToRadians(10.f), XMConvertToRadians(10.f)));
	IkInfo.BoneThetas.push_back(_float4(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(40.f)));
	IkInfo.BoneThetas.push_back(_float4(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(40.f)));
	IkInfo.BoneThetas.push_back(_float4(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(40.f)));
	IkInfo.BoneThetas.push_back(_float4(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(40.f)));
	//	IkInfo.BoneThetas.push_back(_float4(XMConvertToRadians(10.f), XMConvertToRadians(10.f), XMConvertToRadians(10.f), XMConvertToRadians(10.f)));
	//	IkInfo.BoneThetas.push_back(_float4(XMConvertToRadians(10.f), XMConvertToRadians(10.f), XMConvertToRadians(10.f), XMConvertToRadians(10.f)));

	IkInfo.BoneOrientationLimits.clear();
	for (_uint i = 0; i < iNumJoint; ++i)
		IkInfo.BoneOrientationLimits.push_back(XMConvertToRadians(40.f));

	IkInfo.JointTypes.clear();
	/*IkInfo.JointTypes.push_back(JOINT_TYPE::JOINT_BALL);
	IkInfo.JointTypes.push_back(JOINT_TYPE::JOINT_HINGE);
	IkInfo.JointTypes.push_back(JOINT_TYPE::JOINT_BALL);
	IkInfo.JointTypes.push_back(JOINT_TYPE::JOINT_END);*/
	IkInfo.JointTypes.push_back(JOINT_TYPE::JOINT_HINGE);
	IkInfo.JointTypes.push_back(JOINT_TYPE::JOINT_HINGE);
	IkInfo.JointTypes.push_back(JOINT_TYPE::JOINT_HINGE);
	IkInfo.JointTypes.push_back(JOINT_TYPE::JOINT_END);
	//	IkInfo.JointTypes.push_back(i % 2 == 0 ? JOINT_TYPE::JOINT_BALL : JOINT_TYPE::JOINT_HINGE);

//	�ӽ� �߰�
//	�ӽ� �߰�
//	�ӽ� �߰�
//	�ӽ� �߰�

	m_IKInfos[strIKTag] = IkInfo;
}

void CModel::Set_Direction_IK(wstring strIKTag, _fvector vDirection)
{
	map<wstring, IK_INFO>::iterator		iter = { m_IKInfos.find(strIKTag) };
	if (iter == m_IKInfos.end())
		return;

	XMStoreFloat3(&m_IKInfos[strIKTag].vIKDirection, vDirection);
}

void CModel::Set_TargetPosition_IK(wstring strIKTag, _fvector vTargetPosition)
{
	map<wstring, IK_INFO>::iterator		iter = { m_IKInfos.find(strIKTag) };
	if (iter == m_IKInfos.end())
		return;

	XMStoreFloat3(&m_IKInfos[strIKTag].vIKEndTargetPosition, vTargetPosition);
}

void CModel::Set_NumIteration_IK(wstring strIKTag, _uint iNumIteration)
{
	map<wstring, IK_INFO>::iterator		iter = { m_IKInfos.find(strIKTag) };
	if (iter == m_IKInfos.end())
		return;

	m_IKInfos[strIKTag].iNumIteration = iNumIteration;
}

HRESULT CModel::RagDoll()
{
	for (auto& pBone : m_Bones)
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformationMatrix));

	return S_OK;
}

void CModel::Set_Blend_IK(wstring strIKTag, _float fBlend)
{
	map<wstring, IK_INFO>::iterator		iter = { m_IKInfos.find(strIKTag) };
	if (iter == m_IKInfos.end())
		return;

	m_IKInfos[strIKTag].fBlend = fBlend;
}

vector<_float4> CModel::Get_ResultTranslation_IK(const wstring& strIKTag)
{
	return m_IKInfos[strIKTag].BoneTranslationsResult;
}

vector<_float4> CModel::Get_OriginTranslation_IK(const wstring& strIKTag)
{
	return m_IKInfos[strIKTag].BoneTranslationsOrigin;
}

vector<_float4x4> CModel::Get_JointCombinedMatrices_IK(const wstring& strIKTag)
{
	vector<_float4x4>		JointCombinedMatrices;
	for (auto& iBoneIndex : m_IKInfos[strIKTag].JointIndices)
	{
		JointCombinedMatrices.push_back(*m_Bones[iBoneIndex]->Get_CombinedTransformationMatrix());
	}

	return JointCombinedMatrices;
}

void CModel::Apply_IK(class CTransform* pTransform, IK_INFO& IkInfo)
{
	if (-1 == IkInfo.iEndEffectorIndex || -1 == IkInfo.iIKRootBoneIndex)
		return;

	//	���� ���ʹ� ���� ����� ����ȯ �Ͽ� ���û��� �������� ġȯ�Ѵ�.
	_matrix			WorldMatrixInv = { pTransform->Get_WorldMatrix_Inverse() };
	_vector			vEndEffectorMoveDir = { XMLoadFloat3(&IkInfo.vIKDirection) };
	vEndEffectorMoveDir = XMVector3TransformNormal(vEndEffectorMoveDir, WorldMatrixInv);

	_vector			vEndEffectorPosition = { XMVectorSetW(XMLoadFloat3(&IkInfo.vIKEndTargetPosition), 1.f) };
	vEndEffectorMoveDir = XMVector3TransformCoord(vEndEffectorPosition, WorldMatrixInv);
	_vector			vEndEffectorResultPosition = { vEndEffectorPosition };

	//	_vector			vEndEffectorResultPosition = { XMLoadFloat4((_float4*)m_Bones[IkInfo.iEndEffectorIndex]->Get_CombinedTransformationMatrix()->m[CTransform::STATE_POSITION]) + vEndEffectorMoveDir };
	XMStoreFloat4(&IkInfo.vEndEffectorResultPosition, vEndEffectorResultPosition);

	_vector			vTargetJointStartTranslation = { XMLoadFloat4((_float4*)m_Bones[IkInfo.iIKRootBoneIndex]->Get_CombinedTransformationMatrix()->m[CTransform::STATE_POSITION]) };
	XMStoreFloat4(&IkInfo.vTargetJointStartTranslation, vTargetJointStartTranslation);

	//	���� ���� �Ÿ��� ��ġ���� �ʱ�ȭ�� ���� �ִϸ��̼ǿ� �°� ������ ����
	Update_Distances_Translations_IK(IkInfo);

	for (_uint i = 0; i < IkInfo.iNumIteration; ++i)
	{
		////	EndEffector�� ���� StartJoint���� ������ ��ȸ => �ڽ� ���� �θ������...
		//Update_Forward_Reaching_IK(IkInfo);

		////	StartJoint�� ���� EndEffector���� ������ ��ȸ => �θ� ���� �ڽļ�����
		//Update_Backward_Reaching_IK(IkInfo);

		Solve_IK(IkInfo);
	}

	Update_TransformMatrices_BoneChain(IkInfo);

	Update_CombinedMatrices_BoneChain(IkInfo);
}

void CModel::Update_Distances_Translations_IK(IK_INFO& IkInfo)
{
	//	�ε��� ���� ���� => �θ� => �ڽ�
	//	�ڽ� ���� ��ġ���� �Ÿ��� ����Ѵ�.
	_uint			iNumIKIndices = { static_cast<_uint>(IkInfo.JointIndices.size()) };

	IkInfo.TargetDistancesToChild.clear();
	IkInfo.TargetDistancesToParrent.clear();
	IkInfo.BoneTranslationsResult.clear();
	IkInfo.BoneTranslationsOrigin.clear();

	//	�߰�����
	IkInfo.BoneOrientations.clear();

	for (_uint i = 0; i < iNumIKIndices; ++i)
	{
		_matrix			CombinedMatrix = { XMLoadFloat4x4(m_Bones[IkInfo.JointIndices[i]]->Get_CombinedTransformationMatrix()) };

		_vector			vScale, vQuaternion, vTranslation;
		XMMatrixDecompose(&vScale, &vQuaternion, &vTranslation, CombinedMatrix);

		_float4			vQuaternionFloat4;
		XMStoreFloat4(&vQuaternionFloat4, vQuaternion);
		IkInfo.BoneOrientations.push_back(vQuaternionFloat4);
	}

	for (_uint i = 0; i < iNumIKIndices; ++i)
	{
		_bool		isExistParrents = { i != 0 };				//	ù �ε����� �ƴϸ� �θ�������� => ù�ε��� Ÿ�� ����Ʈ
		_bool		isExistChild = { i < iNumIKIndices - 1 };	//	������ �ε����� �ƴϸ� �ڽĻ��� ����

		_vector			vMyTranslation = { XMLoadFloat4((_float4*)(&m_Bones[IkInfo.JointIndices[i]]->Get_CombinedTransformationMatrix()->m[CTransform::STATE_POSITION][0])) };
		_float4			vMyTranslationFloat4;
		XMStoreFloat4(&vMyTranslationFloat4, vMyTranslation);

		IkInfo.BoneTranslationsOrigin.push_back(vMyTranslationFloat4);

		_vector			vParrentTranslation, vChildTranslation;
		if (true == isExistParrents)
		{
			vParrentTranslation = { XMLoadFloat4((_float4*)(&m_Bones[IkInfo.JointIndices[i - 1]]->Get_CombinedTransformationMatrix()->m[CTransform::STATE_POSITION][0])) };
		}
		else
		{
			vParrentTranslation = { XMLoadFloat4(&IkInfo.vTargetJointStartTranslation) };
		}

		if (true == isExistChild)
		{
			vChildTranslation = { XMLoadFloat4((_float4*)(&m_Bones[IkInfo.JointIndices[i + 1]]->Get_CombinedTransformationMatrix()->m[CTransform::STATE_POSITION][0])) };
		}
		else
		{
			vChildTranslation = { XMLoadFloat4(&IkInfo.vEndEffectorResultPosition) };
		}
		_vector			vDirectionToChild = { vChildTranslation - vMyTranslation };
		_float			fDistanceToChild = { XMVectorGetX(XMVector3Length(vDirectionToChild)) };
		IkInfo.TargetDistancesToChild.push_back(fDistanceToChild);

		_vector			vDirectionToParrent = { vParrentTranslation - vMyTranslation };
		_float			fDistanceToParrent = { XMVectorGetX(XMVector3Length(vDirectionToParrent)) };
		IkInfo.TargetDistancesToParrent.push_back(fDistanceToParrent);
	}

	IkInfo.BoneTranslationsResult.resize(IkInfo.BoneTranslationsOrigin.size());
	for (_uint i = 0; i < static_cast<_uint>(IkInfo.BoneTranslationsOrigin.size()); ++i)
	{
		IkInfo.BoneTranslationsResult[i] = IkInfo.BoneTranslationsOrigin[i];
	}

	IkInfo.TargetDistancesToChild[iNumIKIndices - 1] = 0.f;
}

void CModel::Solve_IK(IK_INFO& IkInfo)
{
	Solve_IK_Forward(IkInfo);

	Solve_IK_Backward(IkInfo);
}

void CModel::Solve_IK_Forward(IK_INFO& IkInfo)
{
	_uint			iNumIKIndices = { static_cast<_uint>(IkInfo.JointIndices.size()) };

	//	���� �������� ��ġ�� ��ǥ��ġ�� �ű��.
	IkInfo.BoneTranslationsResult[iNumIKIndices - 1] = IkInfo.vEndEffectorResultPosition;
	//	IkInfo.BoneOrientations[iNumIKIndices - 1] = IkInfo.AdditionalRotateQuaternions;

	for (_int i = iNumIKIndices - 2; i >= 0; --i)
	{
		//	���� ������ �θ��� ���
		_bool		isLastParent = { i == iNumIKIndices - 2 };

		Solve_For_Distance_IK(IkInfo, i + 1, i);
		Solve_For_Orientation_IK(IkInfo, i + 1, i);

		//			�ּ� iNumIKIndeces - 3 => ���ο��� �ڽ� ����� ������... 
		if (false == isLastParent)
		{
			//	��Ÿ, ù ��ġ, ����Ÿ��, ���������̼� ��ϵ�...
			//	�ڽĻ�, ��Ÿ, ù ��ġ ( length�� ���� constranints ������ ), ���� ���� ( ���� Ÿ�� �ڽĻ��� �ε�����... ), ���������̼� ( ȸ���� �ڽĻ� ...)
			//	�ڽĻ� �ε���( IkInfo �󿡼� ... ),
			Rotational_Constranit(IkInfo, i + 1, i);
		}
	}
}

void CModel::Solve_IK_Backward(IK_INFO& IkInfo)
{
	_uint			iNumIKIndices = { static_cast<_uint>(IkInfo.JointIndices.size()) };

	//	���� �������� ��ġ�� ��ǥ��ġ�� �ű��.
	IkInfo.BoneTranslationsResult[0] = IkInfo.vTargetJointStartTranslation;

	for (_int i = 1; i < static_cast<_int>(iNumIKIndices); ++i)
	{
		Solve_For_Distance_IK(IkInfo, i - 1, i);
	}
}

void CModel::Solve_For_Distance_IK(IK_INFO& IkInfo, _int iSrcJointIndex, _int iDstJointIndex)
{
	_vector			vResultOuterJointTranslation = { XMLoadFloat4(&IkInfo.BoneTranslationsResult[iSrcJointIndex]) };
	_vector			vResultInnerJointTranslation = { XMLoadFloat4(&IkInfo.BoneTranslationsResult[iDstJointIndex]) };
	_vector			vOriginOuterJointTranslation = { XMLoadFloat4(&IkInfo.BoneTranslationsOrigin[iSrcJointIndex]) };
	_vector			vOriginInnerJointTranslation = { XMLoadFloat4(&IkInfo.BoneTranslationsOrigin[iDstJointIndex]) };

	_float			fR = { XMVectorGetX(XMVector3Length(vResultOuterJointTranslation - vResultInnerJointTranslation)) };
	_float			fLanda = { XMVectorGetX(XMVector3Length(vOriginOuterJointTranslation - vOriginInnerJointTranslation)) / fR };

	_vector			vResultPos = { XMVectorScale(vResultOuterJointTranslation, 1.f - fLanda) + XMVectorScale(vResultInnerJointTranslation, fLanda) };
	XMStoreFloat4(&IkInfo.BoneTranslationsResult[iDstJointIndex], vResultPos);
}

void CModel::Solve_For_Orientation_IK(IK_INFO& IkInfo, _int iOuterJointIndex, _int iInnerJointIndex)
{
	_vector			vOuterJointOrientaion = { XMLoadFloat4(&IkInfo.BoneOrientations[iOuterJointIndex]) };
	_vector			vInnerJointOrientaion = { XMLoadFloat4(&IkInfo.BoneOrientations[iInnerJointIndex]) };

	_vector			vOuterJointOrientationInv = { XMQuaternionConjugate(vOuterJointOrientaion) };
	_float			vQuternionLength = { XMVectorGetX(XMQuaternionLength(vOuterJointOrientaion)) };
	_vector			vOuterJointOrientationInvResult = { vOuterJointOrientationInv * (1 / vQuternionLength) };

	_vector			vRotor = { XMQuaternionMultiply(vOuterJointOrientationInvResult, vInnerJointOrientaion) };

	_float			fNeededRotation = { 2.f * acosf(XMVectorGetW(vRotor)) };
	_float			fNeededRotation2;
	XMQuaternionToAxisAngle(&vRotor, &fNeededRotation2, XMQuaternionIdentity());

	//	�ʿ� ȸ������ �ڽ� ������ �ִ� ȸ�������� �ִ� ���
	if (fNeededRotation <= IkInfo.BoneOrientationLimits[iOuterJointIndex])
	{
		_vector		vResultInnerJointOrientation = { XMQuaternionMultiply(vOuterJointOrientaion, vRotor) };
		XMStoreFloat4(&IkInfo.BoneOrientations[iInnerJointIndex], vResultInnerJointOrientation);
	}
	else
	{
		//	�ڽ� ������ �ִ� ȸ������ �����´�.
		_float		fLimitAngle = { IkInfo.BoneOrientationLimits[iOuterJointIndex] };

		//	���ʹϾ𿡼� w������ ���� => ���ʹϾ��� ȸ�� �� ���⺤��
		_vector		vLimitedOriendtation = { XMVectorSet(
			XMVectorGetX(vRotor) / (1 / sqrtf(1.f - powf(XMVectorGetW(vRotor),2.f) * sinf(fLimitAngle * 0.5f))),
			XMVectorGetY(vRotor) / (1 / sqrtf(1.f - powf(XMVectorGetW(vRotor),2.f) * sinf(fLimitAngle * 0.5f))),
			XMVectorGetZ(vRotor) / (1 / sqrtf(1.f - powf(XMVectorGetW(vRotor),2.f) * sinf(fLimitAngle * 0.5f))),
			cosf(fLimitAngle * 0.5f))
		};

		XMStoreFloat4(&IkInfo.BoneOrientations[iInnerJointIndex], vLimitedOriendtation);
	}
}

void CModel::Rotational_Constranit(IK_INFO& IkInfo, _int iOuterJointIndex, _int iMyJointIndex)
{
	//	�ڽ�
	_vector			p_i = { XMLoadFloat4(&IkInfo.BoneTranslationsResult[iOuterJointIndex]) };
	//	�ڽ��� �ڽ�
	_vector			p_before = { XMLoadFloat4(&IkInfo.BoneTranslationsResult[iOuterJointIndex - 1]) };
	//	��
	_vector			p_next = { XMLoadFloat4(&IkInfo.BoneTranslationsResult[iOuterJointIndex + 1]) };

	_vector			v_i_next = { p_next - p_i };
	_vector			v_before_i = { p_i - p_before };
	/*_vector		vDirectionToParent = { v_i_next * -1.f };
	_vector		vDirectionFromGrandParentToParent = { p_i - p_before };*/

	_float			s = { XMVectorGetX(XMVector3Dot(v_i_next, v_before_i)) };

	//	# a unit vector of a line passing  from p(i+1) and P(i)
	//	���� �ڽ��� �ٶ󺸴� ����
	_float			l_next_i = { XMVectorGetX(XMVector3Length(p_next - p_i)) };
	_vector			unit_vec_next_i = { XMVector3Normalize(p_i - p_next) };
	//_vector		vDirectionToConeCenterFromParent = { XMVector3Normalize(vDirectionFromGrandParentToParent) * s };

	//	# the center of cone
	_vector			o = { p_i + unit_vec_next_i * s };

	if (JOINT_TYPE::JOINT_HINGE == static_cast<JOINT_TYPE>(IkInfo.JointTypes[iOuterJointIndex]))
	{
		//	normal plane vector of p(next), p(i), p(before)
		_vector		normal_plane = { XMVector3Cross(v_i_next, v_before_i) };
		_vector		uv_normal_plane = { XMVector3Normalize(normal_plane) };

		if (0.9999f < XMVectorGetX(XMVector3Dot(XMVector3Normalize(v_i_next), XMVector3Normalize(v_before_i))))
			return;

		//	 a vector from p_i to o
		_vector		unit_vec_i_o = { XMVector3Normalize(o - p_i) };

		//	 rotating p(i) - o C.C.W to find flexion constraint(left of pi_o)
		_matrix		RoateMatrixDown = { XMMatrixRotationAxis(normal_plane, IkInfo.BoneThetas[iOuterJointIndex].y) };
		_vector		p_down = { XMVector3TransformNormal(unit_vec_i_o, RoateMatrixDown) };

		//	 rotating p(i) - o C.W to find extension constraint(right of pi_o)
		_matrix		RoateMatrixUp = { XMMatrixRotationAxis(normal_plane, IkInfo.BoneThetas[iOuterJointIndex].w * -1.f) };
		_vector		p_up = { XMVector3TransformNormal(unit_vec_i_o, RoateMatrixUp) };

		_float		l_before_i = { XMVectorGetX(XMVector3Length(p_i - p_before)) };
		_vector		uv_i_before = { XMVector3Normalize(p_before - p_i) };

		_vector		vCrossToParentToCone = { XMVector3Cross(uv_i_before, unit_vec_i_o) };
		_float		fDotConePlaneNorm = { XMVectorGetX(XMVector3Dot(vCrossToParentToCone, uv_normal_plane)) };

		_float		fAngle = { acosf(XMVectorGetX(XMVector3Dot(uv_i_before, unit_vec_i_o))) };
		//	means �̰��� vMyTranslation �� �� �ִ�.
		if (fDotConePlaneNorm > 0.f)
		{
			if (IkInfo.BoneThetas[iOuterJointIndex].w != 0.f)
			{
				//	�� �ȿ��ִ�?
				//	# angle between vec(i_before) and vec(i_o)
				if (fAngle <= IkInfo.BoneThetas[iOuterJointIndex].w)
				{
					return;
				}

				else
				{
					_vector		vNearestPoint = { p_i + l_before_i * p_up };
					XMStoreFloat4(&IkInfo.BoneTranslationsResult[iMyJointIndex], vNearestPoint);
				}
			}

			else
			{
				_vector		vNearestPoint = { p_i + l_before_i * unit_vec_i_o };
				XMStoreFloat4(&IkInfo.BoneTranslationsResult[iMyJointIndex], vNearestPoint);
			}
		}

		//	means �̰��� vMyTranslation �Ʒ� �� �ִ�.
		else
		{
			if (IkInfo.BoneThetas[iOuterJointIndex].y != 0.f)
			{
				//	# angle between vec(i_before) and vec(i_o)
				if (fAngle <= IkInfo.BoneThetas[iOuterJointIndex].y)
				{
					return;
				}
				else
				{
					_vector		vNearestPoint = { p_i + l_before_i * p_down };
					XMStoreFloat4(&IkInfo.BoneTranslationsResult[iMyJointIndex], vNearestPoint);
				}
			}

			else
			{
				_vector		vNearestPoint = { p_i + l_before_i * unit_vec_i_o };
				XMStoreFloat4(&IkInfo.BoneTranslationsResult[iMyJointIndex], vNearestPoint);
			}
		}
		return;
	}

	if (JOINT_TYPE::JOINT_BALL == static_cast<JOINT_TYPE>(IkInfo.JointTypes[iOuterJointIndex]))
	{
		// ��Ÿ��ü �Ű� ���� qi(1, 2, 3, 4)
		_vector			vQ = {
			XMVectorSet(
			s * tanf(IkInfo.BoneThetas[iOuterJointIndex].x),
			s * tanf(IkInfo.BoneThetas[iOuterJointIndex].y),
			s * tanf(IkInfo.BoneThetas[iOuterJointIndex].z),
			s * tanf(IkInfo.BoneThetas[iOuterJointIndex].w))
		};

		// ��ǥ�� ������ �ܸ����� �����ϰ� �� ���� (i - 1)��° ��ġ�� ����մϴ�.
		//		�� ���� -> �ڽ� ( ���� ���� �Ÿ� )
		//		# change the coordinate to cross section of cone and calculating the (i-1)th position in it
		_float			l_o_next = { XMVectorGetX(XMVector3Length(o - p_next)) };

		//		���� ���� ������ ����� ��и鿡�ִ��� ã�´�.
		//		ToDo : Theta�� ����ϴ¹� ã�� ���������� ���� �� ���ϴµ� ����
		//		 # the orientation of joint
		_float				fTheta = { acosf(IkInfo.BoneOrientations[iOuterJointIndex].w) * 2.f };
		CONIC_SECTION		eSection = { Find_ConicSection(fTheta) };

		_float2				vTargetPoint = {
			l_o_next * cosf(fTheta),
			l_o_next * sinf(fTheta)
		};

		//	��ǥ���� Ÿ������������ Ȯ��		
		//	���� ���� ��и� ������ ���
		_bool			isInBound = { Is_InBound(eSection, vQ, vTargetPoint) };
		_float2			vNearestPoint = {};

		if (true == isInBound)
			return;

		//	it is out bound of the ellipsoidal shape we should find the nearest point on ellipsoidal shape
		if (false == isInBound && CONIC_SECTION::SECTION_1 == eSection)
		{
			if (vTargetPoint.y != 0.f)
			{
				vNearestPoint = Find_Nearest_Point_Constraints(XMVectorGetZ(vQ), XMVectorGetY(vQ), eSection, vTargetPoint);
			}
			else
			{
				vNearestPoint.x = XMVectorGetZ(vQ);
				vNearestPoint.y = 0.f;
			}
		}

		else if (false == isInBound && CONIC_SECTION::SECTION_2 == eSection)
		{
			if (0.f != vTargetPoint.x)
			{
				vNearestPoint = Find_Nearest_Point_Constraints(XMVectorGetX(vQ), XMVectorGetY(vQ), eSection, vTargetPoint);
			}
			else
			{
				vNearestPoint.x = 0.f;
				vNearestPoint.y = XMVectorGetY(vQ);
			}
		}

		else if (false == isInBound && CONIC_SECTION::SECTION_3 == eSection)
		{
			if (vTargetPoint.y != 0.f)
			{
				vNearestPoint = Find_Nearest_Point_Constraints(XMVectorGetX(vQ), XMVectorGetW(vQ), eSection, vTargetPoint);
			}
			else
			{
				vNearestPoint.x = XMVectorGetX(vQ) * -1.f;
				vNearestPoint.y = 0.f;
			}
		}

		else if (false == isInBound && CONIC_SECTION::SECTION_4 == eSection)
		{
			if (0.f != vTargetPoint.x)
			{
				vNearestPoint = Find_Nearest_Point_Constraints(XMVectorGetZ(vQ), XMVectorGetW(vQ), eSection, vTargetPoint);
			}
			else
			{
				vNearestPoint.x = 0.f;
				vNearestPoint.y = XMVectorGetW(vQ) * -1.f;
			}
		}


		// 3D���� ���� ����� �� �� ���� �����̽� ��ǥ ã��
		//	�θ��� �θ�� ���� ������ ���� ���� �Ÿ�
		_float		l_o_before = { sqrtf(powf(vTargetPoint.x,2.f) + powf(vTargetPoint.y, 2.f)) };
		//	�ֱ��������κ��� ������ ���� ���� �Ÿ�
		_float		l_o_nearest_point = { sqrtf(powf(vNearestPoint.x,2.f) + powf(vNearestPoint.y, 2.f)) };
		//	�θ��� �θ�� ���� �ֱ����� �Ÿ�
		_float		l_nearest_before = { sqrtf(powf((vTargetPoint.x - vNearestPoint.x),2.f) + powf((vNearestPoint.y - vTargetPoint.y), 2.f)) };

		//	������ ��� �������� �θ��� �θ������ ���Ϳ� ������ ����������� �ֱ��� �������� ���� ������ ȸ�� ����
		_float		rot_angle = { acosf((powf(l_nearest_before, 2.f) - powf(l_o_before, 2.f) - powf(l_o_nearest_point, 2.f)) / (-2.f * l_o_before * l_o_nearest_point)) };

		// rotating the vector from o to p_before around vector from p_next to o
		//	������ ������ ���������� ���͸� �߽����� ���͸� ������ �������� �θ��� �θ� ��ġ�� ȸ���մϴ�.

		//	a vector from o to p_before
		// ������ �������� �θ��� �θ������ ����
		_vector		unit_vec_o_before = { XMVector3Normalize(p_before - o) };
		_vector		unit_vec_normal_plane = { XMVector3Normalize(p_i - p_next) };

		if (0.f == rot_angle)
		{
			_vector			p_nearest_point_in_global = { o + l_o_nearest_point * unit_vec_o_before };
			XMStoreFloat4(&IkInfo.BoneTranslationsResult[iMyJointIndex], p_nearest_point_in_global);
		}

		else
		{
			//	���� ����� ������ ��ǥ ������ ���� �Ǵ� �����ʿ� �ִ��� Ȯ��
			_float			orient_near_to_target = { vTargetPoint.x * vNearestPoint.y - vNearestPoint.x * vTargetPoint.y };
			_vector			uv_o_nearest_point = {};
			if (0.f < orient_near_to_target * XMVectorGetZ(unit_vec_next_i))
			{
				//	# to find nearest point should rotate the uv(o-target) in C.C.W
				// ���� ����� ������ ã������ C.C.W���� ���� ����(�������߽� - ���)�� ȸ���ؾ� �մϴ�.
				_matrix			RotationMatrix = { XMMatrixRotationAxis(unit_vec_normal_plane, rot_angle) };
				uv_o_nearest_point = XMVector3TransformNormal(unit_vec_o_before, RotationMatrix);
			}

			else
			{
				_matrix			RotationMatrix = { XMMatrixRotationAxis(unit_vec_normal_plane, rot_angle * -1.f) };
				uv_o_nearest_point = XMVector3TransformNormal(unit_vec_o_before, RotationMatrix);
			}

			_vector			p_nearest_point_in_global = { o + uv_o_nearest_point * l_o_nearest_point };
			XMStoreFloat4(&IkInfo.BoneTranslationsResult[iMyJointIndex], p_nearest_point_in_global);
		}
	}
	return;
}

void CModel::Update_Forward_Reaching_IK(IK_INFO& IkInfo)
{
	_uint			iNumIKIndices = { static_cast<_uint>(IkInfo.JointIndices.size()) };

	//	EndEffector�� ���� StartJoint���� ������ ��ȸ => �ڽ� ���� �θ������...
	_vector			vChildBoneResultTranslation = { XMLoadFloat4(&IkInfo.vEndEffectorResultPosition) };
	_vector			vParentAccRotateQuaternion = { XMQuaternionIdentity() };
	for (_int i = iNumIKIndices - 1; i >= 0; --i)
	{
		_bool			isEndEffector = { iNumIKIndices - 1 == i };
		_vector			vMyTranslation = { XMLoadFloat4(&IkInfo.BoneTranslationsResult[i]) };
		_vector			vDirectionToChild = { vChildBoneResultTranslation - vMyTranslation };
		_vector			vResultTranslation;

		//	������ �濡 ���� ������� ����
		if (false == isEndEffector)
		{
			vResultTranslation = vChildBoneResultTranslation - XMVector3Normalize(vDirectionToChild) * IkInfo.TargetDistancesToChild[i];
		}
		else
		{
			vResultTranslation = vChildBoneResultTranslation;
		}



		//	�̵����� ��ġ�� ���� ��ġ��� �ٽ� ���
		XMStoreFloat4(&IkInfo.BoneTranslationsResult[i], vResultTranslation);
		vChildBoneResultTranslation = vResultTranslation;
	}
}

void CModel::Update_Backward_Reaching_IK(IK_INFO& IkInfo)
{
	_uint			iNumIKIndices = { static_cast<_uint>(IkInfo.JointIndices.size()) };

	//	StartJoint�� ���� EndEffector���� ������ ��ȸ => �θ� ���� �ڽļ�����
	_vector			vParentBoneResultTranslation = { XMLoadFloat4(&IkInfo.vTargetJointStartTranslation) };
	for (_int i = 0; i < static_cast<_int>(iNumIKIndices); ++i)
	{
		_bool			isRootJoint = { 0 == i };
		_vector			vMyTranslation = { XMLoadFloat4(&IkInfo.BoneTranslationsResult[i]) };
		_vector			vDirectionToParent = { vParentBoneResultTranslation - vMyTranslation };
		_vector			vResultTranslation;

		if (false == isRootJoint)
		{
			vResultTranslation = vParentBoneResultTranslation - XMVector3Normalize(vDirectionToParent) * IkInfo.TargetDistancesToParrent[i];
		}
		else
		{
			vResultTranslation = vParentBoneResultTranslation;
		}

		//	�̵����� ��ġ�� ���� ��ġ��� �ٽ� ���
		XMStoreFloat4(&IkInfo.BoneTranslationsResult[i], vResultTranslation);
		vParentBoneResultTranslation = vResultTranslation;
	}
}

void CModel::Update_TransformMatrices_BoneChain(IK_INFO& IkInfo)
{
	_uint			iNumIKIndices = { static_cast<_uint>(IkInfo.JointIndices.size()) };
	_matrix         ResultParentCombinedMatrix = { XMLoadFloat4x4(m_Bones[m_Bones[IkInfo.iIKRootBoneIndex]->Get_ParentIndex()]->Get_CombinedTransformationMatrix()) };

	for (_uint i = 0; i < iNumIKIndices - 1; ++i)
	{
		_vector         vMyOriginTranslation = { XMLoadFloat4(&IkInfo.BoneTranslationsOrigin[i]) };
		_vector         vChildOriginTranslation = { XMLoadFloat4(&IkInfo.BoneTranslationsOrigin[i + 1]) };
		_vector         vMyResultTranslation = { XMLoadFloat4(&IkInfo.BoneTranslationsResult[i]) };
		_vector         vChildResultTranslation = { XMLoadFloat4(&IkInfo.BoneTranslationsResult[i + 1]) };

		_matrix			OriginParentCombinedMatrix = { XMLoadFloat4x4(m_Bones[m_Bones[IkInfo.JointIndices[i]]->Get_ParentIndex()]->Get_CombinedTransformationMatrix()) };
		_matrix			OriginParentCombinedMatrixInv = { XMMatrixInverse(nullptr, OriginParentCombinedMatrix) };
		_matrix			ResultParentCombinedMatrixInv = { XMMatrixInverse(nullptr, ResultParentCombinedMatrix) };

		_vector			vMyOriginTranslationInvalidated = { XMVector3TransformCoord(vMyOriginTranslation, OriginParentCombinedMatrixInv) };
		_vector			vChildOriginTranslationInvalidated = { XMVector3TransformCoord(vChildOriginTranslation, OriginParentCombinedMatrixInv) };
		_vector			vMyResultTranslationInvalidated = { XMVector3TransformCoord(vMyResultTranslation, ResultParentCombinedMatrixInv) };
		_vector			vChildResultTranslationInvalidated = { XMVector3TransformCoord(vChildResultTranslation, ResultParentCombinedMatrixInv) };

		_vector			vOriginDirectionToChild = { vChildOriginTranslationInvalidated - vMyOriginTranslationInvalidated };
		_vector			vResultDirectionToChild = { vChildResultTranslationInvalidated - vMyResultTranslationInvalidated };

		_vector			vDeltaQuaternion = { Compute_Quaternion_From_TwoDirection(vOriginDirectionToChild, vResultDirectionToChild) };
		_vector			vBlendedQuaternion = { XMQuaternionSlerp(XMQuaternionIdentity(), XMQuaternionNormalize(vDeltaQuaternion), IkInfo.fBlend) };

		_matrix			TransformMatrix = { m_Bones[IkInfo.JointIndices[i]]->Get_TrasformationMatrix() };
		_matrix			RotationMatrix = { XMMatrixRotationQuaternion(vBlendedQuaternion) };

		_matrix			OriginTransformMatrix = { m_Bones[IkInfo.JointIndices[i]]->Get_TrasformationMatrix() };
		_matrix			ResultTransformMatrix = { OriginTransformMatrix };
		ResultTransformMatrix.r[CTransform::STATE_POSITION] = { XMVectorSet(0.f, 0.f, 0.f, 1.f) };
		ResultTransformMatrix = ResultTransformMatrix * RotationMatrix;
		ResultTransformMatrix.r[CTransform::STATE_POSITION] = vMyOriginTranslationInvalidated;

		m_Bones[IkInfo.JointIndices[i]]->Set_TransformationMatrix(ResultTransformMatrix);
		ResultParentCombinedMatrix = ResultTransformMatrix * ResultParentCombinedMatrix;
	}
}

void CModel::Update_CombinedMatrices_BoneChain(IK_INFO& IkInfo)
{
	for (auto& iBoneIndex : IkInfo.IKIncludedIndices)
	{
		_int		iParrentIndex = { m_Bones[iBoneIndex]->Get_ParentIndex() };
		_matrix		ParrentCombinedMatrix = { XMLoadFloat4x4(m_Bones[iParrentIndex]->Get_CombinedTransformationMatrix()) };
		_matrix		MyTransformMatrix = { m_Bones[iBoneIndex]->Get_TrasformationMatrix() };
		_matrix		MyCombinedMatrix = { MyTransformMatrix * ParrentCombinedMatrix };

		m_Bones[iBoneIndex]->Set_Combined_Matrix(MyCombinedMatrix);
	}
}

CModel::CONIC_SECTION CModel::Find_ConicSection(_float fTheta)
{
	CONIC_SECTION			Section = { CONIC_SECTION::SECTION_END };

	if (fTheta < XM_PI * 0.5f)
	{
		Section = CONIC_SECTION::SECTION_1;
	}

	else if (fTheta >= XM_PI * 0.5f &&
		fTheta < XM_PI)
	{
		Section = CONIC_SECTION::SECTION_2;
	}

	else if (fTheta >= XM_PI &&
		fTheta < XM_PI * 1.5f)
	{
		Section = CONIC_SECTION::SECTION_3;
	}

	else if (fTheta >= XM_PI * 1.5f &&
		fTheta < XM_PI * 0.5f)
	{
		Section = CONIC_SECTION::SECTION_4;
	}

	return Section;
}

_bool CModel::Is_InBound(CONIC_SECTION eSection, _fvector vQ, _float2 vTarget)
{
	_bool		isInBound = { false };


	//	���� ���� ��и� ������ ���
	if (CONIC_SECTION::SECTION_1 == eSection)
	{
		if (1.f >= roundf(powf(vTarget.x, 2.f) / powf(XMVectorGetZ(vQ), 2.f) + powf(vTarget.y, 2.f) / powf(XMVectorGetY(vQ), 2.f)))
			isInBound = true;
	}
	else if (CONIC_SECTION::SECTION_2 == eSection)
	{
		if (1.f >= roundf(powf(vTarget.x, 2.f) / powf(XMVectorGetX(vQ), 2.f) + powf(vTarget.y, 2.f) / powf(XMVectorGetY(vQ), 2.f)))
			isInBound = true;
	}
	else if (CONIC_SECTION::SECTION_3 == eSection)
	{
		if (1.f >= roundf(powf(vTarget.x, 2.f) / powf(XMVectorGetX(vQ), 2.f) + powf(vTarget.y, 2.f) / powf(XMVectorGetW(vQ), 2.f)))
			isInBound = true;
	}
	else if (CONIC_SECTION::SECTION_4 == eSection)
	{
		if (1.f >= roundf(powf(vTarget.x, 2.f) / powf(XMVectorGetZ(vQ), 2.f) + powf(vTarget.y, 2.f) / powf(XMVectorGetW(vQ), 2.f)))
			isInBound = true;
	}

	return isInBound;
}

_float2 CModel::Find_Nearest_Point_Constraints(_float fMajorAxisLength, _float fMinorAxisLength, CONIC_SECTION eSection, _float2 vTargetPosition)
{
	_float2			vInitialPoint = Find_Initial_Point_Constraints(fMajorAxisLength, fMinorAxisLength, eSection, vTargetPosition);
	_float			fThresh = 0.001f;

	_float2			vResultPoint = Find_Next_Point_Constraints(vInitialPoint, fMajorAxisLength, fMinorAxisLength, vTargetPosition);
	while (vResultPoint.x > fThresh || vResultPoint.y > fThresh)
	{
		vResultPoint = Find_Next_Point_Constraints(vResultPoint, fMajorAxisLength, fMinorAxisLength, vTargetPosition);
	}

	return vResultPoint;
}
//	eccentricity( �̽ɷ� ) => Ÿ���� ���� ���� �󸶳� ��׷�������
_float2 CModel::Find_Initial_Point_Constraints(_float fMajorAxisLength, _float fMinorAxisLength, CONIC_SECTION eSection, _float2 vTargetPosition)
{

	_float2			vK1 = {
		vTargetPosition.x * fMajorAxisLength * fMinorAxisLength / sqrtf(powf(fMinorAxisLength, 2.f) * powf(vTargetPosition.x, 2.f) + powf(fMajorAxisLength, 2.f) * powf(vTargetPosition.y, 2.f)),
		vTargetPosition.y * fMajorAxisLength * fMinorAxisLength / sqrtf(powf(fMinorAxisLength, 2.f) * powf(vTargetPosition.x, 2.f) + powf(fMajorAxisLength, 2.f) * powf(vTargetPosition.y, 2.f))
	};

	_int			iSign_Y = { (eSection == CONIC_SECTION::SECTION_1 || eSection == CONIC_SECTION::SECTION_2) ? 1 : -1 };
	_int			iSign_X = { (eSection == CONIC_SECTION::SECTION_1 || eSection == CONIC_SECTION::SECTION_4) ? 1 : -1 };

	_float2			vK2 = {};
	if (fabsf(vTargetPosition.x) < fMajorAxisLength)
	{
		vK2 = {
			vTargetPosition.x,
			iSign_Y * (fMinorAxisLength / fMajorAxisLength) * sqrtf(powf(fMajorAxisLength, 2.f) - powf(vTargetPosition.x, 2.f))
		};
	}
	else
	{
		vK2 = {
			fMajorAxisLength * iSign_X * fMajorAxisLength,
			0.f
		};
	}

	_float2		vInitialPoint = {
		0.5f * (vK1.x + vK2.x),
		0.5f * (vK1.y + vK2.y)
	};

	return vInitialPoint;
}

_float2 CModel::Find_Next_Point_Constraints(_float2 vCurrentPoint, _float fMajorAxisLength, _float fMinorAxisLength, _float2 vTargetPosition)
{
	_float2			vDelta = { Compute_Delta_Constratins(vCurrentPoint, fMajorAxisLength, fMinorAxisLength, vTargetPosition) };
	_float2			vResultPoint = { vCurrentPoint };

	vResultPoint.x += vDelta.x;
	vResultPoint.y += vDelta.y;

	return vResultPoint;
}

_float2 CModel::Compute_Delta_Constratins(_float2 vCurrentPoint, _float fMajorAxisLength, _float fMinorAxisLength, _float2 vTargetPosition)
{
	_vector		vF = XMVectorSet(
		(powf(fMajorAxisLength, 2.f) * powf(vCurrentPoint.y, 2.f) + powf(fMinorAxisLength, 2.f) * powf(vCurrentPoint.x, 2.f) - powf(fMajorAxisLength, 2.f) * powf(fMinorAxisLength, 2.f)) * 0.5f,
		powf(fMinorAxisLength, 2.f) * vCurrentPoint.x * (vTargetPosition.y - vCurrentPoint.y) - powf(fMajorAxisLength, 2.f) * vCurrentPoint.y * (vTargetPosition.x - vCurrentPoint.x),
		0, 0);

	_matrix		QMatrix = XMLoadFloat4x4(&Compute_QMatrix(vCurrentPoint, fMajorAxisLength, fMinorAxisLength, vTargetPosition));
	_matrix		QMatrixInv = XMMatrixInverse(nullptr, QMatrix);

	_vector		vDelta = XMVector2Transform(vF, QMatrixInv);
	_float2		vDeltaFloat2 = { XMVectorGetX(vDelta), XMVectorGetY(vDelta) };

	return vDeltaFloat2;
}

_float4x4 CModel::Compute_QMatrix(_float2 vCurrentPoint, _float fMajorAxisLength, _float fMinorAxisLength, _float2 vTargetPosition)
{
	_float4x4		QFloat4x4 = {};

	QFloat4x4.m[0][0] = powf(fMinorAxisLength, 2.f) * vCurrentPoint.x;
	QFloat4x4.m[0][1] = (powf(fMajorAxisLength, 2.f) - powf(fMinorAxisLength, 2.f)) * vCurrentPoint.y + powf(fMinorAxisLength, 2.f) * vTargetPosition.y;
	QFloat4x4.m[1][0] = powf(fMajorAxisLength, 2.f) * vCurrentPoint.y;
	QFloat4x4.m[1][1] = (powf(fMajorAxisLength, 2.f) - powf(fMinorAxisLength, 2.f)) * vCurrentPoint.x - powf(fMajorAxisLength, 2.f) * vTargetPosition.x;

	return QFloat4x4;
}

_vector CModel::Compute_Quaternion_From_TwoDirection(_fvector vSrcDirection, _fvector vDstDirection)
{
	_vector			vResultQuaternion = { XMQuaternionIdentity() };

	_vector			vNormalizeSrcDirection = XMVector3Normalize(vSrcDirection);
	_vector			vNormalizeDstDirection = XMVector3Normalize(vDstDirection);

	_float			fDot = { XMVectorGetX(XMVector3Dot(vNormalizeSrcDirection, vNormalizeDstDirection)) };

	if (fDot < 0.9999f && fDot > -0.9999f)
	{
		_vector			vRotateAxis = { XMVector3Cross(vNormalizeSrcDirection, vNormalizeDstDirection) };

		_float			fClampDot = { std::clamp(fDot, -1.f, 1.f) };
		_float			fAngleRadian = { acosf(fClampDot) };

		vResultQuaternion = XMQuaternionRotationAxis(vRotateAxis, fAngleRadian);
	}

	return vResultQuaternion;
}

void CModel::Add_Bone_Layer(const wstring& strLayerTag, list<_uint> BoneIndices)
{
	if (true == BoneIndices.empty())
		return;

	map<wstring, CBone_Layer*>::iterator		iter = { m_BoneLayers.find(strLayerTag) };
	if (iter != m_BoneLayers.end())
		return;

	set<_uint>		BoneIndicesSet;
	for (auto& iIndex : BoneIndices)
	{
		BoneIndicesSet.emplace(iIndex);
	}

	CBone_Layer* pBoneLayer = { CBone_Layer::Create(BoneIndicesSet) };
	if (nullptr == pBoneLayer)
		return;

	m_BoneLayers.emplace(strLayerTag, pBoneLayer);
}

void CModel::Add_Bone_Layer_All_Bone(const wstring& strLayerTag)
{
	list<_uint>			BoneIndices;
	for (_uint i = 0; i < static_cast<_uint>(m_Bones.size()); ++i)
		BoneIndices.emplace_back(i);

	Add_Bone_Layer(strLayerTag, BoneIndices);
}

void CModel::Delete_Bone_Layer(const wstring& strLayerTag)
{
	map<wstring, CBone_Layer*>::iterator		iter = { m_BoneLayers.find(strLayerTag) };
	if (iter == m_BoneLayers.end())
	{
		Safe_Release(iter->second);
		m_BoneLayers.erase(iter);
	}
}

void CModel::Add_Additional_Transformation_World(string strBoneTag, _fmatrix AdditionalTransformMatrix)
{
	_int			iBoneIndex = { Find_BoneIndex(strBoneTag) };
	if (-1 == iBoneIndex)
		return;

	if (0 == m_AdditionalForces.size())
		m_AdditionalForces.resize(m_Bones.size());

	_float4x4			AdditionalTransformationFloat4x4;
	XMStoreFloat4x4(&AdditionalTransformationFloat4x4, AdditionalTransformMatrix);

	m_AdditionalForces[iBoneIndex].push_back(AdditionalTransformationFloat4x4);
}

void CModel::Apply_AdditionalForces(CTransform* pTransform)
{
	m_AdditionalForces.resize(m_Bones.size());

	//	_matrix		WorldMatrixInv = { pTransform->Get_WorldMatrix_Inverse() };
	_matrix		WorldMatrixInv = { XMMatrixIdentity() };
	_uint		iNumBones = { static_cast<_uint>(m_Bones.size()) };
	for (_uint iBoneIndex = 0; iBoneIndex < iNumBones; ++iBoneIndex)
	{
		_matrix			CombinedMatrix = { XMLoadFloat4x4(m_Bones[iBoneIndex]->Get_CombinedTransformationMatrix()) };
		_vector			vCurrentScale, vCurrentQuaternion, vCurrentTranslation;
		XMMatrixDecompose(&vCurrentScale, &vCurrentQuaternion, &vCurrentTranslation, CombinedMatrix);

		_vector			vResultQuaternion = { vCurrentQuaternion };
		_vector			vResultTranslation = { vCurrentTranslation };

		if (true == m_AdditionalForces.empty())
			continue;

		for (_float4x4 InputMatrix : m_AdditionalForces[iBoneIndex])
		{
			_matrix		AdditionalMatrix = { XMLoadFloat4x4(&InputMatrix) };
			_matrix		AdditionalMatrixLocal = { AdditionalMatrix * WorldMatrixInv };
			_vector		vAdditionalScale, vAdditionalQuaternion, vAdditionalTranslation;

			XMMatrixDecompose(&vAdditionalScale, &vAdditionalQuaternion, &vAdditionalTranslation, AdditionalMatrixLocal);

			//	�ϴ� �������� ���������ʰ� �����Ѵ�.
			vResultQuaternion = { XMQuaternionMultiply(XMQuaternionNormalize(vCurrentQuaternion), XMQuaternionNormalize(vAdditionalQuaternion)) };
			vResultTranslation = { XMVectorSetW(vCurrentTranslation + vAdditionalTranslation, 1.f) };
		}

		_matrix			ResultMatrix = { XMMatrixAffineTransformation(vCurrentScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vResultQuaternion, vResultTranslation) };
		m_Bones[iBoneIndex]->Set_Combined_Matrix(ResultMatrix);
	}

	Clear_AdditionalForces();
}

void CModel::Apply_AdditionalForce(_uint iBoneIndex)
{
	m_AdditionalForces.resize(m_Bones.size());

	_matrix			WorldMatrixInv = { XMMatrixIdentity() };
	_matrix			CombinedMatrix = { XMLoadFloat4x4(m_Bones[iBoneIndex]->Get_CombinedTransformationMatrix()) };
	_vector			vCurrentScale, vCurrentQuaternion, vCurrentTranslation;
	XMMatrixDecompose(&vCurrentScale, &vCurrentQuaternion, &vCurrentTranslation, CombinedMatrix);

	_vector			vResultQuaternion = { vCurrentQuaternion };
	_vector			vResultTranslation = { vCurrentTranslation };

	if (true == m_AdditionalForces.empty())
		return;

	for (_float4x4 InputMatrix : m_AdditionalForces[iBoneIndex])
	{
		_matrix		AdditionalMatrix = { XMLoadFloat4x4(&InputMatrix) };
		_matrix		AdditionalMatrixLocal = { AdditionalMatrix * WorldMatrixInv };
		_vector		vAdditionalScale, vAdditionalQuaternion, vAdditionalTranslation;

		XMMatrixDecompose(&vAdditionalScale, &vAdditionalQuaternion, &vAdditionalTranslation, AdditionalMatrixLocal);

		//	�ϴ� �������� ���������ʰ� �����Ѵ�.
		vResultQuaternion = { XMQuaternionMultiply(XMQuaternionNormalize(vCurrentQuaternion), XMQuaternionNormalize(vAdditionalQuaternion)) };
		vResultTranslation = { XMVectorSetW(vCurrentTranslation + vAdditionalTranslation, 1.f) };
	}

	_matrix			ResultMatrix = { XMMatrixAffineTransformation(vCurrentScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vResultQuaternion, vResultTranslation) };
	m_Bones[iBoneIndex]->Set_Combined_Matrix(ResultMatrix);
}

void CModel::Clear_AdditionalForces()
{
	m_AdditionalForces.clear();
	m_AdditionalForces.resize(m_Bones.size());
}

_int CModel::Find_RootBoneIndex()
{
	_int iRootBoneIndex = { -1 };

	for (auto& pBone : m_Bones)
	{
		++iRootBoneIndex;

		if (true == pBone->Is_RootBone())
		{
			return iRootBoneIndex;
		}
	}

	return -1;
}

vector<_float4x4> CModel::Initialize_ResultMatrices(const set<_uint> IncludedBoneIndices)
{
	vector<_float4x4>		ResultMatrices;
	ResultMatrices.resize(m_Bones.size());

	//	���� ���� �� ��ȯ ��ĵ��� ���� �����Ѵ�.
	for (_uint iBoneIndex = 0; iBoneIndex < m_Bones.size(); ++iBoneIndex)
	{
		XMStoreFloat4x4(&ResultMatrices[iBoneIndex], m_Bones[iBoneIndex]->Get_TrasformationMatrix());
	}

	//	�̹��� �ִϸ��̼��� ������ ���� ������ ���� �׵���ķ� ���ʱ�ȭ�Ѵ�.
	for (_uint iBoneIndex : IncludedBoneIndices)
	{
		XMStoreFloat4x4(&ResultMatrices[iBoneIndex], XMMatrixIdentity());
	}

	return ResultMatrices;
}

_float CModel::Compute_Current_TotalWeight(_uint iBoneIndex)
{
	_float		fTotalWeight = { 0.f };
	for (auto& AnimInfo : m_PlayingAnimInfos)
	{
		if (-1 == AnimInfo.iAnimIndex)
			continue;

		if (true == m_Animations[AnimInfo.iAnimIndex]->isFinished())
			continue;


		if (false == m_BoneLayers[AnimInfo.strBoneLayerTag]->Is_Included(iBoneIndex))
			continue;

		fTotalWeight += AnimInfo.fWeight;
	}

	return fTotalWeight;
}

_float4x4 CModel::Compute_BlendTransformation_Additional(_fmatrix SrcMatrix, _fmatrix DstMatrix, _float fAdditionalWeight)
{
	_vector			vSrcScale, vSrcQuaternion, vSrcTranslation;
	XMMatrixDecompose(&vSrcScale, &vSrcQuaternion, &vSrcTranslation, SrcMatrix);

	_vector			vDstScale, vDstQuaternion, vDstTranslation;
	XMMatrixDecompose(&vDstScale, &vDstQuaternion, &vDstTranslation, DstMatrix);

	vDstQuaternion = XMQuaternionSlerp(XMQuaternionIdentity(), vDstQuaternion, fAdditionalWeight);
	vDstScale *= fAdditionalWeight;
	vDstTranslation *= fAdditionalWeight;

	_vector			vResultScale = { vSrcScale + vDstScale };
	_vector			vResultQuaternion = { XMQuaternionMultiply(XMQuaternionNormalize(vSrcQuaternion), XMQuaternionNormalize(vDstQuaternion)) };
	_vector			vResultTranslation = { XMVectorSetW(XMVectorSetW(vSrcTranslation, 0.f) + XMVectorSetW(vDstTranslation,0.f), 1.f) };

	_matrix			ResultMatrix = { XMMatrixAffineTransformation(vResultScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vResultQuaternion, vResultTranslation) };
	_float4x4		ResultFloat4x4;

	XMStoreFloat4x4(&ResultFloat4x4, ResultMatrix);

	return ResultFloat4x4;
}

_float4x4 CModel::GetBoneTransform(string strBoneTag)
{
	_int			iBoneIndex = { Find_BoneIndex(strBoneTag) };
	iBoneIndex = 86;
	if (-1 == iBoneIndex)
		return _float4x4();

	auto CombinedFloat4x4 = m_Bones[iBoneIndex]->Get_CombinedTransformationMatrix();

	return *CombinedFloat4x4;
}

_float4x4 CModel::GetBoneTransform(_int Index)
{
	auto CombinedFloat4x4 = m_Bones[Index]->Get_CombinedTransformationMatrix();

	return *CombinedFloat4x4;
}

void CModel::Apply_RootMotion_Rotation(CTransform* pTransform, _fvector vQuaternion, _float4* pTranslation)
{
	_vector			vPreQuaternion = { XMQuaternionIdentity() };
	_vector			vIdentityQuaternion = { XMQuaternionIdentity() };
	_uint			iRootIndex = { static_cast<_uint>(Find_RootBoneIndex()) };
	_vector			vTotalQuaternion = { vQuaternion };

	for (auto& AnimInfo : m_PlayingAnimInfos)
	{
		if (-1 == AnimInfo.iAnimIndex || true == m_Animations[AnimInfo.iAnimIndex]->isFinished())
			continue;

		if (false == m_BoneLayers[AnimInfo.strBoneLayerTag]->Is_Included(iRootIndex))
			continue;

		_vector			vCurrentQuaternion = { XMLoadFloat4(&AnimInfo.vPreQuaternion) };
		vCurrentQuaternion = { XMQuaternionSlerp(vIdentityQuaternion, vCurrentQuaternion, AnimInfo.fWeight) };
		vPreQuaternion = XMQuaternionMultiply(XMQuaternionNormalize(vPreQuaternion), XMQuaternionNormalize(vCurrentQuaternion));

		AnimInfo.vPreQuaternion = AnimInfo.LastKeyFrames[iRootIndex].vRotation;
	}

	_vector			vCurrentQuaternion = { vQuaternion };

	// ���� ���ʹϾ��� �����ʹϾ� ���ϱ�
	_vector			vPreQuaternionInv = { XMQuaternionInverse(vPreQuaternion) };

	// ���� ���ʹϾ��� �����ʹϾ�� �������ʹϾ��� �� => �����ʹϾ�
	_vector			vQuaternionDifference = { XMQuaternionNormalize(XMQuaternionMultiply(vPreQuaternionInv, vCurrentQuaternion)) };

	_matrix			RotationMatrix = { XMMatrixRotationQuaternion(vQuaternionDifference) };
	_matrix			WorldMatrix = { pTransform->Get_WorldMatrix() };
	_vector			vPosition = { WorldMatrix.r[CTransform::STATE_POSITION] };

	WorldMatrix.r[CTransform::STATE_POSITION] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	_matrix			ResultMatrix = { XMMatrixMultiply(RotationMatrix, WorldMatrix) };

	ResultMatrix.r[CTransform::STATE_POSITION] = vPosition;

	pTransform->Set_WorldMatrix(ResultMatrix);
}

void CModel::Apply_RootMotion_Translation(CTransform* pTransform, _fvector vTranslation, _float3* pMovedDirection, _bool isActiveRotation, _fvector vQuaternion)
{
	_vector			vPreTranslation = { XMVectorZero() };
	_uint			iRootIndex = { static_cast<_uint>(Find_RootBoneIndex()) };

	for (auto& AnimInfo : m_PlayingAnimInfos)
	{
		if (-1 == AnimInfo.iAnimIndex || true == m_Animations[AnimInfo.iAnimIndex]->isFinished())
			continue;

		if (false == m_BoneLayers[AnimInfo.strBoneLayerTag]->Is_Included(iRootIndex))
			continue;

		_vector			vTranslation = { XMLoadFloat3(&AnimInfo.vPreTranslationLocal) };
		vTranslation = { vTranslation * AnimInfo.fWeight };
		vPreTranslation = vPreTranslation + vTranslation;

		AnimInfo.vPreTranslationLocal = AnimInfo.LastKeyFrames[iRootIndex].vTranslation;
	}

	_matrix			WorldMatrix = { pTransform->Get_WorldMatrix() };
	_vector			vTranslationLocal = { vTranslation };

	_vector			vCurrentMoveDirectionLocal = { vTranslationLocal - XMVectorSetW(vPreTranslation, 1.f) };

	if (false == m_isRootMotion_XZ)
	{
		vCurrentMoveDirectionLocal = XMVectorSetX(vCurrentMoveDirectionLocal, 0.f);
		vCurrentMoveDirectionLocal = XMVectorSetZ(vCurrentMoveDirectionLocal, 0.f);
	}

	if (false == m_isRootMotion_Y)
	{
		vCurrentMoveDirectionLocal = XMVectorSetY(vCurrentMoveDirectionLocal, 0.f);
	}

	if (true == isActiveRotation)
	{
		_vector			vCurrentQuaternion = { vQuaternion };
		_vector			vCurrentQuaternionInv = { XMQuaternionNormalize(XMQuaternionInverse(vCurrentQuaternion)) };
		_matrix			RoatationMatrix = { XMMatrixRotationQuaternion(vCurrentQuaternionInv) };

		vCurrentMoveDirectionLocal = XMVector3TransformNormal(vCurrentMoveDirectionLocal, RoatationMatrix);
	}

	_vector			vCurrentMoveDirectionWorld;

	vCurrentMoveDirectionWorld = { XMVector3TransformNormal(vCurrentMoveDirectionLocal, XMLoadFloat4x4(&m_TransformationMatrix)) };
	vCurrentMoveDirectionWorld = { XMVector3TransformNormal(vCurrentMoveDirectionWorld, WorldMatrix) };

	XMStoreFloat3(pMovedDirection, vCurrentMoveDirectionWorld);
}

void CModel::Apply_Translation_Inverse_Rotation(_fvector vTranslation)
{
	if (false == m_isRootMotion_Rotation)
		return;

	if (true == m_isRootMotion_XZ || true == m_isRootMotion_Y)
		return;
}

void CModel::Set_CombinedMatrix(string strBoneTag, _fmatrix CombinedMatrix)
{
	_int			iBoneIndex = { Find_BoneIndex(strBoneTag) };
	if (-1 == iBoneIndex)
		return;

	m_Bones[iBoneIndex]->Set_Combined_Matrix(CombinedMatrix);
}

void CModel::Set_Parent_CombinedMatrix_Ptr(string strBoneTag, _float4x4* pParentMatrix)
{
	_int			iBoneIndex = { Find_BoneIndex(strBoneTag) };
	if (-1 == iBoneIndex)
		return;

	m_Bones[iBoneIndex]->Set_Parent_CombinedMatrix_Ptr(pParentMatrix);
}

void CModel::Set_Surbodinate(string strBoneTag, _bool isSurbodinate)
{
	_int			iBoneIndex = { Find_BoneIndex(strBoneTag) };
	if (-1 == iBoneIndex)
		return;

	m_Bones[iBoneIndex]->Set_Surbodinate(isSurbodinate);
}

vector<string> CModel::Get_BoneNames()
{
	vector<string> BoneNames;

	for (auto Bone : m_Bones)
	{
		BoneNames.push_back(Bone->Get_Name());
	}

	return BoneNames;
}

vector<_float4> CModel::Get_Translations()
{
	vector<_float4> Translations;

	for (auto Bone : m_Bones)
	{
		Translations.push_back(Bone->Get_Translation());
	}

	return Translations;
}

set<string> CModel::Get_MeshTags()
{
	set<string>		MeshTags;
	for (auto& pMesh : m_Meshes)
	{
		const string		strMeshTag = pMesh->Get_MeshName();
		MeshTags.emplace(strMeshTag);
	}

	return MeshTags;
}

list<_uint> CModel::Get_MeshIndices(const string& strMeshTag)
{
	list<_uint>			MeshIndices;
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		if (m_Meshes[i]->Get_MeshName() == strMeshTag)
			MeshIndices.push_back(i);
	}

	return MeshIndices;
}

_float4 CModel::Invalidate_RootNode(const string& strRoot)
{
	for (auto& Bone : m_Bones)
	{
		if (string(Bone->Get_Name()) == strRoot)
		{
			return Bone->Get_Translation();
		}
	}

	return _float4{ 0.f, 0.f, 0.f, 0.f };
}

_uint CModel::Get_CurrentMaxKeyFrameIndex(_uint iPlayingIndex)
{
	if (static_cast<_uint>(m_PlayingAnimInfos.size()) <= iPlayingIndex)
	{
		return 0;
	}

	const vector<_uint>			KeyFrameIndices = { m_Animations[m_PlayingAnimInfos[iPlayingIndex].iAnimIndex]->Get_CurrentKeyFrameIndices() };

	_uint		iMaxIndex = { 0 };
	for (_uint iIndex : KeyFrameIndices)
	{
		if (iIndex > iMaxIndex)
			iMaxIndex = iIndex;
	}

	return iMaxIndex;
}

const vector<_uint>& CModel::Get_CurrentKeyFrameIndices(_uint iPlayingIndex)
{
	return m_Animations[m_PlayingAnimInfos[iPlayingIndex].iAnimIndex]->Get_CurrentKeyFrameIndices();
}

CBone* CModel::Get_BonePtr(const _char* pBoneName) const
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			return pBone->Compare_Name(pBoneName);
		});

	return *iter;
}

_bool CModel::isFinished(_uint iPlayingIndex)
{
	if (iPlayingIndex >= static_cast<_uint>(m_PlayingAnimInfos.size()))
		return false;

	_uint			iAnimIndex = { static_cast<_uint>(m_PlayingAnimInfos[iPlayingIndex].iAnimIndex) };
	return m_Animations[iAnimIndex]->isFinished();
}

void CModel::Get_Child_BoneIndices(string strTargetParentsBoneTag, list<_uint>& ChildBoneIndices)
{
	for (auto& pBone : m_Bones)
	{
		_int		iParentsIndex = { pBone->Get_ParentIndex() };
		if (-1 == iParentsIndex)
			continue;

		string		strMyParentsBoneTag = { m_Bones[iParentsIndex]->Get_Name() };

		if (strMyParentsBoneTag == strTargetParentsBoneTag)
		{
			string		strBoneTag = { pBone->Get_Name() };
			_int		iBoneIndex = { Find_BoneIndex(strBoneTag) };
			if (-1 == iBoneIndex)
				continue;

			ChildBoneIndices.push_back(iBoneIndex);

			Get_Child_BoneIndices(strBoneTag, ChildBoneIndices);
		}
	}
}

const _float4x4* CModel::Get_CombinedMatrix(const string& strBoneTag)
{
	_int		iBoneIndex = { Find_BoneIndex(strBoneTag) };
	if (-1 == iBoneIndex)
		return nullptr;

	return m_Bones[iBoneIndex]->Get_CombinedTransformationMatrix();
}

void CModel::Set_KeyFrameIndex(_uint iPlayingIndex, _uint iKeyFrameIndex)
{
	m_Animations[m_PlayingAnimInfos[iPlayingIndex].iAnimIndex]->Set_KeyFrameIndex(iKeyFrameIndex);
}

_float CModel::Get_TrackPosition(_uint iPlayingIndex)
{
	return m_Animations[m_PlayingAnimInfos[iPlayingIndex].iAnimIndex]->Get_TrackPosition();
}

_float CModel::Get_Duration(_uint iPlayingIndex, _int iAnimIndex)
{
	if (-1 == iAnimIndex)
		iAnimIndex = m_PlayingAnimInfos[iPlayingIndex].iAnimIndex;

	return m_Animations[iAnimIndex]->Get_Duration();
}

void CModel::Set_TrackPosition(_uint iPlayingIndex, _float fTrackPosition)
{
	m_Animations[m_PlayingAnimInfos[iPlayingIndex].iAnimIndex]->Set_TrackPosition(fTrackPosition);
}

HRESULT CModel::Initialize_Prototype(MODEL_TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix)
{
#pragma region (�����ε�)
	//	m_eModelType = eType;
	//
	//	_uint		iOption = { aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded };
	//
	//	iOption = m_eModelType == TYPE_NONANIM ? iOption | aiProcess_PreTransformVertices : iOption | aiProcess_LimitBoneWeights;
	//
	//#pragma region Log
	//	//m_Importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_CAMERAS);
	//	//// �α� ����
	//	//Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
	//	//// �α� �޽����� ��� ���� ReadFile ���� ȣ��
	//
	//	///* ���Ϥ��� ������ �о aiScene�ȿ� ��� �����͸� ����ְԵȴ�. */
	//	//m_pAIScene = m_Importer.ReadFile(strModelFilePath.c_str(), iOption);
	//	//if (nullptr == m_pAIScene)
	//	//{
	//	//	MessageBoxA(NULL, m_Importer.GetErrorString(), "Error", MB_OK);
	//
	//	//	return E_FAIL;
	//	//}
	//#pragma endregion
	//
	//	/* ���Ϥ��� ������ �о aiScene�ȿ� ��� �����͸� ����ְԵȴ�. */
	//	m_pAIScene = m_Importer.ReadFile(strModelFilePath.c_str(), iOption);
	//	if (nullptr == m_pAIScene)
	//		return E_FAIL;
	//
	//	XMStoreFloat4x4(&m_TransformationMatrix, TransformMatrix);
	//
	//	/* ���� ������ ���������ؼ� ���� ����ϱ� ���� �����Ѵ�.  */
	//
	//	/* ���������� ���� �̸����� ���� m_Bones�� �Ҵ�� �ε����� ��ȣ�� �����ϰ� �޽õ� ��ü�� ������ Ž�������� �����ش�. */
	//	/* ReadyBones���� ���� ü��� */
	//	/* ReadyMeshes, ReadyAnimations���� ���� �̿��Ѵ�. */
	//	map<string, _uint> BoneIndices;
	//
	//	/* ���� ����� �Ʒ��� ���ϵ� ������ Ÿ�� ��������� �����ϴ� ���� Ž������ ��Ʈ ��带 �Ѱ��ָ�ȴ� .*/
	//	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, BoneIndices)))
	//		return E_FAIL;
	//
	//	/* ���� �����ϴ� �޽õ��� �����Ѵ�. */
	//	/* �� = �޽� + �޽� + ... */
	//	if (FAILED(Ready_Meshes(BoneIndices)))
	//		return E_FAIL;
	//
	//	if (FAILED(Ready_Materials(strModelFilePath.c_str())))
	//		return E_FAIL;
	//
	//	if (FAILED(Ready_Animations(BoneIndices)))
	//		return E_FAIL;

	//	return S_OK;

#pragma endregion
	filesystem::path FullPath(strModelFilePath);

	string strParentsPath = FullPath.parent_path().string();
	string strFileName = FullPath.stem().string();

	//	 ���ϰ�ο� ���� �����̸��� Ȯ���ڸ� �ٸ��� ���ο� ��� ����
	string strNewPath = strParentsPath + "/" + strFileName + ".bin";

	filesystem::path CheckPath(strNewPath);

	if (true == filesystem::exists(CheckPath))
	{
		if (FAILED(Initialize_Prototype(strModelFilePath, TransformMatrix)))
			return E_FAIL;
	}

	else
	{
		if (FAILED(CModel_Extractor::Extract_FBX(eType, strModelFilePath)))
			return E_FAIL;

		if (FAILED(Initialize_Prototype(strModelFilePath, TransformMatrix)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::Initialize_Prototype(const string& strModelFilePath, _fmatrix TransformMatrix)
{
	filesystem::path FullPath(strModelFilePath);

	string strParentsPath = FullPath.parent_path().string();
	string strFileName = FullPath.stem().string();

	//	 ���ϰ�ο� ���� �����̸��� Ȯ���ڸ� �ٸ��� ���ο� ��� ����
	string strNewPath = strParentsPath + "/" + strFileName + ".bin";

	//	���̳ʸ��� �����͸� �ۼ��ϱ����ؼ� ���̳ʸ� �÷��׸� �����Ͽ���.
	ifstream ifs(strNewPath.c_str(), ios::binary);

	//	ù ��° ���ڴ� �����͸� ������ ������ �ּҸ� ����Ű�� �������̰�, �� ��° ���ڴ� �о�� ����Ʈ ���Դϴ�.
	if (true == ifs.fail())
	{
		MSG_BOX(TEXT("Failed To OpenFile"));

		return E_FAIL;
	}

	ifs.read(reinterpret_cast<_char*>(&m_eModelType), sizeof(MODEL_TYPE));

	XMStoreFloat4x4(&m_TransformationMatrix, TransformMatrix);

	if (FAILED(Ready_Bones(ifs)))
		return E_FAIL;

	if (FAILED(Ready_Meshes(ifs)))
		return E_FAIL;

	if (FAILED(Ready_Materials(ifs)))
		return E_FAIL;

	if (FAILED(Ready_Animations(ifs)))
		return E_FAIL;

	ifs.close();

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	m_fTotalLinearTime = ANIM_DEFAULT_LINEARTIME;

	return S_OK;
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	ZeroMemory(m_MeshBoneMatrices, sizeof(_float4x4) * 512);

	m_Meshes[iMeshIndex]->Stock_Matrices(m_Bones, m_MeshBoneMatrices);

	return pShader->Bind_Matrices(pConstantName, m_MeshBoneMatrices, 512);
}

HRESULT CModel::Bind_Pre_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	ZeroMemory(m_MeshBoneMatrices, sizeof(_float4x4) * 512);

	m_Meshes[iMeshIndex]->Stock_Pre_Matrices(m_Bones, m_MeshBoneMatrices);

	return pShader->Bind_Matrices(pConstantName, m_MeshBoneMatrices, 512);
}

HRESULT CModel::Bind_ShaderResource_Texture(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint		iMeshMaterialIndex = { m_Meshes[iMeshIndex]->Get_MaterialIndex() };

	if (iMeshMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	CTexture* pTexture = m_Materials[iMeshMaterialIndex].MaterialTextures[eTextureType];


	if (nullptr != pTexture)
		return pTexture->Bind_ShaderResource(pShader, pConstantName);
	else {
		pShader->Bind_Texture(pConstantName, nullptr);
	}

	return E_FAIL;
}

HRESULT CModel::Bind_ShaderResource_MaterialDesc(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	if (FAILED(pShader->Bind_RawValue(pConstantName, &m_Meshes[iMeshIndex]->Get_MaterialDesc(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Play_Animations(_float fTimeDelta)
{
	for (auto& pBone : m_Bones)
	{
		_matrix			PreCombinedMatrix = { XMLoadFloat4x4(pBone->Get_CombinedTransformationMatrix()) };

		pBone->Set_Pre_CombinedMatrix(PreCombinedMatrix);
	}

	_uint		iPlayingIndex = { 0 };
	for (auto& AnimInfo : m_PlayingAnimInfos)
	{
		if (-1 == AnimInfo.iAnimIndex)
			continue;

		Update_LinearInterpolation(fTimeDelta, iPlayingIndex);

		//	ä�ο� �ִ� Ʈ�������̼� ��Ʈ������ ���� �ִϸ��̼��� Ű�����ӿ� �°� �缳���Ѵ�.

		/* ���� �ִϸ��̼ǿ� �´� ���� ����(m_TransformationMatrix)�� �������ش�. */
		_bool		isFirstTick = { false };
		m_Animations[AnimInfo.iAnimIndex]->Invalidate_TransformationMatrix(fTimeDelta, m_Bones, AnimInfo.isLoop, &isFirstTick);

		if (true == AnimInfo.isLinearInterpolation)
		{
			m_Animations[AnimInfo.iAnimIndex]->Invalidate_TransformationMatrix_LinearInterpolation(AnimInfo.fAccLinearInterpolation, m_fTotalLinearTime, m_Bones, AnimInfo.LastKeyFrames);
		}

		//	��� ä�ξ�����Ʈ�� ������ �� ���� �Ĺ��ε� ����� �����Ѵ�.
		for (auto& pBone : m_Bones)
			pBone->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformationMatrix));

		if (false == AnimInfo.isLinearInterpolation)
		{
			Update_LastKeyFrames_Bones(AnimInfo.LastKeyFrames);
		}

		++iPlayingIndex;
	}

	return S_OK;
}

HRESULT CModel::Play_Animations_RootMotion(CTransform* pTransform, _float fTimeDelta, _float3* pMovedDirection)
{
	for (auto& pBone : m_Bones)
	{
		_matrix			PreCombinedMatrix = { XMLoadFloat4x4(pBone->Get_CombinedTransformationMatrix()) };

		pBone->Set_Pre_CombinedMatrix(PreCombinedMatrix);
	}
	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////
	////////////////////////////MOTION_BLEND////////////////////////////
	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////

	//	����� ��� �ִϸ��̼��� �ϴ� ����ϰ� �� Ű�������� �����Ͽ� �����´�.
	vector<vector<_float4x4>>		TransformationMatricesLayer;
	set<_uint>						IncludedBoneIndices;
	XMStoreFloat3(pMovedDirection, XMVectorZero());

	//	�ִϸ��̼ǵ��� ����Ѵ�.
	//	���Ե� ������ ���, �� �ִϸ��̼ǿ����� Ʈ������ ��ĵ��� ���̾� ������ �����Ѵ�.
	for (_uint i = 0; i < static_cast<_uint>(m_PlayingAnimInfos.size()); ++i)
	{
		vector<_float4x4>			TransformationMatrices = { Apply_Animation(fTimeDelta, IncludedBoneIndices, i) };
		//	�ִϸ��̼��� ����ǰų� (�������ƴѵ� ����), ����ġ�� ���ų�, �ִϸ��̼� �ε����� ��ϵ���������� �� ����� ��ȯ�޴´�.
		TransformationMatricesLayer.push_back(TransformationMatrices);
	}

	//	��� ��ĵ��� ���� Ʈ������ ��Ŀ� �����Ѵ�.
	Apply_Bone_TransformMatrices(TransformationMatricesLayer, IncludedBoneIndices);

	//	�Ĺ��ε� ��� ���� ��, ��Ʈ��ǿ� ���� ���е��� ������ ����
	Apply_Bone_CombinedMatrices(pTransform, pMovedDirection);

	for (auto& Pair : m_IKInfos)
	{
		Apply_IK(pTransform, Pair.second);
	}

	//	Apply_AdditionalForces(pTransform);
	Clear_AdditionalForces();

	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

vector<_float4x4> CModel::Apply_Animation(_float fTimeDelta, set<_uint>& IncludedBoneIndices, _uint iPlayingAnimIndex)
{
	_int						iRootBoneIndex = { Find_RootBoneIndex() };
	vector<_float4x4>			TransformationMatrices;

	ANIM_PLAYING_INFO& AnimInfo = { m_PlayingAnimInfos[iPlayingAnimIndex] };
	_bool						isFirstTick = { false };

	if (-1 == AnimInfo.iAnimIndex ||
		0.f >= AnimInfo.fWeight ||
		true == m_Animations[AnimInfo.iAnimIndex]->isFinished())
		return TransformationMatrices;

	set<_uint>					TempIncludedBoneIndices;
	TransformationMatrices = m_Animations[AnimInfo.iAnimIndex]->Compute_TransfromationMatrix(fTimeDelta, AnimInfo.isLoop, static_cast<_uint>(m_Bones.size()), TempIncludedBoneIndices, &isFirstTick);
	if (true == TransformationMatrices.empty())
		return TransformationMatrices;

	if (true == isFirstTick)
	{
		if (-1 != m_PlayingAnimInfos[iPlayingAnimIndex].iPreAnimIndex)
		{
			m_PlayingAnimInfos[iPlayingAnimIndex].isLinearInterpolation = true;
			m_PlayingAnimInfos[iPlayingAnimIndex].fAccLinearInterpolation = 0.f;
		}
	}

	//	���� �ִϸ��̼��� �ش�� �� ���̾��� ���鸸 ������� ���� ����Ѵ�.
	for (_uint iBoneIndex : TempIncludedBoneIndices)
	{
		if (true == m_BoneLayers[m_PlayingAnimInfos[iPlayingAnimIndex].strBoneLayerTag]->Is_Included(iBoneIndex))
			IncludedBoneIndices.emplace(iBoneIndex);
	}

	//	�ִϸ��̼��� �����������̾��ٸ� ���������� ��Ʈ������ �� ������Ʈ�Ѵ�.
	Update_LinearInterpolation(fTimeDelta, iPlayingAnimIndex);

	if (true == AnimInfo.isLinearInterpolation)
	{
		//	ù ���� ���� ���� ��Ʈ Ű��������� ��Ʈ������ ���뿡���� ���� ���ο� Ű�������� ��ȯ������ ����
		if (true /*== isFirstTick*/)
		{
			_matrix			RootTransformationMatrix = { XMLoadFloat4x4(&TransformationMatrices[iRootBoneIndex]) };
			_vector			vRootScale, vRootQuaternion, vRootTranslation;

			XMMatrixDecompose(&vRootScale, &vRootQuaternion, &vRootTranslation, RootTransformationMatrix);

			_vector			vLastTranslation = { XMLoadFloat3(&AnimInfo.LastKeyFrames[iRootBoneIndex].vTranslation) };
			_vector			vLastQuaternion = { XMLoadFloat4(&AnimInfo.LastKeyFrames[iRootBoneIndex].vRotation) };
			_vector			vResultTranslation = { vLastTranslation };

			if (true == m_isRootMotion_XZ)
			{
				vResultTranslation = XMVectorSetX(vResultTranslation, XMVectorGetX(vRootTranslation));
				vResultTranslation = XMVectorSetZ(vResultTranslation, XMVectorGetZ(vRootTranslation));
			}

			if (true == m_isRootMotion_Y)
			{
				vResultTranslation = XMVectorSetY(vResultTranslation, XMVectorGetY(vRootTranslation));
			}

			XMStoreFloat3(&AnimInfo.LastKeyFrames[iRootBoneIndex].vTranslation, vResultTranslation);

			if (true == m_isRootMotion_Rotation)
			{
				XMStoreFloat4(&AnimInfo.LastKeyFrames[iRootBoneIndex].vRotation, vRootQuaternion);
			}
		}

		TransformationMatrices = m_Animations[AnimInfo.iAnimIndex]->Compute_TransfromationMatrix_LinearInterpolation(AnimInfo.fAccLinearInterpolation, m_fTotalLinearTime, TransformationMatrices, static_cast<_uint>(m_Bones.size()), AnimInfo.LastKeyFrames);
	}

	if (true == isFirstTick)
	{
		_matrix			RootTransformationMatrix = { XMLoadFloat4x4(&TransformationMatrices[iRootBoneIndex]) };
		_vector			vScale, vQuaternion, vTranslation;

		XMMatrixDecompose(&vScale, &vQuaternion, &vTranslation, RootTransformationMatrix);

		if (true == m_isRootMotion_XZ)
		{
			_vector			vPreTranslationLocal = { XMLoadFloat3(&AnimInfo.vPreTranslationLocal) };
			_vector			vResultTranslationLocal;

			vResultTranslationLocal = XMVectorSetX(vPreTranslationLocal, XMVectorGetX(vTranslation));
			vResultTranslationLocal = XMVectorSetZ(vResultTranslationLocal, XMVectorGetZ(vTranslation));

			XMStoreFloat3(&AnimInfo.vPreTranslationLocal, vResultTranslationLocal);
		}

		if (true == m_isRootMotion_Y)
		{
			_vector			vPreTranslationLocal = { XMLoadFloat3(&AnimInfo.vPreTranslationLocal) };
			_vector			vResultTranslationLocal;

			vResultTranslationLocal = XMVectorSetY(vPreTranslationLocal, XMVectorGetY(vTranslation));

			XMStoreFloat3(&AnimInfo.vPreTranslationLocal, vResultTranslationLocal);
		}

		if (true == m_isRootMotion_Rotation)
		{
			XMStoreFloat4(&AnimInfo.vPreQuaternion, vQuaternion);
		}
	}

	//	���� �������̾ƴϾ��ٸ� ���Ŀ� �Ͼ ���� ������ ����Ͽ� ������ Ű�����ӵ��� �����Ѵ�.
	if (false == AnimInfo.isLinearInterpolation)
	{
		Update_LastKeyFrames(TransformationMatrices, AnimInfo.LastKeyFrames);
	}

	return TransformationMatrices;
}

void CModel::Apply_Bone_CombinedMatrices(CTransform* pTransform, _float3* pMovedDirection)
{
	//	��� ä�ξ�����Ʈ�� ������ �� ���� �Ĺ��ε� ����� �����Ѵ�.
	_uint		iNumBones = { static_cast<_uint>(m_Bones.size()) };
	for (_uint iBoneIndex = 0; iBoneIndex < iNumBones; ++iBoneIndex)
	{
		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
		// TODO:		���̾�ȭ�Ͽ� ���� �۾����� ���� ���Ե��� Ȯ���ϴ� �����ʿ�
		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
		/*_bool		isInclude = { false };
		for (auto& iIndex : AnimInfo.TargetBoneIndices)
		{
			if (pBone == m_Bones[iIndex])
			{
				isInclude = true;
				break;
			}
		}

		if (false == isInclude)
			continue;*/

		_bool		isRootBone = { m_Bones[iBoneIndex]->Is_RootBone() };
		if (true == isRootBone)
		{
			_float4			vTranslation = { 0.f, 0.f, 0.f, 1.f };		_float4			vQuaternion = {};
			_float4* pTranslation = { &vTranslation };			_float4* pQuaternion = { &vQuaternion };

			//	if (false == m_isRootMotion_Rotation) { pQuaternion = nullptr; }
			//	if (false == m_isRootMotion_XZ && false == m_isRootMotion_Y) { pTranslation = nullptr; }

			m_Bones[iBoneIndex]->Invalidate_CombinedTransformationMatrix_RootMotion(m_Bones, m_TransformationMatrix, m_isRootMotion_XZ, m_isRootMotion_Y, m_isRootMotion_Rotation, pTranslation, pQuaternion);

			if (true == m_isRootMotion_Rotation)
				Apply_RootMotion_Rotation(pTransform, XMLoadFloat4(&vQuaternion), pTranslation);

			if (true == m_isRootMotion_XZ || true == m_isRootMotion_Y)
				Apply_RootMotion_Translation(pTransform, XMLoadFloat4(&vTranslation), pMovedDirection, m_isRootMotion_Rotation, XMLoadFloat4(&vQuaternion));
		}

		else
		{
			m_Bones[iBoneIndex]->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformationMatrix));
		}

		Apply_AdditionalForce(iBoneIndex);
	}
}

vector<_float4x4> CModel::Compute_ResultMatrices(const vector<vector<_float4x4>>& TransformationMatricesLayer, const set<_uint>& IncludedBoneIndices)
{
	vector<_float4x4>			ResultTransformationMatrices = { Initialize_ResultMatrices(IncludedBoneIndices) };

	//	�������� ����Ѵ� => ���� ��� ����� �� ���� �ε����� �ش��ϴ� �ε����鿡 �������� �� �����Ѵ�.
	_uint		iPlayAnimIndex = { 0 };
	for (auto& AnimLayer : TransformationMatricesLayer)
	{
		//	�� ��ȯ����� �����Ŀ� �����Ѵ� => ����Ʈ�� �����Ͽ� �����Ѵ�.
		for (_uint iBoneIndex : IncludedBoneIndices)
		{
			if (false == m_BoneLayers[m_PlayingAnimInfos[iPlayAnimIndex].strBoneLayerTag]->Is_Included(iBoneIndex))
				continue;

			_float			fTotalWeight = { Compute_Current_TotalWeight(iBoneIndex) };
			if (0.f >= fTotalWeight)
				continue;

			_float			fWeight = { m_PlayingAnimInfos[iPlayAnimIndex].fWeight / fTotalWeight };
			if (0.f >= fWeight)
				continue;

			_matrix			ResultMatrix = { XMLoadFloat4x4(&ResultTransformationMatrices[iBoneIndex]) };
			_matrix			TransformationMatrix = { XMLoadFloat4x4(&TransformationMatricesLayer[iPlayAnimIndex][iBoneIndex]) };

			_float4x4		ResultFloat4x4 = { Compute_BlendTransformation_Additional(ResultMatrix, TransformationMatrix, fWeight) };

			XMStoreFloat4x4(&ResultTransformationMatrices[iBoneIndex], XMLoadFloat4x4(&ResultFloat4x4));
		}

		++iPlayAnimIndex;
	}

	return ResultTransformationMatrices;
}

void CModel::Apply_Bone_TransformMatrices(const vector<vector<_float4x4>>& TransformationMatricesLayer, const set<_uint>& IncludedBoneIndices)
{
	//	������ ���� ���� ������ ���� ��ȯ��ķ� �ʱ�ȭ�Ѵ�.
	//	������ �������� �׵� ��ķ� �ʱ�ȭ �� ���� �������� ����� �����Ѵ�.
	vector<_float4x4>		ResultTransformationMatrices = { Compute_ResultMatrices(TransformationMatricesLayer, IncludedBoneIndices) };

	_int	iRootBoneIndex = { Find_RootBoneIndex() };

	for (_uint iBoneIndex = 0; iBoneIndex < m_Bones.size(); ++iBoneIndex)
	{
		set<_uint>::iterator		iter = { find(IncludedBoneIndices.begin(), IncludedBoneIndices.end(), iBoneIndex) };
		if (iter == IncludedBoneIndices.end())
			continue;

		_matrix			ResultMatrix = { XMLoadFloat4x4(&ResultTransformationMatrices[iBoneIndex]) };

		_vector			vScale, vQuaternion, vTranslation;
		XMMatrixDecompose(&vScale, &vQuaternion, &vTranslation, ResultMatrix);

		vScale -= XMVectorSet(1.f, 1.f, 1.f, 0.f);

		ResultMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vQuaternion, vTranslation);

		XMStoreFloat4x4(&ResultTransformationMatrices[iBoneIndex], ResultMatrix);
	}

	//	�����ĵ��� ���� Ʈ�������� �����Ѵ�.
	for (_uint iBoneIndex = 0; iBoneIndex < static_cast<_uint>(m_Bones.size()); ++iBoneIndex)
	{
		if (iBoneIndex == iRootBoneIndex)
		{
			int iA = 0;
		}
		m_Bones[iBoneIndex]->Set_TransformationMatrix(ResultTransformationMatrices[iBoneIndex]);
	}
}

void CModel::Static_Mesh_Cooking()
{
	for (int i = 0; i < m_Meshes.size(); ++i)
	{
		m_Meshes[i]->Static_Mesh_Cooking();
	}
}

_int CModel::Find_BoneIndex(const string& strRootTag)
{
	_int		iIndex = { 0 };
	for (auto& Bone : m_Bones)
	{
		if (true == Bone->Compare_Name(strRootTag.c_str()))
			return iIndex;

		++iIndex;
	}

	return -1;
}

void CModel::Compute_RootParentsIndicies(_uint iRootIndex, vector<_uint>& ParentsIndices)
{
	ParentsIndices.push_back(iRootIndex);
	_int iParentIndex = m_Bones[iRootIndex]->Get_ParentIndex();

	if (-1 == iParentIndex)
		return;

	Compute_RootParentsIndicies(iParentIndex, ParentsIndices);
}

void CModel::Motion_Changed(_uint iPlayingIndex)
{
	Reset_LinearInterpolation(iPlayingIndex);

	m_PlayingAnimInfos[iPlayingIndex].isLinearInterpolation = true;
}

void CModel::Update_LastKeyFrames(const vector<_float4x4>& TransformationMatrices, vector<KEYFRAME>& LastKeyFrames)
{
	LastKeyFrames.clear();

	for (_uint iBoneIndex = 0; iBoneIndex < static_cast<_uint>(m_Bones.size()); ++iBoneIndex)
	{
		_vector			vScale, vRotation, vTranslation;
		KEYFRAME		KeyFrame;
		_matrix			TransformationMatrix = { XMLoadFloat4x4(&TransformationMatrices[iBoneIndex]) };

		XMMatrixDecompose(&vScale, &vRotation, &vTranslation, TransformationMatrix);

		XMStoreFloat3(&KeyFrame.vScale, vScale);
		XMStoreFloat4(&KeyFrame.vRotation, vRotation);
		XMStoreFloat3(&KeyFrame.vTranslation, vTranslation);
		KeyFrame.fTime = m_fTotalLinearTime;

		LastKeyFrames.push_back(KeyFrame);
	}
}

void CModel::Update_LastKeyFrames_Bones(vector<KEYFRAME>& LastKeyFrames)
{
	LastKeyFrames.clear();

	for (auto& pBone : m_Bones)
	{
		_vector			vScale, vRotation, vTranslation;
		KEYFRAME		KeyFrame;
		_matrix			TransformationMatrix = { pBone->Get_TrasformationMatrix() };

		XMMatrixDecompose(&vScale, &vRotation, &vTranslation, TransformationMatrix);

		XMStoreFloat3(&KeyFrame.vScale, vScale);
		XMStoreFloat4(&KeyFrame.vRotation, vRotation);
		XMStoreFloat3(&KeyFrame.vTranslation, vTranslation);
		KeyFrame.fTime = m_fTotalLinearTime;

		LastKeyFrames.push_back(KeyFrame);
	}
}

void CModel::Update_LinearInterpolation(_float fTimeDelta, _uint iPlayingIndex)
{
	if (false == m_PlayingAnimInfos[iPlayingIndex].isLinearInterpolation)
		return;

	m_PlayingAnimInfos[iPlayingIndex].fAccLinearInterpolation += fTimeDelta;

	if (m_fTotalLinearTime <= m_PlayingAnimInfos[iPlayingIndex].fAccLinearInterpolation)
	{
		Reset_LinearInterpolation(iPlayingIndex);
	}
}

void CModel::Reset_LinearInterpolation(_uint iPlayingIndex)
{
	m_PlayingAnimInfos[iPlayingIndex].isLinearInterpolation = false;
	m_PlayingAnimInfos[iPlayingIndex].fAccLinearInterpolation = 0.f;
}

HRESULT CModel::Ready_Meshes(const map<string, _uint>& BoneIndices)
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], BoneIndices, XMLoadFloat4x4(&m_TransformationMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

		MESH_MATERIAL			MeshMaterial{};

		for (size_t j = aiTextureType_DIFFUSE; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			aiString		strTextureFilePath;

			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTextureFilePath)))
			{
				MeshMaterial.MaterialTextures[j] = nullptr;

				continue;
			}

			_char			szDrive[MAX_PATH] = { "" };
			_char			szDirectory[MAX_PATH] = { "" };
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);


			_char			szFileName[MAX_PATH] = { "" };
			_char			szEXT[MAX_PATH] = { "" };

			/* ..\Bin\Resources\Models\Fiona\ */
			_splitpath_s(strTextureFilePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

			_char			szFullPath[MAX_PATH] = { "" };
			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDirectory);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szEXT);

			_tchar			szPerfectPath[MAX_PATH] = { L"" };

			MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szPerfectPath, MAX_PATH);


			MeshMaterial.MaterialTextures[j] = CTexture::Create(m_pDevice, m_pContext, szPerfectPath);
			if (nullptr == MeshMaterial.MaterialTextures[j])
				return E_FAIL;
		}

		m_Materials.push_back(MeshMaterial);

	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(aiNode* pAINode, map<string, _uint>& BoneIndices, _int iParentIndex)
{
	CBone* pBone = CBone::Create(pAINode, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	/* ������ �����̴�.. map�� ������ ����ش�. index������ �̸���.. */
	BoneIndices.emplace(pBone->Get_Name(), (_uint)(m_Bones.size() - 1));

	_int iParent = (_int)(m_Bones.size() - 1);

	for (size_t i = 0; i < pAINode->mNumChildren; ++i)
	{
		Ready_Bones(pAINode->mChildren[i], BoneIndices, iParent);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations(const map<string, _uint>& BoneIndices)
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], BoneIndices);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

HRESULT CModel::Ready_Meshes(ifstream& ifs)
{
	ifs.read(reinterpret_cast<_char*>(&m_iNumMeshes), sizeof(_uint));
	ifs.read(reinterpret_cast<_char*>(&m_eModelType), sizeof(MODEL_TYPE));

	_char szName[MAX_PATH] = { "" };
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh::MESH_DESC MeshDesc;

		ifs.read(reinterpret_cast<_char*>(szName), sizeof(_char) * MAX_PATH);
		MeshDesc.strName = szName;
		ifs.read(reinterpret_cast<_char*>(&MeshDesc.iMaterialIndex), sizeof(_uint));

		ifs.read(reinterpret_cast<_char*>(&MeshDesc.iNumVertices), sizeof(_uint));

		VTXANIMMESH Vertex{};
		for (_uint i = 0; i < MeshDesc.iNumVertices; ++i)
		{
			ifs.read(reinterpret_cast<_char*>(&Vertex.vPosition), sizeof(_float3));
			ifs.read(reinterpret_cast<_char*>(&Vertex.vNormal), sizeof(_float3));
			ifs.read(reinterpret_cast<_char*>(&Vertex.vTexcoord), sizeof(_float2));
			ifs.read(reinterpret_cast<_char*>(&Vertex.vTangent), sizeof(_float3));

			if (MODEL_TYPE::TYPE_ANIM == m_eModelType)
			{
				ifs.read(reinterpret_cast<_char*>(&Vertex.vBlendIndices), sizeof(XMUINT4));
				ifs.read(reinterpret_cast<_char*>(&Vertex.vBlendWeights), sizeof(_float4));
			}

			MeshDesc.Vertices.push_back(Vertex);
		}

		ifs.read(reinterpret_cast<_char*>(&MeshDesc.iNumIndices), sizeof(_uint));
		_uint iIndex = { 0 };
		for (_uint i = 0; i < MeshDesc.iNumIndices; ++i)
		{
			ifs.read(reinterpret_cast<_char*>(&iIndex), sizeof(_uint));

			MeshDesc.Indices.push_back(iIndex);
		}

		ifs.read(reinterpret_cast<_char*>(&MeshDesc.iNumBones), sizeof(_uint));
		_uint iBoneIndex = { 0 };
		for (_uint i = 0; i < MeshDesc.iNumBones; ++i)
		{
			ifs.read(reinterpret_cast<_char*>(&iBoneIndex), sizeof(_uint));

			MeshDesc.Bones.push_back(iBoneIndex);
		}

		ifs.read(reinterpret_cast<_char*>(&MeshDesc.iNumOffsetMatrices), sizeof(_uint));
		_float4x4 OffsetMatrix;
		for (_uint i = 0; i < MeshDesc.iNumOffsetMatrices; ++i)
		{
			ifs.read(reinterpret_cast<_char*>(&OffsetMatrix), sizeof(_float4x4));

			MeshDesc.OffsetMatrices.push_back(OffsetMatrix);
		}

		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, MeshDesc);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(ifstream& ifs)
{
	ifs.read(reinterpret_cast<_char*>(&m_iNumMaterials), sizeof(_uint));

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MESH_MATERIAL			MeshMaterial{};

		for (size_t i = aiTextureType_DIFFUSE; i < AI_TEXTURE_TYPE_MAX; ++i)
		{
			_tchar szPath[MAX_PATH] = { TEXT("") };
			ifs.read(reinterpret_cast<_char*>(&szPath), sizeof(_tchar) * MAX_PATH);

			if (wstring(szPath) == L"")
			{
				continue;
			}

			MeshMaterial.MaterialTextures[i] = CTexture::Create(m_pDevice, m_pContext, szPath);
			if (nullptr == MeshMaterial.MaterialTextures[i])
				return E_FAIL;
		}

		m_Materials.push_back(MeshMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(ifstream& ifs)
{
	_uint iNumBones = { 0 };
	ifs.read(reinterpret_cast<_char*>(&iNumBones), sizeof(_uint));

	_char		szName[MAX_PATH] = { "" };

	for (_uint i = 0; i < iNumBones; ++i)
	{
		CBone::BONE_DESC BoneDesc;

		ifs.read(reinterpret_cast<_char*>(&BoneDesc.iParentIndex), sizeof(_int));
		ifs.read(reinterpret_cast<_char*>(szName), sizeof(_char) * MAX_PATH);
		BoneDesc.strName = szName;
		ifs.read(reinterpret_cast<_char*>(&BoneDesc.TransformationMatrix), sizeof(_float4x4));

		CBone* pBone = CBone::Create(BoneDesc);
		if (nullptr == pBone)
			return E_FAIL;

		m_Bones.push_back(pBone);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations(ifstream& ifs)
{
	ifs.read(reinterpret_cast<_char*>(&m_iNumAnimations), sizeof(_uint));

	/* For.Animation */
	_char		szName[MAX_PATH];

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation::ANIM_DESC		AnimDesc;

		ifs.read(reinterpret_cast<_char*>(&szName), sizeof(_char) * MAX_PATH);
		AnimDesc.strName = szName;
		ifs.read(reinterpret_cast<_char*>(&AnimDesc.fDuration), sizeof(_float));
		ifs.read(reinterpret_cast<_char*>(&AnimDesc.fTickPerSecond), sizeof(_float));

		ifs.read(reinterpret_cast<_char*>(&AnimDesc.iNumChannels), sizeof(_uint));
		for (_uint j = 0; j < AnimDesc.iNumChannels; ++j)
		{
			CChannel::CHANNEL_DESC		ChannelDesc;

			ifs.read(reinterpret_cast<_char*>(&szName), sizeof(_char) * MAX_PATH);
			ChannelDesc.strName = szName;
			ifs.read(reinterpret_cast<_char*>(&ChannelDesc.iBoneIndex), sizeof(_uint));

			ifs.read(reinterpret_cast<_char*>(&ChannelDesc.iNumKeyFrames), sizeof(_uint));
			for (_uint k = 0; k < ChannelDesc.iNumKeyFrames; ++k)
			{
				KEYFRAME					KeyFrame;

				ifs.read(reinterpret_cast<_char*>(&KeyFrame.vScale), sizeof(_float3));
				ifs.read(reinterpret_cast<_char*>(&KeyFrame.vRotation), sizeof(_float4));
				ifs.read(reinterpret_cast<_char*>(&KeyFrame.vTranslation), sizeof(_float3));
				ifs.read(reinterpret_cast<_char*>(&KeyFrame.fTime), sizeof(_float));

				ChannelDesc.KeyFrames.push_back(KeyFrame);
			}

			AnimDesc.ChannelDescs.push_back(ChannelDesc);
		}

		CAnimation* pAnim = CAnimation::Create(AnimDesc);
		if (nullptr == pAnim)
			return E_FAIL;

		m_Animations.push_back(pAnim);
	}

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, strModelFilePath, TransformMatrix)))
	{
		MSG_BOX(TEXT("Failed To Created : CModel"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string& strModelFilePath, _fmatrix TransformMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strModelFilePath, TransformMatrix)))
	{
		MSG_BOX(TEXT("Failed To Created : CModel"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CModel"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();

	for (auto& Material : m_Materials)
	{
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			Safe_Release(Material.MaterialTextures[i]);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);
	m_Meshes.clear();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

	for (auto& Pair : m_BoneLayers)
		Safe_Release(Pair.second);
	m_BoneLayers.clear();

	m_Importer.FreeScene();
}