#include "stdafx.h"
#include "Body_Window.h"
#include"Player.h"

#include"Window.h"
CBody_Window::CBody_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_InteractProps{ pDevice, pContext }
{
}

CBody_Window::CBody_Window(const CBody_Window& rhs)
	: CPart_InteractProps{ rhs }
{

}

HRESULT CBody_Window::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Window::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	if (FAILED(Add_Components()))
		return E_FAIL;


	m_pModelCom->Set_RootBone("RootNode");
	m_pModelCom->Add_Bone_Layer_All_Bone(TEXT("Default"));

	m_pModelCom->Add_AnimPlayingInfo(0, false, 0, TEXT("Default"), 1.f);


	m_pModelCom->Active_RootMotion_Rotation(true);

#ifndef NON_COLLISION_PROP

	m_pGameInstance->Create_Px_Collider(m_pModelCom, m_pTransformCom, &m_iPx_Collider_Id);

#endif

	return S_OK;
}

void CBody_Window::Tick(_float fTimeDelta)
{
}

void CBody_Window::Late_Tick(_float fTimeDelta)
{
	switch (*m_pState)
	{
	case CWindow::WINDOW_STATIC:
		//m_pModelCom->Set_TotalLinearInterpolation(0.2f); // �߾˾ư��ϴ� ����
		m_pModelCom->Change_Animation(0, *m_pState);
		break;
	case CWindow::WINDOW_BREAK:
		m_pModelCom->Change_Animation(0, *m_pState);
		break;
	}
	_float4 fTransform4 = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
	_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
	m_pModelCom->Play_Animation_Light(m_pTransformCom, fTimeDelta);

	//m_pColliderCom[Part_INTERACTPROPS_COL_SPHERE]->Tick(m_pTransformCom->Get_WorldMatrix());

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

}

HRESULT CBody_Window::Render()
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

HRESULT CBody_Window::Add_Components()
{
	/* For.Com_Body_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Body_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Body_Model */
	if (FAILED(__super::Add_Component(g_Level, m_strModelComponentName,
		TEXT("Com_Body_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	
	//CBounding_Sphere::BOUNDING_SPHERE_DESC		ColliderDesc{};

	//ColliderDesc.fRadius = _float(100.f);
	//ColliderDesc.vCenter = _float3(-10.f, 1.f, 0.f);
	///* For.Com_Collider */
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
	//	TEXT("Com_Body_Collider"), (CComponent**)&m_pColliderCom[Part_INTERACTPROPS_COL_SPHERE], &ColliderDesc)))
	//	return E_FAIL;
	return S_OK;
}

HRESULT CBody_Window::Add_PartObjects()
{

	return S_OK;
}

HRESULT CBody_Window::Initialize_PartObjects()
{
	return S_OK;
}


CBody_Window* CBody_Window::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBody_Window* pInstance = new CBody_Window(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_Window"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CBody_Window::Clone(void* pArg)
{
	CBody_Window* pInstance = new CBody_Window(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CBody_Window"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Window::Free()
{
	__super::Free();

}
