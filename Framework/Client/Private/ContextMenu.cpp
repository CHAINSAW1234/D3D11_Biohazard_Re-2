#include "stdafx.h"

#include "ContextMenu.h"
#include "Button_UI.h"

CContextMenu::CContextMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice , pContext }
{
}

CContextMenu::CContextMenu(const CContextMenu& rhs)
	: CGameObject{ rhs }
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

	if(FAILED(Create_MenuItem()))
		return E_FAIL;

	m_eContext_State = UI_IDLE;

	return S_OK;
}

void CContextMenu::Tick(_float fTimeDelta)
{
	if (true == m_bDead)
	{
		for (auto& iter : m_vecMenuItem)
			iter->Set_Dead(true);

		return;
	}

	switch (m_eContext_State)
	{
	case Client::POP_UP: {
		PopUp_Operation(fTimeDelta);
		break;
	}
		
	case Client::UI_IDLE: {
		Idle_Operation(fTimeDelta);
		break;
	}
		
	case Client::HIDE: {
		Hide_Operation(fTimeDelta);
		break;
	}

	default:
		break;
	}



}

void CContextMenu::Late_Tick(_float fTimeDelta)
{
	if (true == m_bDead)
	{
		for (auto& iter : m_vecMenuItem)
			iter->Set_Dead(true);

		return;
	}
}

HRESULT CContextMenu::Render()
{
	return S_OK;
}

void CContextMenu::PopUp_Operation(_float fTimeDelta)
{

}

void CContextMenu::Idle_Operation(_float fTimeDelta)
{
	
}

void CContextMenu::Hide_Operation(_float fTimeDelta)
{
	for (auto& iter : m_vecMenuItem)
	{
		iter->Set_Dead(true);
	}
}

void CContextMenu::Set_Operation(ITEM_TYPE eItemType, _bool bActive, _float2 fAppearPos, _float2 fArrivalPos)
{
	m_bDead = false;
	m_fAppearPos = fAppearPos;
	m_fArrivalPos = fArrivalPos;
	m_eContext_State = POP_UP;

	switch (eItemType)
	{
	case Client::EQUIPABLE: { //bActive가 장착 여부임
		m_iContextMenuCount = 4;
		
		m_eContextType = eItemType;

		if (false == bActive)
		{
			m_vecMenuItem[0]->Set_TextureNum(0);
			m_vecMenuItem[0]->Set_Text(0, TEXT("장착"));
		}

		else
		{
			m_vecMenuItem[0]->Set_TextureNum(1);
			m_vecMenuItem[0]->Set_Text(0, TEXT("장착 해제"));
		}

		m_vecMenuItem[1]->Set_TextureNum(3);
		m_vecMenuItem[1]->Set_Text(0, TEXT("검사"));

		m_vecMenuItem[2]->Set_TextureNum(2);
		m_vecMenuItem[2]->Set_Text(0, TEXT("조합"));

		m_vecMenuItem[3]->Set_TextureNum(5);
		m_vecMenuItem[3]->Set_Text(0, TEXT("단축키"));

		break;
	}
		
	case Client::CONSUMABLE_EQUIPABLE: { //bActive가 장착 여부임
		m_iContextMenuCount = 4;

		m_eContextType = eItemType;

		if (false == bActive)
		{
			m_vecMenuItem[0]->Set_TextureNum(9);
			m_vecMenuItem[0]->Set_Text(0, TEXT("장착"));
		}

		else
		{
			m_vecMenuItem[0]->Set_TextureNum(10);
			m_vecMenuItem[0]->Set_Text(0, TEXT("장착 해제"));
		}

		m_vecMenuItem[1]->Set_TextureNum(3);
		m_vecMenuItem[1]->Set_Text(0, TEXT("검사"));

		m_vecMenuItem[2]->Set_TextureNum(2);
		m_vecMenuItem[2]->Set_Text(0, TEXT("조합"));

		m_vecMenuItem[3]->Set_TextureNum(5);
		m_vecMenuItem[3]->Set_Text(0, TEXT("단축키"));

		break;
	}
		
	case Client::USEABLE: { //bActive가 사용 가능 여부임
		m_iContextMenuCount = 4;

		m_eContextType = eItemType;

		m_vecMenuItem[0]->Set_TextureNum(4);
		m_vecMenuItem[0]->Set_Text(0, TEXT("사용"));

		if (false == bActive)
		{
			m_vecMenuItem[0]->Frame_Change_ValueColor(0);
		}

		else
		{
			m_vecMenuItem[0]->Frame_Change_ValueColor(1);
		}

		m_vecMenuItem[1]->Set_TextureNum(3);
		m_vecMenuItem[1]->Set_Text(0, TEXT("검사"));
		m_vecMenuItem[0]->Frame_Change_ValueColor(1);

		m_vecMenuItem[2]->Set_TextureNum(2);
		m_vecMenuItem[2]->Set_Text(0, TEXT("조합"));
		m_vecMenuItem[0]->Frame_Change_ValueColor(1);

		m_vecMenuItem[3]->Set_TextureNum(5);
		m_vecMenuItem[3]->Set_Text(0, TEXT("폐기"));
		m_vecMenuItem[0]->Frame_Change_ValueColor(1);
		break;
	}
		
	case Client::CONSUMABLE: { // 액티브 관계없음
		m_iContextMenuCount = 3;

		m_eContextType = eItemType;

		m_vecMenuItem[0]->Set_TextureNum(4);
		m_vecMenuItem[0]->Set_Text(0, TEXT("사용"));
		m_vecMenuItem[0]->Frame_Change_ValueColor(1);

		m_vecMenuItem[1]->Set_TextureNum(3);
		m_vecMenuItem[1]->Set_Text(0, TEXT("검사"));
		m_vecMenuItem[1]->Frame_Change_ValueColor(1);

		m_vecMenuItem[2]->Set_TextureNum(2);
		m_vecMenuItem[2]->Set_Text(0, TEXT("조합"));
		m_vecMenuItem[2]->Frame_Change_ValueColor(1);

		m_vecMenuItem[3]->Set_TextureNum(5);
		m_vecMenuItem[3]->Set_Text(0, TEXT("폐기"));
		m_vecMenuItem[3]->Frame_Change_ValueColor(1);

		break;
	}
		
	case Client::QUEST: {
		m_iContextMenuCount = 1;

		m_eContextType = eItemType;

		m_vecMenuItem[0]->Set_TextureNum(3);
		m_vecMenuItem[0]->Set_Text(0, TEXT("검사"));
		m_vecMenuItem[0]->Frame_Change_ValueColor(1);

		break;
	}
		
	default:
		break;
	}

	for (_uint i = 0; i < m_iContextMenuCount; i++)
	{
		m_vecMenuItem[i]->Set_Dead(false);
	}

}

HRESULT CContextMenu::Create_MenuItem()
{
	ifstream inputFileStream;
	wstring selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/ContextMenu.dat");

	for (_uint i = 0; i < 4; i++)
	{
		CButton_UI* pButton = { nullptr };

		inputFileStream.open(selectedFilePath, ios::binary);
		if (FAILED(CCustomize_UI::CreatUI_FromDat(inputFileStream, nullptr, TEXT("Prototype_GameObject_Button_UI"),
			(CGameObject**)&pButton, m_pDevice, m_pContext)))
			return E_FAIL;

		m_vecMenuItem.push_back(pButton);

		pButton->Move_State(_float3(0.f, m_fItemInterval * i, 0.f), 0);
		pButton->Set_IsLoad(false);
		pButton->Set_Dead(true);
	}


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
