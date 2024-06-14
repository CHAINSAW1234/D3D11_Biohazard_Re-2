#include "stdafx.h"
#include "..\Public\Zombie.h"
#include "Character_Controller.h"
#include "BehaviorTree.h"
#include "PathFinder.h"

#include "Body_Zombie.h"
#include "Face_Zombie.h"
#include "Clothes_Zombie.h"

#include "PartObject.h"

#include "Header_Package_Zombie.h"

#define MODEL_SCALE 0.01f

CZombie::CZombie(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster{ pDevice, pContext }
{
}

CZombie::CZombie(const CZombie & rhs)
	: CMonster{ rhs }
{

}

HRESULT CZombie::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CZombie::Initialize(void * pArg)
{
	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;	

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Initialize_PartModels()))
		return E_FAIL;

	if (pArg != nullptr)
	{
		MONSTER_DESC* pDesc = (MONSTER_DESC*)pArg;

		m_iIndex = pDesc->Index;
	}

	//	m_pModelCom->Set_Animation(rand() % 20, true);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.f, 0.3f, 5.f, 1.f));
	m_pTransformCom->Set_Scaled(MODEL_SCALE, MODEL_SCALE, MODEL_SCALE);

#pragma region AIController Setup
	m_pController = m_pGameInstance->Create_Controller(m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION), &m_iIndex_CCT, this,0.35f,0.35f);

	m_pBehaviorTree = m_pGameInstance->Create_BehaviorTree(&m_iAIController_ID);
	m_pPathFinder = m_pGameInstance->Create_PathFinder();

	m_pNavigationCom->FindCell(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));
	_int iCurrentIndex = m_pNavigationCom->GetCurrentIndex();
	
	m_pPathFinder->Initiate_PathFinding(iCurrentIndex, iCurrentIndex + 150, m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));
	m_vNextTarget = m_pPathFinder->GetNextTarget_Opt();

	m_vDir = Float4_Normalize(m_vNextTarget - m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));
	m_vDir.w = 0.f;

	m_pBlackBoard = new CBlackBoard_Zombie();
	m_pBlackBoard->Initialize_BlackBoard();
#pragma endregion

	return S_OK;
}

void CZombie::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pController->GetPosition_Float4());

	if (m_bArrived == false)
	{
		m_vDir = Float4_Normalize(m_vNextTarget - m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));
		m_vDir.w = 0.f;

		_float4 vDelta = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION) - m_vNextTarget;
		vDelta.y = 0.f;

		if (XMVectorGetX(XMVector3Length(XMLoadFloat4(&vDelta))) < 0.75f)
		{
			m_bArrived = true;
		}
		else
		{
			m_pController->Move(m_vDir * 0.015f, fTimeDelta);
		}
	}
	else
	{
		m_vNextTarget = m_pPathFinder->GetNextTarget_Opt();
		m_vDir = Float4_Normalize(m_vNextTarget - m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));
		m_vDir.w = 0.f;
	
		if (m_vNextTarget.x == 0 && m_vNextTarget.y == 0 && m_vNextTarget.z == 0)
			m_bArrived = true;
		else
			m_bArrived = false;
	}


	Tick_PartObjects(fTimeDelta);
}

void CZombie::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	//m_pModelCom->Play_Animations(fTimeDelta);
	m_pModelCom->Play_Animations(m_pTransformCom, fTimeDelta, &_float3(0.f,0.f,0.f));

	Late_Tick_PartObjects(fTimeDelta);
}

HRESULT CZombie::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_NormalTexture", static_cast<_uint>(i), aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_PrevBoneMatrices(m_pShaderCom, "g_PrevBoneMatrices", static_cast<_uint>(i))))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_AlphaTexture", static_cast<_uint>(i), aiTextureType_METALNESS)))
		{
			_bool isAlphaTexture = false;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isAlphaTexture", &isAlphaTexture, sizeof(_bool))))
				return E_FAIL;
		}
		else
		{
			_bool isAlphaTexture = true;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isAlphaTexture", &isAlphaTexture, sizeof(_bool))))
				return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_AOTexture", static_cast<_uint>(i), aiTextureType_SHININESS)))
		{
			_bool isAOTexture = false;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isAOTexture", &isAOTexture, sizeof(_bool))))
				return E_FAIL;
		}
		else
		{
			_bool isAOTexture = true;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isAOTexture", &isAOTexture, sizeof(_bool))))
				return E_FAIL;
		}


		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(static_cast<_uint>(i));
	}

	return S_OK;
}

void CZombie::Init_BehaviorTree_Zombie()
{
	return;

	m_pBehaviorTree = m_pGameInstance->Create_BehaviorTree(&m_iAIController_ID);

	//Root Node
	auto pNode_Root = m_pBehaviorTree->GetRootNode();

#pragma region RootNode Childe Section

	/*Root Child_1 Section*/

	//Add RootNode Composite Node - Selector Node_1
	auto pSelectorNode_RootChild_1 = new CComposite_Node();
	pNode_Root->Insert_Child_Node(pSelectorNode_RootChild_1);

	//Add Decorator Node
	auto pIs_Character_In_Range = new CIs_Character_In_Range_Zombie();
	pIs_Character_In_Range->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_1->Insert_Decorator_Node(pIs_Character_In_Range);

	//Add Task Node
	auto pMoveTo = new CMoveTo_Zombie();
	pMoveTo->SetBlackBoard(m_pBlackBoard);
	pSelectorNode_RootChild_1->Insert_Task_Node(pMoveTo);
#pragma endregion
	return;
}

HRESULT CZombie::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"), 
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_LeonBody"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Collider_Head */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = 0.32f;
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.fRadius + 0.6f, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Head"), (CComponent**)&m_pColliderCom[COLLIDER_HEAD], &ColliderDesc)))
		return E_FAIL;

	/* Com_Collider_Body */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderOBBDesc{};

	ColliderOBBDesc.vRotation = _float3(0.f, XMConvertToRadians(45.0f), 0.f);
	ColliderOBBDesc.vSize = _float3(0.8f, 0.6f, 0.8f);
	ColliderOBBDesc.vCenter = _float3(0.f, ColliderOBBDesc.vSize.y * 0.5f, 0.f);
	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider_Body"), (CComponent**)&m_pColliderCom[COLLIDER_BODY], &ColliderOBBDesc)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIGATION_DESC			NavigationDesc{};

	NavigationDesc.iCurrentIndex = 0;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NavigationDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CZombie::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;	

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CZombie::Add_PartObjects()
{
	m_PartObjects.resize(CZombie::PART_ID::PART_END);

	/* For.Part_Body */
	CPartObject*							pBodyObject = { nullptr };
	CBody_Zombie::BODY_MONSTER_DESC			BodyDesc;

	BodyDesc.pParentsTransform = m_pTransformCom;
	BodyDesc.pRootTranslation = &m_vRootTranslation;

	pBodyObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Body_Zombie"), &BodyDesc));
	if (nullptr == pBodyObject)
		return E_FAIL;

	m_PartObjects[CZombie::PART_ID::PART_BODY] = pBodyObject;


	/* For.Part_Face */
	CPartObject*							pFaceObject = { nullptr };
	CFace_Zombie::FACE_MONSTER_DESC			FaceDesc;

	FaceDesc.pParentsTransform = m_pTransformCom;

	pFaceObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Face_Zombie"), &FaceDesc));
	if (nullptr == pFaceObject)
		return E_FAIL;

	m_PartObjects[CZombie::PART_ID::PART_FACE] = pFaceObject;


	/* For.Part_Hat */
	CPartObject*								pHatObject = { nullptr };
	CClothes_Zombie::CLOTHES_MONSTER_DESC		ClothesHatDesc;

	ClothesHatDesc.pParentsTransform = m_pTransformCom;
	ClothesHatDesc.eType = CClothes_Zombie::CLOTHES_TYPE::TYPE_HAT;

	pHatObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Clothes_Zombie"), &ClothesHatDesc));
	if (nullptr == pHatObject)
		return E_FAIL;

	m_PartObjects[CZombie::PART_ID::PART_HAT] = pHatObject;


	/* For.Part_Shirts */
	CPartObject*								pShirtsObject = { nullptr };
	CClothes_Zombie::CLOTHES_MONSTER_DESC		ClothesShirtsDesc;

	ClothesShirtsDesc.pParentsTransform = m_pTransformCom;
	ClothesShirtsDesc.eType = CClothes_Zombie::CLOTHES_TYPE::TYPE_SHIRTS;
		
	pShirtsObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Clothes_Zombie"), &ClothesShirtsDesc));
	if (nullptr == pShirtsObject)
		return E_FAIL;

	m_PartObjects[CZombie::PART_ID::PART_SHIRTS] = pShirtsObject;


	/* For.Part_Pants */
	CPartObject*								pPantsObject = { nullptr };
	CClothes_Zombie::CLOTHES_MONSTER_DESC		ClothesPantsDesc;

	ClothesPantsDesc.pParentsTransform = m_pTransformCom;
	ClothesPantsDesc.eType = CClothes_Zombie::CLOTHES_TYPE::TYPE_PANTS;

	pPantsObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Part_Clothes_Zombie"), &ClothesPantsDesc));
	if (nullptr == pPantsObject)
		return E_FAIL;

	m_PartObjects[CZombie::PART_ID::PART_PANTS] = pPantsObject;

	return S_OK;
}

HRESULT CZombie::Initialize_PartModels()
{
	CModel*					pBodyModel = { dynamic_cast<CModel*>(m_PartObjects[PART_BODY]->Get_Component(TEXT("Com_Model"))) };
	CModel*					pFaceModel = { dynamic_cast<CModel*>(m_PartObjects[PART_FACE]->Get_Component(TEXT("Com_Model"))) };
	CModel*					pShirtsModel = { dynamic_cast<CModel*>(m_PartObjects[PART_SHIRTS]->Get_Component(TEXT("Com_Model"))) };
	CModel*					pPantsModel = { dynamic_cast<CModel*>(m_PartObjects[PART_PANTS]->Get_Component(TEXT("Com_Model"))) };
	CModel*					pHatModel = { dynamic_cast<CModel*>(m_PartObjects[PART_HAT]->Get_Component(TEXT("Com_Model"))) };

	if (nullptr == pBodyModel ||
		nullptr == pFaceModel ||
		nullptr == pShirtsModel ||
		nullptr == pPantsModel ||
		nullptr == pHatModel)
		return E_FAIL;

	if (FAILED(pFaceModel->Link_Bone_Auto(pBodyModel)))
		return E_FAIL;
	if (FAILED(pShirtsModel->Link_Bone_Auto(pBodyModel)))
		return E_FAIL;
	if (FAILED(pPantsModel->Link_Bone_Auto(pBodyModel)))
		return E_FAIL;
	if (FAILED(pHatModel->Link_Bone_Auto(pBodyModel)))
		return E_FAIL;

	return S_OK;
}

CZombie * CZombie::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CZombie*		pInstance = new CZombie(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CZombie"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CZombie::Clone(void * pArg)
{
	CZombie*		pInstance = new CZombie(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CZombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CZombie::Free()
{
	__super::Free();

	Safe_Release(m_pBlackBoard);
}
