#pragma once

#include "Tool_Defines.h"
#include "Tool.h"

BEGIN(Tool)

class CTool_AnimPlayer final : public CTool
{
public:
	typedef struct tagAnimPlayerDesc
	{
		const wstring*							pCurrentBoneLayerTag = { nullptr };
		const wstring*							pCurrentPartObjectTag = { nullptr };
		const string*							pCurrentModelTag = { nullptr };
		_float3*								pMoveDir = { nullptr };
		CTransform*								pTransform = { nullptr };
		class CAnimTestObject*					pTestObject = { nullptr };
	}ANIMPLAYER_DESC;

private:
	CTool_AnimPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTool_AnimPlayer() = default;

public:
	virtual HRESULT								Initialize(void* pArg) override;
	virtual void								Tick(_float fTimeDelta) override;

public:
	void										Play_Animation(_float fTimeDelta);
	void										Change_Animation();
	HRESULT										Set_Models_Ptr(map<string, CModel*>* pModels);
	void										Set_Current_Model(CModel* pModel);
	void										Set_Current_Animation(CAnimation* pAnimation);
	void										Set_TargetTransform(CTransform* pTransform);

public:
	_float										Compute_Ratio();

private:
	void										TrackPosition_Controller();
	void										BlendWeight_Controller();

private:
	_float										Get_CurrentPlayingInfo_Duration();
	_float										Get_CurrentPlayingInfo_TrackPosition();
	_float										Get_CurrentAnim_BlendWeight();
	_uint										Get_CurrentKeyFrame();

	void										Set_TrackPosition(_uint iPlayingIndex, _float fTrackPosition);
	void										Set_BlendWeight(_uint iPlayingIndex, _float fWeight);

private:
	_int										Find_LastPlayingIndex(const string& strModelTag);
	
private:
	void										On_Off_Buttons();
	void										Set_Animation();

private:
	void										Create_PlayingDesc();

private:
	void										Apply_RootMotion();

private:
	class CAnimTestObject*						m_pTestObject = { nullptr };

	map<string, _uint>							m_ModelLastPlayingIndex;
	map<string, _bool>							m_isPlayingAnimations;
	map<string, CModel*>*						m_pModels = { nullptr };
	CModel*										m_pCurrentModel = { nullptr };
	CAnimation*									m_pCurrentAnimation = { nullptr };
	CTransform*									m_pTargetTransform = { nullptr };
	_float3*									m_pRootMoveDir = { nullptr };

	_bool										m_isPlayAnimation = { false };

	_bool										m_isRooActive_XZ = { false };
	_bool										m_isRooActive_Y = { false };
	_bool										m_isRooActive_Rotate = { false };

	const string*								m_pCurrentModelTag = { nullptr };
	const wstring*								m_pCurrentPartObjectTag = { nullptr };
	const wstring*								m_pCurrentBoneLayerTag = { nullptr };

public:
	static CTool_AnimPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END