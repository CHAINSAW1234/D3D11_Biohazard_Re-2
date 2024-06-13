#include "stdafx.h"

#include "Inventory_Manager.h"


CInventory_Manager::CInventory_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CInventory_Manager::Initialize()
{

	for (_uint i = 0; i < 2; i++)
	{
		for (_uint j = 0; j < 4; j++)
		{
			ifstream inputFileStream;
			wstring selectedFilePath;
			_float fPosX = m_fInterval.x * j;
			_float fPosY = m_fInterval.y * i;
			CButton_UI* pOut = { nullptr };
			selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/VoidBox.dat");
			inputFileStream.open(selectedFilePath, ios::binary);
			if (FAILED(CCustomize_UI::CreatUI_FromDat(inputFileStream, nullptr, TEXT("Prototype_GameObject_Button_UI"),
				(CGameObject**)&pOut, m_pDevice, m_pContext)))
				return E_FAIL;

			//pOut->Set_InvenType(CCustomize_UI::INVENTORY_TYPE::MAIN_INVEN);
			pOut->Move_State(_float3(fPosX, fPosY, 0.f), 0);
			m_vecInventoryBox.push_back(pOut);
		}
	}

	for (auto& iter : m_vecInventoryBox)
	{
		if (nullptr != iter)
		{
			Safe_AddRef(iter);
		}
	}


	return S_OK;
}

void CInventory_Manager::Tick(_float fTimeDelta)
{

}

void CInventory_Manager::Late_Tick(_float fTimeDelta)
{

}

void CInventory_Manager::Set_OnOff_Inven(_bool bInput)
{
	for (auto& iter : m_vecInventoryBox)
		iter->Set_Dead(bInput);
}

CInventory_Manager* CInventory_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInventory_Manager* pInstance = new CInventory_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CInventory_Manager"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInventory_Manager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	for (auto& iter : m_vecInventoryBox)
	{
		Safe_Release(iter);
	}
	m_vecInventoryBox.clear();

}
