#include "stdafx.h"

#include "Tab_Window.h"
#include "Customize_UI.h"
#include "Button_UI.h"
#include "Inventory_Item_UI.h"
#include "Cursor_UI.h"
#include "Item_Discription.h"
#include "ItemProp.h"
#include "Read_Item_UI.h"
#include "Map_UI.h"
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

		if (FAILED(Add_Components()))	
			return E_FAIL;

		if (FAILED(Create_Button()))
			return E_FAIL;

		if (FAILED(Create_Inventory()))
			return E_FAIL;

		if (FAILED(Creat_Item_Mesh_Viewer()))
			return E_FAIL;

		if (FAILED(Creat_Item_Discription()))
			return E_FAIL;
		
		m_bDead = true;
	}

	Find_Cursor();

	return S_OK;
}

void CTab_Window::Start()
{
	list<class CGameObject*>* pUILayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));
	_bool bReadItemUI = { false };
	_bool bMapUI = { false };
	for (auto& iter : *pUILayer)
	{
		CRead_Item_UI* pRead = dynamic_cast<CRead_Item_UI*>(iter);
		CMap_UI* pMap = dynamic_cast<CMap_UI*>(iter);
		if(pRead!=nullptr &&!bReadItemUI)
			if (pRead->Get_UI_TYPE() == CRead_Item_UI::READ_UI_TYPE::INTRODUCE_READ)
			{
				m_pRead_Item_UI = pRead;
				bReadItemUI = true;
			}
		if (pMap != nullptr && !bMapUI)
		{
			m_pMap_UI = pMap;
			bMapUI = true;
		}

	}
}

void CTab_Window::Tick(_float fTimeDelta)
{
	FirstTick_Seting();

	if (DOWN == m_pGameInstance->Get_KeyState(VK_TAB))
	{
		OnOff_EventHandle();
	}
	
	if (true == m_bDead)
		return;

	ItemIven_EventHandle(fTimeDelta);

	Button_Act(fTimeDelta);

	switch (m_eWindowType)
	{
	case Client::CTab_Window::MINIMAP: 
	{
		break;
	}
		
	case Client::CTab_Window::INVENTORY: 
	{
		m_pInventory_Manager->Tick(fTimeDelta);
		ITEM_NUMBER eSelectedItemNum = m_pInventory_Manager->Get_Selected_ItemNum();
		m_pItem_Discription->Set_Item_Number(eSelectedItemNum);
		m_pItem_Discription->Tick(fTimeDelta);
		break;
	}
		
	case Client::CTab_Window::HINT: 
	{ 
		break;
	}

	case Client::CTab_Window::EXAMINE: 
	{
		m_pItem_Mesh_Viewer->Tick(fTimeDelta);
		if (m_fCurTime / 0.5f < 1.f)
		{
			m_fCurTime += fTimeDelta;
			m_fAlpha = m_pGameInstance->Get_Ease(Ease_InSine, 0.f, 1.f, m_fCurTime / 0.5f);
		}

		if (DOWN == m_pGameInstance->Get_KeyState(VK_RBUTTON))
		{
			m_eWindowType = INVENTORY;
			m_pHintButton->Set_Dead(false);
			m_pInvenButton->Set_Dead(false);
			m_pMapButton->Set_Dead(false);
			m_pInventory_Manager->Set_OnOff_Inven(false);
			m_pItem_Mesh_Viewer->Set_Operation(HIDE, ITEM_NUMBER_END);

			m_fCurTime = 0.f;
			m_fAlpha = 0.f;
			m_isAlphaControl = false;
		}
		break;
	}

	case Client::CTab_Window::PICK_UP_ITEM_WINDOW: {
		m_pInventory_Manager->Tick(fTimeDelta);
		m_pItem_Discription->Tick(fTimeDelta);
		m_pItem_Mesh_Viewer->Late_Tick(fTimeDelta);
		break;
	}

	default:
		break;
	}

	/* Cursor */
	if (nullptr != m_pCursor[1])
		Select_UI();

	__super::Tick(fTimeDelta);
}
  
void CTab_Window::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);

	switch (m_eWindowType)
	{
	case Client::CTab_Window::MINIMAP: {
		break;
	}

	case Client::CTab_Window::INVENTORY: {
		m_pInventory_Manager->Late_Tick(fTimeDelta);
		m_pItem_Discription->Late_Tick(fTimeDelta);
		break;
	}

	case Client::CTab_Window::HINT: {
		break;
	}

	case Client::CTab_Window::EXAMINE: {
		m_pItem_Mesh_Viewer->Late_Tick(fTimeDelta);
		m_pItem_Discription->Late_Tick(fTimeDelta);
		break;
	}

	case Client::CTab_Window::PICK_UP_ITEM_WINDOW: {
		m_pInventory_Manager->Late_Tick(fTimeDelta);
		m_pItem_Mesh_Viewer->Late_Tick(fTimeDelta);
		m_pItem_Discription->Late_Tick(fTimeDelta);
		break;
	}


	default:
		break;
	}
}

HRESULT CTab_Window::Render()
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



void CTab_Window::ItemIven_EventHandle(_float fTimeDelta)
{
	INVENTORY_EVENT eEvent = m_pInventory_Manager->Get_InventoryEvent();

	switch (eEvent)
	{
	case Client::EXAMINE_ITEM: {
		if (ITEM_NUMBER_END != m_pInventory_Manager->Get_Selected_ItemNum())
		{
			m_eWindowType = EXAMINE;
			m_isMapRender = false;
			m_pHintButton->Set_Dead(true);
			m_pInvenButton->Set_Dead(true);
			m_pMapButton->Set_Dead(true);
			m_pInventory_Manager->Set_OnOff_Inven(true);
			m_isAlphaControl = true;
			ITEM_NUMBER eItem_Num = m_pInventory_Manager->Get_Selected_ItemNum();
			m_pItem_Mesh_Viewer->Set_Operation(POP_UP, eItem_Num);
			m_pItem_Discription->Set_Item_Number(eItem_Num);
		}
		break;
	}

	default:
		break;
	}
}

void CTab_Window::FirstTick_Seting()
{
	if (true == m_isFristTick)
	{
		m_pInventory_Manager->FirstTick_Seting();
		m_isFristTick = false;

		if (nullptr == m_pCursor[0] || nullptr == m_pCursor[1])
		{
			Find_Cursor();

			if (nullptr == m_pCursor)
				MSG_BOX(TEXT("Inventory를 위한 Cursor 생성 불가능"));
		}
	}
}

void CTab_Window::Button_Act(_float fTimeDelta)
{
	if (DOWN == m_pGameInstance->Get_KeyState(VK_LBUTTON))
	{
		if (true == m_pMapButton->IsMouseHover())
		{
			m_eWindowType = MINIMAP;
			m_isMapRender = true;
			m_pInventory_Manager->Set_OnOff_Inven(true);
		}

		else if (true == m_pInvenButton->IsMouseHover())
		{
			m_eWindowType = INVENTORY;
			m_isMapRender = false;
			m_pInventory_Manager->Set_OnOff_Inven(false);

		}

		else if (true == m_pHintButton->IsMouseHover())
		{
			m_eWindowType = HINT;
			m_isMapRender = false;
			m_pInventory_Manager->Set_OnOff_Inven(true);
		}
	}
}

void CTab_Window::OnOff_EventHandle()
{
	if (false == m_bDead)
	{
		m_bDead = true;
		m_isMapRender = false;
		m_pHintButton->Set_Dead(m_bDead);
		m_pInvenButton->Set_Dead(m_bDead);
		m_pMapButton->Set_Dead(m_bDead);
		m_pInventory_Manager->Set_OnOff_Inven(m_bDead);


		if (nullptr != m_pCursor[1])
		{
			m_pCursor[0]->Set_Inven_Open(false);
			m_pCursor[1]->Set_Inven_Open(false);
		}
	}

	else
	{
		m_bDead = false;
		m_pHintButton->Set_Dead(m_bDead);
		m_pInvenButton->Set_Dead(m_bDead);
		m_pMapButton->Set_Dead(m_bDead);
		m_pInventory_Manager->Set_OnOff_Inven(m_bDead);//탭창열때 인벤이 초기값임

		if (nullptr != m_pCursor[1])
		{
			m_pCursor[0]->Set_Inven_Open(true);
			m_pCursor[1]->Set_Inven_Open(true);
		}
	}
}

void CTab_Window::PickUp_Item(CGameObject* pPickedUp_Item)
{
	OnOff_EventHandle();
	m_eWindowType = PICK_UP_ITEM_WINDOW;
	m_pPickedUp_Item = pPickedUp_Item;
	//Safe_AddRef(m_pPickedUp_Item);
	CInteractProps* pProp = static_cast<CInteractProps*>(m_pPickedUp_Item);
	_int iPickedUpItemNum = pProp->Get_iItemIndex();
	
	if (iPickedUpItemNum >= rpddocument01a && iPickedUpItemNum <= mapupperpolice01a)
	{
		m_pRead_Item_UI->Set_ReadItem_Type((CRead_Item_UI::ITEM_READ_TYPE)pProp->Get_PropType());
		//인벤토리 문서 부분에 먹었다 추가=> 아직 없는 것으로 앎 나중에
	}
	else
	{
		//m_pMap_UI->Destory_Item((MAP_FLOOR_TYPE)pProp->Get_Floor(), (LOCATION_MAP_VISIT)pProp->Get_Region(), (ITEM_NUMBER)iPickedUpItemNum);
	}
	/*_int iPickedUpItemQuant = static_cast<CItemProp*>(m_pPickedUp_Item)->Get_i*/
	m_pItem_Discription->Set_Item_Number(static_cast<ITEM_NUMBER>(iPickedUpItemNum), 10);
	m_pInventory_Manager->Set_InventoryEvent(PICK_UP_ITEM);
}

void CTab_Window::AddItem_ToInven(ITEM_NUMBER eAcquiredItem, _int iItemQuantity)
{
	m_pInventory_Manager->AddItem_ToInven(eAcquiredItem, iItemQuantity);
}

_bool CTab_Window::IsCan_AddItem_ToInven()
{
	return 	m_pInventory_Manager->IsCan_AddItem_ToInven();
}

_int CTab_Window::Get_Search_Item_Quantity(ITEM_NUMBER eItemNum)
{
	return 	m_pInventory_Manager->Get_Search_Item_Quantity(eItemNum);
}

ITEM_NUMBER CTab_Window::Get_Item_On_HotKey(_uint iHotKeyNum)
{
	return 	m_pInventory_Manager->Get_Item_On_HotKey(iHotKeyNum);
}

void CTab_Window::UseItem(ITEM_NUMBER eTargetItemNum, _int iUsage)
{
	return 	m_pInventory_Manager->UseItem(eTargetItemNum, iUsage);
}

void CTab_Window::Find_Cursor()
{
	list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));
	_int i = 0;

	for (auto& iter : *pUIList)
	{
		CCursor_UI* pCursor = dynamic_cast<CCursor_UI*>(iter);

		if (nullptr != pCursor)
		{
			m_pCursor[i] = pCursor;
			++i;
			
			if(nullptr != m_pCursor[1])
				break;
		}
	}
}

void CTab_Window::Select_UI()
{
	_bool isResult = { false };

	if (true == m_pMapButton->IsMouseHover())
		isResult = true;

	else if (true == m_pInvenButton->IsMouseHover())
		isResult = true;

	else if (true == m_pHintButton->IsMouseHover())
		isResult = true;

	else if (false == *m_pInventory_Manager->Get_NoHover_InvenBox())
		isResult = true;

	else
		isResult = false;

	m_pCursor[0]->Set_Cursor_Blur(isResult);
	m_pCursor[1]->Set_Cursor_Blur(isResult);
}

HRESULT CTab_Window::Add_Components()
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
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_Tab_Window_BackGround"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isAlphaControl", &m_isAlphaControl, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTab_Window::Create_Button()
{
	ifstream inputFileStream;
	wstring selectedFilePath;

	/* Button_HintWin */
	selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Hint_Button.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	if (FAILED(CCustomize_UI::CreatUI_FromDat(inputFileStream, nullptr, TEXT("Prototype_GameObject_Button_UI"), 
		(CGameObject**)&m_pHintButton, m_pDevice, m_pContext)))
		return E_FAIL;
	
	/* Button_InvenWin */
	selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory_Button.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	if (FAILED(CCustomize_UI::CreatUI_FromDat(inputFileStream, nullptr, TEXT("Prototype_GameObject_Button_UI"), 
		(CGameObject**)&m_pInvenButton, m_pDevice, m_pContext)))
		return E_FAIL;

	/* Button_MapWin */
	selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/MiniMap_Button.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	if (FAILED(CCustomize_UI::CreatUI_FromDat(inputFileStream, nullptr, TEXT("Prototype_GameObject_Button_UI"), 
		(CGameObject**)&m_pMapButton, m_pDevice, m_pContext)))
		return E_FAIL;

	if (nullptr == m_pHintButton || nullptr == m_pInvenButton || nullptr == m_pMapButton)
		return E_FAIL;

	m_pHintButton->Set_Dead(true);
	m_pInvenButton->Set_Dead(true);
	m_pMapButton->Set_Dead(true);

	Safe_AddRef(m_pHintButton);
	Safe_AddRef(m_pInvenButton);
	Safe_AddRef(m_pMapButton);

	return S_OK;
}

HRESULT CTab_Window::Creat_MiniMap()
{
	return S_OK;
}

HRESULT CTab_Window::Creat_Hint()
{
	return S_OK;
}

HRESULT CTab_Window::Creat_Item_Mesh_Viewer()
{
	CGameObject::GAMEOBJECT_DESC GameOBJDesc = {};
	GameOBJDesc.worldMatrix = XMMatrixIdentity();

	CGameObject* pGameOBJ = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Item_Mesh_Viewer"), &GameOBJDesc);
	m_pItem_Mesh_Viewer = dynamic_cast<CItem_Mesh_Viewer*>(pGameOBJ);

	if (nullptr == m_pItem_Mesh_Viewer)
		return E_FAIL;

	return S_OK;
}

HRESULT CTab_Window::Creat_Item_Discription()
{
	CUI::UI_DESC UIDesc = {};
	UIDesc.vPos = { g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f, 0.f };
	UIDesc.vSize = { g_iWinSizeX, g_iWinSizeY };

	CGameObject* pGameOBJ = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Item_Discription"), &UIDesc);
	m_pItem_Discription = dynamic_cast<CItem_Discription*>(pGameOBJ);

	if (nullptr == m_pItem_Discription)
		return E_FAIL;

	return S_OK;
}

HRESULT CTab_Window::Create_Inventory()
{
	m_pInventory_Manager = CInventory_Manager::Create(m_pDevice, m_pContext);
	if (nullptr == m_pInventory_Manager)
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

	Safe_Release(m_pHintButton);
	Safe_Release(m_pInvenButton);
	Safe_Release(m_pMapButton);
	Safe_Release(m_pInventory_Manager);
	Safe_Release(m_pItem_Mesh_Viewer);
	Safe_Release(m_pItem_Discription);
}
