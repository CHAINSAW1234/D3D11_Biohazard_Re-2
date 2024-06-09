#include "IK_Solver.h"
#include "Bone.h"

#include "Transform.h"

CIK_Solver::CIK_Solver()
{
}

HRESULT CIK_Solver::Initialize(void* pArg)
{
	return S_OK;
}

void CIK_Solver::Add_IK(vector<class CBone*>& Bones, list<_uint> IkIncludedIndices, _uint iIKRootBoneIndex, _uint iEndEffectorIndex, string strTargetJointTag, string strEndEffectorTag, const wstring& strIKTag, _uint iNumIteration, _float fBlend)
{
	map<wstring, IK_INFO>::iterator		iter = { m_IKInfos.find(strIKTag) };
	if (iter != m_IKInfos.end())
		return;

	_bool					isCombined = { false };
	IK_INFO					IkInfo;

	IkInfo.iNumIteration = iNumIteration;
	IkInfo.fBlend = fBlend;

	vector<_uint>			JointIndices;
	_uint					iBoneIndex = { static_cast<_uint>(iEndEffectorIndex) };
	JointIndices.push_back(iBoneIndex);
	while (false == isCombined)
	{
		_int		iParrentIndex = { Bones[iBoneIndex]->Get_ParentIndex() };
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
	
	for (auto& iIndex : IkIncludedIndices)
	{
		IkInfo.IKIncludedIndices.push_back(iIndex);
	}

	//	�ӽ� �߰�
	//	�ӽ� �߰�
	//	�ӽ� �߰�
	//	�ӽ� �߰�

	_uint			iNumJoint = { static_cast<_uint>(IkInfo.JointIndices.size()) };

	IkInfo.BoneThetas.clear();
	IkInfo.BoneThetas.push_back(_float4(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(40.f)));
	IkInfo.BoneThetas.push_back(_float4(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(40.f)));
	IkInfo.BoneThetas.push_back(_float4(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(40.f)));
	IkInfo.BoneThetas.push_back(_float4(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(40.f)));

	IkInfo.BoneOrientationLimits.clear();
	for (_uint i = 0; i < iNumJoint; ++i)
		IkInfo.BoneOrientationLimits.push_back(XMConvertToRadians(40.f));

	IkInfo.JointTypes.clear();
	IkInfo.JointTypes.push_back(JOINT_TYPE::JOINT_HINGE);
	IkInfo.JointTypes.push_back(JOINT_TYPE::JOINT_HINGE);
	IkInfo.JointTypes.push_back(JOINT_TYPE::JOINT_HINGE);
	IkInfo.JointTypes.push_back(JOINT_TYPE::JOINT_END);
	//	IkInfo.JointTypes.push_back(i % 2 == 0 ? JOINT_TYPE::JOINT_BALL : JOINT_TYPE::JOINT_HINGE);

	m_IKInfos[strIKTag] = IkInfo;
}

void CIK_Solver::Erase_IK(const wstring& strIKTag)
{
	map<wstring, CIK_Solver::IK_INFO>::iterator			iter = { m_IKInfos.find(strIKTag) };
	if (iter == m_IKInfos.end())
		return;

	m_IKInfos.erase(iter);
}

void CIK_Solver::Set_TargetPosition_IK(const wstring& strIKTag, _fvector vTargetPosition)
{
	map<wstring, IK_INFO>::iterator		iter = { m_IKInfos.find(strIKTag) };
	if (iter == m_IKInfos.end())
		return;

	XMStoreFloat3(&m_IKInfos[strIKTag].vIKEndTargetPosition, vTargetPosition);
}

void CIK_Solver::Set_NumIteration_IK(const wstring& strIKTag, _uint iNumIteration)
{
	map<wstring, IK_INFO>::iterator		iter = { m_IKInfos.find(strIKTag) };
	if (iter == m_IKInfos.end())
		return;

	m_IKInfos[strIKTag].iNumIteration = iNumIteration;
}

void CIK_Solver::Set_Blend_IK(wstring strIKTag, _float fBlend)
{
	map<wstring, IK_INFO>::iterator		iter = { m_IKInfos.find(strIKTag) };
	if (iter == m_IKInfos.end())
		return;

	m_IKInfos[strIKTag].fBlend = fBlend;
}

vector<_float4> CIK_Solver::Get_ResultTranslation_IK(const wstring& strIKTag)
{
	map<wstring, IK_INFO>::iterator		iter = { m_IKInfos.find(strIKTag) };
	if (iter == m_IKInfos.end())
		return vector<_float4>();

	return m_IKInfos[strIKTag].BoneTranslationsResult;
}

vector<_float4> CIK_Solver::Get_OriginTranslation_IK(const wstring& strIKTag)
{
	map<wstring, IK_INFO>::iterator		iter = { m_IKInfos.find(strIKTag) };
	if (iter == m_IKInfos.end())
		return vector<_float4>();

	return m_IKInfos[strIKTag].BoneTranslationsOrigin;
}

vector<_float4x4> CIK_Solver::Get_JointCombinedMatrices_IK(const wstring& strIKTag, vector<CBone*>& Bones)
{
	map<wstring, IK_INFO>::iterator		iter = { m_IKInfos.find(strIKTag) };
	if (iter == m_IKInfos.end())
		return vector<_float4x4>();

	vector<_float4x4>		JointCombinedMatrices;
	for (auto& iBoneIndex : m_IKInfos[strIKTag].JointIndices)
	{
		JointCombinedMatrices.push_back(*Bones[iBoneIndex]->Get_CombinedTransformationMatrix());
	}

	return JointCombinedMatrices;
}

void CIK_Solver::Play_IK(vector<CBone*>& Bones, CTransform* pTransform)
{
	for (auto& Pair : m_IKInfos)
	{
		Apply_IK(Bones, pTransform, Pair.second);
	}
}

void CIK_Solver::Apply_IK(vector<CBone*>& Bones, CTransform* pTransform, IK_INFO& IkInfo)
{
	if (-1 == IkInfo.iEndEffectorIndex || -1 == IkInfo.iIKRootBoneIndex)
		return;

	_matrix			WorldMatrixInv = { pTransform->Get_WorldMatrix_Inverse() };
	_vector			vEndEffectorPosition = { XMVectorSetW(XMLoadFloat3(&IkInfo.vIKEndTargetPosition), 1.f) };

	_vector			vEndEffectorResultPosition = { XMVector3TransformCoord(vEndEffectorPosition, WorldMatrixInv) };
	XMStoreFloat4(&IkInfo.vEndEffectorResultPosition, vEndEffectorResultPosition);

	_vector			vTargetJointStartTranslation = { XMLoadFloat4((_float4*)Bones[IkInfo.iIKRootBoneIndex]->Get_CombinedTransformationMatrix()->m[CTransform::STATE_POSITION]) };
	XMStoreFloat4(&IkInfo.vTargetJointStartTranslation, vTargetJointStartTranslation);

	//	���� ���� �Ÿ��� ��ġ���� �ʱ�ȭ�� ���� �ִϸ��̼ǿ� �°� ������ ����
	Update_Distances_Translations_IK(Bones, IkInfo);

	for (_uint i = 0; i < IkInfo.iNumIteration; ++i)
	{
		Solve_IK(IkInfo);
	}

	Update_TransformMatrices_BoneChain(Bones, IkInfo);
	Update_CombinedMatrices_BoneChain(Bones, IkInfo);
}

void CIK_Solver::Update_Distances_Translations_IK(vector<CBone*>& Bones, IK_INFO& IkInfo)
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
		_matrix			CombinedMatrix = { XMLoadFloat4x4(Bones[IkInfo.JointIndices[i]]->Get_CombinedTransformationMatrix()) };

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

		_vector			vMyTranslation = { XMLoadFloat4((_float4*)(&Bones[IkInfo.JointIndices[i]]->Get_CombinedTransformationMatrix()->m[CTransform::STATE_POSITION][0])) };
		_float4			vMyTranslationFloat4;
		XMStoreFloat4(&vMyTranslationFloat4, vMyTranslation);

		IkInfo.BoneTranslationsOrigin.push_back(vMyTranslationFloat4);

		_vector			vParrentTranslation, vChildTranslation;
		if (true == isExistParrents)
		{
			vParrentTranslation = { XMLoadFloat4((_float4*)(&Bones[IkInfo.JointIndices[i - 1]]->Get_CombinedTransformationMatrix()->m[CTransform::STATE_POSITION][0])) };
		}
		else
		{
			vParrentTranslation = { XMLoadFloat4(&IkInfo.vTargetJointStartTranslation) };
		}

		if (true == isExistChild)
		{
			vChildTranslation = { XMLoadFloat4((_float4*)(&Bones[IkInfo.JointIndices[i + 1]]->Get_CombinedTransformationMatrix()->m[CTransform::STATE_POSITION][0])) };
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

void CIK_Solver::Solve_IK(IK_INFO& IkInfo)
{
	Solve_IK_Forward(IkInfo);
	Solve_IK_Backward(IkInfo);
}

void CIK_Solver::Solve_IK_Forward(IK_INFO& IkInfo)
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

void CIK_Solver::Solve_IK_Backward(IK_INFO& IkInfo)
{
	_uint			iNumIKIndices = { static_cast<_uint>(IkInfo.JointIndices.size()) };

	//	���� �������� ��ġ�� ��ǥ��ġ�� �ű��.
	IkInfo.BoneTranslationsResult[0] = IkInfo.vTargetJointStartTranslation;

	for (_int i = 1; i < static_cast<_int>(iNumIKIndices); ++i)
	{
		Solve_For_Distance_IK(IkInfo, i - 1, i);
	}
}

void CIK_Solver::Solve_For_Distance_IK(IK_INFO& IkInfo, _int iSrcJointIndex, _int iDstJointIndex)
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

void CIK_Solver::Solve_For_Orientation_IK(IK_INFO& IkInfo, _int iOuterJointIndex, _int iInnerJointIndex)
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

void CIK_Solver::Rotational_Constranit(IK_INFO& IkInfo, _int iOuterJointIndex, _int iMyJointIndex)
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

void CIK_Solver::Update_Forward_Reaching_IK(IK_INFO& IkInfo)
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

void CIK_Solver::Update_Backward_Reaching_IK(IK_INFO& IkInfo)
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

void CIK_Solver::Update_TransformMatrices_BoneChain(vector<CBone*>& Bones, IK_INFO& IkInfo)
{
	_uint			iNumIKIndices = { static_cast<_uint>(IkInfo.JointIndices.size()) };
	_matrix         ResultParentCombinedMatrix = { XMLoadFloat4x4(Bones[Bones[IkInfo.iIKRootBoneIndex]->Get_ParentIndex()]->Get_CombinedTransformationMatrix()) };

	for (_uint i = 0; i < iNumIKIndices - 1; ++i)
	{
		_vector         vMyOriginTranslation = { XMLoadFloat4(&IkInfo.BoneTranslationsOrigin[i]) };
		_vector         vChildOriginTranslation = { XMLoadFloat4(&IkInfo.BoneTranslationsOrigin[i + 1]) };
		_vector         vMyResultTranslation = { XMLoadFloat4(&IkInfo.BoneTranslationsResult[i]) };
		_vector         vChildResultTranslation = { XMLoadFloat4(&IkInfo.BoneTranslationsResult[i + 1]) };

		_matrix			OriginParentCombinedMatrix = { XMLoadFloat4x4(Bones[Bones[IkInfo.JointIndices[i]]->Get_ParentIndex()]->Get_CombinedTransformationMatrix()) };
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

		_matrix			TransformMatrix = { Bones[IkInfo.JointIndices[i]]->Get_TrasformationMatrix() };
		_matrix			RotationMatrix = { XMMatrixRotationQuaternion(vBlendedQuaternion) };

		_matrix			OriginTransformMatrix = { Bones[IkInfo.JointIndices[i]]->Get_TrasformationMatrix() };
		_matrix			ResultTransformMatrix = { OriginTransformMatrix };

		ResultTransformMatrix.r[CTransform::STATE_POSITION] = { XMVectorSet(0.f, 0.f, 0.f, 1.f) };
		ResultTransformMatrix = ResultTransformMatrix * RotationMatrix;
		ResultTransformMatrix.r[CTransform::STATE_POSITION] = vMyOriginTranslationInvalidated;

		Bones[IkInfo.JointIndices[i]]->Set_TransformationMatrix(ResultTransformMatrix);
		ResultParentCombinedMatrix = ResultTransformMatrix * ResultParentCombinedMatrix;
	}
}

void CIK_Solver::Update_CombinedMatrices_BoneChain(vector<CBone*>& Bones, IK_INFO& IkInfo)
{
	for (auto& iBoneIndex : IkInfo.IKIncludedIndices)
	{
		_int		iParrentIndex = { Bones[iBoneIndex]->Get_ParentIndex() };
		_matrix		ParrentCombinedMatrix = { XMLoadFloat4x4(Bones[iParrentIndex]->Get_CombinedTransformationMatrix()) };
		_matrix		MyTransformMatrix = { Bones[iBoneIndex]->Get_TrasformationMatrix() };
		_matrix		MyCombinedMatrix = { MyTransformMatrix * ParrentCombinedMatrix };

		Bones[iBoneIndex]->Set_Combined_Matrix(MyCombinedMatrix);
	}
}

_vector CIK_Solver::Compute_Quaternion_From_TwoDirection(_fvector vSrcDirection, _fvector vDstDirection)
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

CIK_Solver::CONIC_SECTION CIK_Solver::Find_ConicSection(_float fTheta)
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

_bool CIK_Solver::Is_InBound(CONIC_SECTION eSection, _fvector vQ, _float2 vTarget)
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

_float2 CIK_Solver::Find_Nearest_Point_Constraints(_float fMajorAxisLength, _float fMinorAxisLength, CONIC_SECTION eSection, _float2 vTargetPosition)
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
_float2 CIK_Solver::Find_Initial_Point_Constraints(_float fMajorAxisLength, _float fMinorAxisLength, CONIC_SECTION eSection, _float2 vTargetPosition)
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

_float2 CIK_Solver::Find_Next_Point_Constraints(_float2 vCurrentPoint, _float fMajorAxisLength, _float fMinorAxisLength, _float2 vTargetPosition)
{
	_float2			vDelta = { Compute_Delta_Constratins(vCurrentPoint, fMajorAxisLength, fMinorAxisLength, vTargetPosition) };
	_float2			vResultPoint = { vCurrentPoint };

	vResultPoint.x += vDelta.x;
	vResultPoint.y += vDelta.y;

	return vResultPoint;
}

_float2 CIK_Solver::Compute_Delta_Constratins(_float2 vCurrentPoint, _float fMajorAxisLength, _float fMinorAxisLength, _float2 vTargetPosition)
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

_float4x4 CIK_Solver::Compute_QMatrix(_float2 vCurrentPoint, _float fMajorAxisLength, _float fMinorAxisLength, _float2 vTargetPosition)
{
	_float4x4		QFloat4x4 = {};

	QFloat4x4.m[0][0] = powf(fMinorAxisLength, 2.f) * vCurrentPoint.x;
	QFloat4x4.m[0][1] = (powf(fMajorAxisLength, 2.f) - powf(fMinorAxisLength, 2.f)) * vCurrentPoint.y + powf(fMinorAxisLength, 2.f) * vTargetPosition.y;
	QFloat4x4.m[1][0] = powf(fMajorAxisLength, 2.f) * vCurrentPoint.y;
	QFloat4x4.m[1][1] = (powf(fMajorAxisLength, 2.f) - powf(fMinorAxisLength, 2.f)) * vCurrentPoint.x - powf(fMajorAxisLength, 2.f) * vTargetPosition.x;

	return QFloat4x4;
}

CIK_Solver* CIK_Solver::Create(void* pArg)
{
	CIK_Solver*			pInstance = { new CIK_Solver() };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CIK_Solver"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIK_Solver::Free()
{
	__super::Free();
}
