#include "stdafx.h"
#include "Map.h"
#include "Model.h"
#include "GameInstance.h"
#include "Light.h"
#include "Octree.h"
#include "Player.h"
#include "Rigid_Static.h"

CMap::CMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMap::CMap(const CMap& rhs)
	: CGameObject{ rhs }
{

}

HRESULT CMap::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMap::Initialize(void* pArg)
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

#pragma region Initialize RigidBody
	m_pRigid_Static = m_pGameInstance->Create_Rigid_Static(m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION), &m_iIndex_RigidBody, this);
	m_pModelCom->Static_Mesh_Cooking(nullptr,&m_iIndex_RigidBody);
#pragma endregion

#pragma region Effect
	m_pModelCom->Init_Decal(LEVEL_GAMEPLAY);
#pragma endregion

#pragma region Initializing Octree

	auto pPlayerLayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, L"Layer_Player");
	auto pPlayer = dynamic_cast<CPlayer*>(*(*pPlayerLayer).begin());

	m_pOctree = new COctree(m_pDevice, m_pContext, m_pGameInstance, m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION), pPlayer);
	m_pOctree->Set_Props_Layer(LEVEL_GAMEPLAY);
	m_pOctree->GetSceneDimensions(m_pModelCom);
	int TotalTriangleCount = m_pOctree->GetSceneTriangleCount(m_pModelCom);
	m_pOctree->CreateNode(m_pModelCom, TotalTriangleCount, m_pOctree->GetCenter(), m_pOctree->GetWidth());
	for (_uint i = 0; i < m_pModelCom->GetNumMesh(); ++i)
	{
		m_pModelCom->Release_IndexBuffer(i);
	}

#pragma endregion

	return S_OK;
}

void CMap::Tick(_float fTimeDelta)
{
	m_fTimeDelay += fTimeDelta;
	if (m_pPlayer == nullptr)
		m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Player"))->front());

	//if (m_pRigid_Static)
	//{
	//	if (m_pRigid_Static->Is_Hit())
	//	{
	//		m_pRigid_Static->Set_Hit(false);
	//		m_pModelCom->InitDecalWorldMatrix(m_pRigid_Static->GetBlockPoint(), m_pRigid_Static->GetHitNormal());
	//		m_pModelCom->Perform_Calc_DecalMap_StaticModel();
	//	}
	//}
}

void CMap::Late_Tick(_float fTimeDelta)
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

	function<void()> job1 = bind(&COctree::DrawOctree_1, m_pOctree);
	m_pGameInstance->Insert_Job(job1);

	function<void()> job2 = bind(&COctree::DrawOctree_2, m_pOctree);
	m_pGameInstance->Insert_Job(job2);

	function<void()> job3 = bind(&COctree::DrawOctree_3, m_pOctree);
	m_pGameInstance->Insert_Job(job3);

	function<void()> job4 = bind(&COctree::DrawOctree_4, m_pOctree);
	m_pGameInstance->Insert_Job(job4);

	function<void()> job5 = bind(&COctree::DrawOctree_5, m_pOctree);
	m_pGameInstance->Insert_Job(job5);

	function<void()> job6 = bind(&COctree::DrawOctree_6, m_pOctree);
	m_pGameInstance->Insert_Job(job6);

	function<void()> job7 = bind(&COctree::DrawOctree_7, m_pOctree);
	m_pGameInstance->Insert_Job(job7);

	function<void()> job8 = bind(&COctree::DrawOctree_8, m_pOctree);
	m_pGameInstance->Insert_Job(job8);

	while (!m_pGameInstance->AllJobsFinished())
	{
		this_thread::yield();
	}

	return;
}

HRESULT CMap::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pOctree->Render_Node(m_pModelCom, m_pShaderCom);

	return S_OK;
}

HRESULT CMap::Render_LightDepth_Dir()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (m_pGameInstance->Get_ShadowLight(CPipeLine::DIRECTION) != nullptr) 
	{
		const CLight* pLight = m_pGameInstance->Get_ShadowLight(CPipeLine::DIRECTION);
		const LIGHT_DESC* pDesc = pLight->Get_LightDesc(0);

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pDesc->ViewMatrix[0])))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pDesc->ProjMatrix)))
			return E_FAIL;

		m_pOctree->Render_Node_LightDepth_Dir(m_pModelCom, m_pShaderCom);
	}

	return S_OK;
}

HRESULT CMap::Render_LightDepth_Spot()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (m_pGameInstance->Get_ShadowLight(CPipeLine::SPOT) != nullptr)
	{
		const CLight* pLight = m_pGameInstance->Get_ShadowLight(CPipeLine::SPOT);
		const LIGHT_DESC* pDesc = pLight->Get_LightDesc(0);

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pDesc->ViewMatrix[0])))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pDesc->ProjMatrix)))
			return E_FAIL;

		m_pOctree->Render_Node_LightDepth_Spot(m_pModelCom, m_pShaderCom);
	}

	return S_OK;
}

HRESULT CMap::Render_LightDepth_Point()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	list<LIGHT_DESC*> LightDescList = m_pGameInstance->Get_ShadowPointLightDesc_List();
	_int iIndex = 0;
	for (auto& pLightDesc : LightDescList) 
	{
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

		m_pOctree->Render_Node_LightDepth_Point(m_pModelCom,m_pShaderCom);

		++iIndex;
	}

	return S_OK;
}

HRESULT CMap::Add_Components()
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
	if (FAILED(__super::Add_Component(g_Level, m_tagPropDesc.strModelComponent,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMap::Bind_ShaderResources()
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

CMap* CMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMap* pInstance = new CMap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMap"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CMap::Clone(void* pArg)
{
	CMap* pInstance = new CMap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CMap"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMap::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pOctree);
}
