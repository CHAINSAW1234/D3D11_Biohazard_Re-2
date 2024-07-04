#include "stdafx.h"
#include "Body_Ladder.h"
#include"Player.h"

#include"Statue.h"
CBody_Ladder::CBody_Ladder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_InteractProps{ pDevice, pContext }
{
}

CBody_Ladder::CBody_Ladder(const CBody_Ladder& rhs)
	: CPart_InteractProps{ rhs }
{

}

HRESULT CBody_Ladder::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Ladder::Initialize(void* pArg)
{
	/*문자식 파트오브젝트 붙혀야하는데 뼈가 문고리에 없어서 직접 찍어야 하는데
	프로토타입 끝나고 뼈 붙혀보겠나이다*/

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;

	//m_pTransformCom->Set_WorldMatrix(m_tagPropDesc.worldMatrix);

#ifndef NON_COLLISION_PROP

	//m_pGameInstance->Create_Px_Collider(m_pModelCom, m_pParentsTransform, &m_iPx_Collider_Id);

#endif

	return S_OK;
}

void CBody_Ladder::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

}

void CBody_Ladder::Late_Tick(_float fTimeDelta)
{

	if (m_pColliderCom[Part_INTERACTPROPS_COL_SPHERE] != nullptr)
		m_pColliderCom[Part_INTERACTPROPS_COL_SPHERE]->Tick(m_pParentsTransform->Get_WorldMatrix());

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

}

HRESULT CBody_Ladder::Render()
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

HRESULT CBody_Ladder::Add_Components()
{
	/* For.Com_Body_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Body_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Body_Model */
	if (FAILED(__super::Add_Component(g_Level, m_strModelComponentName,
		TEXT("Com_Body_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Ladder::Add_PartObjects()
{

	return S_OK;
}

HRESULT CBody_Ladder::Initialize_PartObjects()
{
	return S_OK;
}


CBody_Ladder* CBody_Ladder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Ladder* pInstance = new CBody_Ladder(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_Ladder"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CBody_Ladder::Clone(void* pArg)
{
	CBody_Ladder* pInstance = new CBody_Ladder(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_Ladder"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Ladder::Free()
{
	__super::Free();

}
