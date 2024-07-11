#include "stdafx.h"

#include "Hint.h"
#include "TextBox.h"
#include "Hint_Highliter.h"

CHint::CHint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice , pContext }
{
}

CHint::CHint(const CHint& rhs)
	: CUI{ rhs }
{
}

HRESULT CHint::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHint::Initialize(void* pArg)
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
	UIDesc.vPos = { g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f, 0.f };
	UIDesc.vSize = { g_iWinSizeX, g_iWinSizeY };

	if (FAILED(__super::Initialize(&UIDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_bDead = true;


	return S_OK;
}

void CHint::Start()
{

}

void CHint::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

}

void CHint::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CHint::Render()
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

void CHint::Set_Dead(_bool bDead)
{
	m_bDead = bDead;

}

HRESULT CHint::Bind_ShaderResources()
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

HRESULT CHint::Create_Hint_Highlighter()
{
	CGameObject* pGameOBJ = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Hint_Highliter"));
	m_pHint_Highlighter = dynamic_cast<CHint_Highliter*>(pGameOBJ);

	if (nullptr == m_pHint_Highlighter)
		return E_FAIL;

	return S_OK;
}

HRESULT CHint::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hint_BackGround"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

CHint* CHint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHint* pInstance = new CHint(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CHint"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHint::Clone(void* pArg)
{
	CHint* pInstance = new CHint(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHint"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHint::Free()
{
	__super::Free();

	Safe_Release(m_pHint_Highlighter);
}
