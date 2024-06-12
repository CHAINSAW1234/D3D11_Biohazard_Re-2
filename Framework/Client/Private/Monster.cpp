#include "stdafx.h"
#include "..\Public\Monster.h"
#include "Character_Controller.h"
#include "BehaviorTree.h"
#include "PathFinder.h"

#define MODEL_SCALE 0.01f

CMonster::CMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMonster::CMonster(const CMonster & rhs)
	: CGameObject{ rhs }
{

}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void * pArg)
{
	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;	

	if (pArg != nullptr)
	{
		MONSTER_DESC* pDesc = (MONSTER_DESC*)pArg;

		m_iIndex = pDesc->Index;
	}

	//	m_pModelCom->Set_Animation(rand() % 20, true);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.f, 0.3f, 5.f, 1.f));
	m_pTransformCom->Set_Scaled(MODEL_SCALE, MODEL_SCALE, MODEL_SCALE);

	m_pController = m_pGameInstance->Create_Controller(m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION), &m_iIndex_CCT, this,1.f,0.35f);

	m_pBehaviorTree = m_pGameInstance->Create_BehaviorTree(&m_iAIController_ID);
	m_pPathFinder = m_pGameInstance->Create_PathFinder();

	m_pNavigationCom->FindCell(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));
	_int iCurrentIndex = m_pNavigationCom->GetCurrentIndex();
	
	m_pPathFinder->Initiate_PathFinding(iCurrentIndex, iCurrentIndex + 110, m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));
	m_vNextTarget = m_pPathFinder->GetNextTarget();

	m_vDir = Float4_Normalize(m_vNextTarget - m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));
	m_vDir.w = 0.f;

	return S_OK;
}

void CMonster::Tick(_float fTimeDelta)
{
	/*if (true == m_pModelCom->isFinished())
		int a = 10;*/

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pController->GetPosition_Float4());

	if (m_bArrived == false)
	{
		m_vDir = Float4_Normalize(m_vNextTarget - m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));
		m_vDir.w = 0.f;

		_float4 vDelta = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION) - m_vNextTarget;
		vDelta.y = 0.f;

		if (XMVectorGetX(XMVector3Length(XMLoadFloat4(&vDelta))) < 0.35f)
		{
			m_bArrived = true;
		}
		else
		{
			m_pController->Move(m_vDir * 0.03f, fTimeDelta);
		}
	}
	else
	{
		m_vNextTarget = m_pPathFinder->GetNextTarget();
		m_vDir = Float4_Normalize(m_vNextTarget - m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));
		m_vDir.w = 0.f;
	
		if (m_vNextTarget.x == 0 && m_vNextTarget.y == 0 && m_vNextTarget.z == 0)
			m_bArrived = true;
		else
			m_bArrived = false;
	}
}

void CMonster::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	//m_pModelCom->Play_Animations(fTimeDelta);
	m_pModelCom->Play_Animations(m_pTransformCom, fTimeDelta, &_float3(0.f,0.f,0.f));
}

HRESULT CMonster::Render()
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

void CMonster::Init_BehaviorTree_Zombie()
{
	m_pBehaviorTree = m_pGameInstance->Create_BehaviorTree(&m_iAIController_ID);

	//Root Node
	auto pRootNode = m_pBehaviorTree->GetRootNode();

	return;
}

HRESULT CMonster::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"), 
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LeonBody"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Collider_Head */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = 0.32f;
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.fRadius + 0.6f, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider_Head"), (CComponent**)&m_pColliderCom[COLLIDER_HEAD], &ColliderDesc)))
		return E_FAIL;

	/* Com_Collider_Body */
	CBounding_OBB::BOUNDING_OBB_DESC		ColliderOBBDesc{};

	ColliderOBBDesc.vRotation = _float3(0.f, XMConvertToRadians(45.0f), 0.f);
	ColliderOBBDesc.vSize = _float3(0.8f, 0.6f, 0.8f);
	ColliderOBBDesc.vCenter = _float3(0.f, ColliderOBBDesc.vSize.y * 0.5f, 0.f);
	
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
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

HRESULT CMonster::Bind_ShaderResources()
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

CMonster * CMonster::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonster*		pInstance = new CMonster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMonster"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CMonster::Clone(void * pArg)
{
	CMonster*		pInstance = new CMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CMonster"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster::Free()
{
	__super::Free();

	for (auto& pColliderCom : m_pColliderCom)
		Safe_Release(pColliderCom);

	Safe_Release(m_pShaderCom);	
	Safe_Release(m_pModelCom);
	Safe_Release(m_pBehaviorTree);
	Safe_Release(m_pPathFinder);
	Safe_Release(m_pNavigationCom);
}
