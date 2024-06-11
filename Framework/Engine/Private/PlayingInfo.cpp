#include "PlayingInfo.h"

CPlayingInfo::CPlayingInfo()
{
}

HRESULT CPlayingInfo::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	PLAYING_INFO_DESC* pDesc = { static_cast<PLAYING_INFO_DESC*> (pArg) };

	m_iAnimIndex = pDesc->iAnimIndex;
	m_isLoop = pDesc->isLoop;
	m_fBlendWeight = pDesc->fBlendWeight;
	m_strBoneLayerTag = pDesc->strBoneLayerTag;
	m_iPreAnimIndex = -1;

	m_LastKeyFrames.resize(pDesc->iNumBones);
	m_CurrentKeyFrameIndices.resize(pDesc->iNumChannel);

	return S_OK;
}

_int CPlayingInfo::Get_KeyFrameIndex(_uint iChannelIndex)
{
	_uint		iNumKeyFrame = { static_cast<_uint>(m_CurrentKeyFrameIndices.size()) };
	if (iChannelIndex >= iNumKeyFrame)
		return -1;

	return m_CurrentKeyFrameIndices[iChannelIndex];
}

KEYFRAME CPlayingInfo::Get_LastKeyFrame(_uint iBoneIndex)
{
	KEYFRAME KeyFrame = m_LastKeyFrames[iBoneIndex];

	return KeyFrame;
}

void CPlayingInfo::Change_Animation(_uint iAnimIndex, _uint iNumChannel)
{
	if (m_iAnimIndex == iAnimIndex)
		return;

	m_iPreAnimIndex = m_iAnimIndex;
	m_iAnimIndex = iAnimIndex;

	Reset_CurrentKeyFrameIndices(iNumChannel);
	Reset_TrackPosition();
	Reset_Finished();
}

void CPlayingInfo::Set_PreTranslation(_fvector vPreTranslation)
{
	XMStoreFloat3(&m_vPreTranslationLocal, vPreTranslation);
}

void CPlayingInfo::Set_PreQuaternion(_fvector vPreQuaternion)
{
	XMStoreFloat4(&m_vPreQuaternion, vPreQuaternion);
}

void CPlayingInfo::Set_KeyFrameIndex_AllKeyFrame(_uint iKeyFrameIndex)
{
	for (auto& iIndex : m_CurrentKeyFrameIndices)
	{
		iIndex = iKeyFrameIndex;
	}
}

void CPlayingInfo::Set_KeyFrameIndex(_uint iChannelIndex, _uint iKeyFrameIndex)
{
	_uint			iNumChannel = { static_cast<_uint>(m_CurrentKeyFrameIndices.size()) };
	if (iNumChannel <= iChannelIndex)
		return;

	m_CurrentKeyFrameIndices[iChannelIndex] = iKeyFrameIndex;
}

void CPlayingInfo::Set_LastKeyFrame_Translation(_uint iBoneIndex, _fvector vTranslation)
{
	_uint			iNumBones = { static_cast<_uint>(m_LastKeyFrames.size()) };

	if (iNumBones > iBoneIndex)
	{
		XMStoreFloat3(&m_LastKeyFrames[iBoneIndex].vTranslation, vTranslation);
	}
}

void CPlayingInfo::Set_LastKeyFrame_Rotation(_uint iBoneIndex, _fvector vQuaternion)
{
	_uint			iNumBones = { static_cast<_uint>(m_LastKeyFrames.size()) };

	if (iNumBones > iBoneIndex)
	{
		XMStoreFloat4(&m_LastKeyFrames[iBoneIndex].vRotation, vQuaternion);
	}
}

void CPlayingInfo::Set_LastKeyFrame_Scale(_uint iBoneIndex, _fvector vScale)
{
	_uint			iNumBones = { static_cast<_uint>(m_LastKeyFrames.size()) };

	if (iNumBones > iBoneIndex)
	{
		XMStoreFloat3(&m_LastKeyFrames[iBoneIndex].vScale, vScale);
	}
}

void CPlayingInfo::Reset_CurrentKeyFrameIndices(_uint iNumChannel)
{
	m_CurrentKeyFrameIndices.clear();
	m_CurrentKeyFrameIndices.resize(iNumChannel);
}

void CPlayingInfo::Reset_LinearInterpolation()
{
	m_isLinearInterpolation = false;
	m_fAccLinearInterpolation = 0.f;
}

void CPlayingInfo::Add_TrackPosition(_float fAddTrackPosition)
{
	m_fTrackPosition += fAddTrackPosition;
}

void CPlayingInfo::Add_AccLinearInterpolation(_float fAddLinearInterpolation)
{
	m_fAccLinearInterpolation += fAddLinearInterpolation;
}

void CPlayingInfo::Update_LastKeyFrames(const vector<_float4x4>& TransformationMatrices, _uint iNumBones, _float fTotalLinearTime)
{
	m_LastKeyFrames.clear();

	for (_uint iBoneIndex = 0; iBoneIndex < iNumBones; ++iBoneIndex)
	{
		_vector			vScale, vRotation, vTranslation;
		KEYFRAME		KeyFrame;
		_matrix			TransformationMatrix = { XMLoadFloat4x4(&TransformationMatrices[iBoneIndex]) };

		XMMatrixDecompose(&vScale, &vRotation, &vTranslation, TransformationMatrix);

		XMStoreFloat3(&KeyFrame.vScale, vScale);
		XMStoreFloat4(&KeyFrame.vRotation, vRotation);
		XMStoreFloat3(&KeyFrame.vTranslation, vTranslation);
		KeyFrame.fTime = fTotalLinearTime;

		m_LastKeyFrames.push_back(KeyFrame);
	}
}

CPlayingInfo* CPlayingInfo::Create(void* pArg)
{
	CPlayingInfo* pInstance = { new CPlayingInfo() };
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayingInfo"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayingInfo::Free()
{
	__super::Free();
}
