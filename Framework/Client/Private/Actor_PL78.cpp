#include "stdafx.h"
#include "Actor_PL78.h"
#include "Actor_PartObject.h"

#include "RagDoll_Physics.h"

CActor_PL78::CActor_PL78(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CActor{ pDevice, pContext }
{
}

CActor_PL78::CActor_PL78(const CActor& rhs)
	: CActor{ rhs }
{
}

HRESULT CActor_PL78::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_PL78::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Initialize_Models()))
		return E_FAIL;

	CModel*				pRagDoll_Model = { static_cast<CModel*>(m_PartObjects[static_cast<_uint>(CActor_PL78::ACTOR_PL78_PART::_RAGDOLL)]->Get_Component(TEXT("Com_Model"))) };
	if (nullptr == pRagDoll_Model)
		return E_FAIL;

	m_PartObjects[static_cast<_uint>(ACTOR_PL78_PART::_GUTS)]->Set_Render(false);
	m_pRagDoll = m_pGameInstance->Create_Ragdoll(pRagDoll_Model->GetBoneVector(), m_pTransformCom, m_pDevice, m_pContext, "../Bin/Resources/Models/CutScene/pl7800/pl7800.fbx");

	return S_OK;
}

void CActor_PL78::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CActor_PL78::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CActor_PL78::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	CModel* pBody_Model = { static_cast<CModel*>(m_PartObjects[static_cast<_uint>(ACTOR_PL78_PART::_BODY)]->Get_Component(TEXT("Com_Model"))) };
	if (nullptr != pBody_Model)
	{
		_int			iAnimIndex = { pBody_Model->Get_AnimIndex_PlayingInfo(static_cast<_uint>(0)) };
		_float			fTrackPosition = { pBody_Model->Get_TrackPosition(static_cast<_uint>(0)) };

		if (fTrackPosition > 1360.f && 0 == iAnimIndex)
		{
			Cut_Body();
		}
	}
}

HRESULT CActor_PL78::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_PL78::Add_PartObjects()
{
	m_PartObjects.resize(static_cast<size_t>(ACTOR_PL78_PART::_END));

	CActor_PartObject::ACTOR_PART_DESC			Body_Desc;
	Body_Desc.pParentsTransform = m_pTransformCom;
	Body_Desc.pRootTranslation = &m_vRootTranslation;
	Body_Desc.isBaseObject = true;
	Body_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_PL7800");
	Body_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF93_PL7800"));

	CActor_PartObject*			pPartObject_Body = { CActor_PartObject::Create(m_pDevice, m_pContext, &Body_Desc) };
	if (nullptr == pPartObject_Body)
		return E_FAIL;

	m_PartObjects[static_cast<_uint>(ACTOR_PL78_PART::_BODY)] = pPartObject_Body;

	CActor_PartObject::ACTOR_PART_DESC			Head_Desc;
	Head_Desc.pParentsTransform = m_pTransformCom;
	Head_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_PL7850");
	Head_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF93_PL7850"));

	CActor_PartObject*			pPartObject_Head = { CActor_PartObject::Create(m_pDevice, m_pContext, &Head_Desc) };
	if (nullptr == pPartObject_Head)
		return E_FAIL;

	m_PartObjects[static_cast<_uint>(ACTOR_PL78_PART::_HEAD)] = pPartObject_Head;

	CActor_PartObject::ACTOR_PART_DESC			Hair_Desc;
	Hair_Desc.pParentsTransform = m_pTransformCom;
	Hair_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_PL7870");

	CActor_PartObject*			pPartObject_Hair = { CActor_PartObject::Create(m_pDevice, m_pContext, &Hair_Desc) };
	if (nullptr == pPartObject_Hair)
		return E_FAIL;

	m_PartObjects[static_cast<_uint>(ACTOR_PL78_PART::_HAIR)] = pPartObject_Hair;

	CActor_PartObject::ACTOR_PART_DESC			Guts_Desc;
	Guts_Desc.pParentsTransform = m_pTransformCom;
	Guts_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_PL7880");
	Guts_Desc.AnimPrototypeLayerTags.emplace_back(TEXT("CF93_PL7880"));

	CActor_PartObject*			pPartObject_Guts = { CActor_PartObject::Create(m_pDevice, m_pContext, &Guts_Desc) };
	if (nullptr == pPartObject_Guts)
		return E_FAIL;

	m_PartObjects[static_cast<_uint>(ACTOR_PL78_PART::_GUTS)] = pPartObject_Guts;

	CModel* pHeadModel = { static_cast<CModel*>(m_PartObjects[static_cast<_uint>(ACTOR_PL78_PART::_HEAD)]->Get_Component(TEXT("Com_Model"))) };
	CModel* pHairModel = { static_cast<CModel*>(m_PartObjects[static_cast<_uint>(ACTOR_PL78_PART::_HAIR)]->Get_Component(TEXT("Com_Model"))) };
	pHairModel->Link_Bone_Auto(pHeadModel);


	CActor_PartObject::ACTOR_PART_DESC			RagDoll_Desc;
	RagDoll_Desc.pParentsTransform = m_pTransformCom;
	RagDoll_Desc.pRootTranslation = &m_vRootTranslation;
	RagDoll_Desc.isBaseObject = true;
	RagDoll_Desc.strModelPrototypeTag = TEXT("Prototype_Component_Model_PL7800");

	CActor_PartObject* pPartObject_RagDoll = { CActor_PartObject::Create(m_pDevice, m_pContext, &RagDoll_Desc) };
	if (nullptr == pPartObject_RagDoll)
		return E_FAIL;

	pPartObject_RagDoll->Set_LinkAuto(false);

	m_PartObjects[static_cast<_uint>(ACTOR_PL78_PART::_RAGDOLL)] = pPartObject_RagDoll;

	return S_OK;
}

HRESULT CActor_PL78::Initialize_Models()
{
	CModel* pBody_Model = { static_cast<CModel*>(m_PartObjects[static_cast<_uint>(CActor_PL78::ACTOR_PL78_PART::_BODY)]->Get_Component(TEXT("Com_Model"))) };
	if (nullptr == pBody_Model)
		return E_FAIL;

	vector<string>			BodyMeshTags = { pBody_Model->Get_MeshTags() };
	_uint					iBodyMeshIndex = { 0 };
	for (auto& strMeshTag : BodyMeshTags)
	{
		if (strMeshTag.find("LOD_1_Group_0_Sub_1__pl7800_Body_Mat1") != string::npos)
		{
			pBody_Model->Set_Mesh_Branch(iBodyMeshIndex, static_cast<_uint>(CActor_PL78::ACTOR_PL78_MESH_BRANCH::_UPPER_BODY));
		}

		else if (strMeshTag.find("LOD_1_Group_0_Sub_2__pl7800_Armor_Mat1") != string::npos)
		{
			pBody_Model->Set_Mesh_Branch(iBodyMeshIndex, static_cast<_uint>(CActor_PL78::ACTOR_PL78_MESH_BRANCH::_UPPER_BODY));
		}

		else if (strMeshTag.find("LOD_1_Group_1_Sub_1__pl7800_Body_Mat1") != string::npos)
		{
			pBody_Model->Set_Mesh_Branch(iBodyMeshIndex, static_cast<_uint>(CActor_PL78::ACTOR_PL78_MESH_BRANCH::_LOWER_BODY));
		}

		else if (strMeshTag.find("LOD_1_Group_1_Sub_2__pl7800_Props_Mat1") != string::npos)
		{
			pBody_Model->Set_Mesh_Branch(iBodyMeshIndex, static_cast<_uint>(CActor_PL78::ACTOR_PL78_MESH_BRANCH::_LOWER_BODY));
		}

		else if (strMeshTag.find("LOD_1_Group_2_Sub_1__pl7800_Props_Mat1") != string::npos)
		{
			pBody_Model->Set_Mesh_Branch(iBodyMeshIndex, static_cast<_uint>(CActor_PL78::ACTOR_PL78_MESH_BRANCH::_LOWER_BODY));
		}

		else if (strMeshTag.find("LOD_1_Group_3_Sub_1__pl7800_Props_Mat1") != string::npos)
		{
			pBody_Model->Set_Mesh_Branch(iBodyMeshIndex, static_cast<_uint>(CActor_PL78::ACTOR_PL78_MESH_BRANCH::_UPPER_BODY));
		}

		++iBodyMeshIndex;
	}

	CModel*					pRagDoll_Model = { static_cast<CModel*>(m_PartObjects[static_cast<_uint>(CActor_PL78::ACTOR_PL78_PART::_RAGDOLL)]->Get_Component(TEXT("Com_Model"))) };
	if (nullptr == pRagDoll_Model)
		return E_FAIL;

	vector<string>			RagDollMeshTags = { pRagDoll_Model->Get_MeshTags() };
	_uint					iRagDollMeshIndex = { 0 };
	for (auto& strMeshTag : RagDollMeshTags)
	{
		if (strMeshTag.find("LOD_1_Group_0_Sub_1__pl7800_Body_Mat1") != string::npos)
		{
			pRagDoll_Model->Set_Mesh_Branch(iRagDollMeshIndex, static_cast<_uint>(CActor_PL78::ACTOR_PL78_MESH_BRANCH::_UPPER_BODY));
		}

		else if (strMeshTag.find("LOD_1_Group_0_Sub_2__pl7800_Armor_Mat1") != string::npos)
		{
			pRagDoll_Model->Set_Mesh_Branch(iRagDollMeshIndex, static_cast<_uint>(CActor_PL78::ACTOR_PL78_MESH_BRANCH::_UPPER_BODY));
		}

		else if (strMeshTag.find("LOD_1_Group_1_Sub_1__pl7800_Body_Mat1") != string::npos)
		{
			pRagDoll_Model->Set_Mesh_Branch(iRagDollMeshIndex, static_cast<_uint>(CActor_PL78::ACTOR_PL78_MESH_BRANCH::_LOWER_BODY));
		}

		else if (strMeshTag.find("LOD_1_Group_1_Sub_2__pl7800_Props_Mat1") != string::npos)
		{
			pRagDoll_Model->Set_Mesh_Branch(iRagDollMeshIndex, static_cast<_uint>(CActor_PL78::ACTOR_PL78_MESH_BRANCH::_LOWER_BODY));
		}

		else if (strMeshTag.find("LOD_1_Group_2_Sub_1__pl7800_Props_Mat1") != string::npos)
		{
			pRagDoll_Model->Set_Mesh_Branch(iRagDollMeshIndex, static_cast<_uint>(CActor_PL78::ACTOR_PL78_MESH_BRANCH::_LOWER_BODY));
		}

		else if (strMeshTag.find("LOD_1_Group_3_Sub_1__pl7800_Props_Mat1") != string::npos)
		{
			pRagDoll_Model->Set_Mesh_Branch(iRagDollMeshIndex, static_cast<_uint>(CActor_PL78::ACTOR_PL78_MESH_BRANCH::_UPPER_BODY));
		}

		++iRagDollMeshIndex;
	}


	pRagDoll_Model->Hide_Mesh_Branch(static_cast<_uint>(ACTOR_PL78_MESH_BRANCH::_LOWER_BODY), true);
	pRagDoll_Model->Hide_Mesh_Branch(static_cast<_uint>(ACTOR_PL78_MESH_BRANCH::_UPPER_BODY), true);

	return S_OK;
}

void CActor_PL78::Cut_Body()
{
	if (true == m_isCutBody)
		return;

	CModel*					pBody_Model = { static_cast<CModel*>(m_PartObjects[static_cast<_uint>(CActor_PL78::ACTOR_PL78_PART::_BODY)]->Get_Component(TEXT("Com_Model"))) };
	if (nullptr == pBody_Model)
		return;

	pBody_Model->Hide_Mesh_Branch(static_cast<_uint>(ACTOR_PL78_MESH_BRANCH::_LOWER_BODY), true);

	CModel*					pRagDoll_Model = { static_cast<CModel*>(m_PartObjects[static_cast<_uint>(CActor_PL78::ACTOR_PL78_PART::_RAGDOLL)]->Get_Component(TEXT("Com_Model"))) };
	if (nullptr == pRagDoll_Model)
		return;

	pRagDoll_Model->Hide_Mesh_Branch(static_cast<_uint>(ACTOR_PL78_MESH_BRANCH::_LOWER_BODY), false);

	m_PartObjects[static_cast<_uint>(ACTOR_PL78_PART::_GUTS)]->Set_Render(true);

	m_PartObjects[static_cast<_uint>(ACTOR_PL78_PART::_RAGDOLL)]->Set_Pause_Anim(true);
	Apply_LastAnimCombined_RagDoll();
	SetRagdoll();

	m_isCutBody = true;
}

void CActor_PL78::SetRagdoll()
{
	//	CModel* pRagDoll_Model = { static_cast<CModel*>(m_PartObjects[static_cast<_uint>(CActor_PL78::ACTOR_PL78_PART::_RAGDOLL)]->Get_Component(TEXT("Com_Model"))) };
	//	if (nullptr == pRagDoll_Model)
	//		return;
	//	
	//	//	m_pGameInstance->Start_Ragdoll(m_pRagDoll, iId);
	//	
	//	m_pRagDoll->Add_Force(_float4(0.f, 0.f, 0.f, 0.f), COLLIDER_TYPE::CHEST);
	//	m_bRagdoll = true;
	//	pRagDoll_Model->Set_OptimizationCulling(false);
}

void CActor_PL78::Apply_LastAnimCombined_RagDoll()
{
	CModel* pBody_Model = { static_cast<CModel*>(m_PartObjects[static_cast<_uint>(CActor_PL78::ACTOR_PL78_PART::_BODY)]->Get_Component(TEXT("Com_Model"))) };
	CModel* pRagDoll_Model = { static_cast<CModel*>(m_PartObjects[static_cast<_uint>(CActor_PL78::ACTOR_PL78_PART::_RAGDOLL)]->Get_Component(TEXT("Com_Model"))) };

	if (nullptr == pBody_Model)
		return;
	if (nullptr == pRagDoll_Model)
		return;

	vector<string>			BoneTags = { pBody_Model->Get_BoneNames() };
	for (auto& strBoneTag : BoneTags)
	{
		_float4x4*				pBoneCombiend = { const_cast<_float4x4*>(pBody_Model->Get_CombinedMatrix(strBoneTag)) };
		if (nullptr == pBoneCombiend)
			continue;

		pRagDoll_Model->Set_CombinedMatrix(strBoneTag, XMLoadFloat4x4(pBoneCombiend));
	}

}

CActor_PL78* CActor_PL78::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CActor_PL78*			pInstance = new CActor_PL78(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CActor_PL78"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CActor_PL78::Clone(void* pArg)
{
	CActor_PL78* pInstance = new CActor_PL78(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CActor_PL78"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CActor_PL78::Free()
{
	__super::Free();
}
