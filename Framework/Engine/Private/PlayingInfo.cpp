#include "PlayingInfo.h"

HRESULT CPlayingInfo::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	PLAYING_INFO_DESC*		pDesc = { static_cast<PLAYING_INFO_DESC*> (pArg) };

	m_iAnimIndex = pDesc->iAnimIndex;
	m_isLoop = pDesc->isLoop;
	m_fBlendWeight = pDesc->fBlendWeight;
	m_strBoneLayerTag = pDesc->strBoneLayerTag;
	m_iPreAnimIndex = -1;
	
	m_LastKeyFrames.resize(pDesc->iNumBones);

	return S_OK;
}

inline _int CPlayingInfo::Get_KeyFrameIndex(_uint iBoneIndex)
{
	_uint		iNumKeyFrame = { static_cast<_uint>(m_CurrentKeyFrameIndices.size()) };
	if (iBoneIndex >= iNumKeyFrame)
		return -1;

	return m_CurrentKeyFrameIndices[iBoneIndex];
}

KEYFRAME CPlayingInfo::Get_LastKeyFrame(_uint iKeyFrameIndex)
{
	_uint			iNumKeyFrame = { static_cast<_uint>(m_LastKeyFrames.size()) };
	KEYFRAME		KeyFrame;

	if (iNumKeyFrame > iKeyFrameIndex)
	{
		KeyFrame = m_LastKeyFrames[iKeyFrameIndex];
	}

	return KeyFrame;
}

void CPlayingInfo::Change_Animation(_uint iAnimIndex)
{
	m_iPreAnimIndex = m_iAnimIndex;
	m_iAnimIndex = iAnimIndex;

	Reset_TrackPosition();
	Reset_Finished();
}

inline void CPlayingInfo::Set_PreTranslation(_float3 vPreTranslation)
{
	m_vPreTranslationLocal = vPreTranslation;
}

inline void CPlayingInfo::Set_PreQuaternion(_float4 vPreQuaternion)
{
	m_vPreQuaternion = vPreQuaternion;
}

void CPlayingInfo::Set_KeyFrameIndex_AllKeyFrame(_uint iKeyFrameIndex)
{
	for (auto& iIndex : m_CurrentKeyFrameIndices)
	{
		iIndex = iKeyFrameIndex;
	}
}

void CPlayingInfo::Set_KeyFrameIndex(_uint iBoneIndex, _uint iKeyFrameIndex)
{
	_uint			iNumKeyFrame = { static_cast<_uint>(m_CurrentKeyFrameIndices.size()) };
	if (iNumKeyFrame <= iBoneIndex)
		return;

	m_CurrentKeyFrameIndices[iBoneIndex] = iKeyFrameIndex;
}

inline void CPlayingInfo::Set_LastKeyFrame_Translation(_uint iKeyFrameIndex, _float3 vTranslation)
{
	_uint			iNumKeyFrames = { static_cast<_uint>(m_LastKeyFrames.size()) };

	if (iNumKeyFrames > iKeyFrameIndex)
	{
		XMStoreFloat3(&m_LastKeyFrames[iKeyFrameIndex].vTranslation, vTranslation);
	}
}

inline void CPlayingInfo::Set_LastKeyFrame_Rotation(_uint iKeyFrameIndex, _float4 vQuaternion)
{
	_uint			iNumKeyFrames = { static_cast<_uint>(m_LastKeyFrames.size()) };

	if (iNumKeyFrames > iKeyFrameIndex)
	{
		XMStoreFloat4(&m_LastKeyFrames[iKeyFrameIndex].vRotation, vQuaternion);
	}
}

inline void CPlayingInfo::Set_LastKeyFrame_Scale(_uint iKeyFrameIndex, _float3 vScale)
{
	_uint			iNumKeyFrames = { static_cast<_uint>(m_LastKeyFrames.size()) };

	if (iNumKeyFrames > iKeyFrameIndex)
	{
		XMStoreFloat3(&m_LastKeyFrames[iKeyFrameIndex].vScale, vScale);
	}
}

void CPlayingInfo::Reset_LinearInterpolation()
{
	m_isLinearInterpolation = false;
	m_fAccLinearInterpolation = 0.f;
}

inline void CPlayingInfo::Add_TrackPosition(_float fAddTrackPosition)
{
	m_fTrackPosition += fAddTrackPosition;
}

inline void CPlayingInfo::Add_AccLinearInterpolation(_float fAddLinearInterpolation)
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
	CPlayingInfo*		pInstance = { new CPlayingInfo() };
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
