#pragma once

#include "Channel.h"

/* 하나의 애니메이션이 몇개의 뼈를 컨트롤해야하는지... 그 뼈들은 무엇인지.. */
/* 이 애니메이션을 재생하는데 걸리는 총 거리... */
/* 이 애니메이션을 구동하는 속도. */

BEGIN(Engine)

class CAnimation final : public CBase
{
public:
	typedef struct tagAnimDesc
	{
		string			strName = { "" };
		_float			fDuration = { 0.f };
		_float			fTickPerSecond = { 0.f };
		_uint			iNumChannels = { 0 };
		vector<CChannel::CHANNEL_DESC>		ChannelDescs;
	}ANIM_DESC;

private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	_bool isFinished() const {
		return m_isFinished;
	}

	void Reset_Finish();

	_float Get_TrackPosition() { return m_fTrackPosition; }
	_float Get_Duration() { return m_fDuration; }
	void Reset_TrackPostion() {
		m_fTrackPosition = 0.f;
		for (auto& Index : m_CurrentKeyFrameIndices)
		{
			Index = (_uint)0;
		}
	}

public:
	/* For.FBX_Load*/
	HRESULT Initialize(const aiAnimation* pAIAnimation, const map<string, _uint>& BoneIndices);
	/* For.Binary_Load*/
	HRESULT Initialize(const ANIM_DESC& AnimDesc);
	void Invalidate_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop, _bool* pFirsltTick);
	void Invalidate_TransformationMatrix_LinearInterpolation(_float fAccLinearInterpolation, _float fTotalLinearTime, const vector<class CBone*>& Bones, const vector<KEYFRAME>& LastKeyFrames, _int iRootBoneIndex = -1, _bool isAutoRotate = false);

public:
	const vector<class CChannel*>& Get_Channels() {
		return m_Channels;
	};

	const vector<_uint>& Get_CurrentKeyFrameIndices() {
		return m_CurrentKeyFrameIndices;
	}

	void Set_KeyFrameIndex(_uint iKeyFrameIndex)
	{
		for (auto& iIndex : m_CurrentKeyFrameIndices)
			iIndex = iKeyFrameIndex;
	}

	void Set_TrackPosition(_float fTrackPostion)
	{
		m_fTrackPosition = fTrackPostion;
	}

	void Set_TickPerSec(_float fTickPerSec);

private:
	_char							m_szName[MAX_PATH] = { "" };

	_float							m_fDuration = { 0.f };			/* 전체 재생 길이*/
	_float							m_fTickPerSecond = { 0.f };		/* 초당 얼마나 재생을 해야하는가 ( 속도 ) */
	_float							m_fTrackPosition = { 0.f };		/* 현재 애니메이션이 어디까지 재생되었는지 */

	_uint							m_iNumChannels = { 0 };
	vector<class CChannel*>			m_Channels;
	vector<_uint>					m_CurrentKeyFrameIndices;

	_bool							m_isFinished = { false };

public:
	/* For.FBX_Load*/
	static CAnimation* Create(const aiAnimation* pAIAnimation, const map<string, _uint>& BoneIndices);
	/* For.Binary_Load*/
	static CAnimation* Create(const ANIM_DESC& AnimDesc);
	CAnimation* Clone();
	virtual void Free() override;

#pragma endregion
};

END