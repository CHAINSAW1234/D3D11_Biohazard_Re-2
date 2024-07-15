#include "stdafx.h"

#include "Hint_Blind.h"

CHint_Blind::CHint_Blind(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCustomize_UI{ pDevice , pContext }
{
}

CHint_Blind::CHint_Blind(const CHint_Blind& rhs)
	: CCustomize_UI{ rhs }
{
}

HRESULT CHint_Blind::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHint_Blind::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;
	}

	return S_OK;
}

void CHint_Blind::Start()
{
	m_isLoad = false;
	m_isMask = true;
	m_fMaskControl = _float2{ 1.f, 1.f };
}

void CHint_Blind::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	__super::Tick(fTimeDelta);
}

void CHint_Blind::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CHint_Blind::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CHint_Blind* CHint_Blind::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHint_Blind* pInstance = new CHint_Blind(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CHint_Blind"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHint_Blind::Clone(void* pArg)
{
	CHint_Blind* pInstance = new CHint_Blind(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHint_Blind"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHint_Blind::Free()
{
	__super::Free();

}
