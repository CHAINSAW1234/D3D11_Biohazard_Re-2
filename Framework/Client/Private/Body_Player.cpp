#include "stdafx.h"
#include "..\Public\Body_Player.h"

#include "Player.h"
#include "Light.h"
#include "RagDoll_Physics.h"

CBody_Player::CBody_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBody_Player::CBody_Player(const CBody_Player& rhs)
	: CPartObject{ rhs }
{

}

HRESULT CBody_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Player::Initialize(void* pArg)
{
	BODY_DESC* pDesc = { static_cast<BODY_DESC*>(pArg) };

	pDesc->fSpeedPerSec = 10.f;
	pDesc->fRotationPerSec = XMConvertToRadians(90.0f);
	m_pRootTranslation = pDesc->pRootTranslation;

	if (nullptr == m_pRootTranslation)
		return E_FAIL;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Animations()))
		return E_FAIL;

	m_pModelCom->Hide_Mesh("LOD_1_Group_100_Sub_1__pl0001_Sling_Mat_mesh0006", true);
	m_pModelCom->Hide_Mesh("LOD_1_Group_200_Sub_1__pl0001_Gun_Mat_mesh0007", true);

	m_pModelCom->Add_IK("l_arm_radius", "l_weapon", TEXT("IK_SHOTGUN"), 3, 1.f);

	m_pModelCom->Set_OptimizationCulling(false);

	m_pModelCom->Set_RootBone("root");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));

	_uint			iNumBones = { static_cast<_uint>(m_pModelCom->Get_BoneNames().size()) };

	m_pModelCom->Add_Bone_Layer_ChildIndices(TEXT("Left_Arm"), "l_arm_clavicle");

	m_pModelCom->Add_Bone_Layer_ChildIndices(TEXT("Right_Arm"), "r_arm_clavicle");
	//m_pModelCom->Add_Bone_Layer_Bone(TEXT("Right_Arm"), "spine_2");
	//m_pModelCom->Add_Bone_Layer_Bone(TEXT("Right_Arm"), "spine_1");
	//m_pModelCom->Add_Bone_Layer_Bone(TEXT("Right_Arm"), "spine_0");

	m_pModelCom->Add_Bone_Layer_ChildIndices(TEXT("LowerBody"), "hips");
	m_pModelCom->Add_Bone_Layer_Bone(TEXT("LowerBody"), "root");
	m_pModelCom->Add_Bone_Layer_Bone(TEXT("LowerBody"), "spine_0");

	m_pModelCom->Add_Bone_Layer_ChildIndices(TEXT("UpperBody"), "spine_0");

#pragma region SHOT
	list<string>         L_ShoulderAddBoneTags;
	L_ShoulderAddBoneTags.emplace_back("COG");
	L_ShoulderAddBoneTags.emplace_back("hips");
	L_ShoulderAddBoneTags.emplace_back("l_leg_femur");
	L_ShoulderAddBoneTags.emplace_back("l_leg_tibia");
	L_ShoulderAddBoneTags.emplace_back("l_leg_ankle");
	L_ShoulderAddBoneTags.emplace_back("r_leg_femur");
	L_ShoulderAddBoneTags.emplace_back("r_leg_tibia");
	L_ShoulderAddBoneTags.emplace_back("r_leg_ankle");
	L_ShoulderAddBoneTags.emplace_back("spine_0");
	L_ShoulderAddBoneTags.emplace_back("spine_1");
	L_ShoulderAddBoneTags.emplace_back("spine_2");
	L_ShoulderAddBoneTags.emplace_back("l_arm_clavicle");
	L_ShoulderAddBoneTags.emplace_back("l_arm_humerus");
	//L_ShoulderAddBoneTags.emplace_back("l_arm_radius");
	L_ShoulderAddBoneTags.emplace_back("l_arm_wrist");
	L_ShoulderAddBoneTags.emplace_back("l_scapula_0");
	L_ShoulderAddBoneTags.emplace_back("r_arm_clavicle");
	L_ShoulderAddBoneTags.emplace_back("r_arm_humerus");
	L_ShoulderAddBoneTags.emplace_back("r_arm_radius");
	//L_ShoulderAddBoneTags.emplace_back("r_arm_wrist");
	L_ShoulderAddBoneTags.emplace_back("r_scapula_0");
	L_ShoulderAddBoneTags.emplace_back("neck_0");
	L_ShoulderAddBoneTags.emplace_back("neck_1");
	L_ShoulderAddBoneTags.emplace_back("head");

	
	L_ShoulderAddBoneTags.emplace_back("l_kneeProtector_start");
	L_ShoulderAddBoneTags.emplace_back("l_leg_tibia_twist_2_H");
	L_ShoulderAddBoneTags.emplace_back("l_leg_tibia_twist_1_H");
	L_ShoulderAddBoneTags.emplace_back("l_leg_femur_twist_2_H");
	L_ShoulderAddBoneTags.emplace_back("l_leg_femur_twist_1_H");
	L_ShoulderAddBoneTags.emplace_back("r_kneeProtector_start");
	L_ShoulderAddBoneTags.emplace_back("r_leg_tibia_twist_2_H");
	L_ShoulderAddBoneTags.emplace_back("r_leg_tibia_twist_1_H");
	L_ShoulderAddBoneTags.emplace_back("r_leg_femur_twist_2_H");
	L_ShoulderAddBoneTags.emplace_back("r_leg_femur_twist_1_H");
	L_ShoulderAddBoneTags.emplace_back("l_backVest_start");
	L_ShoulderAddBoneTags.emplace_back("r_backVest_start");
	L_ShoulderAddBoneTags.emplace_back("l_frontVest_start");
	L_ShoulderAddBoneTags.emplace_back("r_frontVest_start");

	L_ShoulderAddBoneTags.emplace_back("r_arm_radius_twist_3_H");
	L_ShoulderAddBoneTags.emplace_back("r_arm_radius_twist_2_H");
	L_ShoulderAddBoneTags.emplace_back("r_arm_radius_twist_1_H");
	L_ShoulderAddBoneTags.emplace_back("r_arm_humerus_twist_3_H");
	L_ShoulderAddBoneTags.emplace_back("r_arm_humerus_twist_2_H");
	L_ShoulderAddBoneTags.emplace_back("r_arm_humerus_twist_1_H");
	



	list<_uint>            L_ShoulderAddBoneIndices;
	for (auto& strBoneTag : L_ShoulderAddBoneTags)
	{
		_int         iBoneIndex = { m_pModelCom->Get_BoneIndex(strBoneTag) };
		if (iBoneIndex == -1)
			MSG_BOX(TEXT("BoneIndex - 1"));
		L_ShoulderAddBoneIndices.emplace_back(iBoneIndex);
	}
	m_pModelCom->Add_Bone_Layer_BoneIndices(TEXT("Shot"), L_ShoulderAddBoneIndices);



#pragma endregion

	m_pModelCom->Add_AnimPlayingInfo(true, 0, TEXT("Default"), 1.f);
	m_pModelCom->Add_AnimPlayingInfo(true, 1, TEXT("Default"), 0.f);
	m_pModelCom->Add_AnimPlayingInfo(false, 2, TEXT("Shot"), 0.f);
	m_pModelCom->Add_AnimPlayingInfo(true, 3, TEXT("UpperBody"), 0.f);
	m_pModelCom->Add_AnimPlayingInfo(true, 4, TEXT("Left_Arm"), 0.f);

	// 크기 월드 위치
	m_pModelCom->Set_TickPerSec(CPlayer::Get_AnimSetEtcName(CPlayer::COMMON), CPlayer::HOLD_LEFTHAND_LIGHT, .1f);
	
	m_bDecalRender = false;
	m_bCloth = true;
	m_bDecal_Player = true;

	m_pModelCom->Active_RootMotion_XZ(true);
	m_pModelCom->Active_RootMotion_Y(true);
	m_pModelCom->Active_RootMotion_Rotation(true);

	return S_OK;
}

void CBody_Player::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CBody_Player::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


}

void CBody_Player::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, m_pRootTranslation);

	if (false != m_bRender)
	{
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
	}
	

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom);
#endif
}

HRESULT CBody_Player::Render()
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

HRESULT CBody_Player::Render_LightDepth_Dir()
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
			if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_LIGHTDEPTH)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	return S_OK;
}

HRESULT CBody_Player::Render_LightDepth_Point()
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

			if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_LIGHTDEPTH_CUBE)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}

		++iIndex;
	}

	return S_OK;
}

HRESULT CBody_Player::Render_LightDepth_Spot()
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

			if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_LIGHTDEPTH)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	Update_WorldMatrix();

	return S_OK;
}

void CBody_Player::SetRagdoll(_int iId,_float4 vForce, COLLIDER_TYPE eType)
{
	m_pGameInstance->Start_Ragdoll(m_pRagdoll, iId);
	m_bRagdoll = true;
}

HRESULT CBody_Player::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_LeonBody"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = 0.5f;
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.fRadius, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;


	for (_uint i = 0; i < 100; ++i)
	{
		CBounding_OBB::BOUNDING_OBB_DESC		ColliderOBBDesc{};
		ColliderOBBDesc.vCenter = { 0.f, 0.f, 0.f };
		ColliderOBBDesc.vRotation = { 0.f, 0.f, 0.f };
		ColliderOBBDesc.vSize = { 10.f, 10.f, 10.f };

		CComponent* pCollider = { m_pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), &ColliderOBBDesc) };
		if (nullptr == pCollider)
			return E_FAIL;

		m_PartColliders.push_back(dynamic_cast<CCollider*>(pCollider));
	}


	return S_OK;
}

HRESULT CBody_Player::Add_Animations()
{
	if(FAILED(m_pModelCom->Add_Animations(TEXT("Player_Move_Fine"), CPlayer::Get_AnimSetMoveName(CPlayer::ANIMSET_MOVE::FINE))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Player_Move_Hg"), CPlayer::Get_AnimSetMoveName(CPlayer::ANIMSET_MOVE::MOVE_HG))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Player_Move_Stg"), CPlayer::Get_AnimSetMoveName(CPlayer::ANIMSET_MOVE::MOVE_STG))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Player_Move_Light"), CPlayer::Get_AnimSetMoveName(CPlayer::ANIMSET_MOVE::FINE_LIGHT))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Player_Move_Caution"), CPlayer::Get_AnimSetMoveName(CPlayer::ANIMSET_MOVE::CAUTION))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Player_Move_Caution_Light"), CPlayer::Get_AnimSetMoveName(CPlayer::ANIMSET_MOVE::CAUTION_LIGHT))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Player_Move_Danger"), CPlayer::Get_AnimSetMoveName(CPlayer::ANIMSET_MOVE::DANGER))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Player_Move_Danger_Light"), CPlayer::Get_AnimSetMoveName(CPlayer::ANIMSET_MOVE::DANGER_LIGHT))))
		return E_FAIL;

	if (FAILED(m_pModelCom->Add_Animations(TEXT("Player_Hold_Hg"), CPlayer::Get_AnimSetHoldName(CPlayer::ANIMSET_HOLD::HOLD_HG))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Player_Hold_Stg"), CPlayer::Get_AnimSetHoldName(CPlayer::ANIMSET_HOLD::HOLD_STG))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Player_Hold_Sup"), CPlayer::Get_AnimSetHoldName(CPlayer::ANIMSET_HOLD::HOLD_SUP))))
		return E_FAIL;

	if (FAILED(m_pModelCom->Add_Animations(TEXT("Player_Common"), CPlayer::Get_AnimSetEtcName(CPlayer::ANIMSET_ETC::COMMON))))
		return E_FAIL;

	if (FAILED(m_pModelCom->Add_Animations(TEXT("Player_Bite_Default"), TEXT("Bite_Default_Front"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Player_Bite_Default_Back"), TEXT("Bite_Default_Back"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Player_Bite_Creep"), TEXT("Bite_Creep"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Player_Bite_Push_Down"), TEXT("Bite_Push_Down"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Player_Bite_Lightly_Hold"), TEXT("Bite_Lightly_Hold"))))
		return E_FAIL;
	if (FAILED(m_pModelCom->Add_Animations(TEXT("Player_Bite_ETC"), TEXT("Bite_ETC"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Player::Bind_ShaderResources()
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

CBody_Player* CBody_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Player* pInstance = new CBody_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_Player"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBody_Player::Clone(void* pArg)
{
	CBody_Player* pInstance = new CBody_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_Player"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Player::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	for (auto& pCollider : m_PartColliders)
		Safe_Release(pCollider);
	m_PartColliders.clear();
}
