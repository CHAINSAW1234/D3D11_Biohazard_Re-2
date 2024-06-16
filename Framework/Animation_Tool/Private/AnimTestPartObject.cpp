#include "stdafx.h"
#include "AnimTestPartObject.h"

#include "Light.h"

CAnimTestPartObject::CAnimTestPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CAnimTestPartObject::CAnimTestPartObject(const CAnimTestPartObject& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CAnimTestPartObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAnimTestPartObject::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	ANIMTESTPART_DESC* pDesc = { static_cast<ANIMTESTPART_DESC*>(pArg) };

	if (nullptr == pDesc->pRootTranslation)
		return E_FAIL;

	m_pRootTranslation = pDesc->pRootTranslation;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CAnimTestPartObject::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CAnimTestPartObject::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CAnimTestPartObject::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
}

HRESULT CAnimTestPartObject::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	list<_uint>			NonHideMeshIndices = { m_pModelCom->Get_NonHideMeshIndices() };
	for(_uint i : NonHideMeshIndices)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_NormalTexture", static_cast<_uint>(i), aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
			return E_FAIL;

	/*	if (FAILED(m_pModelCom->Bind_PrevBoneMatrices(m_pShaderCom, "g_PrevBoneMatrices", static_cast<_uint>(i))))
			return E_FAIL;*/

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_ATOSTexture", static_cast<_uint>(i), aiTextureType_METALNESS))) {
			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;
		}
		else {
			if (FAILED(m_pShaderCom->Begin(1)))
				return E_FAIL;
		}

		m_pModelCom->Render(static_cast<_uint>(i));
	}
	return S_OK;
}

HRESULT CAnimTestPartObject::Render_LightDepth_Dir()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (m_pGameInstance->Get_ShadowLight(CPipeLine::DIRECTION) != nullptr) {

		const CLight*				pLight = { m_pGameInstance->Get_ShadowLight(CPipeLine::DIRECTION) };
		const LIGHT_DESC*			pDesc = { pLight->Get_LightDesc(0) };

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pDesc->ViewMatrix[0])))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pDesc->ProjMatrix)))
			return E_FAIL;

		list<_uint>			NonHideMeshIndices = { m_pModelCom->Get_NonHideMeshIndices() };
		for (_uint i : NonHideMeshIndices)
		{
			if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

			/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
			if (FAILED(m_pShaderCom->Begin(3)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(static_cast<_uint>(i))))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CAnimTestPartObject::Render_LightDepth_Spot()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (m_pGameInstance->Get_ShadowLight(CPipeLine::SPOT) != nullptr) {

		const CLight* pLight = { m_pGameInstance->Get_ShadowLight(CPipeLine::SPOT) };
		const LIGHT_DESC* pDesc = { pLight->Get_LightDesc(0) };

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pDesc->ViewMatrix[0])))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pDesc->ProjMatrix)))
			return E_FAIL;
		list<_uint>			NonHideMeshIndices = { m_pModelCom->Get_NonHideMeshIndices() };
		for (_uint i : NonHideMeshIndices)
		{
			if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(3)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(static_cast<_uint>(i))))
				return E_FAIL;
		}
	}

	Update_WorldMatrix();

	return S_OK;
}

HRESULT CAnimTestPartObject::Render_LightDepth_Point()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	list<LIGHT_DESC*>			LightDescList = { m_pGameInstance->Get_ShadowPointLightDesc_List() };
	_int						iIndex = { 0 };

	for (auto& pLightDesc : LightDescList) {
		const _float4x4*		pLightViewMatrices;
		_float4x4				LightProjMatrix;
		pLightViewMatrices = pLightDesc->ViewMatrix;
		LightProjMatrix = pLightDesc->ProjMatrix;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_LightIndex", &iIndex, sizeof(_int))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrices("g_LightViewMatrix", pLightViewMatrices, 6)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_LightProjMatrix", &LightProjMatrix)))
			return E_FAIL;

		list<_uint>			NonHideMeshIndices = { m_pModelCom->Get_NonHideMeshIndices() };
		for (_uint i : NonHideMeshIndices)
		{
			if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(5)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(static_cast<_uint>(i))))
				return E_FAIL;
		}

		++iIndex;
	}

	return S_OK;
}

HRESULT CAnimTestPartObject::Chanage_Componenet(CComponent* pComponent, COMPONENT_TYPE eType)
{
	if (nullptr == pComponent)
		return E_FAIL;

	if (COMPONENT_TYPE::COM_MODEL == eType)
	{
		CModel* pModel = { dynamic_cast<CModel*>(pComponent) };
		if (nullptr == pModel)
			return E_FAIL;

		Safe_Release(m_pModelCom);
		m_pModelCom = nullptr;

		m_pModelCom = pModel;
		Safe_AddRef(m_pModelCom);
	}

	return S_OK;
}

void CAnimTestPartObject::Set_RootBone(string strRootBoneTag)
{
	if (nullptr == m_pModelCom)
		return;

	m_pModelCom->Set_RootBone(strRootBoneTag);
}

HRESULT CAnimTestPartObject::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_TOOL, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimTestPartObject::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	/*_bool isMotionBlur = m_pGameInstance->Get_ShaderState(MOTION_BLUR);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isMotionBlur", &isMotionBlur, sizeof(_bool))))
		return E_FAIL;*/

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", &m_PrevWorldMatrix)))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevViewMatrix", &m_pGameInstance->Get_PrevTransform_Float4x4(CPipeLine::D3DTS_VIEW))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevProjMatrix", &m_pGameInstance->Get_PrevTransform_Float4x4(CPipeLine::D3DTS_PROJ))))
	//	return E_FAIL;

	return S_OK;
}

CAnimTestPartObject* CAnimTestPartObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAnimTestPartObject* pInstance = { new CAnimTestPartObject(pDevice, pContext) };

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimTestPartObject"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAnimTestPartObject::Clone(void* pArg)
{
	CAnimTestPartObject* pInstance = { new CAnimTestPartObject(*this) };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimTestPartObject"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimTestPartObject::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
