#pragma once

#include "Channel.h"

BEGIN(Engine)

class ENGINE_DLL CAnimation final : public CBase
{
public:
	typedef struct tagAnimDesc
	{
		string								strName = { "" };
		_float								fDuration = { 0.f };
		_float								fTickPerSecond = { 0.f };
		_uint								iNumChannels = { 0 };
		vector<CChannel::CHANNEL_DESC>		ChannelDescs;
	}ANIM_DESC;

private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:		/* For.Access */
	_float									Get_Duration() { return m_fDuration; }
	_float									Get_TickPerSec() { return m_fTickPerSecond; }

public:		/* For.Load */
			/* For.FBX_Load*/
	HRESULT									Initialize_Prototype(const aiAnimation* pAIAnimation, const map<string, _uint>& BoneIndices);
			/* For.Binary_Load*/
	HRESULT									Initialize_Prototype(const ANIM_DESC& AnimDesc);
	HRESULT									Initialize_Prototype(const string& strAnimFilePath);

	HRESULT									Initialize(const vector<class CBone*>& Bones);

public:		/* For.PlayAnimation */
	void									Invalidate_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop, _bool* pFirsltTick, class CPlayingInfo* pPlayingInfo);
	void									Invalidate_TransformationMatrix_LinearInterpolation(_float fAccLinearInterpolation, _float fTotalLinearTime, const vector<class CBone*>& Bones, const vector<KEYFRAME>& LastKeyFrames);

	vector<_float4x4>						Compute_TransfromationMatrix(_float fTimeDelta, _uint iNumBones, const unordered_set<_uint>& IncludedBoneIndices, class CPlayingInfo* pPlayingInfo);
	vector<_float4x4>						Compute_TransfromationMatrix_LinearInterpolation(_float fAccLinearInterpolation, _float fTotalLinearTime, vector<_float4x4>& TransformationMatrices, _uint iNumBones, const vector<KEYFRAME>& LastKeyFrames);

public:		/* For.Access */
	const vector<class CChannel*>&			Get_Channels() { return m_Channels; };
	string									Get_Name() { return m_szName; }
	_uint									Get_NumChannel() { return static_cast<_uint>(m_Channels.size()); }

	void									Set_TickPerSec(_float fTickPerSec);

	KEYFRAME								Get_FirstKeyFrame(_uint iBoneIndex);
	KEYFRAME								Get_CurrentKeyFrame(_uint iBoneIndex, _float fTrackPosition);

private:
	_int									Find_ChannelIndex(_uint iBoneIndex);

private:
	HRESULT									Read_Binary(ifstream& ifs);

private:
	_char									m_szName[MAX_PATH] = { "" };

	_float									m_fDuration = { 0.f };			/* 전체 재생 길이 */
	_float									m_fTickPerSecond = { 0.f };		/* 초당 얼마나 재생을 해야하는가 ( 속도 ) */

	_uint									m_iNumChannels = { 0 };
	vector<class CChannel*>					m_Channels;

public:
	/* For.FBX_Load*/
	static CAnimation* Create(const aiAnimation* pAIAnimation, const map<string, _uint>& BoneIndices);
	/* For.Binary_Load From Model */
	static CAnimation* Create(const ANIM_DESC& AnimDesc);

	/* For.Binary_Load Self */
	static CAnimation* Create(const string& strAnimFilePath);
	CAnimation* Clone(const vector<class CBone*>& Bones);
	virtual void Free() override;

#pragma endregion
};

END