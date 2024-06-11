#include "..\Public\Model.h"

#include "Shader.h"
#include "Texture.h"

#include "GameInstance.h"

#include "Model_Extractor.h"

#include "Animation.h"
#include "Bone.h"
#include "Mesh.h"
#include "Bone_Layer.h"
#include "IK_Solver.h"

#include "Transform.h"
#include "RagDoll_Physics.h"
#include "PlayingInfo.h"

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
{
	for (auto& pPrototypeAnimation : rhs.m_Animations)
		m_Animations.push_back(pPrototypeAnimation->Clone());

	for (auto& pPrototypeBone : rhs.m_Bones)
		m_Bones.push_back(pPrototypeBone->Clone());

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);

	for (auto& Material : m_Materials)
	{
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			Safe_AddRef(Material.MaterialTextures[i]);
	}

	m_PlayingAnimInfos.resize(3);
}

void CModel::Add_AnimPlayingInfo(_uint iAnimIndex, _bool isLoop, _uint iPlayingIndex, const wstring& strBoneLayerTag, _float fBlendWeight)
{
	_bool			isCanCreate = { true };

	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	if (nullptr != pPlayingInfo)
		isCanCreate = false;

	const _uint			iNumAnims = { static_cast<_uint>(m_Animations.size()) };
	if (0 > iAnimIndex || iNumAnims <= iAnimIndex)
		isCanCreate = false;

	CBone_Layer* pBoneLayer = { Find_BoneLayer(strBoneLayerTag) };
	if (nullptr == pBoneLayer)
		isCanCreate = false;

	if (true == isCanCreate)
	{
		_uint			iNumChannel = { m_Animations[iAnimIndex]->Get_NumChannel() };

		CPlayingInfo::PLAYING_INFO_DESC		PlayingInfoDesc;
		PlayingInfoDesc.fBlendWeight = fBlendWeight;
		PlayingInfoDesc.iAnimIndex = iAnimIndex;
		PlayingInfoDesc.iNumBones = static_cast<_uint>(m_Bones.size());
		PlayingInfoDesc.isLoop = isLoop;
		PlayingInfoDesc.strBoneLayerTag = strBoneLayerTag;
		PlayingInfoDesc.iNumChannel = iNumChannel;

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

string CModel::Get_CurrentAnimTag(_uint iPlayingIndex)
{
	_int			iAnimIndex = { Get_CurrentAnimIndex(iPlayingIndex) };
	if (-1 == iAnimIndex)
		return string();

	_uint		iNumAnims = { static_cast<_uint>(m_Animations.size()) };
	if (iNumAnims <= static_cast<_uint>(iAnimIndex))
		return string();

	return m_Animations[iAnimIndex]->Get_Name();
}

void CModel::Set_TickPerSec(_uint iAnimIndex, _float fTickPerSec)
{
	if (iAnimIndex >= m_Animations.size())
		return;

	m_Animations[iAnimIndex]->Set_TickPerSec(fTickPerSec);
}

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

	set<_uint>			BoneIndices;
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

	set<_uint>				BoneIndices;
	for (auto& iChildIndex : ChildBoneIndices)
	{
		BoneIndices.emplace(iChildIndex);
	}

	pBoneLayer = CBone_Layer::Create(BoneIndices);
	if (nullptr == pBoneLayer)
		return;

	m_BoneLayers.emplace(strLayerTag, pBoneLayer);
}

void CModel::Add_Bone_Layer_All_Bone(const wstring& strLayerTag)
{
	CBone_Layer* pBoneLayer = { Find_BoneLayer(strLayerTag) };
	if (nullptr != pBoneLayer)
		return;

	set<_uint>			BoneIndices;
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

void CModel::Add_IK(string strTargetJointTag, string strEndEffectorTag, const wstring& strIKTag, _uint iNumIteration, _float fBlend)
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

		for (_float4x4 InputMatrix : m_AdditionalForces[iBoneIndex])
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

	for (_float4x4 InputMatrix : m_AdditionalForces[iBoneIndex])
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

vector<_float4x4> CModel::Initialize_ResultMatrices(const set<_uint> IncludedBoneIndices)
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

_float CModel::Compute_Current_TotalWeight(_uint iBoneIndex)
{
	_float		fTotalWeight = { 0.f };
	for (auto& pPlayingInfo : m_PlayingAnimInfos)
	{
		if (nullptr == pPlayingInfo)
			continue;

		_int		iAnimIndex = { pPlayingInfo->Get_AnimIndex() };
		if (-1 == iAnimIndex)
			continue;

		_bool		isFinished = { pPlayingInfo->Is_Finished() };
		if (true == isFinished)
			continue;

		wstring		strBoneLayerTag = { pPlayingInfo->Get_BoneLayerTag() };
		if (false == m_BoneLayers[strBoneLayerTag]->Is_Included(iBoneIndex))
			continue;

		_float		fBlendWeight = { pPlayingInfo->Get_BlendWeight() };
		fTotalWeight += fBlendWeight;
	}

	return fTotalWeight;
}

_float4x4 CModel::Compute_BlendTransformation_Additional(_fmatrix SrcMatrix, _fmatrix DstMatrix, _float fAdditionalWeight)
{
	_vector			vSrcScale, vSrcQuaternion, vSrcTranslation;
	XMMatrixDecompose(&vSrcScale, &vSrcQuaternion, &vSrcTranslation, SrcMatrix);

	_vector			vDstScale, vDstQuaternion, vDstTranslation;
	XMMatrixDecompose(&vDstScale, &vDstQuaternion, &vDstTranslation, DstMatrix);

	vDstQuaternion = XMQuaternionSlerp(XMQuaternionIdentity(), vDstQuaternion, fAdditionalWeight);
	vDstScale *= fAdditionalWeight;
	vDstTranslation *= fAdditionalWeight;

	_vector			vResultScale = { vSrcScale + vDstScale };
	_vector			vResultQuaternion = { XMQuaternionMultiply(XMQuaternionNormalize(vSrcQuaternion), XMQuaternionNormalize(vDstQuaternion)) };
	_vector			vResultTranslation = { XMVectorSetW(XMVectorSetW(vSrcTranslation, 0.f) + XMVectorSetW(vDstTranslation,0.f), 1.f) };

	_matrix			ResultMatrix = { XMMatrixAffineTransformation(vResultScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vResultQuaternion, vResultTranslation) };
	_float4x4		ResultFloat4x4;

	XMStoreFloat4x4(&ResultFloat4x4, ResultMatrix);

	return ResultFloat4x4;
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

void CModel::Apply_RootMotion_Rotation(CTransform* pTransform, _fvector vQuaternion, _float4* pTranslation)
{
	_vector			vPreQuaternion = { XMQuaternionIdentity() };
	_vector			vIdentityQuaternion = { XMQuaternionIdentity() };
	_uint			iRootIndex = { static_cast<_uint>(Find_RootBoneIndex()) };
	_vector			vTotalQuaternion = { vQuaternion };

	for (auto& pPlayingInfo : m_PlayingAnimInfos)
	{
		if (nullptr == pPlayingInfo)
			continue;

		_int			iAnimIndex = { pPlayingInfo->Get_AnimIndex() };
		_bool			isFinished = { pPlayingInfo->Is_Finished() };

		if (-1 == iAnimIndex || true == isFinished)
			continue;

		wstring			strBoneLayerTag = { pPlayingInfo->Get_BoneLayerTag() };
		CBone_Layer* pBoneLayer = { Find_BoneLayer(strBoneLayerTag) };
		if (nullptr == pBoneLayer)
			continue;

		if (false == pBoneLayer->Is_Included(iRootIndex))
			continue;

		_float			fBlendWeight = { pPlayingInfo->Get_BlendWeight() };

		_vector			vCurrentQuaternion = { XMLoadFloat4(&pPlayingInfo->Get_PreQuaternion()) };
		vCurrentQuaternion = { XMQuaternionSlerp(vIdentityQuaternion, vCurrentQuaternion, fBlendWeight) };
		vPreQuaternion = XMQuaternionMultiply(XMQuaternionNormalize(vPreQuaternion), XMQuaternionNormalize(vCurrentQuaternion));


		//	AnimInfo.vPreQuaternion = AnimInfo.LastKeyFrames[iRootIndex].vRotation;
		// 
		//	현재 라스트 키프레임의 회전성분을 이전 회전 총합으로 기록
		KEYFRAME		LastRootBoneKeyFrame = { pPlayingInfo->Get_LastKeyFrame(iRootIndex) };
		pPlayingInfo->Set_PreQuaternion(XMLoadFloat4(&LastRootBoneKeyFrame.vRotation));
	}

	_vector			vCurrentQuaternion = { vQuaternion };

	// 이전 쿼터니언의 역쿼터니언 구하기
	_vector			vPreQuaternionInv = { XMQuaternionInverse(vPreQuaternion) };

	// 이전 쿼터니언의 역쿼터니언과 현재쿼터니언의 곱 => 합쿼터니언
	_vector			vQuaternionDifference = { XMQuaternionNormalize(XMQuaternionMultiply(XMQuaternionNormalize(vPreQuaternionInv), XMQuaternionNormalize(vCurrentQuaternion))) };

	_matrix			RotationMatrix = { XMMatrixRotationQuaternion(vQuaternionDifference) };
	_matrix			WorldMatrix = { pTransform->Get_WorldMatrix() };
	_vector			vPosition = { WorldMatrix.r[CTransform::STATE_POSITION] };

	WorldMatrix.r[CTransform::STATE_POSITION] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	_matrix			ResultMatrix = { XMMatrixMultiply(RotationMatrix, WorldMatrix) };

	ResultMatrix.r[CTransform::STATE_POSITION] = vPosition;

	pTransform->Set_WorldMatrix(ResultMatrix);
}

void CModel::Apply_RootMotion_Translation(CTransform* pTransform, _fvector vTranslation, _float3* pMovedDirection, _fvector vQuaternion)
{
	_vector			vPreTranslation = { XMVectorZero() };
	_uint			iRootIndex = { static_cast<_uint>(Find_RootBoneIndex()) };

	for (auto& pPlayingInfo : m_PlayingAnimInfos)
	{
		if (nullptr == pPlayingInfo)
			continue;

		_int			iAnimIndex = { pPlayingInfo->Get_AnimIndex() };
		_bool			isFinished = { pPlayingInfo->Is_Finished() };
		if (-1 == iAnimIndex || true == isFinished)
			continue;

		wstring			strBoneLayerTag = { pPlayingInfo->Get_BoneLayerTag() };
		CBone_Layer* pBoneLayer = { Find_BoneLayer(strBoneLayerTag) };
		if (nullptr == pBoneLayer)
			continue;

		if (false == pBoneLayer->Is_Included(iRootIndex))
			continue;

		_vector			vTranslation = { XMLoadFloat3(&pPlayingInfo->Get_PreTranslation_Local()) };
		_float			fBlendWeight = { pPlayingInfo->Get_BlendWeight() };
		vTranslation = { vTranslation * fBlendWeight };
		vPreTranslation = vPreTranslation + vTranslation;

		KEYFRAME		LastRootBoneKeyFrame = { pPlayingInfo->Get_LastKeyFrame(iRootIndex) };
		pPlayingInfo->Set_PreTranslation(XMLoadFloat3(&LastRootBoneKeyFrame.vTranslation));
	}

	_matrix			WorldMatrix = { pTransform->Get_WorldMatrix() };
	_vector			vTranslationLocal = { vTranslation };

	_vector			vCurrentMoveDirectionLocal = { vTranslationLocal - XMVectorSetW(vPreTranslation, 1.f) };

	if (false == m_isRootMotion_XZ)
	{
		vCurrentMoveDirectionLocal = XMVectorSetX(vCurrentMoveDirectionLocal, 0.f);
		vCurrentMoveDirectionLocal = XMVectorSetZ(vCurrentMoveDirectionLocal, 0.f);
	}

	if (false == m_isRootMotion_Y)
	{
		vCurrentMoveDirectionLocal = XMVectorSetY(vCurrentMoveDirectionLocal, 0.f);
	}

	if (true == m_isRootMotion_Rotation)
	{
		_vector			vCurrentQuaternion = { vQuaternion };
		_vector			vCurrentQuaternionInv = { XMQuaternionNormalize(XMQuaternionInverse(vCurrentQuaternion)) };
		_matrix			RoatationMatrix = { XMMatrixRotationQuaternion(vCurrentQuaternionInv) };

		vCurrentMoveDirectionLocal = XMVector3TransformNormal(vCurrentMoveDirectionLocal, RoatationMatrix);
	}

	_vector			vCurrentMoveDirectionWorld;

	vCurrentMoveDirectionWorld = { XMVector3TransformNormal(vCurrentMoveDirectionLocal, XMLoadFloat4x4(&m_TransformationMatrix)) };
	vCurrentMoveDirectionWorld = { XMVector3TransformNormal(vCurrentMoveDirectionWorld, WorldMatrix) };

	XMStoreFloat3(pMovedDirection, vCurrentMoveDirectionWorld);
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

set<string> CModel::Get_MeshTags()
{
	set<string>		MeshTags;
	for (auto& pMesh : m_Meshes)
	{
		const string		strMeshTag = pMesh->Get_MeshName();
		MeshTags.emplace(strMeshTag);
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

_int CModel::Find_AnimIndex(CAnimation* pAnimation)
{
	_int		iAnimIndex = { -1 };

	_uint		iIndex = { 0 };
	_bool		isFind = { false };
	for (auto& pDstAnimation : m_Animations)
	{
		if (pAnimation == pDstAnimation)
		{
			isFind = true;
			break;
		}
		++iIndex;
	}

	if (true == isFind)
		iAnimIndex = iIndex;

	return iAnimIndex;
}

_int CModel::Find_AnimIndex(const string& strAnimTag)
{
	_int			iAnimIndex = { -1 };

	_uint			iIndex = { 0 };
	_bool			isFind = { false };
	for (auto& pAnimation : m_Animations)
	{
		string			strSrcAnimTag = { pAnimation->Get_Name() };
		if (strSrcAnimTag == strAnimTag)
		{
			isFind = true;
			break;
		}
		++iIndex;
	}

	if (true == isFind)
		iAnimIndex = iIndex;

	return iAnimIndex;
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

_int CModel::Get_BoneIndex(const string& strBoneTag)
{
	_int			iBoneIndex = { Find_BoneIndex(strBoneTag) };
	return iBoneIndex;
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

_float CModel::Get_Duration_From_Anim(_int iAnimIndex)
{
	_float			fDuration = { 0.f };
	_uint			iNumAnims = { static_cast<_uint>(m_Animations.size()) };

	if (static_cast<_int>(iNumAnims) > iAnimIndex && 0 <= iAnimIndex)
	{
		fDuration = m_Animations[iAnimIndex]->Get_Duration();
	}

	return fDuration;
}

_float CModel::Get_Duration_From_Anim(const string& strAnimTag)
{
	_float			fDuration = { 0.f };
	_int			iAnimIndex = { Find_AnimIndex(strAnimTag) };

	fDuration = Get_Duration_From_Anim(iAnimIndex);

	return fDuration;
}

_float CModel::Get_Duration_From_PlayingInfo(_uint iPlayingIndex)
{
	_float					fDuration = { 0.f };
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	if (nullptr != pPlayingInfo)
	{
		_int				iAnimIndex = { pPlayingInfo->Get_AnimIndex() };
		fDuration = Get_Duration_From_Anim(iAnimIndex);
	}

	return fDuration;
}

CBone* CModel::Get_BonePtr(const _char* pBoneName) const
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			return pBone->Compare_Name(pBoneName);
		});

	return *iter;
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

const _float4x4* CModel::Get_CombinedMatrix(const string& strBoneTag)
{
	_int		iBoneIndex = { Find_BoneIndex(strBoneTag) };
	if (-1 == iBoneIndex)
		return nullptr;

	return m_Bones[iBoneIndex]->Get_CombinedTransformationMatrix();
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

string CModel::Get_BoneLayerTag_PlayingInfo(_uint iPlayingIndex)
{
	_int			iAnimIndex = { Get_AnimIndex_PlayingInfo(iPlayingIndex) };
	string			strAnimTag = { "" };
	if (iAnimIndex != -1)
	{
		const _uint			iNumAnims = { static_cast<_uint>(m_Animations.size()) };
		if (static_cast<_int>(iNumAnims) > iAnimIndex)
		{
			strAnimTag = m_Animations[iAnimIndex]->Get_Name();
		}
	}

	return strAnimTag;
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

void CModel::Set_TrackPosition(_uint iPlayingIndex, _float fTrackPosition)
{
	_uint				iNumPlayingInfo = { static_cast<_uint>(m_PlayingAnimInfos.size()) };
	if (iPlayingIndex >= iNumPlayingInfo)
		return;

	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	if (nullptr != pPlayingInfo)
	{
		_int			iAnimIndex = { pPlayingInfo->Get_AnimIndex() };
		if (-1 == iAnimIndex)
			return;

		pPlayingInfo->Set_TrackPosition(fTrackPosition);
	}
}

void CModel::Set_BlendWeight(_uint iPlayingIndex, _float fBlendWeight)
{
	CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
	if (nullptr != pPlayingInfo)
	{
		pPlayingInfo->Set_BlendWeight(fBlendWeight);
	}
}

void CModel::Change_Animation(_uint iPlayingIndex, _uint iAnimIndex)
{
	_uint			iNumAnims = { static_cast<_uint>(m_Animations.size()) };
	if (iNumAnims > iAnimIndex)
	{
		CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingIndex) };
		if (nullptr == pPlayingInfo)
			return;

		_uint				iNumChannel = { m_Animations[iAnimIndex]->Get_NumChannel() };
		pPlayingInfo->Change_Animation(iAnimIndex, iNumChannel);
	}
}

void CModel::Change_Animation(_uint iPlayingIndex, const string& strAnimTag)
{
	_int			iAnimIndex = { Find_AnimIndex(strAnimTag) };

	if (-1 != iAnimIndex)
		Change_Animation(iPlayingIndex, iAnimIndex);
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

HRESULT CModel::Initialize_Prototype(MODEL_TYPE eType, const string& strModelFilePath, _fmatrix TransformMatrix)
{
#pragma region (이전로딩)
	//	m_eModelType = eType;
	//
	//	_uint		iOption = { aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded };
	//
	//	iOption = m_eModelType == TYPE_NONANIM ? iOption | aiProcess_PreTransformVertices : iOption | aiProcess_LimitBoneWeights;
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
		if (FAILED(CModel_Extractor::Extract_FBX(eType, strModelFilePath)))
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

	return S_OK;
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	ZeroMemory(m_MeshBoneMatrices, sizeof(_float4x4) * 256);

	m_Meshes[iMeshIndex]->Stock_Matrices(m_Bones, m_MeshBoneMatrices);

	return pShader->Bind_Matrices(pConstantName, m_MeshBoneMatrices, 256);
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

HRESULT CModel::Play_Animations(CTransform* pTransform, _float fTimeDelta, _float3* pMovedDirection)
{
	if (false == Is_Set_RootBone())
		return E_FAIL;

	for (auto& pBone : m_Bones)
	{
		_matrix			PreCombinedMatrix = { XMLoadFloat4x4(pBone->Get_CombinedTransformationMatrix()) };

		pBone->Set_Pre_CombinedMatrix(PreCombinedMatrix);
	}
	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////
	////////////////////////////MOTION_BLEND////////////////////////////
	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////

	//	재생할 모든 애니메이션을 일단 재생하고 각 키프레임을 저장하여 가져온다.
	vector<vector<_float4x4>>		TransformationMatricesLayer;
	set<_uint>						IncludedBoneIndices;
	XMStoreFloat3(pMovedDirection, XMVectorZero());

	//	애니메이션들을 재생한다.
	//	포함된 뼈들을 등록, 각 애니메이션에대한 트랜스폼 행렬들을 레이어 단위로 저장한다.
	_uint			iNumPlayingInfo = { static_cast<_uint>(m_PlayingAnimInfos.size()) };
	for (_uint i = 0; i < iNumPlayingInfo; ++i)
	{
		vector<_float4x4>			TransformationMatrices = { Apply_Animation(fTimeDelta, IncludedBoneIndices, i) };
		//	애니메이션이 종료되거나 (루프가아닌데 종료), 가중치가 없거나, 애니메이션 인덱스가 등록되지않은경우 빈 행렬벡터를 반환받는다.
		TransformationMatricesLayer.push_back(TransformationMatrices);
	}

	//	결과 행렬들을 뼈의 트랜스폼 행렬에 저장한다.
	Apply_Bone_TransformMatrices(TransformationMatricesLayer, IncludedBoneIndices);

	//	컴바인드 행렬 생성 및, 루트모션에 대한 성분들을 분해후 적용
	Apply_Bone_CombinedMatrices(pTransform, pMovedDirection);

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

vector<_float4x4> CModel::Apply_Animation(_float fTimeDelta, set<_uint>& IncludedBoneIndices, _uint iPlayingIndex)
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

	_bool							isFirstTick = { false };
	const _int						iAnimIndex = { pPlayingInfo->Get_AnimIndex() };
	const _int						iPreAnimIndex = { pPlayingInfo->Get_PreAnimIndex() };
	const _float					fBlendWeight = { pPlayingInfo->Get_BlendWeight() };
	const _uint						iNumBones = { static_cast<_uint>(m_Bones.size()) };

	//	현재 등록된 애니메이션이 없거나
	//	블렌드 웨이트가 0이하 => 섞지않을 애니메이션
	//	종료된 애니메이션 ( 루프도 아님 )
	if (-1 == iAnimIndex || 0.f >= fBlendWeight)
		return TransformationMatrices;

	set<_uint>						TempIncludedBoneIndices = pBoneLayer->Get_IncludedBoneIndices();
	TransformationMatrices = m_Animations[iAnimIndex]->Compute_TransfromationMatrix(fTimeDelta, iNumBones, TempIncludedBoneIndices, &isFirstTick, pPlayingInfo);
	if (true == TransformationMatrices.empty())
		return TransformationMatrices;

	for (auto& iBoneIndex : TempIncludedBoneIndices)
	{
		IncludedBoneIndices.emplace(iBoneIndex);
	}

	if (true == isFirstTick &&
		-1 != iPreAnimIndex)
	{
		pPlayingInfo->Reset_LinearInterpolation();
		pPlayingInfo->Set_LinearInterpolation(true);
	}

	//	애니메이션이 선형보간중이었다면 선형보간된 매트릭스로 재 업데이트한다.
	Update_LinearInterpolation(fTimeDelta, iPlayingIndex);

	const _float					fAccLinearInterpolation = { pPlayingInfo->Get_AccLinearInterpolation() };
	const _bool						isLinearInterpolation = { pPlayingInfo->Is_LinearInterpolation() };
	const vector<KEYFRAME>& LastKeyFrames = { pPlayingInfo->Get_LastKeyFrames() };

	//	선형 보간시 루트 모션시 새로운 시작 변위와 (원점에서 시작하지않는 모션등...)와 선형 보간 이전 키프레임들에서의 변위와의 차이 만큼 빨려들어감 방지 =>	별도 디벨롭 더 필요해요보임
	if (true == isLinearInterpolation)
	{
		//	첫 선형 보간 들어갈때 라스트 키프레임즈에서 루트성분을 적용에따라 현재 새로운 키프레임의 변환값으로 씌움
		if (true == isFirstTick)
		{
			_matrix			RootTransformationMatrix = { XMLoadFloat4x4(&TransformationMatrices[iRootBoneIndex]) };
			_vector			vRootScale, vRootQuaternion, vRootTranslation;

			XMMatrixDecompose(&vRootScale, &vRootQuaternion, &vRootTranslation, RootTransformationMatrix);

			if (true == m_isRootMotion_XZ ||
				true == m_isRootMotion_Y)
			{
				_vector			vLastTranslation = { XMLoadFloat3(&LastKeyFrames[iRootBoneIndex].vTranslation) };
				_vector			vLastQuaternion = { XMLoadFloat4(&LastKeyFrames[iRootBoneIndex].vRotation) };
				_vector			vResultTranslation = { vLastTranslation };

				if (true == m_isRootMotion_XZ)
				{
					vResultTranslation = XMVectorSetX(vResultTranslation, XMVectorGetX(vRootTranslation));
					vResultTranslation = XMVectorSetZ(vResultTranslation, XMVectorGetZ(vRootTranslation));
				}

				if (true == m_isRootMotion_Y)
				{
					vResultTranslation = XMVectorSetY(vResultTranslation, XMVectorGetY(vRootTranslation));
				}

				pPlayingInfo->Set_LastKeyFrame_Translation(iRootBoneIndex, vResultTranslation);
			}

			if (true == m_isRootMotion_Rotation)
			{
				pPlayingInfo->Set_LastKeyFrame_Rotation(iRootBoneIndex, vRootQuaternion);
			}
		}


		const vector<KEYFRAME>& LastKeyFrames = { pPlayingInfo->Get_LastKeyFrames() };
		TransformationMatrices = m_Animations[iAnimIndex]->Compute_TransfromationMatrix_LinearInterpolation(fAccLinearInterpolation, m_fTotalLinearTime, TransformationMatrices, iNumBones, LastKeyFrames);
	}

	if (true == isFirstTick)
	{
		_matrix			RootTransformationMatrix = { XMLoadFloat4x4(&TransformationMatrices[iRootBoneIndex]) };
		_vector			vRootScale, vRootQuaternion, vRootTranslation;

		XMMatrixDecompose(&vRootScale, &vRootQuaternion, &vRootTranslation, RootTransformationMatrix);

		if (true == m_isRootMotion_XZ)
		{
			_vector			vPreTranslationLocal = { XMLoadFloat3(&pPlayingInfo->Get_PreTranslation_Local()) };
			_vector			vResultTranslationLocal = {};

			vResultTranslationLocal = XMVectorSetX(vPreTranslationLocal, XMVectorGetX(vRootTranslation));
			vResultTranslationLocal = XMVectorSetZ(vResultTranslationLocal, XMVectorGetZ(vRootTranslation));

			pPlayingInfo->Set_PreTranslation(vResultTranslationLocal);
		}

		if (true == m_isRootMotion_Y)
		{
			_vector			vPreTranslationLocal = { XMLoadFloat3(&pPlayingInfo->Get_PreTranslation_Local()) };
			_vector			vResultTranslationLocal = {};

			vResultTranslationLocal = XMVectorSetY(vPreTranslationLocal, XMVectorGetY(vRootTranslation));

			pPlayingInfo->Set_PreTranslation(vResultTranslationLocal);
		}

		if (true == m_isRootMotion_Rotation)
		{
			pPlayingInfo->Set_PreQuaternion(vRootQuaternion);
		}
	}

	//	선형 보간중이아니었다면 이후에 일어날 선형 보간을 대비하여 마지막 키프레임들을 저장한다.
	//	=> 선형 보간여부와상관없이 매번 저장
	Update_LastKeyFrames(TransformationMatrices, iPlayingIndex);

	return TransformationMatrices;
}

void CModel::Apply_Bone_CombinedMatrices(CTransform* pTransform, _float3* pMovedDirection)
{
	//	모든 채널업데이트가 끝난후 각 뼈에 컴바인드 행렬을 설정한다.
	const _uint		iNumBones = { static_cast<_uint>(m_Bones.size()) };
	for (_uint iBoneIndex = 0; iBoneIndex < iNumBones; ++iBoneIndex)
	{
		_bool		isRootBone = { m_Bones[iBoneIndex]->Is_RootBone() };
		if (true == isRootBone)
		{
			_float4			vTranslation = { 0.f, 0.f, 0.f, 1.f };			_float4			vQuaternion = {};
			_float4* pTranslation = { &vTranslation };				_float4* pQuaternion = { &vQuaternion };

			m_Bones[iBoneIndex]->Invalidate_CombinedTransformationMatrix_RootMotion(m_Bones, m_TransformationMatrix, m_isRootMotion_XZ, m_isRootMotion_Y, m_isRootMotion_Rotation, pTranslation, pQuaternion);

			if (true == m_isRootMotion_Rotation)
				Apply_RootMotion_Rotation(pTransform, XMLoadFloat4(&vQuaternion), pTranslation);

			if (true == m_isRootMotion_XZ || true == m_isRootMotion_Y)
				Apply_RootMotion_Translation(pTransform, XMLoadFloat4(&vTranslation), pMovedDirection, XMLoadFloat4(&vQuaternion));
		}

		else
		{
			m_Bones[iBoneIndex]->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformationMatrix));
		}

		Apply_AdditionalForce(iBoneIndex);
	}
}

vector<_float4x4> CModel::Compute_ResultMatrices(const vector<vector<_float4x4>>& TransformationMatricesLayer, const set<_uint>& IncludedBoneIndices)
{
	vector<_float4x4>			ResultTransformationMatrices = { Initialize_ResultMatrices(IncludedBoneIndices) };

	//	결과행렬을 계산한다 => 얻어온 모든 행렬을 각 뼈의 인덱스에 해당하는 인덱스들에 결과행렬을 각 저장한다.
	_uint						iPlayingInfoIndex = { 0 };
	for (auto& AnimLayer : TransformationMatricesLayer)
	{
		CPlayingInfo* pPlayingInfo = { Find_PlayingInfo(iPlayingInfoIndex) };
		if (nullptr == pPlayingInfo)
			continue;

		_int				iAnimIndex = { pPlayingInfo->Get_AnimIndex() };
		_bool				isFinished = { pPlayingInfo->Is_Finished() };
		wstring				strBoneLayerTag = { pPlayingInfo->Get_BoneLayerTag() };
		_float				fBlendWeight = { pPlayingInfo->Get_BlendWeight() };

		if (-1 == iAnimIndex || 0.f >= fBlendWeight || true == isFinished)
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

			_float			fTotalWeight = { Compute_Current_TotalWeight(iBoneIndex) };
			if (0.f >= fTotalWeight)
				continue;

			_float			fWeightRatio = { fBlendWeight / fTotalWeight };
			if (0.f >= fWeightRatio)
				continue;

			_matrix			ResultMatrix = { XMLoadFloat4x4(&ResultTransformationMatrices[iBoneIndex]) };
			_matrix			TransformationMatrix = { XMLoadFloat4x4(&TransformationMatricesLayer[iPlayingInfoIndex][iBoneIndex]) };

			_float4x4		ResultFloat4x4 = { Compute_BlendTransformation_Additional(ResultMatrix, TransformationMatrix, fWeightRatio) };

			XMStoreFloat4x4(&ResultTransformationMatrices[iBoneIndex], XMLoadFloat4x4(&ResultFloat4x4));
		}

		++iPlayingInfoIndex;
	}

	return ResultTransformationMatrices;
}

void CModel::Apply_Bone_TransformMatrices(const vector<vector<_float4x4>>& TransformationMatricesLayer, const set<_uint>& IncludedBoneIndices)
{
	//	영향을 받지 않은 뼈들은 이전 변환행렬로 초기화한다.
	//	영향을 받은뼈는 항등 행렬로 초기화 후 이후 과정에서 행렬을 블렌드한다.
	vector<_float4x4>		ResultTransformationMatrices = { Compute_ResultMatrices(TransformationMatricesLayer, IncludedBoneIndices) };

	const _int				iRootBoneIndex = { Find_RootBoneIndex() };
	const _uint				iNumBones = { static_cast<_uint>(m_Bones.size()) };
	for (_uint iBoneIndex = 0; iBoneIndex < iNumBones; ++iBoneIndex)
	{
		//	포함되지 않은 ( 업데이트가 되지않은 ) 뼈는 건너뛴다.
		set<_uint>::iterator		iter = { IncludedBoneIndices.find(iBoneIndex) };
		if (iter == IncludedBoneIndices.end())
			continue;

		_matrix			ResultMatrix = { XMLoadFloat4x4(&ResultTransformationMatrices[iBoneIndex]) };

		_vector			vScale, vQuaternion, vTranslation;
		XMMatrixDecompose(&vScale, &vQuaternion, &vTranslation, ResultMatrix);

		vScale -= XMVectorSet(1.f, 1.f, 1.f, 0.f);
		ResultMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vQuaternion, vTranslation);
		XMStoreFloat4x4(&ResultTransformationMatrices[iBoneIndex], ResultMatrix);
	}

	//	결과행렬들을 뼈의 트랜스폼에 저장한다.
	for (_uint iBoneIndex = 0; iBoneIndex < iNumBones; ++iBoneIndex)
	{
		m_Bones[iBoneIndex]->Set_TransformationMatrix(ResultTransformationMatrices[iBoneIndex]);
	}
}

void CModel::Static_Mesh_Cooking()
{
	for (int i = 0; i < m_Meshes.size(); ++i)
	{
		m_Meshes[i]->Static_Mesh_Cooking();
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

	for (size_t i = 0; i < m_iNumAnimations; ++i)
	{
		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], BoneIndices);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

HRESULT CModel::Ready_Meshes(ifstream& ifs)
{
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
	}

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

			MeshMaterial.MaterialTextures[i] = CTexture::Create(m_pDevice, m_pContext, szPath);
			if (nullptr == MeshMaterial.MaterialTextures[i])
				return E_FAIL;
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
	_char		szName[MAX_PATH];

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

		m_Animations.push_back(pAnim);
	}

	return S_OK;
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

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();

	for (auto& Pair : m_BoneLayers)
		Safe_Release(Pair.second);
	m_BoneLayers.clear();

	for (auto& pPlayingInfo : m_PlayingAnimInfos)
		Safe_Release(pPlayingInfo);
	m_PlayingAnimInfos.clear();

	Safe_Release(m_pIK_Solver);

	m_Importer.FreeScene();
}

