#include "stdafx.h"
#include "..\Public\Muzzle_Light_SG.h"

#include "GameInstance.h"

CMuzzle_Light_SG::CMuzzle_Light_SG(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect{ pDevice, pContext }
{
}

CMuzzle_Light_SG::CMuzzle_Light_SG(const CMuzzle_Light_SG & rhs)
	: CEffect{ rhs }
{
}

HRESULT CMuzzle_Light_SG::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMuzzle_Light_SG::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_bRender = false;

	return S_OK;
}

void CMuzzle_Light_SG::Tick(_float fTimeDelta)
{
	if (m_bRender == false)
		return;

	Setup_Billboard();
}

void CMuzzle_Light_SG::Late_Tick(_float fTimeDelta)
{
	if(m_bRender == true)
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_EFFECT_BLOOM, this);
}

HRESULT CMuzzle_Light_SG::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if(m_iType == 0)
		m_pShaderCom->Begin(6);
	else
		m_pShaderCom->Begin(9);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

void CMuzzle_Light_SG::Setup_Billboard()
{
	m_pTransformCom->Look_At(m_pGameInstance->Get_CamPosition_Vector());
}

void CMuzzle_Light_SG::SetSize(_float fSizeX, _float fSizeY)
{
	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;

	m_pTransformCom->Set_Scaled(fSizeX, fSizeY,1.f);
}

void CMuzzle_Light_SG::SetPosition(_float4 Pos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Pos);
	m_iFrame = 0;
	m_fAlpha_Delta_Sum = 0.f;
}

HRESULT CMuzzle_Light_SG::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Muzzle_Light_SG"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMuzzle_Light_SG::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (m_iType == 1)
	{
		if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShaderCom, TEXT("Target_Field_Depth"), "g_DepthTexture")))
			return E_FAIL;
	}

	return S_OK;
}

CMuzzle_Light_SG * CMuzzle_Light_SG::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMuzzle_Light_SG*		pInstance = new CMuzzle_Light_SG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMuzzle_Light_SG"));

		Safe_Release(pInstance);
	}

	pInstance->Initialize(nullptr);

	return pInstance;

}

CGameObject * CMuzzle_Light_SG::Clone(void * pArg)
{
	CMuzzle_Light_SG*		pInstance = new CMuzzle_Light_SG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CMuzzle_Light_SG"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMuzzle_Light_SG::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
