#include "stdafx.h"

#include "Hint_Display.h"


CHint_Display::CHint_Display(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice , pContext }
{
}

CHint_Display::CHint_Display(const CHint_Display& rhs)
	: CUI{ rhs }
{
}

HRESULT CHint_Display::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHint_Display::Initialize(void* pArg)
{
	CUI::UI_DESC UIDesc = {};
	UIDesc.vPos = { 121.f, 197.f, 0.f };
	UIDesc.vSize = { 320.f, 50.f };

	if (FAILED(__super::Initialize(&UIDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_bDead = true;
	
	m_isAlphaControl = true;

	m_fAlpha = 0.3f;

	return S_OK;
}

void CHint_Display::Start()
{
	
}

void CHint_Display::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

}

void CHint_Display::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CHint_Display::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CHint_Display::Set_Dead(_bool bDead)
{
	m_bDead = bDead;

}

HRESULT CHint_Display::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isAlphaControl", &m_isAlphaControl, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

HRESULT CHint_Display::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAlphaSortTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Long_Box_Select_Click"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

CHint_Display* CHint_Display::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHint_Display* pInstance = new CHint_Display(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CHint_Display"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHint_Display::Clone(void* pArg)
{
	CHint_Display* pInstance = new CHint_Display(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHint_Display"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHint_Display::Free()
{
	__super::Free();

}
