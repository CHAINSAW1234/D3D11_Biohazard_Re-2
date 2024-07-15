#include "stdafx.h"
#include "Item_Statue.h"
#include"Player.h"
#include "Camera_Gimmick.h"

CItem_Statue::CItem_Statue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_InteractProps{ pDevice, pContext }
{
}

CItem_Statue::CItem_Statue(const CItem_Statue& rhs)
	: CPart_InteractProps{ rhs }
{

}

HRESULT CItem_Statue::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItem_Statue::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (pArg != nullptr)
	{
		BODY_ITEM_STATUE* desc = (BODY_ITEM_STATUE*)pArg;

		m_Item_Type = static_cast<STATUE_ITEM>(desc->eItemType);
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_RootBone("RootNode");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));
	m_pModelCom->Add_AnimPlayingInfo(false, 0, TEXT("Default"), 1.f);
	m_pModelCom->Active_RootMotion_Rotation(true);

#ifndef NON_COLLISION_PROP
	m_pGameInstance->Create_Px_Collider(m_pModelCom, m_pParentsTransform, &m_iPx_Collider_Id);
#endif


	return S_OK;
}

void CItem_Statue::Tick(_float fTimeDelta)
{
	if (DOWN == m_pGameInstance->Get_KeyState('J') )
	{
		m_pCameraGimmick->Active_Camera(true);
	}
}

void CItem_Statue::Late_Tick(_float fTimeDelta)
{
	return;
	if (m_bDead)
		return;

	_float3	vDirection = { };
	_float4 fTransform4 = m_pParentsTransform->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };

	m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, &vDirection);

	// 
	_matrix			WorldMatrix = { m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pSocketMatrix) * (m_pParentsTransform->Get_WorldMatrix()) };
	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
}

HRESULT CItem_Statue::Render()
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

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
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

HRESULT CItem_Statue::Add_Components()
{
	/* For.Com_Body_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
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

HRESULT CItem_Statue::Add_PartObjects()
{

	return S_OK;
}

HRESULT CItem_Statue::Initialize_PartObjects()
{
	return S_OK;
}


CItem_Statue* CItem_Statue::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItem_Statue* pInstance = new CItem_Statue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CItem_Statue"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CItem_Statue::Clone(void* pArg)
{
	CItem_Statue* pInstance = new CItem_Statue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CItem_Statue"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem_Statue::Free()
{
	__super::Free();

}
