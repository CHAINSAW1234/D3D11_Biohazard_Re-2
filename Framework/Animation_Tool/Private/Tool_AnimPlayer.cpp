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

	ANIMPLAYER_DESC* pDesc = { static_cast<ANIMPLAYER_DESC*>(pArg) };
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
		//	기본 적인 불변수 제어 => 라디오 박스 이용
		On_Off_Buttons();

		Show_CurrentSelectedInfos();

		Set_Animation();
		
		if (true == m_isShowBoneTags)
		{
			Show_BoneTags();
		}

		if (true == m_isShowLayerTags)
		{
			Show_BoneLayers();
		}

		Apply_RootMotion();
		Play_Animation(fTimeDelta);
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
	if (nullptr == m_pAnimModel || nullptr == m_pCurrentAnimation || nullptr == m_pRootMoveDir || false == m_isPlayAnimation)
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

void CTool_AnimPlayer::On_Off_Buttons()
{
	if (ImGui::RadioButton("Playing ANIM ##AnimPlayer", m_isPlayAnimation))
	{
		m_isPlayAnimation = !m_isPlayAnimation;
	}

	if (ImGui::RadioButton("Show Bone Tags ##AnimPlayer", m_isShowBoneTags))
	{
		m_isShowBoneTags = !m_isShowBoneTags;
	}

	if (ImGui::RadioButton("Show Layers Tags ##AnimPlayer", m_isShowLayerTags))
	{
		m_isShowLayerTags = !m_isShowLayerTags;
	}


	ImGui::NewLine();
	ImGui::Text("RootMotion Control ##AnimPlayer");
	if (ImGui::RadioButton("Active XZ ##AnimPlayer", m_isRooActive_XZ))
	{
		m_isRooActive_XZ = !m_isRooActive_XZ;
	}	ImGui::SameLine();
	if (ImGui::RadioButton("Active Y ##AnimPlayer", m_isRooActive_Y))
	{
		m_isRooActive_Y = !m_isRooActive_Y;
	}	ImGui::SameLine();
	if (ImGui::RadioButton("Active Rotate Tags ##AnimPlayer", m_isRooActive_Rotate))
	{
		m_isRooActive_Rotate = !m_isRooActive_Rotate;
	}
}

void CTool_AnimPlayer::Set_Animation()
{
	ImGui::NewLine();

	CModel::ANIM_PLAYING_DESC		AnimDesc;

	list<wstring>			LayerTags;
	_bool					isSelectLayer = { false };
	static string			strTargetLayerTag = { "" };	

	static _uint			iAnimIndex = { 0 };
	_bool					isSetAnimIndex = { false };

	static _int				iPlayingIndex = { 0 };
	_bool					isSelectPlayingIndex = { false };
	_uint					iNumPlayingInfo = { 0 };


	static void* pPreModel = { nullptr };
	if (pPreModel != m_pAnimModel)
	{
		strTargetLayerTag = { "" };
		iAnimIndex = { 0 };
		iPlayingIndex = { 0 };
	}
	pPreModel = { m_pAnimModel };



	if (nullptr != m_pAnimModel)
	{
		LayerTags = m_pAnimModel->Get_BoneLayer_Tags();
	}

	_uint				iLayerCnt = { 0 };
	for (auto& wstrLayerTag : LayerTags)
	{
		string			strLayerTag = { Convert_Wstring_String(wstrLayerTag) };
		string			strLayerCnt = { to_string(iLayerCnt) };
		if (ImGui::Button(string(string("Lyaer Select : ") + strLayerTag + string(" ## ") + strLayerCnt).c_str()))
		{
			isSelectLayer = true;
			strTargetLayerTag = strLayerTag;
			break;
		}

		++iLayerCnt;
	}


	if (nullptr != m_pCurrentAnimation && nullptr != m_pAnimModel)
	{
		iAnimIndex = { m_pAnimModel->Find_AnimIndex(m_pCurrentAnimation) };
		isSetAnimIndex = true;
	}

	static _float			fWeight = { 1.f };
	ImGui::SliderFloat("Anim Weight ## Tool_AnimPlayer SetANim Blend", &fWeight, 0.f, 1.f);

	static _bool			isLoop = { false };
	if (ImGui::RadioButton("Anim Loop Active ## Tool_AnimPlayer SetANim Blend", isLoop))
	{
		isLoop = !isLoop;
	}

	if (nullptr != m_pAnimModel)
	{
		iNumPlayingInfo = m_pAnimModel->Get_NumPlayingInfos();
	}

	if (ImGui::InputInt("Anim Playing Index ## Tool_AnimPlayer SetANim Blend", &iPlayingIndex))
	{
		if (iPlayingIndex > iNumPlayingInfo)
			iPlayingIndex = iNumPlayingInfo - 1;

		if (iPlayingIndex < 0)
			iPlayingIndex = 0;

		isSelectPlayingIndex = true;
	}


	if (ImGui::Button("Set Animation ## Tool_AnimPlayer SetANim Blend"))
	{
		AnimDesc.iAnimIndex = iAnimIndex;
		AnimDesc.fWeight = fWeight;
		AnimDesc.strBoneLayerTag = Convert_String_Wstring(strTargetLayerTag);
		AnimDesc.isLoop = isLoop;

		m_pAnimModel->Set_Animation_Blend(AnimDesc, iPlayingIndex);
	}
	
	ImGui::NewLine();
}

void CTool_AnimPlayer::Show_CurrentSelectedInfos()
{
	ImGui::NewLine();
	ImGui::Text("============= Current SelectInfos ===============");

	ImGui::Text(string(string("Bone : ") + m_strCurrentSelectBoneTag + string("## Show_CurrentSelectedInfos")).c_str());
	ImGui::Text(string(string("Layer : ") + m_strCurrentSelectLayerTag + string("## Show_CurrentSelectedInfos")).c_str());

	ImGui::Text("=================================================");
}

void CTool_AnimPlayer::Show_BoneLayers()
{
	ImGui::NewLine();
	ImGui::Text("================ BoneLayerTag ===================");
	ImGui::BeginChild("Scrolling ##Show_BoneLayers() ");

	_int			iLayerCnt = { 0 };
	if (nullptr != m_pAnimModel)
	{
		list<wstring>			LayerTags = { m_pAnimModel->Get_BoneLayer_Tags() };
		for (auto& wstrLayerTag : LayerTags)
		{
			string		strLayerTag = { Convert_Wstring_String(wstrLayerTag) };
			string		strLayerCnt = { to_string(iLayerCnt++) };
			if (ImGui::Button(string(string("Select ## AnimPlayer_Show_BoneLayers") + strLayerCnt).c_str()))
			{
				m_strCurrentSelectBoneTag = strLayerTag;
			}

			ImGui::SameLine();
			ImGui::Text(strLayerTag.c_str());
		}
	}
	ImGui::EndChild();
	ImGui::Text("=================================================");
}

void CTool_AnimPlayer::Show_BoneTags()
{
	ImGui::NewLine();
	ImGui::Text("================== Bone Tags ====================");
	ImGui::BeginChild("Scrolling ## ShowBoneTags() ");

	_int			iBoneCnt = { 0 };
	if (nullptr != m_pAnimModel)
	{
		vector<string>			BoneTags = { m_pAnimModel->Get_BoneNames() };
		for (auto& strBoneTag : BoneTags)
		{
			string		strBoneCnt = { to_string(iBoneCnt++) };
			if (ImGui::Button(string(string("Select ## AnimPlayer_Show_BoneTags") + strBoneCnt).c_str()))
			{
				m_strCurrentSelectBoneTag = strBoneTag;
			}

			ImGui::SameLine();

			if (ImGui::Button(string(string("RootBone ## AnimPlayer_Show_BoneTags") + strBoneCnt).c_str()))
			{
				Set_RootBone(strBoneTag);
			}

			ImGui::SameLine();

			ImGui::Text(strBoneTag.c_str());
		}
	}

	ImGui::EndChild();
	ImGui::Text("=================================================");
}

void CTool_AnimPlayer::Set_RootBone(const string& strRootBoneTag)
{
	if (nullptr == m_pAnimModel)
		return;

	m_pAnimModel->Set_RootBone(strRootBoneTag);
}

void CTool_AnimPlayer::Apply_RootMotion()
{
	if (nullptr == m_pAnimModel)
		return;

	m_pAnimModel->Active_RootMotion_Rotation(m_isRooActive_Rotate);
	m_pAnimModel->Active_RootMotion_XZ(m_isRooActive_XZ);
	m_pAnimModel->Active_RootMotion_Y(m_isRooActive_Y);
}

CTool_AnimPlayer* CTool_AnimPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTool_AnimPlayer* pInatnace = { new CTool_AnimPlayer(pDevice, pContext) };

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
