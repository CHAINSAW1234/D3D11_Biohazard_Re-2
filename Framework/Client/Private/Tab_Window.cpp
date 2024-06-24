#include "stdafx.h"

#include "Tab_Window.h"
#include "Customize_UI.h"
#include "Button_UI.h"
#include "Inventory_Item_UI.h"
#include "Cursor_UI.h"

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
		
		m_bDead = true;
	}

	Find_Cursor();

	return S_OK;
}

void CTab_Window::Tick(_float fTimeDelta)
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

	if (DOWN == m_pGameInstance->Get_KeyState(VK_TAB))
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

	if (true == m_bDead)
		return;

	if (EXAMINE_ITEM == m_pInventory_Manager->Get_InventoryEvent())
	{
		m_eWindowType = EXAMINE;
		m_isMapRender = false;
		m_pHintButton->Set_Dead(true);
		m_pInvenButton->Set_Dead(true);
		m_pMapButton->Set_Dead(true);
		m_pInventory_Manager->Set_OnOff_Inven(true);
		m_isAlphaControl = true;

		if (-1 == m_pInventory_Manager->Get_Selected_ItemNum())
		{
			m_eWindowType = INVENTORY;
			m_isMapRender = false;
			m_pHintButton->Set_Dead(false);
			m_pInvenButton->Set_Dead(false);
			m_pMapButton->Set_Dead(false);
			m_pInventory_Manager->Set_OnOff_Inven(false);
			m_isAlphaControl = false;
		}

		else
		{
			m_pItem_Mesh_Viewer->Set_Operation(POP_UP, static_cast<ITEM_NUMBER>(m_pInventory_Manager->Get_Selected_ItemNum()));
		}

	}

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

	switch (m_eWindowType)
	{
	case Client::CTab_Window::MINIMAP: {
		break;
	}
		
	case Client::CTab_Window::INVENTORY: {
		m_pInventory_Manager->Tick(fTimeDelta);

		break;
	}
		
	case Client::CTab_Window::HINT: { 
		break;
	}

	case Client::CTab_Window::EXAMINE: {
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
		break;
	}

	case Client::CTab_Window::HINT: {
		break;
	}

	case Client::CTab_Window::EXAMINE: {
		m_pItem_Mesh_Viewer->Late_Tick(fTimeDelta);
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

void CTab_Window::AddItem_ToInven(ITEM_NUMBER eAcquiredItem)
{
	m_pInventory_Manager->AddItem_ToInven(eAcquiredItem);
}

_bool CTab_Window::IsCan_AddItem_ToInven()
{
	return 	m_pInventory_Manager->IsCan_AddItem_ToInven();
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
		isResult = false;

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
}
