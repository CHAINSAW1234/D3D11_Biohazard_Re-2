#include "stdafx.h"
#include "Body_Zombie.h"

#include "Light.h"
#include "RagDoll_Physics.h"

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

	BODY_MONSTER_DESC*		pDesc = { static_cast<BODY_MONSTER_DESC*>(pArg) };

	m_pRootTranslation = pDesc->pRootTranslation;
	if (nullptr == m_pRootTranslation)
		return E_FAIL;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Initialize_Model()))
		return E_FAIL;

	m_eCurrentMotionType = MOTION_TYPE::MOTION_A;

	m_pRagdoll = m_pGameInstance->Create_Ragdoll(m_pModelCom->GetBoneVector(), m_pParentsTransform, "../Bin/Resources/Models/Zombie/Body.fbx");

	return S_OK;
}

void CBody_Zombie::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

}

void CBody_Zombie::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if(m_bRagdoll)
	{
		auto vPos = m_pParentsTransform->Get_State_Vector(CTransform::STATE_POSITION);
		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + CONTROLLER_GROUND_GAP_ZOMBIE);
		if (!m_pGameInstance->isInFrustum_WorldSpace(vPos, 1.f))
		{
			m_pRagdoll->SetCulling(true);
		}
		else
		{
			m_pRagdoll->SetCulling(false);
		}
	}
	else
	{
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
}

void CBody_Zombie::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if(m_bRagdoll == false)
		m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, m_pRootTranslation);

	//	현재 모션이 A ~ F 타입인지 판단하고 저장 => 다음 틱에 태스크 노드에서 참조하여 모션을 결정할것이다.
	Update_Current_MotionType();

	if(m_bRender)
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


		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(static_cast<_uint>(i));
	}

	return S_OK;
}

HRESULT CBody_Zombie::Render_LightDepth_Dir()
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

HRESULT CBody_Zombie::Render_LightDepth_Point()
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

HRESULT CBody_Zombie::Render_LightDepth_Spot()
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

HRESULT CBody_Zombie::Initialize_Model()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	/* Set_Bone */
	m_pModelCom->Set_RootBone(ROOT_BONE_TAG);

	/* Create_Bone_Layer*/
	m_pModelCom->Add_Bone_Layer_All_Bone(BONE_LAYER_DEFAULT_TAG);

	/* Create_AnimPlaying_Info */
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_0), BONE_LAYER_DEFAULT_TAG, 1.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_1), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_2), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_3), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_4), BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, static_cast<_uint>(PLAYING_INDEX::INDEX_5), BONE_LAYER_DEFAULT_TAG, 0.f);

	/* Set_Root_Motion */
	m_pModelCom->Active_RootMotion_XZ(true);
	m_pModelCom->Active_RootMotion_Y(true);
	m_pModelCom->Active_RootMotion_Rotation(true);

	/* Set_Hide_Mesh */
	vector<string>			MeshTags  = { m_pModelCom->Get_MeshTags() };

	vector<string>			ResultMeshTags;
	for (auto& strMeshTag : MeshTags)
	{
		if (strMeshTag.find("Body") != string::npos)
		{
			if (strMeshTag.find("Inside") != string::npos)
				continue;

			if (strMeshTag.find("Joint") != string::npos)
				continue;

			ResultMeshTags.push_back(strMeshTag);
		}
	}

	for (auto& strMeshTag : MeshTags)
	{
		m_pModelCom->Hide_Mesh(strMeshTag, true);
	}

	for (auto& strMeshTag : ResultMeshTags)
	{
		m_pModelCom->Hide_Mesh(strMeshTag, false);
	}

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
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Ordinary_Idle"), TEXT("Ordinary_Box_Idle"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Ordinary_PivotTurn"), TEXT("Ordinary_PivotTurn"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Ordinary_Stairs_PivotTurn"), TEXT("Ordinary_Stairs_PivotTurn"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Ordinary_Stairs_Walk"), TEXT("Ordinary_Box_Stairs_Walk"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Body_Zombie_Ordinary_StandUp"), TEXT("Ordinary_Box_StandUp"))))
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

#pragma endregion
	return S_OK;
}

void CBody_Zombie::SetRagdoll(_int iId, _float4 vForce, COLLIDER_TYPE eType)
{
	m_pGameInstance->Start_Ragdoll(m_pRagdoll, iId);
	m_pRagdoll->Add_Force(vForce,eType);
	m_bRagdoll = true;
}

void CBody_Zombie::SetCulling(_bool boolean)
{
	if(m_pRagdoll)
		m_pRagdoll->SetCulling(boolean);
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

_bool CBody_Zombie::Is_Start_Anim(_uint iAnimIndex)
{
	set<_uint>::iterator		iter = { m_StartAnimIndices.find(iAnimIndex) };
	
	return iter != m_StartAnimIndices.end();
}

_bool CBody_Zombie::Is_Loop_Anim(_uint iAnimIndex)
{
	set<_uint>::iterator		iter = { m_LoopAnimIndices.find(iAnimIndex) };

	return iter != m_LoopAnimIndices.end();
}

_bool CBody_Zombie::Is_Move_Anim(_uint iAnimIndex)
{
	set<_uint>::iterator		iter = { m_MoveAnimIndices.find(iAnimIndex) };

	return iter != m_MoveAnimIndices.end();
}

_bool CBody_Zombie::Is_Turn_Anim(_uint iAnimIndex)
{
	set<_uint>::iterator		iter = { m_TurnAnimIndices.find(iAnimIndex) };

	return iter != m_TurnAnimIndices.end();
}

HRESULT CBody_Zombie::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_ZombieBody"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Zombie::Bind_ShaderResources()
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

	return S_OK;
}

CBody_Zombie* CBody_Zombie::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Zombie*		pInstance = { new CBody_Zombie(pDevice, pContext) };

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_Zombie"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Zombie::Clone(void* pArg)
{
	CBody_Zombie*		pInstance = { new CBody_Zombie(*this) };

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
