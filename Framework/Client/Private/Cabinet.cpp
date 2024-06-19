#include "stdafx.h"
#include "Cabinet.h"
#include"Player.h"
#include "PxCollider.h"
#include "Bone.h"

CCabinet::CCabinet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInteractProps{ pDevice, pContext }
{
}

CCabinet::CCabinet(const CCabinet& rhs)
	: CInteractProps{ rhs }
{

}

HRESULT CCabinet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCabinet::Initialize(void* pArg)
{
	/*문자식 파트오브젝트 붙혀야하는데 뼈가 문고리에 없어서 직접 찍어야 하는데
	프로토타입 끝나고 뼈 붙혀보겠나이다*/

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;


	m_pModelCom->Set_RootBone("RootNode");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));

	m_pModelCom->Add_AnimPlayingInfo(0, false, 0, TEXT("Default"), 1.f);


	m_pModelCom->Active_RootMotion_Rotation(true);
	m_pTransformCom->Set_WorldMatrix(m_tagPropDesc.worldMatrix);

	//m_pPx_Collider = m_pGameInstance->Create_Px_Collider(m_pModelCom, m_pTransformCom, &m_iPx_Collider_Id);

	m_pRotationBone = m_pModelCom->Get_BonePtr("_00");

	return S_OK;
}

void CCabinet::Tick(_float fTimeDelta)
{
	if (m_eState == CABINET_OPEN && m_pModelCom->isFinished(0))
		return;

	__super::Check_Player();

	if (m_pPlayer == nullptr)
		return;

	if (m_bCol&& !m_bActive)
	{
		//UI띄우고
		if (*m_pPlayerInteract)
			Active();
		m_bCol = false;
	}

	//auto Combined = XMLoadFloat4x4(m_pRotationBone->Get_CombinedTransformationMatrix());
	//Combined = /*XMMatrixRotationY(-PxPi) **/ Combined * m_pTransformCom->Get_WorldMatrix();
	//_float4x4 ResultMat;
	//XMStoreFloat4x4(&ResultMat, Combined);
	//m_pPx_Collider->Update_Transform(&ResultMat);
}

void CCabinet::Late_Tick(_float fTimeDelta)
{

	if (m_bRender == false)
		return;

	Check_Col_Sphere_Player(); // 여긴 m_bCol 을 true로만 바꿔주기 때문에 반드시 false를 해주는 부분이 있어야함

	switch (m_eState)
	{
	case CABINET_CLOSED:
		m_pModelCom->Change_Animation(0, m_eState);
		break;
	case CABINET_OPEN:
		m_pModelCom->Change_Animation(0, m_eState);
		break;
	case CABINET_OPENED:
		m_pModelCom->Change_Animation(0, m_eState);
		break;
	}

	_float4 fTransform4 = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
	m_pModelCom->Play_Animations(m_pTransformCom, fTimeDelta, &fTransform3);


	m_pColliderCom[INTERACTPROPS_COL_SPHERE]->Tick(m_pTransformCom->Get_WorldMatrix());

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponents(m_pColliderCom[INTERACTPROPS_COL_SPHERE]);
#endif
}

HRESULT CCabinet::Render()
{
	if (m_bRender == false)
		return S_OK;
	else
		m_bRender = false;


	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderResource_Texture(m_pShaderCom, "g_NormalTexture", static_cast<_uint>(i), aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", static_cast<_uint>(i))))
			return E_FAIL;

		/*if (FAILED(m_pModelCom->Bind_PrevBoneMatrices(m_pShaderCom, "g_PrevBoneMatrices", static_cast<_uint>(i))))
			return E_FAIL;*/

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

HRESULT CCabinet::Add_Components()
{

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(g_Level, m_tagPropDesc.strModelComponent,
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	
	CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	ColliderDesc.fRadius = _float(50.f);
	ColliderDesc.vCenter = _float3(50.f, 1.f, 0.f);
	/* For.Com_Collider */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), (CComponent**)&m_pColliderCom[INTERACTPROPS_COL_SPHERE], &ColliderDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CCabinet::Add_PartObjects()
{

	return S_OK;
}

HRESULT CCabinet::Initialize_PartObjects()
{

	return S_OK;
}

HRESULT CCabinet::Bind_ShaderResources()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_bool isMotionBlur = m_pGameInstance->Get_ShaderState(MOTION_BLUR);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isMotionBlur", &isMotionBlur, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", &m_pTransformCom->Get_WorldFloat4x4())))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevViewMatrix", &m_pGameInstance->Get_PrevTransform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevProjMatrix", &m_pGameInstance->Get_PrevTransform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

void CCabinet::Active()
{
	*m_pPlayerInteract = false;
	m_bActive = true;
	m_eState = CABINET_OPEN;




}

CCabinet* CCabinet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCabinet* pInstance = new CCabinet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCabinet"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CCabinet::Clone(void* pArg)
{
	CCabinet* pInstance = new CCabinet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CCabinet"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCabinet::Free()
{
	__super::Free();

}
