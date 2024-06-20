#include "stdafx.h"

#include "ContextMenu.h"

CContextMenu::CContextMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCustomize_UI{ pDevice , pContext }
{
}

CContextMenu::CContextMenu(const CContextMenu& rhs)
	: CCustomize_UI{ rhs }
{
}

HRESULT CContextMenu::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CContextMenu::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;
	}

	return S_OK;
}

void CContextMenu::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
	{
		m_pBackground->Set_Dead(m_bDead);
		return;
	}

	m_pBackground->Tick(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CContextMenu::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
	{
		m_pBackground->Set_Dead(m_bDead);
		return;
	}

	m_pBackground->Late_Tick(fTimeDelta);

	__super::Late_Tick(fTimeDelta);
}

HRESULT CContextMenu::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CContextMenu::Create_Background()
{
	ifstream inputFileStream;
	wstring selectedFilePath;

	/* Button_HintWin */
	selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/ContextMenu_BackGround.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	if (FAILED(CCustomize_UI::CreatUI_FromDat(inputFileStream, nullptr, TEXT("Prototype_GameObject_ContextMenu"),
		(CGameObject**)&m_pBackground, m_pDevice, m_pContext)))
		return E_FAIL;

	return S_OK;
}

CContextMenu* CContextMenu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CContextMenu* pInstance = new CContextMenu(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CContextMenu"));

		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CContextMenu::Clone(void* pArg)
{
	CContextMenu* pInstance = new CContextMenu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CContextMenu"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CContextMenu::Free()
{
	__super::Free();
}
