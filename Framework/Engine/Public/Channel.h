#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
public:
	typedef struct tagChannelDesc
	{
		string						strName = { "" };
		_uint						iBoneIndex = { 0 };
		_uint						iNumKeyFrames = { 0 };
		vector<KEYFRAME>			KeyFrames;
	}CHANNEL_DESC;

private:
	CChannel();
	virtual ~CChannel() = default;

public:
	/* For.FBX_Load*/
	HRESULT Initialize(const aiNodeAnim* pAIChannel, const map<string, _uint>& BoneIndices);
	/* For.Binary_Load*/
	HRESULT Initialize(const CHANNEL_DESC& ChannelDesc);
	void Invalidate_TransformationMatrix(const vector<class CBone*>& Bones, _float fTrackPosition, _uint* pCurrentKeyFrameIndex);
	void Invalidate_TransformationMatrix_LinearInterpolation(const vector<class CBone*>& Bones, _float fAccLinearInterpolation, _float fTotalLinearTime, const vector<KEYFRAME>& LastKeyFrames, _int iRootBoneIndex, _bool isAutoRotate = false);

public:
	const vector<KEYFRAME>& Get_KeyFrames() {
		return m_KeyFrames;
	}
	_uint Get_BoneIndex() { return m_iBoneIndex; }

private:
	_char					m_szName[MAX_PATH] = { "" };
	_int					m_iBoneIndex = { -1 };
	_uint					m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>		m_KeyFrames;

public:
	/* For.FBX_Load*/
	static CChannel* Create(const aiNodeAnim* pAiChannel, const map<string, _uint>& BoneIndices);
	/* For.Binary_Load*/
	static CChannel* Create(const CHANNEL_DESC& ChannelDesc);
	virtual void Free() override;

};

END