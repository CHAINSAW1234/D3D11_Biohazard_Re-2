#include "Animation.h"
#include "Channel.h"
#include "Bone.h"
#include "PlayingInfo.h"

#include "Model_Extractor.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_fDuration{ rhs.m_fDuration }
	, m_fTickPerSecond{ rhs.m_fTickPerSecond }
	, m_iNumChannels{ rhs.m_iNumChannels }
{	
	for (auto& pChannel : rhs.m_Channels)
	{
		m_Channels.emplace_back(pChannel);
		Safe_AddRef(pChannel);
	}
	//  이름 복사
	strcpy_s(m_szName, rhs.m_szName);
}
HRESULT CAnimation::Initialize_Prototype(const aiAnimation* pAIAnimation, const map<string, _uint>& BoneIndices)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_fDuration = (_float)pAIAnimation->mDuration;
	m_fTickPerSecond = 60.f;
	//m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;

	/* 이 애니메이션은 몇 개의 뼈를 컨트롤 하는가 */
	m_iNumChannels = pAIAnimation->mNumChannels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], BoneIndices);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}
	
	return S_OK;
}

HRESULT CAnimation::Initialize_Prototype(const ANIM_DESC& AnimDesc)
{
	strcpy_s(m_szName, AnimDesc.strName.c_str());

	m_fDuration = AnimDesc.fDuration;
	m_fTickPerSecond = 60.f;
	//m_fTickPerSecond = AnimDesc.fTickPerSecond;
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

HRESULT CAnimation::Initialize_Prototype(const string& strAnimFilePath)
{
	filesystem::path			FullPath(strAnimFilePath);

	string						strParentsPath = FullPath.parent_path().string();
	string						strFileName = FullPath.stem().string();

	//	 동일경로에 동일 파일이름에 확장자만 다르게 새로운 경로 생성
	string						strNewPath = strParentsPath + "/" + strFileName + ".bin";

	filesystem::path			CheckPath(strNewPath);

	if (true == filesystem::exists(CheckPath))
	{
		//	바이너리로 데이터를 작성하기위해서 바이너리 플래그를 포함하였다.
		ifstream ifs(strNewPath.c_str(), ios::binary);

		//	첫 번째 인자는 데이터를 저장할 공간의 주소를 가리키는 포인터이고, 두 번째 인자는 읽어올 바이트 수입니다.
		if (true == ifs.fail())
		{
			MSG_BOX(TEXT("Failed To OpenFile"));

			return E_FAIL;
		}

		if (FAILED(Read_Binary(ifs)))
			return E_FAIL;

		ifs.close();

		return S_OK;
	}

	else
	{
		if (FAILED(CModel_Extractor::Extract_FBX_AnimOnly(strAnimFilePath)))
			return E_FAIL;

		if (FAILED(Initialize_Prototype(strAnimFilePath)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CAnimation::Initialize(const vector<class CBone*>& Bones)
{
	for (auto& pChannel : m_Channels)
	{
		CChannel*			pClonedChannel = { pChannel->Clone(Bones) };
		Safe_Release(pChannel);
		pChannel = pClonedChannel;
	}

	for (auto& iter = m_Channels.begin(); iter != m_Channels.end(); )
	{
		if (-1 == (*iter)->Get_BoneIndex())
		{
			Safe_Release(*iter);
			iter = m_Channels.erase(iter);
			m_iNumChannels -= 1;
		}
		else
			++iter;
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

	for (_uint iChannelIndex = 0; iChannelIndex < m_iNumChannels; ++iChannelIndex)
	{
		_int			iBoneIndex = { m_Channels[iChannelIndex]->Get_BoneIndex() };
		if (-1 == iBoneIndex)
			continue;
		_int			iKeyFrameIndex = { pPlayingInfo->Get_KeyFrameIndex(iChannelIndex) };
		if (-1 == iKeyFrameIndex)
			continue;

		/* 이 뼈의 상태행렬을 만들어서 CBone의 TransformationMatrix를 바꿔라. */
		m_Channels[iChannelIndex]->Invalidate_TransformationMatrix(Bones, fTrackPosition, &iKeyFrameIndex);

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

vector<_float4x4> CAnimation::Compute_TransfromationMatrix(_float fTimeDelta, _uint iNumBones, const unordered_set<_uint>& IncludedBoneIndices, const vector<_float4x4>& T_Pose_Matrices, CPlayingInfo* pPlayingInfo)
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
		}

		else
		{
			fTrackPosition = fTrackPosition - m_fDuration;
			pPlayingInfo->Set_FirstTick(true);
			pPlayingInfo->Set_PreAnimIndex(pPlayingInfo->Get_AnimIndex());
			pPlayingInfo->Set_PreAnimLayerTag(pPlayingInfo->Get_AnimLayerTag());
		}
	}

	memcpy(&TransformationMatrices[0], &T_Pose_Matrices[0], sizeof(_float4x4) * iNumBones);

	for (_uint iChannelIndex = 0; iChannelIndex < m_iNumChannels; ++iChannelIndex)
	{
		/* 이 뼈의 상태행렬을 만들어서 CBone의 TransformationMatrix를 바꿔라. */
		_int			iBoneIndex = { m_Channels[iChannelIndex]->Get_BoneIndex() };
		if (-1 == iBoneIndex)
			continue;
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
		_int				iBoneIndex = { m_Channels[i]->Get_BoneIndex() };
		if (-1 == iBoneIndex)
			continue;

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
		_int			iDstBoneIndex = { pChannel->Get_BoneIndex() };
		if (-1 == iDstBoneIndex)
			continue;

		if (static_cast<_uint>(iDstBoneIndex) == iBoneIndex)
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

HRESULT CAnimation::Read_Binary(ifstream& ifs)
{

	_uint			iNumAnimation = {};
	ifs.read(reinterpret_cast<_char*>(&iNumAnimation), sizeof(_char) * sizeof(_uint));

	for (_uint i = 0; i < iNumAnimation; ++i)
	{
		CAnimation::ANIM_DESC		AnimDesc;

		_char		szName[MAX_PATH];
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

		if (FAILED(Initialize_Prototype(AnimDesc)))
			return E_FAIL;
	}	

	return S_OK;
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

	vector<KEYFRAME*>		KeyFrames = m_Channels[static_cast<_uint>(iChannelIndex)]->Get_KeyFrames();

	FirstKeyFrame = { *(KeyFrames.front()) };

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

	vector<KEYFRAME*>		KeyFrames = m_Channels[static_cast<_uint>(iChannelIndex)]->Get_KeyFrames();
	_uint					iNumKeyFrame = { static_cast<_uint>(KeyFrames.size()) };

	for (_uint i = 0; i < iNumKeyFrame - 1; ++i)
	{
		_float				fStartTime = { KeyFrames[i]->fTime };
		_float				fFinishTime = { KeyFrames[i + 1]->fTime };

		if (fStartTime > fTrackPosition || fTrackPosition > fFinishTime)
		{
			if (iNumKeyFrame - 2 == i)
			{
				fTrackPosition = fFinishTime;
			}
			else
				continue;
		}

		_float				fLength = { fFinishTime - fStartTime };
		_float				fRatio = { (fTrackPosition - fStartTime) / fLength };

		_vector				vScale, vQuaternion, vTranslation;
		vScale = XMVectorLerp(XMLoadFloat3(&KeyFrames[i]->vScale),
			XMLoadFloat3(&KeyFrames[i + 1]->vScale), fRatio);

		vQuaternion = XMQuaternionSlerp(XMLoadFloat4(&KeyFrames[i]->vRotation),
			XMLoadFloat4(&KeyFrames[i + 1]->vRotation), fRatio);

		vTranslation = XMVectorLerp(XMLoadFloat3(&KeyFrames[i]->vTranslation),
			XMLoadFloat3(&KeyFrames[i + 1]->vTranslation), fRatio);

		XMStoreFloat3(&CurrentKeyFrame.vScale, vScale);
		XMStoreFloat4(&CurrentKeyFrame.vRotation, vQuaternion);
		XMStoreFloat3(&CurrentKeyFrame.vTranslation, vTranslation);

		break;
	}


	return CurrentKeyFrame;
}

CAnimation* CAnimation::Create(const aiAnimation* pAIAnimation, const map<string, _uint>& BoneIndices)
{
	CAnimation*				pInstance = { new CAnimation() };

	if (FAILED(pInstance->Initialize_Prototype(pAIAnimation, BoneIndices)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Create(const ANIM_DESC& AnimDesc)
{
	CAnimation*				pInstance = { new CAnimation() };

	if (FAILED(pInstance->Initialize_Prototype(AnimDesc)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Create(const string& strAnimFilePath)
{
	CAnimation*				pInstance = { new CAnimation() };

	if (FAILED(pInstance->Initialize_Prototype(strAnimFilePath)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Clone(const vector<class CBone*>& Bones)
{
	CAnimation*				pInstance = { new CAnimation(*this) };

	if (FAILED(pInstance->Initialize(Bones)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CAnimation"));

		Safe_Release(pInstance);
	}

	return pInstance;
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
