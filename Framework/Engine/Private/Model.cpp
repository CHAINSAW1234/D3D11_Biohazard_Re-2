#include "..\Public\Model.h"

#include "Shader.h"
#include "Texture.h"

#include "GameInstance.h"

#include "Model_Extractor.h"

#include "Animation.h"
#include "Bone.h"
#include "Mesh.h"

#include "Transform.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
	XMStoreFloat4x4(&m_PreTranslationMatrix, XMMatrixIdentity());
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
	, m_PreTranslationMatrix{ rhs.m_PreTranslationMatrix }
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

	m_PlayingAnimInfos.resize(2);
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
	if (iPlayingIndex >= static_cast<_uint>(m_PlayingAnimInfos.size()))
		return false;

	_uint			iAnimIndex = { static_cast<_uint>(m_PlayingAnimInfos[iPlayingIndex].iAnimIndex) };
	return m_Animations[iAnimIndex]->isFinished();
}

//void CModel::Set_Animation(_uint iAnimIndex, _bool isLoop)
//{
//	if (iAnimIndex >= m_iNumAnimations)
//		return;
//
//	if (iAnimIndex == m_iCurrentAnimIndex)
//		return;
//
//	m_Animations[m_iCurrentAnimIndex]->Reset_TrackPostion();
//	m_Animations[m_iCurrentAnimIndex]->Reset_Finish();
//
//	m_iPreAnimIndex = m_iCurrentAnimIndex;
//	m_iCurrentAnimIndex = iAnimIndex;
//	m_Animations[m_iCurrentAnimIndex]->Reset_TrackPostion();
//
//	Motion_Changed();
//
//	m_isLoop = isLoop;
//}

void CModel::Set_Animation_Blend(ANIM_PLAYING_DESC AnimDesc, _uint iPlayingIndex)
{
	if (static_cast<_uint>(AnimDesc.iAnimIndex) >= m_iNumAnimations)
		return;

	if (iPlayingIndex >= static_cast<_uint>(m_PlayingAnimInfos.size()))
		return;

	if (AnimDesc.iAnimIndex == m_PlayingAnimInfos[iPlayingIndex].iAnimIndex)
		return;

	for (auto& AnimInfo : m_PlayingAnimInfos)
	{
		if (AnimInfo.iAnimIndex == AnimDesc.iAnimIndex)
			return;
	}

	vector<KEYFRAME>			LastKeyFrames;
	LastKeyFrames.resize(m_Animations[AnimDesc.iAnimIndex]->Get_Channels().size());

	_bool		isAnimFirst = { true };
	if (-1 != m_PlayingAnimInfos[iPlayingIndex].iAnimIndex)
	{
		m_Animations[m_PlayingAnimInfos[iPlayingIndex].iAnimIndex]->Reset_TrackPostion();
		m_Animations[m_PlayingAnimInfos[iPlayingIndex].iAnimIndex]->Reset_Finish();

		LastKeyFrames = m_PlayingAnimInfos[iPlayingIndex].LastKeyFrames;
		isAnimFirst = false;
	}

	ANIM_PLAYING_INFO		AnimInfo;
	AnimInfo.iAnimIndex = AnimDesc.iAnimIndex;
	AnimInfo.TargetBoneIndices = AnimDesc.TargetBoneIndices;
	AnimInfo.isLoop = AnimDesc.isLoop;
	AnimInfo.fWeight = AnimDesc.fWeight;

	AnimInfo.LastKeyFrames = LastKeyFrames;

	m_PlayingAnimInfos[iPlayingIndex] = AnimInfo;

	m_Animations[m_PlayingAnimInfos[iPlayingIndex].iAnimIndex]->Reset_TrackPostion();

	if (false == isAnimFirst)
		m_PlayingAnimInfos[iPlayingIndex].isLinearInterpolation = true;

	//	Motion_Changed(iPlayingIndex);
}

_uint CModel::Get_CurrentAnimIndex(_uint iPlayingIndex)
{
	if (iPlayingIndex >= static_cast<_uint>(m_PlayingAnimInfos.size()))
		return false;

	_uint			iAnimIndex = { static_cast<_uint>(m_PlayingAnimInfos[iPlayingIndex].iAnimIndex) };
	return iAnimIndex;
}

void CModel::Set_TickPerSec(_uint iAnimIndex, _float fTickPerSec)
{
	if (iAnimIndex >= m_Animations.size())
		return;

	m_Animations[iAnimIndex]->Set_TickPerSec(fTickPerSec);
}

void CModel::Set_Weight(_uint iPlayingIndex, _float fWeight)
{
	if (iPlayingIndex >= static_cast<_uint>(m_PlayingAnimInfos.size()))
		return;

	if (0.f > fWeight)
		fWeight = 0.f;

	m_PlayingAnimInfos[iPlayingIndex].fWeight = fWeight;
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
	for (_uint iBoneIndex = 0; iBoneIndex < m_Bones.size(); ++iBoneIndex)
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

_float CModel::Compute_Current_TotalWeight()
{
	_float		fTotalWeight = { 0.f };
	for (auto& AnimInfo : m_PlayingAnimInfos)
	{
		fTotalWeight += AnimInfo.fWeight;
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

void CModel::Set_SpineBone(string strBoneTag)
{
	_int		iBoneIndex = { Find_BoneIndex(strBoneTag) };

	if (iBoneIndex != -1)
		m_pSpineBone = m_Bones[iBoneIndex];
}

void CModel::Init_Separate_Bones()
{
	for (int i = 0; i < 61; ++i)
	{
		m_Bones_Lower.push_back(m_Bones[i]);
	}

	for (int i = 61; i < m_Bones.size(); ++i)
	{
		m_Bones_Upper.push_back(m_Bones[i]);
	}
}

//HRESULT CModel::Play_Animation_Separation(CTransform* pTransform, _float fTimeDelta, _float3* pMovedDirection)
//{
//	_bool		isFirstTick = { false };
//	m_Animations[33]->Invalidate_TransformationMatrix(fTimeDelta, m_Bones, m_isLoop, &isFirstTick);
//
//	//		TODO:		추 후 선형보간 혹은 모션 블렌딩으로 대체하여 삽입하기
//	_bool		isActiveXZ = { m_Animations[m_iCurrentAnimIndex]->Is_Active_RootMotion_XZ() };
//	_bool		isActiveY = { m_Animations[m_iCurrentAnimIndex]->Is_Active_RootMotion_Y() };
//	_bool		isActiveRotation = { m_Animations[m_iCurrentAnimIndex]->Is_Active_RootMotion_Rotation() };
//
//	//	모든 채널업데이트가 끝난후 각 뼈에 컴바인드 행렬을 설정한다.
//	for (auto& pBone : m_Bones_Lower)
//	{
//		_bool		isRootBone = { pBone->Is_RootBone() };
//		if (true == isRootBone)
//		{
//			_float4			vTranslation = { 0.f, 0.f, 0.f, 1.f };
//			_float4			vQuaternion = {};
//			_float4* pTranslation = { &vTranslation };
//			_float4* pQuaternion = { &vQuaternion };
//
//			if (false == isActiveRotation)
//			{
//				pQuaternion = nullptr;
//			}
//
//			if (false == isActiveXZ && false == isActiveY)
//			{
//				pTranslation = nullptr;
//			}
//
//			pBone->Invalidate_CombinedTransformationMatrix_RootMotion(m_Bones, m_TransformationMatrix, isActiveXZ, isActiveY, pTranslation, pQuaternion);
//
//			if (true == isActiveRotation)
//			{
//				if (true == isFirstTick)
//				{
//					m_vPreQuaternion = vQuaternion;
//				}
//
//				_vector			vPreQuaternion = { XMLoadFloat4(&m_vPreQuaternion) };
//				_vector			vCurrentQuaternion = { XMLoadFloat4(&vQuaternion) };
//
//				// 이전 쿼터니언의 역쿼터니언 구하기
//				_vector			vPreQuaternionInv = { XMQuaternionInverse(vPreQuaternion) };
//
//				// 이전 쿼터니언의 역쿼터니언과 현재쿼터니언의 곱 => 합쿼터니언
//				_vector			vQuaternionDiffrence = { XMQuaternionNormalize(XMQuaternionMultiply(vPreQuaternionInv, vCurrentQuaternion)) };
//
//				_matrix			RotationMatrix = { XMMatrixRotationQuaternion(vQuaternionDiffrence) };
//				_matrix			WorldMatrix = { pTransform->Get_WorldMatrix() };
//				_vector			vPosition = { WorldMatrix.r[CTransform::STATE_POSITION] };
//				WorldMatrix.r[CTransform::STATE_POSITION] = XMVectorSet(0.f, 0.f, 0.f, 1.f);
//
//				_matrix			ResultMatrix = { XMMatrixMultiply(RotationMatrix, WorldMatrix) };
//
//				ResultMatrix.r[CTransform::STATE_POSITION] = vPosition;
//				pTransform->Set_WorldMatrix(ResultMatrix);
//
//				m_vPreQuaternion = vQuaternion;
//			}
//
//			if (true == isActiveXZ || true == isActiveY)
//			{
//				_matrix			WorldMatrix = { pTransform->Get_WorldMatrix() };
//				_vector			vTranslationLocal = { XMLoadFloat4(&vTranslation) };
//				vTranslationLocal = XMVector4Transform(vTranslationLocal, XMLoadFloat4x4(&m_TransformationMatrix));
//
//				/* 첫 틱인 경우 이전 이동량을 현재 결과 이동량으로 넣어준다 => 이동이 않일어나게끔 */
//				if (true == isFirstTick)
//				{
//					XMStoreFloat3(&m_vPreTranslationLocal, vTranslationLocal);
//				}
//
//				_vector			vCurrentMoveDirectionLocal = { vTranslationLocal - XMVectorSetW(XMLoadFloat3(&m_vPreTranslationLocal), 1.f) };
//
//				if (true == isActiveRotation)
//				{
//					_vector			vCurrentQuaternion = { XMLoadFloat4(&vQuaternion) };
//					_vector			vCurrentQuaternionInv = { XMQuaternionNormalize(XMQuaternionInverse(vCurrentQuaternion)) };
//					_matrix			RoatationMatrix = { XMMatrixRotationQuaternion(vCurrentQuaternionInv) };
//
//					vCurrentMoveDirectionLocal = XMVector3TransformNormal(vCurrentMoveDirectionLocal, RoatationMatrix);
//				}
//
//				_vector			vCurrentMoveDirectionWorld = { XMVector3TransformNormal(vCurrentMoveDirectionLocal, WorldMatrix) };
//
//				XMStoreFloat3(pMovedDirection, vCurrentMoveDirectionWorld);
//				XMStoreFloat3(&m_vPreTranslationLocal, vTranslationLocal);
//			}
//		}
//
//		else
//		{
//			pBone->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformationMatrix));
//		}
//	}
//
//	m_Animations[53]->Invalidate_TransformationMatrix(fTimeDelta, m_Bones, m_isLoop, &isFirstTick);
//
//	for (int i = 0; i < m_Bones_Upper.size(); ++i)
//	{
//		m_Bones_Upper[i]->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformationMatrix));
//	}
//
//	return S_OK;
//}

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

void CModel::Apply_RootMotion_Rotation(CTransform* pTransform, _fvector vQuaternion)
{
	_vector			vPreQuaternion = { XMLoadFloat4(&m_vPreQuaternion) };
	_vector			vCurrentQuaternion = { vQuaternion };

	// 이전 쿼터니언의 역쿼터니언 구하기
	_vector			vPreQuaternionInv = { XMQuaternionInverse(vPreQuaternion) };

	// 이전 쿼터니언의 역쿼터니언과 현재쿼터니언의 곱 => 합쿼터니언
	_vector			vQuaternionDifference = { XMQuaternionNormalize(XMQuaternionMultiply(vPreQuaternionInv, vCurrentQuaternion)) };

	_matrix			RotationMatrix = { XMMatrixRotationQuaternion(vQuaternionDifference) };
	_matrix			WorldMatrix = { pTransform->Get_WorldMatrix() };
	_vector			vPosition = { WorldMatrix.r[CTransform::STATE_POSITION] };
	WorldMatrix.r[CTransform::STATE_POSITION] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	_matrix			ResultMatrix = { XMMatrixMultiply(RotationMatrix, WorldMatrix) };

	ResultMatrix.r[CTransform::STATE_POSITION] = vPosition;
	pTransform->Set_WorldMatrix(ResultMatrix);

	XMStoreFloat4(&m_vPreQuaternion, vCurrentQuaternion);
}

void CModel::Apply_RootMotion_Translation(CTransform* pTransform, _fvector vTranslation, _float3* pMovedDirection, _bool isActiveRotation, _fvector vQuaternion)
{
	_matrix			WorldMatrix = { pTransform->Get_WorldMatrix() };
	_vector			vTranslationLocal = { vTranslation };

	_vector			vCurrentMoveDirectionLocal = { vTranslationLocal - XMVectorSetW(XMLoadFloat3(&m_vPreTranslationLocal), 1.f) };

	if (true == isActiveRotation)
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
	XMStoreFloat3(&m_vPreTranslationLocal, vTranslationLocal);
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

MATERIAL_DESC CModel::Get_Mesh_MaterialDesc(_uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return MATERIAL_DESC();

	return m_Meshes[iMeshIndex]->Get_MaterialDesc();
}

void CModel::Set_Mesh_MaterialDesc(_uint iMeshIndex, const MATERIAL_DESC& MaterialDesc)
{
	if (iMeshIndex >= m_iNumMeshes)
		return;

	m_Meshes[iMeshIndex]->Set_MatreialDesc(MaterialDesc);
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

	return S_OK;
}

_float3 CModel::Invalidate_BonesCombinedMatix_Translation(_int iRootIndex)
{
	_vector vResultTranslationDir = { 0.f, 0.f, 0.f, 0.f };

	_uint		iIndex = { 0 };
	for (auto& pBone : m_Bones)
	{
		_bool		isCheck = { false };
		if (iIndex == iRootIndex)
		{
			CBone* pBone = m_Bones[iRootIndex];

			vResultTranslationDir = XMLoadFloat3(&pBone->Invalidate_CombinedTransformationMatrix_RootMotion_Translation(m_Bones, XMLoadFloat4x4(&m_TransformationMatrix)));
			isCheck = true;
		}

		if (false == isCheck)
			pBone->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformationMatrix));

		++iIndex;
	}

	_float3 vResultDir;
	XMStoreFloat3(&vResultDir, vResultTranslationDir);
	return vResultDir;
}

_float4x4 CModel::Invalidate_BonesCombinedMatix_TranslationMatrix(_int iRootIndex)
{
	_float4x4		ResultMatrix{};
	XMStoreFloat4x4(&ResultMatrix, XMMatrixIdentity());

	_uint			iindex = { 0 };
	for (auto& pBone : m_Bones)
	{
		_bool		ischeck = { false };
		if (iindex == iRootIndex)
		{
			ResultMatrix = pBone->Invalidate_CombinedTransformationMatrix_RootMotion_WorldMatrix(m_Bones, XMLoadFloat4x4(&m_TransformationMatrix));
			ischeck = true;
		}

		if (false == ischeck)
			pBone->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformationMatrix));

		++iindex;
	}

	return ResultMatrix;
}

HRESULT CModel::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, _uint iMeshIndex)
{
	ZeroMemory(m_MeshBoneMatrices, sizeof(_float4x4) * 512);

	m_Meshes[iMeshIndex]->Stock_Matrices(m_Bones, m_MeshBoneMatrices);

	return pShader->Bind_Matrices(pConstantName, m_MeshBoneMatrices, 512);
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

HRESULT CModel::Play_Animations(_float fTimeDelta)
{
	_uint		iPlayingIndex = { 0 };
	for (auto& AnimInfo : m_PlayingAnimInfos)
	{
		if (-1 == AnimInfo.iAnimIndex)
			continue;

		Update_LinearInterpolation(fTimeDelta, iPlayingIndex);

		//	채널에 있는 트랜슬레이션 매트릭스를 현재 애니메이션의 키프레임에 맞게 재설정한다.

		/* 현재 애니메이션에 맞는 뼈의 상태(m_TransformationMatrix)를 갱신해준다. */
		_bool		isFirstTick = { false };
		m_Animations[AnimInfo.iAnimIndex]->Invalidate_TransformationMatrix(fTimeDelta, m_Bones, AnimInfo.isLoop, &isFirstTick);

		if (true == AnimInfo.isLinearInterpolation)
		{
			m_Animations[AnimInfo.iAnimIndex]->Invalidate_TransformationMatrix_LinearInterpolation(AnimInfo.fAccLinearInterpolation, m_fTotalLinearTime, m_Bones, AnimInfo.LastKeyFrames);
		}

		//	모든 채널업데이트가 끝난후 각 뼈에 컴바인드 행렬을 설정한다.
		for (auto& pBone : m_Bones)
			pBone->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformationMatrix));

		if (false == AnimInfo.isLinearInterpolation)
		{
			Update_LastKeyFrames_Bones(AnimInfo.LastKeyFrames);
		}

		++iPlayingIndex;
	}

	return S_OK;
}

HRESULT CModel::Play_Animations_RootMotion(CTransform* pTransform, _float fTimeDelta, _float3* pMovedDirection)
{
#pragma region 1안

	//_uint		iPlayingIndex = { 0 };
	//for (auto& AnimInfo : m_PlayingAnimInfos)
	//{
	//	_bool		isFirstTick = { false };
	//	if (-1 == AnimInfo.iAnimIndex)
	//		continue;

	//	m_Animations[AnimInfo.iAnimIndex]->Invalidate_TransformationMatrix(fTimeDelta, m_Bones, AnimInfo.isLoop, &isFirstTick);

	//	Update_LinearInterpolation(fTimeDelta, iPlayingIndex);
	//	if (true == AnimInfo.isLinearInterpolation)
	//	{
	//		m_Animations[AnimInfo.iAnimIndex]->Invalidate_TransformationMatrix_LinearInterpolation(AnimInfo.fAccLinearInterpolation, m_fTotalLinearTime, m_Bones, AnimInfo.LastKeyFrames);
	//	}

	//	_bool		isActiveXZ = { m_Animations[AnimInfo.iAnimIndex]->Is_Active_RootMotion_XZ() };
	//	_bool		isActiveY = { m_Animations[AnimInfo.iAnimIndex]->Is_Active_RootMotion_Y() };
	//	_bool		isActiveRotation = { m_Animations[AnimInfo.iAnimIndex]->Is_Active_RootMotion_Rotation() };

	//	//	모든 채널업데이트가 끝난후 각 뼈에 컴바인드 행렬을 설정한다.

	//	for (auto& pBone : m_Bones)
	//	{
	//		_bool		isInclude = { false };
	//		for (auto& iIndex : AnimInfo.TargetBoneIndices)
	//		{
	//			if (pBone == m_Bones[iIndex])
	//			{
	//				isInclude = true;
	//				break;
	//			}
	//		}

	//		if (false == isInclude)
	//			continue;

	//		_bool		isRootBone = { pBone->Is_RootBone() };
	//		if (true == isRootBone)
	//		{
	//			_float4			vTranslation = { 0.f, 0.f, 0.f, 1.f };
	//			_float4			vQuaternion = {};
	//			_float4*		pTranslation = { &vTranslation };
	//			_float4*		pQuaternion = { &vQuaternion };

	//			if (false == isActiveRotation)
	//			{
	//				pQuaternion = nullptr;
	//			}

	//			if (false == isActiveXZ && false == isActiveY)
	//			{
	//				pTranslation = nullptr;
	//			}

	//			pBone->Invalidate_CombinedTransformationMatrix_RootMotion(m_Bones, m_TransformationMatrix, isActiveXZ, isActiveY, pTranslation, pQuaternion);

	//			if (true == isActiveRotation)
	//			{
	//				if (true == isFirstTick)
	//				{
	//					m_vPreQuaternion = vQuaternion;
	//				}

	//				Apply_RootMotion_Rotation(pTransform, XMLoadFloat4(&vQuaternion));
	//			}

	//			if (true == isActiveXZ || true == isActiveY)
	//			{
	//				if (true == isFirstTick)
	//				{
	//					XMStoreFloat3(&m_vPreTranslationLocal, XMLoadFloat4(&vTranslation));
	//				}

	//				Apply_RootMotion_Translation(pTransform, XMLoadFloat4(&vTranslation), pMovedDirection, isActiveRotation, XMLoadFloat4(&vQuaternion));
	//			}
	//		}

	//		else
	//		{
	//			pBone->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformationMatrix));
	//		}
	//	}

	//	if (false == AnimInfo.isLinearInterpolation)
	//	{
	//		Update_LastKeyFrames(AnimInfo.LastKeyFrames);
	//	}

	//	++iPlayingIndex;
	//}

#pragma endregion

#pragma region 2안

	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////
	////////////////////////////MOTION_BLEND////////////////////////////
	////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////

	//	재생할 모든 애니메이션을 일단 재생하고 각 키프레임을 저장하여 가져온다.
	_uint							iPlayingIndex = { 0 };
	vector<vector<_float4x4>>		TransformationMatricesLayer;
	set<_uint>						IncludedBoneIndices;
	XMStoreFloat3(pMovedDirection, XMVectorZero());
	
	_vector							vRollbackRootTranslation = {XMVectorSet(0.f, 0.f, 0.f, 1.f)};
	_vector							vRollbackRootQuaternion = { XMQuaternionIdentity() };

	_int							iRootBoneIndex = { Find_RootBoneIndex()};	

	for (auto& AnimInfo : m_PlayingAnimInfos)
	{
		_bool		isFirstTick = { false };
		//	무슨 애니메이션을 재생할건지
		if (-1 == AnimInfo.iAnimIndex)
			continue;

		vector<_float4x4>			TransformationMatrices;
		TransformationMatrices = m_Animations[AnimInfo.iAnimIndex]->Compute_TransfromationMatrix(fTimeDelta, AnimInfo.isLoop, static_cast<_uint>(m_Bones.size()), IncludedBoneIndices, &isFirstTick);
		if (true == TransformationMatrices.empty())
			continue;

		//	애니메이션이 선형보간중이었다면 선형보간된 매트릭스로 재 업데이트한다.
		Update_LinearInterpolation(fTimeDelta, iPlayingIndex);
		if (true == AnimInfo.isLinearInterpolation)
		{
			TransformationMatrices = m_Animations[AnimInfo.iAnimIndex]->Compute_TransfromationMatrix_LinearInterpolation(AnimInfo.fAccLinearInterpolation, m_fTotalLinearTime, TransformationMatrices, static_cast<_uint>(m_Bones.size()), AnimInfo.LastKeyFrames);
		}		

		//	첫틱의 경우 루트모션을 롤백하기위한 성분을 분해한다.
		if ((true == m_isRootMotion_XZ || true == m_isRootMotion_Y || true == m_isRootMotion_Rotation) &&
			true == isFirstTick &&
			-1 != iRootBoneIndex)
		{
			_vector			vScale, vQuaternion, vTranslation;
			_matrix			TransformationMatrix = { XMLoadFloat4x4(&TransformationMatrices[iRootBoneIndex]) };

			XMMatrixDecompose(&vScale, &vQuaternion, &vTranslation, TransformationMatrix);

			//	TODO:		=> 반대로 새로운 모션의 시작값역시 루트모션 시작시에 넣어줘야한다.
			if (true == m_isRootMotion_XZ)
			{
				_vector			vLastTranslation = { XMLoadFloat3(&AnimInfo.LastKeyFrames[iRootBoneIndex].vTranslation) };

				vRollbackRootTranslation += XMVectorSet(XMVectorGetX(vLastTranslation), 0.f, XMVectorGetZ(vLastTranslation), 0.f);
				vRollbackRootTranslation -= XMVectorSet(XMVectorGetX(vTranslation), 0.f, XMVectorGetZ(vTranslation), 0.f);
			}

			if (true == m_isRootMotion_Y)
			{
				_vector			vLastTranslation = { XMLoadFloat3(&AnimInfo.LastKeyFrames[iRootBoneIndex].vTranslation) };

				vRollbackRootTranslation += XMVectorSet(0.f, XMVectorGetY(vLastTranslation), 0.f, 0.f);
				vRollbackRootTranslation -= XMVectorSet(XMVectorGetX(vTranslation), 0.f, XMVectorGetZ(vTranslation), 0.f);
			}

			if (true == m_isRootMotion_Rotation)
			{
				_vector			vLastQuaternion = { XMLoadFloat4(&AnimInfo.LastKeyFrames[iRootBoneIndex].vRotation) };
				_vector			vQuaternionInv = { XMQuaternionInverse(vQuaternion) };

				vRollbackRootQuaternion = XMQuaternionNormalize(XMQuaternionMultiply(vLastQuaternion, vQuaternionInv));
			}

			vRollbackRootTranslation *= AnimInfo.fWeight;
			vRollbackRootQuaternion = XMQuaternionSlerp(XMQuaternionIdentity(), vRollbackRootQuaternion, AnimInfo.fWeight);
		}

		//	선형 보간중이아니었다면 이후에 일어날 선형 보간을 대비하여 마지막 키프레임들을 저장한다.
		if (false == AnimInfo.isLinearInterpolation)
		{			
			Update_LastKeyFrames(TransformationMatrices, AnimInfo.LastKeyFrames);
		}
		

		TransformationMatricesLayer.push_back(TransformationMatrices);

		++iPlayingIndex;
	}

	//	영향을 받지 않은 뼈들은 이전 변환행렬로 초기화한다.
	//	영향을 받은뼈는 항등 행렬로 초기화 후 이후 과정에서 행렬을 블렌드한다.
	vector<_float4x4>			ResultTransformationMatrices = { Initialize_ResultMatrices(IncludedBoneIndices) };
	_float fTotalWeight = { Compute_Current_TotalWeight()};

	//	결과행렬을 계산한다 => 얻어온 모든 행렬을 각 뼈의 인덱스에 해당하는 인덱스들에 결과행렬을 각 저장한다.
	_uint		iLayerIndex = { 0 };
	for (auto& Layer : TransformationMatricesLayer)
	{
		_uint		iBoneIndex = { 0 };
		_float		fWeight = { m_PlayingAnimInfos[iLayerIndex].fWeight / fTotalWeight };

		//	각 변환행렬을 결과행렬에 가산한다 => 웨이트를 적용하여 가산한다.
		for (auto& TransformationFloat4x4 : Layer)
		{
			_matrix			ResultMatrix = { XMLoadFloat4x4(&ResultTransformationMatrices[iBoneIndex]) };
			_matrix			TransformationMatrix = { XMLoadFloat4x4(&TransformationFloat4x4) };

			_float4x4		ResultFloat4x4 = { Compute_BlendTransformation_Additional(ResultMatrix, TransformationMatrix, fWeight) };
			
			XMStoreFloat4x4(&ResultTransformationMatrices[iBoneIndex], XMLoadFloat4x4(&ResultFloat4x4));

			if (iBoneIndex == iRootBoneIndex)
			{
				int iA = 0;
			}

			++iBoneIndex;
		}

		++iLayerIndex;
	}

	//	변환이 위에서 가해진 뼈들은 스케일이 항등행렬에 가산된 값이므로 1만큼 크다.. 그러므로 모든 스케일을 1씩 줄여준다.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*for (_uint iBoneIndex : IncludedBoneIndices)
	{
		_matrix			ResultMatrix = { XMLoadFloat4x4(&ResultTransformationMatrices[iBoneIndex]) };

		_vector			vScale, vQuaternion, vTranslation;
		XMMatrixDecompose(&vScale, &vQuaternion, &vTranslation, ResultMatrix);

		vScale -= XMVectorSet(1.f, 1.f, 1.f, 0.f);

		ResultMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vQuaternion, vTranslation);

		XMStoreFloat4x4(&ResultTransformationMatrices[iBoneIndex], ResultMatrix);
	}*/

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	TEST 위에서 항등행렬도 어차피 전부 가해지므로 모든 행렬에 대해서 스케일을 줄여주는게 맞다.
	for (auto& ResultFloat4x4 : ResultTransformationMatrices)
	{
		_matrix			ResultMatrix = { XMLoadFloat4x4(&ResultFloat4x4) };

		_vector			vScale, vQuaternion, vTranslation;
		XMMatrixDecompose(&vScale, &vQuaternion, &vTranslation, ResultMatrix);

		vScale -= XMVectorSet(1.f, 1.f, 1.f, 0.f);

		ResultMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vQuaternion, vTranslation);

		XMStoreFloat4x4(&ResultFloat4x4, ResultMatrix);
	}

	//	결과행렬들을 뼈의 트랜스폼에 저장한다.
	for (_uint iBoneIndex = 0; iBoneIndex < static_cast<_uint>(m_Bones.size()); ++iBoneIndex)
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//	TEST 포함되지않았던 행렬은 항등행렬이 아닌 이전 정보들을 담아놧으므로 그냥 전부 그대로 넣어준다.
		/*set<_uint>::iterator		iter = { IncludedBoneIndices.find(iBoneIndex) };

		if (iter == IncludedBoneIndices.end())
		{
			m_Bones[iBoneIndex]->Set_TransformationMatrix(XMMatrixIdentity());
			continue;
		}*/

		////	찾은 인덱스들은 재탐색하면 손해이므로 삭제해준다.
		//else
		//{
		//	IncludedBoneIndices.erase(iter);
		//}
		if (iBoneIndex == iRootBoneIndex)
		{
			int iA = 0;
		}
		m_Bones[iBoneIndex]->Set_TransformationMatrix(ResultTransformationMatrices[iBoneIndex]);
	}

	//	모든 채널업데이트가 끝난후 각 뼈에 컴바인드 행렬을 설정한다.
	for (auto& pBone : m_Bones)
	{
		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
		// TODO:		레이어화하여 현재 작업중인 뼈가 포함된지 확인하는 과정필요
		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////
		/*_bool		isInclude = { false };
		for (auto& iIndex : AnimInfo.TargetBoneIndices)
		{
			if (pBone == m_Bones[iIndex])
			{
				isInclude = true;
				break;
			}
		}

		if (false == isInclude)
			continue;*/

		_bool		isRootBone = { pBone->Is_RootBone() };
		if (true == isRootBone)
		{
			_float4			vTranslation = { 0.f, 0.f, 0.f, 1.f };
			_float4			vQuaternion = {};
			_float4* pTranslation = { &vTranslation };
			_float4* pQuaternion = { &vQuaternion };

			if (false == m_isRootMotion_Rotation)
			{
				pQuaternion = nullptr;
			}

			if (false == m_isRootMotion_XZ && false == m_isRootMotion_Y)
			{
				pTranslation = nullptr;
			}

			pBone->Invalidate_CombinedTransformationMatrix_RootMotion(m_Bones, m_TransformationMatrix, m_isRootMotion_XZ, m_isRootMotion_Y, pTranslation, pQuaternion);

			if (true == m_isRootMotion_Rotation)
			{
				_vector			vPreQuaternion = { XMLoadFloat4(&m_vPreQuaternion) };
				_vector			vRollbackQuaternionInv = { XMQuaternionInverse(vRollbackRootQuaternion) };

				_vector			vResultQuaternion = { XMQuaternionNormalize(XMQuaternionMultiply(vPreQuaternion, vRollbackQuaternionInv)) };

				XMStoreFloat4(&m_vPreQuaternion, vResultQuaternion);

				Apply_RootMotion_Rotation(pTransform, XMLoadFloat4(&vQuaternion));
			}

			if (true == m_isRootMotion_XZ || true == m_isRootMotion_Y)
			{
				_vector			vPreTranslation = { XMVectorSetW(XMLoadFloat3(&m_vPreTranslationLocal), 0.f) };

				_vector			vResultTranslation = { vPreTranslation - XMVectorSetW(vRollbackRootTranslation, 0.f) };

				XMStoreFloat3(&m_vPreTranslationLocal, vResultTranslation);

				Apply_RootMotion_Translation(pTransform, XMLoadFloat4(&vTranslation), pMovedDirection, m_isRootMotion_Rotation, XMLoadFloat4(&vQuaternion));
			}
		}

		else
		{
			pBone->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformationMatrix));
		}
	}

#pragma endregion

#pragma region 3안

	//_uint			iPlayingIndex = { 0 };
	//for (auto& AnimInfo : m_PlayingAnimInfos)
	//{
	//	_bool		isFirstTick = { false };
	//	if (-1 == AnimInfo.iAnimIndex)
	//		continue;

	//	m_Animations[AnimInfo.iAnimIndex]->Invalidate_TransformationMatrix(fTimeDelta, m_Bones, AnimInfo.isLoop, &isFirstTick);

	//	Update_LinearInterpolation(fTimeDelta, iPlayingIndex);
	//	if (true == AnimInfo.isLinearInterpolation)
	//	{
	//		m_Animations[AnimInfo.iAnimIndex]->Invalidate_TransformationMatrix_LinearInterpolation(AnimInfo.fAccLinearInterpolation, m_fTotalLinearTime, m_Bones, AnimInfo.LastKeyFrames);
	//	}				

	//	if (false == AnimInfo.isLinearInterpolation)
	//	{
	//		Update_LastKeyFrames(AnimInfo.LastKeyFrames);
	//	}

	//	for (auto& iBoneIndex : AnimInfo.TargetBoneIndices)
	//	{
	//		
	//	}

	//	++iPlayingIndex;
	//}

	////	현재 재생중인 애니메이션들에 포함된 모든 뼈를 종합한다.
	//list<_uint>		IncludedBoneIndices;
	//for (auto& AnimInfo : m_PlayingAnimInfos)
	//{
	//	for (_uint iBoneIndex : AnimInfo.TargetBoneIndices)
	//	{
	//		list<_uint>::iterator iter = { find_if(IncludedBoneIndices.begin(), IncludedBoneIndices.end(), iBoneIndex) };
	//		if (iter == IncludedBoneIndices.end())
	//		{
	//			IncludedBoneIndices.push_back(iBoneIndex);
	//		}
	//	}
	//}
	//IncludedBoneIndices.sort();

	////	해당 뼈에 가중치를 지닌 애니메이션들의 값을 합쳐서 넣어준다.
	//for (auto& iIndex : IncludedBoneIndices)
	//{
	//	vector<KEYFRAME>			KeyFrames;
	//	vector<_float>			Weights;

	//	for (auto& PlayingAnimInfo : m_PlayingAnimInfos)
	//	{
	//		PlayingAnimInfo.TargetBoneIndices.begin()
	//	}

	//	if (false == KeyFrames.empty())
	//	{
	//		_vector		vQuaternion = { XMQuaternionIdentity() };
	//		_vector		vScale = { XMVectorSet(0.f, 0.f, 0.f, 0.f) };
	//		_vector		vTranslation = { XMVectorSet(0.f, 0.f, 0.f, 1.f) };

	//		for (_uint i = 0; i < KeyFrames.size(); ++i)
	//		{
	//			KEYFRAME		KeyFrame = { KeyFrames[i] };
	//			_float			fWeight = { Weights[i] };

	//			XMQuaternionMultiply(vQuaternion, XMQuaternionSlerp(XMQuaternionIdentity(), XMLoadFloat4(&KeyFrame.vRotation), fWeight));
	//			vScale += { XMLoadFloat3(&KeyFrame.vScale) * fWeight };
	//			vTranslation += { XMLoadFloat3(&KeyFrame.vTranslation) * fWeight };
	//			
	//		}

	//		_matrix			TransformationMatrix = { XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vQuaternion, vTranslation) };

	//		m_Bones[iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
	//	}
	//}


	//_bool		isActiveXZ = { m_Animations[AnimInfo.iAnimIndex]->Is_Active_RootMotion_XZ() };
	//_bool		isActiveY = { m_Animations[AnimInfo.iAnimIndex]->Is_Active_RootMotion_Y() };
	//_bool		isActiveRotation = { m_Animations[AnimInfo.iAnimIndex]->Is_Active_RootMotion_Rotation() };

	////	모든 채널업데이트가 끝난후 각 뼈에 컴바인드 행렬을 설정한다.

	//for (auto& pBone : m_Bones)
	//{
	//	_bool		isRootBone = { pBone->Is_RootBone() };
	//	if (true == isRootBone)
	//	{
	//		_float4			vTranslation = { 0.f, 0.f, 0.f, 1.f };
	//		_float4			vQuaternion = {};
	//		_float4* pTranslation = { &vTranslation };
	//		_float4* pQuaternion = { &vQuaternion };

	//		if (false == isActiveRotation)
	//		{
	//			pQuaternion = nullptr;
	//		}

	//		if (false == isActiveXZ && false == isActiveY)
	//		{
	//			pTranslation = nullptr;
	//		}

	//		pBone->Invalidate_CombinedTransformationMatrix_RootMotion(m_Bones, m_TransformationMatrix, isActiveXZ, isActiveY, pTranslation, pQuaternion);

	//		if (true == isActiveRotation)
	//		{
	//			if (true == isFirstTick)
	//			{
	//				m_vPreQuaternion = vQuaternion;
	//			}

	//			Apply_RootMotion_Rotation(pTransform, XMLoadFloat4(&vQuaternion));
	//		}

	//		if (true == isActiveXZ || true == isActiveY)
	//		{
	//			if (true == isFirstTick)
	//			{
	//				XMStoreFloat3(&m_vPreTranslationLocal, XMLoadFloat4(&vTranslation));
	//			}

	//			Apply_RootMotion_Translation(pTransform, XMLoadFloat4(&vTranslation), pMovedDirection, isActiveRotation, XMLoadFloat4(&vQuaternion));
	//		}
	//	}

	//	else
	//	{
	//		pBone->Invalidate_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_TransformationMatrix));
	//	}
	//}

#pragma endregion

	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

void CModel::Static_Mesh_Cooking()
{
	for (int i = 0; i < m_Meshes.size(); ++i)
	{
		m_Meshes[i]->Static_Mesh_Cooking();
	}
}

const _float4x4* CModel::Get_CombinedMatrix(const string& strBoneTag)
{
	_int		iBoneIndex = { Find_BoneIndex(strBoneTag) };
	if (-1 == iBoneIndex)
		return nullptr;

	return m_Bones[iBoneIndex]->Get_CombinedTransformationMatrix();
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

void CModel::Reset_PreTranslation_Translation(_int iRootIndex)
{
	//	보간된 키프레임 => 새로 시작할 애니메이션의 시작점에대한 이동량이 현재 전체 누적 이동량으로 시작하면된다.
	_vector vPreDir = XMLoadFloat4(&m_Bones[iRootIndex]->Get_Translation());
	vPreDir = XMVector3TransformNormal(vPreDir, XMLoadFloat4x4(&m_TransformationMatrix));
	XMStoreFloat3(&m_vPreTranslationLocal, vPreDir);
}

void CModel::Reset_PreTranslation_WorldMatrix(_int iRootIndex)
{
	_matrix			PreTranslationMatrix = m_Bones[iRootIndex]->Get_TrasformationMatrix();

	//	PreTranslationMatrix = PreTranslationMatrix;//* XMLoadFloat4x4(&m_TransformationMatrix);
	PreTranslationMatrix = PreTranslationMatrix * XMLoadFloat4x4(&m_TransformationMatrix);

	XMStoreFloat4x4(&m_PreTranslationMatrix, PreTranslationMatrix);
}

_uint CModel::Get_CurrentMaxKeyFrameIndex(_uint iPlayingIndex)
{
	if (static_cast<_uint>(m_PlayingAnimInfos.size()) <= iPlayingIndex)
	{
		return 0;
	}

	const vector<_uint>			KeyFrameIndices = { m_Animations[m_PlayingAnimInfos[iPlayingIndex].iAnimIndex]->Get_CurrentKeyFrameIndices() };

	_uint		iMaxIndex = { 0 };
	for (_uint iIndex : KeyFrameIndices)
	{
		if (iIndex > iMaxIndex)
			iMaxIndex = iIndex;
	}

	return iMaxIndex;
}

const vector<_uint>& CModel::Get_CurrentKeyFrameIndices(_uint iPlayingIndex)
{
	return m_Animations[m_PlayingAnimInfos[iPlayingIndex].iAnimIndex]->Get_CurrentKeyFrameIndices();
}

void CModel::Set_KeyFrameIndex(_uint iPlayingIndex, _uint iKeyFrameIndex)
{
	m_Animations[m_PlayingAnimInfos[iPlayingIndex].iAnimIndex]->Set_KeyFrameIndex(iKeyFrameIndex);
}

_float CModel::Get_TrackPosition(_uint iPlayingIndex)
{
	return m_Animations[m_PlayingAnimInfos[iPlayingIndex].iAnimIndex]->Get_TrackPosition();
}

_float CModel::Get_Duration(_uint iPlayingIndex, _int iAnimIndex)
{
	if (-1 == iAnimIndex)
		iAnimIndex = m_PlayingAnimInfos[iPlayingIndex].iAnimIndex;

	return m_Animations[iAnimIndex]->Get_Duration();
}

void CModel::Set_TrackPosition(_uint iPlayingIndex, _float fTrackPosition)
{
	m_Animations[m_PlayingAnimInfos[iPlayingIndex].iAnimIndex]->Set_TrackPosition(fTrackPosition);
}

void CModel::Motion_Changed(_uint iPlayingIndex)
{
	Reset_LinearInterpolation(iPlayingIndex);

	m_PlayingAnimInfos[iPlayingIndex].isLinearInterpolation = true;
}

void CModel::Update_LastKeyFrames(const vector<_float4x4>& TransformationMatrices, vector<KEYFRAME>& LastKeyFrames)
{
	LastKeyFrames.clear();

	for (_uint iBoneIndex = 0; iBoneIndex < static_cast<_uint>(m_Bones.size()); ++iBoneIndex)
	{
		_vector			vScale, vRotation, vTranslation;
		KEYFRAME		KeyFrame;
		_matrix			TransformationMatrix = { XMLoadFloat4x4(&TransformationMatrices[iBoneIndex]) };

		XMMatrixDecompose(&vScale, &vRotation, &vTranslation, TransformationMatrix);

		XMStoreFloat3(&KeyFrame.vScale, vScale);
		XMStoreFloat4(&KeyFrame.vRotation, vRotation);
		XMStoreFloat3(&KeyFrame.vTranslation, vTranslation);
		KeyFrame.fTime = m_fTotalLinearTime;

		LastKeyFrames.push_back(KeyFrame);
	}	
}

void CModel::Update_LastKeyFrames_Bones(vector<KEYFRAME>& LastKeyFrames)
{
	LastKeyFrames.clear();

	for (auto& pBone : m_Bones)
	{
		_vector			vScale, vRotation, vTranslation;
		KEYFRAME		KeyFrame;
		_matrix			TransformationMatrix = { pBone->Get_TrasformationMatrix() };

		XMMatrixDecompose(&vScale, &vRotation, &vTranslation, TransformationMatrix);

		XMStoreFloat3(&KeyFrame.vScale, vScale);
		XMStoreFloat4(&KeyFrame.vRotation, vRotation);
		XMStoreFloat3(&KeyFrame.vTranslation, vTranslation);
		KeyFrame.fTime = m_fTotalLinearTime;

		LastKeyFrames.push_back(KeyFrame);
	}
}

void CModel::Update_LinearInterpolation(_float fTimeDelta, _uint iPlayingIndex)
{
	if (false == m_PlayingAnimInfos[iPlayingIndex].isLinearInterpolation)
		return;

	m_PlayingAnimInfos[iPlayingIndex].fAccLinearInterpolation += fTimeDelta;

	if (m_fTotalLinearTime <= m_PlayingAnimInfos[iPlayingIndex].fAccLinearInterpolation)
	{
		Reset_LinearInterpolation(iPlayingIndex);
	}
}

void CModel::Reset_LinearInterpolation(_uint iPlayingIndex)
{
	m_PlayingAnimInfos[iPlayingIndex].isLinearInterpolation = false;
	m_PlayingAnimInfos[iPlayingIndex].fAccLinearInterpolation = 0.f;
	m_PlayingAnimInfos[iPlayingIndex].LastKeyFrames.clear();
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

	m_Importer.FreeScene();
}



/* 폐기 테스트 코드

//_bool CModel::Compare(CModel* pCompareModel)
//{
//	_uint iSizeDst = pCompareModel->m_Bones.size();
//	_uint iSizeSrc = m_Bones.size();
//
//	if (iSizeDst != iSizeSrc)
//	{
//		int iA = 0;
//	}
//
//	for (_uint i = 0; i < iSizeDst; ++i)
//	{
//		m_Bones[i]->Compare(pCompareModel->m_Bones[i]);
//	}
//
//	if (m_iNumMeshes != pCompareModel->m_iNumMeshes)
//	{
//		int iA = 0;
//	}
//
//	for (_uint i = 0; i < m_iNumMeshes; ++i)
//	{
//		m_Meshes[i]->Compare(pCompareModel->m_Meshes[i]);
//	}
//
//	if (m_iNumAnimations != pCompareModel->m_iNumAnimations)
//	{
//		int iA = 0;
//	}
//
//	for (_uint i = 0; i < m_iNumAnimations; ++i)
//	{
//		m_Animations[i]->Compare(pCompareModel->m_Animations[i]);
//	}
//	return true;
//}

*/
