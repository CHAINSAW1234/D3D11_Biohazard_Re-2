#pragma once

#include "Base.h"

BEGIN(Engine)

class CPlayingInfo final : public CBase
{
public:
	typedef struct ENGINE_DLL tagPlayingInfoDesc
	{
		_int					iAnimIndex = { -1 };
		_uint					iNumBones = { 0 };
		_bool					isLoop = { false };
		_float					fBlendWeight = { 0.f };
		wstring					strBoneLayerTag = { TEXT("") };
	}PLAYING_INFO_DESC;

private:
	CPlayingInfo();
	virtual ~CPlayingInfo() = default;

public:
	HRESULT								Initialize(void* pArg);

public:		/* For.Access */
	inline _bool						Is_Loop() { return m_isLoop; }
	inline _bool						Is_Finished() { return m_isFinished; }
	inline _bool						Is_LinearInterpolation() { return m_isLinearInterpolation; }

	inline _int							Get_PreAnimIndex() { return m_iPreAnimIndex; }
	inline _int							Get_AnimIndex() { return m_iAnimIndex; }
	inline _float						Get_TrackPosition() { return m_fTrackPosition; }
	inline _float						Get_BlendWeight() { return m_fBlendWeight; }
	inline _float						Get_AccLinearInterpolation() { return m_fAccLinearInterpolation; }
	inline vector<_uint>				Get_KeyFrameIndices() { return m_CurrentKeyFrameIndices; }
	inline _int							Get_KeyFrameIndex(_uint iBoneIndex);
	inline _float3						Get_PreTranslation_Local() { return m_vPreTranslationLocal; }
	inline _float3						Get_PreQuaternion() { return m_vPreQuaternion; }
	inline wstring						Get_BoneLayerTag() { return m_strBoneLayerTag; }
	inline const vector<KEYFRAME>&		Get_LastKeyFrames() { return m_LastKeyFrames; }
	KEYFRAME							Get_LastKeyFrame(_uint iKeyFrameIndex);

	void								Change_Animation(_uint iAnimIndex);

	inline void							Set_Loop(_bool isLoop) { m_isLoop = isLoop; }
	inline void							Set_BlendWeight(_float fBlendWeight) { m_fBlendWeight = fBlendWeight; }
	inline void							Set_TrackPosition(_float fTrackPosition) { m_fTrackPosition = fTrackPosition; }
	inline void							Set_LinearInterpolation(_bool isLinearInterPolation) { m_isLinearInterpolation = isLinearInterPolation; }
	inline void							Set_AccLinearInterpolation(_float fAccLinear) { m_fAccLinearInterpolation = fAccLinear; }
	inline void							Set_BoneLayerTag(const wstring& strBoneLayerTag) { m_strBoneLayerTag = strBoneLayerTag; }
	inline void							Set_Finished(_bool isFinished) { m_isFinished = isFinished; }

	inline void							Set_PreTranslation(_float3 PreTranslation);
	inline void							Set_PreQuaternion(_float4 vPreQuaternion);

	void								Set_KeyFrameIndex_AllKeyFrame(_uint iKeyFrameIndex);
	void								Set_KeyFrameIndex(_uint iBoneIndex, _uint iKeyFrameIndex);

	inline void							Set_LastKeyFrame_Translation(_uint iKeyFrameIndex, _float3 vTranslation);
	inline void							Set_LastKeyFrame_Rotation(_uint iKeyFrameIndex, _float4 vQuaternion);
	inline void							Set_LastKeyFrame_Scale(_uint iKeyFrameIndex, _float3 vScale);

	inline void							Reset_TrackPosition() { m_fTrackPosition = 0.f; }
	inline void							Reset_Finished() { m_isFinished = false; }
	void								Reset_LinearInterpolation();

	inline void							Add_TrackPosition(_float fAddTrackPosition);
	inline void							Add_AccLinearInterpolation(_float fAddLinearInterpolation);

	void								Update_LastKeyFrames(const vector<_float4x4>& TransformationMatrices, _uint iNumBones, _float fTotalLinearTime);

private:
	_bool								m_isLoop = { false };
	_int								m_iPreAnimIndex = { -1 };
	_int								m_iAnimIndex = { -1 };

	_float								m_fBlendWeight = { 0.f };
	wstring								m_strBoneLayerTag = { TEXT("") };

	_bool								m_isLinearInterpolation = { false };
	_float								m_fAccLinearInterpolation = { 0.f };
	_float3								m_vPreTranslationLocal = { 0.f, 0.f, 0.f };
	_float4								m_vPreQuaternion = { 0.f, 0.f, 0.f, 0.f };

	_float								m_fTrackPosition = { 0.f };
	_bool								m_isFinished = { false };

	vector<_uint>						m_CurrentKeyFrameIndices;
	vector<KEYFRAME>					m_LastKeyFrames;

public:
	static CPlayingInfo* Create(void* pArg);
	virtual void Free() override;
};

END