#include "Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel, const map<string, _uint>& BoneIndices)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	m_iBoneIndex = BoneIndices.find(m_szName)->second;

	m_iNumKeyFrames = max(max(pAIChannel->mNumPositionKeys, pAIChannel->mNumRotationKeys), pAIChannel->mNumScalingKeys);

	_float3			vScale;
	_float4			vRotation;
	_float3			vTranslation;
	_float			fTime;

	for (size_t i = 0; i < m_iNumKeyFrames; ++i)
	{
		KEYFRAME			KeyFrame;
		ZeroMemory(&KeyFrame, sizeof(KEYFRAME));

		if (i < pAIChannel->mNumScalingKeys)
		{
			memcpy_s(&vScale, sizeof(_float3), &pAIChannel->mScalingKeys[i].mValue, sizeof(aiVector3D));
			fTime = (_float)pAIChannel->mScalingKeys[i].mTime;
		}

		if (i < pAIChannel->mNumRotationKeys)
		{
			//	aiQuaternion�� w, x, y, z������ �����̵Ǿ� �״�� memcopy�ϸ� �ȉ�
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;

			fTime = (_float)pAIChannel->mRotationKeys[i].mTime;
		}

		if (i < pAIChannel->mNumPositionKeys)
		{
			memcpy_s(&vTranslation, sizeof(_float3), &pAIChannel->mPositionKeys[i].mValue, sizeof(aiVector3D));
			fTime = (_float)pAIChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vTranslation = vTranslation;
		KeyFrame.fTime = fTime;

		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

HRESULT CChannel::Initialize(const CHANNEL_DESC& ChannelDesc)
{
	strcpy_s(m_szName, ChannelDesc.strName.c_str());

	m_iBoneIndex = ChannelDesc.iBoneIndex;

	m_iNumKeyFrames = ChannelDesc.iNumKeyFrames;

	for (auto& KeyFrame : ChannelDesc.KeyFrames)
	{
		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

void CChannel::Invalidate_TransformationMatrix(const vector<class CBone*>& Bones, _float fTrackPosition, _uint* pCurrentKeyFrameIndex)
{
	if (0.0f == fTrackPosition)
		(*pCurrentKeyFrameIndex) = 0;

	//	������ Ű�������� ���´�.
	KEYFRAME		KeyFrame = m_KeyFrames.back();

	_vector			vScale, vRotation, vTranslation;

	//	�̹� ��� ��ġ�� ������ Ű�������� ��ġ�� �Ѿ� ���ٸ�..
	//	==> �� �̻��� ������Ʈ������ ������ ����� �����ϴ»����ϰ��..
	if (KeyFrame.fTime <= fTrackPosition)
	{
		vScale = XMLoadFloat3(&KeyFrame.vScale);
		vRotation = XMLoadFloat4(&KeyFrame.vRotation);
		vTranslation = XMLoadFloat3(&KeyFrame.vTranslation);
	}
	//	���� �����ġ�� ������ Ű�����ӿ� �������� ���� ���
	else
	{
		//	���� Ű�����Ӻ��� ���� �����ġ�� ���ų� ���� ��� ���� Ű�������� ���� Ű���������� �ø���.
		while (fTrackPosition >= m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTime)
			++(*pCurrentKeyFrameIndex);

		while (fTrackPosition < m_KeyFrames[*pCurrentKeyFrameIndex].fTime)
			--(*pCurrentKeyFrameIndex);

		//	Ratio :: ���� => ���������ϱ����� ��ġ
		//	(Ʈ���� ���� ��ġ - ���� Ű�������� ���� �ð�)
		//	-------------------------------------------------------------------------
		//	(���� Ű�������� ���۽ð� - ���� Ű�������� ���� �ð�) == ���� Ű�������� ���� 

		//	==>>	���� Ű������ ������ ��� ������ ����
		_float		fRatio = (fTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTime)
			/ (m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTime - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTime);

		//	���� Ű�����Ӱ� ���� Ű������ ������ ��ġ���� ��ġ�� ���� ������ ���� ���� Ű�����Ӱ� ���� ������ �Ѵ�. 
		//	XMVectorLerp == > Result = V0 + t * (V1 - V0);
		vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vScale),
			XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vScale), fRatio);

		//	���ʹϾ��� XMQuaternionSlerp �Լ��� �̿��Ͽ� ���������� �����Ѵ�.
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vRotation),
			XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vRotation), fRatio);

		vTranslation = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vTranslation),
			XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vTranslation), fRatio);
	}

	_matrix			TransformationMatrix = XMMatrixAffineTransformation(
		vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, XMVectorSetW(vTranslation, 1.f));

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Invalidate_TransformationMatrix_LinearInterpolation(const vector<class CBone*>& Bones, _float fAccLinearInterpolation, _float fTotalLinearTime, const vector<KEYFRAME>& LastKeyFrames)
{
	KEYFRAME		LinearStartKeyFrame = LastKeyFrames[m_iBoneIndex];
	_vector			vScale, vRotation, vTranslation;

	_matrix			TransformationMatrix = { Bones[m_iBoneIndex]->Get_TrasformationMatrix() };

	XMMatrixDecompose(&vScale, &vRotation, &vTranslation, TransformationMatrix);

	_float		fRatio = fAccLinearInterpolation / fTotalLinearTime;

	//	���� Ű�����Ӱ� ���� Ű������ ������ ��ġ���� ��ġ�� ���� ������ ���� ���� Ű�����Ӱ� ���� ������ �Ѵ�. 
	//	XMVectorLerp == > Result = V0 + t * (V1 - V0);
	vScale = XMVectorLerp(XMLoadFloat3(&LinearStartKeyFrame.vScale),
		vScale, fRatio);

	//	���ʹϾ��� XMQuaternionSlerp �Լ��� �̿��Ͽ� ���������� �����Ѵ�.
	vRotation = XMQuaternionSlerp(XMLoadFloat4(&LinearStartKeyFrame.vRotation),
		vRotation, fRatio);

	vTranslation = XMVectorLerp(XMLoadFloat3(&LinearStartKeyFrame.vTranslation),
		vTranslation, fRatio);

	TransformationMatrix = XMMatrixAffineTransformation(
		vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, XMVectorSetW(vTranslation, 1.f));

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

CChannel* CChannel::Create(const aiNodeAnim* pAiChannel, const map<string, _uint>& BoneIndices)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pAiChannel, BoneIndices)))
	{
		MSG_BOX(TEXT("Failed To Created : CChannel"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CChannel* CChannel::Create(const CHANNEL_DESC& ChannelDesc)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(ChannelDesc)))
	{
		MSG_BOX(TEXT("Failed To Created : CChannel"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
	__super::Free();

	m_KeyFrames.clear();
}
