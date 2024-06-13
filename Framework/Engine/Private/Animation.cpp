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

	//  ä�� ����
	for (auto& pChannel : rhs.m_Channels)
	{
		m_Channels.push_back(pChannel);
		Safe_AddRef(pChannel);
	}
	//  �̸� ����
	strcpy_s(m_szName, rhs.m_szName);
}
HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const map<string, _uint>& BoneIndices)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_fDuration = (_float)pAIAnimation->mDuration;
	m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;

	/* �� �ִϸ��̼��� �� ���� ���� ��Ʈ�� �ϴ°� */
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

		/* �� ���� ��������� ���� CBone�� TransformationMatrix�� �ٲ��. */
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

vector<_float4x4> CAnimation::Compute_TransfromationMatrix(_float fTimeDelta, _uint iNumBones, const set<_uint>& IncludedBoneIndices, _bool* pFirstTick, CPlayingInfo* pPlayingInfo)
{
	vector<_float4x4>			TransformationMatrices;
	_bool						isFinished = false;
	_float						fTrackPosition = { pPlayingInfo->Get_TrackPosition() };
	_bool						isLoop = { pPlayingInfo->Is_Loop() };

	TransformationMatrices.resize(iNumBones);

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
			pPlayingInfo->Set_Finished(isFinished);

			vector<KEYFRAME>			LastKeyFrames = { pPlayingInfo->Get_LastKeyFrames() };

			for(_uint iBoneIndex = 0; iBoneIndex < iNumBones; ++iBoneIndex)
			{
				KEYFRAME		LastKeyFrame = { LastKeyFrames[iBoneIndex] };

				_vector			vQuaternion = { XMLoadFloat4(&LastKeyFrame.vRotation) };
				_vector			vTranslation = { XMVectorSetW(XMLoadFloat3(&LastKeyFrame.vTranslation), 1.f) };
				_vector			vScale = { XMLoadFloat3(&LastKeyFrame.vScale) };

				_matrix			TransformationMatrix = { XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vQuaternion, vTranslation) };

				TransformationMatrices[iBoneIndex] = TransformationMatrix;
			}

			return TransformationMatrices;
		}

		else
		{
			fTrackPosition = fTrackPosition - m_fDuration;
			*pFirstTick = true;
			pPlayingInfo->Set_PreAnimIndex(pPlayingInfo->Get_AnimIndex());
		}	
	}

	for (auto& TransformationMatrix : TransformationMatrices)
	{
		XMStoreFloat4x4(&TransformationMatrix, XMMatrixIdentity());
	}

	for (_uint iChannelIndex = 0; iChannelIndex < m_iNumChannels; ++iChannelIndex)
	{
		/* �� ���� ��������� ���� CBone�� TransformationMatrix�� �ٲ��. */
		_uint			iBoneIndex = { m_Channels[iChannelIndex]->Get_BoneIndex() };
		_int			iKeyFrameIndex = { pPlayingInfo->Get_KeyFrameIndex(iChannelIndex) };

		set<_uint>::iterator		iter = { IncludedBoneIndices.find(iBoneIndex) };
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
