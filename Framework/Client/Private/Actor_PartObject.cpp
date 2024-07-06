#include "stdafx.h"
#include "Actor_PartObject.h"

#include "Light.h"

CActor_PartObject::CActor_PartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CActor_PartObject::CActor_PartObject(const CActor_PartObject& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CActor_PartObject::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	ACTOR_PART_DESC*				pDesc = { static_cast<ACTOR_PART_DESC*>(pArg) };
	m_pRootTranslation = pDesc->pRootTranslation;
	m_isBaseObject = pDesc->isBaseObject;
	if(false == pDesc->AnimPrototypeLayerTags.empty())
		m_strAnimLayerTag = pDesc->AnimPrototypeLayerTags.front();

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components(pDesc->strModelPrototypeTag)))
		return E_FAIL;

	if (FAILED(Add_Animations(pDesc->AnimPrototypeLayerTags)))
		return E_FAIL;

	m_pModelCom->Set_RootBone(m_pModelCom->Get_BoneNames().front());
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));
	m_pModelCom->Add_AnimPlayingInfo(true, 0, TEXT("Default"), 1.f);
	m_pModelCom->Set_OptimizationCulling(false);
	m_pModelCom->Set_Loop(0, false);
	m_pModelCom->Change_Animation(0, m_strAnimLayerTag, 0);

	return S_OK;
}

void CActor_PartObject::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CActor_PartObject::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CActor_PartObject::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (true == m_isBaseObject)
	{
		m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, m_pRootTranslation);
		//	if (m_strAnimLayerTag != TEXT(""))
		//		m_pModelCom->Play_Animation_Light(m_pParentsTransform, fTimeDelta);
	}
	else
	{
		_float3				vTempDirection = {};
		m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, &vTempDirection);

		//if(m_strAnimLayerTag != TEXT(""))
		//	m_pModelCom->Play_Animation_Light(m_pParentsTransform, fTimeDelta);
	}
	

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
}

HRESULT CActor_PartObject::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	list<_uint>				NonHideMeshIndices = { m_pModelCom->Get_NonHideMeshIndices() };
	for (_uint i : NonHideMeshIndices)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_NormalTexture", static_cast<_uint>(i), aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
			return E_FAIL;

		_bool				isDecalRender = { false };
		if (FAILED(m_pShaderCom->Bind_RawValue("g_DecalRender", &isDecalRender, sizeof(_bool))))
			return E_FAIL;

	/*	if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_ATOSTexture", static_cast<_uint>(i), aiTextureType_METALNESS))) {
			if (FAILED(m_pShaderCom->Begin(0)))
				return E_FAIL;
		}
		else {
			if (FAILED(m_pShaderCom->Begin(1)))
				return E_FAIL;
		}*/

		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(static_cast<_uint>(i));
	}
	return S_OK;
}

void CActor_PartObject::Set_Animation(_uint iPlayingIndex, const wstring& strAnimLayerTag, _uint iAnimIndex)
{
	if (nullptr == m_pModelCom)
		return;

	m_pModelCom->Change_Animation(iPlayingIndex, strAnimLayerTag, iAnimIndex);
}

void CActor_PartObject::Set_Animation_Index(_uint iPlayingIndex, _uint iAnimIndex)
{
	if (nullptr == m_pModelCom)
		return;

	wstring				strAnimLayerTag = { m_pModelCom->Get_CurrentAnimLayerTag(iPlayingIndex) };
	m_pModelCom->Change_Animation(iPlayingIndex, strAnimLayerTag, iAnimIndex);
}

void CActor_PartObject::Set_Loop(_uint iPlayingIndex, _bool isLoop)
{
	if (nullptr == m_pModelCom)
		return;

	m_pModelCom->Set_Loop(iPlayingIndex, isLoop);
}

HRESULT CActor_PartObject::Render_LightDepth_Dir()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (m_pGameInstance->Get_ShadowLight(CPipeLine::DIRECTION) != nullptr) {

		const CLight* pLight = { m_pGameInstance->Get_ShadowLight(CPipeLine::DIRECTION) };
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

			/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
			if (FAILED(m_pShaderCom->Begin(3)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(static_cast<_uint>(i))))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CActor_PartObject::Render_LightDepth_Spot()
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

HRESULT CActor_PartObject::Render_LightDepth_Point()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	list<LIGHT_DESC*>			LightDescList = { m_pGameInstance->Get_ShadowPointLightDesc_List() };
	_int						iIndex = { 0 };

	for (auto& pLightDesc : LightDescList) {
		const _float4x4* pLightViewMatrices;
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

HRESULT CActor_PartObject::Add_Components(const wstring& strModelPrototypeTag)
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(g_Level, strModelPrototypeTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CActor_PartObject::Add_Animations(const list<wstring>& AnimPrototypeLayerTags)
{
	for (auto& strAnimPrototypeLayerTag : AnimPrototypeLayerTags)
	{
		if (FAILED(m_pModelCom->Add_Animations(strAnimPrototypeLayerTag, strAnimPrototypeLayerTag)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CActor_PartObject::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CActor_PartObject* CActor_PartObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CActor_PartObject* pInstance = { new CActor_PartObject(pDevice, pContext) };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CActor_PartObject"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CActor_PartObject::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
