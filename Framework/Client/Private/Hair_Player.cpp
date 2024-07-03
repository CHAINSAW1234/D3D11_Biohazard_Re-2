#include "stdafx.h"
#include "Hair_Player.h"

#include "Player.h"
#include "Light.h"

CHair_Player::CHair_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject{ pDevice, pContext }
{
}

CHair_Player::CHair_Player(const CHair_Player& rhs)
	: CPartObject{ rhs }
{

}

HRESULT CHair_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHair_Player::Initialize(void* pArg)
{
	HAIR_DESC*		pDesc = { static_cast<HAIR_DESC*>(pArg) };

	pDesc->fSpeedPerSec = 10.f;
	pDesc->fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	_uint		iNumBones = { static_cast<_uint>(m_pModelCom->Get_BoneNames().size()) };
	list<_uint>		iBoneIndices;
	for (_uint i = 0; i < iNumBones; ++i)
	{
		iBoneIndices.emplace_back(i);
	}

	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));
	m_pModelCom->Add_AnimPlayingInfo(true, 0, TEXT("Default"), 1.f);
	m_pModelCom->Set_RootBone("RootNode");
	m_pModelCom->Change_Animation(0, TEXT("Default"), 0);

	vector<string>		BoneTags = { m_pModelCom->Get_BoneNames() };
	 
	m_bDecalRender = false;

	return S_OK;
}

void CHair_Player::Priority_Tick(_float fTimeDelta)
{
	__super::Priority_Tick(fTimeDelta);
}

void CHair_Player::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));
}

void CHair_Player::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	static _float		fAdditionalHeight = { 0.f };
	static _float		fAdditionalAngle = { 0.f };
	list<string>		BoneTags;
	BoneTags.push_back("hair01_1");
	BoneTags.push_back("hair02_1");
	BoneTags.push_back("hair03_1");
	BoneTags.push_back("hair04_1");
	BoneTags.push_back("hair05_1");
	BoneTags.push_back("hair06_1");
	BoneTags.push_back("hair07_1");
	BoneTags.push_back("hair08_1");
	BoneTags.push_back("hair09_1");
	BoneTags.push_back("hair01_0");
	BoneTags.push_back("hair02_0");
	BoneTags.push_back("hair03_0");
	BoneTags.push_back("hair04_0");
	BoneTags.push_back("hair05_0");
	BoneTags.push_back("hair06_0");
	BoneTags.push_back("hair07_0");
	BoneTags.push_back("hair08_0");
	BoneTags.push_back("hair09_0");

	if (PRESSING == m_pGameInstance->Get_KeyState(VK_LEFT))
	{
		fAdditionalHeight -= 10.f * fTimeDelta;
		//	fAdditionalAngle -= 10.f * fTimeDelta;
	}

	if (PRESSING == m_pGameInstance->Get_KeyState(VK_RIGHT))
	{
		fAdditionalHeight += 10.f * fTimeDelta;
		//	fAdditionalAngle += 10.f * fTimeDelta;
	}

	static _bool		isRotationInverse = { false };
	static _bool		isTranslationInverse = { false };
	if (true == isRotationInverse)
	{
		fAdditionalAngle -= 15.f * fTimeDelta;

		if (fAdditionalAngle < -10.f)
		{
			fAdditionalAngle = -10.f;
			isRotationInverse = false;
		}
	}
	else
	{
		fAdditionalAngle += 15.f * fTimeDelta;

		if (fAdditionalAngle > 10.f)
		{
			fAdditionalAngle = 10.f;
			isRotationInverse = true;
		}
	}

	if (true == isTranslationInverse)
	{
		fAdditionalHeight -= 0.6f * fTimeDelta;

		if (fAdditionalHeight < -0.3f)
		{
			fAdditionalHeight = -0.3f;
			isTranslationInverse = false;
		}
	}

	else
	{
		fAdditionalHeight += 0.6f * fTimeDelta;

		if (fAdditionalHeight > 0.3f)
		{
			fAdditionalHeight = 0.3f;
			isTranslationInverse = true;
		}
	}

	_matrix			TranslationMatrix = { XMMatrixTranslation(0.f, fAdditionalHeight, 0.f) };
	_matrix			RotaionMatrix = { XMMatrixRotationAxis(m_pTransformCom->Get_State_Vector(CTransform::STATE_UP), XMConvertToRadians(fAdditionalAngle)) };
	_matrix			WorldMatrix = { m_pTransformCom->Get_WorldMatrix() };

	//	_matrix			TransformationMatrix = { RotaionMatrix * TranslationMatrix * WorldMatrix };
	_matrix			HeadBoneCombinedMatrix = { XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix("head"))};
	_matrix			HeadBoneWorldMatrix = { HeadBoneCombinedMatrix * WorldMatrix };

	_float4x4		IdentityFloat4x4;
	XMStoreFloat4x4(&IdentityFloat4x4, HeadBoneWorldMatrix);
	static _float4x4		HeadBonePreTransformMatrix = { IdentityFloat4x4 };

	_vector			vCurrentScale, vCurrentQuaternion, vCurrentTranslation;
	XMMatrixDecompose(&vCurrentScale, &vCurrentQuaternion, &vCurrentTranslation, HeadBoneWorldMatrix);

	_vector			vPreScale, vPreQuaternion, vPreTranslation;
	XMMatrixDecompose(&vPreScale, &vPreQuaternion, &vPreTranslation, XMLoadFloat4x4(&HeadBonePreTransformMatrix));

	//	_vector			vDeltaScale = { vCurrentScale - vPreScale };
	_vector			vDeltaScale = { vCurrentScale };
	_vector			vDeltaQuaternion = { XMQuaternionMultiply(XMQuaternionNormalize(vCurrentQuaternion), XMQuaternionInverse(XMQuaternionNormalize(vPreQuaternion))) };
	//	_vector			vDeltaQuaternion = { XMQuaternionIdentity()};
	_vector			vDeltaTranslation = { XMVectorSetW(vCurrentTranslation - vPreTranslation, 1.f) };
	vDeltaTranslation *= 1.5f;

	if (XMVectorGetX(XMVector3Length(vDeltaTranslation)) > 1.5f)
	{
		vDeltaTranslation = XMVectorSetW(XMVector3Normalize(vDeltaTranslation), 1.f) * 1.5f;
	}

	_matrix			DeltaMatrix = { XMMatrixAffineTransformation(vDeltaScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vDeltaQuaternion, vDeltaTranslation) };

	XMStoreFloat4x4(&HeadBonePreTransformMatrix, HeadBoneCombinedMatrix);

	for (auto& strBoneTag : BoneTags)
	{
		_matrix			CombinedMatrix = { XMLoadFloat4x4(m_pModelCom->Get_CombinedMatrix(strBoneTag)) };

		m_pModelCom->Add_Additional_Transformation_World(strBoneTag, DeltaMatrix);
	}

	_float3		vMoveDir = {};
	m_pModelCom->Play_Animations(m_pParentsTransform, fTimeDelta, &vMoveDir);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_POINT, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom);
#endif
}

HRESULT CHair_Player::Render()
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

HRESULT CHair_Player::Render_LightDepth_Dir()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (m_bRagdoll)
	{
		if (m_bRagdoll_Ready == true)
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

			if (FAILED(m_pShaderCom->Begin(2)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	return S_OK;
}

HRESULT CHair_Player::Render_LightDepth_Point()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (m_bRagdoll)
	{
		if (m_bRagdoll_Ready == true)
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
			m_bRagdoll_Ready = true;

			if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
				return E_FAIL;
		}
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

HRESULT CHair_Player::Render_LightDepth_Spot()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (m_bRagdoll)
	{
		if (m_bRagdoll_Ready == true)
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

			/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
			if (FAILED(m_pShaderCom->Begin(2)))
				return E_FAIL;

			m_pModelCom->Render(static_cast<_uint>(i));
		}
	}

	Update_WorldMatrix();

	return S_OK;
}

HRESULT CHair_Player::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_LeonHair"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* Com_Collider */
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = 0.5f;
	ColliderDesc.vCenter = _float3(0.f, ColliderDesc.fRadius, 0.f);


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHair_Player::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	_bool isMotionBlur = m_pGameInstance->Get_ShaderState(MOTION_BLUR);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isMotionBlur", &isMotionBlur, sizeof(_bool))))
		return E_FAIL;

	if (m_bRagdoll)
	{
		if (m_bRagdoll_Ready == true)
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

	return S_OK;
}

CHair_Player* CHair_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHair_Player* pInstance = new CHair_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CHair_Player"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CHair_Player::Clone(void* pArg)
{
	CHair_Player* pInstance = new CHair_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHair_Player"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHair_Player::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
