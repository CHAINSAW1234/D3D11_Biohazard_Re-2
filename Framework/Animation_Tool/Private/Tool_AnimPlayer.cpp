#include "stdafx.h"
#include "Tool_AnimPlayer.h"

#include "AnimTestObject.h"
#include "AnimTestPartObject.h"

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
	m_pTestObject = pDesc->pTestObject;
	m_pCurrentBoneLayerTag = pDesc->pCurrentBoneLayerTag;
	m_pCurrentPartObjectTag = pDesc->pCurrentPartObjectTag;
	m_pCurrentModelTag = pDesc->pCurrentModelTag;

	if (nullptr == m_pCurrentBoneLayerTag || nullptr == m_pCurrentPartObjectTag || nullptr == m_pCurrentModelTag)
		return E_FAIL;

	Safe_AddRef(m_pTargetTransform);
	Safe_AddRef(m_pTestObject);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_strCollasingTag = "Tool_AnimPlayer";

	return S_OK;
}

void CTool_AnimPlayer::Tick(_float fTimeDelta)
{

	ImGui::Begin("Animation_Player");
	//	기본 적인 불변수 제어 => 라디오 박스 이용
	On_Off_Buttons();

	Set_Animation();

	Apply_RootMotion();

	Play_Animation(fTimeDelta);
	TrackPosition_Controller();

	ImGui::End();
}

void CTool_AnimPlayer::Play_Animation(_float fTimeDelta)
{
	if (nullptr == m_pCurrentModel || nullptr == m_pRootMoveDir || false == m_isPlayAnimation)
		return;

	/*for (auto& Pair: m_isPlayingAnimations)
	{
		_bool			isPlaying = { Pair.second };
		if (true == isPlaying)
		{
			string		strModelTag = { Pair.first };
			map<string, CModel*>::iterator		iter = { m_pModels->find(strModelTag) };
			if (iter != m_pModels->end())
				iter->second->Play_Animations_RootMotion(m_pTargetTransform, fTimeDelta, m_pRootMoveDir);
		}
	}*/

	if (true == m_isPlayAnimation)
	{
		for (auto& Pair : *m_pModels)
		{
			CModel*			pSrcModel = { Pair.second };
			wstring			strRootPartTag = { m_pTestObject->Get_RootActivePartTag() };
			wstring			strModelPartTag = { TEXT("") };
			_bool			isRootPart = { false };
			map<wstring, CAnimTestPartObject*>			PartObjects = { m_pTestObject->Get_PartObjects() };
			for (auto& Pair : PartObjects)
			{
				CModel*		pModel = { Pair.second->Get_CurrentModelComponent() };
				if (pModel == pSrcModel)
				{
					strModelPartTag = Pair.first;
					
					isRootPart = strRootPartTag == strModelPartTag;

					break;
				}
			}
			
			if (false == isRootPart)
			{
				_float3				vTempDir = {};
				Pair.second->Play_Animations_RootMotion(m_pTargetTransform, fTimeDelta, &vTempDir);
			}
			else
			{
				Pair.second->Play_Animations_RootMotion(m_pTargetTransform, fTimeDelta, m_pRootMoveDir);
			}
		}
	}
}

HRESULT CTool_AnimPlayer::Set_Models_Ptr(map<string, CModel*>* pModels)
{
	if (nullptr == pModels)
		return E_FAIL;

	m_pModels = pModels;
	for (auto& Pair : *m_pModels)
	{
		string			strModelTag = { Pair.first };
		CModel*			pModel = { Pair.second };
		if (nullptr == pModel || "" == strModelTag)
			return E_FAIL;
	}

	return S_OK;
}

void CTool_AnimPlayer::Set_Current_Model(CModel* pModel)
{
	if (nullptr == pModel)
		return;

	m_pCurrentModel = pModel;
	Safe_AddRef(m_pCurrentModel);
}

void CTool_AnimPlayer::Set_Current_Animation(CAnimation* pAnimation)
{
	if (nullptr == pAnimation)
		return;

	m_pCurrentAnimation = pAnimation;
	Safe_AddRef(m_pCurrentAnimation);
}

void CTool_AnimPlayer::Set_TargetTransform(CTransform* pTransform)
{
	if (nullptr == pTransform)
		return;

	m_pTargetTransform = pTransform;
	Safe_AddRef(m_pTargetTransform);
}

_float CTool_AnimPlayer::Compute_Ratio()
{
	_float			fDuration = { Get_CurrentPlayingInfo_Duration() };
	_float			fTrackPosition = { Get_CurrentPlayingInfo_TrackPosition() };

	_float			fRatio = { fTrackPosition / fDuration };

	return fRatio;
}

void CTool_AnimPlayer::TrackPosition_Controller()
{
	ImGui::SeparatorText("Play Bar");

	_float			fTrackPosition = { Get_CurrentPlayingInfo_TrackPosition() };
	_float			fDuration = { Get_CurrentPlayingInfo_Duration() };

	string			strTrackPosition = { to_string(fTrackPosition) };
	string			strDuration = { to_string(fDuration) };

	ImGui::Text(string(string("TrackPosition : ") + strTrackPosition).c_str()); 
	ImGui::Text(string(string("Duration : ") + strDuration).c_str());

	ImGui::SeparatorText("##");

	ImGui::SliderFloat("##CTool_AnimPlayer::TrackPosition_Controller()", &fTrackPosition, 0.f, fDuration);

	_int			iPlayingIndex = { -1 };
	map<string, _uint>::iterator			iter = { m_ModelLastPlayingIndex.find(*m_pCurrentModelTag) };
	if (iter != m_ModelLastPlayingIndex.end())
	{
		iPlayingIndex = m_ModelLastPlayingIndex[*m_pCurrentModelTag];
	}

	if (-1 != iPlayingIndex)
	{
		Set_TrackPosition(iPlayingIndex, fTrackPosition);
	}

	ImGui::SeparatorText("##");
}

void CTool_AnimPlayer::BlendWeight_Controller()
{
	ImGui::SeparatorText("Blend Weight");

	_float			fBlendWeight = { Get_CurrentAnim_BlendWeight() };
	string			strBlendWeight = { to_string(fBlendWeight) };

	ImGui::Text(string(string("BlendWeight : ") + strBlendWeight).c_str());

	ImGui::SeparatorText("##");

	ImGui::SliderFloat("##CTool_AnimPlayer::BlendWeight_Controller()", &fBlendWeight, 0.f, 1.f);

	_int			iPlayingIndex = { -1 };
	map<string, _uint>::iterator			iter = { m_ModelLastPlayingIndex.find(*m_pCurrentModelTag) };
	if (iter != m_ModelLastPlayingIndex.end())
	{
		iPlayingIndex = m_ModelLastPlayingIndex[*m_pCurrentModelTag];
	}

	if (-1 != iPlayingIndex)
	{
		Set_BlendWeight(iPlayingIndex, fBlendWeight);
	}

	ImGui::SeparatorText("##");
}

_float CTool_AnimPlayer::Get_CurrentPlayingInfo_Duration()
{
	if (nullptr == m_pCurrentModel)
		return 0.f;

	_int			iPlayingIndex = { Find_LastPlayingIndex(*m_pCurrentModelTag) };
	if (-1 == iPlayingIndex)
		return 0.f;

	_float			fDuration = { m_pCurrentModel->Get_Duration(iPlayingIndex) };

	return fDuration;
}

_float CTool_AnimPlayer::Get_CurrentPlayingInfo_TrackPosition()
{
	if (nullptr == m_pCurrentModel)
		return 0.f;

	_int			iPlayingIndex = { Find_LastPlayingIndex(*m_pCurrentModelTag) };
	if (-1 == iPlayingIndex)
		return 0.f;

	_float			fTrackPosition = { m_pCurrentModel->Get_TrackPosition(iPlayingIndex) };

	return fTrackPosition;
}

_float CTool_AnimPlayer::Get_CurrentAnim_BlendWeight()
{
	if (nullptr == m_pCurrentModel)
		return 0.f;

	_int			iPlayingIndex = { Find_LastPlayingIndex(*m_pCurrentModelTag) };
	if (-1 == iPlayingIndex)
		return 0.f;

	_float			fBlendWeight = { m_pCurrentModel->Get_BlendWeight(iPlayingIndex) };

	return fBlendWeight;
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

void CTool_AnimPlayer::Set_TrackPosition(_uint iPlayingIndex, _float fTrackPosition)
{
	if (nullptr == m_pCurrentModel)
		return;

	m_pCurrentModel->Set_TrackPosition(iPlayingIndex, fTrackPosition);
}

void CTool_AnimPlayer::Set_BlendWeight(_uint iPlayingIndex, _float fWeight)
{
	if (nullptr == m_pCurrentModel)
		return;

	m_pCurrentModel->Set_BlendWeight(iPlayingIndex, fWeight);
}

_int CTool_AnimPlayer::Find_LastPlayingIndex(const string& strModelTag)
{
	_int			iPlayingIndex = { -1 };
	map<string, _uint>::iterator		iter = { m_ModelLastPlayingIndex.find(strModelTag) };
	
	if (iter == m_ModelLastPlayingIndex.end())
		return iPlayingIndex;

	iPlayingIndex = m_ModelLastPlayingIndex[strModelTag];

	return iPlayingIndex;
}

void CTool_AnimPlayer::On_Off_Buttons()
{
	if (ImGui::RadioButton("Playing ANIM ##AnimPlayer", m_isPlayAnimation))
	{
		m_isPlayAnimation = !m_isPlayAnimation;
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

	Create_PlayingDesc();

	ImGui::NewLine();
}

void CTool_AnimPlayer::Create_PlayingDesc()
{
	if (nullptr == m_pCurrentAnimation || nullptr == m_pCurrentModel)
		return;

	CModel::ANIM_PLAYING_DESC		AnimDesc;

	ImGui::SeparatorText("##");

	static _float			fWeight = { 1.f };
	ImGui::SliderFloat("Anim Weight ## Tool_AnimPlayer SetANim Blend", &fWeight, 0.f, 1.f);

	static _bool			isLoop = { false };
	if (ImGui::RadioButton("Anim Loop Active ##CTool_AnimPlayer::Create_PlayingDesc()", isLoop))
		isLoop = !isLoop;

	string					strSrcAnimTag = { m_pCurrentAnimation->Get_Name() };
	vector<CAnimation*>		Animations = { m_pCurrentModel->Get_Animations() };

	_uint					iNumAnim = { static_cast<_uint>(Animations.size()) };
	_uint					iAnimIndex = { 0 };
	for (auto& pAnimation : Animations)
	{
		string			strDstAnimTag = { pAnimation->Get_Name() };

		if (strSrcAnimTag == strDstAnimTag)
			break;

		++iAnimIndex;
	}

	_uint					iNumPlayingInfo = {};
	if (nullptr != m_pCurrentModel)
	{
		iNumPlayingInfo = m_pCurrentModel->Get_NumPlayingInfos();
	}
	string					strNumPlayingInfo = { to_string(static_cast<_int>(iNumPlayingInfo)) };

	ImGui::SeparatorText("Set Playing Info Index");

	ImGui::Text(string(string("Num Playing Info : ") + strNumPlayingInfo).c_str());

	static _int					iPlayingIndex = { 0 };
	if (ImGui::InputInt("Index ##CTool_AnimPlayer::Create_PlayingDesc()", &iPlayingIndex))
	{
		if (iPlayingIndex > static_cast<_int>(iNumPlayingInfo))
			iPlayingIndex = static_cast<_int>(iNumPlayingInfo) - 1;

		if (iPlayingIndex < 0)
			iPlayingIndex = 0;
	}

	ImGui::SeparatorText("##");

	if (ImGui::Button("Set Animation ##CTool_AnimPlayer::Create_PlayingDesc()"))
	{
		_bool				isCanCreate = { true };
		if (iAnimIndex >= iNumAnim)
			isCanCreate = false;

		list<wstring>		BoneLayerTags = { m_pCurrentModel->Get_BoneLayer_Tags() };
		_bool				isIncludedLayer = { false };
		for (auto& strBoneLayerTag : BoneLayerTags)
		{
			if (strBoneLayerTag == *m_pCurrentBoneLayerTag)
				isIncludedLayer = true;
		}

		if (false == isIncludedLayer)
			isCanCreate = false;

		if (0 == iNumPlayingInfo)
			isCanCreate = false;

		if (true == isCanCreate)
		{
			AnimDesc.iAnimIndex = iAnimIndex;
			AnimDesc.fWeight = fWeight;
			AnimDesc.strBoneLayerTag = *m_pCurrentBoneLayerTag;
			AnimDesc.isLoop = isLoop;
		}

		m_pCurrentModel->Set_Animation_Blend(AnimDesc, iPlayingIndex);
	}
	return;
}

void CTool_AnimPlayer::Apply_RootMotion()
{
	if (nullptr == m_pCurrentModel)
		return;

	m_pCurrentModel->Active_RootMotion_Rotation(m_isRooActive_Rotate);
	m_pCurrentModel->Active_RootMotion_XZ(m_isRooActive_XZ);
	m_pCurrentModel->Active_RootMotion_Y(m_isRooActive_Y);
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
	Safe_Release(m_pCurrentModel);
	Safe_Release(m_pTargetTransform);
	Safe_Release(m_pTestObject);
}
