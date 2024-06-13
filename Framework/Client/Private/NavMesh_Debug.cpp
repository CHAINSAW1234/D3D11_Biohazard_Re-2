#include "stdafx.h"
#include "NavMesh_Debug.h"
#include "Model.h"
#include "GameInstance.h"
#include "Player.h"

CNavMesh_Debug::CNavMesh_Debug(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CNavMesh_Debug::CNavMesh_Debug(const CNavMesh_Debug& rhs)
	: CGameObject{ rhs }
{

}

HRESULT CNavMesh_Debug::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNavMesh_Debug::Initialize(void* pArg)
{
	PROPS_DESC* pObj_desc = (PROPS_DESC*)pArg;

	if(pArg != nullptr)
	{
		m_tagPropDesc.bAnim = pObj_desc->bAnim;
		m_tagPropDesc.iIndex = pObj_desc->iIndex;
		m_tagPropDesc.strGamePrototypeName = pObj_desc->strGamePrototypeName;
		m_tagPropDesc.strModelComponent = pObj_desc->strModelComponent;
		m_tagPropDesc.strObjectPrototype = pObj_desc->strObjectPrototype;
		m_tagPropDesc.worldMatrix = pObj_desc->worldMatrix;
		m_tagPropDesc.BelongIndexs = pObj_desc->BelongIndexs;
		m_tagPropDesc.iRegionDir = pObj_desc->iRegionDir;

		for (auto iter : m_tagPropDesc.BelongIndexs)
		{
			m_tagPropDesc.BelongIndexs2[iter] = true;
		}
		pObj_desc->fSpeedPerSec = 1.f;
		pObj_desc->fRotationPerSec = XMConvertToRadians(1.f);
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	m_pTransformCom->Set_WorldMatrix(m_tagPropDesc.worldMatrix);

	return S_OK;
}

void CNavMesh_Debug::Tick(_float fTimeDelta)
{
	m_fTimeTest += fTimeDelta;
	if (m_pPlayer == nullptr)
		m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Player"))->front());
	if (m_pGameInstance->Get_KeyState(VK_F9) == DOWN && m_fTimeTest > 0.5f)
		m_bOctotree = !m_bOctotree;
	if (m_pGameInstance->Get_KeyState(VK_F10) == DOWN && m_fTimeTest > 0.5f)
		m_bShadow = !m_bShadow;
}

void CNavMesh_Debug::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CNavMesh_Debug::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(3)))
			return E_FAIL;

		m_pModelCom->Render(static_cast<_uint>(i));
	}

	return S_OK;
}

HRESULT CNavMesh_Debug::Add_Components()
{
	if (m_tagPropDesc.bAnim)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
			TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
			TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation_Debug_Model"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNavMesh_Debug::Bind_ShaderResources()
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

CNavMesh_Debug* CNavMesh_Debug::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNavMesh_Debug* pInstance = new CNavMesh_Debug(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CNavMesh_Debug"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CNavMesh_Debug::Clone(void* pArg)
{
	CNavMesh_Debug* pInstance = new CNavMesh_Debug(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CNavMesh_Debug"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNavMesh_Debug::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
