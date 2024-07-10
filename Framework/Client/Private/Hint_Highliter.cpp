#include "stdafx.h"

#include "Hint_Highliter.h"


CHint_Highliter::CHint_Highliter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice , pContext }
{
}

CHint_Highliter::CHint_Highliter(const CHint_Highliter& rhs)
	: CUI{ rhs }
{
}

HRESULT CHint_Highliter::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHint_Highliter::Initialize(void* pArg)
{
	//if (nullptr != pArg)
	//{
	//	if (FAILED(__super::Initialize(&pArg)))
	//		return E_FAIL;
	//	if (FAILED(Add_Components()))
	//		return E_FAIL;
	//	m_bDead = true;
	//}

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

void CHint_Highliter::Start()
{
	
}

void CHint_Highliter::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

}

void CHint_Highliter::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CHint_Highliter::Render()
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

void CHint_Highliter::Set_Dead(_bool bDead)
{
	m_bDead = bDead;

}

HRESULT CHint_Highliter::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom || nullptr == m_pTextureCom)
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

HRESULT CHint_Highliter::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAlphaSortTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	///* For.Com_Texture */
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Long_Box_Select_Click"),
	//	TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	//	return E_FAIL;

	return S_OK;
}

CHint_Highliter* CHint_Highliter::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHint_Highliter* pInstance = new CHint_Highliter(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CHint_Highliter"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHint_Highliter::Clone(void* pArg)
{
	CHint_Highliter* pInstance = new CHint_Highliter(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHint_Highliter"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHint_Highliter::Free()
{
	__super::Free();

}
