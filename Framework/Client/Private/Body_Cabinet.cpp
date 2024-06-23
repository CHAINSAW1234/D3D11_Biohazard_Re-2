#include "stdafx.h"
#include "Body_Cabinet.h"
#include"Player.h"
#include "PxCollider.h"
#include "Bone.h"

#include"Cabinet.h"

CBody_Cabinet::CBody_Cabinet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_InteractProps{ pDevice, pContext }
{
}

CBody_Cabinet::CBody_Cabinet(const CBody_Cabinet& rhs)
	: CPart_InteractProps{ rhs }
{

}

HRESULT CBody_Cabinet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Cabinet::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pModelCom->Set_RootBone("RootNode");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));

	m_pModelCom->Add_AnimPlayingInfo(false, 0, TEXT("Default"), 1.f);


	m_pModelCom->Active_RootMotion_Rotation(true);
	
#ifndef NON_COLLISION_PROP

	if (m_strModelComponentName.find(L"60_033") != string::npos)
	{
		//m_pPx_Collider = m_pGameInstance->Create_Px_Collider(m_pModelCom, m_pTransformCom, &m_iPx_Collider_Id);
	}
	else
	{
		if (m_strModelComponentName.find(L"44_002") != string::npos)
		{
			m_pPx_Collider = m_pGameInstance->Create_Px_Collider_Toilet(m_pModelCom, m_pTransformCom, &m_iPx_Collider_Id);
		}
		else
		{
			m_pPx_Collider = m_pGameInstance->Create_Px_Collider_Cabinet(m_pModelCom, m_pTransformCom, &m_iPx_Collider_Id);
		}
	}

	m_vecRotationBone[ANIM_BONE_TYPE_COLLIDER_CABINET::ATC_CABINET_DOOR] = m_pModelCom->Get_BonePtr("_01");

#endif
	return S_OK;
}

void CBody_Cabinet::Tick(_float fTimeDelta)
{
	if (*m_pState == CCabinet::CABINET_OPEN && m_pModelCom->isFinished(0))
		return;
}

void CBody_Cabinet::Late_Tick(_float fTimeDelta)
{
	Check_Col_Sphere_Player(); // 여긴 m_bCol 을 true로만 바꿔주기 때문에 반드시 false를 해주는 부분이 있어야함

	switch (*m_pState)
	{
	case CCabinet::CABINET_CLOSED:
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pState);
		break;
	case CCabinet::CABINET_OPEN:
	{
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pState);

		if (m_pPx_Collider && m_vecRotationBone[FIRE_WALL_ROTATE_BONE_TYPE::DOOR])
		{
			auto Combined = m_vecRotationBone[FIRE_WALL_ROTATE_BONE_TYPE::DOOR]->Get_TrasformationMatrix();
			_float4x4 ResultMat;
			XMStoreFloat4x4(&ResultMat, Combined);
			m_pPx_Collider->Update_Transform_Cabinet(&ResultMat);
		}

		break;
	}
	case CCabinet::CABINET_OPENED:
		m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pState);
		break;
	}

	_float4 fTransform4 = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
	m_pModelCom->Play_Animation_Light(m_pTransformCom, fTimeDelta);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

}

HRESULT CBody_Cabinet::Render()
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

HRESULT CBody_Cabinet::Add_Components()
{

	/* For.Com_Body_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Body_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Body_Model */
	if (FAILED(__super::Add_Component(g_Level, m_strModelComponentName,
		TEXT("Com_Body_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBody_Cabinet::Add_PartObjects()
{

	return S_OK;
}

HRESULT CBody_Cabinet::Initialize_PartObjects()
{

	return S_OK;
}

CBody_Cabinet* CBody_Cabinet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Cabinet* pInstance = new CBody_Cabinet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_Cabinet"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CBody_Cabinet::Clone(void* pArg)
{
	CBody_Cabinet* pInstance = new CBody_Cabinet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_Cabinet"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Cabinet::Free()
{
	__super::Free();

}
