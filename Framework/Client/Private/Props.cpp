#include "stdafx.h"
#include "Props.h"
#include "Model.h"
#include "GameInstance.h"
#include "Light.h"
#include "Octree.h"
#include"Player.h"
CProps::CProps(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CProps::CProps(const CProps& rhs)
	: CGameObject{ rhs }
{

}

HRESULT CProps::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProps::Initialize(void* pArg)
{
	PROPS_DESC* pObj_desc = (PROPS_DESC*)pArg;

	m_tagPropDesc.bAnim = pObj_desc->bAnim;
	m_tagPropDesc.iIndex = pObj_desc->iIndex;
	m_tagPropDesc.strGamePrototypeName = pObj_desc->strGamePrototypeName;
	m_tagPropDesc.strModelComponent = pObj_desc->strModelComponent;
	m_tagPropDesc.strObjectPrototype = pObj_desc->strObjectPrototype;
	m_tagPropDesc.worldMatrix = pObj_desc->worldMatrix;
	m_tagPropDesc.BelongIndexs = pObj_desc->BelongIndexs;
	m_tagPropDesc.iRegionDir = pObj_desc->iRegionDir;

	//memcpy_s(&m_tagPropDesc.iBelongIndexs2, sizeof(_int) * iMaxNum, &pObj_desc->iBelongIndexs2, sizeof(_int) * iMaxNum);
	for (auto iter : m_tagPropDesc.BelongIndexs)
	{
		m_tagPropDesc.BelongIndexs2[iter] = true;
	}
	pObj_desc->fSpeedPerSec = 1.f;
	pObj_desc->fRotationPerSec = XMConvertToRadians(1.f);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	m_pTransformCom->Set_WorldMatrix(m_tagPropDesc.worldMatrix);
	m_pModelCom->Static_Mesh_Cooking();


	/*m_pOctree = new COctree(m_pDevice, m_pContext, m_pGameInstance, m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));
	m_pOctree->GetSceneDimensions(m_pModelCom);
	int TotalTriangleCount = m_pOctree->GetSceneTriangleCount(m_pModelCom);
	m_pOctree->CreateNode(m_pModelCom, TotalTriangleCount, m_pOctree->GetCenter(), m_pOctree->GetWidth());*/

	//for (int i = 0; i < m_pModelCom->GetNumMesh(); ++i)
	//{
	//	m_pModelCom->Release_IndexBuffer(i);
	//}

	return S_OK;
}

void CProps::Tick(_float fTimeDelta)
{
	m_fTimeTest += fTimeDelta;
	if (m_pPlayer == nullptr)
		m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Player"))->front());
	if (m_pGameInstance->Get_KeyState(VK_F9) == DOWN && m_fTimeTest > 0.5f)
		m_bOctotree = !m_bOctotree;
	if (m_pGameInstance->Get_KeyState(VK_F10) == DOWN && m_fTimeTest > 0.5f)
		m_bShadow = !m_bShadow;
}

void CProps::Late_Tick(_float fTimeDelta)
{
	if (m_pPlayer->Get_Player_RegionChange() == true)
	{
		if (m_tagPropDesc.iRegionDir == DIRECTION_MID)
		{
			;
		}
		else if (m_pPlayer->Get_Player_Direction() != m_tagPropDesc.iRegionDir)
		{
			m_bVisible = false;
			return;
		}

		m_bVisible = m_tagPropDesc.BelongIndexs2[m_pPlayer->Get_Player_ColIndex()];
	}



	if (/*m_bVisible && true == m_pGameInstance->isInFrustum_LocalSpace(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION), 1.0f)*/1)
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

		if (m_bShadow)
		{
			m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_POINT, this);
			m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_DIR, this);
			m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
		}
	}
}

HRESULT CProps::Render()
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

	/*std::function<void()> job1 = std::bind(&COctree::DrawOctree_1, m_pOctree);
	m_pGameInstance->Insert_Job(job1);

	std::function<void()> job2 = std::bind(&COctree::DrawOctree_2, m_pOctree);
	m_pGameInstance->Insert_Job(job2);

	std::function<void()> job3 = std::bind(&COctree::DrawOctree_3, m_pOctree);
	m_pGameInstance->Insert_Job(job3);

	std::function<void()> job4 = std::bind(&COctree::DrawOctree_4, m_pOctree);
	m_pGameInstance->Insert_Job(job4);

	std::function<void()> job5 = std::bind(&COctree::DrawOctree_5, m_pOctree);
	m_pGameInstance->Insert_Job(job5);

	std::function<void()> job6 = std::bind(&COctree::DrawOctree_6, m_pOctree);
	m_pGameInstance->Insert_Job(job6);

	std::function<void()> job7 = std::bind(&COctree::DrawOctree_7, m_pOctree);
	m_pGameInstance->Insert_Job(job7);

	std::function<void()> job8 = std::bind(&COctree::DrawOctree_8, m_pOctree);
	m_pGameInstance->Insert_Job(job8);

	while (!m_pGameInstance->AllJobsFinished())
	{
		this_thread::yield();
	}

	m_pOctree->Render_Node(m_pModelCom, m_pShaderCom);*/

	return S_OK;
}

HRESULT CProps::Render_LightDepth_Dir()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (m_pGameInstance->Get_ShadowLight(CPipeLine::DIRECTION) != nullptr) {

		const CLight* pLight = m_pGameInstance->Get_ShadowLight(CPipeLine::DIRECTION);
		const LIGHT_DESC* pDesc = pLight->Get_LightDesc(0);

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pDesc->ViewMatrix[0])))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pDesc->ProjMatrix)))
			return E_FAIL;

		_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
				return E_FAIL;

			/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
			if (FAILED(m_pShaderCom->Begin(1)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	return S_OK;
}

HRESULT CProps::Render_LightDepth_Spot()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (m_pGameInstance->Get_ShadowLight(CPipeLine::SPOT) != nullptr) {

		const CLight* pLight = m_pGameInstance->Get_ShadowLight(CPipeLine::SPOT);
		const LIGHT_DESC* pDesc = pLight->Get_LightDesc(0);

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pDesc->ViewMatrix[0])))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pDesc->ProjMatrix)))
			return E_FAIL;

		_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
				return E_FAIL;

			/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
			if (FAILED(m_pShaderCom->Begin(1)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	return S_OK;
}

HRESULT CProps::Render_LightDepth_Point()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	list<LIGHT_DESC*> LightDescList = m_pGameInstance->Get_ShadowPointLightDesc_List();
	_int iIndex = 0;
	for (auto& pLightDesc : LightDescList) {
		const _float4x4* pLightViewMatrices;
		_float4x4 LightProjMatrix;
		pLightViewMatrices = pLightDesc->ViewMatrix;
		LightProjMatrix = pLightDesc->ProjMatrix;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_LightIndex", &iIndex, sizeof(_int))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrices("g_LightViewMatrix", pLightViewMatrices, 6)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_LightProjMatrix", &LightProjMatrix)))
			return E_FAIL;

		_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
				return E_FAIL;

			/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
			if (FAILED(m_pShaderCom->Begin(2)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}

		++iIndex;
	}

	return S_OK;
}

HRESULT CProps::Add_Components()
{
	if (m_tagPropDesc.bAnim)
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
			TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
			TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
			return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_tagPropDesc.strModelComponent,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CProps::Bind_ShaderResources()
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

CProps* CProps::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CProps* pInstance = new CProps(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CProps"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CProps::Clone(void* pArg)
{
	CProps* pInstance = new CProps(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CProps"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProps::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pOctree);
}
