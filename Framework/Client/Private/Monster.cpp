#include "stdafx.h"
#include "..\Public\Monster.h"
#include "Character_Controller.h"
#include "BehaviorTree.h"
#include "PathFinder.h"

#include "PartObject.h"

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

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Initialize_PartModels()))
		return E_FAIL;

	if (FAILED(Initialize_Status()))
		return E_FAIL;

	return S_OK;
}

void CMonster::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

	Priority_Tick_PartObjects(fTimeDelta);
}

void CMonster::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Tick_PartObjects(fTimeDelta);
}

void CMonster::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	Late_Tick_PartObjects(fTimeDelta);
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

_bool CMonster::Distance_Culling()
{
	auto vPlayerPos = m_pGameInstance->GetPlayerPos();
	auto vMonsterPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
	auto vDelta = vPlayerPos - vMonsterPos;
	auto fDelta = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vDelta)));

	if (fDelta < CULLING_DISTANCE)
		return true;
	else
		return false;
}

void CMonster::Move(_float4 vDir, _float fTimeDelta)
{
	if (m_pController)
		m_pController->Move(vDir, fTimeDelta);
}

void CMonster::Priority_Tick_PartObjects(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
	{
		if (nullptr != pPartObject)
			pPartObject->Priority_Tick(fTimeDelta);
	}
}

void CMonster::Tick_PartObjects(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
	{
		if (nullptr != pPartObject)
			pPartObject->Tick(fTimeDelta);
	}
}

void CMonster::Late_Tick_PartObjects(_float fTimeDelta)
{
	static _uint			iCurrentShirts = { 0 };

	if (DOWN == m_pGameInstance->Get_KeyState(VK_UP))
	{
		iCurrentShirts += 1;
		if (iCurrentShirts > 2)
		{
			iCurrentShirts = 0;
		}
	}

	for (auto& pPartObject : m_PartObjects)
	{
		if (nullptr != pPartObject)
		{
			if (iCurrentShirts == 0)
			{
				if (pPartObject == m_PartObjects[PART_SHIRTS2])
					continue;

				if (pPartObject == m_PartObjects[PART_SHIRTS3])
					continue;


				if (pPartObject == m_PartObjects[PART_FACE2])
					continue;

				if (pPartObject == m_PartObjects[PART_FACE3])
					continue;
			}
			
			if (iCurrentShirts == 1)
			{
				if (pPartObject == m_PartObjects[PART_SHIRTS])
					continue;

				if (pPartObject == m_PartObjects[PART_SHIRTS3])
					continue;


				if (pPartObject == m_PartObjects[PART_FACE])
					continue;

				if (pPartObject == m_PartObjects[PART_FACE3])
					continue;
			}

			if (iCurrentShirts == 2)
			{
				if (pPartObject == m_PartObjects[PART_SHIRTS])
					continue;

				if (pPartObject == m_PartObjects[PART_SHIRTS2])
					continue;

				if (pPartObject == m_PartObjects[PART_FACE])
					continue;

				if (pPartObject == m_PartObjects[PART_FACE2])
					continue;
			}

			pPartObject->Late_Tick(fTimeDelta);
		}
	}
}

CModel* CMonster::Get_Model_From_PartObject(PART_ID eID)
{
	CModel*				pModel = { nullptr };
	
	if (eID >= PART_END)
		return pModel;

	CPartObject*		pPartObject = { m_PartObjects[static_cast<_uint>(eID)] };
	pModel = dynamic_cast<CModel*>(pPartObject->Get_Component(TEXT("Com_Model")));

	return pModel;
}

CPartObject* CMonster::Get_PartObject(PART_ID eID)
{
	if (eID >= PART_END)
		return nullptr;

	return m_PartObjects[static_cast<_uint>(eID)];
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

	for (auto& pPartObject : m_PartObjects)
	{
		Safe_Release(pPartObject);
		pPartObject = nullptr;
	}
	m_PartObjects.clear();

	Safe_Delete(m_pStatus);
}
