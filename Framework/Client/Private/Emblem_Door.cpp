#include "stdafx.h"
#include "Emblem_Door.h"
#include "Player.h"
#include "Camera_Gimmick.h"
#include "Light.h"

#include"Door.h"
CEmblem_Door::CEmblem_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_InteractProps{ pDevice, pContext }
{
}

CEmblem_Door::CEmblem_Door(const CEmblem_Door& rhs)
	: CPart_InteractProps{ rhs }
{

}

HRESULT CEmblem_Door::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEmblem_Door::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		BODY_EMBLEM_DOOR* desc = (BODY_EMBLEM_DOOR*)pArg;

		m_pEmblem_Anim = desc->EmblemAnim;

		m_eEmblemType = desc->eEmblemType;
		m_pDoorState = desc->pDoorState;
		m_pKeyUsing = desc->pKeyUsing;
	}

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_RootBone("RootNode");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));
	m_pModelCom->Add_AnimPlayingInfo(false, 0, TEXT("Default"), 1.f);
	m_pModelCom->Active_RootMotion_Rotation(false);

#ifndef NON_COLLISION_PROP
	//m_pGameInstance->Create_Px_Collider(m_pModelCom, m_pParentsTransform, &m_iPx_Collider_Id);
#endif

	return S_OK;
}

void CEmblem_Door::Tick(_float fTimeDelta)
{
	
}

void CEmblem_Door::Late_Tick(_float fTimeDelta)
{
	_int iRand = m_pGameInstance->GetRandom_Int(0,1);
	if (m_bDead)
		return;
	switch (*m_pEmblem_Anim)
	{
	case (_int)EMBLEM_ANIM::STATIC_ANIM:
		//
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pEmblem_Anim);
		

		break;

	case (_int)EMBLEM_ANIM::START_ANIM : 
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pEmblem_Anim);
		//if ((!*m_pSoundCue) && m_pModelCom->isFinished(0) != true)
		//	*m_pSoundCue = true;
		//sound_Map_sm40_door_handle2_1
		
		if (!m_pModelCom->isFinished(0)&&!m_pGameInstance->Is_Playing_Sound(m_pParentsTransform,0))
			Change_Sound(TEXT("sound_Map_sm40_door_handle2_1.mp3"), 0);
		
		break;
		/* LN : 키 클리어 : sound_Map_sm40_conveni_keyhole2_2 
		(emblem이 두번 돌아가는데 소리는 한 번만 나오고 총 두번 나와야 함)*/

	case (_int)EMBLEM_ANIM::OPEN_ANIM:
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pEmblem_Anim);
		if (m_pModelCom->isFinished(0))
			*m_pEmblem_Anim = (_int)EMBLEM_ANIM::OPENED_ANIM;

		/* LN : 키 클리어 : sound_Map_sm40_conveni_keyhole2_4 */
		//if ((!*m_pSoundCue) && m_pModelCom->isFinished(0) != true)
		//	*m_pSoundCue = true;
		if (!m_pModelCom->isFinished(0)&& m_pModelCom->Get_TrackPosition(0))
			Change_Sound(TEXT("sound_Map_sm40_conveni_keyhole2_4.mp3"), 0);

			//if (m_pModelCom->Get_TrackPosition(0) >= )
		//sound_Map_sm40_conveni_keyhole2_4
		break;
	case (_int)EMBLEM_ANIM::OPENED_ANIM:
		m_bClear = true;
		break;	
	}
	m_pTransformCom->Rotation(m_pTransformCom->Get_State_Vector(CTransform::STATE_UP), XMConvertToRadians(360.f));

	_matrix			mWorldMatrix = m_pTransformCom->Get_WorldMatrix();
	mWorldMatrix.r[3] += _float4(95.f, 95.f, -5.f, 0.f);

	_matrix			WorldMatrix = { };
	switch (*m_pDoorState)
	{
	case CDoor::ONEDOOR_OPEN_L:
		WorldMatrix = { mWorldMatrix * XMLoadFloat4x4(m_pSocketMatrix_01) * (m_pParentsTransform->Get_WorldMatrix()) };
		XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
		break;
	case CDoor::ONEDOOR_OPEN_R:
		WorldMatrix = { mWorldMatrix * XMLoadFloat4x4(m_pSocketMatrix_01) * (m_pParentsTransform->Get_WorldMatrix()) };
		XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
		break;
	case CDoor::ONEDOOR_STATIC:
		WorldMatrix = { mWorldMatrix * XMLoadFloat4x4(m_pSocketMatrix_01) * (m_pParentsTransform->Get_WorldMatrix()) };
		XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
		break;
	}

	_float3	vDirection = { };
	_float4 fTransform4 = m_pParentsTransform->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };

	m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, 0);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
}

HRESULT CEmblem_Door::Render()
{
	if (m_bRender == false)
		return S_OK;
	else
		m_bRender = false;

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

		if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_ALPHABLEND)))
			return E_FAIL;

		m_pModelCom->Render(static_cast<_uint>(i));
	}
	
	m_pTransformCom->Rotation(m_pTransformCom->Get_State_Vector(CTransform::STATE_UP),XMConvertToRadians(180.f));
	_matrix			mWorldMatrix = m_pTransformCom->Get_WorldMatrix();
	
	mWorldMatrix.r[3] += _float4(95.f, 95.f, 0.f, 0.f);

	_matrix			WorldMatrix = { };
	switch (*m_pDoorState)
	{
	case CDoor::ONEDOOR_OPEN_L:
		WorldMatrix = { mWorldMatrix * XMLoadFloat4x4(m_pSocketMatrix_01) * (m_pParentsTransform->Get_WorldMatrix()) };
		m_WorldMatrix = WorldMatrix;
		break;
	case CDoor::ONEDOOR_OPEN_R:
		WorldMatrix = { mWorldMatrix * XMLoadFloat4x4(m_pSocketMatrix_01) * (m_pParentsTransform->Get_WorldMatrix()) };
		m_WorldMatrix = WorldMatrix;
		break;
	case CDoor::ONEDOOR_STATIC:
		WorldMatrix = { mWorldMatrix * XMLoadFloat4x4(m_pSocketMatrix_01) * (m_pParentsTransform->Get_WorldMatrix()) };
		m_WorldMatrix = WorldMatrix;
		break;
	}


	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	NonHideIndices = { m_pModelCom->Get_NonHideMeshIndices() };

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

		if (FAILED(m_pShaderCom->Begin((_uint)SHADER_PASS_VTXANIMMODEL::PASS_ALPHABLEND)))
			return E_FAIL;

		m_pModelCom->Render(static_cast<_uint>(i));
	}


	return S_OK;
}

HRESULT CEmblem_Door::Render_LightDepth_Dir()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
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

HRESULT CEmblem_Door::Render_LightDepth_Point()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
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

HRESULT CEmblem_Door::Render_LightDepth_Spot()
{
	if (m_bRender == false)
		return S_OK;

	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
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

HRESULT CEmblem_Door::Add_Components()
{
	/* For.Com_Body_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Body_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Body_Model */
	if (FAILED(__super::Add_Component(g_Level, m_strModelComponentName,
		TEXT("Com_Body_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
#ifdef _DEBUG
#ifdef UI_POS
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(20.f);
	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Body_Collider"), (CComponent**)&m_pColliderCom[Part_INTERACTPROPS_COL_SPHERE], &ColliderDesc)))
		return E_FAIL;
#endif
#endif
	return S_OK;
}

HRESULT CEmblem_Door::Add_PartObjects()
{
	return S_OK;
}

HRESULT CEmblem_Door::Initialize_PartObjects()
{
	return S_OK;
}

CEmblem_Door* CEmblem_Door::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEmblem_Door* pInstance = new CEmblem_Door(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEmblem_Door"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CEmblem_Door::Clone(void* pArg)
{
	CEmblem_Door* pInstance = new CEmblem_Door(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CEmblem_Door"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEmblem_Door::Free()
{
	__super::Free();

}
