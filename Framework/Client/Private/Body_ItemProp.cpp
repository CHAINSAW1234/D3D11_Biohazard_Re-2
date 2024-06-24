#include "stdafx.h"
#include "Body_ItemProp.h"
#include"Player.h"

#include"ItemProp.h"
CBody_ItemProp::CBody_ItemProp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_InteractProps{ pDevice, pContext }
{
}

CBody_ItemProp::CBody_ItemProp(const CBody_ItemProp& rhs)
	: CPart_InteractProps{ rhs }
{

}

HRESULT CBody_ItemProp::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_ItemProp::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;

	BODY_ITEMPROPS_DESC* pDesc = (BODY_ITEMPROPS_DESC*)pArg;
	m_pObtain = pDesc->pObtain;

	/*m_pModelCom->Set_RootBone("RootNode");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));

	m_pModelCom->Add_AnimPlayingInfo(false, 0, TEXT("Default"), 1.f);


	m_pModelCom->Active_RootMotion_Rotation(true);*/
	//m_pTransformCom->Set_WorldMatrix(m_tagPropDesc.worldMatrix);

	if (m_strModelComponentName.find(TEXT("_Anim")) == wstring::npos)
		m_pTransformCom->Set_Scaled(100.f, 100.f, 100.f);



#ifndef NON_COLLISION_PROP

	m_pGameInstance->Create_Px_Collider(m_pModelCom, m_pParentsTransform, &m_iPx_Collider_Id);

#endif

	return S_OK;
}

void CBody_ItemProp::Tick(_float fTimeDelta)
{
	//__super::Tick(fTimeDelta);
}

void CBody_ItemProp::Late_Tick(_float fTimeDelta)
{
	if (m_bRealDead)
		return;
	if (m_bDead)
	{
		// 아이템 정보 던지고
		m_bRealDead = true;
		return;
	}
	/*
	m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pState);

	_float4 fTransform4 = m_pParentsTransform->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
	m_pModelCom->Play_Animation_Light(m_pParentsTransform, fTimeDelta);*/
	if (m_pSocketMatrix != nullptr)
	{
		_matrix			WorldMatrix = { m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pSocketMatrix) * (m_pParentsTransform->Get_WorldMatrix() /** XMMatrixScaling(100.f,100.f,100.f)*/) };
		XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
	}


	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

}

HRESULT CBody_ItemProp::Render()
{
	/*if (m_bRender == false)
		return S_OK;
	else
		m_bRender = false;*/
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
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
}

HRESULT CBody_ItemProp::Add_Components()
{
	/* For.Com_Body_Shader */
	//와 이거 나중에 골때려지겠네 Anim 아이템은 어떻게 가져오냐ㅠ
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Body_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Body_Model */
	if (FAILED(__super::Add_Component(g_Level, m_strModelComponentName,
		TEXT("Com_Body_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_ItemProp::Add_PartObjects()
{

	return S_OK;
}

HRESULT CBody_ItemProp::Initialize_PartObjects()
{
	return S_OK;
}


CBody_ItemProp* CBody_ItemProp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_ItemProp* pInstance = new CBody_ItemProp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_ItemProp"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CBody_ItemProp::Clone(void* pArg)
{
	CBody_ItemProp* pInstance = new CBody_ItemProp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_ItemProp"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_ItemProp::Free()
{
	__super::Free();

}
