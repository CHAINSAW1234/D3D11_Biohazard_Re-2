#pragma once

typedef struct tagAnimPlayingDesc
{
	_int					iAnimIndex = { -1 };
	_bool					isLoop = { false };
	_float					fWeight = { 0.f };
	wstring					strBoneLayerTag;
}ANIM_PLAYING_DESC;

typedef struct tagAnimPlayingInfo : public ANIM_PLAYING_DESC
{
	_int					iPreAnimIndex = { -1 };
	_bool					isLinearInterpolation = { false };
	_float					fAccLinearInterpolation = { 0.f };
	_float3					vPreTranslationLocal = { 0.f, 0.f, 0.f };
	_float4					vPreQuaternion = { 0.f, 0.f, 0.f, 0.f };
	vector<KEYFRAME>		LastKeyFrames;
}ANIM_PLAYING_INFO;

typedef struct tagIK_Info
{
	_int					iEndEffectorIndex = { -1 };
	_int					iIKRootBoneIndex = { -1 };
	vector<_uint>			JointIndices;
	_float3					vIKDirection = {};
	list<_uint>				IKChildIndices;

	_uint					iNumIteration = { 0 };

	_float					fBlend = { 0.f };

	_float4					vTargetJointStartTranslation;
	_float4					vEndEffectorResultPosition;

	vector<_float>			TargetDistancesToChild;
	vector<_float>			TargetDistancesToParrent;
	vector<_float4>			BoneTranslations;
	vector<_float4>			BoneTranslationsOrigin;
}IK_INFO;