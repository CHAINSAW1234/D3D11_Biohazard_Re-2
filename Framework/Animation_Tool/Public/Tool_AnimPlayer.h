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
		_float3*								pMoveDir = { nullptr };
		CTransform*								pTransform = { nullptr };
	}ANIMPLAYER_DESC;

private:
	CTool_AnimPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTool_AnimPlayer() = default;

public:
	virtual HRESULT								Initialize(void* pArg) override;
	virtual void								Tick(_float fTimeDelta) override;

public:
	void										Play_Animation(_float fTimeDelta);
	void										Set_Current_Animation(CAnimation* pAnimation);
	void										Set_Current_Model(CModel* pModel);
	void										Set_TargetTransform(CTransform* pTransform);

public:
	_float										Compute_Ratio();

private:
	void										Play_Bar();

private:
	_float										Get_CurrentAnim_Duration();
	_float										Get_CurrentAnim_TrackPosition();
	_uint										Get_CurrentKeyFrame();

	void										Set_TrackPosition(_float fTrackPosition);

private:
	void										On_Off_Buttons();
	void										Set_Animation();

private:
	void										Create_PlayingDesc();

private:
	void										Apply_RootMotion();

private:
	CAnimation*									m_pCurrentAnimation = { nullptr };
	CModel*										m_pCurrentModel = { nullptr };
	CTransform*									m_pTargetTransform = { nullptr };
	_float3*									m_pRootMoveDir = { nullptr };

	_bool										m_isPlayAnimation = { false };

	_bool										m_isRooActive_XZ = { false };
	_bool										m_isRooActive_Y = { false };
	_bool										m_isRooActive_Rotate = { false };

	const wstring*								m_pCurrentPartObjectTag = { nullptr };
	const wstring*								m_pCurrentBoneLayerTag = { nullptr };

public:
	static CTool_AnimPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END