#include "stdafx.h"
#include "Actor_PL78.h"
#include "Actor_PartObject.h"

#include "RagDoll_Physics.h"
#include "Blood.h"
#include "Blood_Drop.h"

#define BLOOD_COUNT 10
#define BIG_ATTACK_BLOOD_SIZE 6.f
#define BIG_ATTACK_BLOOD_SIZE_DROP 5.f
#define NORMAL_ATTACK_BLOOD_SIZE 4.f
#define NORMAL_ATTACK_BLOOD_SIZE_DROP 3.f
#define SHOTGUN_BLOOD_COUNT 8
#define BLOOD_DROP_COUNT 10
#define BLOOD_DROP_COUNT_STG 10
#define HEADBLOW_BLOOD_SIZE 6.5f
#define HEADBLOW_BLOOD_SIZE_DROP 6.5f

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


	m_vecBlood.clear();
	m_vecBlood_Drop.clear();

	for (size_t i = 0; i < BLOOD_COUNT; ++i)
	{
		auto pBlood = CBlood::Create(m_pDevice, m_pContext);
		pBlood->SetSize(HEADBLOW_BLOOD_SIZE, HEADBLOW_BLOOD_SIZE, HEADBLOW_BLOOD_SIZE);
		m_vecBlood.push_back(pBlood);
		pBlood->Start();
		pBlood->SetSound(false);
	}

	for (size_t i = 0; i < BLOOD_DROP_COUNT; ++i)
	{
		auto pBlood_Drop = CBlood_Drop::Create(m_pDevice, m_pContext);
		pBlood_Drop->SetSize(HEADBLOW_BLOOD_SIZE_DROP, HEADBLOW_BLOOD_SIZE_DROP, HEADBLOW_BLOOD_SIZE_DROP);
		m_vecBlood_Drop.push_back(pBlood_Drop);
		pBlood_Drop->SetDropDir_CutScene();
		pBlood_Drop->SetCutScene_Blood(true);
		pBlood_Drop->Start();
		pBlood_Drop->SetSound(false);
	}

	CModel* pBody_Model = { static_cast<CModel*>(m_PartObjects[static_cast<_uint>(ACTOR_PL78_PART::_BODY)]->Get_Component(TEXT("Com_Model"))) };
	m_SpineCombined = pBody_Model->Get_CombinedMatrix("spine_2");

	m_BloodDelay =120;

	return S_OK;
}

void CActor_PL78::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CActor_PL78::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CModel* pBody_Model = { static_cast<CModel*>(m_PartObjects[static_cast<_uint>(ACTOR_PL78_PART::_BODY)]->Get_Component(TEXT("Com_Model"))) };

	_float fTrackPosition = pBody_Model->Get_TrackPosition(0);

	if (abs(fTrackPosition - 810.f) < 1.5f)
	{
		m_bBlood = true;
		m_BloodTime = GetTickCount64();
	}

	if (m_bBlood)
	{
		SetBlood();
	}

	for (size_t i = 0; i < m_vecBlood.size(); ++i)
	{
		m_vecBlood[i]->Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecBlood_Drop.size(); ++i)
	{
		m_vecBlood_Drop[i]->Tick(fTimeDelta);
	}
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

			if (abs(fTrackPosition - 1360.f) < 1.5f)
			{
				CModel* pBody_Model = { static_cast<CModel*>(m_PartObjects[static_cast<_uint>(ACTOR_PL78_PART::_BODY)]->Get_Component(TEXT("Com_Model"))) };
				m_SpineCombined = pBody_Model->Get_CombinedMatrix("spine_1");
				m_BloodTime = GetTickCount64();
				m_BloodDelay = 20;
				m_bBlood = true;
			}
		}
	}

	for (size_t i = 0; i < m_vecBlood.size(); ++i)
	{
		m_vecBlood[i]->Late_Tick(fTimeDelta);
	}

	for (size_t i = 0; i < m_vecBlood_Drop.size(); ++i)
	{
		m_vecBlood_Drop[i]->Late_Tick(fTimeDelta);
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

void CActor_PL78::SetBlood()
{
	if (m_iBloodCount >= m_vecBlood.size())
	{
		m_bBlood = false;
		m_iBloodCount = 0;
		return;
	}

	if (m_BloodDelay + m_BloodTime < GetTickCount64())
	{
		m_BloodTime = GetTickCount64();

		m_vecBlood[m_iBloodCount]->Set_Render(true);
		//m_vecBlood[m_iBloodCount]->SetWorldMatrix_With_HitNormal(m_vHitNormal);
		m_vecBlood[m_iBloodCount]->SetType(m_pGameInstance->GetRandom_Int(1, 7));
		m_vecBlood[m_iBloodCount]->SetSize(HEADBLOW_BLOOD_SIZE, HEADBLOW_BLOOD_SIZE, HEADBLOW_BLOOD_SIZE);

		auto vPos = _float4(m_SpineCombined->_41*0.01f, m_SpineCombined->_42*0.01f+0.1f, m_SpineCombined->_43*0.01f, 1.f);

		_float4 vDelta = _float4(m_pGameInstance->GetRandom_Real(-0.1f, 0.1f),
			m_pGameInstance->GetRandom_Real(0.f, 0.1f),
			m_pGameInstance->GetRandom_Real(-0.1f, 0.1f),
			0.f);
		vPos += vDelta;

		m_vecBlood[m_iBloodCount]->SetPosition(vPos);

		if (m_iBloodCount < BLOOD_DROP_COUNT)
		{
			//m_vecBlood_Drop[m_iBloodCount]->SetWorldMatrix_With_HitNormal(m_vHitNormal);
			m_vecBlood_Drop[m_iBloodCount]->Set_Render(true);
			m_vecBlood_Drop[m_iBloodCount]->SetPosition(m_vecBlood[m_iBloodCount]->GetPosition());
			m_vecBlood_Drop[m_iBloodCount]->SetType(m_pGameInstance->GetRandom_Int(1, 7));
			m_vecBlood_Drop[m_iBloodCount]->SetSize(HEADBLOW_BLOOD_SIZE_DROP, HEADBLOW_BLOOD_SIZE_DROP, HEADBLOW_BLOOD_SIZE_DROP);
		}

		++m_iBloodCount;

		if (m_iBloodCount >= m_vecBlood.size())
		{
			m_bBlood = false;
			m_iBloodCount = 0;
			return;
		}
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

	for (size_t i = 0; i < m_vecBlood.size(); ++i)
	{
		Safe_Release(m_vecBlood[i]);
	}

	for (size_t i = 0; i < m_vecBlood_Drop.size(); ++i)
	{
		Safe_Release(m_vecBlood_Drop[i]);
	}
}
