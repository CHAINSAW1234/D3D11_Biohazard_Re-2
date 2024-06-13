#include "stdafx.h"

#include "Tab_Window.h"
#include "Customize_UI.h"
#include "Button_UI.h"
#include "Inventory_Item_UI.h"

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



		

		m_bDead = true;
	}

	return S_OK;
}

void CTab_Window::Tick(_float fTimeDelta)
{
	if (DOWN == m_pGameInstance->Get_KeyState(VK_TAB))
	{
		if (false == m_bDead)
		{
			m_bDead = true;
			m_pHintButton->Set_Dead(m_bDead);
			m_pInvenButton->Set_Dead(m_bDead);
			m_pMapButton->Set_Dead(m_bDead);

		}

		else
		{
			m_bDead = false;
			m_pHintButton->Set_Dead(m_bDead);
			m_pInvenButton->Set_Dead(m_bDead);
			m_pMapButton->Set_Dead(m_bDead);

		}
	}

	if (true == m_bDead)
		return;

	if (DOWN == m_pGameInstance->Get_KeyState(VK_LBUTTON))
	{
		if (true == m_pMapButton->IsMouseHover())
		{
			m_eWindowType = MINIMAP;
			m_pInventory_Manager->Set_OnOff_Inven(false);
		}

		else if (true == m_pInvenButton->IsMouseHover())
		{
			m_eWindowType = INVENTORY;
			m_pInventory_Manager->Set_OnOff_Inven(true);

		}
			
		else if (true == m_pHintButton->IsMouseHover())
		{
			m_eWindowType = HINT;
			m_pInventory_Manager->Set_OnOff_Inven(false);
		}
	}

	__super::Tick(fTimeDelta);
}

void CTab_Window::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
		return;

	__super::Late_Tick(fTimeDelta);

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CTab_Window::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
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
}
