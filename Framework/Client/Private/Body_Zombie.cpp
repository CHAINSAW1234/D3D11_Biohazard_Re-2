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

	m_pRagdoll = m_pGameInstance->Create_Ragdoll(m_pModelCom->GetBoneVector(), m_pParentsTransform, "../Bin/Resources/Models/Ex_Default_Zombie/Body.fbx");

	return S_OK;
}

void CBody_Zombie::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);

}

void CBody_Zombie::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CBody_Zombie::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if(m_bRagdoll == false)
		m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, m_pRootTranslation);

	//	현재 모션이 A ~ F 타입인지 판단하고 저장 => 다음 틱에 태스크 노드에서 참조하여 모션을 결정할것이다.
	Update_Current_MotionType();

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
}

HRESULT CBody_Zombie::Render()
{
	auto vPos = m_pParentsTransform->Get_State_Vector(CTransform::STATE_POSITION);
	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + CONTROLLER_GROUND_GAP_ZOMBIE);
	if (!m_pGameInstance->isInFrustum_WorldSpace(vPos, 1.f) && m_bRagdoll == false)
		return S_OK;

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

		if (FAILED(m_pModelCom->Bind_PrevBoneMatrices(m_pShaderCom, "g_PrevBoneMatrices", static_cast<_uint>(i))))
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

	return S_OK;
}

HRESULT CBody_Zombie::Render_LightDepth_Dir()
{
	auto vPos = m_pParentsTransform->Get_State_Vector(CTransform::STATE_POSITION);
	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + CONTROLLER_GROUND_GAP_ZOMBIE);
	if (!m_pGameInstance->isInFrustum_WorldSpace(vPos, 1.f) && m_bRagdoll == false)
		return S_OK;

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
	auto vPos = m_pParentsTransform->Get_State_Vector(CTransform::STATE_POSITION);
	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + CONTROLLER_GROUND_GAP_ZOMBIE);
	if (!m_pGameInstance->isInFrustum_WorldSpace(vPos, 1.f) && m_bRagdoll == false)
		return S_OK;

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
	auto vPos = m_pParentsTransform->Get_State_Vector(CTransform::STATE_POSITION);
	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + CONTROLLER_GROUND_GAP_ZOMBIE);
	if (!m_pGameInstance->isInFrustum_WorldSpace(vPos, 1.f) && m_bRagdoll == false)
		return S_OK;

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
	m_pModelCom->Add_AnimPlayingInfo(0, false, PLAYING_INDEX::INDEX_0, BONE_LAYER_DEFAULT_TAG, 1.f);
	m_pModelCom->Add_AnimPlayingInfo(0, false, PLAYING_INDEX::INDEX_1, BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(0, false, PLAYING_INDEX::INDEX_2, BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(0, false, PLAYING_INDEX::INDEX_3, BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(0, false, PLAYING_INDEX::INDEX_4, BONE_LAYER_DEFAULT_TAG, 0.f);
	m_pModelCom->Add_AnimPlayingInfo(0, false, PLAYING_INDEX::INDEX_5, BONE_LAYER_DEFAULT_TAG, 0.f);

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

	/* Branch Anim Type ( Start, Loop ) */
#pragma region Start Anims
	m_StartAnimIndices.emplace(static_cast<_uint>(ANIM_START::ANIM_WALK_START_A));
	m_StartAnimIndices.emplace(static_cast<_uint>(ANIM_START::ANIM_WALK_START_B));
	m_StartAnimIndices.emplace(static_cast<_uint>(ANIM_START::ANIM_WALK_START_C));
	m_StartAnimIndices.emplace(static_cast<_uint>(ANIM_START::ANIM_WALK_START_D));
	m_StartAnimIndices.emplace(static_cast<_uint>(ANIM_START::ANIM_WALK_START_E));
	m_StartAnimIndices.emplace(static_cast<_uint>(ANIM_START::ANIM_WALK_START_F));

	m_LoopAnimIndices.emplace(static_cast<_uint>(ANIM_START::ANIM_WALK_LOOP_TO_B_WALK));
	m_LoopAnimIndices.emplace(static_cast<_uint>(ANIM_START::ANIM_WALK_LOOP_TO_C_WALK));
	m_LoopAnimIndices.emplace(static_cast<_uint>(ANIM_START::ANIM_WALK_LOOP_TO_D_WALK));
	m_LoopAnimIndices.emplace(static_cast<_uint>(ANIM_START::ANIM_WALK_LOOP_TO_E_WALK));
	m_LoopAnimIndices.emplace(static_cast<_uint>(ANIM_START::ANIM_WALK_LOOP_TO_F_WALK));

	for (_uint iIndex = static_cast<_uint>(ANIM_START::ANIM_WALK_TOTTER_F_A);
		iIndex <= static_cast<_uint>(ANIM_START::ANIM_WALK_TOTTER_F_F); ++iIndex)
	{
		m_StartAnimIndices.emplace(iIndex);
	}

	for (_uint iIndex = static_cast<_uint>(ANIM_START::ANIM_WALK_TURN_L180_A);
		iIndex <= static_cast<_uint>(ANIM_START::ANIM_WALK_TURN_R180_F); ++iIndex)
	{
		m_StartAnimIndices.emplace(iIndex);
	}

	for (_uint iIndex = static_cast<_uint>(ANIM_START::ANIM_PIVOT_TURN_L90_A);
		iIndex <= static_cast<_uint>(ANIM_START::ANIM_PIVOT_TURN_R90_F); ++iIndex)
	{
		m_StartAnimIndices.emplace(iIndex);
	}

	for (_uint iIndex = static_cast<_uint>(ANIM_START::ANIM_PIVOT_TURN_L180_A);
		iIndex <= static_cast<_uint>(ANIM_START::ANIM_PIVOT_TURN_R180_F); ++iIndex)
	{
		m_StartAnimIndices.emplace(iIndex);
	}

	for (_uint iIndex = static_cast<_uint>(ANIM_START::ANIM_STANDUP_FACEDOWN_F);
		iIndex <= static_cast<_uint>(ANIM_START::ANIM_STANDUP_FACEDOWN_R); ++iIndex)
	{
		m_StartAnimIndices.emplace(iIndex);
	}

	for (_uint iIndex = static_cast<_uint>(ANIM_START::ANIM_STANDUP_FACEUP_F);
		iIndex <= static_cast<_uint>(ANIM_START::ANIM_STANDUP_FACEUP_R); ++iIndex)
	{
		m_StartAnimIndices.emplace(iIndex);
	}
#pragma endregion

#pragma region Loop Anims
	m_LoopAnimIndices.emplace(static_cast<_uint>(ANIM_LOOP::ANIM_WALK_LOOP_A));
	m_LoopAnimIndices.emplace(static_cast<_uint>(ANIM_LOOP::ANIM_WALK_LOOP_B));
	m_LoopAnimIndices.emplace(static_cast<_uint>(ANIM_LOOP::ANIM_WALK_LOOP_C));
	m_LoopAnimIndices.emplace(static_cast<_uint>(ANIM_LOOP::ANIM_WALK_LOOP_D));
	m_LoopAnimIndices.emplace(static_cast<_uint>(ANIM_LOOP::ANIM_WALK_LOOP_E));
	m_LoopAnimIndices.emplace(static_cast<_uint>(ANIM_LOOP::ANIM_WALK_LOOP_F));

	for (_uint iIndex = static_cast<_uint>(ANIM_LOOP::ANIM_TURNING_LOOP_FL_A);
		iIndex <= static_cast<_uint>(ANIM_LOOP::ANIM_TURNING_LOOP_FR_A); ++iIndex)
	{
		m_LoopAnimIndices.emplace(iIndex);
	}

	for (_uint iIndex = static_cast<_uint>(ANIM_LOOP::ANIM_TURNING_LOOP_FL_B);
		iIndex <= static_cast<_uint>(ANIM_LOOP::ANIM_TURNING_LOOP_FR_B); ++iIndex)
	{
		m_LoopAnimIndices.emplace(iIndex);
	}

	for (_uint iIndex = static_cast<_uint>(ANIM_LOOP::ANIM_TURNING_LOOP_FL_C);
		iIndex <= static_cast<_uint>(ANIM_LOOP::ANIM_TURNING_LOOP_FR_C); ++iIndex)
	{
		m_LoopAnimIndices.emplace(iIndex);
	}

	for (_uint iIndex = static_cast<_uint>(ANIM_LOOP::ANIM_TURNING_LOOP_FL_D);
		iIndex <= static_cast<_uint>(ANIM_LOOP::ANIM_TURNING_LOOP_FR_D); ++iIndex)
	{
		m_LoopAnimIndices.emplace(iIndex);
	}

	for (_uint iIndex = static_cast<_uint>(ANIM_LOOP::ANIM_TURNING_LOOP_FL_E);
		iIndex <= static_cast<_uint>(ANIM_LOOP::ANIM_TURNING_LOOP_FR_E); ++iIndex)
	{
		m_LoopAnimIndices.emplace(iIndex);
	}

	for (_uint iIndex = static_cast<_uint>(ANIM_LOOP::ANIM_TURNING_LOOP_FL_F);
		iIndex <= static_cast<_uint>(ANIM_LOOP::ANIM_TURNING_LOOP_FR_F); ++iIndex)
	{
		m_LoopAnimIndices.emplace(iIndex);
	}

#pragma endregion


#pragma region MOVE_ANIM

	m_MoveAnimIndices.emplace(static_cast<_uint>(CBody_Zombie::ANIM_WALK_START_A));	
	m_MoveAnimIndices.emplace(static_cast<_uint>(CBody_Zombie::ANIM_WALK_START_B ));
	m_MoveAnimIndices.emplace(static_cast<_uint>(CBody_Zombie::ANIM_WALK_START_C ));
	m_MoveAnimIndices.emplace(static_cast<_uint>(CBody_Zombie::ANIM_WALK_START_D ));
	m_MoveAnimIndices.emplace(static_cast<_uint>(CBody_Zombie::ANIM_WALK_START_E ));
	m_MoveAnimIndices.emplace(static_cast<_uint>(CBody_Zombie::ANIM_WALK_START_F ));
	m_MoveAnimIndices.emplace(static_cast<_uint>(CBody_Zombie::ANIM_WALK_LOOP_TO_B_WALK ));
	m_MoveAnimIndices.emplace(static_cast<_uint>(CBody_Zombie::ANIM_WALK_LOOP_TO_C_WALK ));
	m_MoveAnimIndices.emplace(static_cast<_uint>(CBody_Zombie::ANIM_WALK_LOOP_TO_D_WALK ));
	m_MoveAnimIndices.emplace(static_cast<_uint>(CBody_Zombie::ANIM_WALK_LOOP_TO_E_WALK ));
	m_MoveAnimIndices.emplace(static_cast<_uint>(CBody_Zombie::ANIM_WALK_LOOP_TO_F_WALK ));
	m_MoveAnimIndices.emplace(static_cast<_uint>(CBody_Zombie::ANIM_WALK_TOTTER_F_A ));
	m_MoveAnimIndices.emplace(static_cast<_uint>(CBody_Zombie::ANIM_WALK_TOTTER_F_B ));
	m_MoveAnimIndices.emplace(static_cast<_uint>(CBody_Zombie::ANIM_WALK_TOTTER_F_C ));
	m_MoveAnimIndices.emplace(static_cast<_uint>(CBody_Zombie::ANIM_WALK_TOTTER_F_D ));
	m_MoveAnimIndices.emplace(static_cast<_uint>(CBody_Zombie::ANIM_WALK_TOTTER_F_E ));
	m_MoveAnimIndices.emplace(static_cast<_uint>(CBody_Zombie::ANIM_WALK_TOTTER_F_F ));
	m_MoveAnimIndices.emplace(static_cast<_uint>(CBody_Zombie::ANIM_WALK_LOOP_A ));
	m_MoveAnimIndices.emplace(static_cast<_uint>(CBody_Zombie::ANIM_WALK_LOOP_B ));
	m_MoveAnimIndices.emplace(static_cast<_uint>(CBody_Zombie::ANIM_WALK_LOOP_C ));
	m_MoveAnimIndices.emplace(static_cast<_uint>(CBody_Zombie::ANIM_WALK_LOOP_D ));
	m_MoveAnimIndices.emplace(static_cast<_uint>(CBody_Zombie::ANIM_WALK_LOOP_E ));
	m_MoveAnimIndices.emplace(static_cast<_uint>(CBody_Zombie::ANIM_WALK_LOOP_F));

#pragma endregion

#pragma region TURN_ANIM

	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_TURNING_LOOP_FL_A));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_TURNING_LOOP_FR_A));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_TURNING_LOOP_FL_B));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_TURNING_LOOP_FR_B));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_TURNING_LOOP_FL_C));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_TURNING_LOOP_FR_C));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_TURNING_LOOP_FL_D));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_TURNING_LOOP_FR_D));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_TURNING_LOOP_FL_E));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_TURNING_LOOP_FR_E));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_TURNING_LOOP_FL_F));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_TURNING_LOOP_FR_F));

	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_L90_A));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_R90_A));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_L90_B));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_R90_B));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_L90_C));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_R90_C));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_L90_D));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_R90_D));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_L90_E));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_R90_E));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_L90_F));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_R90_F));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_L180_A));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_R180_A));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_L180_B));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_R180_B));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_L180_C));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_R180_C));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_L180_D));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_R180_D));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_L180_E));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_R180_E));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_L180_F));
	m_TurnAnimIndices.emplace(static_cast<_uint>(ANIM_PIVOT_TURN_R180_F));

#pragma endregion

	return S_OK;
}

void CBody_Zombie::SetRagdoll(_int iId, _float4 vForce, COLLIDER_TYPE eType)
{
	m_pGameInstance->Start_Ragdoll(m_pRagdoll, iId);
	m_pRagdoll->Add_Force(vForce,eType);
	m_bRagdoll = true;
}

void CBody_Zombie::Update_Current_MotionType()
{
	if (nullptr == m_pModelCom)
		return;

	m_ePreMotionType = m_eCurrentMotionType;
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
	}

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
