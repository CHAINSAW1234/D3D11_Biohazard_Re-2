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

	m_isLoad = false;

	if (false == m_IsChild)
	{
		m_vecTextBoxes[0]->Set_isTransformBase(false);
	}

	else
	{
		//m_fBlending = 1.f;
	}

	return S_OK;
}

void CHint_Directory::Start()
{
	Set_Value_Color(&m_vColor[0]);

	if (true == m_IsChild)
	{
		m_fBlending = 0.1f;
		m_pTransformCom->Set_Scaled(560.f, 30.f, 1.f);
	}
}

void CHint_Directory::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;
	
	//__super::Tick(fTimeDelta);
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

HRESULT CHint_Directory::Change_Tool()
{
	return S_OK;
}

void CHint_Directory::Set_Directory(ITEM_READ_TYPE eIRT, wstring wstrText)
{
	m_eItem_Read_Type = eIRT;
	m_vecTextBoxes[0]->Set_Text(wstrText);
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
