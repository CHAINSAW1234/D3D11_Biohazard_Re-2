#include "stdafx.h"
#include "Tool_AnimPlayer.h"

CTool_AnimPlayer::CTool_AnimPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTool{ pDevice, pContext }
{
}

HRESULT CTool_AnimPlayer::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	ANIMPLAYER_DESC*		pDesc = { static_cast<ANIMPLAYER_DESC*>(pArg) };
	m_pTargetTransform = pDesc->pTransform;
	m_pRootMoveDir = pDesc->pMoveDir;

	Safe_AddRef(m_pTargetTransform);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_strCollasingTag = "Tool_AnimPlayer";

	return S_OK;
}

void CTool_AnimPlayer::Tick(_float fTimeDelta)
{
	if (ImGui::CollapsingHeader(m_strCollasingTag.c_str()))
	{
		if (ImGui::RadioButton("Playing ANIM ## AnimPlayer", m_isPlayAnimation))
		{
			m_isPlayAnimation = !m_isPlayAnimation;
		}

		if (true == m_isPlayAnimation)
		{
			Play_Animation(fTimeDelta);
		}
	}
}	

void CTool_AnimPlayer::Change_Animation(CAnimation* pAnimation)
{
	if (nullptr == pAnimation)
		return;

	m_pCurrentAnimation = pAnimation;
	Safe_AddRef(m_pCurrentAnimation);
}

void CTool_AnimPlayer::Play_Animation(_float fTimeDelta)
{
	if (nullptr == m_pAnimModel || nullptr == m_pCurrentAnimation || nullptr == m_pRootMoveDir)
		return;

	m_pAnimModel->Play_Animations_RootMotion(m_pTargetTransform, fTimeDelta, m_pRootMoveDir);
}

void CTool_AnimPlayer::Change_Model(CModel* pModel)
{
	if (nullptr == pModel)
		return;

	m_pAnimModel = pModel;
	Safe_AddRef(m_pAnimModel);
}

void CTool_AnimPlayer::Set_TargetTransform(CTransform* pTransform)
{
	if (nullptr == pTransform)
		return;

	m_pTargetTransform = pTransform;
	Safe_AddRef(m_pTargetTransform);
}

_float CTool_AnimPlayer::Get_Ratio()
{
	_float			fDuration = { Get_CurrentAnim_Duration() };
	_float			fTrackPosition = { Get_CurrentAnim_TrackPosition() };

	_float			fRatio = { fTrackPosition / fDuration };

	return fRatio;
}

_float CTool_AnimPlayer::Get_CurrentAnim_Duration()
{
	if (nullptr == m_pCurrentAnimation)
		return 0.f;

	return m_pCurrentAnimation->Get_Duration();
}

_float CTool_AnimPlayer::Get_CurrentAnim_TrackPosition()
{
	if (nullptr == m_pCurrentAnimation)
		return 0.f;

	return m_pCurrentAnimation->Get_TrackPosition();
}

_uint CTool_AnimPlayer::Get_CurrentKeyFrame()
{
	if (nullptr == m_pCurrentAnimation)
		return 0;

	vector<_uint>		KeyFrameIndices = { m_pCurrentAnimation->Get_CurrentKeyFrameIndices() };

	_uint		iCurrentKeyFrame = { 0 };
	for (auto& iKeyFrame : KeyFrameIndices)
		if (iCurrentKeyFrame < iKeyFrame)
			iCurrentKeyFrame = iKeyFrame;

	return iCurrentKeyFrame;
}

CTool_AnimPlayer* CTool_AnimPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTool_AnimPlayer*			pInatnace = { new CTool_AnimPlayer(pDevice, pContext) };

	if (FAILED(pInatnace->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTool_AnimPlayer"));

		Safe_Release(pInatnace);
	}

	return pInatnace;
}

void CTool_AnimPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pCurrentAnimation);
	Safe_Release(m_pAnimModel);
	Safe_Release(m_pTargetTransform);
}
