#include "stdafx.h"
#include "Mark_Door.h"
#include "Player.h"
#include "Camera_Gimmick.h"

#include"Door.h"
CMark_Door::CMark_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_InteractProps{ pDevice, pContext }
{
}

CMark_Door::CMark_Door(const CMark_Door& rhs)
	: CPart_InteractProps{ rhs }
{

}

HRESULT CMark_Door::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMark_Door::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		EMBLEMMARK_DOOR_DESC* desc = (EMBLEMMARK_DOOR_DESC*)pArg;

		m_pEmblem_Anim = desc->EmblemAnim;
		m_eEmblemType = desc->eEmblemType;
		m_pDoorState = desc->pDoorState;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

#ifndef NON_COLLISION_PROP
	//m_pGameInstance->Create_Px_Collider(m_pModelCom, m_pParentsTransform, &m_iPx_Collider_Id);
#endif

	return S_OK;
}

void CMark_Door::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_Scaled(100.f,100.f,100.f);
}

void CMark_Door::Late_Tick(_float fTimeDelta)
{
	if (m_bDead)
		return;

	m_pTransformCom->Rotation(m_pTransformCom->Get_State_Vector(CTransform::STATE_UP), XMConvertToRadians(360.f));
	_float4x4			mWorldMatrix = { m_pTransformCom->Get_WorldMatrix() };

	mWorldMatrix._41 += 50.f;
	mWorldMatrix._42 += 145.f;
	mWorldMatrix._43 -= 5.f;



	_matrix			WorldMatrix = { };
	switch (*m_pDoorState)
	{
	case CDoor::ONEDOOR_OPEN_L:
		WorldMatrix = { mWorldMatrix **m_pSocketMatrix_01 * (m_pParentsTransform->Get_WorldMatrix()) };
		m_WorldMatrix = WorldMatrix;
		break;
	case CDoor::ONEDOOR_OPEN_R:
		WorldMatrix = { mWorldMatrix **m_pSocketMatrix_01* (m_pParentsTransform->Get_WorldMatrix()) };
		m_WorldMatrix = WorldMatrix;
		break;
	case CDoor::ONEDOOR_STATIC:
		WorldMatrix = { mWorldMatrix * *m_pSocketMatrix_01 * (m_pParentsTransform->Get_WorldMatrix()) };
		m_WorldMatrix = WorldMatrix;
		break;
	}

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
}

HRESULT CMark_Door::Render()
{
	if (m_bRender == false)
		return S_OK;
	else
		m_bRender = false;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	list<_uint>			NonHideIndices = { m_pModelCom->Get_NonHideMeshIndices() };

	for (auto& i : NonHideIndices)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_NormalTexture", static_cast<_uint>(i), aiTextureType_NORMALS)))
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
	m_pTransformCom->Rotation(m_pTransformCom->Get_State_Vector(CTransform::STATE_UP), XMConvertToRadians(180.f));
	_float4x4			mWorldMatrix = { m_pTransformCom->Get_WorldMatrix() };

	mWorldMatrix._41 += 50.f;
	mWorldMatrix._42 += 145.f;
	//mWorldMatrix._43 -= 2.f;


	_matrix			WorldMatrix = { };
	switch (*m_pDoorState)
	{
	case CDoor::ONEDOOR_OPEN_L:
		WorldMatrix = { mWorldMatrix * *m_pSocketMatrix_01 * (m_pParentsTransform->Get_WorldMatrix()) };
		m_WorldMatrix = WorldMatrix;
		break;
	case CDoor::ONEDOOR_OPEN_R:
		WorldMatrix = { mWorldMatrix * *m_pSocketMatrix_01 * (m_pParentsTransform->Get_WorldMatrix()) };
		m_WorldMatrix = WorldMatrix;
		break;
	case CDoor::ONEDOOR_STATIC:
		WorldMatrix = { mWorldMatrix * *m_pSocketMatrix_01 * (m_pParentsTransform->Get_WorldMatrix()) };
		m_WorldMatrix = WorldMatrix;
		break;
	}

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	NonHideIndices = { m_pModelCom->Get_NonHideMeshIndices() };

	for (auto& i : NonHideIndices)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_NormalTexture", static_cast<_uint>(i), aiTextureType_NORMALS)))
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

HRESULT CMark_Door::Add_Components()
{
	/* For.Com_Body_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Body_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Body_Model */
	if (FAILED(__super::Add_Component(g_Level, m_strModelComponentName,
		TEXT("Com_Body_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
#ifdef _DEBUG
#ifdef UI_POS
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(20.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Body_Collider"), (CComponent**)&m_pColliderCom[Part_INTERACTPROPS_COL_SPHERE], &ColliderDesc)))
		return E_FAIL;
#endif
#endif
	return S_OK;
}

HRESULT CMark_Door::Add_PartObjects()
{
	return S_OK;
}

HRESULT CMark_Door::Initialize_PartObjects()
{
	return S_OK;
}

CMark_Door* CMark_Door::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMark_Door* pInstance = new CMark_Door(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMark_Door"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CMark_Door::Clone(void* pArg)
{
	CMark_Door* pInstance = new CMark_Door(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CMark_Door"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMark_Door::Free()
{
	__super::Free();

}
