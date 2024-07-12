#include "stdafx.h"
#include "Throwing_Weapon.h"
#include "Light.h"
#include "Rigid_Dynamic.h"

CThrowing_Weapon::CThrowing_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject { pDevice, pContext }
{
}

CThrowing_Weapon::CThrowing_Weapon(const CThrowing_Weapon& rhs)
    : CGameObject { rhs }
{
}

HRESULT CThrowing_Weapon::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CThrowing_Weapon::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	THROWING_WEAPON_DESC* pDesc = (THROWING_WEAPON_DESC*)pArg;
	m_eEquip = pDesc->eEquip;


    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

	m_pTransformCom->Set_WorldMatrix(pDesc->worldMatrix);
	m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);

	switch (m_eEquip) {
	case CPlayer::GRENADE:
		m_pModelCom->Hide_Mesh("LOD_1_Group_1_Sub_1__wp6200_Grenade_Mat_mesh0001", true);
		break;
	case CPlayer::FLASHBANG:
		m_pModelCom->Hide_Mesh("LOD_1_Group_1_Sub_1__WP6300_Flash_Mat_mesh0001", true);
		break;
	default:
		return E_FAIL;
	}


	m_pModelCom->Set_RootBone("root");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));
	m_pModelCom->Add_AnimPlayingInfo(false, 0, TEXT("Default"), 1.f);
	m_pModelCom->Change_Animation(0, TEXT("Default"), 0);

	m_pRigid_Dynamic = m_pGameInstance->Create_Rigid_Dynamic(m_pModelCom, m_pTransformCom, &m_iIndex_RigidBody, this);
	m_pRigid_Dynamic->SetKinematic(true);

	m_pModelCom->Release_Decal_Dump();

	Initiate(m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION), 
		m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK), 
		m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK));


    return S_OK;
}

void CThrowing_Weapon::Tick(_float fTimeDelta)
{
	if (m_pRigid_Dynamic)
	{
		m_pTransformCom->Set_WorldMatrix(m_pRigid_Dynamic->GetWorldMatrix_Rigid_Dynamic(m_pTransformCom->Get_Scaled()));

		m_pTransformCom->Set_Scaled(0.01f, 0.01f, 0.01f);
	}
}

void CThrowing_Weapon::Late_Tick(_float fTimeDelta)
{
	_float3				vDirection = { };
	m_pModelCom->Play_Animations(m_pTransformCom, fTimeDelta, &vDirection);

    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
}

HRESULT CThrowing_Weapon::Render()
{
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

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_EmissiveTexture", static_cast<_uint>(i), aiTextureType_EMISSIVE)))
		{
			_bool isEmissive = false;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isEmissiveTexture", &isEmissive, sizeof(_bool))))
				return E_FAIL;
		}
		else
		{
			_bool isEmissive = true;
			if (FAILED(m_pShaderCom->Bind_RawValue("g_isEmissiveTexture", &isEmissive, sizeof(_bool))))
				return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_DEFAULT)))
			return E_FAIL;

		m_pModelCom->Render(static_cast<_uint>(i));
	}

	return S_OK;
}

HRESULT CThrowing_Weapon::Render_LightDepth_Dir()
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

		list<_uint>			NonHideIndices = { m_pModelCom->Get_NonHideMeshIndices() };
		for (auto& i : NonHideIndices)
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_LIGHTDEPTH)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	return S_OK;
}
HRESULT CThrowing_Weapon::Render_LightDepth_Point()
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

		list<_uint>			NonHideIndices = { m_pModelCom->Get_NonHideMeshIndices() };
		for (auto& i : NonHideIndices)
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_LIGHTDEPTH_CUBE)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}

		++iIndex;
	}

	return S_OK;
}

HRESULT CThrowing_Weapon::Render_LightDepth_Spot()
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

		list<_uint>			NonHideIndices = { m_pModelCom->Get_NonHideMeshIndices() };
		for (auto& i : NonHideIndices)
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_LIGHTDEPTH)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	return S_OK;
}

void CThrowing_Weapon::Initiate(_float4 vPos, _float4 vDir, _float4 vLook)
{
	m_bRender = true;

	m_pRigid_Dynamic->SetPosition(vPos);

	m_vDir = _float4(vDir.x /*+ m_pGameInstance->GetRandom_Real(-0.03f, 0.03f)*/,
		vDir.y,
		vDir.z/* + m_pGameInstance->GetRandom_Real(-0.03f, 0.03f)*/,
		0.f);

	m_vDir = Float4_Normalize(m_vDir);
	m_vDir.y += m_pGameInstance->GetRandom_Real(0.05f, 0.1f);

	m_pRigid_Dynamic->SetKinematic(false);
	m_pRigid_Dynamic->AddForce(m_vDir);
}

HRESULT CThrowing_Weapon::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	wstring strModelTag;
	switch (m_eEquip) {
	case CPlayer::GRENADE:
		strModelTag = TEXT("Prototype_Component_Model_Grenade");
		break;
	case CPlayer::FLASHBANG:
		strModelTag = TEXT("Prototype_Component_Model_FlashBang");
		break;
	default:
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(g_Level, strModelTag,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

    return S_OK;
}

HRESULT CThrowing_Weapon::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if(FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DecalRender", &m_bDecalRender, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

CThrowing_Weapon* CThrowing_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CThrowing_Weapon* pInstance = new CThrowing_Weapon(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CThrowing_Weapon"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CThrowing_Weapon::Clone(void* pArg)
{
    CThrowing_Weapon* pInstance = new CThrowing_Weapon(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CThrowing_Weapon"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CThrowing_Weapon::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
