#include "stdafx.h"
#include "InteractProps.h"
#include "Model.h"
#include "GameInstance.h"
#include "Light.h"
#include"Player.h"
#include"PartObject.h"

#include"Part_InteractProps.h"

CInteractProps::CInteractProps(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CInteractProps::CInteractProps(const CInteractProps& rhs)
	: CGameObject{ rhs }
{

}

HRESULT CInteractProps::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteractProps::Initialize(void* pArg)
{
	INTERACTPROPS_DESC* pObj_desc = (INTERACTPROPS_DESC*)pArg;

	m_tagPropDesc.bAnim = pObj_desc->bAnim;
	m_tagPropDesc.iIndex = pObj_desc->iIndex;
	m_tagPropDesc.strGamePrototypeName = pObj_desc->strGamePrototypeName;
	m_tagPropDesc.strModelComponent = pObj_desc->strModelComponent;
	m_tagPropDesc.strObjectPrototype = pObj_desc->strObjectPrototype;
	m_tagPropDesc.worldMatrix = pObj_desc->worldMatrix;
	m_tagPropDesc.BelongIndexs = pObj_desc->BelongIndexs;
	m_tagPropDesc.iRegionDir = pObj_desc->iRegionDir;
	m_tagPropDesc.iRegionNum = pObj_desc->iRegionNum;
	m_tagPropDesc.iPartObj = pObj_desc->iPartObj;
	m_tagPropDesc.iPropType = pObj_desc->iPropType;
	m_tagPropDesc.tagDoor = pObj_desc->tagDoor;
	m_tagPropDesc.tagCabinet = pObj_desc->tagCabinet;
	m_tagPropDesc.tagBigStatue = pObj_desc->tagBigStatue;
	m_tagPropDesc.tagHallStatue = pObj_desc->tagHallStatue;
	m_tagPropDesc.tagShutter = pObj_desc->tagShutter;
	m_tagPropDesc.tagStatue = pObj_desc->tagStatue;
	m_tagPropDesc.tagWindow = pObj_desc->tagWindow;
	m_tagPropDesc.tagItemDesc = pObj_desc->tagItemDesc;


	for (auto iter : m_tagPropDesc.BelongIndexs)
	{
		m_tagPropDesc.BelongIndexs2[iter] = true;
	}
	pObj_desc->fSpeedPerSec = 1.f;
	pObj_desc->fRotationPerSec = XMConvertToRadians(1.f);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	//파트 오브젝이나 컴포넌트는 커스텀
	m_pTransformCom->Set_WorldMatrix(m_tagPropDesc.worldMatrix);

	//

	return S_OK;
}

void CInteractProps::Priority_Tick(_float fTimeDelta)
{
	Priority_Tick_PartObjects(fTimeDelta);
}

void CInteractProps::Tick(_float fTimeDelta)
{
	Tick_PartObjects(fTimeDelta);
}

void CInteractProps::Late_Tick(_float fTimeDelta)
{
	Late_Tick_PartObjects(fTimeDelta);
}

HRESULT CInteractProps::Render()
{
	return S_OK;
}

void CInteractProps::Priority_Tick_PartObjects(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
	{
		if (pPartObject == nullptr)
			continue;
		pPartObject->Priority_Tick(fTimeDelta);
	}
}

void CInteractProps::Tick_PartObjects(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
	{
		if (pPartObject == nullptr)
			continue;
		pPartObject->Tick(fTimeDelta);
	}
}


void CInteractProps::Late_Tick_PartObjects(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
	{
		if (pPartObject == nullptr)
			continue;
		pPartObject->Late_Tick(fTimeDelta);
	}
}

void CInteractProps::Check_Player()
{
	if (m_pPlayer != nullptr)
		return;
	m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Player"))->front());
	m_pPlayerInteract = m_pPlayer->Get_Player_Interact_Ptr();
	m_pPlayerTransform = static_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));
	for (auto& iter: m_PartObjects)
	{
		if (iter == nullptr)
			continue;
		static_cast<CPart_InteractProps*>(iter)->Set_PlayerSetting(m_pPlayer, m_pPlayerInteract, m_pPlayerTransform);
	}

}

void CInteractProps::Check_Col_Sphere_Player()
{
	if (m_pPlayer == nullptr)
		return;
	if (m_pColliderCom[INTERACTPROPS_COL_SPHERE] == nullptr)
		return;
	CCollider* pPlayerCol = static_cast<CCollider*>( m_pPlayer->Get_Component(TEXT("Com_Collider")));
	if (pPlayerCol->Intersect(m_pColliderCom[INTERACTPROPS_COL_SPHERE]))
		m_bCol = true; 

}

_bool CInteractProps::Visible()
{
	if (m_pPlayer == nullptr)
		return false;

	if (m_pPlayer->Get_Player_RegionChange() == true)
	{
		if (m_tagPropDesc.iRegionDir == DIRECTION_MID)
		{
			;
		}
		else if (m_pPlayer->Get_Player_Direction() != m_tagPropDesc.iRegionDir)
		{
			return m_bVisible = false;
		}

		m_bVisible = m_tagPropDesc.BelongIndexs2[m_pPlayer->Get_Player_ColIndex()];
	}

	return m_bVisible;
}

HRESULT CInteractProps::Render_LightDepth_Dir()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
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

			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

			/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
			if (FAILED(m_pShaderCom->Begin(2)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	return S_OK;
}

HRESULT CInteractProps::Render_LightDepth_Spot()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
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

			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(4)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}

		++iIndex;
	}

	return S_OK;
}

HRESULT CInteractProps::Render_LightDepth_Point()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
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

			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(2)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	return S_OK;
}

_bool* CInteractProps::ComeClose_toPlayer(_float _come)
{
	if (nullptr == m_pPlayer)
		return nullptr;
	
	m_isNYResult = false;

	/* Player와의 거리 */
	_vector vDistanceVector = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION) - m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);
	_float fPlayer_Distance = XMVectorGetX(XMVector3Length(vDistanceVector));

	if (fPlayer_Distance <= _come)
		m_isNYResult = true;

	else
		m_isNYResult = false;

	return &m_isNYResult;
}

HRESULT CInteractProps::Add_Components()
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

HRESULT CInteractProps::Add_PartObjects()
{
	return S_OK;
}

HRESULT CInteractProps::Initialize_PartObjects()
{
	return S_OK;
}

HRESULT CInteractProps::Bind_ShaderResources()
{
	return S_OK;
}

void CInteractProps::Free()
{
	__super::Free();


	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	for (size_t i = 0; i < INTERACTPROPS_COL_END; i++)
	{
		if (m_pColliderCom[i] == nullptr)
			continue;


		Safe_Release(m_pColliderCom[i]);
		m_pColliderCom[i] = nullptr;
	}
	
	for (auto& pPartObject : m_PartObjects)
		Safe_Release(pPartObject);
	m_PartObjects.clear();
}
