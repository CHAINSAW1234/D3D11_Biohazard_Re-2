#include "stdafx.h"
#include "Body_Zombie.h"

#include "Light.h"
#include "RagDoll_Physics.h"
#include "Zombie.h"

#include "Part_Breaker_Zombie.h"

CBody_Zombie::CBody_Zombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBody_Zombie::CBody_Zombie(const CBody_Zombie& rhs)
	: CPartObject{ rhs }
{
}

HRESULT CBody_Zombie::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Zombie::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	BODY_MONSTER_DESC* pDesc = { static_cast<BODY_MONSTER_DESC*>(pArg) };


	m_ppPart_Breaker = pDesc->ppPart_Breaker;
	m_pRender = pDesc->pRender;
	m_pRootTranslation = pDesc->pRootTranslation;
	m_eBodyModelType = pDesc->eBodyType;

	if (nullptr == m_ppPart_Breaker)
		return E_FAIL;

	if (nullptr == m_pRootTranslation)
		return E_FAIL;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Initialize_Model()))
		return E_FAIL;

	m_eCurrentMotionType = MOTION_TYPE::MOTION_A;

	if (m_eBodyModelType == ZOMBIE_BODY_TYPE::_MALE)
		m_pRagdoll = m_pGameInstance->Create_Ragdoll(m_pModelCom->GetBoneVector(), m_pParentsTransform,m_pDevice,m_pContext, "../Bin/Resources/Models/Zombie_Male/Body_Male.fbx");

	if (m_eBodyModelType == ZOMBIE_BODY_TYPE::_FEMALE)
		m_pRagdoll = m_pGameInstance->Create_Ragdoll(m_pModelCom->GetBoneVector(), m_pParentsTransform, m_pDevice, m_pContext, "../Bin/Resources/Models/Zombie_Female/Body_Female.fbx");

	if (m_eBodyModelType == ZOMBIE_BODY_TYPE::_MALE_BIG)
		m_pRagdoll = m_pGameInstance->Create_Ragdoll(m_pModelCom->GetBoneVector(), m_pParentsTransform, m_pDevice, m_pContext, "../Bin/Resources/Models/Zombie_Male_Big/Body_Male_Big.fbx");

#pragma region Effect
	m_pModelCom->Init_Decal(LEVEL_GAMEPLAY);
#pragma endregion

	m_bDecalRender = true;

	return S_OK;
}

void CBody_Zombie::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBody_Zombie::Tick(_float fTimeDelta)
{
	if (m_bRagdoll)
	{
		m_bRender = true;
	}

	__super::Tick(fTimeDelta);
}

void CBody_Zombie::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_bRagdoll == false &&
		true == *m_pRender)
	{
		_bool			isCulled = { false };
		if (FAILED(m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, m_pRootTranslation)))
			isCulled = true;

		if (true == m_isActiveIK && false == isCulled)
		{
			m_pModelCom->Play_IK(m_pParentsTransform, fTimeDelta);
		}
	}

	//	현재 모션이 A ~ F 타입인지 판단하고 저장 => 다음 틱에 태스크 노드에서 참조하여 모션을 결정할것이다.
	Update_Current_MotionType();

	if (true == *m_pRender &&
		true == m_bRender)
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
	}
}

HRESULT CBody_Zombie::Render()
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

		if ((*m_ppPart_Breaker)->Is_RagDoll_Mesh_Body(i))
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices_Ragdoll(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i), m_pRagdoll->GetBoneMatrices_Ragdoll())))
				return E_FAIL;
		}
		else
		{
			if (m_bRender == false && m_bRagdoll == false)
				continue;

			if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
				return E_FAIL;
		}

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

		auto iBranch = m_pModelCom->Get_Mesh_Branch(i);
		if (iBranch != (_int)CBody_Zombie::BODY_MESH_TYPE::_INNER && iBranch != (_int)CBody_Zombie::BODY_MESH_TYPE::_DAMAGED && iBranch != (_int)CBody_Zombie::BODY_MESH_TYPE::_DEFICIT
			&& iBranch != (_int)CBody_Zombie::BODY_MESH_TYPE::_BROKEN_HEAD)
		{
			m_bDecalRender = true;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_DecalRender", &m_bDecalRender, sizeof(_bool))))
				return E_FAIL;
			m_pModelCom->Bind_DecalMap(i, m_pShaderCom);
		}
		else
		{
			m_bDecalRender = false;

			if (FAILED(m_pShaderCom->Bind_RawValue("g_DecalRender", &m_bDecalRender, sizeof(_bool))))
				return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_DEFAULT)))
			return E_FAIL;

		m_pModelCom->Render(static_cast<_uint>(i));
	}

	return S_OK;
}

HRESULT CBody_Zombie::Render_LightDepth_Dir()
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
			auto iBranch = m_pModelCom->Get_Mesh_Branch(i);
			if (iBranch != (_int)CBody_Zombie::BODY_MESH_TYPE::_OUTTER)
				continue;

			if (FAILED(Bind_WorldMatrix(i)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
				return E_FAIL;

			if ((*m_ppPart_Breaker)->Is_RagDoll_Mesh_Body(i))
			{
				if (FAILED(m_pModelCom->Bind_BoneMatrices_Ragdoll(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i), m_pRagdoll->GetBoneMatrices_Ragdoll())))
					return E_FAIL;
			}
			else
			{
				if (m_bRender == false && m_bRagdoll == false)
					continue;

				if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
					return E_FAIL;
			}

			/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
			if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_LIGHTDEPTH)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	return S_OK;
}

HRESULT CBody_Zombie::Render_LightDepth_Point()
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
			auto iBranch = m_pModelCom->Get_Mesh_Branch(i);
			if (iBranch != (_int)CBody_Zombie::BODY_MESH_TYPE::_OUTTER)
				continue;

			if (FAILED(Bind_WorldMatrix(i)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
				return E_FAIL;

			if ((*m_ppPart_Breaker)->Is_RagDoll_Mesh_Body(i))
			{
				if (FAILED(m_pModelCom->Bind_BoneMatrices_Ragdoll(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i), m_pRagdoll->GetBoneMatrices_Ragdoll())))
					return E_FAIL;
			}
			else
			{
				if (m_bRender == false && m_bRagdoll == false)
					continue;

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

HRESULT CBody_Zombie::Render_LightDepth_Spot()
{
	if (nullptr == m_pShaderCom)
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
			auto iBranch = m_pModelCom->Get_Mesh_Branch(i);
			if (iBranch != (_int)CBody_Zombie::BODY_MESH_TYPE::_OUTTER)
				continue;

			if (FAILED(Bind_WorldMatrix(i)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
				return E_FAIL;

			if ((*m_ppPart_Breaker)->Is_RagDoll_Mesh_Body(i))
			{
				if (FAILED(m_pModelCom->Bind_BoneMatrices_Ragdoll(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i), m_pRagdoll->GetBoneMatrices_Ragdoll())))
					return E_FAIL;
			}
			else
			{
				if (m_bRender == false && m_bRagdoll == false)
					continue;

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

void CBody_Zombie::Add_RenderGroup()
{
	if (m_bRender)
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
	}
}

HRESULT CBody_Zombie::Initialize_Model()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	/* Set_Bone */
	m_pModelCom->Set_RootBone(ZOMBIE_ROOT_BONE_TAG);

	/* Create_Bone_Layer*/
	m_pModelCom->Add_Bone_Layer_All_Bone(BONE_LAYER_DEFAULT_TAG);

	/* Mesh Types */
	if (FAILED(Initialize_MeshTypes()))
		return E_FAIL;

	/* Create_AnimPlaying_Info */
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_0), BONE_LAYER_DEFAULT_TAG, 1.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_1), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_2), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_3), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_4), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_5), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_6), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_7), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_8), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_9), BONE_LAYER_DEFAULT_TAG, 0.f);

	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_10), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_11), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_12), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_13), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_14), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_15), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_16), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_17), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_18), BONE_LAYER_DEFAULT_TAG, 0.f);

	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_20), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_21), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_22), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_23), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_24), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_25), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_26), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_27), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_28), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_29), BONE_LAYER_DEFAULT_TAG, 0.f);

	/* Set_Root_Motion */
	m_pModelCom->Active_RootMotion_XZ(true);
	m_pModelCom->Active_RootMotion_Y(true);
	m_pModelCom->Active_RootMotion_Rotation(true);

	if (FAILED(Add_Animations()))
		return E_FAIL;

	if (FAILED(Register_Animation_Branches_AnimGroup()))
		return E_FAIL;

	if (FAILED(Register_Animation_Branches_AnimType()))
		return E_FAIL;

	if (FAILED(Register_BoneLayer_Additional_TwisterBones()))
		return E_FAIL;

	if (FAILED(SetUp_IK()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Zombie::Add_Animations()
{
#pragma region Orinary Anims 

	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Ordinary_Box_ClimbOver"), TEXT("Ordinary_Box_ClimbOver"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Ordinary_ETC"), TEXT("Ordinary_ETC"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Ordinary_Hold"), TEXT("Ordinary_Hold"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Ordinary_Idle"), TEXT("Ordinary_Idle"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Ordinary_PivotTurn"), TEXT("Ordinary_PivotTurn"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Ordinary_Stairs_PivotTurn"), TEXT("Ordinary_Stairs_PivotTurn"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Ordinary_Stairs_Walk"), TEXT("Ordinary_Stairs_Walk"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Ordinary_StandUp"), TEXT("Ordinary_StandUp"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Ordinary_Walk"), TEXT("Ordinary_Walk"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Ordinary_Walk_Lose"), TEXT("Ordinary_Walk_Lose"))))
		return E_FAIL;

#pragma endregion

#pragma region Add Anims

	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Add_Arm_L"), TEXT("Add_Arm_L"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Add_Arm_R"), TEXT("Add_Arm_R"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Add_Body"), TEXT("Add_Body"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Add_Head"), TEXT("Add_Head"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Add_Leg_L"), TEXT("Add_Leg_L"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Add_Leg_R"), TEXT("Add_Leg_R"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Add_Shoulder_L"), TEXT("Add_Shoulder_L"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Add_Shoulder_R"), TEXT("Add_Shoulder_R"))))
		return E_FAIL;

#pragma endregion

#pragma region Bite Anims 

	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Bite_Push_Down"), TEXT("Bite_Push_Down"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Bite_Creep"), TEXT("Bite_Creep"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Bite_Default_Front"), TEXT("Bite_Default_Front"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Bite_Default_Back"), TEXT("Bite_Default_Back"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Bite_Lightly_Hold"), TEXT("Bite_Lightly_Hold"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Bite_ETC"), TEXT("Bite_ETC"))))
		return E_FAIL;

#pragma endregion

#pragma region Damage Anims

	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Damage_Burst"), TEXT("Damage_Burst"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Damage_Default"), TEXT("Damage_Default"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Damage_Down"), TEXT("Damage_Down"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Damage_Electric_Shock"), TEXT("Damage_Electric_Shock"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Damage_Knockback"), TEXT("Damage_Knockback"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Damage_Lost"), TEXT("Damage_Lost"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Damage_Stairs_Down"), TEXT("Damage_Stairs_Down"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Damage_Stairs_Up"), TEXT("Damage_Stairs_Up"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Damage_Stun"), TEXT("Damage_Stun"))))
		return E_FAIL;

#pragma endregion

#pragma region Dead Anims 

	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Dead_Default"), TEXT("Dead_Default"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Dead_Bench"), TEXT("Dead_Bench"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Dead_Hide_Locker"), TEXT("Dead_Hide_Locker"))))
		return E_FAIL;

#pragma endregion

#pragma region Lost Anims 

	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Lost_Bite"), TEXT("Lost_Bite"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Lost_Dead2_Idle"), TEXT("Lost_Dead2_Idle"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Lost_Hold"), TEXT("Lost_Hold"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Lost_Idle"), TEXT("Lost_Idle"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Lost_Turn"), TEXT("Lost_Turn"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Lost_TurnOver"), TEXT("Lost_TurnOver"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Lost_Walk"), TEXT("Lost_Walk"))))
		return E_FAIL;

#pragma endregion

#pragma region Sick Anims

	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Sick_FlashGranade"), TEXT("Sick_FlashGranade"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Sick_Knife"), TEXT("Sick_Knife"))))
		return E_FAIL;

#pragma endregion

#pragma region Undiscovered Anims

	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Undiscovered_Cage"), TEXT("Undiscovered_Cage"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Undiscovered_Capture"), TEXT("Undiscovered_Capture"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Undiscovered_Celling_Fall"), TEXT("Undiscovered_Celling_Fall"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Undiscovered_Dead"), TEXT("Undiscovered_Dead"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Undiscovered_Dead_Pose"), TEXT("Undiscovered_Dead_Pose"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Undiscovered_Eat"), TEXT("Undiscovered_Eat"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Undiscovered_Fance"), TEXT("Undiscovered_Fance"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Undiscovered_HeadBang"), TEXT("Undiscovered_HeadBang"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Undiscovered_Lounge"), TEXT("Undiscovered_Lounge"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Undiscovered_Prison"), TEXT("Undiscovered_Prison"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Undiscovered_Railing_Fall"), TEXT("Undiscovered_Railing_Fall"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Undiscovered_Railing_Stund"), TEXT("Undiscovered_Railing_Stund"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Undiscovered_Wall"), TEXT("Undiscovered_Wall"))))
		return E_FAIL;

#pragma endregion

#pragma region Gimmick Anims

	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Gimmick_Window"), TEXT("Gimmick_Window"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Gimmick_Door"), TEXT("Gimmick_Door"))))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CBody_Zombie::Initialize_MeshTypes()
{
	_uint						iNumMesh = { m_pModelCom->Get_NumMeshes() };
	vector<string>				MeshTags = { m_pModelCom->Get_MeshTags() };

	for (auto& strMeshTag : MeshTags)
	{
		if (strMeshTag.find("Inside") != string::npos || strMeshTag.find("inside") != string::npos)
		{
			m_pModelCom->Set_Mesh_Branch(strMeshTag, static_cast<_uint>(BODY_MESH_TYPE::_INNER));
		}

		else if (strMeshTag.find("Joint") != string::npos || strMeshTag.find("joint") != string::npos)
		{
			m_pModelCom->Set_Mesh_Branch(strMeshTag, static_cast<_uint>(BODY_MESH_TYPE::_JOINT));
		}

		else if (strMeshTag.find("Deficit") != string::npos || strMeshTag.find("deficit") != string::npos)
		{
			m_pModelCom->Set_Mesh_Branch(strMeshTag, static_cast<_uint>(BODY_MESH_TYPE::_DEFICIT));
		}

		else if (strMeshTag.find("Damage") != string::npos || strMeshTag.find("damage") != string::npos)
		{
			m_pModelCom->Set_Mesh_Branch(strMeshTag, static_cast<_uint>(BODY_MESH_TYPE::_DAMAGED));
		}

		else if (strMeshTag.find("Internal_Mat") != string::npos || strMeshTag.find("internal_Mat") != string::npos)
		{
			m_pModelCom->Set_Mesh_Branch(strMeshTag, static_cast<_uint>(BODY_MESH_TYPE::_INTERNAL_MAT));
		}

		else if (strMeshTag.find("BrokenHead") != string::npos || strMeshTag.find("brokenHead") != string::npos)
		{
			m_pModelCom->Set_Mesh_Branch(strMeshTag, static_cast<_uint>(BODY_MESH_TYPE::_BROKEN_HEAD));
		}

		else
		{
			m_pModelCom->Set_Mesh_Branch(strMeshTag, static_cast<_uint>(BODY_MESH_TYPE::_OUTTER));
		}
	}

	m_pModelCom->Hide_Mesh_Branch(static_cast<_uint>(BODY_MESH_TYPE::_INNER), false);
	m_pModelCom->Hide_Mesh_Branch(static_cast<_uint>(BODY_MESH_TYPE::_OUTTER), false);
	m_pModelCom->Hide_Mesh_Branch(static_cast<_uint>(BODY_MESH_TYPE::_JOINT), true);
	m_pModelCom->Hide_Mesh_Branch(static_cast<_uint>(BODY_MESH_TYPE::_DEFICIT), true);
	m_pModelCom->Hide_Mesh_Branch(static_cast<_uint>(BODY_MESH_TYPE::_DAMAGED), true);
	m_pModelCom->Hide_Mesh_Branch(static_cast<_uint>(BODY_MESH_TYPE::_BROKEN_HEAD), true);
	m_pModelCom->Hide_Mesh_Branch(static_cast<_uint>(BODY_MESH_TYPE::_INTERNAL_MAT), true);


	return S_OK;
}

HRESULT CBody_Zombie::Register_Animation_Branches_AnimType()
{
	m_AnimTypeAnimLayerTags.resize(static_cast<size_t>(ZOMBIE_BODY_ANIM_TYPE::_END));

	/* For.Anim Type BlendMask */
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_BLEND_MASK)].emplace(TEXT("Add_Arm_L"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_BLEND_MASK)].emplace(TEXT("Add_Arm_R"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_BLEND_MASK)].emplace(TEXT("Add_Body"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_BLEND_MASK)].emplace(TEXT("Add_Head"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_BLEND_MASK)].emplace(TEXT("Add_Leg_L"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_BLEND_MASK)].emplace(TEXT("Add_Leg_R"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_BLEND_MASK)].emplace(TEXT("Add_Shoulder_L"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_BLEND_MASK)].emplace(TEXT("Add_Shoulder_R"));

	/* For.Anim Type Idle*/
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_IDLE)].emplace(TEXT("Lost_Idle"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_IDLE)].emplace(TEXT("Ordinary_Idle"));

	/* For.Anim Type Move */
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_MOVE)].emplace(TEXT("Lost_Walk"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_MOVE)].emplace(TEXT("Ordinary_Walk"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_MOVE)].emplace(TEXT("Ordinary_Walk_Lose"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_MOVE)].emplace(TEXT("Ordinary_Stairs_Walk"));

	/* For.Anim Type Turn */
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_TURN)].emplace(TEXT("Lost_Turn"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_TURN)].emplace(TEXT("Ordinary_PivotTurn"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_TURN)].emplace(TEXT("Ordinary_Stairs_PivotTurn"));

	/* For.Anim Type Damage */
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_DAMAGE)].emplace(TEXT("Damage_Burst"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_DAMAGE)].emplace(TEXT("Damage_Default"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_DAMAGE)].emplace(TEXT("Damage_Down"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_DAMAGE)].emplace(TEXT("Damage_Electric_Shock"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_DAMAGE)].emplace(TEXT("Damage_Knockback"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_DAMAGE)].emplace(TEXT("Damage_Lost"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_DAMAGE)].emplace(TEXT("Damage_Stairs_Down"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_DAMAGE)].emplace(TEXT("Damage_Stairs_Up"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_DAMAGE)].emplace(TEXT("Damage_Stun"));

	/* For.Anim Type Dead */
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_DEAD)].emplace(TEXT("Dead_Default"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_DEAD)].emplace(TEXT("Lost_Dead2_Idle"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_DEAD)].emplace(TEXT("Undiscovered_Dead"));
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_DEAD)].emplace(TEXT("Undiscovered_Dead_Pose"));

	/* For.Anim Type Bite */
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_DEAD)].emplace(TEXT("Lost_Bite"));

	/* For.Anim Type StandUp */
	m_AnimTypeAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_TYPE::_DEAD)].emplace(TEXT("Ordinary_StandUp"));


	/*(TEXT("Lost_Hold"));
	(TEXT("Lost_TurnOver"));

	(TEXT("Ordinary_Box_ClimbOver"));
	(TEXT("Ordinary_ETC"));
	(TEXT("Ordinary_Hold"));

	(TEXT("Sick_FlashGranade"));
	(TEXT("Sick_Knife"));

	(TEXT("Undiscovered_Cage"));
	(TEXT("Undiscovered_Capture"));
	(TEXT("Undiscovered_Celling_Fall"));

	(TEXT("Undiscovered_Eat"));
	(TEXT("Undiscovered_Fance"));
	(TEXT("Undiscovered_HeadBang"));
	(TEXT("Undiscovered_Lounge"));
	(TEXT("Undiscovered_Prison"));
	(TEXT("Undiscovered_Railing_Fall"));
	(TEXT("Undiscovered_Railing_Stund"));*/

	return S_OK;
}

HRESULT CBody_Zombie::Register_Animation_Branches_AnimGroup()
{
	m_GroupAnimLayerTags.resize(static_cast<size_t>(ZOMBIE_BODY_ANIM_GROUP::_END));

	/* For.Anim Group Add */
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_ADD)].emplace(TEXT("Add_Arm_L"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_ADD)].emplace(TEXT("Add_Arm_R"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_ADD)].emplace(TEXT("Add_Body"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_ADD)].emplace(TEXT("Add_Head"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_ADD)].emplace(TEXT("Add_Leg_L"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_ADD)].emplace(TEXT("Add_Leg_R"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_ADD)].emplace(TEXT("Add_Shoulder_L"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_ADD)].emplace(TEXT("Add_Shoulder_R"));

	/* For.Anim Group Bite */
	//	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_BITE)].emplace(TEXT("Add_Arm_L"));

	/* For.Anim Group Damage */
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_DAMAGE)].emplace(TEXT("Damage_Burst"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_DAMAGE)].emplace(TEXT("Damage_Default"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_DAMAGE)].emplace(TEXT("Damage_Down"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_DAMAGE)].emplace(TEXT("Damage_Electric_Shock"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_DAMAGE)].emplace(TEXT("Damage_Knockback"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_DAMAGE)].emplace(TEXT("Damage_Lost"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_DAMAGE)].emplace(TEXT("Damage_Stairs_Down"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_DAMAGE)].emplace(TEXT("Damage_Stairs_Up"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_DAMAGE)].emplace(TEXT("Damage_Stun"));

	/* For.Anim Group Dead */
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_DEAD)].emplace(TEXT("Dead_Default"));

	/* For.Anim Group Lost */
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_LOST)].emplace(TEXT("Lost_Bite"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_LOST)].emplace(TEXT("Lost_Dead2_Idle"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_LOST)].emplace(TEXT("Lost_Hold"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_LOST)].emplace(TEXT("Lost_Idle"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_LOST)].emplace(TEXT("Lost_Turn"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_LOST)].emplace(TEXT("Lost_TurnOver"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_LOST)].emplace(TEXT("Lost_Walk"));

	/* For.Anim Group Ordinary */
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_ORDINARY)].emplace(TEXT("Ordinary_Box_ClimbOver"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_ORDINARY)].emplace(TEXT("Ordinary_ETC"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_ORDINARY)].emplace(TEXT("Ordinary_Hold"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_ORDINARY)].emplace(TEXT("Ordinary_Idle"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_ORDINARY)].emplace(TEXT("Ordinary_PivotTurn"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_ORDINARY)].emplace(TEXT("Ordinary_Stairs_PivotTurn"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_ORDINARY)].emplace(TEXT("Ordinary_Stairs_Walk"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_ORDINARY)].emplace(TEXT("Ordinary_StandUp"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_ORDINARY)].emplace(TEXT("Ordinary_Walk"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_ORDINARY)].emplace(TEXT("Ordinary_Walk_Lose"));

	/* For.Anim Group Sick */
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_SICK)].emplace(TEXT("Sick_FlashGranade"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_SICK)].emplace(TEXT("Sick_Knife"));

	/* For.Anim Group Undiscovered */
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_UNDISCOVERED)].emplace(TEXT("Undiscovered_Cage"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_UNDISCOVERED)].emplace(TEXT("Undiscovered_Capture"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_UNDISCOVERED)].emplace(TEXT("Undiscovered_Celling_Fall"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_UNDISCOVERED)].emplace(TEXT("Undiscovered_Dead"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_UNDISCOVERED)].emplace(TEXT("Undiscovered_Dead_Pose"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_UNDISCOVERED)].emplace(TEXT("Undiscovered_Eat"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_UNDISCOVERED)].emplace(TEXT("Undiscovered_Fance"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_UNDISCOVERED)].emplace(TEXT("Undiscovered_HeadBang"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_UNDISCOVERED)].emplace(TEXT("Undiscovered_Lounge"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_UNDISCOVERED)].emplace(TEXT("Undiscovered_Prison"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_UNDISCOVERED)].emplace(TEXT("Undiscovered_Railing_Fall"));
	m_GroupAnimLayerTags[static_cast<_uint>(ZOMBIE_BODY_ANIM_GROUP::_UNDISCOVERED)].emplace(TEXT("Undiscovered_Railing_Stund"));

	return S_OK;
}

HRESULT CBody_Zombie::SetUp_IK()
{
	m_pModelCom->Add_IK(ZOMBIE_LEFT_HUMEROUS_BONE_TAG, ZOMBIE_LEFT_ARM_WRIST_BONE_TAG, ZOMBIE_IK_L_HUMEROUS_WRIST_TAG, 3, 0.f);
	m_pModelCom->Add_IK(ZOMBIE_RIGHT_HUMEROUS_BONE_TAG, ZOMBIE_RIGHT_ARM_WRIST_BONE_TAG, ZOMBIE_IK_R_HUMEROUS_WRIST_TAG, 3, 0.f);

	m_pModelCom->Add_IK(ZOMBIE_LEFT_HUMEROUS_BONE_TAG, ZOMBIE_LEFT_RADIUS_BONE_TAG, ZOMBIE_IK_L_HUMEROUS_RADIUS_TAG, 3, 0.f);
	m_pModelCom->Add_IK(ZOMBIE_RIGHT_HUMEROUS_BONE_TAG, ZOMBIE_RIGHT_RADIUS_BONE_TAG, ZOMBIE_IK_R_HUMEROUS_RADIUS_TAG, 3, 0.f);

	return S_OK;
}

HRESULT CBody_Zombie::Register_BoneLayer_Additional_TwisterBones()
{
	if (FAILED(Register_BoneLayer_Childs_NonInclude_Joint(BONE_LAYER_L_LEG_TWIST_TAG, "l_leg_femur", "l_leg_ball")))
		return E_FAIL;

	if (FAILED(Register_BoneLayer_Childs_NonInclude_Joint(BONE_LAYER_R_LEG_TWIST_TAG, "r_leg_femur", "r_leg_ball")))
		return E_FAIL;

	if (FAILED(Register_BoneLayer_Childs_NonInclude_Joint(BONE_LAYER_L_ARM_TWIST_TAG, "l_arm_humerus", "l_arm_wrist")))
		return E_FAIL;

	if (FAILED(Register_BoneLayer_Childs_NonInclude_Joint(BONE_LAYER_R_ARM_TWIST_TAG, "r_arm_humerus", "r_arm_wrist")))
		return E_FAIL;

	//if (FAILED(Register_BoneLayer_Childs_NonInclude_Joint(BONE_LAYER_L_SHOULDER_TWIST_TAG, "l_arm_clavicle", "l_arm_wrist")))
	////	if (FAILED(Register_BoneLayer_Childs_NonInclude_Joint(BONE_LAYER_L_SHOULDER_TWIST_TAG, "spine_1", "l_arm_wrist")))
	//	return E_FAIL;

	//if (FAILED(Register_BoneLayer_Childs_NonInclude_Joint(BONE_LAYER_R_SHOULDER_TWIST_TAG, "r_arm_clavicle", "r_arm_wrist")))
	////	if (FAILED(Register_BoneLayer_Childs_NonInclude_Joint(BONE_LAYER_R_SHOULDER_TWIST_TAG, "spine_1", "r_arm_wrist")))
	//	return E_FAIL;

	list<string>			R_ShoulderAddBoneTags;
	R_ShoulderAddBoneTags.emplace_back("spine_1");
	R_ShoulderAddBoneTags.emplace_back("spine_2");
	R_ShoulderAddBoneTags.emplace_back("r_arm_clavicle");
	R_ShoulderAddBoneTags.emplace_back("r_arm_humerus");
	R_ShoulderAddBoneTags.emplace_back("r_arm_radius");
	R_ShoulderAddBoneTags.emplace_back("r_arm_wrist");
	R_ShoulderAddBoneTags.emplace_back("r_arm_radius_twist_3_H");
	R_ShoulderAddBoneTags.emplace_back("r_arm_radius_twist_2_H");
	R_ShoulderAddBoneTags.emplace_back("r_arm_radius_twist_1_H");
	R_ShoulderAddBoneTags.emplace_back("r_arm_humerus_twist_3_H");
	R_ShoulderAddBoneTags.emplace_back("r_arm_humerus_twist_2_H");
	R_ShoulderAddBoneTags.emplace_back("r_arm_humerus_twist_1_H");
	R_ShoulderAddBoneTags.emplace_back("neck_0");
	R_ShoulderAddBoneTags.emplace_back("neck_1");

	list<_uint>				R_ShoulderAddBoneIndices;
	for (auto& strBoneTag : R_ShoulderAddBoneTags)
	{
		_int			iBoneIndex = { m_pModelCom->Get_BoneIndex(strBoneTag) };
		if (iBoneIndex == -1)
			MSG_BOX(TEXT("BoneIndex - 1"));
		R_ShoulderAddBoneIndices.emplace_back(iBoneIndex);
	}
	m_pModelCom->Add_Bone_Layer_BoneIndices(BONE_LAYER_R_SHOULDER_TWIST_TAG, R_ShoulderAddBoneIndices);


	list<string>			L_ShoulderAddBoneTags;
	L_ShoulderAddBoneTags.emplace_back("spine_1");
	L_ShoulderAddBoneTags.emplace_back("spine_2");
	L_ShoulderAddBoneTags.emplace_back("l_arm_clavicle");
	L_ShoulderAddBoneTags.emplace_back("l_arm_humerus");
	L_ShoulderAddBoneTags.emplace_back("l_arm_radius");
	L_ShoulderAddBoneTags.emplace_back("l_arm_wrist");
	L_ShoulderAddBoneTags.emplace_back("l_arm_radius_twist_3_H");
	L_ShoulderAddBoneTags.emplace_back("l_arm_radius_twist_2_H");
	L_ShoulderAddBoneTags.emplace_back("l_arm_radius_twist_1_H");
	L_ShoulderAddBoneTags.emplace_back("l_arm_humerus_twist_3_H");
	L_ShoulderAddBoneTags.emplace_back("l_arm_humerus_twist_2_H");
	L_ShoulderAddBoneTags.emplace_back("l_arm_humerus_twist_1_H");
	L_ShoulderAddBoneTags.emplace_back("neck_0");
	L_ShoulderAddBoneTags.emplace_back("neck_1");

	list<_uint>				L_ShoulderAddBoneIndices;
	for (auto& strBoneTag : L_ShoulderAddBoneTags)
	{
		_int			iBoneIndex = { m_pModelCom->Get_BoneIndex(strBoneTag) };
		if (iBoneIndex == -1)
			MSG_BOX(TEXT("BoneIndex - 1"));
		L_ShoulderAddBoneIndices.emplace_back(iBoneIndex);
	}
	m_pModelCom->Add_Bone_Layer_BoneIndices(BONE_LAYER_L_SHOULDER_TWIST_TAG, L_ShoulderAddBoneIndices);

	//if (FAILED(Register_BoneLayer_Childs_NonInclude_Joint(BONE_LAYER_L_LEG_TWIST_TAG, "COG", "l_leg_ball")))
	//	return E_FAIL;

	//if (FAILED(Register_BoneLayer_Childs_NonInclude_Joint(BONE_LAYER_L_LEG_TWIST_TAG, "neck_0", "head")))
	//	return E_FAIL;

	return S_OK;
}

void CBody_Zombie::Play_Animations(_float fTimeDelta)
{
	if (m_bRagdoll == false)
	{
		m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, m_pRootTranslation);
	}
}

HRESULT CBody_Zombie::Register_BoneLayer_Childs_NonInclude_Joint(const wstring& strBoneLayerTag, const string& strTopParentTag, const string& strEndEffectorTag)
{
	list<_uint>				ChildBoneIndices;
	m_pModelCom->Get_Child_BoneIndices(strTopParentTag, ChildBoneIndices);

	list<_uint>				ChildJointIndices;
	m_pModelCom->Get_Child_ZointIndices(strTopParentTag, strEndEffectorTag, ChildJointIndices);

	for (auto& iterSrc = ChildBoneIndices.begin(); iterSrc != ChildBoneIndices.end(); )
	{
		list<_uint>::iterator		iterDst = { find(ChildJointIndices.begin(), ChildJointIndices.end(), *iterSrc) };
		if (iterDst != ChildJointIndices.end())
		{
			iterSrc = ChildBoneIndices.erase(iterSrc);
		}
		else
			++iterSrc;
	}
	m_pModelCom->Add_Bone_Layer_BoneIndices(strBoneLayerTag, ChildBoneIndices);

	return S_OK;
}

ZOMBIE_BODY_ANIM_TYPE CBody_Zombie::Get_Current_AnimType(PLAYING_INDEX eIndex)
{
	ZOMBIE_BODY_ANIM_TYPE			eType = { ZOMBIE_BODY_ANIM_TYPE::_END };

	wstring			strAnimLayerTag = { m_pModelCom->Get_CurrentAnimLayerTag(static_cast<_uint>(eIndex)) };
	if (TEXT("") == strAnimLayerTag)
		return eType;

	_uint							iType = { 0 };
	for (auto& AnimTypeAnimLayerTags : m_AnimTypeAnimLayerTags)
	{
		unordered_set<wstring>::iterator			iter = { AnimTypeAnimLayerTags.find(strAnimLayerTag) };
		if (iter != AnimTypeAnimLayerTags.end())
		{
			eType = static_cast<ZOMBIE_BODY_ANIM_TYPE>(iType);
			break;
		}
		++iType;
	}

	return eType;
}

ZOMBIE_BODY_ANIM_GROUP CBody_Zombie::Get_Current_AnimGroup(PLAYING_INDEX eIndex)
{
	ZOMBIE_BODY_ANIM_GROUP			eType = { ZOMBIE_BODY_ANIM_GROUP::_END };

	wstring			strAnimLayerTag = { m_pModelCom->Get_CurrentAnimLayerTag(static_cast<_uint>(eIndex)) };
	if (TEXT("") == strAnimLayerTag)
		return eType;

	_uint							iType = { 0 };
	for (auto& GroupAnimLayerTags : m_GroupAnimLayerTags)
	{
		unordered_set<wstring>::iterator			iter = { GroupAnimLayerTags.find(strAnimLayerTag) };
		if (iter != GroupAnimLayerTags.end())
		{
			eType = static_cast<ZOMBIE_BODY_ANIM_GROUP>(iType);
			break;
		}
		++iType;
	}

	return eType;
}

void CBody_Zombie::SetRagdoll(_int iId, _float4 vForce, COLLIDER_TYPE eType)
{
	m_pGameInstance->Start_Ragdoll(m_pRagdoll, iId);

	m_pRagdoll->Add_Force(vForce, eType);
	m_bRagdoll = true;
	m_pModelCom->Set_OptimizationCulling(false);
}

void CBody_Zombie::SetPartialRagdoll(_int iId, _float4 vForce, COLLIDER_TYPE eType)
{
	if (eType == COLLIDER_TYPE::HEAD)
		return;

	m_pGameInstance->Start_PartialRagdoll(m_pRagdoll, iId, eType);

	m_bPartial_Ragdoll = true;
	m_pModelCom->Set_OptimizationCulling(false);
}

void CBody_Zombie::SetCulling(_bool boolean)
{
	if (m_pRagdoll)
		m_pRagdoll->SetCulling(boolean);

	m_bRender = !boolean;
}

PxRigidDynamic* CBody_Zombie::Get_Ragdoll_RigidBody(COLLIDER_TYPE eType)
{
	return m_pRagdoll->GetRigidBody(eType);
}

_float4 CBody_Zombie::GetRigidBodyPos(COLLIDER_TYPE eType)
{
	return m_pRagdoll->GetRigidBodyPos(eType);
}

void CBody_Zombie::Set_Render(_bool boolean)
{
	m_bRender = boolean;

	if (m_bRagdoll)
		m_bRender = true;
}

void CBody_Zombie::Update_Current_MotionType()
{
	if (nullptr == m_pModelCom)
		return;

#pragma region 다시 살려야함

	/*m_ePreMotionType = m_eCurrentMotionType;
	_int			iCurrentAnimIndex = { m_pModelCom->Get_AnimIndex_PlayingInfo(PLAYING_INDEX::INDEX_0) };

	if (-1 == iCurrentAnimIndex)
	{
		m_eCurrentMotionType = MOTION_END;
	}

	else if (iCurrentAnimIndex == ANIM_IDLE_LOOP_A ||
		iCurrentAnimIndex == ANIM_WALK_LOOP_A ||
		iCurrentAnimIndex == ANIM_TURNING_LOOP_FR_A ||
		iCurrentAnimIndex == ANIM_WALK_START_A ||
		iCurrentAnimIndex == ANIM_WALK_TOTTER_F_A ||
		iCurrentAnimIndex == ANIM_WALK_TURN_L180_A ||
		iCurrentAnimIndex == ANIM_WALK_TURN_R180_A ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_L90_A ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_R90_A ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_L180_A ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_R180_A
		)
	{
		m_eCurrentMotionType = MOTION_A;
	}

	else if (iCurrentAnimIndex == ANIM_IDLE_LOOP_B ||
		iCurrentAnimIndex == ANIM_WALK_LOOP_B ||
		iCurrentAnimIndex == ANIM_TURNING_LOOP_FR_B ||
		iCurrentAnimIndex == ANIM_WALK_START_B ||
		iCurrentAnimIndex == ANIM_WALK_TOTTER_F_B ||
		iCurrentAnimIndex == ANIM_WALK_TURN_L180_B ||
		iCurrentAnimIndex == ANIM_WALK_TURN_R180_B ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_L90_B ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_R90_B ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_L180_B ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_R180_B
		)
	{
		m_eCurrentMotionType = MOTION_B;
	}

	else if (iCurrentAnimIndex == ANIM_IDLE_LOOP_C ||
		iCurrentAnimIndex == ANIM_WALK_LOOP_C ||
		iCurrentAnimIndex == ANIM_TURNING_LOOP_FR_C ||
		iCurrentAnimIndex == ANIM_WALK_START_C ||
		iCurrentAnimIndex == ANIM_WALK_TOTTER_F_C ||
		iCurrentAnimIndex == ANIM_WALK_TURN_L180_C ||
		iCurrentAnimIndex == ANIM_WALK_TURN_R180_C ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_L90_C ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_R90_C ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_L180_C ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_R180_C
		)
	{
		m_eCurrentMotionType = MOTION_C;
	}

	else if (iCurrentAnimIndex == ANIM_IDLE_LOOP_D ||
		iCurrentAnimIndex == ANIM_WALK_LOOP_D ||
		iCurrentAnimIndex == ANIM_TURNING_LOOP_FR_D ||
		iCurrentAnimIndex == ANIM_WALK_START_D ||
		iCurrentAnimIndex == ANIM_WALK_TOTTER_F_D ||
		iCurrentAnimIndex == ANIM_WALK_TURN_L180_D ||
		iCurrentAnimIndex == ANIM_WALK_TURN_R180_D ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_L90_D ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_R90_D ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_L180_D ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_R180_D
		)
	{
		m_eCurrentMotionType = MOTION_D;
	}

	else if (iCurrentAnimIndex == ANIM_IDLE_LOOP_E ||
		iCurrentAnimIndex == ANIM_WALK_LOOP_E ||
		iCurrentAnimIndex == ANIM_TURNING_LOOP_FR_E ||
		iCurrentAnimIndex == ANIM_WALK_START_E ||
		iCurrentAnimIndex == ANIM_WALK_TOTTER_F_E ||
		iCurrentAnimIndex == ANIM_WALK_TURN_L180_E ||
		iCurrentAnimIndex == ANIM_WALK_TURN_R180_E ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_L90_E ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_R90_E ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_L180_E ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_R180_E
		)
	{
		m_eCurrentMotionType = MOTION_E;
	}

	else if (iCurrentAnimIndex == ANIM_IDLE_LOOP_F ||
		iCurrentAnimIndex == ANIM_WALK_LOOP_F ||
		iCurrentAnimIndex == ANIM_TURNING_LOOP_FR_F ||
		iCurrentAnimIndex == ANIM_WALK_START_F ||
		iCurrentAnimIndex == ANIM_WALK_TOTTER_F_F ||
		iCurrentAnimIndex == ANIM_WALK_TURN_L180_F ||
		iCurrentAnimIndex == ANIM_WALK_TURN_R180_F ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_L90_F ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_R90_F ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_L180_F ||
		iCurrentAnimIndex == ANIM_PIVOT_TURN_R180_F
		)
	{
		m_eCurrentMotionType = MOTION_F;
	}

	else
	{
		m_eCurrentMotionType = m_eCurrentMotionType;
	}*/

#pragma endregion

}

void CBody_Zombie::WakeUp_Ragdoll()
{
	m_pRagdoll->WakeUp();
}

void CBody_Zombie::SetKinematic_JumpScare(_bool boolean,COLLIDER_TYPE eType)
{
	m_pRagdoll->Set_Part_Kinematic(boolean, eType);
}

void CBody_Zombie::SetKinematic_JumpScare_All(_bool boolean)
{
	m_pRagdoll->Set_Kinematic_JumpScare_All(boolean);
}

HRESULT CBody_Zombie::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (ZOMBIE_BODY_TYPE::_FEMALE == m_eBodyModelType)
	{
		if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Body_Female"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

	else if (ZOMBIE_BODY_TYPE::_MALE == m_eBodyModelType)
	{
		if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Body_Male"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

	else if (ZOMBIE_BODY_TYPE::_MALE_BIG == m_eBodyModelType)
	{
		if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_Zombie_Body_Male_Big"),
			TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
	}

#ifdef _DEBUG
	else
	{
		MSG_BOX(TEXT("Called : HRESULT CBody_Zombie::Add_Components() 좀비 담당자 호출"));
	}
#endif

	return S_OK;
}

HRESULT CBody_Zombie::Bind_ShaderResources()
{

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

	auto bHair = false;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Hair", &bHair, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Zombie::Bind_WorldMatrix(_uint iIndex)
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
		if ((*m_ppPart_Breaker)->Is_RagDoll_Mesh_Body(iIndex))
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
	return S_OK;
}

CBody_Zombie* CBody_Zombie::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Zombie* pInstance = { new CBody_Zombie(pDevice, pContext) };

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Zombie::Clone(void* pArg)
{
	CBody_Zombie* pInstance = { new CBody_Zombie(*this) };

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Zombie::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
