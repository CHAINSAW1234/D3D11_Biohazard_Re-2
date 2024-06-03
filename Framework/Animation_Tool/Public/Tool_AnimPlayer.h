#pragma once

#include "Tool_Defines.h"
#include "Tool.h"

BEGIN(Tool)

class CTool_AnimPlayer final : public CTool
{
public:
	typedef struct tagAnimPlayerDesc
	{
		_float3*			pMoveDir = { nullptr };
		CTransform*			pTransform = { nullptr };
	}ANIMPLAYER_DESC;

private:
	CTool_AnimPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CTool_AnimPlayer() = default;

public:
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Tick(_float fTimeDelta) override;

public:
	void					Change_Animation(CAnimation* pAnimation);
	void					Play_Animation(_float fTimeDelta);
	void					Change_Model(CModel* pModel);
	void					Set_TargetTransform(CTransform* pTransform);

public:
	_float					Get_Ratio();

private:
	_float					Get_CurrentAnim_Duration();
	_float					Get_CurrentAnim_TrackPosition();
	_uint					Get_CurrentKeyFrame();

private:
	CAnimation*				m_pCurrentAnimation = { nullptr };
	CModel*					m_pAnimModel = { nullptr };
	CTransform*				m_pTargetTransform = { nullptr };
	_float3*				m_pRootMoveDir = { nullptr };
	_bool					m_isPlayAnimation = { false };

public:
	static CTool_AnimPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END