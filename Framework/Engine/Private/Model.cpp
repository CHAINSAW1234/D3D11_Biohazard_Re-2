#include "..\Public\Model.h"

#include "Shader.h"
#include "Texture.h"

#include "GameInstance.h"

#include "Model_Extractor.h"

#include "Animation.h"
#include "Bone.h"
#include "Mesh.h"
#include "Bone_Layer.h"
#include "Animation_Layer.h"
#include "IK_Solver.h"

#include "Transform.h"
#include "RagDoll_Physics.h"
#include "PlayingInfo.h"

#include "Texture.h"

unordered_map<wstring, class CTexture*>	CModel::m_LoadedTextures;

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

CModel::CModel(const CModel& rhs)
	: CComponent{ rhs }
	, m_eModelType{ rhs.m_eModelType }
	, m_iNumMeshes{ rhs.m_iNumMeshes }
	, m_Meshes{ rhs.m_Meshes }
	, m_iNumMaterials{ rhs.m_iNumMaterials }
	, m_Materials{ rhs.m_Materials }
	, m_TransformationMatrix{ rhs.m_TransformationMatrix }
	, m_iNumAnimations{ rhs.m_iNumAnimations }
	, m_vCenterPoint{ rhs.m_vCenterPoint }
{
	for (auto& pPrototypeBone : rhs.m_Bones)
		m_Bones.push_back(pPrototypeBone->Clone());

	for (auto& Pair : rhs.m_AnimationLayers)
	{
		wstring						strLayerTag = { Pair.first };
		vector<CAnimation*>			Animations = { Pair.second->Get_Animations() };

		CAnimation_Layer* pAnimLayer = { CAnimation_Layer::Create() };
		for (auto& pAnimation : Animations)
		{
			CAnimation* pClonedAnimation = { pAnimation->Clone(m_Bones) };
			pAnimLayer->Add_Animation(pClonedAnimation);
		}
		m_AnimationLayers.emplace(Pair.first, pAnimLayer);
	}

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& Material : m_Materials)
	{
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			Safe_AddRef(Material.MaterialTextures[i]);
	}

	m_PlayingAnimInfos.resize(100);

	Initialize_DecalMap();
}

#pragma region PlayInfo

void CModel::Add_AnimPlayingInfo(_bool isLoop, _uint iPlayingIndex, const wstring& strBoneLayerTag, _float fBlendWeight)
{
	_bool				isCanCreate = { true };

	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	if (nullptr != pPlayingInfo)
		isCanCreate = false;

	CBone_Layer* pBoneLayer = { Find_BoneLayer(strBoneLayerTag) };
	if (nullptr == pBoneLayer)
		isCanCreate = false;

	if (true == isCanCreate)
	{
		CPlayingInfo::PLAYING_INFO_DESC		PlayingInfoDesc;
		PlayingInfoDesc.fBlendWeight = fBlendWeight;
		PlayingInfoDesc.iAnimIndex = -1;
		PlayingInfoDesc.iNumBones = static_cast<_uint>(m_Bones.size());
		PlayingInfoDesc.isLoop = isLoop;
		PlayingInfoDesc.strBoneLayerTag = strBoneLayerTag;
		PlayingInfoDesc.iNumChannel = 0;

		CPlayingInfo* pNewPlayingInfo = { CPlayingInfo::Create(&PlayingInfoDesc) };
		m_PlayingAnimInfos[iPlayingIndex] = pNewPlayingInfo;
	}
}

void CModel::Erase_AnimPlayingInfo(_uint iPlayingIndex)
{
	vector<CPlayingInfo*>::iterator			iter = { m_PlayingAnimInfos.begin() + iPlayingIndex };
	if (iter != m_PlayingAnimInfos.end())
	{
		Safe_Release(*iter);
		m_PlayingAnimInfos.erase(iter);
	}
}

_uint CModel::Get_NumAnims(const wstring& strAnimLayerTag)
{
	_uint			iNumAnims = { 0 };

	unordered_map<wstring, CAnimation_Layer*>::iterator		iter = { m_AnimationLayers.find(strAnimLayerTag) };
	if (iter != m_AnimationLayers.end())
	{
		iNumAnims = iter->second->Get_NumAnims();
	}

	return iNumAnims;
}

_int CModel::Get_CurrentAnimIndex(_uint iPlayingIndex)
{
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	_int				iAnimIndex = { -1 };
	if (nullptr != pPlayingInfo)
	{
		iAnimIndex = { pPlayingInfo->Get_AnimIndex() };
	}

	return iAnimIndex;
}

wstring CModel::Get_CurrentAnimLayerTag(_uint iPlayingIndex)
{
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	wstring				strAnimLayerTag = { TEXT("") };
	if (nullptr != pPlayingInfo)
	{
		strAnimLayerTag = { pPlayingInfo->Get_AnimLayerTag() };
	}

	return strAnimLayerTag;
}

string CModel::Get_CurrentAnimTag(_uint iPlayingIndex)
{
	string			strAnimTag = { "" };
	_int			iAnimIndex = { Get_CurrentAnimIndex(iPlayingIndex) };
	if (-1 == iAnimIndex)
		return strAnimTag;

	wstring			strAnimLayerTag = { Get_CurrentAnimLayerTag(iPlayingIndex) };
	if (TEXT("") == strAnimLayerTag)
		return strAnimTag;

	unordered_map<wstring, CAnimation_Layer*>::iterator	 iter = { m_AnimationLayers.find(strAnimLayerTag) };
	if (iter != m_AnimationLayers.end())
	{
		strAnimTag = iter->second->Get_Animation(iAnimIndex)->Get_Name();
	}

	return strAnimTag;
}

_float4x4 CModel::Get_TransformationMatrix()
{
	return m_TransformationMatrix;
}

_matrix CModel::Get_FirstKeyFrame_Root_TransformationMatrix(const wstring& strAnimLayerTag, _int iAnimIndex)
{
	_matrix				ResultMatrix = { XMMatrixIdentity() };
	if (iAnimIndex < 0)
		return ResultMatrix;

	unordered_map<wstring, CAnimation_Layer*>::iterator		iter = { m_AnimationLayers.find(strAnimLayerTag) };
	if (iter != m_AnimationLayers.end())
	{
		_uint			iNumAnims = { iter->second->Get_NumAnims() };
		if (iNumAnims <= static_cast<_uint>(iAnimIndex))
			return ResultMatrix;

		_int			iRootBoneIndex = { Find_RootBoneIndex() };
		if (iRootBoneIndex < 0)
			return ResultMatrix;

		KEYFRAME			KeyFrame = { iter->second->Get_Animation(iAnimIndex)->Get_FirstKeyFrame(iRootBoneIndex) };
		_vector				vScale = { XMLoadFloat3(&KeyFrame.vScale) };
		_vector				vQuaternion = { XMLoadFloat4(&KeyFrame.vRotation) };
		_vector				vTranslation = { XMLoadFloat3(&KeyFrame.vTranslation) };

		ResultMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vQuaternion, vTranslation);
	}

	return ResultMatrix;
}

_matrix CModel::Get_FirstKeyFrame_Root_CombinedMatrix(const wstring& strAnimLayerTag, _int iAnimIndex)
{
	_matrix				ResultMatrix = { XMMatrixIdentity() };
	if (iAnimIndex < 0)
		return ResultMatrix;

	unordered_map<wstring, CAnimation_Layer*>::iterator		iter = { m_AnimationLayers.find(strAnimLayerTag) };
	if (iter != m_AnimationLayers.end())
	{
		ResultMatrix = { XMLoadFloat4x4(&m_TransformationMatrix) };
		list<_uint>			BoneIndices;
		_int			iBoneIndex = { Find_RootBoneIndex() };
		while (iBoneIndex != -1)
		{
			BoneIndices.emplace_front(iBoneIndex);
			iBoneIndex = m_Bones[iBoneIndex]->Get_ParentIndex();
		}

		for (auto& iBoneIndex : BoneIndices)
		{
			KEYFRAME			KeyFrame = { iter->second->Get_Animation(iAnimIndex)->Get_FirstKeyFrame(iBoneIndex) };
			_vector				vScale = { XMLoadFloat3(&KeyFrame.vScale) };
			_vector				vQuaternion = { XMLoadFloat4(&KeyFrame.vRotation) };
			_vector				vTranslation = { XMLoadFloat3(&KeyFrame.vTranslation) };

			_matrix				TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vQuaternion, vTranslation);

			ResultMatrix = TransformationMatrix * ResultMatrix;
		}
	}

	return ResultMatrix;
}

_matrix CModel::Get_CurrentKeyFrame_Root_CombinedMatrix(_uint iPlayingIndex)
{
	CPlayingInfo*				pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	if (nullptr == pPlayingInfo)
		return XMMatrixIdentity();

	_int						iAnimationIndex = { pPlayingInfo->Get_AnimIndex() };
	wstring						strAnimLayerTag = { pPlayingInfo->Get_AnimLayerTag() };

	if (-1 == iAnimationIndex || TEXT("") == strAnimLayerTag)
		return XMMatrixIdentity();

	unordered_map<wstring, CAnimation_Layer*>::iterator		iter = { m_AnimationLayers.find(strAnimLayerTag) };
	if (iter == m_AnimationLayers.end())
		return XMMatrixIdentity();

	CAnimation_Layer*			pAnimationLayer = { iter->second };	
	CAnimation*					pAnimation = { pAnimationLayer->Get_Animation(iAnimationIndex) };

	_float						fTrackPosition = { pPlayingInfo->Get_TrackPosition() };
	_int						iRootBoneIndex = { Find_RootBoneIndex() };

	if (-1 == iRootBoneIndex)
		return XMMatrixIdentity();

	KEYFRAME					KeyFrame = { pAnimation->Get_CurrentKeyFrame(iRootBoneIndex, fTrackPosition) };

	_vector						vScale = { XMLoadFloat3(&KeyFrame.vScale) };
	_vector						vTranslation = { XMVectorSetW(XMLoadFloat3(&KeyFrame.vTranslation), 1.f) };
	_vector						vQuaternion = { XMLoadFloat4(&KeyFrame.vRotation) };

	return XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vQuaternion, vTranslation);
}

void CModel::Reset_PreAnimation(_uint iPlayingIndex)
{
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	if (nullptr != pPlayingInfo)
	{
		pPlayingInfo->Set_PreAnimIndex(-1);
		pPlayingInfo->Set_PreAnimLayerTag(TEXT(""));
	}
}

void CModel::Reset_PreAnim_CurrentAnim(_uint iPlayingIndex)
{
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	if (nullptr != pPlayingInfo)
	{
		pPlayingInfo->Reset_PreAnim_CurrentAnim();
	}
}

//	strPrototypeLayerTag = > 프로토 타입으로 등록했던 태그
//	strAnimLayerTag => 내가 변수로 저장할 태그map 키값
HRESULT CModel::Add_Animations(const wstring& strPrototypeLayerTag, const wstring& strAnimLayerTag)
{
	CAnimation_Layer* pAnimation_Layer = { nullptr };
	unordered_map<wstring, CAnimation_Layer*>::iterator		iter = { m_AnimationLayers.find(strAnimLayerTag) };
	if (iter == m_AnimationLayers.end())
	{
		pAnimation_Layer = { CAnimation_Layer::Create() };
		if (nullptr == pAnimation_Layer)
			return E_FAIL;
	}

	else
		return E_FAIL;

	_uint			iNumAnims = { m_pGameInstance->Get_NumAnim_Prototypes(strPrototypeLayerTag) };
	for (_uint i = 0; i < iNumAnims; ++i)
	{
		CAnimation* pAnimation = { nullptr };
		m_pGameInstance->Clone_Animation(strPrototypeLayerTag, i, m_Bones, &pAnimation);

		if (nullptr == pAnimation)
			return E_FAIL;

		if (FAILED(pAnimation_Layer->Add_Animation(pAnimation)))
		{
			MSG_BOX(TEXT("Failed To Add Animation :: HRESULT CModel::Add_Animations(const wstring& strPrototypeLayerTag, const wstring& strAnimLayerTag)"));

			return E_FAIL;
		}
	}

	m_AnimationLayers.emplace(strAnimLayerTag, pAnimation_Layer);

	return S_OK;
}

HRESULT CModel::Add_Bones(CModel* pModel)
{
	for (auto& pDstBone : pModel->m_Bones)
	{
		_char* pDstName = { pDstBone->Get_Name() };

		_bool				isAlreadyIn = { false };
		for (auto& pSrcBone : m_Bones)
		{
			_char* pSrcName = { pSrcBone->Get_Name() };
			if (0 == strcmp(pSrcName, pDstName))
			{
				isAlreadyIn = true;
				break;
			}
		}

		if (false == isAlreadyIn)
		{
			m_Bones.emplace_back(pDstBone);
			Safe_AddRef(pDstBone);
		}
	}
	return S_OK;
}

#pragma endregion

_float CModel::Compute_NewTimeDelta_Distatnce_Optimization(_float fTimeDelta, CTransform* pTransform)
{
	/* TODO:
	*LOD를 사용하면 RAGDOLL로 전환시 모델이 한 번 깜빡이는 증상이 있음
	*수정 필요.
	*/

#ifndef ANIMATION_LOD
	return fTimeDelta;
#endif

	if (false == m_isOptimization)
		return fTimeDelta;

	_matrix			CamWorldMatrix = { m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW) };
	_vector			vCamPosition = { CamWorldMatrix.r[CTransform::STATE_POSITION] };
	_matrix			MyWorldMatirx = { pTransform->Get_WorldMatrix() };
	_vector			vMyPosition = { MyWorldMatirx.r[CTransform::STATE_POSITION] };

	_float			fDistance = { XMVectorGetX(XMVector3Length(vCamPosition - vMyPosition)) };

	m_fAccOptimizationTime += fTimeDelta;
	if (fDistance < DISTANCE_FPSMAX)
	{
		if (TIME_FPSMAX > m_fAccOptimizationTime)
			return 0.f;

		fTimeDelta = m_fAccOptimizationTime;
		m_fAccOptimizationTime = 0.f;
	}

	else if (fDistance < DISTANCE_FPS45)
	{
		if (TIME_FPS45 > m_fAccOptimizationTime)
			return 0.f;

		fTimeDelta = m_fAccOptimizationTime;
		m_fAccOptimizationTime = 0.f;
	}

	else if (fDistance < DISTANCE_FPS30)
	{
		if (TIME_FPS30 > m_fAccOptimizationTime)
			return 0.f;

		fTimeDelta = m_fAccOptimizationTime;
		m_fAccOptimizationTime = 0.f;
	}

	else if (fDistance < DISTANCE_FPS20)
	{
		if (TIME_FPS20 > m_fAccOptimizationTime)
			return 0.f;

		fTimeDelta = m_fAccOptimizationTime;
		m_fAccOptimizationTime = 0.f;
	}

	else if (fDistance < DISTANCE_FPS10)
	{
		if (TIME_FPS10 > m_fAccOptimizationTime)
			return 0.f;

		fTimeDelta = m_fAccOptimizationTime;
		m_fAccOptimizationTime = 0.f;
	}

	// 5프레임
	else if (fDistance < DISTANCE_FPS5)
	{
		if (TIME_FPS5 > m_fAccOptimizationTime)
			return 0.f;

		fTimeDelta = m_fAccOptimizationTime;
		m_fAccOptimizationTime = 0.f;
	}

	else
	{
		if (TIME_FPS1 > m_fAccOptimizationTime)
			return 0.f;

		fTimeDelta = m_fAccOptimizationTime;
		m_fAccOptimizationTime = 0.f;
	}

	return fTimeDelta;
}

void CModel::Set_TickPerSec(const wstring& strAnimLayerTag, _uint iAnimIndex, _float fTickPerSec)
{
	unordered_map<wstring, CAnimation_Layer*>::iterator		iter = { m_AnimationLayers.find(strAnimLayerTag) };

	if (m_AnimationLayers.end() != iter)
	{
		CAnimation* pAnimation = { iter->second->Get_Animation(iAnimIndex) };
		if (nullptr != pAnimation)
		{
			pAnimation->Set_TickPerSec(fTickPerSec);
		}
	}
}

#pragma region RootControll 

_bool CModel::Is_Active_RootMotion_XZ()
{
	return m_isRootMotion_XZ;
}

_bool CModel::Is_Active_RootMotion_Y()
{
	return m_isRootMotion_Y;
}

_bool CModel::Is_Active_RootMotion_Rotation()
{
	return m_isRootMotion_Rotation;
}

void CModel::Active_RootMotion_XZ(_bool isActive)
{
	m_isRootMotion_XZ = isActive;
}

void CModel::Active_RootMotion_Y(_bool isActive)
{
	m_isRootMotion_Y = isActive;
}

void CModel::Active_RootMotion_Rotation(_bool isActive)
{
	m_isRootMotion_Rotation = isActive;
}

void CModel::Set_RootBone(string strBoneTag)
{
	_int		iBoneIndex = { Find_BoneIndex(strBoneTag) };
	if (-1 == iBoneIndex)
		return;

	for (auto& pBone : m_Bones)
		pBone->Set_RootBone(false);

	m_Bones[iBoneIndex]->Set_RootBone(true);
}

_bool CModel::Is_Set_RootBone()
{
	_bool		isSetRoot = { false };

	for (auto& pBone : m_Bones)
	{
		if (true == pBone->Is_RootBone())
			isSetRoot = true;
	}

	return isSetRoot;
}

#pragma endregion

void CModel::Update_LastKeyFrames(const vector<_float4x4>& TransformationMatrices, _uint iPlayingIndex)
{
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };

	if (nullptr != pPlayingInfo)
	{
		_uint			iNumBones = { static_cast<_uint>(m_Bones.size()) };
		pPlayingInfo->Update_LastKeyFrames(TransformationMatrices, iNumBones, m_fTotalLinearTime);
	}
}

HRESULT CModel::RagDoll()
{
	for (auto& pBone : m_Bones)
		pBone->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformationMatrix));

	return S_OK;
}

_vector CModel::Compute_Quaternion_From_TwoDirection(_fvector vSrcDirection, _fvector vDstDirection)
{
	_vector			vResultQuaternion = { XMQuaternionIdentity() };

	_vector			vNormalizeSrcDirection = XMVector3Normalize(vSrcDirection);
	_vector			vNormalizeDstDirection = XMVector3Normalize(vDstDirection);

	_float			fDot = { XMVectorGetX(XMVector3Dot(vNormalizeSrcDirection, vNormalizeDstDirection)) };

	if (fDot < 0.9999f && fDot > -0.9999f)
	{
		_vector			vRotateAxis = { XMVector3Cross(vNormalizeSrcDirection, vNormalizeDstDirection) };

		_float			fClampDot = { std::clamp(fDot, -1.f, 1.f) };
		_float			fAngleRadian = { acosf(fClampDot) };

		vResultQuaternion = XMQuaternionRotationAxis(vRotateAxis, fAngleRadian);
	}

	return vResultQuaternion;
}

#pragma region Access

void CModel::Add_Bone_Layer_Range(const wstring& strLayerTag, const string& strStartBoneTag, const string& strEndBoneTag)
{
	_int			iStartBoneIndex = { Find_BoneIndex(strStartBoneTag) };
	_int			iEndBoneIndex = { Find_BoneIndex(strEndBoneTag) };

	if (0 > iStartBoneIndex || 0 > iEndBoneIndex)
		return;

	Add_Bone_Layer_Range(strLayerTag, static_cast<_uint>(iStartBoneIndex), static_cast<_uint>(iEndBoneIndex));
}

void CModel::Add_Bone_Layer_Range(const wstring& strLayerTag, _uint iStartBoneIndex, _uint iEndBoneIndex)
{
	CBone_Layer* pBoneLayer = { Find_BoneLayer(strLayerTag) };
	if (nullptr != pBoneLayer)
		return;

	_uint				iNumBones = { static_cast<_uint>(m_Bones.size()) };
	if (iStartBoneIndex >= iNumBones ||
		iEndBoneIndex >= iNumBones)
		return;

	unordered_set<_uint>			BoneIndices;
	for (_uint i = iStartBoneIndex; i <= iEndBoneIndex; ++i)
	{
		BoneIndices.emplace(i);
	}

	pBoneLayer = CBone_Layer::Create(BoneIndices);
	if (nullptr == pBoneLayer)
		return;

	m_BoneLayers.emplace(strLayerTag, pBoneLayer);
}

void CModel::Add_Bone_Layer_ChildIndices(const wstring& strLayerTag, const string& strBoneTag)
{
	_int			iBoneIndex = { Find_BoneIndex(strBoneTag) };

	if (0 > iBoneIndex)
		return;

	Add_Bone_Layer_ChildIndices(strLayerTag, iBoneIndex);
}

void CModel::Add_Bone_Layer_ChildIndices(const wstring& strLayerTag, _uint iParentBoneIndex)
{
	CBone_Layer* pBoneLayer = { Find_BoneLayer(strLayerTag) };
	if (nullptr != pBoneLayer)
		return;

	_uint				iNumBones = { static_cast<_uint>(m_Bones.size()) };
	if (iParentBoneIndex >= iNumBones)
		return;

	string			strBoneTag = { m_Bones[iParentBoneIndex]->Get_Name() };

	list<_uint>				ChildBoneIndices;
	Get_Child_BoneIndices(strBoneTag, ChildBoneIndices);

	if (true == ChildBoneIndices.empty())
		return;

	unordered_set<_uint>				BoneIndices;
	for (auto& iChildIndex : ChildBoneIndices)
	{
		BoneIndices.emplace(iChildIndex);
	}

	pBoneLayer = CBone_Layer::Create(BoneIndices);
	if (nullptr == pBoneLayer)
		return;

	m_BoneLayers.emplace(strLayerTag, pBoneLayer);
}

void CModel::Add_Bone_Layer_BoneIndices(const wstring& strLayerTag, const list<_uint>& BoneIndices)
{
	CBone_Layer* pBoneLayer = { Find_BoneLayer(strLayerTag) };
	if (nullptr != pBoneLayer)
		return;

	unordered_set<_uint>			BoneIndicesSet;
	for (auto& iBoneIndex : BoneIndices)
	{
		BoneIndicesSet.emplace(iBoneIndex);
	}

	pBoneLayer = CBone_Layer::Create(BoneIndicesSet);
	if (nullptr == pBoneLayer)
		return;

	m_BoneLayers.emplace(strLayerTag, pBoneLayer);
}

void CModel::Add_Bone_Layer_All_Bone(const wstring& strLayerTag)
{
	CBone_Layer* pBoneLayer = { Find_BoneLayer(strLayerTag) };
	if (nullptr != pBoneLayer)
		return;

	unordered_set<_uint>			BoneIndices;
	for (_uint i = 0; i < static_cast<_uint>(m_Bones.size()); ++i)
		BoneIndices.emplace(i);

	pBoneLayer = CBone_Layer::Create(BoneIndices);
	if (nullptr == pBoneLayer)
		return;

	m_BoneLayers.emplace(strLayerTag, pBoneLayer);
}

void CModel::Erase_Bone_Layer(const wstring& strLayerTag)
{
	map<wstring, CBone_Layer*>::iterator		iter = { m_BoneLayers.find(strLayerTag) };
	if (iter != m_BoneLayers.end())
	{
		Safe_Release(iter->second);
		m_BoneLayers.erase(iter);
	}
}

void CModel::Add_Bone_Layer_Bone(const wstring& strBoneLayerTag, _uint iBoneIndex)
{
	_uint		iNumBones = { static_cast<_uint>(m_Bones.size()) };
	if (iBoneIndex >= iNumBones)
		return;

	CBone_Layer* pBoneLayer = { Find_BoneLayer(strBoneLayerTag) };
	if (nullptr != pBoneLayer)
	{
		pBoneLayer->Add_BoneIndex(iBoneIndex);
	}
}

void CModel::Add_Bone_Layer_Bone(const wstring& strBoneLayerTag, const string& strBoneTag)
{
	_int			iBoneIndex = { Find_BoneIndex(strBoneTag) };
	if (-1 == iBoneIndex)
		return;

	Add_Bone_Layer_Bone(strBoneLayerTag, iBoneIndex);
}

void CModel::Add_IK(const string& strTargetJointTag, const string& strEndEffectorTag, const wstring& strIKTag, _uint iNumIteration, _float fBlend)
{
	if (nullptr == m_pIK_Solver)
		return;

	_int		iIKRootBoneIndex = { Find_BoneIndex(strTargetJointTag) };
	_int		iEndEffectorIndex = { Find_BoneIndex(strEndEffectorTag) };

	_int		iNumBones = { static_cast<_int>(m_Bones.size()) };

	if (iIKRootBoneIndex >= iNumBones ||
		iEndEffectorIndex >= iNumBones)
		return;

	if (-1 == iIKRootBoneIndex || -1 == iEndEffectorIndex)
		return;

	list<_uint>			IKIncludedIndices;
	Get_Child_BoneIndices(strTargetJointTag, IKIncludedIndices);

	m_pIK_Solver->Add_IK(m_Bones, IKIncludedIndices, iIKRootBoneIndex, iEndEffectorIndex, strTargetJointTag, strEndEffectorTag, strIKTag, iNumIteration, fBlend);
}

void CModel::Erase_IK(const wstring& strIKTag)
{
	if (nullptr == m_pIK_Solver)
		return;

	m_pIK_Solver->Erase_IK(strIKTag);
}

void CModel::Set_TargetPosition_IK(const wstring& strIKTag, _fvector vTargetPosition)
{
	if (nullptr == m_pIK_Solver)
		return;

	m_pIK_Solver->Set_TargetPosition_IK(strIKTag, vTargetPosition);
}

void CModel::Set_NumIteration_IK(const wstring& strIKTag, _uint iNumIteration)
{
	if (nullptr == m_pIK_Solver)
		return;

	m_pIK_Solver->Set_NumIteration_IK(strIKTag, iNumIteration);
}

void CModel::Set_Blend_IK(const wstring& strIKTag, _float fBlend)
{
	if (nullptr == m_pIK_Solver)
		return;

	m_pIK_Solver->Set_Blend_IK(strIKTag, fBlend);
}

vector<_float4> CModel::Get_ResultTranslation_IK(const wstring& strIKTag)
{
	if (nullptr == m_pIK_Solver)
		return vector<_float4>();

	return m_pIK_Solver->Get_ResultTranslation_IK(strIKTag);
}

vector<_float4> CModel::Get_OriginTranslation_IK(const wstring& strIKTag)
{
	if (nullptr == m_pIK_Solver)
		return vector<_float4>();

	return m_pIK_Solver->Get_OriginTranslation_IK(strIKTag);
}

vector<_float4x4> CModel::Get_JointCombinedMatrices_IK(const wstring& strIKTag)
{
	if (nullptr == m_pIK_Solver)
		return vector<_float4x4>();

	return m_pIK_Solver->Get_JointCombinedMatrices_IK(strIKTag, m_Bones);
}

#pragma endregion

#pragma region Additional Transformation

void CModel::Add_Additional_Transformation_World(string strBoneTag, _fmatrix AdditionalTransformMatrix)
{
	_int			iBoneIndex = { Find_BoneIndex(strBoneTag) };
	if (-1 == iBoneIndex)
		return;

	if (0 == m_AdditionalForces.size())
		m_AdditionalForces.resize(m_Bones.size());

	_float4x4			AdditionalTransformationFloat4x4;
	XMStoreFloat4x4(&AdditionalTransformationFloat4x4, AdditionalTransformMatrix);

	m_AdditionalForces[iBoneIndex].push_back(AdditionalTransformationFloat4x4);
}

void CModel::Apply_AdditionalForces(CTransform* pTransform)
{
	m_AdditionalForces.resize(m_Bones.size());

	//	_matrix		WorldMatrixInv = { pTransform->Get_WorldMatrix_Inverse() };
	_matrix		WorldMatrixInv = { XMMatrixIdentity() };
	_uint		iNumBones = { static_cast<_uint>(m_Bones.size()) };
	for (_uint iBoneIndex = 0; iBoneIndex < iNumBones; ++iBoneIndex)
	{
		_matrix			CombinedMatrix = { XMLoadFloat4x4(m_Bones[iBoneIndex]->Get_CombinedTransformationMatrix()) };
		_vector			vCurrentScale, vCurrentQuaternion, vCurrentTranslation;
		XMMatrixDecompose(&vCurrentScale, &vCurrentQuaternion, &vCurrentTranslation, CombinedMatrix);

		_vector			vResultQuaternion = { vCurrentQuaternion };
		_vector			vResultTranslation = { vCurrentTranslation };

		if (true == m_AdditionalForces.empty())
			continue;

		for (_float4x4& InputMatrix : m_AdditionalForces[iBoneIndex])
		{
			_matrix		AdditionalMatrix = { XMLoadFloat4x4(&InputMatrix) };
			_matrix		AdditionalMatrixLocal = { AdditionalMatrix * WorldMatrixInv };
			_vector		vAdditionalScale, vAdditionalQuaternion, vAdditionalTranslation;

			XMMatrixDecompose(&vAdditionalScale, &vAdditionalQuaternion, &vAdditionalTranslation, AdditionalMatrixLocal);

			//	일단 스케일은 가산하지않고 무시한다.
			vResultQuaternion = { XMQuaternionMultiply(XMQuaternionNormalize(vCurrentQuaternion), XMQuaternionNormalize(vAdditionalQuaternion)) };
			vResultTranslation = { XMVectorSetW(vCurrentTranslation + vAdditionalTranslation, 1.f) };
		}

		_matrix			ResultMatrix = { XMMatrixAffineTransformation(vCurrentScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vResultQuaternion, vResultTranslation) };
		m_Bones[iBoneIndex]->Set_Combined_Matrix(ResultMatrix);
	}

	Clear_AdditionalForces();
}

void CModel::Apply_AdditionalForce(_uint iBoneIndex)
{
	m_AdditionalForces.resize(m_Bones.size());

	_matrix			WorldMatrixInv = { XMMatrixIdentity() };
	_matrix			CombinedMatrix = { XMLoadFloat4x4(m_Bones[iBoneIndex]->Get_CombinedTransformationMatrix()) };
	_vector			vCurrentScale, vCurrentQuaternion, vCurrentTranslation;
	XMMatrixDecompose(&vCurrentScale, &vCurrentQuaternion, &vCurrentTranslation, CombinedMatrix);

	_vector			vResultQuaternion = { vCurrentQuaternion };
	_vector			vResultTranslation = { vCurrentTranslation };

	if (true == m_AdditionalForces.empty())
		return;

	for (_float4x4& InputMatrix : m_AdditionalForces[iBoneIndex])
	{
		_matrix		AdditionalMatrix = { XMLoadFloat4x4(&InputMatrix) };
		_matrix		AdditionalMatrixLocal = { AdditionalMatrix * WorldMatrixInv };
		_vector		vAdditionalScale, vAdditionalQuaternion, vAdditionalTranslation;

		XMMatrixDecompose(&vAdditionalScale, &vAdditionalQuaternion, &vAdditionalTranslation, AdditionalMatrixLocal);

		//	일단 스케일은 가산하지않고 무시한다.
		vResultQuaternion = { XMQuaternionMultiply(XMQuaternionNormalize(vCurrentQuaternion), XMQuaternionNormalize(vAdditionalQuaternion)) };
		vResultTranslation = { XMVectorSetW(vCurrentTranslation + vAdditionalTranslation, 1.f) };
	}
	m_AdditionalForces[iBoneIndex].clear();

	_matrix			ResultMatrix = { XMMatrixAffineTransformation(vCurrentScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vResultQuaternion, vResultTranslation) };
	m_Bones[iBoneIndex]->Set_Combined_Matrix(ResultMatrix);
}

void CModel::Clear_AdditionalForces()
{
	m_AdditionalForces.clear();
	m_AdditionalForces.resize(m_Bones.size());
}

#pragma endregion

_int CModel::Find_RootBoneIndex()
{
	_int iRootBoneIndex = { -1 };

	for (auto& pBone : m_Bones)
	{
		++iRootBoneIndex;

		if (true == pBone->Is_RootBone())
		{
			return iRootBoneIndex;
		}
	}

	return -1;
}

vector<_float4x4> CModel::Initialize_ResultMatrices_Blend(const unordered_set<_uint>& IncludedBoneIndices)
{
	vector<_float4x4>		ResultMatrices;
	ResultMatrices.resize(m_Bones.size());

	//	이전 저장 된 변환 행렬들을 전부 저장한다.
	const _uint			iNumBones = { static_cast<_uint>(m_Bones.size()) };
	for (_uint iBoneIndex = 0; iBoneIndex < iNumBones; ++iBoneIndex)
	{
		XMStoreFloat4x4(&ResultMatrices[iBoneIndex], m_Bones[iBoneIndex]->Get_TrasformationMatrix());
	}

	//	이번에 애니메이션의 영향을 받은 뼈들은 전부 항등행렬로 재초기화한다.
	for (_uint iBoneIndex : IncludedBoneIndices)
	{
		XMStoreFloat4x4(&ResultMatrices[iBoneIndex], XMMatrixIdentity());
	}

	return ResultMatrices;
}

_float CModel::Compute_Current_TotalBlendWeight(_uint iBoneIndex)
{
	_float		fTotalWeight = { 0.f };
	for (auto& pPlayingInfo : m_PlayingAnimInfos)
	{
		if (nullptr == pPlayingInfo)
			continue;

		if (false == pPlayingInfo->Is_Set_CurrentAnimation())
			continue;

		if (true == pPlayingInfo->Is_AdditionalMasking())
			continue;
		wstring		strBoneLayerTag = { pPlayingInfo->Get_BoneLayerTag() };
		if (false == m_BoneLayers[strBoneLayerTag]->Is_Included(iBoneIndex))
			continue;

		_float		fBlendWeight = { pPlayingInfo->Get_BlendWeight() };
		if (0.f >= fBlendWeight)
			continue;

		fTotalWeight += fBlendWeight;
	}

	return fTotalWeight;
}

void CModel::Compute_Current_TotalBlendWeights()
{
	m_TotalWeights.clear();
	_uint						iNumBones = { static_cast<_uint>(m_Bones.size()) };
	m_TotalWeights.resize(iNumBones);

	for (_uint iBoneIndex = 0; iBoneIndex < iNumBones; ++iBoneIndex)
	{
		_float			fTotalWeight = { Compute_Current_TotalBlendWeight(iBoneIndex) };

		m_TotalWeights[iBoneIndex] = fTotalWeight;
	}
}

_float4x4 CModel::Compute_BlendTransformation_Additional(_fmatrix SrcMatrix, _fmatrix DstMatrix, _float fAdditionalWeight)
{
	_vector			vSrcScale, vSrcQuaternion, vSrcTranslation;
	XMMatrixDecompose(&vSrcScale, &vSrcQuaternion, &vSrcTranslation, SrcMatrix);

	_vector			vDstScale, vDstQuaternion, vDstTranslation;
	XMMatrixDecompose(&vDstScale, &vDstQuaternion, &vDstTranslation, DstMatrix);

	vDstQuaternion = XMQuaternionSlerp(XMQuaternionIdentity(), XMQuaternionNormalize(vDstQuaternion), fAdditionalWeight);
	vDstTranslation *= fAdditionalWeight;

	_vector			vResultScale = { vDstScale };
	_vector			vResultQuaternion = { XMQuaternionMultiply(XMQuaternionNormalize(vSrcQuaternion), XMQuaternionNormalize(vDstQuaternion)) };
	_vector			vResultTranslation = { XMVectorSetW(XMVectorSetW(vSrcTranslation, 0.f) + XMVectorSetW(vDstTranslation,0.f), 1.f) };

	_matrix			ResultMatrix = { XMMatrixAffineTransformation(vResultScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vResultQuaternion, vResultTranslation) };
	_float4x4		ResultFloat4x4;

	XMStoreFloat4x4(&ResultFloat4x4, ResultMatrix);

	return ResultFloat4x4;
}

unordered_set<_uint> CModel::Compute_IncludedBoneIndices_AllBoneLayer_Blend()
{
	unordered_set<_uint>					ResultIncludedBoneIndices;
	for (auto& pPlayingInfo : m_PlayingAnimInfos)
	{
		if (nullptr == pPlayingInfo)
			continue;

		if (true == pPlayingInfo->Is_AdditionalMasking())
			continue;

		_int							iAnimIndex = { pPlayingInfo->Get_AnimIndex() };
		wstring							strAnimLayerTag = { pPlayingInfo->Get_AnimLayerTag() };
		_float							fWeight = { pPlayingInfo->Get_BlendWeight() };
		if (-1 == iAnimIndex || TEXT("") == strAnimLayerTag || 0.f >= fWeight)
			continue;

		wstring			strBoneLayerTag = { pPlayingInfo->Get_BoneLayerTag() };
		CBone_Layer* pBoneLayer = { Find_BoneLayer(strBoneLayerTag) };

		if (nullptr == pBoneLayer)
			continue;

		unordered_set<_uint>			TempIncludedBoneIndices = { pBoneLayer->Get_IncludedBoneIndices() };

		for (auto& iIndex : TempIncludedBoneIndices)
		{
			ResultIncludedBoneIndices.emplace(iIndex);
		}
	}



	return ResultIncludedBoneIndices;
}

unordered_set<_uint> CModel::Compute_IncludedBoneIndices_AllBoneLayer_AdditionalMasking()
{
	unordered_set<_uint>			ResultIncludedBoneIndices;
	for (auto& pPlayingInfo : m_PlayingAnimInfos)
	{
		if (nullptr == pPlayingInfo)
			continue;

		if (false == pPlayingInfo->Is_AdditionalMasking())
			continue;

		_int							iAnimIndex = { pPlayingInfo->Get_AnimIndex() };
		wstring							strAnimLayerTag = { pPlayingInfo->Get_AnimLayerTag() };
		_float							fWeight = { pPlayingInfo->Get_BlendWeight() };
		if (-1 == iAnimIndex || TEXT("") == strAnimLayerTag || 0.f >= fWeight)
			continue;

		wstring			strBoneLayerTag = { pPlayingInfo->Get_BoneLayerTag() };
		CBone_Layer* pBoneLayer = { Find_BoneLayer(strBoneLayerTag) };

		if (nullptr == pBoneLayer)
			continue;

		unordered_set<_uint>			TempIncludedBoneIndices = { pBoneLayer->Get_IncludedBoneIndices() };
		unordered_set<_uint>			EnoughKeyFrameBoneIndices;


		vector<CChannel*>			Channels = { m_AnimationLayers[strAnimLayerTag]->Get_Animations()[iAnimIndex]->Get_Channels() };
		for (auto& pChannel : Channels)
		{
			_uint			iNumKeyFrame = { pChannel->Get_Num_KeyFrame() };
			if (true == pPlayingInfo->Is_Enough_NumKeyFrame(iNumKeyFrame))
			{
				EnoughKeyFrameBoneIndices.emplace(pChannel->Get_BoneIndex());
			}
		}

		for (auto& iterSrc = TempIncludedBoneIndices.begin(); iterSrc != TempIncludedBoneIndices.end(); )
		{
			unordered_set<_uint>::iterator			iterDst = { EnoughKeyFrameBoneIndices.find(*iterSrc) };
			if (iterDst == EnoughKeyFrameBoneIndices.end())
			{
				iterSrc = TempIncludedBoneIndices.erase(iterSrc);
			}
			else
				++iterSrc;
		}

		for (auto& iIndex : TempIncludedBoneIndices)
		{
			ResultIncludedBoneIndices.emplace(iIndex);
		}
	}

	return ResultIncludedBoneIndices;
}

_float4x4 CModel::GetBoneTransform(string strBoneTag)
{
	_int			iBoneIndex = { Find_BoneIndex(strBoneTag) };
	iBoneIndex = 86;
	if (-1 == iBoneIndex)
		return _float4x4();

	auto CombinedFloat4x4 = m_Bones[iBoneIndex]->Get_CombinedTransformationMatrix();

	return *CombinedFloat4x4;
}

_float4x4 CModel::GetBoneTransform(_int Index)
{
	auto CombinedFloat4x4 = m_Bones[Index]->Get_CombinedTransformationMatrix();

	return *CombinedFloat4x4;
}

void CModel::Apply_RootMotion_Rotation(CTransform* pTransform)
{
	_vector			vTotalDeltaQuaternion = { XMQuaternionIdentity() };
	_vector			vIdentityQuaternion = { XMQuaternionIdentity() };
	_uint			iRootIndex = { static_cast<_uint>(Find_RootBoneIndex()) };
	_float			fTotalWeight = { Compute_Current_TotalBlendWeight(iRootIndex) };

	if (0.f >= fTotalWeight)
		return;

	for (auto& pPlayingInfo : m_PlayingAnimInfos)
	{
		if (nullptr == pPlayingInfo)
			continue;

		if (false == pPlayingInfo->Is_Set_CurrentAnimation())
			continue;

		_float			fBlendWeight = { pPlayingInfo->Get_BlendWeight() };
		_float			fBlendWeightRatio = { fBlendWeight / fTotalWeight };

		_vector			vCurrentRotation = { XMLoadFloat4(&pPlayingInfo->Get_CurrentQuaternion()) };
		_vector			vPreRotation = { XMLoadFloat4(&pPlayingInfo->Get_PreQuaternion()) };

		_vector			vPreRotationInv = { XMQuaternionInverse(XMQuaternionNormalize(vPreRotation)) };
		_vector			vDeltaQuaternion = { XMQuaternionMultiply(XMQuaternionNormalize(vPreRotationInv), XMQuaternionNormalize(vCurrentRotation)) };

		vDeltaQuaternion = XMQuaternionSlerp(XMQuaternionIdentity(), vDeltaQuaternion, fBlendWeightRatio);

		vTotalDeltaQuaternion = XMQuaternionMultiply(vTotalDeltaQuaternion, vDeltaQuaternion);

		pPlayingInfo->Set_PreQuaternion(vCurrentRotation);
	}

	_vector				vRootRotateAxis = { XMVectorSetW(vTotalDeltaQuaternion, 0.f) };
	vRootRotateAxis = { XMVector3TransformNormal(vRootRotateAxis, XMLoadFloat4x4(&m_TransformationMatrix)) };
	vTotalDeltaQuaternion = { XMVectorSetW(vRootRotateAxis, XMVectorGetW(vTotalDeltaQuaternion)) };

	//	_vector				vScale = { XMLoadFloat3(&pTransform->Get_Scaled()) };

	_matrix			RotationMatrix = { XMMatrixRotationQuaternion(XMQuaternionNormalize(vTotalDeltaQuaternion)) };
	_matrix			WorldMatrix = { pTransform->Get_WorldMatrix() };
	//	RotationMatrix.r[CTransform::STATE_POSITION].m128_f32[3] = 0.f;

	_vector			vPosition = { WorldMatrix.r[CTransform::STATE_POSITION] };
	WorldMatrix.r[CTransform::STATE_POSITION] = XMVectorSetW(XMVectorZero(), 1.f);

	_matrix			ResultMatrix = { XMMatrixMultiply(WorldMatrix, RotationMatrix) };
	ResultMatrix.r[CTransform::STATE_POSITION] = vPosition;

	pTransform->Set_WorldMatrix(ResultMatrix);
	//	pTransform->Set_Scaled(XMVectorGetX(vScale), XMVectorGetY(vScale), XMVectorGetZ(vScale));
}

void CModel::Apply_RootMotion_Translation(CTransform* pTransform, _float3* pMovedDirection)
{
	_vector				vTotalDeltaTranslation = { XMVectorZero() };
	_uint				iRootIndex = { static_cast<_uint>(Find_RootBoneIndex()) };
	_float				fTotalWeight = { Compute_Current_TotalBlendWeight(iRootIndex) };

	for (auto& pPlayingInfo : m_PlayingAnimInfos)
	{
		if (nullptr == pPlayingInfo)
			continue;

		if (false == pPlayingInfo->Is_Set_CurrentAnimation())
			continue;

		_float			fBlendWeight = { pPlayingInfo->Get_BlendWeight() };
		_float			fBlendWeightRatio = { fBlendWeight / fTotalWeight };

		_vector			vCurrentTranslation = { XMLoadFloat3(&pPlayingInfo->Get_CurrentTranslation_Local()) };
		_vector			vPreTranslation = { XMLoadFloat3(&pPlayingInfo->Get_PreTranslation_Local()) };

		if (true == m_isRootMotion_Rotation)
		{
			//	변위량은 현재 누적된 => 루트에 씌워지게된 총회전량인 이번회전량을 이용하여 역회전을 하여 델타에 적용해야한다.
			_vector			vCurrentQuaternion = { XMLoadFloat4(&pPlayingInfo->Get_CurrentQuaternion()) };
			_vector			vCurrentQuaternionInv = { XMQuaternionInverse(vCurrentQuaternion) };

			_matrix			InverseCurrentRotationMatrix = { XMMatrixRotationQuaternion(vCurrentQuaternionInv) };

			vCurrentTranslation = XMVector3TransformNormal(vCurrentTranslation, InverseCurrentRotationMatrix);
			vPreTranslation = XMVector3TransformNormal(vPreTranslation, InverseCurrentRotationMatrix);
		}
		_vector			vDeltaTranslation = { vCurrentTranslation - vPreTranslation };

		vDeltaTranslation = vDeltaTranslation * fBlendWeightRatio;
		vTotalDeltaTranslation = vTotalDeltaTranslation + vDeltaTranslation;

		pPlayingInfo->Set_PreTranslation(XMLoadFloat3(&pPlayingInfo->Get_CurrentTranslation_Local()));
	}

	_matrix			WorldMatrix = { pTransform->Get_WorldMatrix() };

	vTotalDeltaTranslation = { XMVector3TransformNormal(vTotalDeltaTranslation, XMLoadFloat4x4(&m_TransformationMatrix)) };
	vTotalDeltaTranslation = { XMVector3TransformNormal(vTotalDeltaTranslation, WorldMatrix) };

	_vector			vMoveDirection = { XMLoadFloat3(pMovedDirection) };
	vMoveDirection += vTotalDeltaTranslation;
	XMStoreFloat3(pMovedDirection, vMoveDirection);
}

void CModel::Apply_Translation_Inverse_Rotation(_fvector vTranslation)
{
	if (false == m_isRootMotion_Rotation)
		return;

	if (true == m_isRootMotion_XZ || true == m_isRootMotion_Y)
		return;
}

void CModel::Set_CombinedMatrix(string strBoneTag, _fmatrix CombinedMatrix)
{
	_int			iBoneIndex = { Find_BoneIndex(strBoneTag) };
	if (-1 == iBoneIndex)
		return;

	m_Bones[iBoneIndex]->Set_Combined_Matrix(CombinedMatrix);
}

void CModel::Set_Parent_CombinedMatrix_Ptr(string strBoneTag, _float4x4* pParentMatrix)
{
	_int			iBoneIndex = { Find_BoneIndex(strBoneTag) };
	if (-1 == iBoneIndex)
		return;

	m_Bones[iBoneIndex]->Set_Parent_CombinedMatrix_Ptr(pParentMatrix);
}

void CModel::Set_Surbodinate(string strBoneTag, _bool isSurbodinate)
{
	_int			iBoneIndex = { Find_BoneIndex(strBoneTag) };
	if (-1 == iBoneIndex)
		return;

	m_Bones[iBoneIndex]->Set_Surbodinate(isSurbodinate);
}

#pragma region MeshControll 

_bool CModel::Is_Hide_Mesh(string strMeshTag)
{
	_int			iMeshIndex = { Find_Mesh_Index(strMeshTag) };
	if (-1 == iMeshIndex)
		return false;

	return m_IsHideMesh[iMeshIndex];
}

_bool CModel::Is_Hide_Mesh(_uint iMeshIndex)
{
	_uint			iNumMeshes = { static_cast<_uint>(m_Meshes.size()) };
	if (iMeshIndex >= iNumMeshes)
		return false;

	return m_IsHideMesh[iMeshIndex];
}

void CModel::Hide_Mesh_Branch(_uint iMeshBranch, _bool isHide)
{
	_uint			iMeshIndex = { 0 };
	for (auto& iBranch : m_MeshBranches)
	{
		if (iBranch == iMeshBranch)
		{
			m_IsHideMesh[iMeshIndex] = isHide;
		}

		++iMeshIndex;
	}
}

void CModel::Hide_Mesh(_uint iMeshIndex, _bool isHide)
{
	_uint			iNumMesh = { static_cast<_uint>(m_Meshes.size()) };
	if (iNumMesh <= iMeshIndex)
		return;

	m_IsHideMesh[iMeshIndex] = isHide;
}

void CModel::Hide_Mesh(string strMeshTag, _bool isHide)
{
	_int			iIndex = { Find_Mesh_Index(strMeshTag) };
	if (-1 == iIndex)
		return;

	m_IsHideMesh[iIndex] = isHide;
}

_int CModel::Find_Mesh_Index(string strMeshTag)
{
	_uint			iNumMeshes = { static_cast<_uint>(m_Meshes.size()) };
	for (_uint iIndex = 0; iIndex < iNumMeshes; ++iIndex)
	{
		string			strSrcMeshTag = { m_Meshes[iIndex]->Get_MeshName() };
		if (strSrcMeshTag == strMeshTag)
			return iIndex;
	}

	return -1;
}

string CModel::Find_Mesh_Tag(_uint iMeshIndex)
{
	_uint			iNumMeshes = { static_cast<_uint>(m_Meshes.size()) };
	string			strMeshTag = { "" };

	if (iMeshIndex < iNumMeshes)
	{
		strMeshTag = m_Meshes[iMeshIndex]->Get_MeshName();
	}

	return strMeshTag;
}

list<_uint> CModel::Get_NonHideMeshIndices()
{
	_uint			iNumMeshes = { static_cast<_uint>(m_Meshes.size()) };
	list<_uint>		MeshIndices;
	for (_uint iIndex = 0; iIndex < iNumMeshes; ++iIndex)
	{
		if (false == m_IsHideMesh[iIndex])
		{
			MeshIndices.push_back(iIndex);
		}
	}

	return MeshIndices;
}

_float4 CModel::Get_Mesh_Local_Pos(string strMeshTag)
{
	_int			iIndex = { Find_Mesh_Index(strMeshTag) };
	if (-1 == iIndex)
		return _float4();

	return _float4(m_Meshes[iIndex]->Get_CenterPoint().x, m_Meshes[iIndex]->Get_CenterPoint().y, m_Meshes[iIndex]->Get_CenterPoint().z, 1.f);
}

#pragma endregion

#pragma region Access2

vector<string> CModel::Get_BoneNames()
{
	vector<string> BoneNames;

	for (auto Bone : m_Bones)
	{
		BoneNames.push_back(Bone->Get_Name());
	}

	return BoneNames;
}

vector<_float4> CModel::Get_Translations()
{
	vector<_float4> Translations;

	for (auto Bone : m_Bones)
	{
		Translations.push_back(Bone->Get_Translation());
	}

	return Translations;
}

vector<string> CModel::Get_MeshTags()
{
	vector<string>		MeshTags;
	for (auto& pMesh : m_Meshes)
	{
		const string		strMeshTag = pMesh->Get_MeshName();
		MeshTags.emplace_back(strMeshTag);
	}

	return MeshTags;
}

list<_uint> CModel::Get_MeshIndices(const string& strMeshTag)
{
	list<_uint>			MeshIndices;
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		if (m_Meshes[i]->Get_MeshName() == strMeshTag)
			MeshIndices.push_back(i);
	}

	return MeshIndices;
}

vector<CAnimation*> CModel::Get_Animations(const wstring& strAnimLayerTag)
{
	unordered_map<wstring, CAnimation_Layer*>::iterator		iter = { m_AnimationLayers.find(strAnimLayerTag) };
	if (m_AnimationLayers.end() != iter)
	{
		return iter->second->Get_Animations();
	}

	return vector<CAnimation*>();
}

list<wstring> CModel::Get_BoneLayer_Tags()
{
	list <wstring>		BoneLayerTags;
	for (auto& Pair : m_BoneLayers)
	{
		wstring			BoneLayerTag = { Pair.first };

		BoneLayerTags.push_back(BoneLayerTag);
	}

	return BoneLayerTags;
}

list<wstring> CModel::Get_AnimationLayer_Tags()
{
	list <wstring>		AnimLayerTags;
	for (auto& Pair : m_AnimationLayers)
	{
		wstring			AnimLayerTag = { Pair.first };

		AnimLayerTags.push_back(AnimLayerTag);
	}

	return AnimLayerTags;
}

list<string> CModel::Get_Animation_Tags(const wstring& strAnimLayerTag)
{
	list<string>		AnimationTags;
	unordered_map<wstring, CAnimation_Layer*>::iterator			iter = { m_AnimationLayers.find(strAnimLayerTag) };
	if (m_AnimationLayers.end() != iter)
	{
		vector<CAnimation*>				Animations = { iter->second->Get_Animations() };
		for (auto& pAnimation : Animations)
		{
			AnimationTags.emplace_back(pAnimation->Get_Name());
		}
	}

	return AnimationTags;
}

_int CModel::Get_Mesh_Branch(_uint iMeshIndex)
{
	_uint			iNumMeshes = { static_cast<_uint>(m_Meshes.size()) };
	if (iNumMeshes <= iMeshIndex)
		return -1;

	return m_MeshBranches[iMeshIndex];
}

_int CModel::Get_Mesh_Branch(const string& strMeshTag)
{
	_uint			iNumMeshes = { static_cast<_uint>(m_Meshes.size()) };
	for (_uint iMeshIndex = 0; iMeshIndex < iNumMeshes; ++iMeshIndex)
	{
		CMesh* pMesh = { m_Meshes[iMeshIndex] };
		if (nullptr == pMesh)
			continue;

		if (strMeshTag == pMesh->Get_MeshName())
		{
			return Get_Mesh_Branch(iMeshIndex);
		}
	}

	return -1;
}

_bool CModel::Find_AnimIndex(_int* pAnimIndex, wstring* pAnimLayerTag, CAnimation* pAnimation)
{
	_int		iAnimIndex = { -1 };
	_bool		isFind = { false };
	wstring		strAnimLayerTag = { TEXT("") };

	for (auto& PairLayer : m_AnimationLayers)
	{
		CAnimation_Layer* pAnimationLayer = { PairLayer.second };
		if (nullptr == pAnimationLayer)
			continue;

		const vector<CAnimation*>& Animations = { pAnimationLayer->Get_Animations() };

		_uint		iIndex = { 0 };
		for (auto& pDstAnimation : Animations)
		{
			if (pAnimation == pDstAnimation)
			{
				isFind = true;
				strAnimLayerTag = PairLayer.first;
				break;
			}

			if (true == isFind)
				iAnimIndex = iIndex;

			++iIndex;
		}
	}

	*pAnimIndex = iAnimIndex;
	*pAnimLayerTag = strAnimLayerTag;


	return isFind;
}

_bool CModel::Find_AnimIndex(_int* pAnimIndex, wstring* pAnimLayerTag, const string& strAnimTag)
{
	_int		iAnimIndex = { -1 };
	_bool		isFind = { false };
	wstring		strAnimLayerTag = { TEXT("") };

	for (auto& PairLayer : m_AnimationLayers)
	{
		CAnimation_Layer* pAnimationLayer = { PairLayer.second };
		if (nullptr == pAnimationLayer)
			continue;

		const vector<CAnimation*>& Animations = { pAnimationLayer->Get_Animations() };

		_uint		iIndex = { 0 };
		for (auto& pDstAnimation : Animations)
		{
			string			strDestAnimTag = { pDstAnimation->Get_Name() };
			if (strDestAnimTag == strAnimTag)
			{
				isFind = true;
				strAnimLayerTag = PairLayer.first;
				break;
			}

			++iIndex;
		}

		if (true == isFind)
		{
			iAnimIndex = iIndex;
			break;
		}

	}

	*pAnimIndex = iAnimIndex;
	*pAnimLayerTag = strAnimLayerTag;


	return isFind;
}

string CModel::Find_RootBoneTag()
{
	string			strBoneTag = { "" };
	for (auto& pBone : m_Bones)
	{
		if (true == pBone->Is_RootBone())
		{
			strBoneTag = pBone->Get_Name();
		}
	}

	return strBoneTag;
}

CPlayingInfo* CModel::Find_PlayingInfo(_uint iPlayingIndex)
{
	CPlayingInfo* pPlayingInfo = { nullptr };
	_uint				iNumPlayingInfo = { static_cast<_uint>(m_PlayingAnimInfos.size()) };
	if (iNumPlayingInfo > iPlayingIndex)
	{
		pPlayingInfo = m_PlayingAnimInfos[iPlayingIndex];
	}

	return pPlayingInfo;
}

CBone_Layer* CModel::Find_BoneLayer(const wstring& strBoneLayerTag)
{
	map<wstring, CBone_Layer*>::iterator		iter = { m_BoneLayers.find(strBoneLayerTag) };
	if (iter == m_BoneLayers.end())
		return nullptr;

	return iter->second;
}

HRESULT CModel::Link_Bone_Auto(CModel* pTargetModel)
{
	if (nullptr == pTargetModel)
		return E_FAIL;

	if (this == pTargetModel)
		return E_FAIL;

	vector<string>			SrcBoneTags = { Get_BoneNames() };
	vector<string>			DstBoneTags = { pTargetModel->Get_BoneNames() };

	sort(SrcBoneTags.begin(), SrcBoneTags.end());
	sort(DstBoneTags.begin(), DstBoneTags.end());

	vector<string>			IntersectionBoneTags;
	IntersectionBoneTags.resize(min(SrcBoneTags.size(), DstBoneTags.size()));

	vector<string>::iterator			iter = { set_intersection(SrcBoneTags.begin(), SrcBoneTags.end(), DstBoneTags.begin(), DstBoneTags.end(), IntersectionBoneTags.begin()) };

	IntersectionBoneTags.resize(iter - IntersectionBoneTags.begin());

	for (auto& strIntersectBoneTag : IntersectionBoneTags)
	{
		_float4x4* pDstCombiendMatrix = { const_cast<_float4x4*>(pTargetModel->Get_CombinedMatrix(strIntersectBoneTag)) };
		if (nullptr == pDstCombiendMatrix)
			continue;

		Set_Surbodinate(strIntersectBoneTag, true);
		Set_Parent_CombinedMatrix_Ptr(strIntersectBoneTag, pDstCombiendMatrix);
	}

	return S_OK;
}

_int CModel::Get_BoneIndex(const string& strBoneTag)
{
	_int			iBoneIndex = { Find_BoneIndex(strBoneTag) };
	return iBoneIndex;
}

HRESULT CModel::Linked_Bone_Indices(CModel* pTargetModel, vector<_int>& LinkedBoneIndices)
{
	if (nullptr == pTargetModel)
		return E_FAIL;

	if (this == pTargetModel)
		return E_FAIL;

	vector<string>			SrcBoneTags = { Get_BoneNames() };
	vector<string>			DstBoneTags = { pTargetModel->Get_BoneNames() };

	sort(SrcBoneTags.begin(), SrcBoneTags.end());
	sort(DstBoneTags.begin(), DstBoneTags.end());

	vector<string>			IntersectionBoneTags;
	IntersectionBoneTags.resize(min(SrcBoneTags.size(), DstBoneTags.size()));

	vector<string>::iterator			iter = { set_intersection(SrcBoneTags.begin(), SrcBoneTags.end(), DstBoneTags.begin(), DstBoneTags.end(), IntersectionBoneTags.begin()) };

	IntersectionBoneTags.resize(iter - IntersectionBoneTags.begin());

	for (auto& strIntersectBoneTag : IntersectionBoneTags)
	{
		_float4x4* pDstCombiendMatrix = { const_cast<_float4x4*>(pTargetModel->Get_CombinedMatrix(strIntersectBoneTag)) };
		if (nullptr == pDstCombiendMatrix)
			continue;

		Set_Surbodinate(strIntersectBoneTag, true);
		Set_Parent_CombinedMatrix_Ptr(strIntersectBoneTag, pDstCombiendMatrix);
	}

	return S_OK;
}

_float4 CModel::Invalidate_RootNode(const string& strRoot)
{
	for (auto& Bone : m_Bones)
	{
		if (string(Bone->Get_Name()) == strRoot)
		{
			return Bone->Get_Translation();
		}
	}

	return _float4{ 0.f, 0.f, 0.f, 0.f };
}

_bool CModel::Is_Surbodinate_Bone(const string& strBoneTag)
{
	_int			iBoneIndex = { Find_BoneIndex(strBoneTag) };
	if (-1 == iBoneIndex)
		return false;

	return m_Bones[iBoneIndex]->Is_Surbodinate();
}

_bool CModel::Is_Root_Bone(const string& strBoneTag)
{
	_int			iBoneIndex = { Find_BoneIndex(strBoneTag) };
	if (-1 == iBoneIndex)
		return false;

	return m_Bones[iBoneIndex]->Is_RootBone();
}

_uint CModel::Get_CurrentMaxKeyFrameIndex(_uint iPlayingIndex)
{
	_uint		iMaxIndex = { 0 };

	if (static_cast<_uint>(m_PlayingAnimInfos.size()) <= iPlayingIndex)
	{
		return iMaxIndex;
	}

	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	if (nullptr != pPlayingInfo)
	{
		const vector<_uint>			KeyFrameIndices = { pPlayingInfo->Get_KeyFrameIndices() };
		for (_uint iIndex : KeyFrameIndices)
		{
			if (iIndex > iMaxIndex)
				iMaxIndex = iIndex;
		}
	}

	return iMaxIndex;
}

_float CModel::Get_Duration_From_Anim(const wstring& strAnimLayerTag, _uint iAnimIndex)
{
	_float			fDuration = { 0.f };

	unordered_map<wstring, CAnimation_Layer*>::iterator		iter = { m_AnimationLayers.find(strAnimLayerTag) };
	if (iter == m_AnimationLayers.end())
		return fDuration;

	if (iAnimIndex >= iter->second->Get_NumAnims())
		return fDuration;

	fDuration = iter->second->Get_Animation(iAnimIndex)->Get_Duration();

	return fDuration;
}

_float CModel::Get_Duration_From_Anim(const wstring& strAnimLayerTag, const string& strAnimTag)
{
	_float			fDuration = { 0.f };

	unordered_map<wstring, CAnimation_Layer*>::iterator		iter = { m_AnimationLayers.find(strAnimLayerTag) };
	if (iter == m_AnimationLayers.end())
		return fDuration;

	const vector<CAnimation*>& Animations = { iter->second->Get_Animations() };
	for (auto& pAnimation : Animations)
	{
		if (pAnimation->Get_Name() == strAnimTag)
		{
			fDuration = pAnimation->Get_Duration();
			break;
		}
	}

	return fDuration;
}

_float CModel::Get_Duration_From_PlayingInfo(_uint iPlayingIndex)
{
	_float					fDuration = { 0.f };
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	if (nullptr != pPlayingInfo)
	{
		_int				iAnimIndex = { pPlayingInfo->Get_AnimIndex() };
		wstring				strAnimLayerTag = { pPlayingInfo->Get_AnimLayerTag() };

		fDuration = Get_Duration_From_Anim(strAnimLayerTag, iAnimIndex);
	}

	return fDuration;
}

_float CModel::Get_TickPerSec_From_Anim(const wstring& strAnimLayerTag, _uint iAnimIndex)
{
	_float			fTickPerSec = { 0.f };

	unordered_map<wstring, CAnimation_Layer*>::iterator		iter = { m_AnimationLayers.find(strAnimLayerTag) };
	if (iter == m_AnimationLayers.end())
		return fTickPerSec;

	if (iAnimIndex >= iter->second->Get_NumAnims())
		return fTickPerSec;

	fTickPerSec = iter->second->Get_Animation(iAnimIndex)->Get_TickPerSec();

	return fTickPerSec;
}

_float CModel::Get_TickPerSec_From_Anim(const wstring& strAnimLayerTag, const string& strAnimTag)
{
	_float			fTickPerSec = { 0.f };

	unordered_map<wstring, CAnimation_Layer*>::iterator		iter = { m_AnimationLayers.find(strAnimLayerTag) };
	if (iter == m_AnimationLayers.end())
		return fTickPerSec;

	const vector<CAnimation*>& Animations = { iter->second->Get_Animations() };
	for (auto& pAnimation : Animations)
	{
		if (pAnimation->Get_Name() == strAnimTag)
		{
			fTickPerSec = pAnimation->Get_TickPerSec();
			break;
		}
	}

	return fTickPerSec;
}

_float CModel::Get_TickPerSec_From_PlayingInfo(_uint iPlayingIndex)
{
	_float					fTIckPerSec = { 0.f };
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	if (nullptr != pPlayingInfo)
	{
		_int				iAnimIndex = { pPlayingInfo->Get_AnimIndex() };
		wstring				strAnimLayerTag = { pPlayingInfo->Get_AnimLayerTag() };

		fTIckPerSec = Get_TickPerSec_From_Anim(strAnimLayerTag, iAnimIndex);
	}

	return fTIckPerSec;
}

CBone* CModel::Get_BonePtr(const _char* pBoneName) const
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			return pBone->Compare_Name(pBoneName);
		});

	if (iter != m_Bones.end())
		return *iter;
	else
		return nullptr;
}

CBone* CModel::Get_BonePtr(_int iIndex)
{
	return m_Bones[iIndex];
}

_bool CModel::isFinished(_uint iPlayingIndex)
{
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	_bool					isFinished = { false };

	if (nullptr != pPlayingInfo)
	{
		isFinished = pPlayingInfo->Is_Finished();
	}

	return isFinished;
}

void CModel::Get_Child_BoneIndices(string strTargetParentsBoneTag, list<_uint>& ChildBoneIndices)
{
	for (auto& pBone : m_Bones)
	{
		_int		iParentsIndex = { pBone->Get_ParentIndex() };
		if (-1 == iParentsIndex)
			continue;

		string		strMyParentsBoneTag = { m_Bones[iParentsIndex]->Get_Name() };

		if (strMyParentsBoneTag == strTargetParentsBoneTag)
		{
			string		strBoneTag = { pBone->Get_Name() };
			_int		iBoneIndex = { Find_BoneIndex(strBoneTag) };
			if (-1 == iBoneIndex)
				continue;

			ChildBoneIndices.push_back(iBoneIndex);

			Get_Child_BoneIndices(strBoneTag, ChildBoneIndices);
		}
	}
}

void CModel::Get_Child_ZointIndices(string strStartBoneTag, const string& strEndBoneTag, list<_uint>& ChildZointIndices)
{
	_int			iStartBoneIndex = { Find_BoneIndex(strStartBoneTag) };
	_int			iEndBoneIndex = { Find_BoneIndex(strEndBoneTag) };
	if (-1 == iEndBoneIndex || -1 == iStartBoneIndex)
		return;


	_int			iParentIndex = { -1 };
	_int			iCurrentBoneIndex = { iEndBoneIndex };
	while (iParentIndex != iStartBoneIndex)
	{
		iParentIndex = m_Bones[iCurrentBoneIndex]->Get_ParentIndex();

		if (-1 == iParentIndex)
		{
			ChildZointIndices.clear();
			break;
		}

		ChildZointIndices.push_front(iCurrentBoneIndex);
		iCurrentBoneIndex = iParentIndex;
	}
}

const _float4x4* CModel::Get_CombinedMatrix(const string& strBoneTag)
{
	_int		iBoneIndex = { Find_BoneIndex(strBoneTag) };
	if (-1 == iBoneIndex)
		return nullptr;

	return m_Bones[iBoneIndex]->Get_CombinedTransformationMatrix();
}

HRESULT CModel::Initialize_Prototype_TEMP(MODEL_TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix)
{
	m_eModelType = eType;

	_uint		iOption = { aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded };

	iOption = m_eModelType == TYPE_NONANIM ? iOption | aiProcess_PreTransformVertices : iOption | aiProcess_LimitBoneWeights;

#pragma region Log
	//m_Importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_CAMERAS);
	//// 로그 생성
	//Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
	//// 로그 메시지를 얻기 위해 ReadFile 전에 호출

	///* 파일ㄹ의 정보를 읽어서 aiScene안에 모든 데이터를 담아주게된다. */
	//m_pAIScene = m_Importer.ReadFile(strModelFilePath.c_str(), iOption);
	//if (nullptr == m_pAIScene)
	//{
	//	MessageBoxA(NULL, m_Importer.GetErrorString(), "Error", MB_OK);

	//	return E_FAIL;
	//}
#pragma endregion

	/* 파일ㄹ의 정보를 읽어서 aiScene안에 모든 데이터를 담아주게된다. */
	m_pAIScene = m_Importer.ReadFile(strModelFilePath.c_str(), iOption);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	XMStoreFloat4x4(&m_TransformationMatrix, TransformMatrix);

	/* 읽은 정보를 바탕으로해서 내가 사용하기 좋게 정리한다.  */

	/* 지역내에서 뼈의 이름별로 현재 m_Bones상에 할당된 인덱스의 번호를 저장하고 메시등 객체를 생성시 탐색용으로 던져준다. */
	/* ReadyBones에서 값을 체우고 */
	/* ReadyMeshes, ReadyAnimations에서 값을 이용한다. */
	map<string, _uint> BoneIndices;

	/* 레디 본즈는 아래의 차일드 노드들을 타고 재귀적으로 생성하는 이진 탐색으로 루트 노드를 넘겨주면된다 .*/
	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, BoneIndices)))
		return E_FAIL;

	/* 모델을 구성하는 메시들을 생성한다. */
	/* 모델 = 메시 + 메시 + ... */
	if (FAILED(Ready_Meshes(BoneIndices)))
		return E_FAIL;

	if (FAILED(Ready_Materials(strModelFilePath.c_str())))
		return E_FAIL;

	if (FAILED(Ready_Animations(BoneIndices)))
		return E_FAIL;

	return S_OK;
}

void CModel::Set_Additional_Masking(_uint iPlayingIndex, _bool isAdditionalMasking, _uint iNumNeedKeyFrame)
{
	CPlayingInfo* pPLayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	if (nullptr == pPLayingInfo)
		return;

	pPLayingInfo->Set_Additional_Masking(isAdditionalMasking, iNumNeedKeyFrame);
}

void CModel::Set_KeyFrameIndex_AllKeyFrame(_uint iPlayingIndex, _uint iKeyFrameIndex)
{
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };

	if (nullptr != pPlayingInfo)
	{
		pPlayingInfo->Set_KeyFrameIndex_AllKeyFrame(iKeyFrameIndex);
	}
}

_float CModel::Get_TrackPosition(_uint iPlayingIndex)
{
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	_float					fTrackPosition = { 0.f };

	if (nullptr != pPlayingInfo)
	{
		fTrackPosition = pPlayingInfo->Get_TrackPosition();
	}

	return fTrackPosition;
}

_float CModel::Get_BlendWeight(_uint iPlayingIndex)
{
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	_float					fBlendWeight = { 0.f };

	if (nullptr != pPlayingInfo)
	{
		fBlendWeight = pPlayingInfo->Get_BlendWeight();
	}

	return fBlendWeight;
}

_int CModel::Get_AnimIndex_PlayingInfo(_uint iPlayingIndex)
{
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	_int				iAnimIndex = { -1 };

	if (nullptr != pPlayingInfo)
	{
		iAnimIndex = pPlayingInfo->Get_AnimIndex();
	}

	return iAnimIndex;
}

wstring CModel::Get_BoneLayerTag_PlayingInfo(_uint iPlayingIndex)
{
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	wstring				strBoneLayerTag = { TEXT("") };

	if (nullptr != pPlayingInfo)
	{
		strBoneLayerTag = pPlayingInfo->Get_BoneLayerTag();
	}

	return strBoneLayerTag;
}

_int CModel::Get_PlayingIndex_From_BoneLayerTag(wstring strBoneLayerTag)
{
	_int			iBoneLayerIndex = { 0 };
	_bool			isFind = { false };
	for (auto& pPlayingInfo : m_PlayingAnimInfos)
	{
		if (nullptr == pPlayingInfo)
			continue;

		wstring			strSrcBoneLayerTag = { pPlayingInfo->Get_BoneLayerTag() };
		if (strBoneLayerTag == strSrcBoneLayerTag)
		{
			isFind = true;
			break;
		}

		++iBoneLayerIndex;
	}

	if (false == isFind)
		iBoneLayerIndex = -1;

	return iBoneLayerIndex;
}

list<_uint> CModel::Get_Created_PlayingInfo_Indices()
{
	_uint			iIndex = { 0 };
	list<_uint>		Indices;
	for (auto& pPlayingInfo : m_PlayingAnimInfos)
	{
		if (nullptr != pPlayingInfo)
		{
			Indices.push_back(iIndex);
		}

		iIndex++;
	}

	return Indices;
}

_bool CModel::Is_Loop_PlayingInfo(_uint iPlayingIndex)
{
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	_bool					isLoop = { false };

	if (nullptr != pPlayingInfo)
	{
		isLoop = pPlayingInfo->Is_Loop();
	}

	return isLoop;
}

void CModel::Set_Loop(_uint iPlayingIndex, _bool isLoop)
{
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };

	if (nullptr != pPlayingInfo)
	{
		pPlayingInfo->Set_Loop(isLoop);
	}
}

void CModel::Set_TrackPosition(_uint iPlayingIndex, _float fTrackPosition, _bool isResetRootPre)
{
	_uint				iNumPlayingInfo = { static_cast<_uint>(m_PlayingAnimInfos.size()) };
	if (iPlayingIndex >= iNumPlayingInfo)
		return;

	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	if (nullptr != pPlayingInfo)
	{
		if (false == pPlayingInfo->Is_Set_CurrentAnimation())
			return;

		if (false == isResetRootPre)
		{
			pPlayingInfo->Set_TrackPosition(fTrackPosition);
		}

		else
		{
			pPlayingInfo->Set_TrackPosition_ResetRootPre(fTrackPosition);
		}
	}
}

void CModel::Set_BlendWeight(_uint iPlayingIndex, _float fBlendWeight, _float fLinearTime)
{
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	if (nullptr != pPlayingInfo)
	{
		pPlayingInfo->Set_BlendWeight(fBlendWeight, fLinearTime);
	}
}

void CModel::Change_Animation(_uint iPlayingIndex, const wstring& strAnimLayerTag, _uint iAnimIndex)
{
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	if (nullptr == pPlayingInfo)
		return;

	unordered_map<wstring, CAnimation_Layer*>::iterator		iter = { m_AnimationLayers.find(strAnimLayerTag) };
	if (iter == m_AnimationLayers.end())
		return;

	CAnimation* pAnimation = { iter->second->Get_Animation(iAnimIndex) };
	if (nullptr == pAnimation)
		return;

	_uint			iNumChannel = { pAnimation->Get_NumChannel() };

	pPlayingInfo->Change_Animation(strAnimLayerTag, iAnimIndex, iNumChannel);


}

void CModel::Change_Animation(_uint iPlayingIndex, const wstring& strAnimLayerTag, const string& strAnimTag)
{
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	if (nullptr == pPlayingInfo)
		return;

	unordered_map<wstring, CAnimation_Layer*>::iterator		iter = { m_AnimationLayers.find(strAnimLayerTag) };
	if (iter == m_AnimationLayers.end())
		return;

	_int			iAnimIndex;
	wstring			strTempAnimLayerTag;
	Find_AnimIndex(&iAnimIndex, &strTempAnimLayerTag, strAnimTag);

	Change_Animation(iPlayingIndex, strAnimLayerTag, iAnimIndex);
}

void CModel::Set_BoneLayer_PlayingInfo(_uint iPlayingIndex, const wstring& strBoneLayerTag)
{
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	if (nullptr != pPlayingInfo)
	{
		CBone_Layer* pBoneLayer = { Find_BoneLayer(strBoneLayerTag) };
		if (nullptr == pBoneLayer)
			return;

		pPlayingInfo->Set_BoneLayerTag(strBoneLayerTag);
	}
}

void CModel::Set_Mesh_Branch(const string& strMeshTag, _uint iMeshBranch)
{
	_uint			iNumBranch = { static_cast<_uint>(m_MeshBranches.size()) };
	for (_uint iMeshIndex = 0; iMeshIndex < iNumBranch; ++iMeshIndex)
	{
		CMesh* pMesh = { m_Meshes[iMeshIndex] };
		if (nullptr == pMesh)
			continue;

		if (strMeshTag == pMesh->Get_MeshName())
		{
			Set_Mesh_Branch(iMeshIndex, iMeshBranch);
			return;
		}
	}
}

void CModel::Set_Mesh_Branch(_uint iMeshIndex, _uint iMeshBranch)
{
	_uint			iNumBranch = { static_cast<_uint>(m_MeshBranches.size()) };
	if (iMeshIndex >= iNumBranch)
		return;

	m_MeshBranches[iMeshIndex] = iMeshBranch;
}

#pragma endregion

#pragma region Initialize

HRESULT CModel::Initialize_Prototype(MODEL_TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix)
{
#pragma region (이전로딩)
	//	m_eModelType = eType;
	//
	//	_uint		iOption = { aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded };
	//
	//	iOption = m_eModelType == TYPE_NONANIM ? iOption | aiProcess_PreTransformVertices : iOption;
	//
	//#pragma region Log
	//	//m_Importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_CAMERAS);
	//	//// 로그 생성
	//	//Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
	//	//// 로그 메시지를 얻기 위해 ReadFile 전에 호출
	//
	//	///* 파일ㄹ의 정보를 읽어서 aiScene안에 모든 데이터를 담아주게된다. */
	//	//m_pAIScene = m_Importer.ReadFile(strModelFilePath.c_str(), iOption);
	//	//if (nullptr == m_pAIScene)
	//	//{
	//	//	MessageBoxA(NULL, m_Importer.GetErrorString(), "Error", MB_OK);
	//
	//	//	return E_FAIL;
	//	//}
	//#pragma endregion
	//
	//	/* 파일ㄹ의 정보를 읽어서 aiScene안에 모든 데이터를 담아주게된다. */
	//	m_pAIScene = m_Importer.ReadFile(strModelFilePath.c_str(), iOption);
	//	if (nullptr == m_pAIScene)
	//		return E_FAIL;
	//
	//	XMStoreFloat4x4(&m_TransformationMatrix, TransformMatrix);
	//
	//	/* 읽은 정보를 바탕으로해서 내가 사용하기 좋게 정리한다.  */
	//
	//	/* 지역내에서 뼈의 이름별로 현재 m_Bones상에 할당된 인덱스의 번호를 저장하고 메시등 객체를 생성시 탐색용으로 던져준다. */
	//	/* ReadyBones에서 값을 체우고 */
	//	/* ReadyMeshes, ReadyAnimations에서 값을 이용한다. */
	//	map<string, _uint> BoneIndices;
	//
	//	/* 레디 본즈는 아래의 차일드 노드들을 타고 재귀적으로 생성하는 이진 탐색으로 루트 노드를 넘겨주면된다 .*/
	//	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, BoneIndices)))
	//		return E_FAIL;
	//
	//	/* 모델을 구성하는 메시들을 생성한다. */
	//	/* 모델 = 메시 + 메시 + ... */
	//	if (FAILED(Ready_Meshes(BoneIndices)))
	//		return E_FAIL;
	//
	//	if (FAILED(Ready_Materials(strModelFilePath.c_str())))
	//		return E_FAIL;
	//
	//	if (FAILED(Ready_Animations(BoneIndices)))
	//		return E_FAIL;

	//	return S_OK;

#pragma endregion
	filesystem::path FullPath(strModelFilePath);

	string strParentsPath = FullPath.parent_path().string();
	string strFileName = FullPath.stem().string();

	//	 동일경로에 동일 파일이름에 확장자만 다르게 새로운 경로 생성
	string strNewPath = strParentsPath + "/" + strFileName + ".bin";

	filesystem::path CheckPath(strNewPath);

	if (true == filesystem::exists(CheckPath))
	{
		if (FAILED(Initialize_Prototype(strModelFilePath, TransformMatrix)))
			return E_FAIL;
	}

	else
	{
		if (FAILED(CModel_Extractor::Extract_FBX(eType, strModelFilePath, TransformMatrix)))
			return E_FAIL;

		if (FAILED(Initialize_Prototype(strModelFilePath, TransformMatrix)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::Initialize_Prototype(const string& strModelFilePath, _fmatrix TransformMatrix)
{
	filesystem::path FullPath(strModelFilePath);

	string strParentsPath = FullPath.parent_path().string();
	string strFileName = FullPath.stem().string();

	//	 동일경로에 동일 파일이름에 확장자만 다르게 새로운 경로 생성
	string strNewPath = strParentsPath + "/" + strFileName + ".bin";

	//	바이너리로 데이터를 작성하기위해서 바이너리 플래그를 포함하였다.
	ifstream ifs(strNewPath.c_str(), ios::binary);

	//	첫 번째 인자는 데이터를 저장할 공간의 주소를 가리키는 포인터이고, 두 번째 인자는 읽어올 바이트 수입니다.
	if (true == ifs.fail())
	{
		MSG_BOX(TEXT("Failed To OpenFile"));

		return E_FAIL;
	}

	ifs.read(reinterpret_cast<_char*>(&m_eModelType), sizeof(MODEL_TYPE));

	XMStoreFloat4x4(&m_TransformationMatrix, TransformMatrix);

	if (FAILED(Ready_Bones(ifs)))
		return E_FAIL;

	if (FAILED(Ready_Meshes(ifs)))
		return E_FAIL;

	if (FAILED(Ready_Materials(ifs)))
		return E_FAIL;

	if (FAILED(Ready_Animations(ifs)))
		return E_FAIL;

	ifs.close();

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	m_fTotalLinearTime = ANIM_DEFAULT_LINEARTIME;

	m_pIK_Solver = CIK_Solver::Create(nullptr);
	if (nullptr == m_pIK_Solver)
		return E_FAIL;

	m_IsHideMesh.resize(m_iNumMeshes);

	m_MeshBranches.resize(m_Meshes.size());

	_uint iBoneIndex = { 0 };
	m_T_Pose_Matrices.resize(m_Bones.size());
	for (auto& pBone : m_Bones)
	{
		XMStoreFloat4x4(&m_T_Pose_Matrices[iBoneIndex++], pBone->Get_TrasformationMatrix());
	}

	for (auto& iIndex : m_MeshBranches)
	{
		iIndex = -1;
	}

	return S_OK;
}

#pragma endregion

#pragma region Bind Resource

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	ZeroMemory(m_MeshBoneMatrices, sizeof(_float4x4) * MAX_COUNT_BONE);

	m_Meshes[iMeshIndex]->Stock_Matrices(m_Bones, m_MeshBoneMatrices);

	return pShader->Bind_Matrices(pConstantName, m_MeshBoneMatrices, MAX_COUNT_BONE);
}

HRESULT CModel::Bind_BoneMatrices_Ragdoll(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, _float4x4* pBoneMatrices)
{
	ZeroMemory(m_MeshBoneMatrices, sizeof(_float4x4) * MAX_COUNT_BONE);

	m_Meshes[iMeshIndex]->Stock_Matrices_Ragdoll(m_Bones, m_MeshBoneMatrices, pBoneMatrices);

	return pShader->Bind_Matrices(pConstantName, m_MeshBoneMatrices, MAX_COUNT_BONE);
}

HRESULT CModel::Bind_PrevBoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	ZeroMemory(m_MeshBoneMatrices, sizeof(_float4x4) * 256);

	m_Meshes[iMeshIndex]->Stock_PrevMatrices(m_Bones, m_MeshBoneMatrices);

	return pShader->Bind_Matrices(pConstantName, m_MeshBoneMatrices, 256);
}

HRESULT CModel::Bind_ShaderResource_Texture(CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	_uint		iMeshMaterialIndex = { m_Meshes[iMeshIndex]->Get_MaterialIndex() };

	if (iMeshMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	CTexture* pTexture = m_Materials[iMeshMaterialIndex].MaterialTextures[eTextureType];


	if (nullptr != pTexture)
		return pTexture->Bind_ShaderResource(pShader, pConstantName);
	else {
		pShader->Bind_Texture(pConstantName, nullptr);
	}

	return E_FAIL;
}

HRESULT CModel::Bind_ShaderResource_MaterialDesc(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	if (FAILED(pShader->Bind_RawValue(pConstantName, &m_Meshes[iMeshIndex]->Get_MaterialDesc(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

#pragma endregion

HRESULT CModel::Play_Animations(CTransform* pTransform, _float fTimeDelta, _float3* pMovedDirection)
{
	if (false == Is_Set_RootBone())
		return E_FAIL;

	fTimeDelta = Compute_NewTimeDelta_Distatnce_Optimization(fTimeDelta, pTransform);
	if (0.f == fTimeDelta)
		return E_FAIL;

	for (auto& pPlayingInfo : m_PlayingAnimInfos)
	{
		if (nullptr == pPlayingInfo)
			continue;

		wstring			strAnimLayerTag = { pPlayingInfo->Get_AnimLayerTag() };
		_int			iAnimIndex = { pPlayingInfo->Get_AnimIndex() };

		_float			fTickPerSec = { 0.f };
		if (iAnimIndex != -1 && TEXT("") != strAnimLayerTag)
		{
			fTickPerSec = m_AnimationLayers[strAnimLayerTag]->Get_Animations()[iAnimIndex]->Get_TickPerSec();
		}
		pPlayingInfo->Update_BlendWeight_Linear(fTimeDelta * fTickPerSec);
	}

	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////
	////////////////////////////MOTION_BLEND////////////////////////////
	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////

	//	재생할 모든 애니메이션을 일단 재생하고 각 키프레임을 저장하여 가져온다.
	vector<vector<_float4x4>>		BlendTransformationMatricesLayer;
	vector<vector<_float4x4>>		AdditionalTransformationMatricesLayer;

	//	애니메이션들을 재생한다.
	//	포함된 뼈들을 등록, 각 애니메이션에대한 트랜스폼 행렬들을 레이어 단위로 저장한다.
	_uint			iNumPlayingInfo = { static_cast<_uint>(m_PlayingAnimInfos.size()) };
	BlendTransformationMatricesLayer.resize(iNumPlayingInfo);
	AdditionalTransformationMatricesLayer.resize(iNumPlayingInfo);
	for (_uint i = 0; i < iNumPlayingInfo; ++i)
	{
		CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(i) };
		if (nullptr == pPlayingInfo)
			continue;

		if (true == pPlayingInfo->Is_AdditionalMasking())
		{
			vector<_float4x4>			TransformationMatrices = { Apply_Animation(fTimeDelta, i) };
			AdditionalTransformationMatricesLayer[i] = TransformationMatrices;
		}
		else
		{
			vector<_float4x4>			TransformationMatrices = { Apply_Animation(fTimeDelta, i) };
			//	애니메이션이 종료되거나 (루프가아닌데 종료), 가중치가 없거나, 애니메이션 인덱스가 등록되지않은경우 빈 행렬벡터를 반환받는다.
			BlendTransformationMatricesLayer[i] = TransformationMatrices;
		}
	}

	//	결과 행렬들을 뼈의 트랜스폼 행렬에 저장한다.
	Apply_Bone_TransformMatrices(BlendTransformationMatricesLayer, AdditionalTransformationMatricesLayer);

	//	컴바인드 행렬 생성 및, 루트모션에 대한 성분들을 분해후 적용
	Apply_Bone_CombinedMatrices(pTransform, pMovedDirection);

	return S_OK;
}

HRESULT CModel::Play_Animation_Light(CTransform* pTransform, _float fTimeDelta)
{
	fTimeDelta = Compute_NewTimeDelta_Distatnce_Optimization(fTimeDelta, pTransform);
	if (0.f == fTimeDelta)
		return S_OK;

	_bool		isFinished = { false };

	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(0) };
	if (nullptr == pPlayingInfo)
	{
		MSG_BOX(TEXT("Default Playing Info 생성 필요 HRESULT CModel::Play_Animation_Light(CTransform* pTransform, _float fTimeDelta)"));

		return E_FAIL;
	}

	CBone_Layer* pBoneLayer = { Find_BoneLayer(pPlayingInfo->Get_BoneLayerTag()) };
	if (nullptr == pBoneLayer)
	{
		MSG_BOX(TEXT("Bone Layer 설정 하시오, HRESULT CModel::Play_Animation_Light(CTransform* pTransform, _float fTimeDelta)"));

		return E_FAIL;
	}

	_int				iAnimIndex = { pPlayingInfo->Get_AnimIndex() };
	_uint				iNumBones = { static_cast<_uint>(m_Bones.size()) };
	unordered_set<_uint>			IncludeBoneIndices = { pBoneLayer->Get_IncludedBoneIndices() };
	vector<_float4x4>				TransformationMatrices;
	if (-1 != iAnimIndex)
	{
		unordered_map<wstring, CAnimation_Layer*>::iterator		iter = { m_AnimationLayers.find(pPlayingInfo->Get_AnimLayerTag()) };
		CAnimation* pAnimation = { iter->second->Get_Animation(iAnimIndex) };

		TransformationMatrices = { pAnimation->Compute_TransfromationMatrix(fTimeDelta, iNumBones, IncludeBoneIndices, m_T_Pose_Matrices, pPlayingInfo) };

		Update_LinearInterpolation(fTimeDelta, 0);
		if (true == pPlayingInfo->Is_LinearInterpolation())
		{
			_float						fAccLinearTime = { pPlayingInfo->Get_AccLinearInterpolation() };
			vector<KEYFRAME>			LinearStartKeyFrames = { pPlayingInfo->Get_LinearStartKeyFrames() };

			for (_uint i = 0; i < iNumBones; ++i)
			{
				TransformationMatrices = pAnimation->Compute_TransfromationMatrix_LinearInterpolation(fAccLinearTime, m_fTotalLinearTime, TransformationMatrices, iNumBones, LinearStartKeyFrames);
			}
		}
	}	

	else
	{
		for (auto& T_Pose_Matrix : m_T_Pose_Matrices)
		{
			TransformationMatrices.push_back(T_Pose_Matrix);
		}
	}

	for (_uint i = 0; i < iNumBones; ++i)
	{
		m_Bones[i]->Set_TransformationMatrix(TransformationMatrices[i]);
	}

	pPlayingInfo->Update_LastKeyFrames(TransformationMatrices, iNumBones, m_fTotalLinearTime);
	pPlayingInfo->Set_FirstTick(false);

	for (_uint i = 0; i < iNumBones; ++i)
	{
		m_Bones[i]->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformationMatrix));
	}

	return S_OK;
}

HRESULT CModel::Play_Pose(_uint iPlayingIndex)
{
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	if (nullptr == pPlayingInfo)
		return E_FAIL;

	_int				iAnimIndex = { pPlayingInfo->Get_AnimIndex() };
	if (false == pPlayingInfo->Is_Set_CurrentAnimation())
	{
		MSG_BOX(TEXT("false == pPlayingInfo->Is_Set_CurrentAnimation(),  HRESULT CModel::Play_Animation_Light(CTransform* pTransform, _float fTimeDelta)"));

		return E_FAIL;
	}

	CBone_Layer* pBoneLayer = { Find_BoneLayer(pPlayingInfo->Get_BoneLayerTag()) };
	if (nullptr == pBoneLayer)
	{
		MSG_BOX(TEXT("Bone Layer 설정 하시오, HRESULT CModel::Play_Animation_Light(CTransform* pTransform, _float fTimeDelta)"));

		return E_FAIL;
	}

	_uint				iNumBones = { static_cast<_uint>(m_Bones.size()) };
	unordered_set<_uint>			IncludeBoneIndices = { pBoneLayer->Get_IncludedBoneIndices() };

	unordered_map<wstring, CAnimation_Layer*>::iterator		iter = { m_AnimationLayers.find(pPlayingInfo->Get_AnimLayerTag()) };
	CAnimation* pAnimation = { iter->second->Get_Animation(iAnimIndex) };

	vector<_float4x4>				TransformationMatrices = { pAnimation->Compute_TransfromationMatrix(0.f, iNumBones, IncludeBoneIndices, m_T_Pose_Matrices, pPlayingInfo) };

	for (_uint i = 0; i < iNumBones; ++i)
	{
		m_Bones[i]->Set_TransformationMatrix(TransformationMatrices[i]);
	}

	pPlayingInfo->Update_LastKeyFrames(TransformationMatrices, iNumBones, m_fTotalLinearTime);
	pPlayingInfo->Set_FirstTick(false);

	for (_uint i = 0; i < iNumBones; ++i)
	{
		m_Bones[i]->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformationMatrix));
	}

	return S_OK;
}

HRESULT CModel::Play_IK(CTransform* pTransform, _float fTimeDelta)
{
	if (nullptr == m_pIK_Solver)
		return E_FAIL;

	m_pIK_Solver->Play_IK(m_Bones, pTransform);
	//	Apply_AdditionalForces(pTransform);
	Clear_AdditionalForces();

	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

vector<_float4x4> CModel::Apply_Animation(_float fTimeDelta, _uint iPlayingIndex)
{
	const _int						iRootBoneIndex = { Find_RootBoneIndex() };
	vector<_float4x4>				TransformationMatrices;
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };

	if (nullptr == pPlayingInfo)
		return TransformationMatrices;

	const wstring					strBoneLayerTag = { pPlayingInfo->Get_BoneLayerTag() };
	CBone_Layer* pBoneLayer = { Find_BoneLayer(strBoneLayerTag) };

	if (nullptr == pBoneLayer)
		return TransformationMatrices;

	const _int						iAnimIndex = { pPlayingInfo->Get_AnimIndex() };
	const _int						iPreAnimIndex = { pPlayingInfo->Get_PreAnimIndex() };
	const _float					fBlendWeight = { pPlayingInfo->Get_BlendWeight() };
	const _uint						iNumBones = { static_cast<_uint>(m_Bones.size()) };

	//	현재 등록된 애니메이션이 없거나
	//	블렌드 웨이트가 0이하 => 섞지않을 애니메이션
	//	종료된 애니메이션 ( 루프도 아님 )
	if (false == pPlayingInfo->Is_Set_CurrentAnimation() || 0.f >= fBlendWeight)
		return TransformationMatrices;

	unordered_map<wstring, CAnimation_Layer*>::iterator		iter = { m_AnimationLayers.find(pPlayingInfo->Get_AnimLayerTag()) };
	CAnimation* pAnimation = { iter->second->Get_Animation(iAnimIndex) };
	//	트랙포지션의 이동으로 초기화된경우 애니메이션 재생이전 프레임을 보관한다.
	_bool							isResetRootPre = { pPlayingInfo->Is_ResetRootPre() };
	KEYFRAME						AnimPlayingPreKeyFrame;
	if (true == isResetRootPre)
	{
		AnimPlayingPreKeyFrame = { pAnimation->Get_CurrentKeyFrame(iRootBoneIndex, pPlayingInfo->Get_TrackPosition()) };

		if (m_isRootMotion_XZ || m_isRootMotion_Y)
			pPlayingInfo->Set_LastKeyFrame_Translation(iRootBoneIndex, XMLoadFloat3(&AnimPlayingPreKeyFrame.vTranslation));

		if (m_isRootMotion_Rotation)
			pPlayingInfo->Set_LastKeyFrame_Rotation(iRootBoneIndex, XMLoadFloat4(&AnimPlayingPreKeyFrame.vRotation));

		pPlayingInfo->Set_Root_Pre(false);
	}

	unordered_set<_uint>						TempIncludedBoneIndices = pBoneLayer->Get_IncludedBoneIndices();
	TransformationMatrices = pAnimation->Compute_TransfromationMatrix(fTimeDelta, iNumBones, TempIncludedBoneIndices, m_T_Pose_Matrices, pPlayingInfo);

	const _bool						isFirstTick = { pPlayingInfo->Is_FirstTick() };
	const _bool						isFinished = { pPlayingInfo->Is_Finished() };
	if (true == isFinished)
		return TransformationMatrices;

	//	애니메이션이 선형보간중이었다면 선형보간된 매트릭스로 재 업데이트한다.
	Update_LinearInterpolation(fTimeDelta, iPlayingIndex);

	const _float					fAccLinearInterpolation = { pPlayingInfo->Get_AccLinearInterpolation() };
	const _bool						isLinearInterpolation = { pPlayingInfo->Is_LinearInterpolation() };
	const vector<KEYFRAME>& LastKeyFrames = { pPlayingInfo->Get_LastKeyFrames() };

	//	선형 보간시 루트 모션시 새로운 시작 변위와 (원점에서 시작하지않는 모션등...)와 선형 보간 이전 키프레임들에서의 변위와의 차이 만큼 빨려들어감 방지 
	if (true == isLinearInterpolation)
		/*if (true == isLinearInterpolation)*/
	{
		//	첫 선형 보간 들어갈때 라스트 키프레임즈에서 루트성분을 적용에따라 현재 새로운 키프레임의 변환값으로 씌움
		//	전 애니메이션의 최종 루트성분을 현재 애니메이션의 시작 로컬 스페이스상의 루트로 맞춘다.			
		unordered_map<wstring, CAnimation_Layer*>::iterator		iter = { m_AnimationLayers.find(pPlayingInfo->Get_AnimLayerTag()) };
		CAnimation* pAnimation = { iter->second->Get_Animation(iAnimIndex) };

		if (true == isFirstTick)
		{
			KEYFRAME				FirstKeyFrame = { pAnimation->Get_FirstKeyFrame(iRootBoneIndex) };

			if (m_isRootMotion_XZ || m_isRootMotion_Y)
				pPlayingInfo->Set_LastKeyFrame_Translation(iRootBoneIndex, XMLoadFloat3(&FirstKeyFrame.vTranslation));

			if (m_isRootMotion_Rotation)
				pPlayingInfo->Set_LastKeyFrame_Rotation(iRootBoneIndex, XMLoadFloat4(&FirstKeyFrame.vRotation));

			vector<KEYFRAME>			LastKeyFramesTemp = { pPlayingInfo->Get_LastKeyFrames() };
			pPlayingInfo->Update_LinearStateKeyFrames(LastKeyFramesTemp);
		}

		//	이번에 재생된 애니메이션과 이전 최종 키프레임간의 혼합 과정		
		const vector<KEYFRAME>& LinearStartKeyFrames = { pPlayingInfo->Get_LinearStartKeyFrames() };
		TransformationMatrices = pAnimation->Compute_TransfromationMatrix_LinearInterpolation(fAccLinearInterpolation, m_fTotalLinearTime, TransformationMatrices, iNumBones, LinearStartKeyFrames);
	}

	//	첫 틱에 이전 변위량들을 새로 기입해줌
	//	위의 마지막 키프레임을 맞춰주는것과 같은 원리
	if (true == isFirstTick || true == isResetRootPre)
	{
		_matrix			RootTransformationMatrix = { XMLoadFloat4x4(&TransformationMatrices[iRootBoneIndex]) };
		_vector			vRootScale, vRootQuaternion, vRootTranslation;

		//	현재 애니메이션의 첫 키프레임으로 부터의 변위량 계산하기
		KEYFRAME		FirstKeyFrame;

		if (true == isFirstTick)
		{
			FirstKeyFrame = { pAnimation->Get_FirstKeyFrame(iRootBoneIndex) };
		}

		else if (true == isResetRootPre)
		{
			FirstKeyFrame = { AnimPlayingPreKeyFrame };
		}


		XMMatrixDecompose(&vRootScale, &vRootQuaternion, &vRootTranslation, RootTransformationMatrix);

		_vector			vPreRotationLocal = { XMLoadFloat4(&pPlayingInfo->Get_PreQuaternion()) };
		_vector			vFirstKeyFrameRotationLocal = { XMLoadFloat4(&FirstKeyFrame.vRotation) };
		_vector			vResultRotationLocal = { vPreRotationLocal };

		_vector			vPreTranslationLocal = { XMLoadFloat3(&pPlayingInfo->Get_PreTranslation_Local()) };
		_vector			vFirstKeyFrameTranslationLocal = { XMLoadFloat3(&FirstKeyFrame.vTranslation) };
		_vector			vResultTranslationLocal = { vPreTranslationLocal };

		if (true == m_isRootMotion_Rotation)
		{
			vResultRotationLocal = vFirstKeyFrameRotationLocal;
		}

		pPlayingInfo->Set_PreQuaternion(vResultRotationLocal);

		if (true == m_isRootMotion_XZ)
		{
			vResultTranslationLocal = XMVectorSetX(vResultTranslationLocal, XMVectorGetX(vFirstKeyFrameTranslationLocal));
			vResultTranslationLocal = XMVectorSetZ(vResultTranslationLocal, XMVectorGetZ(vFirstKeyFrameTranslationLocal));
		}

		if (true == m_isRootMotion_Y)
		{
			vResultTranslationLocal = XMVectorSetY(vResultTranslationLocal, XMVectorGetY(vFirstKeyFrameTranslationLocal));
		}

		pPlayingInfo->Set_PreTranslation(vResultTranslationLocal);
	}


	//	선형 보간중이아니었다면 이후에 일어날 선형 보간을 대비하여 마지막 키프레임들을 저장한다.
	//	=> 선형 보간여부와상관없이 매번 저장
	Update_LastKeyFrames(TransformationMatrices, iPlayingIndex);
	pPlayingInfo->Set_FirstTick(false);

	return TransformationMatrices;
}

void CModel::Apply_Bone_CombinedMatrices(CTransform* pTransform, _float3* pMovedDirection, _uint iStartBoneIndex)
{
	//	모든 채널업데이트가 끝난후 각 뼈에 컴바인드 행렬을 설정한다.
	const _uint		iNumBones = { static_cast<_uint>(m_Bones.size()) };
	for (_uint iBoneIndex = iStartBoneIndex; iBoneIndex < iNumBones; ++iBoneIndex)
	{
		_bool		isRootBone = { m_Bones[iBoneIndex]->Is_RootBone() };

		m_Bones[iBoneIndex]->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformationMatrix));
		if (true == isRootBone)
		{
			if (true == m_isRootMotion_XZ || true == m_isRootMotion_Y)
			{
				Apply_RootMotion_Translation(pTransform, pMovedDirection);
			}

			if (true == m_isRootMotion_Rotation)
			{
				Apply_RootMotion_Rotation(pTransform);
			}
		}

		Apply_AdditionalForce(iBoneIndex);
	}
}

vector<_float4x4> CModel::Compute_ResultMatrices_Blend(const vector<vector<_float4x4>>& TransformationMatricesLayer)
{
	unordered_set<_uint>		IncludedBoneIndices = { Compute_IncludedBoneIndices_AllBoneLayer_Blend() };
	vector<_float4x4>			ResultTransformationMatrices = { Initialize_ResultMatrices_Blend(IncludedBoneIndices) };

	//	결과행렬을 계산한다 => 얻어온 모든 행렬을 각 뼈의 인덱스에 해당하는 인덱스들에 결과행렬을 각 저장한다.
	_uint						iPlayingInfoIndex = { 0 };
	_uint						iNumPlayingInfo = { static_cast<_uint>(m_PlayingAnimInfos.size()) };
	_uint						iRootBoneIndex = { static_cast<_uint>(Find_RootBoneIndex()) };

	_uint						iNumBones = { static_cast<_uint>(m_Bones.size()) };
	Compute_Current_TotalBlendWeights();

	for (_uint iPlayingInfoIndex = 0; iPlayingInfoIndex < iNumPlayingInfo; ++iPlayingInfoIndex)
	{
		CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingInfoIndex) };
		if (nullptr == pPlayingInfo)
			continue;

		if (true == pPlayingInfo->Is_AdditionalMasking())
			continue;

		_int				iAnimIndex = { pPlayingInfo->Get_AnimIndex() };
		wstring				strBoneLayerTag = { pPlayingInfo->Get_BoneLayerTag() };
		_float				fBlendWeight = { pPlayingInfo->Get_BlendWeight() };

		if (false == pPlayingInfo->Is_Set_CurrentAnimation() || 0.f >= fBlendWeight)
			continue;
		CBone_Layer* pBoneLayer = { Find_BoneLayer(strBoneLayerTag) };
		if (nullptr == pBoneLayer)
			continue;

		//	각 변환행렬을 결과행렬에 가산한다 => 웨이트를 적용하여 가산한다.
		//	각자의 가중치 만큼 보간하여 더해준 매트릭스 만들기 => 0.3 + 0.4 + 0.3 이런식으로 마지막에 최종적으로 혼합된 매트릭스가 만들어짐
		for (_uint iBoneIndex : IncludedBoneIndices)
		{
			if (false == pBoneLayer->Is_Included(iBoneIndex))
				continue;

			_float			fTotalWeightThisBone = { m_TotalWeights[iBoneIndex] };
			_float			fWeightRatio = { fBlendWeight / fTotalWeightThisBone };
			if (fTotalWeightThisBone <= 0.f)
				continue;

			if (0.f >= fWeightRatio)
				continue;

			_matrix			ResultMatrix = { XMLoadFloat4x4(&ResultTransformationMatrices[iBoneIndex]) };
			_matrix			TransformationMatrix = { XMLoadFloat4x4(&TransformationMatricesLayer[iPlayingInfoIndex][iBoneIndex]) };
			////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////
			if (iBoneIndex == iRootBoneIndex)
			{
				_vector			vTempScale, vTempRotation, vTempTranslation;
				XMMatrixDecompose(&vTempScale, &vTempRotation, &vTempTranslation, TransformationMatrix);

				_vector			vResultScale = { vTempScale };
				_vector			vResultRotation = { vTempRotation };
				_vector			vResultTranslation = { vTempTranslation };

				//	루트 회전이 적용 된 경우
				if (true == m_isRootMotion_Rotation)
				{
					//	기존 분해 된 회전량을 루트로 전달하기위해 저장
					pPlayingInfo->Set_CurrentQuaternion(vTempRotation);

					//	분해 한 회전에 대한 역회전을 트랜스폼매트릭스의 이동량에 적용
					_vector			vRotationInverse = { XMQuaternionInverse(vTempRotation) };
					_matrix			InverseRotateMatrix = { XMMatrixRotationQuaternion(vRotationInverse) };
					vResultTranslation = XMVectorSetW(XMVector3TransformNormal(vTempTranslation, InverseRotateMatrix), 1.f);

					vResultRotation = XMQuaternionIdentity();
				}

				else
				{
					pPlayingInfo->Set_CurrentQuaternion(XMQuaternionIdentity());
				}

				//	루트 이동이 적용된 경우
				if (true == m_isRootMotion_XZ || true == m_isRootMotion_Y)
				{
					_vector			vCurrentTranslation = { XMVectorSet(0.f, 0.f, 0.f, 1.f) };

					if (true == m_isRootMotion_XZ)
					{
						vCurrentTranslation = XMVectorSet(XMVectorGetX(vTempTranslation), XMVectorGetY(vCurrentTranslation), XMVectorGetZ(vTempTranslation), 1.f);
						vResultTranslation = XMVectorSet(0.f, XMVectorGetY(vResultTranslation), 0.f, 1.f);
					}

					if (true == m_isRootMotion_Y) {

						vCurrentTranslation = XMVectorSet(XMVectorGetX(vCurrentTranslation), XMVectorGetY(vTempTranslation), XMVectorGetZ(vCurrentTranslation), 1.f);
						vResultTranslation = XMVectorSet(XMVectorGetX(vResultTranslation), 0.f, XMVectorGetZ(vResultTranslation), 1.f);
					}

					pPlayingInfo->Set_CurrentTranslation(vCurrentTranslation);
				}

				else
				{
					pPlayingInfo->Set_CurrentTranslation(XMVectorZero());
				}


				TransformationMatrix = XMMatrixAffineTransformation(vTempScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vResultRotation, vResultTranslation);
			}
			////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////


			_float4x4		ResultFloat4x4 = { Compute_BlendTransformation_Additional(ResultMatrix, TransformationMatrix, fWeightRatio) };
			XMStoreFloat4x4(&ResultTransformationMatrices[iBoneIndex], XMLoadFloat4x4(&ResultFloat4x4));
		}
	}

	return ResultTransformationMatrices;
}

vector<_float4x4> CModel::Compute_ResultMatrices_AdditionalMsking(const vector<vector<_float4x4>>& AdditionalMaskingTransformationMatricesLayer)
{
	unordered_set<_uint>		IncludedBoneIndices = { Compute_IncludedBoneIndices_AllBoneLayer_AdditionalMasking() };
	vector<_float4x4>			ResultTransformationMatrices /*= { Initialize_ResultMatrices_Blend(IncludedBoneIndices) };*/;
	_uint						iNumBones = { static_cast<_uint>(m_Bones.size()) };
	ResultTransformationMatrices.resize(iNumBones);
	for (_uint i = 0; i < iNumBones; ++i)
	{
		XMStoreFloat4x4(&ResultTransformationMatrices[i], XMMatrixIdentity());
	}

	//	결과행렬을 계산한다 => 얻어온 모든 행렬을 각 뼈의 인덱스에 해당하는 인덱스들에 결과행렬을 각 저장한다.
	_uint						iPlayingInfoIndex = { 0 };
	_uint						iNumPlayingInfo = { static_cast<_uint>(m_PlayingAnimInfos.size()) };
	_uint						iRootBoneIndex = { static_cast<_uint>(Find_RootBoneIndex()) };


	for (_uint iPlayingInfoIndex = 0; iPlayingInfoIndex < iNumPlayingInfo; ++iPlayingInfoIndex)
	{
		CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingInfoIndex) };
		if (nullptr == pPlayingInfo)
			continue;

		if (false == pPlayingInfo->Is_AdditionalMasking())
			continue;

		_int				iAnimIndex = { pPlayingInfo->Get_AnimIndex() };
		wstring				strBoneLayerTag = { pPlayingInfo->Get_BoneLayerTag() };
		_float				fBlendWeight = { pPlayingInfo->Get_BlendWeight() };

		if (false == pPlayingInfo->Is_Set_CurrentAnimation() || 0.f >= fBlendWeight)
			continue;

		CBone_Layer* pBoneLayer = { Find_BoneLayer(strBoneLayerTag) };
		if (nullptr == pBoneLayer)
			continue;

		//	각 변환행렬을 결과행렬에 가산한다 => 웨이트를 적용하여 가산한다.
		//	각자의 가중치 만큼 보간하여 더해준 매트릭스 만들기 => 0.3 + 0.4 + 0.3 이런식으로 마지막에 최종적으로 혼합된 매트릭스가 만들어짐
		for (_uint iBoneIndex : IncludedBoneIndices)
		{
			if (false == pBoneLayer->Is_Included(iBoneIndex))
				continue;

			_matrix			ResultMatrix = { XMLoadFloat4x4(&ResultTransformationMatrices[iBoneIndex]) };
			_matrix			TransformationMatrix = { XMLoadFloat4x4(&AdditionalMaskingTransformationMatricesLayer[iPlayingInfoIndex][iBoneIndex]) };

			_float4x4		ResultFloat4x4 = { Compute_BlendTransformation_Additional(ResultMatrix, TransformationMatrix, fBlendWeight) };
			XMStoreFloat4x4(&ResultTransformationMatrices[iBoneIndex], XMLoadFloat4x4(&ResultFloat4x4));
		}
	}

	return ResultTransformationMatrices;
}

void CModel::Apply_Bone_TransformMatrices(const vector<vector<_float4x4>>& BlendTransformationMatricesLayer, const vector<vector<_float4x4>>& AdditionalTransformationMatricesLayer)
{
	//	영향을 받지 않은 뼈들은 이전 변환행렬로 초기화한다.
	//	영향을 받은뼈는 항등 행렬로 초기화 후 이후 과정에서 행렬을 블렌드한다.
	vector<_float4x4>		ResultBlendTransformationMatrices = { Compute_ResultMatrices_Blend(BlendTransformationMatricesLayer) };
	vector<_float4x4>		ResultAditionalMaskingTransformationMatrices = { Compute_ResultMatrices_AdditionalMsking(AdditionalTransformationMatricesLayer) };

	const _uint				iNumBones = { static_cast<_uint>(m_Bones.size()) };
	//	결과행렬들을 뼈의 트랜스폼에 저장한다.
	for (_uint iBoneIndex = 0; iBoneIndex < iNumBones; ++iBoneIndex)
	{
		_matrix				BlendResultMatrix = { ResultBlendTransformationMatrices[iBoneIndex] };
		_matrix				AdditionalResultMatrix = { ResultAditionalMaskingTransformationMatrices[iBoneIndex] };

		_vector				vBlendScale, vBlendQuaternion, vBlendTranslation;
		_vector				vAddScale, vAddQuaternion, vAddTranslation;
		XMMatrixDecompose(&vBlendScale, &vBlendQuaternion, &vBlendTranslation, BlendResultMatrix);
		XMMatrixDecompose(&vAddScale, &vAddQuaternion, &vAddTranslation, AdditionalResultMatrix);

		_vector				vResultQuaternion = { XMQuaternionNormalize(XMQuaternionMultiply(XMQuaternionNormalize(vBlendQuaternion), XMQuaternionNormalize(vAddQuaternion))) };
		_matrix				CombiendResultMatrix = { XMMatrixAffineTransformation(vBlendScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vResultQuaternion, vBlendTranslation) };

		m_Bones[iBoneIndex]->Set_TransformationMatrix(CombiendResultMatrix);
		//	m_Bones[iBoneIndex]->Set_TransformationMatrix(ResultBlendTransformationMatrices[iBoneIndex]);
	}
}

void CModel::Static_Mesh_Cooking(CTransform* pTransform, _int* pIndex)
{
	for (int i = 0; i < m_Meshes.size(); ++i)
	{
		m_Meshes[i]->Static_Mesh_Cooking(pTransform, pIndex);
	}
}

void CModel::Dynamic_Mesh_Cooking(vector<PxRigidDynamic*>* pColliders, vector<PxTransform>* pTransforms, CTransform* pTransform)
{
	for (int i = 0; i < m_Meshes.size(); ++i)
	{
		m_Meshes[i]->Dynamic_Mesh_Cooking(pColliders, pTransforms, pTransform);
	}
}

void CModel::Convex_Mesh_Cooking(vector<PxRigidDynamic*>* pColliders, vector<PxTransform>* pTransforms, CTransform* pTransform)
{
	for (int i = 0; i < m_Meshes.size(); ++i)
	{
		m_Meshes[i]->Convex_Mesh_Cooking(pColliders, pTransforms, pTransform);
	}
}

void CModel::Convex_Mesh_Cooking_RigidDynamic(PxRigidDynamic** pCollider, CTransform* pTransform)
{
	for (int i = 0; i < m_Meshes.size(); ++i)
	{
		m_Meshes[i]->Convex_Mesh_Cooking_RigidDynamic(pCollider, pTransform);
	}
}

void CModel::Convex_Mesh_Cooking_RigidDynamic_Grenade(PxRigidDynamic** pCollider, CTransform* pTransform)
{
	m_Meshes[0]->Convex_Mesh_Cooking_RigidDynamic(pCollider, pTransform);
}

void CModel::Convex_Mesh_Cooking_Convert_Root(vector<PxRigidDynamic*>* pColliders, vector<PxTransform>* pTransforms, CTransform* pTransform)
{
	for (int i = 0; i < m_Meshes.size(); ++i)
	{
		if (i == 1)
			m_Meshes[i]->Convex_Mesh_Cooking_Convert_Root(pColliders, pTransforms, pTransform, _float4(-200.f, 0.f, 0.f, 0.f));
		else
			m_Meshes[i]->Convex_Mesh_Cooking(pColliders, pTransforms, pTransform);
	}
}

void CModel::Convex_Mesh_Cooking_Convert_Root_Double_Door(vector<PxRigidDynamic*>* pColliders, vector<PxTransform>* pTransforms, CTransform* pTransform)
{
	for (int i = 0; i < m_Meshes.size(); ++i)
	{
		if (i == 0)
			m_Meshes[i]->Convex_Mesh_Cooking_Convert_Root(pColliders, pTransforms, pTransform, _float4(200.f, 0.f, 0.f, 0.f));
		else
			m_Meshes[i]->Convex_Mesh_Cooking(pColliders, pTransforms, pTransform);
	}
}

void CModel::Convex_Mesh_Cooking_Convert_Root_Double_Door_No_Rotate(vector<PxRigidDynamic*>* pColliders, vector<PxTransform>* pTransforms, CTransform* pTransform)
{
	for (int i = 0; i < m_Meshes.size(); ++i)
	{
		if (i == 1)
			m_Meshes[i]->Convex_Mesh_Cooking(pColliders, pTransforms, pTransform);
		else
			m_Meshes[i]->Convex_Mesh_Cooking_Convert_Root_No_Rotate(pColliders, pTransforms, pTransform, _float4(-200.f, 0.f, 0.f, 0.f));
	}
}

void CModel::Convex_Mesh_Cooking_Cabinet(vector<PxRigidDynamic*>* pColliders, vector<PxTransform>* pTransforms, CTransform* pTransform)
{
	for (int i = 0; i < m_Meshes.size(); ++i)
	{
		if (i == 2)
			m_Meshes[i]->Convex_Mesh_Cooking(pColliders, pTransforms, pTransform);
		else
			m_Meshes[i]->Static_Mesh_Cooking_NoRotation(pTransform);
	}
}

void CModel::Convex_Mesh_Cooking_Toilet(vector<PxRigidDynamic*>* pColliders, vector<PxTransform>* pTransforms, CTransform* pTransform)
{
	for (int i = 0; i < m_Meshes.size(); ++i)
	{
		if (i == 1)
			m_Meshes[i]->Convex_Mesh_Cooking(pColliders, pTransforms, pTransform);
		else
			m_Meshes[i]->Static_Mesh_Cooking_NoRotation(pTransform);
	}
}

void CModel::Create_SoftBody()
{
	for (int i = 0; i < 1; ++i)
	{
		m_Meshes[i]->Create_SoftBody();
	}
}

void CModel::Create_Cloth()
{
	for (int i = 0; i < 1; ++i)
	{
		m_Meshes[i]->Create_Cloth();
	}
}

_int CModel::Find_BoneIndex(const string& strRootTag)
{
	_int		iIndex = { 0 };
	for (auto& Bone : m_Bones)
	{
		if (true == Bone->Compare_Name(strRootTag.c_str()))
			return iIndex;

		++iIndex;
	}

	return -1;
}

void CModel::Compute_RootParentsIndicies(_uint iRootIndex, vector<_uint>& ParentsIndices)
{
	ParentsIndices.push_back(iRootIndex);
	_int iParentIndex = m_Bones[iRootIndex]->Get_ParentIndex();

	if (-1 == iParentIndex)
		return;

	Compute_RootParentsIndicies(iParentIndex, ParentsIndices);
}

void CModel::Motion_Changed(_uint iPlayingIndex)
{
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	if (nullptr != pPlayingInfo)
	{
		pPlayingInfo->Reset_LinearInterpolation();
		pPlayingInfo->Set_LinearInterpolation(true);
	}
}

void CModel::Update_LinearInterpolation(_float fTimeDelta, _uint iPlayingIndex)
{
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	const _bool				isLinearInterpolation = { pPlayingInfo->Is_LinearInterpolation() };
	if (nullptr != pPlayingInfo &&
		true == isLinearInterpolation)
	{
		pPlayingInfo->Add_AccLinearInterpolation(fTimeDelta);
		if (m_fTotalLinearTime <= pPlayingInfo->Get_AccLinearInterpolation())
		{
			pPlayingInfo->Reset_LinearInterpolation();
		}
	}
}

#pragma region 로컬 트렌스폼 제어
void CModel::Set_Local_Scaled(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	Set_Local_State(STATE_LOCAL_RIGHT, XMVector3Normalize(Get_Local_State_Vector(STATE_LOCAL_RIGHT)) * fScaleX);
	Set_Local_State(STATE_LOCAL_UP, XMVector3Normalize(Get_Local_State_Vector(STATE_LOCAL_UP)) * fScaleY);
	Set_Local_State(STATE_LOCAL_LOOK, XMVector3Normalize(Get_Local_State_Vector(STATE_LOCAL_LOOK)) * fScaleZ);
}
#pragma endregion

#pragma region Ready_Object

HRESULT CModel::Ready_Meshes(const map<string, _uint>& BoneIndices)
{
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], BoneIndices, XMLoadFloat4x4(&m_TransformationMatrix));
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

		MESH_MATERIAL			MeshMaterial{};

		for (size_t j = aiTextureType_DIFFUSE; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			aiString		strTextureFilePath;

			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTextureFilePath)))
			{
				MeshMaterial.MaterialTextures[j] = nullptr;

				continue;
			}

			_char			szDrive[MAX_PATH] = { "" };
			_char			szDirectory[MAX_PATH] = { "" };
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);


			_char			szFileName[MAX_PATH] = { "" };
			_char			szEXT[MAX_PATH] = { "" };

			/* ..\Bin\Resources\Models\Fiona\ */
			_splitpath_s(strTextureFilePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

			_char			szFullPath[MAX_PATH] = { "" };
			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDirectory);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szEXT);

			_tchar			szPerfectPath[MAX_PATH] = { L"" };

			MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szPerfectPath, MAX_PATH);


			MeshMaterial.MaterialTextures[j] = CTexture::Create(m_pDevice, m_pContext, szPerfectPath);
			if (nullptr == MeshMaterial.MaterialTextures[j])
				return E_FAIL;
		}

		m_Materials.push_back(MeshMaterial);

	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(aiNode* pAINode, map<string, _uint>& BoneIndices, _int iParentIndex)
{
	CBone* pBone = CBone::Create(pAINode, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);

	/* 별도의 수정이다.. map에 별도로 담아준다. index정보와 이름을.. */
	BoneIndices.emplace(pBone->Get_Name(), (_uint)(m_Bones.size() - 1));

	_int iParent = (_int)(m_Bones.size() - 1);

	for (size_t i = 0; i < pAINode->mNumChildren; ++i)
	{
		Ready_Bones(pAINode->mChildren[i], BoneIndices, iParent);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations(const map<string, _uint>& BoneIndices)
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	if (0 != m_iNumAnimations)
	{
		m_AnimationLayers.emplace(TEXT("Default"), CAnimation_Layer::Create());

		for (size_t i = 0; i < m_iNumAnimations; ++i)
		{
			CAnimation* pAnim = CAnimation::Create(m_pAIScene->mAnimations[i], BoneIndices);
			if (nullptr == pAnim)
				return E_FAIL;

			m_AnimationLayers[TEXT("Default")]->Add_Animation(pAnim);
		}
	}

	return S_OK;
}

HRESULT CModel::Ready_Meshes(ifstream& ifs)
{
	_float3 vTotal_CenterPoint = _float3(0.f, 0.f, 0.f);

	ifs.read(reinterpret_cast<_char*>(&m_iNumMeshes), sizeof(_uint));
	ifs.read(reinterpret_cast<_char*>(&m_eModelType), sizeof(MODEL_TYPE));

	_char szName[MAX_PATH] = { "" };
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMesh::MESH_DESC MeshDesc;

		ifs.read(reinterpret_cast<_char*>(szName), sizeof(_char) * MAX_PATH);
		MeshDesc.strName = szName;
		ifs.read(reinterpret_cast<_char*>(&MeshDesc.iMaterialIndex), sizeof(_uint));

		ifs.read(reinterpret_cast<_char*>(&MeshDesc.iNumVertices), sizeof(_uint));

		VTXANIMMESH Vertex{};
		for (_uint i = 0; i < MeshDesc.iNumVertices; ++i)
		{
			ifs.read(reinterpret_cast<_char*>(&Vertex.vPosition), sizeof(_float3));
			ifs.read(reinterpret_cast<_char*>(&Vertex.vNormal), sizeof(_float3));
			ifs.read(reinterpret_cast<_char*>(&Vertex.vTexcoord), sizeof(_float2));
			ifs.read(reinterpret_cast<_char*>(&Vertex.vTangent), sizeof(_float3));

			if (MODEL_TYPE::TYPE_ANIM == m_eModelType)
			{
				ifs.read(reinterpret_cast<_char*>(&Vertex.vBlendIndices), sizeof(XMUINT4));
				ifs.read(reinterpret_cast<_char*>(&Vertex.vBlendWeights), sizeof(_float4));
			}

			MeshDesc.Vertices.push_back(Vertex);
		}

		ifs.read(reinterpret_cast<_char*>(&MeshDesc.iNumIndices), sizeof(_uint));
		_uint iIndex = { 0 };
		for (_uint i = 0; i < MeshDesc.iNumIndices; ++i)
		{
			ifs.read(reinterpret_cast<_char*>(&iIndex), sizeof(_uint));

			MeshDesc.Indices.push_back(iIndex);
		}

		ifs.read(reinterpret_cast<_char*>(&MeshDesc.iNumBones), sizeof(_uint));
		_uint iBoneIndex = { 0 };
		for (_uint i = 0; i < MeshDesc.iNumBones; ++i)
		{
			ifs.read(reinterpret_cast<_char*>(&iBoneIndex), sizeof(_uint));

			MeshDesc.Bones.push_back(iBoneIndex);
		}

		ifs.read(reinterpret_cast<_char*>(&MeshDesc.iNumOffsetMatrices), sizeof(_uint));
		_float4x4 OffsetMatrix;
		for (_uint i = 0; i < MeshDesc.iNumOffsetMatrices; ++i)
		{
			ifs.read(reinterpret_cast<_char*>(&OffsetMatrix), sizeof(_float4x4));

			MeshDesc.OffsetMatrices.push_back(OffsetMatrix);
		}

		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, MeshDesc);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.push_back(pMesh);

		vTotal_CenterPoint += pMesh->Get_CenterPoint();
	}

	vTotal_CenterPoint = vTotal_CenterPoint / (_float)m_iNumMeshes;
	m_vCenterPoint = Convert_Float3_To_Float4_Coord(vTotal_CenterPoint);

	return S_OK;
}

HRESULT CModel::Ready_Materials(ifstream& ifs)
{
	ifs.read(reinterpret_cast<_char*>(&m_iNumMaterials), sizeof(_uint));

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MESH_MATERIAL			MeshMaterial{};

		for (size_t i = aiTextureType_DIFFUSE; i < AI_TEXTURE_TYPE_MAX; ++i)
		{
			_tchar szPath[MAX_PATH] = { TEXT("") };
			ifs.read(reinterpret_cast<_char*>(&szPath), sizeof(_tchar) * MAX_PATH);

			if (wstring(szPath) == L"")
			{
				continue;
			}

			wstring				strPath = { szPath };
			_uint				iIndex = { static_cast<_uint>(strPath.rfind(TEXT("/"))) };
			wstring				strFileName = { strPath.substr(iIndex + 1) };

			unordered_map<wstring, CTexture*>::iterator			iter = { m_LoadedTextures.find(strFileName) };
			if (iter == m_LoadedTextures.end())
			{
				CTexture* pTexture = { CTexture::Create(m_pDevice, m_pContext, szPath) };
				MeshMaterial.MaterialTextures[i] = pTexture;
				if (nullptr == MeshMaterial.MaterialTextures[i])
					return E_FAIL;

				m_LoadedTextures.emplace(strFileName, pTexture);
			}
			else
			{
				MeshMaterial.MaterialTextures[i] = static_cast<CTexture*>(iter->second->Clone(nullptr));
				if (nullptr == MeshMaterial.MaterialTextures[i])
					return E_FAIL;
			}
		}

		m_Materials.push_back(MeshMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(ifstream& ifs)
{
	_uint iNumBones = { 0 };
	ifs.read(reinterpret_cast<_char*>(&iNumBones), sizeof(_uint));

	_char		szName[MAX_PATH] = { "" };

	for (_uint i = 0; i < iNumBones; ++i)
	{
		CBone::BONE_DESC BoneDesc;

		ifs.read(reinterpret_cast<_char*>(&BoneDesc.iParentIndex), sizeof(_int));
		ifs.read(reinterpret_cast<_char*>(szName), sizeof(_char) * MAX_PATH);
		BoneDesc.strName = szName;
		ifs.read(reinterpret_cast<_char*>(&BoneDesc.TransformationMatrix), sizeof(_float4x4));

		CBone* pBone = CBone::Create(BoneDesc);
		if (nullptr == pBone)
			return E_FAIL;

		m_Bones.push_back(pBone);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations(ifstream& ifs)
{
	ifs.read(reinterpret_cast<_char*>(&m_iNumAnimations), sizeof(_uint));

	/* For.Animation */
	_char		szName[MAX_PATH] = {};

	if (0 != m_iNumAnimations)
	{
		m_AnimationLayers.emplace(TEXT("Default"), CAnimation_Layer::Create());

		for (_uint i = 0; i < m_iNumAnimations; ++i)
		{
			CAnimation::ANIM_DESC		AnimDesc;

			ifs.read(reinterpret_cast<_char*>(&szName), sizeof(_char) * MAX_PATH);
			AnimDesc.strName = szName;
			ifs.read(reinterpret_cast<_char*>(&AnimDesc.fDuration), sizeof(_float));
			ifs.read(reinterpret_cast<_char*>(&AnimDesc.fTickPerSecond), sizeof(_float));

			ifs.read(reinterpret_cast<_char*>(&AnimDesc.iNumChannels), sizeof(_uint));
			for (_uint j = 0; j < AnimDesc.iNumChannels; ++j)
			{
				CChannel::CHANNEL_DESC		ChannelDesc;

				ifs.read(reinterpret_cast<_char*>(&szName), sizeof(_char) * MAX_PATH);
				ChannelDesc.strName = szName;
				ifs.read(reinterpret_cast<_char*>(&ChannelDesc.iBoneIndex), sizeof(_uint));

				ifs.read(reinterpret_cast<_char*>(&ChannelDesc.iNumKeyFrames), sizeof(_uint));
				for (_uint k = 0; k < ChannelDesc.iNumKeyFrames; ++k)
				{
					KEYFRAME					KeyFrame;

					ifs.read(reinterpret_cast<_char*>(&KeyFrame.vScale), sizeof(_float3));
					ifs.read(reinterpret_cast<_char*>(&KeyFrame.vRotation), sizeof(_float4));
					ifs.read(reinterpret_cast<_char*>(&KeyFrame.vTranslation), sizeof(_float3));
					ifs.read(reinterpret_cast<_char*>(&KeyFrame.fTime), sizeof(_float));

					ChannelDesc.KeyFrames.push_back(KeyFrame);
				}

				AnimDesc.ChannelDescs.push_back(ChannelDesc);
			}

			CAnimation* pAnim = CAnimation::Create(AnimDesc);
			if (nullptr == pAnim)
				return E_FAIL;

			m_AnimationLayers[TEXT("Default")]->Add_Animation(pAnim);
		}
	}


	return S_OK;
}

#pragma endregion

#pragma region Decal
void CModel::Bind_Resource_Skinning(_uint iIndex)
{
	m_Meshes[iIndex]->Bind_Resource_Skinning();
}

void CModel::Bind_Essential_Resource_Skinning(_float4x4* pWorldMat)
{
	m_pGameInstance->Bind_Essential_Resource_Skinning(pWorldMat, m_MeshBoneMatrices);
}

void CModel::Staging_Skinning(_uint iIndex)
{
	m_Meshes[iIndex]->Staging_Skinning();
}

void CModel::Perform_Skinning(_uint iIndex)
{
	m_pGameInstance->Perform_Skinning(m_Meshes[iIndex]->GetNumVertices());
}

void CModel::SetDecalWorldMatrix(_uint iIndex, _float4x4 WorldMatrix, _int iMeshType, _bool bBigAttack)
{
	list<_uint> NonHideIndex = Get_NonHideMeshIndices();

	for (auto& i : NonHideIndex)
	{
		if (m_MeshBranches[i] != iMeshType)
			m_Meshes[i]->SetDecalWorldMatrix(WorldMatrix, bBigAttack);
	}
}

void CModel::SetDecalWorldMatrix_Player_Front(_float4x4 WorldMatrix)
{
	list<_uint> NonHideIndex = Get_NonHideMeshIndices();

	for (auto& i : NonHideIndex)
	{
		m_Meshes[i]->SetDecalWorldMatrix_Player(WorldMatrix,true);
	}
}

void CModel::SetDecalWorldMatrix_Player_Back(_float4x4 WorldMatrix)
{
	list<_uint> NonHideIndex = Get_NonHideMeshIndices();

	for (auto& i : NonHideIndex)
	{
		m_Meshes[i]->SetDecalWorldMatrix_Player(WorldMatrix,false);
	}
}

void CModel::InitDecalWorldMatrix(_float4 vPos, _float4 vNormal)
{
	_matrix WorldMat = XMMatrixIdentity();
	_float4x4 WorldMat_Decal;
	XMStoreFloat4x4(&WorldMat_Decal, WorldMat);
	WorldMat_Decal._41 = vPos.x;
	WorldMat_Decal._42 = vPos.y;
	WorldMat_Decal._43 = vPos.z;

	auto iNumMesh = m_Meshes.size();
	for (size_t i = 0; i < iNumMesh; ++i)
	{
		m_Meshes[i]->SetDecalWorldMatrix(WorldMat_Decal, false);
		m_Meshes[i]->SetDecalExtent(_float3(0.2f, 0.2f, 0.2f));
	}
}

void CModel::Init_Decal(_uint iLevel)
{
	for (auto& pMesh : m_Meshes)
		pMesh->Init_Decal(iLevel);
}

void CModel::Bind_Resource_NonCShader_Decal(_uint iIndex, class CShader* pShader)
{
	m_Meshes[iIndex]->Bind_Resource_NonCShader_Decal(pShader);
}

void CModel::Calc_DecalMap_NonCS(CShader* pShader, _int iMeshType)
{
	list<_uint> NonHideIndex = Get_NonHideMeshIndices();

	for (auto& i : NonHideIndex)
	{
		if (m_MeshBranches[i] != iMeshType)
		{
			pShader->Begin(6);
			pShader->Bind_Matrices("g_BoneMatrices", m_MeshBoneMatrices, 512);
			m_Meshes[i]->Bind_Resource_NonCShader_Decal(pShader);
			m_Meshes[i]->Calc_NonCS_Decal_Map(pShader);
		}
	}
}

void CModel::Initialize_DecalMap()
{
	size_t iNumMesh = m_Meshes.size();

	m_vecSRV_DecalMap.resize(iNumMesh);
	m_vecUAV_DecalMap.resize(iNumMesh);
	m_vecSB_DecalMap.resize(iNumMesh);
	m_vecDecal_Map.clear();

	for (size_t i = 0; i < iNumMesh; ++i)
	{
		auto iNumVertices = m_Meshes[i]->GetNumVertices();
		auto pDecalMap = new _float2[iNumVertices];
		ZeroMemory(pDecalMap, sizeof(_float2) * iNumVertices);
		m_vecDecal_Map.push_back(pDecalMap);

		//Buffer for Write
		{
			D3D11_BUFFER_DESC sbDesc = {};
			sbDesc.Usage = D3D11_USAGE_DEFAULT;
			sbDesc.ByteWidth = sizeof(_float2) * iNumVertices;
			sbDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
			sbDesc.StructureByteStride = sizeof(_float2);
			sbDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

			D3D11_SUBRESOURCE_DATA initData = {};
			initData.pSysMem = m_vecDecal_Map[i];

			m_pDevice->CreateBuffer(&sbDesc, &initData, &m_vecSB_DecalMap[i]);

			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.Format = DXGI_FORMAT_UNKNOWN;
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uavDesc.Buffer.NumElements = sbDesc.ByteWidth / sbDesc.StructureByteStride;

			m_pDevice->CreateUnorderedAccessView(m_vecSB_DecalMap[i], &uavDesc, &m_vecUAV_DecalMap[i]);

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			srvDesc.Buffer.NumElements = iNumVertices;

			m_pDevice->CreateShaderResourceView(m_vecSB_DecalMap[i], &srvDesc, &m_vecSRV_DecalMap[i]);
		}
	}
}

_uint CModel::Perform_RayCasting(_uint iIndex, AddDecalInfo Info, _float* pDist)
{
	return m_Meshes[iIndex]->RayCasting_Decal(Info, pDist);
}

void CModel::Perform_Calc_DecalInfo(_uint iIndex)
{
	m_Meshes[iIndex]->Calc_Decal_Info();
}

void CModel::Bind_Resource_DecalMap(_uint iIndex, class CShader* pShader)
{
	m_Meshes[iIndex]->Bind_Resource_DecalMap(pShader);
}

void CModel::Perform_Init_DecalMap(_uint iIndex, class CShader* pShader)
{
	m_Meshes[iIndex]->Init_DecalMap(pShader);
}

void CModel::Perform_Calc_DecalMap(_int iMeshType)
{
	list<_uint> NonHideIndex = Get_NonHideMeshIndices();
	for (auto& i : NonHideIndex)
	{
		if (m_MeshBranches[i] != iMeshType)
		{
			m_Meshes[i]->Bind_Resource_CalcDecalMap(m_vecUAV_DecalMap[i]);
			m_Meshes[i]->Perform_Calc_DecalMap();
		}
	}
}

void CModel::Perform_Calc_DecalMap_Player()
{
	for (size_t i = 0; i <m_Meshes.size();++i)
	{
		m_Meshes[i]->Bind_Resource_CalcDecalMap(m_vecUAV_DecalMap[i]);
		m_Meshes[i]->Perform_Calc_DecalMap();
	}
}

void CModel::Perform_Calc_DecalMap_StaticModel()
{
	auto iNumMesh = m_Meshes.size();
	for (size_t i = 0; i < iNumMesh; ++i)
	{
		m_Meshes[i]->Bind_Resource_CalcDecalMap_StaticModel(m_vecUAV_DecalMap[i]);
		m_Meshes[i]->Perform_Calc_DecalMap_StaticModel();
	}
}

void CModel::Bind_DecalMap(_uint iIndex, CShader* pShader)
{
	pShader->Bind_Structured_Buffer("g_DecalMap", m_vecSRV_DecalMap[iIndex]);
	m_Meshes[iIndex]->Bind_Decal_Map(pShader);
}

#pragma endregion

#pragma region Create, Release

CModel* CModel::Create_Temp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype_TEMP(eType, strModelFilePath, TransformMatrix)))
	{
		MSG_BOX(TEXT("Failed To Created : CModel"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, strModelFilePath, TransformMatrix)))
	{
		MSG_BOX(TEXT("Failed To Created : CModel"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string& strModelFilePath, _fmatrix TransformMatrix)
{
	CModel* pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(strModelFilePath, TransformMatrix)))
	{
		MSG_BOX(TEXT("Failed To Created : CModel"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CModel"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();

	for (auto& Material : m_Materials)
	{
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			Safe_Release(Material.MaterialTextures[i]);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);
	m_Meshes.clear();

	for (auto& pAnimLayer : m_AnimationLayers)
	{
		Safe_Release(pAnimLayer.second);
		pAnimLayer.second = nullptr;
	}
	m_AdditionalForces.clear();

	for (auto& Pair : m_BoneLayers)
		Safe_Release(Pair.second);
	m_BoneLayers.clear();

	for (auto& pPlayingInfo : m_PlayingAnimInfos)
		Safe_Release(pPlayingInfo);
	m_PlayingAnimInfos.clear();

	Safe_Release(m_pIK_Solver);

	m_Importer.FreeScene();




	for (size_t i = 0; i < m_vecDecal_Map.size(); ++i)
	{
		Safe_Delete_Array(m_vecDecal_Map[i]);
	}

	for (size_t i = 0; i < m_vecSB_DecalMap.size(); ++i)
	{
		Safe_Release(m_vecSB_DecalMap[i]);
	}

	for (size_t i = 0; i < m_vecSRV_DecalMap.size(); ++i)
	{
		Safe_Release(m_vecSRV_DecalMap[i]);
	}

	for (size_t i = 0; i < m_vecUAV_DecalMap.size(); ++i)
	{
		Safe_Release(m_vecUAV_DecalMap[i]);
	}
}

void CModel::Release_IndexBuffer(_uint iNumMesh)
{
	m_Meshes[iNumMesh]->Release_IndexBuffer();
}

void CModel::Release_Decal_Dump()
{
	for (size_t i = 0; i < m_Meshes.size(); ++i)
	{
		m_Meshes[i]->Release_Decal();
		Safe_Delete_Array(m_vecDecal_Map[i]);
		Safe_Release(m_vecSRV_DecalMap[i]);
		Safe_Release(m_vecSB_DecalMap[i]);
		Safe_Release(m_vecUAV_DecalMap[i]);
	}
}

void CModel::Release_Dump()
{
	for (size_t i = 0; i < m_Meshes.size(); ++i)
	{
		m_Meshes[i]->Release_Dump();
	}
}

#pragma endregion