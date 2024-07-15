#include "stdafx.h"

#include "Hint_Directory.h"

CHint_Directory::CHint_Directory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCustomize_UI{ pDevice , pContext }
{
}

CHint_Directory::CHint_Directory(const CHint_Directory& rhs)
	: CCustomize_UI{ rhs }
{
}

HRESULT CHint_Directory::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHint_Directory::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;
	}

	//	m_isLoad = false;

	return S_OK;
}

void CHint_Directory::Start()
{

}

void CHint_Directory::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	__super::Tick(fTimeDelta);
}

void CHint_Directory::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	__super::Late_Tick(fTimeDelta);
}

HRESULT CHint_Directory::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CHint_Directory* CHint_Directory::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHint_Directory* pInstance = new CHint_Directory(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CHint_Directory"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHint_Directory::Clone(void* pArg)
{
	CHint_Directory* pInstance = new CHint_Directory(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHint_Directory"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHint_Directory::Free()
{
	__super::Free();

}
