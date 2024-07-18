#include "stdafx.h"

#include "Hint_Highliter.h"

CHint_Highliter::CHint_Highliter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCustomize_UI{ pDevice , pContext }
{
}

CHint_Highliter::CHint_Highliter(const CHint_Highliter& rhs)
	: CCustomize_UI{ rhs }
{
}

HRESULT CHint_Highliter::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHint_Highliter::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;
	}

	m_isLoad = false;

	return S_OK;
}

void CHint_Highliter::Start()
{
	Set_Value_Color(&m_vColor[0]);
	m_fBlending = 0.8f;
}

void CHint_Highliter::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	m_fBlending = 0.4f;

	__super::Tick(fTimeDelta);
}

void CHint_Highliter::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CHint_Highliter::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHint_Highliter::Change_Tool()
{
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
