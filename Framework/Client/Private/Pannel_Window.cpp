#include "stdafx.h"
#include "Pannel_Window.h"
#include"Player.h"

#include"Window.h"
CPannel_Window::CPannel_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPart_InteractProps{ pDevice, pContext }
{
}

CPannel_Window::CPannel_Window(const CPannel_Window& rhs)
	: CPart_InteractProps{ rhs }
{

}

HRESULT CPannel_Window::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPannel_Window::Initialize(void* pArg)
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

	//m_pGameInstance->Create_Px_Collider(m_pModelCom, m_pParentsTransform, &m_iPx_Collider_Id);

#endif

	return S_OK;
}

void CPannel_Window::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CPannel_Window::Late_Tick(_float fTimeDelta)
{
	if (*m_pState == CWindow::BARRIGATE_NEW|| *m_pState == CWindow::BARRIGATE_NO)
	{
		;
	}
	else
	{
		switch (*m_pState)
		{
		case CWindow::BARRIGATE_BREAK:
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pState);
			break;
		case CWindow::BARRIGATE_STATIC:
			m_pModelCom->Change_Animation(0, TEXT("Default"), *m_pState);
			break;
		//case CWindow::BARRIGATE_NO:
		//	m_pModelCom->Change_Animation(0, TEXT("Default"), CWindow::BARRIGATE_STATIC);
		//	break;
		}
		_float4 fTransform4 = m_pParentsTransform->Get_State_Float4(CTransform::STATE_POSITION);
		_float3 fTransform3 = _float3{ fTransform4.x,fTransform4.y,fTransform4.z };
		m_pModelCom->Play_Animation_Light(m_pParentsTransform, fTimeDelta);
	}
	

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_POINT, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_FIELD_SHADOW_DIR, this);
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW_SPOT, this);

}

HRESULT CPannel_Window::Render()
{
	if (*m_pState == CWindow::BARRIGATE_NO)
		return S_OK;
	if (m_bRender == false)
		return S_OK;
	else
		m_bRender = false;




	if (*m_pState == CWindow::BARRIGATE_NEW)
	{
		if (nullptr == m_pShaderNonAnimCom)
			return E_FAIL;

		if (FAILED(m_pShaderNonAnimCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
			return E_FAIL;

		if (FAILED(m_pShaderNonAnimCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
			return E_FAIL;
		if (FAILED(m_pShaderNonAnimCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
			return E_FAIL;
		if (FAILED(m_pShaderNonAnimCom->Bind_RawValue("g_DecalRender", &m_bDecalRender, sizeof(_bool))))
			return E_FAIL;

		_uint iNumMeshes = m_pModelNonAnimCom->Get_NumMeshes();

		for (size_t i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelNonAnimCom->Bind_ShaderResource_Texture(m_pShaderNonAnimCom, "g_DiffuseTexture", static_cast<_uint>(i), aiTextureType_DIFFUSE)))
				return E_FAIL;

			if (FAILED(m_pModelNonAnimCom->Bind_ShaderResource_Texture(m_pShaderNonAnimCom, "g_NormalTexture", static_cast<_uint>(i), aiTextureType_NORMALS)))
				return E_FAIL;

			if (FAILED(m_pModelNonAnimCom->Bind_ShaderResource_Texture(m_pShaderNonAnimCom, "g_AlphaTexture", static_cast<_uint>(i), aiTextureType_METALNESS)))
			{
				_bool isAlphaTexture = false;
				if (FAILED(m_pShaderNonAnimCom->Bind_RawValue("g_isAlphaTexture", &isAlphaTexture, sizeof(_bool))))
					return E_FAIL;
			}
			else
			{
				_bool isAlphaTexture = true;
				if (FAILED(m_pShaderNonAnimCom->Bind_RawValue("g_isAlphaTexture", &isAlphaTexture, sizeof(_bool))))
					return E_FAIL;
			}

			if (FAILED(m_pModelNonAnimCom->Bind_ShaderResource_Texture(m_pShaderNonAnimCom, "g_AOTexture", static_cast<_uint>(i), aiTextureType_SHININESS)))
			{
				_bool isAOTexture = false;
				if (FAILED(m_pShaderNonAnimCom->Bind_RawValue("g_isAOTexture", &isAOTexture, sizeof(_bool))))
					return E_FAIL;
			}
			else
			{
				_bool isAOTexture = true;
				if (FAILED(m_pShaderNonAnimCom->Bind_RawValue("g_isAOTexture", &isAOTexture, sizeof(_bool))))
					return E_FAIL;
			}


			if (FAILED(m_pShaderNonAnimCom->Begin(0)))
				return E_FAIL;

			m_pModelNonAnimCom->Render(static_cast<_uint>(i));
		}
	}
	else
	{
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
	}
	

	return S_OK;
}

HRESULT CPannel_Window::Add_Components()
{
	/* For.Com_Pannel_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		TEXT("Com_Pannel_NONAnim_Shader"), (CComponent**)&m_pShaderNonAnimCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Pannel_Anim_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Pannel_Model */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_sm40_034_windowbarricade01a"),
		TEXT("Com_Pannel_Model_NonAnim"), (CComponent**)&m_pModelNonAnimCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Model_sm40_035_windowoldbarricade01a"),
		TEXT("Com_Pannel_Model_Anim"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPannel_Window::Add_PartObjects()
{

	return S_OK;
}

HRESULT CPannel_Window::Initialize_PartObjects()
{
	return S_OK;
}


CPannel_Window* CPannel_Window::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPannel_Window* pInstance = new CPannel_Window(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPannel_Window"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject* CPannel_Window::Clone(void* pArg)
{
	CPannel_Window* pInstance = new CPannel_Window(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CPannel_Window"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPannel_Window::Free()
{
	__super::Free();
	Safe_Release(m_pModelNonAnimCom);
	Safe_Release(m_pShaderNonAnimCom);
}
