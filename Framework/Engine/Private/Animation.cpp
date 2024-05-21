#include "Animation.h"
#include "Channel.h"
#include "Bone.h"

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

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);
}

HRESULT CAnimation::Initialize(const aiAnimation* pAIAnimation, const map<string, _uint>& BoneIndices)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_fDuration = (_float)pAIAnimation->mDuration;
	m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;

	/* 이 애니메이션은 몇 개의 뼈를 컨트롤 하는가 */
	m_iNumChannels = pAIAnimation->mNumChannels;

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

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

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(AnimDesc.ChannelDescs[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

void CAnimation::Invalidate_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop, _bool* pFirstTick)
{
	m_isFinished = false;

	if (m_fTrackPosition == 0.f)
	{
		*pFirstTick = true;
	}
	else
	{
		*pFirstTick = false;
	}

	m_fTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (m_fDuration <= m_fTrackPosition)
	{
		if (false == isLoop)
		{
			m_isFinished = true;
			return;
		}

		m_fTrackPosition = 0.f;
	}

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		/* 이 뼈의 상태행렬을 만들어서 CBone의 TransformationMatrix를 바꿔라. */
		m_Channels[i]->Invalidate_TransformationMatrix(Bones, m_fTrackPosition, &m_CurrentKeyFrameIndices[i]);
	}
}

void CAnimation::Invalidate_TransformationMatrix_LinearInterpolation(_float fAccLinearInterpolation, _float fTotalLinearTime, const vector<class CBone*>& Bones, const vector<KEYFRAME>& LastKeyFrames, _int iRootBoneIndex, _bool isAutoRotate)
{
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		m_Channels[i]->Invalidate_TransformationMatrix_LinearInterpolation(Bones, fAccLinearInterpolation, fTotalLinearTime, LastKeyFrames, iRootBoneIndex, isAutoRotate);
	}
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
