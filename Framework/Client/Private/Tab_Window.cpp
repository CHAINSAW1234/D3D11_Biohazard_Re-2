#include "stdafx.h"

#include "Tab_Window.h"
#include "Customize_UI.h"
#include "Button_UI.h"
#include "Cursor_UI.h"
#include "Item_Discription.h"
#include "ItemProp.h"
#include "Read_Item_UI.h"
#include "Item_Map_UI.h"
#include "Targeting_Map_UI.h"
#include "Player.h"


constexpr _float BACKGROUND_MIN_ALPHA = 0.8f;


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

		if (FAILED(Creat_Hint()))
			return E_FAIL;

		m_bDead = true;

		m_isAlphaControl = true;

		m_fAlpha = 0.8f;
	}

	Find_Cursor();

	return S_OK;
}

void CTab_Window::Start()
{
	if (nullptr == m_pCursor[0] || nullptr == m_pCursor[1])
	{
		Find_Cursor();

		if (nullptr == m_pCursor)
			MSG_BOX(TEXT("Inventory를 위한 Cursor 생성 불가능"));
	}

	m_pInventory_Manager->FirstTick_Seting();

	list<class CGameObject*>* pUILayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));
	_bool bReadItemUI = { false };
	_bool bMapUI = { false };

	for (auto& iter : *pUILayer)
	{

		// if(&&!bReadItemUI)

		CRead_Item_UI* pRead = dynamic_cast<CRead_Item_UI*>(iter);
		if (pRead != nullptr && pRead->Get_UI_TYPE() == CRead_Item_UI::READ_UI_TYPE::INTRODUCE_READ)
		{
			m_pRead_Item_UI = pRead;
			bReadItemUI = true;
		}

		CItem_Map_UI* pItem = dynamic_cast<CItem_Map_UI*>(iter);
		if (pItem != nullptr && !bMapUI)
		{
			m_pMapItem_UI = pItem;
			bMapUI = true;
		}

		CTargeting_Map_UI* pTarget = dynamic_cast<CTargeting_Map_UI*>(iter);
		if (nullptr != pTarget 
			&& nullptr == m_pTargetNotify_UI 
			&& CMap_Manager::MAP_UI_TYPE::TARGET_NOTIFY == pTarget->Get_MapComponent_Type())
		{
			m_pTargetNotify_UI = pTarget;
		}

		CHotKey* pHotkey = dynamic_cast<CHotKey*>(iter);
		if (nullptr != pHotkey)
		{
			m_pHotKey = pHotkey;
			Safe_AddRef(m_pHotKey);
		}
	}
}

void CTab_Window::Tick(_float fTimeDelta)
{
	if (DOWN == m_pGameInstance->Get_KeyState(VK_TAB) && PICK_UP_ITEM_WINDOW != m_eWindowType)
	{
		OnOff_EventHandle();
	}
	
	if (true == m_bDead)
		return;

	ItemIven_EventHandle(fTimeDelta);

	Button_Act(fTimeDelta);

	switch (m_eWindowType)
	{
	case Client::CTab_Window::MINIMAP: {
		break;
	}
		
	case Client::CTab_Window::INVENTORY: {
		INVENTORY_Operation(fTimeDelta);
		break;
	}
		
	case Client::CTab_Window::HINT: { 
		HINT_Operation(fTimeDelta);
		break;
	}

	case Client::CTab_Window::EXAMINE: {
		EXAMINE_Operation(fTimeDelta);
		break;
	}

	case Client::CTab_Window::PICK_UP_ITEM_WINDOW: {
		PICK_UP_ITEM_WINDOW_Operation(fTimeDelta);
		break;
	}

	case Client::CTab_Window::INTERACT_PROPS: {
		INTERACT_PROPS_Operation(fTimeDelta);
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
		m_pHint->Late_Tick(fTimeDelta);
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

	case Client::CTab_Window::INTERACT_PROPS: {
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

void CTab_Window::MINIMAP_Operation(_float fTimeDelta)
{
}

void CTab_Window::INVENTORY_Operation(_float fTimeDelta)
{
	m_pInventory_Manager->Tick(fTimeDelta);
	ITEM_NUMBER eSelectedItemNum = m_pInventory_Manager->Get_Selected_ItemNum();
	m_pItem_Discription->Set_Item_Number(eSelectedItemNum);
	m_pItem_Discription->Tick(fTimeDelta);
}

void CTab_Window::HINT_Operation(_float fTimeDelta)
{
	m_pHint->Tick(fTimeDelta);
}

void CTab_Window::EXAMINE_Operation(_float fTimeDelta)
{
	m_pItem_Mesh_Viewer->Tick(fTimeDelta);

	switch (m_eSequence)
	{
	case Client::POP_UP: {
		if (m_fCurTime / 0.5f < 1.f)
		{
			m_fCurTime += fTimeDelta;
			m_fAlpha = m_pGameInstance->Get_Ease(Ease_InSine, BACKGROUND_MIN_ALPHA, 1.f, m_fCurTime / 0.5f);
		}

		if (DOWN == m_pGameInstance->Get_KeyState(VK_RBUTTON))
		{
			m_fCurTime = 0.f;
			m_eSequence = HIDE;
			m_pItem_Mesh_Viewer->Set_Operation(HIDE, ITEM_NUMBER_END, 0);

			m_pHintButton->Set_Dead(false);
			m_pInvenButton->Set_Dead(false);
			m_pMapButton->Set_Dead(false);
			m_pInventory_Manager->Set_OnOff_Inven(false);
			m_pHotKey->Set_Dead(false);
		}
		break;
	}
		

	case Client::HIDE: {
		if (m_fCurTime / 0.5f < 1.f)
		{
			m_fCurTime += fTimeDelta;
			m_fAlpha = m_pGameInstance->Get_Ease(Ease_InSine, 1.f, BACKGROUND_MIN_ALPHA, m_fCurTime / 0.5f);
		}

		else
		{
			m_eWindowType = INVENTORY;

			m_pItem_Mesh_Viewer->Set_Dead(true);

			m_fCurTime = 0.f;
		}

		break;
	}
		
	default:
		break;
	}
}

void CTab_Window::PICK_UP_ITEM_WINDOW_Operation(_float fTimeDelta)
{
	switch (m_eSequence)
	{
	case Client::POP_UP: {
		m_pItem_Mesh_Viewer->Tick(fTimeDelta);
		m_pItem_Discription->Tick(fTimeDelta);

		if (m_fCurTime / 0.5f < 1.f)
		{
			m_fCurTime += fTimeDelta;
			m_fAlpha = m_pGameInstance->Get_Ease(Ease_InSine, BACKGROUND_MIN_ALPHA, 1.f, m_fCurTime / 0.5f);
		}

		else
		{
			if (DOWN == m_pGameInstance->Get_KeyState(VK_LBUTTON))
			{
				/*Item_Discription 세팅*/
				m_eSequence = UI_IDLE;

				/*Item_Mesh_Viewer 세팅*/
				m_pItem_Mesh_Viewer->Set_Operation(UI_IDLE, ITEM_NUMBER_END, 1);

				/*Inventory_Manager 세팅*/
				_int iPickedUpItemNum = static_cast<CInteractProps*>(m_pPickedUp_Item)->Get_iItemIndex();
				ITEM_NUMBER ePickedItemNum = static_cast<ITEM_NUMBER>(iPickedUpItemNum);
				m_pInventory_Manager->PUO_Seting(ePickedItemNum, CInventory_Manager::PickUpItem_Quantity_Classify(ePickedItemNum));

				/*Cursor 세팅*/
				if (nullptr != m_pCursor[1])
				{
					m_pCursor[0]->Set_Inven_Open(true);
					m_pCursor[1]->Set_Inven_Open(true);
				}

				/*TabWindow 세팅*/
				m_fCurTime = 0.f;
				break;
			}
		}
		break;
	}	

	case Client::UI_IDLE: {
		if (GET_ITEM == m_pInventory_Manager->Get_InventoryEvent())
		{
			_int iCollectNum = static_cast<CInteractProps*>(m_pPickedUp_Item)->Get_iItemIndex();
			m_vecCollect_ITEM.push_back(static_cast<ITEM_NUMBER>(iCollectNum));
			m_pPickedUp_Item->Set_Dead(true);
			m_fCurTime = 0.f;
			m_eSequence = HIDE;
			m_pItem_Mesh_Viewer->Set_Operation(HIDE, ITEM_NUMBER_END, 1);
			break;
		}

		else if (DROP_ITEM == m_pInventory_Manager->Get_InventoryEvent())
		{
			m_fCurTime = 0.f;
			m_eSequence = HIDE;
			m_pItem_Mesh_Viewer->Set_Operation(HIDE, ITEM_NUMBER_END, 1);
			break;
		}

		m_pItem_Mesh_Viewer->Tick(fTimeDelta);
		m_pItem_Discription->Tick(fTimeDelta);
		m_pInventory_Manager->Tick(fTimeDelta);

		if (m_fCurTime / 0.5f < 1.f)
		{
			m_fCurTime += fTimeDelta;
			m_fAlpha = m_pGameInstance->Get_Ease(Ease_InSine, 1.f, BACKGROUND_MIN_ALPHA, m_fCurTime / 0.5f);
		}
		break;
	}
		

	case Client::HIDE: {
		if (m_fCurTime / 0.5f <1.f)
		{
			m_fCurTime += fTimeDelta;
			m_pItem_Mesh_Viewer->Tick(fTimeDelta);
			m_pItem_Discription->Tick(fTimeDelta);
			m_pInventory_Manager->Tick(fTimeDelta);
		}

		else
		{
			m_fCurTime = 0.f;
			m_eSequence = STATE_END;
			m_pPickedUp_Item = nullptr;
			m_pGameInstance->Set_IsPaused(false);
			CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Player"))->front());
			pPlayer->Set_isCamTurn(false);
			OnOff_EventHandle();
		}
		break;
	}
		

	default:
		break;
	}

}

void CTab_Window::INTERACT_PROPS_Operation(_float fTimeDelta)
{
	m_pInventory_Manager->Tick(fTimeDelta);
	ITEM_NUMBER eSelectedItemNum = m_pInventory_Manager->Get_Selected_ItemNum();
	m_pItem_Discription->Set_Item_Number(eSelectedItemNum);
	m_pItem_Discription->Tick(fTimeDelta);
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
			m_eSequence = POP_UP;
			m_isMapRender = false;
			m_pHintButton->Set_Dead(true);
			m_pInvenButton->Set_Dead(true);
			m_pMapButton->Set_Dead(true);
			m_pInventory_Manager->Set_OnOff_Inven(true);
			m_pHotKey->Set_Dead(true);
			m_isAlphaControl = true;
			ITEM_NUMBER eItem_Num = m_pInventory_Manager->Get_Selected_ItemNum();
			m_pItem_Mesh_Viewer->Set_Operation(POP_UP, eItem_Num, 0);
			m_pItem_Discription->Set_Item_Number(eItem_Num);
		}
		break;
	}

	default:
		break;
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
			m_pHotKey->Set_Dead(true);
			m_pHint->Set_Dead(true);

			m_pGameInstance->Play_Sound(TEXT("ui_ingame_media.bnk.2_51.mp3"), CHANNELID::CH30);

		}

		else if (true == m_pInvenButton->IsMouseHover())
		{
			m_eWindowType = INVENTORY;
			m_isMapRender = false;
			m_pInventory_Manager->Set_OnOff_Inven(false);
			m_pHotKey->Set_Dead(false);
			m_pHint->Set_Dead(true);
			
			m_pGameInstance->Play_Sound(TEXT("ui_ingame_media.bnk.2_51.mp3"), CHANNELID::CH30);

		}

		else if (true == m_pHintButton->IsMouseHover())
		{
			m_eWindowType = HINT;
			m_isMapRender = false;
			m_pInventory_Manager->Set_OnOff_Inven(true);
			m_pHotKey->Set_Dead(true);
			m_pHint->Set_Dead(false);
			
			m_pGameInstance->Play_Sound(TEXT("ui_ingame_media.bnk.2_51.mp3"), CHANNELID::CH30);
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
		m_eWindowType = WINDOW_TYPE_END;
		m_pInventory_Manager->Set_OnOff_Inven(m_bDead);
		m_pHotKey->Set_Dead(m_bDead);

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
		m_eWindowType = INVENTORY;
		m_pInventory_Manager->Set_OnOff_Inven(m_bDead);//탭창열때 인벤이 초기값임
		m_pHotKey->Set_Dead(m_bDead);

		if (nullptr != m_pCursor[1])
		{
			m_pCursor[0]->Set_Inven_Open(true);
			m_pCursor[1]->Set_Inven_Open(true);
		}
	}
}

void CTab_Window::PickUp_Item(CGameObject* pPickedUp_Item)
{
	CInteractProps* pProp = dynamic_cast<CInteractProps*>(pPickedUp_Item);
	if (nullptr == pProp)
		return;

	_int iPickedUpItemNum = pProp->Get_iItemIndex();
	ITEM_NUMBER ePickedItemNum = static_cast<ITEM_NUMBER>(iPickedUpItemNum);
	ITEM_TYPE eItemType = CInventory_Manager::ItemType_Classify_ByNumber(ePickedItemNum);
	
	if (eItemType != DOCUMENT)
	{
		/*TabWindow 세팅*/
		m_bDead = false;
		m_eWindowType = PICK_UP_ITEM_WINDOW;
		m_eSequence = POP_UP;
		m_pPickedUp_Item = pPickedUp_Item;
		m_isAlphaControl = true;

		/*Item_Discription 세팅*/
		m_pItem_Discription->Set_Item_Number(ePickedItemNum, CInventory_Manager::PickUpItem_Quantity_Classify(ePickedItemNum));

		/*Item_Mesh_Viewer 세팅*/
		m_pItem_Mesh_Viewer->Set_Operation(POP_UP, ePickedItemNum, 1);
	}

	else
	{
		if (iPickedUpItemNum >= rpddocument01a && iPickedUpItemNum <= mapupperpolice01a)
		{
			m_pRead_Item_UI->Set_ReadItem_Type((ITEM_READ_TYPE)pProp->Get_PropType());
			//인벤토리 문서 부분에 먹었다 추가=> 아직 없는 것으로 앎 나중에
			pPickedUp_Item->Set_Dead(true);
		}

		else
		{
			/* 아이템 삭제 */
			m_pMapItem_UI->Destory_Item((MAP_FLOOR_TYPE)pProp->Get_Floor(), (LOCATION_MAP_VISIT)pProp->Get_Region(), (ITEM_NUMBER)iPickedUpItemNum);
			
			/* 아이템으로 인한 맵 확인 */
			if (nullptr != m_pTargetNotify_UI)
			{
				/* Tab Window -> Target Notify -> Main Map*/
				m_pTargetNotify_UI->Set_SearchFor_Verification_MapType(pProp->Get_Floor(), pProp->Get_Region());
			}
		}
	}
}

void CTab_Window::interact_Props(CGameObject* pPickedUp_Item)
{
	CInteractProps* pProp = dynamic_cast<CInteractProps*>(pPickedUp_Item);
	if (nullptr == pProp)
		return;
	_int iItemIndex = pProp->Get_NeedItem_Index(); // 필요한 아이템 인덱스 get tto da ze -
	pProp->Do_Interact_Props(); // 프롭의 동작 함수 (올바른 아이템을 사용했을시 이 함수 호출- 일단 창문만 해놨어요)
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
	return 	m_pHotKey->Get_Item_On_HotKey(iHotKeyNum);
}

void CTab_Window::UseItem(ITEM_NUMBER eTargetItemNum, _int iUsage)
{
	return 	m_pInventory_Manager->UseItem(eTargetItemNum, iUsage);
}

void CTab_Window::Hotkey_PopUp()
{
	m_pHotKey->PopUp_Call();
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
	//CUI::UI_DESC UIDesc = {};
	//UIDesc.vPos = { g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f, 0.f };
	//UIDesc.vSize = { g_iWinSizeX, g_iWinSizeY };

	CGameObject* pGameOBJ = m_pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Hint"));
	m_pHint = dynamic_cast<CHint*>(pGameOBJ);

	if (nullptr == m_pHint)
		return E_FAIL;

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
	Safe_Release(m_pHotKey);
	Safe_Release(m_pHint);
}