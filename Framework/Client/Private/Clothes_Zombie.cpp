#include "stdafx.h"
#include "Clothes_Zombie.h"
#include "Zombie.h"

#include "Light.h"
#include "Part_Breaker_Zombie.h"
#include "RagDoll_Physics.h"

CClothes_Zombie::CClothes_Zombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CClothes_Zombie::CClothes_Zombie(const CClothes_Zombie& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CClothes_Zombie::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CClothes_Zombie::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	CLOTHES_MONSTER_DESC* pDesc = { static_cast<CLOTHES_MONSTER_DESC*>(pArg) };
	m_pRender = pDesc->pRender;
	m_eClothesType = pDesc->eClothesType;
	m_eBodyType = pDesc->eBodyType;
	m_iClothesModelID = pDesc->iClothesModelID;
	m_ppPart_Breaker = pDesc->ppPart_Breaker;

	if (ZOMBIE_CLOTHES_TYPE::_END == m_eClothesType ||
		ZOMBIE_BODY_TYPE::_END == m_eBodyType ||
		-1 == m_iClothesModelID)
		return E_FAIL;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Initialize_Model()))
		return E_FAIL;

#pragma region Effect
	m_pModelCom->Init_Decal(LEVEL_GAMEPLAY);
#pragma endregion

	m_bDecalRender = true;
	m_bCloth = true;
	m_bDecal_Player = false;

	return S_OK;
}

void CClothes_Zombie::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CClothes_Zombie::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	/*auto vPos = m_pParentsTransform->Get_State_Vector(CTransform::STATE_POSITION);
	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + CONTROLLER_GROUND_GAP_ZOMBIE);
	if (!m_pGameInstance->isInFrustum_WorldSpace(vPos, 1.f))
	{
		m_bRender = false;
	}
	else
	{
		m_bRender = true;
	}*/

	m_bRender = true;
}

void CClothes_Zombie::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	_float3			vTempTranslation = {};
	if(true == *m_pRender)
		m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, &vTempTranslation);
	//	m_pModelCom->Play_Animation_Light(m_pParentsTransform, fTimeDelta);
	//	m_pModelCom->Play_Pose(m_pParentsTransform, fTimeDelta);

	if (true == *m_pRender &&
		true == m_bRender)
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
	}
}

HRESULT CClothes_Zombie::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	list<_uint>			NonHideIndices = { m_pModelCom->Get_NonHideMeshIndices() };
	for (auto& i : NonHideIndices)
	{
		if (FAILED(Bind_WorldMatrix(i)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_NormalTexture", static_cast<_uint>(i), aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
			return E_FAIL;

		if (ZOMBIE_CLOTHES_TYPE::_SHIRTS == m_eClothesType)
		{
			if ((*m_ppPart_Breaker)->Is_RagDoll_Mesh_Shirt(i))
			{
				if (FAILED(m_pModelCom->Bind_BoneMatrices_Ragdoll(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i), m_pRagdoll->GetBoneMatrices_Ragdoll())))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
					return E_FAIL;
			}
		}
		
		else
		{
			if ((*m_ppPart_Breaker)->Is_RagDoll_Mesh_Pants(i))
			{
				if (FAILED(m_pModelCom->Bind_BoneMatrices_Ragdoll(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i), m_pRagdoll->GetBoneMatrices_Ragdoll())))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
					return E_FAIL;
			}
		}

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

		m_pModelCom->Bind_DecalMap(static_cast<_uint>(i), m_pShaderCom);
		if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_NONCULL)))
			return E_FAIL;
		m_pModelCom->Render(static_cast<_uint>(i));
	}
	return S_OK;
}

HRESULT CClothes_Zombie::Render_LightDepth_Dir()
{
	if (nullptr == m_pShaderCom)
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
			if (FAILED(Bind_WorldMatrix(i)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
				return E_FAIL;

			if ((*m_ppPart_Breaker)->Is_RagDoll_Mesh_Shirt(i))
			{
				if (FAILED(m_pModelCom->Bind_BoneMatrices_Ragdoll(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i), m_pRagdoll->GetBoneMatrices_Ragdoll())))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
					return E_FAIL;
			}

			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;

			/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
			if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_LIGHTDEPTH)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	return S_OK;
}

HRESULT CClothes_Zombie::Render_LightDepth_Point()
{
	if (nullptr == m_pShaderCom)
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
			if (FAILED(Bind_WorldMatrix(i)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
				return E_FAIL;

			if ((*m_ppPart_Breaker)->Is_RagDoll_Mesh_Shirt(i))
			{
				if (FAILED(m_pModelCom->Bind_BoneMatrices_Ragdoll(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i), m_pRagdoll->GetBoneMatrices_Ragdoll())))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
					return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_LIGHTDEPTH_CUBE)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}

		++iIndex;
	}

	return S_OK;
}

HRESULT CClothes_Zombie::Render_LightDepth_Spot()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (m_bRagdoll)
	{
		auto WorldMat = m_pParentsTransform->Get_WorldFloat4x4();
		WorldMat._41 = 0.f;
		WorldMat._42 = 0.f;
		WorldMat._43 = 0.f;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMat)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			return E_FAIL;
	}

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
			if (FAILED(Bind_WorldMatrix(i)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
				return E_FAIL;

			if ((*m_ppPart_Breaker)->Is_RagDoll_Mesh_Shirt(i))
			{
				if (FAILED(m_pModelCom->Bind_BoneMatrices_Ragdoll(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i), m_pRagdoll->GetBoneMatrices_Ragdoll())))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
					return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_LIGHTDEPTH)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	Update_WorldMatrix();

	return S_OK;
}

void CClothes_Zombie::Create_Cloth()
{
	m_pModelCom->Create_Cloth();
}

void CClothes_Zombie::Add_RenderGroup()
{
	if (m_bRender)
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
	}
}

HRESULT CClothes_Zombie::Initialize_Model()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	m_pModelCom->Set_RootBone(m_pModelCom->Get_BoneNames().front());
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));
	m_pModelCom->Add_AnimPlayingInfo(true, 0, TEXT("Default"), 1.f);

	m_pModelCom->Active_RootMotion_XZ(false);
	m_pModelCom->Active_RootMotion_Y(false);
	m_pModelCom->Active_RootMotion_Rotation(false);

	return S_OK;
}

void CClothes_Zombie::Set_RagDoll_Ptr(CRagdoll_Physics* pRagDoll)
{

	Safe_Release(m_pRagdoll);
	m_pRagdoll = pRagDoll;
	Safe_AddRef(m_pRagdoll);
}

HRESULT CClothes_Zombie::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (ZOMBIE_BODY_TYPE::_MALE == m_eBodyType)
	{
		if (ZOMBIE_CLOTHES_TYPE::_PANTS == m_eClothesType)
		{
			if (ZOMBIE_MALE_PANTS::_00 == static_cast<ZOMBIE_MALE_PANTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants00_Male"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_PANTS::_01 == static_cast<ZOMBIE_MALE_PANTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants01_Male"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_PANTS::_02 == static_cast<ZOMBIE_MALE_PANTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants02_Male"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_PANTS::_03 == static_cast<ZOMBIE_MALE_PANTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants03_Male"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_PANTS::_04 == static_cast<ZOMBIE_MALE_PANTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants04_Male"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_PANTS::_05 == static_cast<ZOMBIE_MALE_PANTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants05_Male"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_PANTS::_06 == static_cast<ZOMBIE_MALE_PANTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants06_Male"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else
				return E_FAIL;
		}

		else if (ZOMBIE_CLOTHES_TYPE::_SHIRTS == m_eClothesType)
		{
			if (ZOMBIE_MALE_SHIRTS::_00 == static_cast<ZOMBIE_MALE_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts00_Male"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_SHIRTS::_01 == static_cast<ZOMBIE_MALE_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts01_Male"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_SHIRTS::_02 == static_cast<ZOMBIE_MALE_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts02_Male"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_SHIRTS::_03 == static_cast<ZOMBIE_MALE_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts03_Male"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_SHIRTS::_04 == static_cast<ZOMBIE_MALE_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts04_Male"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_SHIRTS::_05 == static_cast<ZOMBIE_MALE_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts05_Male"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_SHIRTS::_06 == static_cast<ZOMBIE_MALE_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts06_Male"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_SHIRTS::_08 == static_cast<ZOMBIE_MALE_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts08_Male"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_SHIRTS::_09 == static_cast<ZOMBIE_MALE_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts09_Male"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_SHIRTS::_11 == static_cast<ZOMBIE_MALE_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts11_Male"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_SHIRTS::_12 == static_cast<ZOMBIE_MALE_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts12_Male"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_SHIRTS::_70 == static_cast<ZOMBIE_MALE_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts70_Male"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_SHIRTS::_72 == static_cast<ZOMBIE_MALE_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts72_Male"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_SHIRTS::_73 == static_cast<ZOMBIE_MALE_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts73_Male"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_SHIRTS::_81 == static_cast<ZOMBIE_MALE_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts81_Male"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else
				return E_FAIL;
		}
	}

	else if (ZOMBIE_BODY_TYPE::_FEMALE == m_eBodyType)
	{
		if (ZOMBIE_CLOTHES_TYPE::_PANTS == m_eClothesType)
		{
			if (ZOMBIE_FEMALE_PANTS::_00 == static_cast<ZOMBIE_FEMALE_PANTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants00_Female"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_FEMALE_PANTS::_01 == static_cast<ZOMBIE_FEMALE_PANTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants01_Female"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_FEMALE_PANTS::_02 == static_cast<ZOMBIE_FEMALE_PANTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants02_Female"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_FEMALE_PANTS::_04 == static_cast<ZOMBIE_FEMALE_PANTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants04_Female"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else
				return E_FAIL;
		}

		else if (ZOMBIE_CLOTHES_TYPE::_SHIRTS == m_eClothesType)
		{
			if (ZOMBIE_FEMALE_SHIRTS::_00 == static_cast<ZOMBIE_FEMALE_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts00_Female"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_FEMALE_SHIRTS::_01 == static_cast<ZOMBIE_FEMALE_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts01_Female"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_FEMALE_SHIRTS::_02 == static_cast<ZOMBIE_FEMALE_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts02_Female"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_FEMALE_SHIRTS::_03 == static_cast<ZOMBIE_FEMALE_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts03_Female"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_FEMALE_SHIRTS::_04 == static_cast<ZOMBIE_FEMALE_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts04_Female"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_FEMALE_SHIRTS::_05 == static_cast<ZOMBIE_FEMALE_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts05_Female"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else
				return E_FAIL;
		}
	}

	else if (ZOMBIE_BODY_TYPE::_MALE_BIG == m_eBodyType)
	{
		if (ZOMBIE_CLOTHES_TYPE::_PANTS == m_eClothesType)
		{
			if (ZOMBIE_MALE_BIG_PANTS::_00 == static_cast<ZOMBIE_MALE_BIG_PANTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants00_Male_Big"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_BIG_PANTS::_01 == static_cast<ZOMBIE_MALE_BIG_PANTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants01_Male_Big"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_BIG_PANTS::_02 == static_cast<ZOMBIE_MALE_BIG_PANTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants02_Male_Big"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_BIG_PANTS::_70 == static_cast<ZOMBIE_MALE_BIG_PANTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants70_Male_Big"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else
				return E_FAIL;
		}

		else if (ZOMBIE_CLOTHES_TYPE::_SHIRTS == m_eClothesType)
		{
			if (ZOMBIE_MALE_BIG_SHIRTS::_00 == static_cast<ZOMBIE_MALE_BIG_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts00_Male_Big"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_BIG_SHIRTS::_01 == static_cast<ZOMBIE_MALE_BIG_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts01_Male_Big"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_BIG_SHIRTS::_02 == static_cast<ZOMBIE_MALE_BIG_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts02_Male_Big"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_BIG_SHIRTS::_03 == static_cast<ZOMBIE_MALE_BIG_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts03_Male_Big"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else if (ZOMBIE_MALE_BIG_SHIRTS::_70 == static_cast<ZOMBIE_MALE_BIG_SHIRTS>(m_iClothesModelID))
			{
				if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts70_Male_Big"),
					TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
					return E_FAIL;
			}

			else
				return E_FAIL;
		}
	}

	else
		return E_FAIL;



	return S_OK;
}

HRESULT CClothes_Zombie::Bind_ShaderResources()
{
	/*if (m_bRagdoll)
	{
		auto WorldMat = m_pParentsTransform->Get_WorldFloat4x4();
		WorldMat._41 = 0.f;
		WorldMat._42 = 0.f;
		WorldMat._43 = 0.f;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMat)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			return E_FAIL;
	}*/

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", &m_PrevWorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevViewMatrix", &m_pGameInstance->Get_PrevTransform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevProjMatrix", &m_pGameInstance->Get_PrevTransform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DecalRender", &m_bDecalRender, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Cloth", &m_bCloth, sizeof(_bool))))
		return E_FAIL;

	auto bHair = false;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Hair", &bHair, sizeof(_bool))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_bPlayer", &m_bDecal_Player, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

HRESULT CClothes_Zombie::Bind_WorldMatrix(_uint iIndex)
{
	if (m_bRagdoll)
	{
		auto WorldMat = m_pParentsTransform->Get_WorldFloat4x4();
		WorldMat._41 = 0.f;
		WorldMat._42 = 0.f;
		WorldMat._43 = 0.f;
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMat)))
			return E_FAIL;
	}
	else
	{
		if (ZOMBIE_CLOTHES_TYPE::_PANTS == m_eClothesType)
		{
			if ((*m_ppPart_Breaker)->Is_RagDoll_Mesh_Pants(iIndex))
			{
				auto WorldMat = m_pParentsTransform->Get_WorldFloat4x4();
				WorldMat._41 = 0.f;
				WorldMat._42 = 0.f;
				WorldMat._43 = 0.f;
				if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMat)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
					return E_FAIL;
			}
		}
		
		else
		{
			if (ZOMBIE_CLOTHES_TYPE::_PANTS == m_eClothesType)
			{
				if ((*m_ppPart_Breaker)->Is_RagDoll_Mesh_Shirt(iIndex))
				{
					auto WorldMat = m_pParentsTransform->Get_WorldFloat4x4();
					WorldMat._41 = 0.f;
					WorldMat._42 = 0.f;
					WorldMat._43 = 0.f;
					if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMat)))
						return E_FAIL;
				}
				else
				{
					if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
						return E_FAIL;
				}
			}
		}
	}
	return S_OK;
}

CClothes_Zombie* CClothes_Zombie::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CClothes_Zombie* pInstance = { new CClothes_Zombie(pDevice, pContext) };

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CClothes_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CClothes_Zombie::Clone(void* pArg)
{
	CClothes_Zombie* pInstance = { new CClothes_Zombie(*this) };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CClothes_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CClothes_Zombie::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRagdoll);
}
