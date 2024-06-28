#include "stdafx.h"
#include "..\Public\Muzzle_Flash.h"

#include "GameInstance.h"

CMuzzle_Flash::CMuzzle_Flash(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect{ pDevice, pContext }
{
}

CMuzzle_Flash::CMuzzle_Flash(const CMuzzle_Flash & rhs)
	: CEffect{ rhs }
{
}

HRESULT CMuzzle_Flash::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMuzzle_Flash::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(static_cast<_float>(rand() % 10), 3.f, static_cast<_float>(rand() % 10), 1.f));


	return S_OK;
}

void CMuzzle_Flash::Tick(_float fTimeDelta)
{
	++m_iFrame;

	if (m_iFrame >= 2)
		m_iFrame = 0;

	Compute_CurrentUV();
}

void CMuzzle_Flash::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CMuzzle_Flash::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CMuzzle_Flash::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Muzzle_Flash"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMuzzle_Flash::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMinUV_X", &m_fMinUV_X,sizeof(m_fMinUV_X))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMinUV_Y", &m_fMinUV_Y,sizeof(m_fMinUV_Y))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaxUV_X", &m_fMaxUV_X,sizeof(m_fMaxUV_X))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fMaxUV_Y", &m_fMaxUV_Y, sizeof(m_fMaxUV_Y))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", (_uint)m_fFrame)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RTShaderResource(m_pShaderCom, TEXT("Target_Depth"), "g_DepthTexture")))
		return E_FAIL;

	return S_OK;
}

CMuzzle_Flash * CMuzzle_Flash::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMuzzle_Flash*		pInstance = new CMuzzle_Flash(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMuzzle_Flash"));

		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CMuzzle_Flash::Clone(void * pArg)
{
	CMuzzle_Flash*		pInstance = new CMuzzle_Flash(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CMuzzle_Flash"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMuzzle_Flash::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
