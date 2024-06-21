#include "Animation.h"
#include "Channel.h"
#include "Bone.h"
#include "PlayingInfo.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_fDuration{ rhs.m_fDuration }
	, m_fTickPerSecond{ rhs.m_fTickPerSecond }
	, m_iNumChannels{ rhs.m_iNumChannels }
{

	//  채널 복사
	for (auto& pChannel : rhs.m_Channels)
	{
		m_Channels.push_back(pChannel);
		Safe_AddRef(pChannel);
	}
	//  이름 복사
	strcpy_s(m_szName, rhs.m_szName);
}
HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const map<string, _uint>& BoneIndices)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_fDuration = (_float)pAIAnimation->mDuration;
	m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;

	/* 이 애니메이션은 몇 개의 뼈를 컨트롤 하는가 */
	m_iNumChannels = pAIAnimation->mNumChannels;

	for (size_t i = 0; i < m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], BoneIndices);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

HRESULT CAnimation::Initialize(const ANIM_DESC& AnimDesc)
{
	strcpy_s(m_szName, AnimDesc.strName.c_str());

	m_fDuration = AnimDesc.fDuration;
	m_fTickPerSecond = AnimDesc.fTickPerSecond;
	m_iNumChannels = AnimDesc.iNumChannels;

	for (size_t i = 0; i < m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(AnimDesc.ChannelDescs[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

void CAnimation::Invalidate_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop, _bool* pFirstTick, CPlayingInfo* pPlayingInfo)
{
	_bool		isFinished = false;
	_float		fTrackPosition = { pPlayingInfo->Get_TrackPosition() };

	if (fTrackPosition == 0.f)
	{
		*pFirstTick = true;
	}

	else
	{
		*pFirstTick = false;
	}

	fTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (m_fDuration <= fTrackPosition)
	{
		if (false == isLoop)
		{
			isFinished = true;
			return;
		}

		fTrackPosition = 0.f;
		*pFirstTick = true;
	}

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		_uint			iBoneIndex = { m_Channels[i]->Get_BoneIndex() };
		_int			iKeyFrameIndex = { pPlayingInfo->Get_KeyFrameIndex(iBoneIndex) };
		if (-1 == iKeyFrameIndex)
			continue;

		/* 이 뼈의 상태행렬을 만들어서 CBone의 TransformationMatrix를 바꿔라. */
		m_Channels[i]->Invalidate_TransformationMatrix(Bones, fTrackPosition, &iKeyFrameIndex);

		pPlayingInfo->Set_KeyFrameIndex(iBoneIndex, iKeyFrameIndex);
	}

	pPlayingInfo->Set_TrackPosition(fTrackPosition);
	pPlayingInfo->Set_Finished(isFinished);
}

void CAnimation::Invalidate_TransformationMatrix_LinearInterpolation(_float fAccLinearInterpolation, _float fTotalLinearTime, const vector<class CBone*>& Bones, const vector<KEYFRAME>& LastKeyFrames)
{
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		m_Channels[i]->Invalidate_TransformationMatrix_LinearInterpolation(Bones, fAccLinearInterpolation, fTotalLinearTime, LastKeyFrames);
	}
}

vector<_float4x4> CAnimation::Compute_TransfromationMatrix(_float fTimeDelta, _uint iNumBones, const unordered_set<_uint>& IncludedBoneIndices, CPlayingInfo* pPlayingInfo)
{
	vector<_float4x4>			TransformationMatrices;
	_bool						isFinished = false;
	_float						fTrackPosition = { pPlayingInfo->Get_TrackPosition() };
	_bool						isLoop = { pPlayingInfo->Is_Loop() };

	TransformationMatrices.resize(iNumBones);

	if (fTrackPosition == 0.f)
	{
		pPlayingInfo->Set_FirstTick(true);
	}

	fTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (m_fDuration <= fTrackPosition)
	{
		if (false == isLoop)
		{
			isFinished = true;
			fTrackPosition = m_fDuration;
			pPlayingInfo->Set_Finished(isFinished);
			pPlayingInfo->Set_TrackPosition(m_fDuration);

			////	루프가아닌 경우 애니메이션 종료 시 마지막 키프레임들을 그대로 반환한다. 
			////	마지막 포즈와 혼합되어야함
			//for(_uint iBoneIndex = 0; iBoneIndex < iNumBones; ++iBoneIndex)
			//{
			//	KEYFRAME		LastKeyFrame = { Get_CurrentKeyFrame(iBoneIndex, m_fDuration) };

			//	_vector			vQuaternion = { XMLoadFloat4(&LastKeyFrame.vRotation) };
			//	_vector			vTranslation = { XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f) };
			//	_vector			vScale = { XMLoadFloat3(&LastKeyFrame.vScale) };

			//	_matrix			TransformationMatrix = { XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vQuaternion, vTranslation) };

			//	TransformationMatrices[iBoneIndex] = TransformationMatrix;
			//}

			//return TransformationMatrices;
		}

		else
		{
			fTrackPosition = fTrackPosition - m_fDuration;
			pPlayingInfo->Set_FirstTick(true);
			pPlayingInfo->Set_PreAnimIndex(pPlayingInfo->Get_AnimIndex());
		}	
	}

	for (auto& TransformationMatrix : TransformationMatrices)
	{
		XMStoreFloat4x4(&TransformationMatrix, XMMatrixIdentity());
	}

	for (_uint iChannelIndex = 0; iChannelIndex < m_iNumChannels; ++iChannelIndex)
	{
		/* 이 뼈의 상태행렬을 만들어서 CBone의 TransformationMatrix를 바꿔라. */
		_uint			iBoneIndex = { m_Channels[iChannelIndex]->Get_BoneIndex() };
		_int			iKeyFrameIndex = { pPlayingInfo->Get_KeyFrameIndex(iChannelIndex) };

		unordered_set<_uint>::iterator		iter = { IncludedBoneIndices.find(iBoneIndex) };
		if (iter == IncludedBoneIndices.end())
			continue;

		if (-1 == iKeyFrameIndex)
			continue;

		_float4x4			TransformationFloat4x4 = { m_Channels[iChannelIndex]->Compute_TransformationMatrix(fTrackPosition, &iKeyFrameIndex) };

		TransformationMatrices[iBoneIndex] = TransformationFloat4x4;
		pPlayingInfo->Set_KeyFrameIndex(iChannelIndex, iKeyFrameIndex);
	}


	pPlayingInfo->Set_TrackPosition(fTrackPosition);
	pPlayingInfo->Set_Finished(isFinished);

	return TransformationMatrices;
}

vector<_float4x4> CAnimation::Compute_TransfromationMatrix_LinearInterpolation(_float fAccLinearInterpolation, _float fTotalLinearTime, vector<_float4x4>& TransformationMatrices, _uint iNumBones, const vector<KEYFRAME>& LastKeyFrames)
{
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		_uint				iBoneIndex = { m_Channels[i]->Get_BoneIndex() };

		_float4x4			TransformationFloat4x4 = { m_Channels[i]->Compute_TransformationMatrix_LinearInterpolation(TransformationMatrices, fAccLinearInterpolation, fTotalLinearTime, LastKeyFrames) };
		TransformationMatrices[iBoneIndex] = TransformationFloat4x4;
	}

	return TransformationMatrices;
}

void CAnimation::Set_TickPerSec(_float fTickPerSec)
{
	if (0.f >= fTickPerSec)
		return;

	m_fTickPerSecond = fTickPerSec;
}

_int CAnimation::Find_ChannelIndex(_uint iBoneIndex)
{
	_int			iChannelIndex = { 0 };
	_bool			isFind = { false };
	for (auto& pChannel : m_Channels)
	{
		_uint			iDstBoneIndex = { pChannel->Get_BoneIndex() };
		
		if (iDstBoneIndex == iBoneIndex)
		{
			isFind = true;

			break;
		}

		++iChannelIndex;
	}

	if (false == isFind)
		iChannelIndex = -1;

	return iChannelIndex;
}

KEYFRAME CAnimation::Get_FirstKeyFrame(_uint iBoneIndex)
{
	KEYFRAME				FirstKeyFrame;
	_int			iChannelIndex = { Find_ChannelIndex(iBoneIndex) };
	if (-1 == iChannelIndex)
	{
		return FirstKeyFrame;
		//	MSG_BOX(TEXT("CAnimation::Get_FirstKeyFrame(_uint iBoneIndex)"));
		//	return FirstKeyFrame;
	}

	vector<KEYFRAME>		KeyFrames = m_Channels[static_cast<_uint>(iChannelIndex)]->Get_KeyFrames();

	FirstKeyFrame = { KeyFrames.front() };

	return FirstKeyFrame;
}

KEYFRAME CAnimation::Get_CurrentKeyFrame(_uint iBoneIndex, _float fTrackPosition)
{
	KEYFRAME				CurrentKeyFrame;
	_int					iChannelIndex = { Find_ChannelIndex(iBoneIndex) };
	if (-1 == iChannelIndex)
	{
		return CurrentKeyFrame;
	}

	vector<KEYFRAME>		KeyFrames = m_Channels[static_cast<_uint>(iChannelIndex)]->Get_KeyFrames();
	_uint					iNumKeyFrame = { static_cast<_uint>(KeyFrames.size()) };

	for (_uint i = 0; i < iNumKeyFrame - 1; ++i)
	{
		_float				fStartTime = { KeyFrames[i].fTime };
		_float				fFinishTime = { KeyFrames[i + 1].fTime };

		if (fStartTime > fTrackPosition && fTrackPosition <= fFinishTime)
			continue;

		_float				fLength = { fFinishTime - fStartTime };
		_float				fRatio = { (fTrackPosition - fStartTime) / fLength };

		_vector				vScale, vQuaternion, vTranslation;
		vScale = XMVectorLerp(XMLoadFloat3(&KeyFrames[i].vScale),
			XMLoadFloat3(&KeyFrames[i + 1].vScale), fRatio);

		vQuaternion = XMQuaternionSlerp(XMLoadFloat4(&KeyFrames[i].vRotation),
			XMLoadFloat4(&KeyFrames[i + 1].vRotation), fRatio);

		vTranslation = XMVectorLerp(XMLoadFloat3(&KeyFrames[i].vTranslation),
			XMLoadFloat3(&KeyFrames[i + 1].vTranslation), fRatio);

		XMStoreFloat3(&CurrentKeyFrame.vScale, vScale);
		XMStoreFloat4(&CurrentKeyFrame.vRotation, vQuaternion);
		XMStoreFloat3(&CurrentKeyFrame.vTranslation, vTranslation);
	}


	return CurrentKeyFrame;
}

CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, const map<string, _uint>& BoneIndices)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, BoneIndices)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Create(const ANIM_DESC& AnimDesc)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(AnimDesc)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	__super::Free();

	for (auto& pChannel : m_Channels)
	{
		Safe_Release(pChannel);
	}

	m_Channels.clear();
}
