#pragma once

#include "Base.h"

BEGIN(Engine)

class CPlayingInfo final : public CBase
{
public:
	typedef struct ENGINE_DLL tagPlayingInfoDesc
	{
		_int							iAnimIndex = { -1 };
		_uint							iNumChannel = { 0 }; 
		_uint							iNumBones = { 0 };
		_bool							isLoop = { false };
		_float							fBlendWeight = { 0.f };
		wstring							strBoneLayerTag = { TEXT("") };
	}PLAYING_INFO_DESC;

private:
	CPlayingInfo();
	virtual ~CPlayingInfo() = default;

public:
	HRESULT								Initialize(void* pArg);

public:		/* For.Access */
	inline _bool						Is_Loop() { return m_isLoop; }
	inline _bool						Is_Finished() { return m_isFinished; }
	inline _bool						Is_FirstTick() { return m_isFirstTick; }
	inline _bool						Is_LinearInterpolation() { return m_isLinearInterpolation; }
	inline _bool						Is_AdditionalMasking() { return m_isAdditionalMasking; }

	inline _int							Get_PreAnimIndex() { return m_iPreAnimIndex; }
	inline _int							Get_AnimIndex() { return m_iAnimIndex; }
	inline wstring						Get_AnimLayerTag() { return m_strAnimLayerTag; }
	inline _float						Get_TrackPosition() { return m_fTrackPosition; }
	inline _float						Get_BlendWeight() { return m_fBlendWeight; }
	inline _float						Get_AccLinearInterpolation() { return m_fAccLinearInterpolation; }
	inline vector<_uint>				Get_KeyFrameIndices() { return m_CurrentKeyFrameIndices; }
	_int								Get_KeyFrameIndex(_uint iChannelIndex);
	inline _float3						Get_PreTranslation_Local() { return m_vPreTranslationLocal; }
	inline _float4						Get_PreQuaternion() { return m_vPreQuaternion; }
	inline _float3						Get_CurrentTranslation_Local() { return m_vCurrentTranslationLocal; }
	inline _float4						Get_CurrentQuaternion() { return m_vCurrentQuaternion; }
	inline wstring						Get_BoneLayerTag() { return m_strBoneLayerTag; }
	inline const vector<KEYFRAME>&		Get_LastKeyFrames() { return m_LastKeyFrames; }
	inline const vector<KEYFRAME>&		Get_LinearStartKeyFrames() { return m_LinearStartKeyFrames; }
	KEYFRAME							Get_LastKeyFrame(_uint iBoneIndex);
	_bool								Is_ResetRootPre() { return m_isResetRootPre; }

	_bool								Is_Set_CurrentAnimation() { return m_iAnimIndex != -1 && m_strAnimLayerTag != TEXT(""); }

	inline _bool						Is_Enough_NumKeyFrame(_uint iNumKeyFrame) { return iNumKeyFrame >= m_iNumNeedKeyFrame; }

	void								Set_Root_Pre(_bool isResetRootPre) { m_isResetRootPre = isResetRootPre; }

	void								Change_Animation(const wstring& strAnimLayerTag, _uint iAnimIndex, _uint iNumChannel);

	void								Reset_PreAnim_CurrentAnim();

	inline void							Set_Additional_Masking(_bool isAdditionalMasking, _uint iNumNeedKeyFrame) { m_isAdditionalMasking = isAdditionalMasking; m_iNumNeedKeyFrame = iNumNeedKeyFrame; }
	inline void							Set_PreAnimIndex(_int iAnimIndex) { m_iPreAnimIndex = iAnimIndex; }
	inline void							Set_PreAnimLayerTag(const wstring& strPreAnimLayerTag) { m_strPreAnimLayerTag = strPreAnimLayerTag; }
	inline void							Set_Loop(_bool isLoop) { m_isLoop = isLoop; }
	void								Set_BlendWeight(_float fBlendWeight, _float fBlendLinearTime = 0.f);
	void								Set_TrackPosition(_float fTrackPosition);
	void								Set_TrackPosition_ResetRootPre(_float fTrackPosition);
	inline void							Set_LinearInterpolation(_bool isLinearInterPolation) { m_isLinearInterpolation = isLinearInterPolation; }
	inline void							Set_AccLinearInterpolation(_float fAccLinear) { m_fAccLinearInterpolation = fAccLinear; }
	inline void							Set_BoneLayerTag(const wstring& strBoneLayerTag) { m_strBoneLayerTag = strBoneLayerTag; }
	inline void							Set_Finished(_bool isFinished) { m_isFinished = isFinished; }
	inline void							Set_FirstTick(_bool isFirstTick) { m_isFirstTick = isFirstTick; }

	void								Set_PreTranslation(_fvector vPreTranslation);
	void								Set_PreQuaternion(_fvector vPreQuaternion);
	void								Set_CurrentTranslation(_fvector vCurrentTranslation);
	void								Set_CurrentQuaternion(_fvector vCurrentQuaternion);

	void								Set_KeyFrameIndex_AllKeyFrame(_uint iKeyFrameIndex);
	void								Set_KeyFrameIndex(_uint iChannelIndex, _uint iKeyFrameIndex);

	void								Set_LastKeyFrame_Translation(_uint iBoneIndex, _fvector vTranslation);
	void								Set_LastKeyFrame_Rotation(_uint iBoneIndex, _fvector vQuaternion);
	void								Set_LastKeyFrame_Scale(_uint iBoneIndex, _fvector vScale);

	inline void							Reset_TrackPosition() { m_fTrackPosition = 0.f; }
	inline void							Reset_Finished() { m_isFinished = false; }
	void								Reset_CurrentKeyFrameIndices(_uint iNumChannel);
	void								Reset_LinearInterpolation();

	void								Add_TrackPosition(_float fAddTrackPosition);
	void								Add_AccLinearInterpolation(_float fAddLinearInterpolation);

	void								Update_LastKeyFrames(const vector<_float4x4>& TransformationMatrices, _uint iNumBones, _float fTotalLinearTime);
	void								Update_LinearStateKeyFrames(const vector<KEYFRAME>& KeyFrames);

	void								Update_BlendWeight_Linear(_float fTimeDelta);

private:
	_bool								m_isLoop = { false };
	_int								m_iPreAnimIndex = { -1 };
	_int								m_iAnimIndex = { -1 };

	wstring								m_strAnimLayerTag = { TEXT("") };
	wstring								m_strPreAnimLayerTag = { TEXT("") };

	_float								m_fBlendWeight = { 0.f };
	_float								m_fStartBlendWeight = { 0.f };
	_float								m_fTargetBlendWeight = { 0.f };
	_float								m_fAccLinearTime_Blend = { 0.f };
	_float								m_fTotalLinearTime_Blend = { 0.f };

	wstring								m_strBoneLayerTag = { TEXT("") };

	_bool								m_isLinearInterpolation = { false };
	_float								m_fAccLinearInterpolation = { 0.f };

	_float3								m_vCurrentTranslationLocal = { 0.f, 0.f, 0.f };
	_float4								m_vCurrentQuaternion = { 0.f, 0.f, 0.f, 0.f };

	_float3								m_vPreTranslationLocal = { 0.f, 0.f, 0.f };
	_float4								m_vPreQuaternion = { 0.f, 0.f, 0.f, 0.f };

	_float								m_fTrackPosition = { 0.f };
	_bool								m_isFinished = { true };

	_bool								m_isResetRootPre = { false };

	_bool								m_isFirstTick = { false };

	vector<_uint>						m_CurrentKeyFrameIndices;
	vector<KEYFRAME>					m_LastKeyFrames;
	vector<KEYFRAME>					m_LinearStartKeyFrames;

	_bool								m_isAdditionalMasking = { false };
	_uint								m_iNumNeedKeyFrame = { 4 };

public:
	static CPlayingInfo* Create(void* pArg);
	virtual void Free() override;
};

END