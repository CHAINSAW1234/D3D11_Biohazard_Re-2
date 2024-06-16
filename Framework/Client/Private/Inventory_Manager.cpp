#include "stdafx.h"

#include "Inventory_Manager.h"


CInventory_Manager::CInventory_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CInventory_Manager::Initialize()
{
	ifstream inputFileStream;
	wstring selectedFilePath;

#pragma region 인벤토리 박스 20개 미리생성
	selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/VoidBox.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	vector<CCustomize_UI::CUSTOM_UI_DESC> vecCustomInvenUIDesc;
	CCustomize_UI::ExtractData_FromDat(inputFileStream, &vecCustomInvenUIDesc, true);

	for (_uint i = 0; i < 5; i++)
	{
		for (_uint j = 0; j < 4; j++)
		{
			_float fPosX = m_fInterval.x * j;
			_float fPosY = -m_fInterval.y * i;
			if (FAILED(Create_InvenUI(&vecCustomInvenUIDesc, _float3(fPosX, fPosY, 0.f))))
				return E_FAIL;
		}
	}

	for (auto& iter : m_vecInventoryUI)
	{
		if (nullptr != iter)
		{
			Safe_AddRef(iter);
			Set_OnOff_Inven(true);
		}
	}
#pragma endregion

#pragma region SelectBox
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Inventory_SelectBox.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	vector<CCustomize_UI::CUSTOM_UI_DESC> vecCustomSelecUIDesc;
	CCustomize_UI::ExtractData_FromDat(inputFileStream, &vecCustomSelecUIDesc, true);
	if (FAILED(Create_SelectUI(&vecCustomSelecUIDesc)))
		return E_FAIL;

	m_pSelectBox->Set_Dead(true);
	m_pSelectBoxTransform = dynamic_cast<CTransform*>(m_pSelectBox->Get_Component(g_strTransformTag));

	m_fSelectBoxResetPos = m_vecInventoryUI[0]->GetPosition();

	Safe_AddRef(m_pSelectBox);
	Safe_AddRef(m_pSelectBoxTransform);
#pragma endregion

	return S_OK;
}

void CInventory_Manager::FirstTick_Seting()
{
	m_pSelectBox->FirstTick_Seting();
	m_pSelectBox->ResetPosition(m_fSelectBoxResetPos);
}

void CInventory_Manager::Tick(_float fTimeDelta)
{
	_bool IsNoOneHover = true;
	CButton_UI* pHoveredObj = nullptr;

	for (auto& iter : m_vecInventoryUI)
	{
		if (true == iter->IsMouseHover())
		{
			IsNoOneHover = false;
			pHoveredObj = iter;
		}
	}
	m_pSelectBox->Set_Dead(IsNoOneHover);


	if (false == IsNoOneHover)
	{
		m_pSelectBoxTransform->Set_State(CTransform::STATE_POSITION,
			dynamic_cast<CTransform*>(pHoveredObj->Get_Component(g_strTransformTag))->Get_State_Float4(CTransform::STATE_POSITION));
	}

}

void CInventory_Manager::Late_Tick(_float fTimeDelta)
{

}

void CInventory_Manager::Set_OnOff_Inven(_bool bInput)
{
	for (_uint i = 0; i < m_iInvenNum; i++)
	{
		m_vecInventoryUI[i] -> Set_Dead(bInput);
	}
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



HRESULT CInventory_Manager::Create_InvenUI(vector<CCustomize_UI::CUSTOM_UI_DESC>* vecInvenUI, _float3 fInterval)
{
	CButton_UI* pParentInvenUI = { nullptr };

	for (auto& iter : *vecInvenUI)
	{
		if (0 == iter.fMaxFrame && TEXT("") != iter.wstrDefaultTexturPath)
		{
			/* For.Prototype_Component_Texture_ */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, iter.wstrDefaultTexturComTag,
				CTexture::Create(m_pDevice, m_pContext, iter.wstrDefaultTexturPath)))) {
				int a = 0;
			}
		}

		else if (0 < iter.fMaxFrame && TEXT("") != iter.wstrDefaultTexturPath)
		{
			/* For.Prototype_Component_Texture_ */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, iter.wstrDefaultTexturComTag,
				CTexture::Create(m_pDevice, m_pContext, iter.wstrDefaultTexturPath, iter.fMaxFrame)))) {
				int a = 0;
			}
		}

		if (TEXT("") != iter.wstrMaskPath)
		{
			/* For.Prototype_Component_Texture_ */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, iter.wstrMaskComTag,
				CTexture::Create(m_pDevice, m_pContext, iter.wstrMaskPath)))) {
				int a = 0;
			}
		}

		//CGameObject* pGameOBJ = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Inventory_Item_UI"), &iter);
		//if (nullptr == pGameOBJ)
		//	return E_FAIL;

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Button_UI"), &iter)))
			return E_FAIL;

		CGameObject* pGameObj = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"))->back();

		CButton_UI* pInvenUI = dynamic_cast<CButton_UI*>(pGameObj);

		pInvenUI->Move_State(fInterval, 0);
		pInvenUI->Set_IsLoad(false);
		pInvenUI->Set_Dead(true);

		if (nullptr != pParentInvenUI)
		{
			pParentInvenUI->PushBack_Child(pGameObj);
			pParentInvenUI = nullptr;
		}

		else
		{
			m_vecInventoryUI.push_back(pInvenUI);
		}

		pParentInvenUI = pInvenUI;
	}

	return S_OK;
}

HRESULT CInventory_Manager::Create_SelectUI(vector<CCustomize_UI::CUSTOM_UI_DESC>* vecInvenUI)
{
	CInventorySelect* pParentInvenUI = { nullptr };

	for (auto& iter : *vecInvenUI)
	{
		if (0 == iter.fMaxFrame && TEXT("") != iter.wstrDefaultTexturPath)
		{
			/* For.Prototype_Component_Texture_ */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, iter.wstrDefaultTexturComTag,
				CTexture::Create(m_pDevice, m_pContext, iter.wstrDefaultTexturPath)))) {
				int a = 0;
			}
		}

		else if (0 < iter.fMaxFrame && TEXT("") != iter.wstrDefaultTexturPath)
		{
			/* For.Prototype_Component_Texture_ */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, iter.wstrDefaultTexturComTag,
				CTexture::Create(m_pDevice, m_pContext, iter.wstrDefaultTexturPath, iter.fMaxFrame)))) {
				int a = 0;
			}
		}

		if (TEXT("") != iter.wstrMaskPath)
		{
			/* For.Prototype_Component_Texture_ */
			if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, iter.wstrMaskComTag,
				CTexture::Create(m_pDevice, m_pContext, iter.wstrMaskPath)))) {
				int a = 0;
			}
		}

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_InventorySelect"), &iter)))
			return E_FAIL;

		CGameObject* pGameObj = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"))->back();

		CInventorySelect* pInvenUI = dynamic_cast<CInventorySelect*>(pGameObj);

		pInvenUI->Set_Dead(true);

		if (nullptr != pParentInvenUI)
		{
			pParentInvenUI->PushBack_Child(pGameObj);
			pParentInvenUI = nullptr;
		}

		else
		{
			m_pSelectBox = pInvenUI;
		}

		pParentInvenUI = pInvenUI;
	}

	return S_OK;
}

void CInventory_Manager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

	for (auto& iter : m_vecInventoryUI)
	{
		Safe_Release(iter);
	}
	m_vecInventoryUI.clear();

	Safe_Release(m_pSelectBox);
	Safe_Release(m_pSelectBoxTransform);
}