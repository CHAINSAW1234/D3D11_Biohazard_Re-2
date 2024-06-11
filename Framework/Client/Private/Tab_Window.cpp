#include "stdafx.h"

#include "Tab_Window.h"

CTab_Window::CTab_Window(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice , pContext }
{
}

CTab_Window::CTab_Window(const CTab_Window& rhs)
	: CUI{ rhs }
{
}

HRESULT CTab_Window::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTab_Window::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;
	}

	return S_OK;
}

void CTab_Window::Tick(_float fTimeDelta)
{
	if (DOWN == m_pGameInstance->Get_KeyState(VK_TAB))
		m_IsActivation = !m_IsActivation;

	if (true == m_IsActivation)
	{
		__super::Tick(fTimeDelta);
	}
}

void CTab_Window::Late_Tick(_float fTimeDelta)
{
	if (true == m_IsActivation)
	{
		__super::Late_Tick(fTimeDelta);
	}
}

HRESULT CTab_Window::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTab_Window::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PlayerUI"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTab_Window::Bind_ShaderResources()
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

	return S_OK;
}

CTab_Window* CTab_Window::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTab_Window* pInstance = new CTab_Window(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTab_Window"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTab_Window::Clone(void* pArg)
{
	CTab_Window* pInstance = new CTab_Window(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTab_Window"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTab_Window::Free()
{
	__super::Free();
}
