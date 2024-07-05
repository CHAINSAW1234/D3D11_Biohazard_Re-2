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
	CChannel(const CChannel& rhs);
	virtual ~CChannel() = default;

public:
	/* For.FBX_Load*/
	HRESULT									Initialize_Prototype(const aiNodeAnim* pAIChannel, const map<string, _uint>& BoneIndices);
	/* For.Binary_Load*/
	HRESULT									Initialize_Prototype(const CHANNEL_DESC& ChannelDesc);
	HRESULT									Initialize(const vector<class CBone*> Bones);

public:
	void									Invalidate_TransformationMatrix(const vector<class CBone*>& Bones, _float fTrackPosition, _int* pCurrentKeyFrameIndex);
	void									Invalidate_TransformationMatrix_LinearInterpolation(const vector<class CBone*>& Bones, _float fAccLinearInterpolation, _float fTotalLinearTime, const vector<KEYFRAME>& LastKeyFrames);

	_float4x4								Compute_TransformationMatrix(_float fTrackPosition, _int* pCurrentKeyFrameIndex);
	_float4x4								Compute_TransformationMatrix_LinearInterpolation(const vector<_float4x4>& TransformationMatrices, _float fAccLinearInterpolation, _float fTotalLinearTime, const vector<KEYFRAME>& LastKeyFrames);

public:
	inline const vector<KEYFRAME*>&			Get_KeyFrames() { return *m_pKeyFrames; }
	inline _int								Get_BoneIndex() { return m_iBoneIndex; }
	inline _uint							Get_Num_KeyFrame() { return m_iNumKeyFrames; }

public:
	void					Link_Bone(const unordered_map<string, _uint>& BoneNameIndices);

private:
	_char									m_szName[MAX_PATH] = { "" };
	_int									m_iBoneIndex = { -1 };
	_uint									m_iNumKeyFrames = { 0 };
	vector<KEYFRAME*>*						m_pKeyFrames = { nullptr };

	_bool									m_isCloned = { false };

public:
	/* For.FBX_Load*/
	static CChannel* Create(const aiNodeAnim* pAiChannel, const map<string, _uint>& BoneIndices);
	/* For.Binary_Load*/
	static CChannel* Create(const CHANNEL_DESC& ChannelDesc);
	CChannel* Clone(const vector<class CBone*> Bones);
	virtual void Free() override;

};

END