#pragma once

#include "Model_Enums.h"

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

	_float					fTrackPosition = { 0.f };
	_bool					isFinished = { false };

	vector<_uint>			CurrentKeyFrameIndices;
	vector<KEYFRAME>		LastKeyFrames;
}ANIM_PLAYING_INFO;

