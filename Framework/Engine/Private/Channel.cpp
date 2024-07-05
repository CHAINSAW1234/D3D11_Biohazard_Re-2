#include "Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

CChannel::CChannel(const CChannel& rhs)
	: m_iNumKeyFrames { rhs.m_iNumKeyFrames }
	, m_iBoneIndex { rhs.m_iBoneIndex }
	, m_pKeyFrames { rhs.m_pKeyFrames }
{
	strcpy_s(m_szName, rhs.m_szName);

	m_isCloned = true;
}

HRESULT CChannel::Initialize_Prototype(const aiNodeAnim* pAIChannel, const map<string, _uint>& BoneIndices)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	m_iBoneIndex = BoneIndices.find(m_szName)->second;

	m_iNumKeyFrames = max(max(pAIChannel->mNumPositionKeys, pAIChannel->mNumRotationKeys), pAIChannel->mNumScalingKeys);
	m_pKeyFrames = new vector<KEYFRAME*>();

	_float3			vScale;
	_float4			vRotation;
	_float3			vTranslation;
	_float			fTime;

	for (size_t i = 0; i < m_iNumKeyFrames; ++i)
	{
		KEYFRAME*			pKeyFrame = { new KEYFRAME()};
		ZeroMemory(pKeyFrame, sizeof(KEYFRAME));

		if (i < pAIChannel->mNumScalingKeys)
		{
			memcpy_s(&vScale, sizeof(_float3), &pAIChannel->mScalingKeys[i].mValue, sizeof(aiVector3D));
			fTime = (_float)pAIChannel->mScalingKeys[i].mTime;
		}

		if (i < pAIChannel->mNumRotationKeys)
		{
			//	aiQuaternion은 w, x, y, z순으로 저장이되어 그대로 memcopy하면 안됌
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

		pKeyFrame->vScale = vScale;
		pKeyFrame->vRotation = vRotation;
		pKeyFrame->vTranslation = vTranslation;
		pKeyFrame->fTime = fTime;

		m_pKeyFrames->push_back(pKeyFrame);
	}

	return S_OK;
}

HRESULT CChannel::Initialize_Prototype(const CHANNEL_DESC& ChannelDesc)
{
	strcpy_s(m_szName, ChannelDesc.strName.c_str());

	m_iBoneIndex = ChannelDesc.iBoneIndex;
	m_iNumKeyFrames = ChannelDesc.iNumKeyFrames;
	m_pKeyFrames = new vector<KEYFRAME*>();

	for (auto& KeyFrame : ChannelDesc.KeyFrames)
	{
		KEYFRAME*					pKeyFrame = { new KEYFRAME() };
		memcpy(pKeyFrame, &KeyFrame, sizeof(KEYFRAME));
		m_pKeyFrames->push_back(pKeyFrame);
	}

	return S_OK;
}

HRESULT CChannel::Initialize(const vector<class CBone*> Bones)
{
	_uint			iNumBones = { static_cast<_uint>(Bones.size()) };
	_bool			isFind = { false };
	for (_uint i = 0; i < iNumBones; ++i)
	{
		_char*					pName = { Bones[i]->Get_Name() };
		static const _int		iEquall = { 0 };
		if (iEquall == strcmp(pName, m_szName))
		{
			m_iBoneIndex = i;
			isFind = true;
			break;
		}		
	}

	if (false == isFind)
		m_iBoneIndex = -1;
	
	return S_OK;
}

void CChannel::Invalidate_TransformationMatrix(const vector<class CBone*>& Bones, _float fTrackPosition, _int* pCurrentKeyFrameIndex)
{
	if (0.0f == fTrackPosition)
		(*pCurrentKeyFrameIndex) = 0;

	//	마지막 키프레임을 얻어온다.
	KEYFRAME*		pKeyFrame = m_pKeyFrames->back();

	_vector			vScale, vRotation, vTranslation;

	//	이미 재생 위치가 마지막 키프레임의 위치를 넘어 섰다면..
	//	==> 더 이상의 업데이트가없고 마지막 행렬을 유지하는상태일경우..
	if (pKeyFrame->fTime <= fTrackPosition)
	{
		vScale = XMLoadFloat3(&pKeyFrame->vScale);
		vRotation = XMLoadFloat4(&pKeyFrame->vRotation);
		vTranslation = XMLoadFloat3(&pKeyFrame->vTranslation);
	}
	//	아직 재생위치가 마지막 키프레임에 도달하지 못한 경우
	else
	{
		//	다음 키프레임보다 현재 재생위치가 높거나 같을 경우 현재 키프레임을 다음 키프레임으로 올린다.
		while (fTrackPosition >= (*m_pKeyFrames)[(*pCurrentKeyFrameIndex) + 1]->fTime)
			++(*pCurrentKeyFrameIndex);

		while (fTrackPosition < (*m_pKeyFrames)[*pCurrentKeyFrameIndex]->fTime)
			--(*pCurrentKeyFrameIndex);

		//	Ratio :: 비율 => 선형보간하기위한 수치
		//	(트랙의 현재 위치 - 현재 키프레임의 시작 시간)
		//	-------------------------------------------------------------------------
		//	(다음 키프레임의 시작시간 - 현재 키프레임의 시작 시간) == 현재 키프레임의 구간 

		//	==>>	현재 키프레임 구간의 재생 정도를 얻어낸다
		_float		fRatio = (fTrackPosition - (*m_pKeyFrames)[(*pCurrentKeyFrameIndex)]->fTime)
			/ ((*m_pKeyFrames)[(*pCurrentKeyFrameIndex) + 1]->fTime - (*m_pKeyFrames)[(*pCurrentKeyFrameIndex)]->fTime);

		//	현재 키프레임과 다음 키프레임 사이의 위치에서 위치의 진행 정도에 따라 다음 키프레임과 선형 보간을 한다. 
		//	XMVectorLerp == > Result = V0 + t * (V1 - V0);
		vScale = XMVectorLerp(XMLoadFloat3(&(*m_pKeyFrames)[(*pCurrentKeyFrameIndex)]->vScale),
			XMLoadFloat3(&(*m_pKeyFrames)[(*pCurrentKeyFrameIndex) + 1]->vScale), fRatio);

		//	쿼터니언은 XMQuaternionSlerp 함수를 이용하여 선형보간을 진행한다.
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&(*m_pKeyFrames)[(*pCurrentKeyFrameIndex)]->vRotation),
			XMLoadFloat4(&(*m_pKeyFrames)[(*pCurrentKeyFrameIndex) + 1]->vRotation), fRatio);

		vTranslation = XMVectorLerp(XMLoadFloat3(&(*m_pKeyFrames)[(*pCurrentKeyFrameIndex)]->vTranslation),
			XMLoadFloat3(&(*m_pKeyFrames)[(*pCurrentKeyFrameIndex) + 1]->vTranslation), fRatio);
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

	//	현재 키프레임과 다음 키프레임 사이의 위치에서 위치의 진행 정도에 따라 다음 키프레임과 선형 보간을 한다. 
	//	XMVectorLerp == > Result = V0 + t * (V1 - V0);
	vScale = XMVectorLerp(XMLoadFloat3(&LinearStartKeyFrame.vScale),
		vScale, fRatio);

	//	쿼터니언은 XMQuaternionSlerp 함수를 이용하여 선형보간을 진행한다.
	vRotation = XMQuaternionSlerp(XMLoadFloat4(&LinearStartKeyFrame.vRotation),
		vRotation, fRatio);

	vTranslation = XMVectorLerp(XMLoadFloat3(&LinearStartKeyFrame.vTranslation),
		vTranslation, fRatio);

	TransformationMatrix = XMMatrixAffineTransformation(
		vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, XMVectorSetW(vTranslation, 1.f));

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

_float4x4 CChannel::Compute_TransformationMatrix(_float fTrackPosition, _int* pCurrentKeyFrameIndex)
{
	if (0.0f == fTrackPosition)
		(*pCurrentKeyFrameIndex) = 0;

	//	마지막 키프레임을 얻어온다.
	KEYFRAME*		pKeyFrame = m_pKeyFrames->back();
	_vector			vScale, vRotation, vTranslation;

	//	이미 재생 위치가 마지막 키프레임의 위치를 넘어 섰다면..
	//	==> 더 이상의 업데이트가없고 마지막 행렬을 유지하는상태일경우..
	if (pKeyFrame->fTime <= fTrackPosition)
	{
		vScale = XMLoadFloat3(&pKeyFrame->vScale);
		vRotation = XMLoadFloat4(&pKeyFrame->vRotation);
		vTranslation = XMLoadFloat3(&pKeyFrame->vTranslation);
	}
	//	아직 재생위치가 마지막 키프레임에 도달하지 못한 경우
	else
	{
		//	다음 키프레임보다 현재 재생위치가 높거나 같을 경우 현재 키프레임을 다음 키프레임으로 올린다.
		while (fTrackPosition >= (*m_pKeyFrames)[(*pCurrentKeyFrameIndex) + 1]->fTime)
			++(*pCurrentKeyFrameIndex);

		while (fTrackPosition < (*m_pKeyFrames)[*pCurrentKeyFrameIndex]->fTime)
			--(*pCurrentKeyFrameIndex);

		//	Ratio :: 비율 => 선형보간하기위한 수치
		//	(트랙의 현재 위치 - 현재 키프레임의 시작 시간)
		//	-------------------------------------------------------------------------
		//	(다음 키프레임의 시작시간 - 현재 키프레임의 시작 시간) == 현재 키프레임의 구간 

		//	==>>	현재 키프레임 구간의 재생 정도를 얻어낸다
		_float		fRatio = (fTrackPosition - (*m_pKeyFrames)[(*pCurrentKeyFrameIndex)]->fTime)
			/ ((*m_pKeyFrames)[(*pCurrentKeyFrameIndex) + 1]->fTime - (*m_pKeyFrames)[(*pCurrentKeyFrameIndex)]->fTime);

		//	현재 키프레임과 다음 키프레임 사이의 위치에서 위치의 진행 정도에 따라 다음 키프레임과 선형 보간을 한다. 
		//	XMVectorLerp == > Result = V0 + t * (V1 - V0);
		vScale = XMVectorLerp(XMLoadFloat3(&(*m_pKeyFrames)[(*pCurrentKeyFrameIndex)]->vScale),
			XMLoadFloat3(&(*m_pKeyFrames)[(*pCurrentKeyFrameIndex) + 1]->vScale), fRatio);

		//	쿼터니언은 XMQuaternionSlerp 함수를 이용하여 선형보간을 진행한다.
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&(*m_pKeyFrames)[(*pCurrentKeyFrameIndex)]->vRotation),
			XMLoadFloat4(&(*m_pKeyFrames)[(*pCurrentKeyFrameIndex) + 1]->vRotation), fRatio);

		vTranslation = XMVectorLerp(XMLoadFloat3(&(*m_pKeyFrames)[(*pCurrentKeyFrameIndex)]->vTranslation),
			XMLoadFloat3(&(*m_pKeyFrames)[(*pCurrentKeyFrameIndex) + 1]->vTranslation), fRatio);
	}

	_matrix			TransformationMatrix = XMMatrixAffineTransformation(
		vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, XMVectorSetW(vTranslation, 1.f));

	_float4x4		TransformationFloat4x4;
	XMStoreFloat4x4(&TransformationFloat4x4, TransformationMatrix);

	return TransformationFloat4x4;
}

_float4x4 CChannel::Compute_TransformationMatrix_LinearInterpolation(const vector<_float4x4>& TransformationMatrices, _float fAccLinearInterpolation, _float fTotalLinearTime, const vector<KEYFRAME>& LastKeyFrames)
{
	KEYFRAME		LinearStartKeyFrame = { LastKeyFrames[m_iBoneIndex] };
	_vector			vScale, vRotation, vTranslation;

	_matrix			TransformationMatrix = { XMLoadFloat4x4(&TransformationMatrices[m_iBoneIndex]) };

	XMMatrixDecompose(&vScale, &vRotation, &vTranslation, TransformationMatrix);

	_float			fRatio = { fAccLinearInterpolation / fTotalLinearTime };

	//	현재 키프레임과 다음 키프레임 사이의 위치에서 위치의 진행 정도에 따라 다음 키프레임과 선형 보간을 한다. 
	//	XMVectorLerp == > Result = V0 + t * (V1 - V0);
	vScale = XMVectorLerp(XMLoadFloat3(&LinearStartKeyFrame.vScale),
		vScale, fRatio);

	vScale = vScale;

	//	쿼터니언은 XMQuaternionSlerp 함수를 이용하여 선형보간을 진행한다.
	vRotation = XMQuaternionSlerp(XMLoadFloat4(&LinearStartKeyFrame.vRotation),
		vRotation, fRatio);

	vTranslation = XMVectorLerp(XMLoadFloat3(&LinearStartKeyFrame.vTranslation),
		vTranslation, fRatio);

	TransformationMatrix = XMMatrixAffineTransformation(
		vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, XMVectorSetW(vTranslation, 1.f));

	_float4x4		TransformationFloat4x4;
	XMStoreFloat4x4(&TransformationFloat4x4, TransformationMatrix);

	return TransformationFloat4x4;
}

void CChannel::Link_Bone(const unordered_map<string, _uint>& BoneNameIndices)
{
	unordered_map<string, _uint>::const_iterator				iter = { BoneNameIndices.find(m_szName) };
	if (iter != BoneNameIndices.end())
	{
		m_iBoneIndex = iter->second;
	}

	else
	{
		m_iBoneIndex = -1;
#ifdef _DEBUG
		MSG_BOX(TEXT("Non Included Bone"));
		MSG_BOX(TEXT("Called : void CChannel::Link_Bone(const unordered_map<string, _uint>& BoneNameIndices)"));
#endif
	}
}

CChannel* CChannel::Create(const aiNodeAnim* pAiChannel, const map<string, _uint>& BoneIndices)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize_Prototype(pAiChannel, BoneIndices)))
	{
		MSG_BOX(TEXT("Failed To Created : CChannel"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CChannel* CChannel::Create(const CHANNEL_DESC& ChannelDesc)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize_Prototype(ChannelDesc)))
	{
		MSG_BOX(TEXT("Failed To Created : CChannel"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CChannel* CChannel::Clone(const vector<class CBone*> Bones)
{
	CChannel*				pInstance = { new CChannel(*this) };

	if (FAILED(pInstance->Initialize(Bones)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CChannel"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
	__super::Free();

	if (false == m_isCloned)
	{
		for (auto& pKeyFrame : *m_pKeyFrames)
		{
			Safe_Delete(pKeyFrame);
			pKeyFrame = nullptr;
		}
		m_pKeyFrames->clear();
		Safe_Delete(m_pKeyFrames);
	}
	else
		m_pKeyFrames = nullptr;
}
