#include "stdafx.h"
#include "Face_Zombie.h"

#include "Light.h"

CFace_Zombie::CFace_Zombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CFace_Zombie::CFace_Zombie(const CFace_Zombie& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CFace_Zombie::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFace_Zombie::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	FACE_MONSTER_DESC*			pDesc = { static_cast<FACE_MONSTER_DESC*>(pArg) };
	m_eBodyType = pDesc->eBodyType;
	m_iFaceModelID = pDesc->iFaceModelID;

	if (ZOMBIE_BODY_TYPE::_END == m_eBodyType ||
		-1 == m_iFaceModelID)
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
	m_bCloth = false;


	return S_OK;
}

void CFace_Zombie::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CFace_Zombie::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	auto vPos = m_pParentsTransform->Get_State_Vector(CTransform::STATE_POSITION);
	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + CONTROLLER_GROUND_GAP_ZOMBIE);
	if (!m_pGameInstance->isInFrustum_WorldSpace(vPos, 1.f))
	{
		m_bRender = false;
	}
	else
	{
		m_bRender = true;
	}
}

void CFace_Zombie::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	_float3			vTempTranslation = {};
	m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, &vTempTranslation);
	//	m_pModelCom->Change_Animation(0, TEXT("Default"), 0);
	//	m_pModelCom->Play_Animation_Light(m_pParentsTransform, fTimeDelta);
	//	m_pModelCom->Play_Pose(m_pParentsTransform, fTimeDelta);

	if(m_bRender)
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
	}
}


HRESULT CFace_Zombie::Render()
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

		//	if (FAILED(m_pModelCom->Bind_PrevBoneMatrices(m_pShaderCom, "g_PrevBoneMatrices", static_cast<_uint>(i))))
		//		return E_FAIL;

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

		if (m_pModelCom->Get_Mesh_Branch(i) != (_int)CFace_Zombie::FACE_MESH_TYPE::_INNER)
		{
			m_bDecalRender = true;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_DecalRender", &m_bDecalRender, sizeof(_bool))))
				return E_FAIL;

			m_pModelCom->Bind_DecalMap(static_cast<_uint>(i), m_pShaderCom);
		}
		else
		{
			m_bDecalRender = false;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_DecalRender", &m_bDecalRender, sizeof(_bool))))
				return E_FAIL;
		}
		
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;
		m_pModelCom->Render(static_cast<_uint>(i));
	}

	return S_OK;
}

HRESULT CFace_Zombie::Render_LightDepth_Dir()
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

HRESULT CFace_Zombie::Render_LightDepth_Point()
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

HRESULT CFace_Zombie::Render_LightDepth_Spot()
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

	Update_WorldMatrix();

	return S_OK;
}

void CFace_Zombie::Add_RenderGroup()
{
	if(m_bRender)
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
	}
}

HRESULT CFace_Zombie::Initialize_Model()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	m_pModelCom->Set_RootBone(m_pModelCom->Get_BoneNames().front());
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));
	m_pModelCom->Add_AnimPlayingInfo(true, 0, TEXT("Default"), 1.f);

	m_pModelCom->Active_RootMotion_XZ(false);
	m_pModelCom->Active_RootMotion_Y(false);
	m_pModelCom->Active_RootMotion_Rotation(false);

	if (FAILED(Initialize_MeshType()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFace_Zombie::Initialize_MeshType()
{
	_uint						iNumMesh = { m_pModelCom->Get_NumMeshes() };
	vector<string>				MeshTags = { m_pModelCom->Get_MeshTags() };

	for (auto& strMeshTag : MeshTags)
	{
		if (strMeshTag.find("Inside") != string::npos ||
			strMeshTag.find("inside") != string::npos)
		{
			m_pModelCom->Set_Mesh_Branch(strMeshTag, static_cast<_uint>(FACE_MESH_TYPE::_INNER));
		}

		else if (strMeshTag.find("Face") != string::npos ||
			strMeshTag.find("face") != string::npos)
		{
			m_pModelCom->Set_Mesh_Branch(strMeshTag, static_cast<_uint>(FACE_MESH_TYPE::_OUTTER));
		}

		else if (strMeshTag.find("Hair") != string::npos ||
			strMeshTag.find("hair") != string::npos)
		{
			m_pModelCom->Set_Mesh_Branch(strMeshTag, static_cast<_uint>(FACE_MESH_TYPE::_HAIR));
		}

		else if (strMeshTag.find("Teeth") != string::npos ||
			strMeshTag.find("teeth") != string::npos)
		{
			m_pModelCom->Set_Mesh_Branch(strMeshTag, static_cast<_uint>(FACE_MESH_TYPE::_TEETH));
		}
	}

	m_pModelCom->Hide_Mesh_Branch(static_cast<_uint>(FACE_MESH_TYPE::_INNER), false);
	m_pModelCom->Hide_Mesh_Branch(static_cast<_uint>(FACE_MESH_TYPE::_OUTTER), true);
	m_pModelCom->Hide_Mesh_Branch(static_cast<_uint>(FACE_MESH_TYPE::_HAIR), true);
	m_pModelCom->Hide_Mesh_Branch(static_cast<_uint>(FACE_MESH_TYPE::_TEETH), true);

	return S_OK;
}

HRESULT CFace_Zombie::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */

	if (ZOMBIE_BODY_TYPE::_MALE == m_eBodyType)
	{
		if (ZOMBIE_MALE_FACE::_00 == static_cast<ZOMBIE_MALE_FACE>(m_iFaceModelID))
		{
			if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Face00_Male"),
				TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}

		else if (ZOMBIE_MALE_FACE::_01 == static_cast<ZOMBIE_MALE_FACE>(m_iFaceModelID))
		{
			if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Face01_Male"),
				TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}

		else if (ZOMBIE_MALE_FACE::_02 == static_cast<ZOMBIE_MALE_FACE>(m_iFaceModelID))
		{
			if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Face02_Male"),
				TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}

		else if (ZOMBIE_MALE_FACE::_03 == static_cast<ZOMBIE_MALE_FACE>(m_iFaceModelID))
		{
			if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Face03_Male"),
				TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}

		else if (ZOMBIE_MALE_FACE::_04 == static_cast<ZOMBIE_MALE_FACE>(m_iFaceModelID))
		{
			if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Face04_Male"),
				TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}

		else if (ZOMBIE_MALE_FACE::_05 == static_cast<ZOMBIE_MALE_FACE>(m_iFaceModelID))
		{
			if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Face05_Male"),
				TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}

		else if (ZOMBIE_MALE_FACE::_06 == static_cast<ZOMBIE_MALE_FACE>(m_iFaceModelID))
		{
			if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Face06_Male"),
				TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}

		else if (ZOMBIE_MALE_FACE::_07 == static_cast<ZOMBIE_MALE_FACE>(m_iFaceModelID))
		{
			if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Face07_Male"),
				TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}

		else if (ZOMBIE_MALE_FACE::_08 == static_cast<ZOMBIE_MALE_FACE>(m_iFaceModelID))
		{
			if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Face08_Male"),
				TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}

		else if (ZOMBIE_MALE_FACE::_09 == static_cast<ZOMBIE_MALE_FACE>(m_iFaceModelID))
		{
			if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Face09_Male"),
				TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}

		else if (ZOMBIE_MALE_FACE::_10 == static_cast<ZOMBIE_MALE_FACE>(m_iFaceModelID))
		{
			if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Face10_Male"),
				TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}
	}

	else if (ZOMBIE_BODY_TYPE::_FEMALE == m_eBodyType)
	{
		if (ZOMBIE_FEMALE_FACE::_00 == static_cast<ZOMBIE_FEMALE_FACE>(m_iFaceModelID))
		{
			if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Face00_Female"),
				TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}

		else if (ZOMBIE_FEMALE_FACE::_01 == static_cast<ZOMBIE_FEMALE_FACE>(m_iFaceModelID))
		{
			if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Face01_Female"),
				TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}

		else if (ZOMBIE_FEMALE_FACE::_02 == static_cast<ZOMBIE_FEMALE_FACE>(m_iFaceModelID))
		{
			if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Face02_Female"),
				TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}

		else if (ZOMBIE_FEMALE_FACE::_03 == static_cast<ZOMBIE_FEMALE_FACE>(m_iFaceModelID))
		{
			if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Face03_Female"),
				TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}

		else if (ZOMBIE_FEMALE_FACE::_04 == static_cast<ZOMBIE_FEMALE_FACE>(m_iFaceModelID))
		{
			if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Face04_Female"),
				TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}
	}

	else if (ZOMBIE_BODY_TYPE::_MALE_BIG == m_eBodyType)
	{
		if (ZOMBIE_MALE_BIG_FACE::_00 == static_cast<ZOMBIE_MALE_BIG_FACE>(m_iFaceModelID))
		{
			if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Face00_Male_Big"),
				TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}

		else if (ZOMBIE_MALE_BIG_FACE::_01 == static_cast<ZOMBIE_MALE_BIG_FACE>(m_iFaceModelID))
		{
			if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Face01_Male_Big"),
				TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}

		else if (ZOMBIE_MALE_BIG_FACE::_03 == static_cast<ZOMBIE_MALE_BIG_FACE>(m_iFaceModelID))
		{
			if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Face03_Male_Big"),
				TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}

		else if (ZOMBIE_MALE_BIG_FACE::_04 == static_cast<ZOMBIE_MALE_BIG_FACE>(m_iFaceModelID))
		{
			if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Face04_Male_Big"),
				TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}

		else if (ZOMBIE_MALE_BIG_FACE::_70 == static_cast<ZOMBIE_MALE_BIG_FACE>(m_iFaceModelID))
		{
			if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Face70_Male_Big"),
				TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CFace_Zombie::Bind_ShaderResources()
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
		if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			return E_FAIL;
	}

	_bool isMotionBlur = m_pGameInstance->Get_ShaderState(MOTION_BLUR);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isMotionBlur", &isMotionBlur, sizeof(_bool))))
		return E_FAIL;

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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Cloth", &m_bCloth, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

CFace_Zombie* CFace_Zombie::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFace_Zombie* pInstance = { new CFace_Zombie(pDevice, pContext) };

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CFace_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFace_Zombie::Clone(void* pArg)
{
	CFace_Zombie* pInstance = { new CFace_Zombie(*this) };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CFace_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFace_Zombie::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
