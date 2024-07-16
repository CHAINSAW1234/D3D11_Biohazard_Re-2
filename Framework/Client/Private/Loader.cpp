#include "stdafx.h"
#include "..\Public\Loader.h"
#include <process.h>

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Camera_Event.h"
#include "Camera_Gimmick.h"

/* Player */
#include "Player.h"
#include "Body_Player.h"
#include "Head_Player.h"
#include "Hair_Player.h"
#include "Weapon.h"
#include "FlashLight.h"
#include "Throwing_Weapon.h"
#include "Throwing_Weapon_Pin.h"

/* Monster */
#include "Body_Zombie.h"
#include "Face_Zombie.h"
#include "Clothes_Zombie.h"

#include "BackGround.h"
#include "Terrain.h"
#include "Zombie.h"


#include "Effect.h"
#include "Sky.h"
#include "CustomCollider.h"
#include "NavMesh_Debug.h"

/* Actor */
#include "Actor_PL78.h"
#include "Actor_PL00.h"
#include "Actor_EM00.h"

/* CutScene */
#include "Cut_Scene_CF93.h"

/* Decal*/
#include "Decal_SSD.h"

/* MapObject*/
#include "Props.h"
#include "Map.h"
#include "Door.h"
#include "Body_Door.h"
#include "Cabinet.h"
#include "Body_Cabinet.h"
#include"Lock_Cabinet.h"
#include"Card_Cabinet.h"
#include "Window.h"
#include "Body_Window.h"
#include "Pannel_Window.h"
#include "NewpoliceStatue.h"
#include "Medal_NewpoliceStatue.h"
#include "Body_NewpoliceStatue.h"
#include "ItemProp.h"
#include "Body_ItemProp.h"
#include "EventProp.h"
#include "Body_EventProp.h"
#include "Statue.h"
#include "Body_Statue.h"
#include "Shutter.h"
#include "Body_Shutter.h"
#include "ItemLocker.h"
#include "Body_ItemLocker.h"
#include "Ladder.h"
#include "Body_Ladder.h"
#include "ReaderMachine.h"
#include "Body_ReaderMachine.h"
#include "Key_ReaderMachine.h"
#include "MovingShelf.h"
#include "Body_MovingShelf.h"
#include "Lever.h"
#include "Body_Lever.h"
#include "Item_Statue.h"
#include "Emblem_Door.h"
#include "Key_Door.h"
/* UI */
#include "Customize_UI.h"
#include "Inventory_Item_UI.h"
#include "Tab_Window.h"
#include "Button_UI.h"
#include "Inventory_Slot.h"
#include "Crosshair_UI.h"
#include "Cursor_UI.h"
#include "HPBar_UI.h"
#include "Bullet_UI.h"
#include "Title_UI.h"
#include "MissionBar_UI.h"
#include "Tutorial_UI.h"
#include "Selector_UI.h"
#include "Slot_Highlighter.h"
#include "Item_UI.h"
#include "Player_Map_UI.h"
#include "Main_Map_UI.h"
#include "Targeting_Map_UI.h"
#include "Static_Map_UI.h"
#include "Item_Map_UI.h"
#include "Floor_Map_UI.h"
#include "Item_Mesh_Viewer.h"
#include "ContextMenu.h"
#include "Context_Highlighter.h"
#include "Read_Item_UI.h"
#include "Loading_UI.h"
#include "Item_Discription.h"
#include "LayOut_UI.h"
#include "Damage_UI.h"
#include "HotKey.h"

#include "Hint.h"
#include "Hint_Directory.h"
#include "Hint_Display.h"
#include "Hint_Highliter.h"
#include "Hint_Blind.h"

// EnvMap 큐브맵이라는뜻
#include"EnvCube.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	CLoader* pLoader = (CLoader*)pArg;

	if (FAILED(pLoader->Start()))
		return 1;

	CoUninitialize();

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_Critical_Section);

	/* 스레드를 생성하낟. */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Start()
{
	EnterCriticalSection(&m_Critical_Section);

	HRESULT		hr = { 0 };

	static int isStart = true;
	if (isStart) {
		g_Level = LEVEL_STATIC;
		Load_Prototype();
		Loading_For_Static_Component();
		isStart = false;
	}

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		g_Level = LEVEL_LOGO;
		hr = Loading_For_Logo();
		break;
	case LEVEL_GAMEPLAY:
		g_Level = LEVEL_GAMEPLAY;

	/*	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
			return E_FAIL;*/

		hr = Loading_For_GamePlay();
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	LeaveCriticalSection(&m_Critical_Section);

	return S_OK;
}


HRESULT CLoader::Ready_Layer_UI(const wstring& strLayerTag)
{
	ifstream inputFileStream;
	wstring selectedFilePath;

	/* UI_Loading */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Loading.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* UI_Loading_Text */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Loading_Text.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);
	
	return S_OK;

}

void CLoader::UI_Distinction(wstring& selectedFilePath)
{
	/* 판별*/
	wstring FileName;
	size_t lastSlashPos = (selectedFilePath).rfind(L'/');
	if (!(lastSlashPos == wstring::npos)) {
		FileName = (selectedFilePath).substr(lastSlashPos + 1);
	}

	size_t lastDotPos = FileName.rfind(L'.');
	if (lastDotPos != std::wstring::npos) {
		FileName = FileName.substr(0, lastDotPos);
	}
	selectedFilePath = FileName;
}

void CLoader::CreatFromDat(ifstream& inputFileStream, wstring strListName, CGameObject* pGameParentsObj, wstring fileName, _int iWhich_Child)
{
	CCustomize_UI::CUSTOM_UI_DESC CustomizeUIDesc;

	if(TEXT("UI_Loading_Text") != fileName)
		inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.eMapUI_Type), sizeof(LOCATION_MAP_VISIT));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isLoad), sizeof(_bool));

	_char DefaultTexturePath[MAX_PATH] = "";
	_char DefaultTextureTag[MAX_PATH] = "";
	_char MaskTexturePath[MAX_PATH] = "";
	_char MaskTextureTag[MAX_PATH] = "";

	inputFileStream.read(reinterpret_cast<_char*>(DefaultTexturePath), sizeof(_char) * MAX_PATH);
	inputFileStream.read(reinterpret_cast<_char*>(DefaultTextureTag), sizeof(_char) * MAX_PATH);
	inputFileStream.read(reinterpret_cast<_char*>(MaskTexturePath), sizeof(_char) * MAX_PATH);
	inputFileStream.read(reinterpret_cast<_char*>(MaskTextureTag), sizeof(_char) * MAX_PATH);

	CustomizeUIDesc.wstrDefaultTexturPath = wstring(DefaultTexturePath, DefaultTexturePath + strlen(DefaultTexturePath));
	CustomizeUIDesc.wstrDefaultTexturComTag = wstring(DefaultTextureTag, DefaultTextureTag + strlen(DefaultTextureTag));
	CustomizeUIDesc.wstrMaskPath = wstring(MaskTexturePath, MaskTexturePath + strlen(MaskTexturePath));
	CustomizeUIDesc.wstrMaskComTag = wstring(MaskTextureTag, MaskTextureTag + strlen(MaskTextureTag));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.worldMatrix), sizeof(_float4x4));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.vSize), sizeof(_float2));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isPlay), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.fColorTimer_Limit), sizeof(_float));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.iEndingType), sizeof(_int));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.fMaxFrame), sizeof(_float));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isFrame), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isLoopStart), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isLoop), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isLoopStop), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.ReStart), sizeof(_bool));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.iColorMaxNum), sizeof(_uint));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.iTextBoxCount), sizeof(_uint));

	for (_int i = 0; i <= (_int)CustomizeUIDesc.iColorMaxNum; i++)
	{
		inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.SavePos[i]), sizeof(_float4x4));

		inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.vColor[i]), sizeof(CCustomize_UI::Value_Color));

		inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.Mask[i]), sizeof(CCustomize_UI::Value_Mask));
	}

	for (_int i = 0; i < (_int)CustomizeUIDesc.iTextBoxCount; i++)
	{
		CTextBox::TextBox_DESC TextBoxDesc = {};

		_tchar FontString[MAX_PATH] = L"";

		_char FontType[MAX_PATH] = "";

		inputFileStream.read(reinterpret_cast<_char*>(FontString), sizeof(_tchar) * MAX_PATH);

		inputFileStream.read(reinterpret_cast<_char*>(FontType), sizeof(_char) * MAX_PATH);

		TextBoxDesc.wstrText = FontString;

		TextBoxDesc.wstrFontType = wstring(FontType, FontType + strlen(FontType));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.vFontColor), sizeof(_vector));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.iFontSize), sizeof(_uint));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.vPos), sizeof(_float3));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.vSize), sizeof(_float2));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.isOuterLine), sizeof(_bool));

		inputFileStream.read(reinterpret_cast<_char*>(&TextBoxDesc.vOutLineColor), sizeof(_vector));


		CustomizeUIDesc.vecTextBoxDesc.push_back(TextBoxDesc);
	}

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.IsChild), sizeof(_bool));

	if (0 == CustomizeUIDesc.fMaxFrame && TEXT("") != CustomizeUIDesc.wstrDefaultTexturPath)
	{
		/* For.Prototype_Component_Texture_ */
		if (FAILED(m_pGameInstance->Add_Prototype(g_Level, CustomizeUIDesc.wstrDefaultTexturComTag,
			CTexture::Create(m_pDevice, m_pContext, CustomizeUIDesc.wstrDefaultTexturPath)))) {
			int a = 0;
		}
	}

	else if (0 < CustomizeUIDesc.fMaxFrame && TEXT("") != CustomizeUIDesc.wstrDefaultTexturPath)
	{
		/* For.Prototype_Component_Texture_ */
		if (FAILED(m_pGameInstance->Add_Prototype(g_Level, CustomizeUIDesc.wstrDefaultTexturComTag,
			CTexture::Create(m_pDevice, m_pContext, CustomizeUIDesc.wstrDefaultTexturPath, (_uint)CustomizeUIDesc.fMaxFrame)))) {
			int a = 0;
		}
	}

	if (TEXT("") != CustomizeUIDesc.wstrMaskPath)
	{
		/* For.Prototype_Component_Texture_ */
		if (FAILED(m_pGameInstance->Add_Prototype(g_Level, CustomizeUIDesc.wstrMaskComTag,
			CTexture::Create(m_pDevice, m_pContext, CustomizeUIDesc.wstrMaskPath)))) {
			int a = 0;
		}
	}

	if (false == CustomizeUIDesc.IsChild)
		CustomizeUIDesc.iWhich_Child = 0;
	else
		CustomizeUIDesc.iWhich_Child = iWhich_Child;

	CustomizeUIDesc.wstrFileName = fileName;
	if (FAILED(m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Loading_UI"), &CustomizeUIDesc)))
		MSG_BOX(TEXT("Failed to Add Clone"));

	CGameObject* pGameObj = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"))->back();
	CLoading_UI* pLoading = static_cast<CLoading_UI*>(pGameObj);

	m_eLoadingList.push_back(pLoading);

	if (nullptr != pGameParentsObj)
	{
		dynamic_cast<CCustomize_UI*>(pGameParentsObj)->PushBack_Child(pGameObj);
	}

	/*자식이 있으면 자식 정보 read*/
	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.iChild), sizeof(_int));


	if (true == 0 < CustomizeUIDesc.iChild)
	{
		for (_int i = 0; i < CustomizeUIDesc.iChild; i++)
		{
			CreatFromDat(inputFileStream, strListName, pGameObj, fileName, iWhich_Child + 1);
		}
	}

	if (nullptr == pGameParentsObj)
	{
		inputFileStream.close();
	}
}

HRESULT CLoader::Load_Prototype()
{
	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Event"),
		CCamera_Event::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Camera_Gimmick */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Gimmick"),
		CCamera_Gimmick::Create(m_pDevice, m_pContext))))
		return E_FAIL;


#pragma region Player
	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Part_Body_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Part_Body_Player"),
		CBody_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Part_Head_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Part_Head_Player"),
		CHead_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Part_Hair_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Part_Hair_Player"),
		CHair_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Part_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Part_Weapon"),
		CWeapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Throwing_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Throwing_Weapon"),
		CThrowing_Weapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Throwing_Weapon_Pin */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Throwing_Weapon_Pin"),
		CThrowing_Weapon_Pin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Part_FlashLight */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Part_FlashLight"),
		CFlashLight::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion
	
#pragma region Monster

	/* For.Prototype_GameObject_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Zombie"),
		CZombie::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Part_Body_Zombie"),
		CBody_Zombie::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Part_Face_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Part_Face_Zombie"),
		CFace_Zombie::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Part_Clothes_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Part_Clothes_Zombie"),
		CClothes_Zombie::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region SSD_DECAL
	/* For.Prototype_GameObject_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Decal_SSD"),
		CDecal_SSD::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion

#pragma region UI
	/* UI Object */
	/* For.Prototype_GameObject_Crosshair_UI*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Crosshair_UI"),
		CCrosshair_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CNavigation_Debug */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NavMesh_Debug"),
		CNavMesh_Debug::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Cursor_UI*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cursor_UI"),
		CCursor_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Inventory_Item_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Inventory_Item_UI"),
		CInventory_Item_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Inventory_Item_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HPBar_UI"),
		CHPBar_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Inventory_Item_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet_UI"),
		CBullet_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Inventory_Item_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Title_UI"),
		CTitle_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_TextBox */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MissionBar_UI"),
		CMissionBar_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_TextBox */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TextBox"),
		CTextBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Tab_Window */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tab_Window"),
		CTab_Window::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Tutorial_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Tutorial_UI"),
		CTutorial_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Tutorial_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Selector_UI"),
		CSelector_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Button_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Button_UI"),
		CButton_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_InventorySlot */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_InventorySlot"),
		CInventory_Slot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_SlotHighlighter */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SlotHighlighter"),
		CSlot_Highlighter::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ItemUI */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ItemUI"),
		CItem_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Main_Map_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Main_Map_UI"),
		CMain_Map_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Player_Map_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Map_UI"),
		CPlayer_Map_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Targeting_Map_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Targeting_Map_UI"),
		CTargeting_Map_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Targeting_Map_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Static_Map_UI"),
		CStatic_Map_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Item_Map_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Map_UI"),
		CItem_Map_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Floor_Map_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Floor_Map_UI"),
		CFloor_Map_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Item_Mesh_Viewer */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Mesh_Viewer"),
		CItem_Mesh_Viewer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ContextMenu */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ContextMenu"),
		CContextMenu::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ContextMenu */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Context_Highlighter"),
		CContext_Highlighter::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Read_Item_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Read_Item_UI"),
		CRead_Item_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Item_Discription */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Discription"),
		CItem_Discription::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_LayOut_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LayOut_UI"),
		CLayOut_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Damage_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Damage_UI"),
		CDamage_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_HotKey */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HotKey"),
		CHotKey::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_Hint */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hint"),
		CHint::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Hint_Directory */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hint_Directory"),
		CHint_Directory::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Hint_Display */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hint_Display"),
		CHint_Display::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Hint_Highliter */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hint_Highliter"),
		CHint_Highliter::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Hint_Blind */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hint_Blind"),
		CHint_Blind::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

#pragma region CubeMap
	/* For.Prototype_GameObject_EnvCube */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnvCube"),
		CEnvCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion
	/* For.Prototype_GameObject_Collider */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Collider"),
		CCustomCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CLoader::Loading_For_Static_Component()
{
	/* Prototype_Component_VIBuffer_Cube */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_FSM"),
		CFSM::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region Collider
	m_strLoadingText = TEXT("Now Loading ... Collider");

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;
#pragma endregion

#pragma region 셰이더
	m_strLoadingText = TEXT("Now Loading ... Shader");

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxNorTex */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxCube */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::Elements, VTXCUBE::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAlphaSortTex */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Shader_VtxAlphaSortTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAlphaSortTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Shader_VtxInstance_Rect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Rect.hlsl"), VTXINSTANCE_RECT::Elements, VTXINSTANCE_RECT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxInstance_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Point.hlsl"), VTXINSTANCE_POINT::Elements, VTXINSTANCE_POINT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Effect */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Shader_Effect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Effect.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;
#pragma endregion


#pragma region 텍스쳐
	/*Prototype_Component_Texture_Items*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Items"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Items/Item_%d.png"), 74))))
		return E_FAIL;

	/*Prototype_Component_Texture_Filled*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Filled"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/Box_Store.png")))))
		return E_FAIL;

	/*Prototype_Component_Texture_Filled*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_WholeMouse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/WholeMouse.png")))))
		return E_FAIL;

	/*Prototype_Component_Texture_Hint_BackGround*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Hint_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Hint/Hint_BackGround.dds")))))
		return E_FAIL;

	/*Prototype_Component_Texture_Long_Box_Select_Click*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Long_Box_Select_Click"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/Long_Box_Select_Click.png")))))
		return E_FAIL;

#pragma endregion

	return S_OK;
}

HRESULT CLoader::Load_Field_Prototype(const wstring& filePath)
{
	HANDLE		hFile = CreateFile(filePath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD	dwByte(0);

	_uint iObjectNum = { 0 };
	if (!ReadFile(hFile, &iObjectNum, sizeof(_uint), &dwByte, nullptr))
		return E_FAIL;

	//vector<wstring> ItemModelTags; // 창균

	for (_uint i = 0; iObjectNum > i; ++i)
	{
		
		PROTOTYPE_INFORM* Inform = new PROTOTYPE_INFORM;

		_uint dwLen = { 0 };

		_bool bAnim = { false };
		if (!ReadFile(hFile, &bAnim, sizeof(_uint), &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			CloseHandle(hFile);
			return E_FAIL;
		}
		Inform->bAnim = bAnim;


		if (!ReadFile(hFile, &dwLen, sizeof(_uint), &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			CloseHandle(hFile);
			return E_FAIL;
		}
		char* strModelPath = new char[dwLen / sizeof(char) + 1];
		if (!ReadFile(hFile, strModelPath, dwLen, &dwByte, nullptr))
		{
			delete[] strModelPath;
			Safe_Delete(Inform);
			CloseHandle(hFile);

			return E_FAIL;
		}
		strModelPath[dwLen / sizeof(char)] = '\0';
		Inform->strModelPath = strModelPath;
		delete[] strModelPath;


		if (!ReadFile(hFile, &dwLen, sizeof(_uint), &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			CloseHandle(hFile);
			return E_FAIL;
		}
		wchar_t* wstrModelPrototypeName = new wchar_t[dwLen / sizeof(wchar_t) + 1];
		if (!ReadFile(hFile, wstrModelPrototypeName, dwLen, &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			delete[] wstrModelPrototypeName;
			CloseHandle(hFile);
			return E_FAIL;
		}
		wstrModelPrototypeName[dwLen / sizeof(wchar_t)] = L'\0';
		Inform->wstrModelPrototypeName = wstrModelPrototypeName;
		delete[] wstrModelPrototypeName;

		if (!ReadFile(hFile, &dwLen, sizeof(_uint), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			Safe_Delete(Inform);
			return E_FAIL;
		}
		wchar_t* wstrGameObjectPrototypeName = new wchar_t[dwLen / sizeof(wchar_t) + 1];
		if (!ReadFile(hFile, wstrGameObjectPrototypeName, dwLen, &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			delete[] wstrGameObjectPrototypeName;
			CloseHandle(hFile);
			return E_FAIL;
		}
		wstrGameObjectPrototypeName[dwLen / sizeof(wchar_t)] = L'\0';
		Inform->wstrGameObjectPrototypeName = wstrGameObjectPrototypeName;
		delete[] wstrGameObjectPrototypeName;

		if (!ReadFile(hFile, &dwLen, sizeof(_uint), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			Safe_Delete(Inform);
			return E_FAIL;
		}
		char* strGameObjectPrototypeName = new char[dwLen / sizeof(char) + 1];
		if (!ReadFile(hFile, strGameObjectPrototypeName, dwLen, &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			delete[] strGameObjectPrototypeName;

			CloseHandle(hFile);
			return E_FAIL;
		}
		strGameObjectPrototypeName[dwLen / sizeof(char)] = '\0';
		Inform->strGameObjectPrototypeName = strGameObjectPrototypeName;
		delete[] strGameObjectPrototypeName;

		 

		if (Inform->strGameObjectPrototypeName.find("_Anim") != string::npos)
		{
			Inform->bAnim = true;

			_matrix Ininitmatrix  = XMMatrixRotationY(XMConvertToRadians(180.f));
			/*if(Inform->wstrGameObjectPrototypeName.find(TEXT("sm40_007")) != wstring::npos)
				m_pGameInstance->Add_Prototype(m_eNextLevelID , Inform->wstrModelPrototypeName, CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Inform->strModelPath.c_str(), Ininitmatrix));
			else*/

			m_pGameInstance->Add_Prototype(m_eNextLevelID , Inform->wstrModelPrototypeName, CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Inform->strModelPath.c_str(), Ininitmatrix));
		}
		else
		{
			m_pGameInstance->Add_Prototype(m_eNextLevelID, Inform->wstrModelPrototypeName, CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Inform->strModelPath.c_str(), XMMatrixIdentity()));

		}

		_bool bDo = { false };

		if (!bDo && (Inform->wstrGameObjectPrototypeName.find(TEXT("sm40_016")) != wstring::npos) && (bDo = true))
			m_pGameInstance->Add_Prototype(Inform->wstrGameObjectPrototypeName, CBody_Ladder::Create(m_pDevice, m_pContext));

		if (!bDo && Inform->bAnim)
		{
			if (!bDo && (Inform->wstrGameObjectPrototypeName.find(TEXT("zombiewindow")) != wstring::npos) && (bDo = true))
				m_pGameInstance->Add_Prototype(Inform->wstrGameObjectPrototypeName, CBody_Window::Create(m_pDevice, m_pContext));
			
			if (!bDo && (Inform->wstrGameObjectPrototypeName.find(TEXT("sm41_022")) != wstring::npos) && (bDo = true))
				m_pGameInstance->Add_Prototype(Inform->wstrGameObjectPrototypeName, CBody_Statue::Create(m_pDevice, m_pContext));

			if (!bDo && (Inform->wstrGameObjectPrototypeName.find(TEXT("sm40")) != wstring::npos) && (bDo = true))
				m_pGameInstance->Add_Prototype(Inform->wstrGameObjectPrototypeName, CBody_Door::Create(m_pDevice, m_pContext));

			if (!bDo && (Inform->wstrGameObjectPrototypeName.find(TEXT("sm42_162")) != wstring::npos) && (bDo = true))
				m_pGameInstance->Add_Prototype(Inform->wstrGameObjectPrototypeName, CBody_Lever::Create(m_pDevice, m_pContext));

			if (!bDo && (Inform->wstrGameObjectPrototypeName.find(TEXT("sm42_197")) != wstring::npos || Inform->wstrGameObjectPrototypeName.find(TEXT("sm42_232")) != wstring::npos) && (bDo = true))
				m_pGameInstance->Add_Prototype(Inform->wstrGameObjectPrototypeName, CBody_MovingShlef::Create(m_pDevice, m_pContext));
			
			if (!bDo && (Inform->wstrGameObjectPrototypeName.find(TEXT("sm41_006")) != wstring::npos) && (bDo = true))
				m_pGameInstance->Add_Prototype(Inform->wstrGameObjectPrototypeName, CBody_ReaderMachine::Create(m_pDevice, m_pContext));

			if (!bDo && (Inform->wstrGameObjectPrototypeName.find(TEXT("sm41_024_newpolicestatue01a")) != wstring::npos) && (bDo = true))
				m_pGameInstance->Add_Prototype(Inform->wstrGameObjectPrototypeName, CBody_NewpoliceStatue::Create(m_pDevice, m_pContext));
			if (!bDo &&
				((Inform->wstrGameObjectPrototypeName.find(TEXT("sm44")) != wstring::npos)
					|| (Inform->wstrGameObjectPrototypeName.find(TEXT("sm41_011")) != wstring::npos))
				&& (bDo = true))
				m_pGameInstance->Add_Prototype(Inform->wstrGameObjectPrototypeName, CBody_Cabinet::Create(m_pDevice, m_pContext));

			if (!bDo && (Inform->wstrGameObjectPrototypeName.find(TEXT("sm7")) != wstring::npos) && (bDo = true))
				m_pGameInstance->Add_Prototype(Inform->wstrGameObjectPrototypeName, CBody_ItemProp::Create(m_pDevice, m_pContext));
			if (!bDo && (
				(Inform->wstrGameObjectPrototypeName.find(TEXT("sm42_003")) != wstring::npos) ||
				(Inform->wstrGameObjectPrototypeName.find(TEXT("sm42_010")) != wstring::npos) ||
				(Inform->wstrGameObjectPrototypeName.find(TEXT("sm60_034")) != wstring::npos) ||
				(Inform->wstrGameObjectPrototypeName.find(TEXT("sm60_033")) != wstring::npos)
				) && (bDo = true))
				m_pGameInstance->Add_Prototype(Inform->wstrGameObjectPrototypeName, CBody_Shutter::Create(m_pDevice, m_pContext));
			if(!bDo && (bDo = true))
				m_pGameInstance->Add_Prototype(Inform->wstrGameObjectPrototypeName, CBody_EventProp::Create(m_pDevice, m_pContext));
		}
		else
		{
				
			if (!bDo && (Inform->wstrGameObjectPrototypeName.find(TEXT("sm7")) != wstring::npos) && (bDo = true))
				m_pGameInstance->Add_Prototype(Inform->wstrGameObjectPrototypeName, CBody_ItemProp::Create(m_pDevice, m_pContext));

			if (!bDo && (Inform->wstrGameObjectPrototypeName.find(TEXT("Merge")) != wstring::npos) && (bDo = true))
				m_pGameInstance->Add_Prototype(Inform->wstrGameObjectPrototypeName, CMap::Create(m_pDevice, m_pContext));

			if (!bDo)
				m_pGameInstance->Add_Prototype(Inform->wstrGameObjectPrototypeName, CProps::Create(m_pDevice, m_pContext));
		}
		

		//
		//if (Inform->wstrGameObjectPrototypeName.find(TEXT("sm7")) != wstring::npos)
		//{
		//	ItemModelTags.push_back(Inform->wstrModelPrototypeName);//창균
		//}

		//if (Inform->wstrGameObjectPrototypeName.find(TEXT("sm71_901")) != wstring::npos)
		//{
		//	int a = 0;
		//}

		Safe_Delete(Inform);

	}
	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ZombieWindow"), CWindow::Create(m_pDevice, m_pContext));
	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Door"), CDoor::Create(m_pDevice, m_pContext));
	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NewPoliceStatue"), CNewpoliceStatue::Create(m_pDevice, m_pContext));
	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cabinet"), CCabinet::Create(m_pDevice, m_pContext));

	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Shutter"), CShutter::Create(m_pDevice, m_pContext));
	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Statue"), CStatue::Create(m_pDevice, m_pContext));
	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EventProp"), CEventProp::Create(m_pDevice, m_pContext));
	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ItemProp"), CItemProp::Create(m_pDevice, m_pContext));

	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ItemLocker"), CItemLocker::Create(m_pDevice, m_pContext));
	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Ladder"), CLadder::Create(m_pDevice, m_pContext));

	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lever"), CLever::Create(m_pDevice, m_pContext));
	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MovingShelf"), CMovingShelf::Create(m_pDevice, m_pContext));
	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Reader"), CReaderMachine::Create(m_pDevice, m_pContext));


	_matrix Ininitmatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_sm40_034_windowbarricade01a"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM,"..\\Bin\\Resources\\Models\\Map\\Prop\\Gimmick\\sm40\\sm40_034_windowbarricade01a.fbx", Ininitmatrix));
	m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_sm40_035_windowoldbarricade01a"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "..\\Bin\\Resources\\Models\\Map\\Prop\\Gimmick\\sm40\\sm40_035_windowoldbarricade01a.fbx", Ininitmatrix));
	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PannelWindow"), CPannel_Window::Create(m_pDevice, m_pContext));
	
	m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_sm42_019_safeboxdial01a_Anim"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "..\\Bin\\Resources\\Models\\Map\\Prop\\Gimmick\\sm42\\sm42_019_safeboxdial01a_Anim.fbx", Ininitmatrix));
	m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_sm42_014_diallock01a_Anim"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "..\\Bin\\Resources\\Models\\Map\\Prop\\Gimmick\\sm42\\sm42_014_diallock01a_Anim.fbx", Ininitmatrix));
	m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_sm42_174_cardreader04a_Anim"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "..\\Bin\\Resources\\Models\\Map\\Prop\\Gimmick\\sm42\\sm42_174_cardreader04a_Anim.fbx", Ininitmatrix));
	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lock_Cabinet"), CLock_Cabinet::Create(m_pDevice, m_pContext));
	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Card_Cabinet"), CCard_Cabinet::Create(m_pDevice, m_pContext));

	/* Medal*/
	m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_sm73_102_unicornmedal01a"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "..\\Bin\\Resources\\Models\\Map\\Item\\sm73\\sm73_102_unicornmedal01a.fbx", Ininitmatrix));
	m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_sm73_139_virginmedal01a"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "..\\Bin\\Resources\\Models\\Map\\Item\\sm73\\sm73_139_virginmedal01a.fbx", Ininitmatrix));
	m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_sm73_145_virginmedal02a"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "..\\Bin\\Resources\\Models\\Map\\Item\\sm73\\sm73_145_virginmedal02a.fbx", Ininitmatrix));
	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Medal_NewpoliceStatue"), CMedal_NewpoliceStatue::Create(m_pDevice, m_pContext));

	/* Statue*/
	m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_sm73_109_kingscepter01a_Anim"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "..\\Bin\\Resources\\Models\\Map\\Item\\sm73\\sm73_109_kingscepter01a_Anim.fbx", Ininitmatrix));
	m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_sm73_136_statuehand01a_Anim"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "..\\Bin\\Resources\\Models\\Map\\Item\\sm73\\sm73_136_statuehand01a_Anim.fbx", Ininitmatrix));
	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Medal_Body_ItemStatue"), CItem_Statue::Create(m_pDevice, m_pContext));
	
	m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_sm42_020_keystrokedevice01a_Anim"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "..\\Bin\\Resources\\Models\\Map\\Prop\\Gimmick\\sm42\\sm42_020_keystrokedevice01a_Anim.fbx", Ininitmatrix));
	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Key_Reader"), CKey_ReaderMachine::Create(m_pDevice, m_pContext));

	/* Door Emblem */
	/* 1. Space */
	m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_sm40_500_dooremblem01a_00md_Anim"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "..\\Bin\\Resources\\Models\\Map\\Prop\\Gimmick\\sm40\\sm40_500_dooremblem01a_00md_Anim.fbx", Ininitmatrix));
	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Medal_EmblemDoor"), CEmblem_Door::Create(m_pDevice, m_pContext));
	
	m_pGameInstance->Add_Prototype(m_eNextLevelID, TEXT("Prototype_Component_Model_sm73_103_spadekey01a"), CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "..\\Bin\\Resources\\Models\\Map\\Item\\sm73\\sm73_103_spadekey01a.fbx", Ininitmatrix));
	m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Medal_KeyDoor"), CKey_Door::Create(m_pDevice, m_pContext));

	CloseHandle(hFile);
	return S_OK;
}

HRESULT CLoader::Load_Item_Prototype(const wstring& filePath)
{
	HANDLE		hFile = CreateFile(filePath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD	dwByte(0);

	_uint iObjectNum = { 0 };
	if (!ReadFile(hFile, &iObjectNum, sizeof(_uint), &dwByte, nullptr))
		return E_FAIL;

	//vector<wstring> ItemModelTags; // 창균

	for (_uint i = 0; iObjectNum > i; ++i)
	{

		PROTOTYPE_INFORM* Inform = new PROTOTYPE_INFORM;

		_uint dwLen = { 0 };

		_bool bAnim = { false };
		if (!ReadFile(hFile, &bAnim, sizeof(_uint), &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			CloseHandle(hFile);
			return E_FAIL;
		}
		Inform->bAnim = bAnim;


		if (!ReadFile(hFile, &dwLen, sizeof(_uint), &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			CloseHandle(hFile);
			return E_FAIL;
		}
		char* strModelPath = new char[dwLen / sizeof(char) + 1];
		if (!ReadFile(hFile, strModelPath, dwLen, &dwByte, nullptr))
		{
			delete[] strModelPath;
			Safe_Delete(Inform);
			CloseHandle(hFile);

			return E_FAIL;
		}
		strModelPath[dwLen / sizeof(char)] = '\0';
		Inform->strModelPath = strModelPath;
		delete[] strModelPath;


		if (!ReadFile(hFile, &dwLen, sizeof(_uint), &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			CloseHandle(hFile);
			return E_FAIL;
		}
		wchar_t* wstrModelPrototypeName = new wchar_t[dwLen / sizeof(wchar_t) + 1];
		if (!ReadFile(hFile, wstrModelPrototypeName, dwLen, &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			delete[] wstrModelPrototypeName;
			CloseHandle(hFile);
			return E_FAIL;
		}
		wstrModelPrototypeName[dwLen / sizeof(wchar_t)] = L'\0';
		Inform->wstrModelPrototypeName = wstrModelPrototypeName;
		delete[] wstrModelPrototypeName;

		if (!ReadFile(hFile, &dwLen, sizeof(_uint), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			Safe_Delete(Inform);
			return E_FAIL;
		}
		wchar_t* wstrGameObjectPrototypeName = new wchar_t[dwLen / sizeof(wchar_t) + 1];
		if (!ReadFile(hFile, wstrGameObjectPrototypeName, dwLen, &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			delete[] wstrGameObjectPrototypeName;
			CloseHandle(hFile);
			return E_FAIL;
		}
		wstrGameObjectPrototypeName[dwLen / sizeof(wchar_t)] = L'\0';
		Inform->wstrGameObjectPrototypeName = wstrGameObjectPrototypeName;
		delete[] wstrGameObjectPrototypeName;

		if (!ReadFile(hFile, &dwLen, sizeof(_uint), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			Safe_Delete(Inform);
			return E_FAIL;
		}
		char* strGameObjectPrototypeName = new char[dwLen / sizeof(char) + 1];
		if (!ReadFile(hFile, strGameObjectPrototypeName, dwLen, &dwByte, nullptr))
		{
			Safe_Delete(Inform);
			delete[] strGameObjectPrototypeName;

			CloseHandle(hFile);
			return E_FAIL;
		}
		strGameObjectPrototypeName[dwLen / sizeof(char)] = '\0';
		Inform->strGameObjectPrototypeName = strGameObjectPrototypeName;
		delete[] strGameObjectPrototypeName;



		if (Inform->strGameObjectPrototypeName.find("_Anim") != string::npos)
		{
			Inform->bAnim = true;

			_matrix Ininitmatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

			m_pGameInstance->Add_Prototype(m_eNextLevelID, Inform->wstrModelPrototypeName, CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, Inform->strModelPath.c_str(), Ininitmatrix));
		}
		else
		{
			m_pGameInstance->Add_Prototype(m_eNextLevelID, Inform->wstrModelPrototypeName, CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, Inform->strModelPath.c_str(), XMMatrixIdentity()));

		}
		Safe_Delete(Inform);

	}
	CloseHandle(hFile);
	return S_OK;
}

HRESULT CLoader::Create_Prototypes_Actor()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Actor_EM0000"),
		CActor_EM00::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Actor_PL0000"),
		CActor_PL00::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Actor_PL7800"),
		CActor_PL78::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Create_Prototypes_CutScene()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CutScene_CF93"),
		CCut_Scene_CF93::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_For_Logo()
{
	m_strLoadingText = TEXT("Now Loadin ... Texture");

	m_strLoadingText = TEXT("Now Loading ... Model");

	m_strLoadingText = TEXT("Now Loading ... Shader");

	m_strLoadingText = TEXT("Now Loading ... Object Prototype");

	m_strLoadingText = TEXT("Loading Complete");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay()
{
	m_strLoadingText = TEXT("Now Loading Texture");

#pragma region UI Texture
	/*Prototype_Component_Texture_Tab_Window_BackGround*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Item_Description_BG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Item_Discription_Background/Item_Description_BG.dds")))))
		return E_FAIL;

	/*Prototype_Component_Texture_HP_Mask*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_HP_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/HP/IMANGE_05.png")))))
		return E_FAIL;

	/*Prototype_Component_Texture_HP_Mask*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_LogoBroken"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Title/Logo2.png")))))
		return E_FAIL;

	/*Prototype_Component_Texture_HP_Mask*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_BackGroundHP_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/HP/IMANGE_08.png")))))
		return E_FAIL;

	/*Prototype_Component_Texture_BackGround_0*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Main0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Aiming/Main0.png")))))
		return E_FAIL;

	/*Prototype_Component_Texture_Main1*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Main1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Aiming/Main1.png")))))
		return E_FAIL;

	/*Prototype_Component_Texture_WaringHP_UI*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_WaringHP_UI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/HP/IMANGE_02.png")))))
		return E_FAIL;

	/*Prototype_Component_Texture_DangerHP_UI*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_DangerHP_UI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/HP/IMANGE_03.png")))))
		return E_FAIL;

	/*Prototype_Component_Texture_Box_Store*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Box_Store"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/Box_Store.png")))))
		return E_FAIL;

	/*Prototype_Component_Texture_Box_Select_Click*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Box_Select_Click"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/Box_Select_Click.png")))))
		return E_FAIL;

	/*Prototype_Component_Texture_Tab_Window_BackGround*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Tab_Window_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Inventory/TabWindow_BackGround.dds")))))
		return E_FAIL;

	/*Prototype_Component_Texture_Perspective_Selecter_Check*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Perspective_Selecter_Check"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Perspective_Selecter/Check.png")))))
		return E_FAIL;

	/*Prototype_Component_Texture_NoteBook_Texture2*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_NoteBook_Texture2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/Note/ui3210_file_13_1_iam.tex_noesispreviewdata.png")))))
		return E_FAIL;


	/* Read Texture*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Document1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/document01a.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_SewrHintposter1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/sewerhintposter01a.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_ChessHints1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/chesshints01a.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Report1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/rpdreport01b.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Memo1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/memo01a.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Report2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/report01a.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_NestLcokout1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/nestlcokout01a.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_GuidePamphlet1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/guidepamphlet01a.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Pamphlet1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/pamphlet01a.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_DocumentBlood1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/rpddocumentblood01a.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Document2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/rpddocument01a.png")))))
		return E_FAIL;


	/* TEXT TYPE */
	/* 사건 일지 A INCIDENT_LOG_NOTE*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_ReadType_Incident_log1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/A_1.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_ReadType_Incident_log2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/A_2.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_ReadType_Incident_log3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/A_3.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_ReadType_Incident_log4"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/A_4.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_ReadType_Incident_log5"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/A_5.png")))))
		return E_FAIL;

	/* 작전 보고서 OPERATE_REPORT_NOTE*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Operate_Report1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/B_1.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Operate_Report2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/B_2.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Operate_Report3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/B_3.png")))))
		return E_FAIL;

	/* 신입의 첫 번째 과제 TASK_NOTE*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Task1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/C_1.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Task2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/C_2.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Task3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/C_3.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Task4"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/C_4.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Task5"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/C_5.png")))))
		return E_FAIL;

	/* 약초의 의학적 효능 MEDICINAL_NOTE*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Medicinal1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/D_1.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Medicinal2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/D_2.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Medicinal3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/D_3.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Medicinal4"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/D_4.png")))))
		return E_FAIL;

	/* 화약 설명서 GUNPOWDER_NOTE*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Gunpowder_Note1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/E_1.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Gunpowder_Note2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/E_2.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Gunpowder_Note3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/E_3.png")))))
		return E_FAIL;

	/* 금고 자판 고쳐놔 관련 노트 FIX_LOCKER_NOTE*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Fix_Lock_Note1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/G_1.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Fix_Lock_Note2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/G_2.png")))))
		return E_FAIL;

	/* 휴대용 금고 지침 HAND_HELD_SAFE_NOTE */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Portable_Safe1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/H_1.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Portable_Safe2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/H_2.png")))))
		return E_FAIL;

	/* 금고 자판 고쳐놔 관련 노트 SAFE_PASSWARD_NOTE*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Safe_PassWard_Note1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/I_1.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Safe_PassWard_Note2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/I_2.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Safe_PassWard_Note3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/I_3.png")))))
		return E_FAIL;

	/* 릭커에 대한 이야기 RICKER_NOTE*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_ReadType_Ricker1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/J_1.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_ReadType_Ricker2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/J_2.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_ReadType_Ricker3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/J_3.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_ReadType_Ricker4"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/J_4.png")))))
		return E_FAIL;

	/* 팜플렛 PAMPHLET*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_PamphletNote1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/F_1.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_PamphletNote2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/F_2.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_PamphletNote3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/F_3.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_PamphletNote4"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/ReadType_Item/Text/F_4.png")))))
		return E_FAIL;

	/* 경관의 수첩 OFFICER_NOTE*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_ReadType_Police_Note1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/Note/PoliceNote1.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_ReadType_Police_Note2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Get_Item_UI/Note/PoliceNote2.png")))))
		return E_FAIL;
#pragma endregion

#pragma region Effect
	/*Prototype_Component_Texture_Muzzle_Flash_SG*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Decal_Blood"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Decal/Blood_Decal.dds")))))
		return E_FAIL;

	/*Prototype_Component_Texture_Muzzle_Flash_SG*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Decal_Blood_SSD"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Decal/Blood_%d.png"),11))))
		return E_FAIL;

	/*Prototype_Component_Texture_Dissolve*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Dissolve"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Dissolve/Dissolve.png")))))
		return E_FAIL;

	/*Prototype_Component_Texture_Muzzle_Light*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Muzzle_Light_SG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Muzzle_Light_SG/Muzzle_Light_SG.dds")))))
		return E_FAIL;

	/*Prototype_Component_Texture_Impact*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Impact"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Impact/Impact_%d.dds"), 3))))
		return E_FAIL;

	/*Prototype_Component_Texture_BulletHole*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Decal_BulletHole"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/BulletHole/BulletHole.dds")))))
		return E_FAIL;

	//Muzzle Flash
	CTexture::TEXTURE_DESC Desc{};
	Desc.iWidth = 1024;
	Desc.iHeight = 512;
	Desc.iCountX = 2;
	Desc.iCountY = 1;

	/*Prototype_Component_Texture_Muzzle_Flash*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Muzzle_Flash"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Muzzle_Flash/Muzzle_Flash.dds"),1,&Desc))))
		return E_FAIL;

	Desc.iWidth = 512;
	Desc.iHeight = 256;
	Desc.iCountX = 2;
	Desc.iCountY = 1;

	/*Prototype_Component_Texture_Muzzle_Flash_SG*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Muzzle_Flash_SG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Muzzle_Flash_SG/Muzzle_Flash_SG.dds"), 1, &Desc))))
		return E_FAIL;

	Desc.iWidth = 512;
	Desc.iHeight = 256;
	Desc.iCountX = 16;
	Desc.iCountY = 8;

	/*Prototype_Component_Texture_Muzzle_Smoke_Trail*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Muzzle_Smoke_Trail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Muzzle_Smoke/Muzzle_Smoke_Trail.dds"), 1, &Desc))))
		return E_FAIL;

	Desc.iWidth = 1024;
	Desc.iHeight = 1024;
	Desc.iCountX = 8;
	Desc.iCountY = 8;

	/*Prototype_Component_Texture_Muzzle_Smoke_Trail*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Muzzle_ShckWave_SG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Muzzle_Smoke_SG/Muzzle_Smoke_SG.dds"), 1, &Desc))))
		return E_FAIL;

	Desc.iWidth = 1024;
	Desc.iHeight = 1024;
	Desc.iCountX = 8;
	Desc.iCountY = 8;

	/*Prototype_Component_Texture_Muzzle_Smoke_Trail*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Muzzle_ShckWave_SG_01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Muzzle_Smoke_SG/Muzzle_Smoke_SG_01.dds"), 1, &Desc))))
		return E_FAIL;

	Desc.iWidth = 512;
	Desc.iHeight = 256;
	Desc.iCountX = 16;
	Desc.iCountY = 4;

	/*Prototype_Component_Texture_Muzzle_Flash_SG*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Muzzle_Smoke_SG_Trail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Muzzle_Smoke_SG/Muzzle_Smoke_SG_Trail.dds"), 1, &Desc))))
		return E_FAIL;

	Desc.iWidth = 512;
	Desc.iHeight = 128;
	Desc.iCountX = 4;
	Desc.iCountY = 1;

	/*Prototype_Component_Texture_Muzzle_Spark_SG*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Muzzle_Spark_SG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Muzzle_Spark_SG/Muzzle_Spark_SG.dds"), 1, &Desc))))
		return E_FAIL;

	Desc.iWidth = 512;
	Desc.iHeight = 128;
	Desc.iCountX = 4;
	Desc.iCountY = 1;

	/*Prototype_Component_Texture_Muzzle_Spark_SG*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Muzzle_Spark_SG_01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Muzzle_Spark_SG/Muzzle_Spark_SG_01.dds"), 1, &Desc))))
		return E_FAIL;

	Desc.iWidth = 1024;
	Desc.iHeight = 512;
	Desc.iCountX = 4;
	Desc.iCountY = 2;

	/*Prototype_Component_Texture_Muzzle_Spark_SG*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Muzzle_Spark_SG_02"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Muzzle_Spark_SG/Muzzle_Spark_SG_02.dds"), 1, &Desc))))
		return E_FAIL;

	Desc.iWidth = 1024;
	Desc.iHeight = 256;
	Desc.iCountX = 4;
	Desc.iCountY = 1;

	/*Prototype_Component_Texture_Muzzle_Spark_SG*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Muzzle_Spark_SG_03"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Muzzle_Spark_SG/Muzzle_Spark_SG_03.dds"), 1, &Desc))))
		return E_FAIL;

	Desc.iWidth = 1024;
	Desc.iHeight = 256;
	Desc.iCountX = 4;
	Desc.iCountY = 1;

	/*Prototype_Component_Texture_Muzzle_Spark_SG*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Muzzle_Spark_SG_04"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Muzzle_Spark_SG/Muzzle_Spark_SG_04.dds"), 1, &Desc))))
		return E_FAIL;

	Desc.iWidth = 1024;
	Desc.iHeight = 256;
	Desc.iCountX = 4;
	Desc.iCountY = 1;

	/*Prototype_Component_Texture_Muzzle_Spark_SG*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Muzzle_Spark_SG_05"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Muzzle_Spark_SG/Muzzle_Spark_SG_05.dds"), 1, &Desc))))
		return E_FAIL;

	Desc.iWidth = 2048;
	Desc.iHeight = 1024;
	Desc.iCountX = 8;
	Desc.iCountY = 4;

	/*Prototype_Component_Texture_Muzzle_Spark_SG*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Hit"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Hit/Hit.dds"), 1, &Desc))))
		return E_FAIL;

	Desc.iWidth = 512;
	Desc.iHeight = 128;
	Desc.iCountX = 4;
	Desc.iCountY = 1;

	/*Prototype_Component_Texture_Muzzle_Spark_SG*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Hit_Props"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Hit_Props/Hit_Props.dds"), 1, &Desc))))
		return E_FAIL;

	Desc.iWidth = 512;
	Desc.iHeight = 128;
	Desc.iCountX = 4;
	Desc.iCountY = 1;

	/*Prototype_Component_Texture_Muzzle_Spark_SG*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Hit_Props_1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Hit_Props/Hit_Props_1.dds"), 1, &Desc))))
		return E_FAIL;

	Desc.iWidth = 1024;
	Desc.iHeight = 256;
	Desc.iCountX = 4;
	Desc.iCountY = 1;

	/*Prototype_Component_Texture_Muzzle_Spark_SG*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Hit_Props_2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Hit_Props/Hit_Props_2.dds"), 1, &Desc))))
		return E_FAIL;

	Desc.iWidth = 1024;
	Desc.iHeight = 256;
	Desc.iCountX = 4;
	Desc.iCountY = 1;

	/*Prototype_Component_Texture_Muzzle_Spark_SG*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Hit_Props_3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Hit_Props/Hit_Props_3.dds"), 1, &Desc))))
		return E_FAIL;

	Desc.iWidth = 1024;
	Desc.iHeight = 256;
	Desc.iCountX = 4;
	Desc.iCountY = 1;

	/*Prototype_Component_Texture_Muzzle_Spark_SG*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Hit_Props_4"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Hit_Props/Hit_Props_4.dds"), 1, &Desc))))
		return E_FAIL;

	Desc.iWidth = 512;
	Desc.iHeight = 128;
	Desc.iCountX = 4;
	Desc.iCountY = 1;

	/*Prototype_Component_Texture_Muzzle_Spark_SG*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Hit_Props_5"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/Hit_Props/Hit_Props_5.dds"), 1, &Desc))))
		return E_FAIL;
#pragma endregion

#pragma region CubeMap

	filesystem::path FullPath(TEXT("../Bin/Data/EnvCubeSetting.dat"));



	ifstream ifs(FullPath.c_str(), ios::binary);

	if (true == ifs.fail())
	{
		MSG_BOX(TEXT("Failed To OpenFile"));

		return E_FAIL;
	}
	_int iEntireTextures;
	ifs.read(reinterpret_cast<_char*>(&iEntireTextures), sizeof(_int));

	/* For.Com_Texture */
	for (_int i = 0; i < iEntireTextures; i++)
	{
		_tchar szPrototypeComName[MAX_PATH];
		_tchar szPath[MAX_PATH];
		_int iTextureNum;
		ifs.read(reinterpret_cast<_char*>(&szPrototypeComName), sizeof(_tchar) * MAX_PATH);
		ifs.read(reinterpret_cast<_char*>(&szPath), sizeof(_tchar) * MAX_PATH);
		ifs.read(reinterpret_cast<_char*>(&iTextureNum), sizeof(_int));
		if (FAILED(m_pGameInstance->Add_Prototype(g_Level, szPrototypeComName,
			CTexture::Create(m_pDevice, m_pContext, szPath, iTextureNum))))
			return E_FAIL;
	}

	ifs.close();


#pragma endregion

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;
				
#pragma region Model
	m_strLoadingText = TEXT("Now Loading ... Model");

	_matrix			TransformMatrix = { XMMatrixIdentity() };
	_matrix			LeonTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
#pragma region Players Model 
	/* Prototype_Component_Model_LeonBody */
	/*if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_LeonBody"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/LeonTest/LeonBody.fbx",
			LeonTransformMatrix))))
		return E_FAIL;*/

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_LeonBody"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/LeonTest/LeonBody.fbx",
			LeonTransformMatrix))))
		return E_FAIL;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/* Prototype_Component_Model_LeonFace */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_LeonFace"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/LeonTest/LeonFace.fbx",
			LeonTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LeonHair */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_LeonHair"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/LeonTest/LeonHair.fbx",
			LeonTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_HandGun */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_HandGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Weapon/HandGun/HandGun.fbx",
			TransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_ShotGun */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_ShotGun"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Weapon/ShotGun/ShotGun.fbx",
			TransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_HandGun */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Grenade"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Weapon/Grenade/Grenade.fbx",
			TransformMatrix))))
		return E_FAIL;


	/* Prototype_Component_Model_ShotGun */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_FlashBang"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Weapon/FlashBang/FlashBang.fbx",
			TransformMatrix))))
		return E_FAIL;

	_matrix			LightTransformMatrix = { XMMatrixRotationX(XMConvertToRadians(180.f)) };
					LightTransformMatrix *= XMMatrixRotationY(XMConvertToRadians(-20.f));

	/* Prototype_Component_Model_FlashLight */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_FlashLight"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Weapon/FlashLight/FlashLight.fbx",
			LightTransformMatrix))))
		return E_FAIL;
#pragma endregion

#pragma region Monsters Model

	_matrix				ZombieTrnasformMatrix = { XMMatrixRotationY(XMConvertToRadians(180.f)) };

#pragma region ZOMBIE_MALE
	//////////////////////////////////////////////////
	//	BODY
	//////////////////////////////////////////////////
	/* Prototype_Component_Model_Zombie_Body_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Body_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Body_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	//////////////////////////////////////////////////
	//	HAT
	//////////////////////////////////////////////////
	/* Prototype_Component_Model_Zombie_Hat00_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Hat00_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Hat00_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Hat01_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Hat01_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Hat01_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	//////////////////////////////////////////////////
	//	Face
	//////////////////////////////////////////////////
	/* Prototype_Component_Model_Zombie_Face00_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Face00_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Face00_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face01_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Face01_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Face01_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face02_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Face02_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Face02_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face03_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Face03_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Face03_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face04_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Face04_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Face04_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face05_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Face05_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Face05_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face06_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Face06_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Face06_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face07_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Face07_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Face07_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face08_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Face08_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Face08_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face09_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Face09_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Face09_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face10_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Face10_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Face10_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	//////////////////////////////////////////////////
	//	Pants
	//////////////////////////////////////////////////
	/* Prototype_Component_Model_Zombie_Pants00_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants00_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Pants00_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Pants01_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants01_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Pants01_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Pants02_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants02_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Pants02_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Pants03_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants03_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Pants03_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Pants04_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants04_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Pants04_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Pants05_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants05_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Pants05_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Pants06_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants06_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Pants06_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	//////////////////////////////////////////////////
	//	Shirts
	//////////////////////////////////////////////////
	/* Prototype_Component_Model_Zombie_Shirts00_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts00_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts00_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts01_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts01_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts01_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts02_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts02_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts02_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts03_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts03_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts03_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts04_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts04_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts04_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts05_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts05_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts05_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts06_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts06_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts06_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts08_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts08_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts08_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts09_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts09_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts09_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts11_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts11_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts11_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts12_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts12_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts12_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts70_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts70_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts70_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts72_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts72_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts72_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts73_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts73_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts73_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts81_Male */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts81_Male"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male/Shirts81_Male.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

#pragma endregion

#pragma region ZOMBIE_FEMALE
	//////////////////////////////////////////////////
	//	BODY
	//////////////////////////////////////////////////
	/* Prototype_Component_Model_Zombie_Body_Female */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Body_Female"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Female/Body_Female.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	//////////////////////////////////////////////////
	//	Face
	//////////////////////////////////////////////////
	/* Prototype_Component_Model_Zombie_Face00_Female */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Face00_Female"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Female/Face00_Female.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face01_Female */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Face01_Female"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Female/Face01_Female.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face02_Female */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Face02_Female"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Female/Face02_Female.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face03_Female */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Face03_Female"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Female/Face03_Female.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face04_Female */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Face04_Female"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Female/Face04_Female.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	//////////////////////////////////////////////////
	//	Pants
	//////////////////////////////////////////////////
	/* Prototype_Component_Model_Zombie_Pants00_Female */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants00_Female"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Female/Pants00_Female.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Pants01_Female */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants01_Female"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Female/Pants01_Female.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Pants02_Female */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants02_Female"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Female/Pants02_Female.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Pants04_Female */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants04_Female"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Female/Pants04_Female.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;


	//////////////////////////////////////////////////
	//	Shirts
	//////////////////////////////////////////////////
	/* Prototype_Component_Model_Zombie_Shirts00_Female */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts00_Female"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Female/Shirts00_Female.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts01_Female */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts01_Female"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Female/Shirts01_Female.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts02_Female */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts02_Female"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Female/Shirts02_Female.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts03_Female */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts03_Female"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Female/Shirts03_Female.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts04_Female */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts04_Female"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Female/Shirts04_Female.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts05_Female */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts05_Female"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Female/Shirts05_Female.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;



#pragma endregion

#pragma region ZOMBIE_MALE_BIG

	//////////////////////////////////////////////////
	//	BODY
	//////////////////////////////////////////////////
	/* Prototype_Component_Model_Zombie_Body_Male_Big */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Body_Male_Big"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male_Big/Body_Male_Big.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	//////////////////////////////////////////////////
	//	Face
	//////////////////////////////////////////////////
	/* Prototype_Component_Model_Zombie_Face00_Male_Big */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Face00_Male_Big"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male_Big/Face00_Male_Big.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face01_Male_Big */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Face01_Male_Big"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male_Big/Face01_Male_Big.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face03_Male_Big */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Face03_Male_Big"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male_Big/Face03_Male_Big.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Face04_Male_Big */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Face04_Male_Big"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male_Big/Face04_Male_Big.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;


	/* Prototype_Component_Model_Zombie_Face70_Male_Big */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Face70_Male_Big"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male_Big/Face70_Male_Big.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	//////////////////////////////////////////////////
	//	Pants
	//////////////////////////////////////////////////
	/* Prototype_Component_Model_Zombie_Pants00_Male_Big */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants00_Male_Big"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male_Big/Pants00_Male_Big.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Pants01_Male_Big */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants01_Male_Big"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male_Big/Pants01_Male_Big.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Pants02_Male_Big */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants02_Male_Big"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male_Big/Pants02_Male_Big.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Pants70_Male_Big */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Pants70_Male_Big"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male_Big/Pants70_Male_Big.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	//////////////////////////////////////////////////
	//	Shirts
	//////////////////////////////////////////////////
	/* Prototype_Component_Model_Zombie_Shirts00_Male_Big */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts00_Male_Big"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male_Big/Shirts00_Male_Big.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts01_Male_Big */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts01_Male_Big"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male_Big/Shirts01_Male_Big.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts02_Male_Big */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts02_Male_Big"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male_Big/Shirts02_Male_Big.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts03_Male_Big */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts03_Male_Big"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male_Big/Shirts03_Male_Big.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Zombie_Shirts70_Male_Big */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Zombie_Shirts70_Male_Big"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie_Male_Big/Shirts70_Male_Big.fbx",
			ZombieTrnasformMatrix))))
		return E_FAIL;

#pragma endregion

#pragma region Effect
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Blood_01"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect/Blood/Blood_01.fbx",
			TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Blood_02"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect/Blood/Blood_02.fbx",
			TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Blood_03"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect/Blood/Blood_03.fbx",
			TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Blood_04"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect/Blood/Blood_04.fbx",
			TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Blood_05"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect/Blood/Blood_05.fbx",
			TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Blood_06"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect/Blood/Blood_06.fbx",
			TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Blood_07"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect/Blood/Blood_07.fbx",
			TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Blood_08"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect/Blood/Blood_08.fbx",
			TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Blood_09"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect/Blood/Blood_09.fbx",
			TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Blood_10"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect/Blood/Blood_10.fbx",
			TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_Blood_11"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect/Blood/Blood_11.fbx",
			TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_HG_Cartridge"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect/HG_Cartridge/HG_Cartridge.fbx",
			TransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_SG_Cartridge"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Effect/SG_Cartridge/SG_Cartridge.fbx",
			TransformMatrix))))
		return E_FAIL;
#pragma endregion



	/* Prototype_Component_VIBuffer_Terrain */
	//if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_VIBuffer_Terrain"),
	//	CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
	//	return E_FAIL;

	m_strLoadingText = TEXT("Now Loading ... Navigation Mesh");
	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Navigation_Debug_Model"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/NavMesh/NavMesh.fbx", TransformMatrix))))
		return E_FAIL;
#pragma endregion

#pragma region CutScene_Model
	
#pragma region PL7800 

	/* Prototype_Component_Model_PL7800 */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_PL7800"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/CutScene/pl7800/pl7800.fbx",
			LeonTransformMatrix))))
		return E_FAIL;


	/* Prototype_Component_Model_PL7850 */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_PL7850"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/CutScene/pl7850/pl7850.fbx",
			LeonTransformMatrix))))
		return E_FAIL;


	/* Prototype_Component_Model_PL7870 */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_PL7870"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/CutScene/pl7870/pl7870.fbx",
			LeonTransformMatrix))))
		return E_FAIL;


	/* Prototype_Component_Model_PL7880 */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_PL7880"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/CutScene/pl7880/pl7880.fbx",
			LeonTransformMatrix))))
		return E_FAIL;
#pragma endregion		//	PL7800

#pragma endregion		//	CutScene_Model

	m_strLoadingText = TEXT("Now Loading ... Animations");

#pragma region Animation Load 


	if (FAILED(Load_Animations()))
		return E_FAIL;

#pragma endregion

#pragma region Actor Object Prototypes

	Create_Prototypes_Actor();

#pragma endregion

#pragma region CutScene Object Prototypes

	Create_Prototypes_CutScene();

#pragma endregion

	//CGameObject* pGameObj = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"))->back();

	//if(nullptr != pGameObj)
	//{
	//	static_cast<CLoading_UI*>(pGameObj)->Set_LoadingEnd(true);
	//}

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Models/NavMesh/NavMesh.bin")))))
		return E_FAIL;


	m_strLoadingText = TEXT("Now Loading ... Object");


#pragma region YeEun Add
#ifdef MAP_NOTHING
	if (FAILED(Load_Item_Prototype(TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/Item_Prototype.dat"))))
		return E_FAIL;
#endif
#ifdef MAP_INTERACT
	if (FAILED(Load_Field_Prototype(TEXT("../Bin/Data/Level_InteractObj/Make_Prototype.dat"))))
		return E_FAIL;
#endif 
#ifdef Map_J
	if (FAILED(Load_Item_Prototype(TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/Item_Prototype.dat"))))
		return E_FAIL;
	if (FAILED(Load_Field_Prototype(TEXT("../Bin/Data/Level_J/Make_Prototype.dat"))))
		return E_FAIL;
#endif 
#ifdef Map_Ye
	if (FAILED(Load_Item_Prototype(TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/Item_Prototype.dat"))))
		return E_FAIL;
	if (FAILED(Load_Field_Prototype(TEXT("../Bin/Data/Level_Ye/Make_Prototype.dat"))))
		return E_FAIL;
#endif 
#ifdef Map_TEST2
	if (FAILED(Load_Item_Prototype(TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/Item_Prototype.dat"))))
		return E_FAIL;
	if (FAILED(Load_Field_Prototype(TEXT("../Bin/Data/Level_TEST2/Make_Prototype.dat"))))
		return E_FAIL;
#endif 

#ifdef MAP_TEST
	if (FAILED(Load_Field_Prototype(TEXT("../Bin/Data/Level_Test/Make_Prototype.dat"))))
		return E_FAIL;
	if (FAILED(Load_Item_Prototype(TEXT("../Bin/DataFiles/Scene_TabWindow/Inventory/Item_Prototype.dat"))))
		return E_FAIL;
#endif 
#pragma endregion

	m_strLoadingText = TEXT("Loading Complete.");

	m_isFinished = true;

	if (true == m_isFinished)
	{
		for (auto& iter : m_eLoadingList)
			iter->Set_IsRender(false);
	}

	//if (true == m_isFinished)
	//{
	//	m_pGameInstance->Release_Layer(g_Level, TEXT("Layer_UI"));
	//}

	return S_OK;
}

HRESULT CLoader::Load_Animations()
{

#pragma region Player
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Move_Fine"), "../Bin/Resources/Animations/Player/Body/move/move_fine/")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Move_Hg"), "../Bin/Resources/Animations/Player/Body/move/move_hg/")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Move_Stg"), "../Bin/Resources/Animations/Player/Body/move/move_stg/")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Move_Light"), "../Bin/Resources/Animations/Player/Body/move/move_stlight/")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Move_Caution"), "../Bin/Resources/Animations/Player/Body/move/move_caution/")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Move_Caution_Light"), "../Bin/Resources/Animations/Player/Body/move/move_caution_stlight/")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Move_Danger"), "../Bin/Resources/Animations/Player/Body/move/move_danger/")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Move_Danger_Light"), "../Bin/Resources/Animations/Player/Body/move/move_danger_stlight/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Hold_Hg"), "../Bin/Resources/Animations/Player/Body/hold/hg_hold/")))
		return E_FAIL;															   
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Hold_Stg"), "../Bin/Resources/Animations/Player/Body/hold/stg_hold/")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Hold_Sup"), "../Bin/Resources/Animations/Player/Body/hold/sup_hold/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Common"), "../Bin/Resources/Animations/Player/Body/move/move_common/")))
		return E_FAIL;

#pragma region Player_Bite
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Bite_Push_Down"), "../Bin/Resources/Animations/Player/Body/bite/Bite_Push_Down/")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Bite_Creep"), "../Bin/Resources/Animations/Player/Body/bite/Bite_Creep/")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Bite_Default"), "../Bin/Resources/Animations/Player/Body/bite/Bite_Default/")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Bite_Default_Back"), "../Bin/Resources/Animations/Player/Body/bite/Bite_Default_Back/")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Bite_Lightly_Hold"), "../Bin/Resources/Animations/Player/Body/bite/Bite_Lightly_Hold/")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Player_Bite_ETC"), "../Bin/Resources/Animations/Player/Body/bite/Bite_ETC/")))
		return E_FAIL;
#pragma endregion

#pragma endregion


#pragma region Default Zombie Ordinary Anims

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Ordinary_Box_ClimbOver"), "../Bin/Resources/Animations/Body_Zombie/Ordinary/Box_ClimbOver/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Ordinary_ETC"), "../Bin/Resources/Animations/Body_Zombie/Ordinary/ETC/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Ordinary_Hold"), "../Bin/Resources/Animations/Body_Zombie/Ordinary/Hold/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Ordinary_Idle"), "../Bin/Resources/Animations/Body_Zombie/Ordinary/Idle/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Ordinary_PivotTurn"), "../Bin/Resources/Animations/Body_Zombie/Ordinary/PivotTurn/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Ordinary_Stairs_PivotTurn"), "../Bin/Resources/Animations/Body_Zombie/Ordinary/Stairs_PivotTurn/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Ordinary_Stairs_Walk"), "../Bin/Resources/Animations/Body_Zombie/Ordinary/Stairs_Walk/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Ordinary_StandUp"), "../Bin/Resources/Animations/Body_Zombie/Ordinary/StandUp/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Ordinary_Walk"), "../Bin/Resources/Animations/Body_Zombie/Ordinary/Walk/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Ordinary_Walk_Lose"), "../Bin/Resources/Animations/Body_Zombie/Ordinary/Walk_Lose/")))
		return E_FAIL;

#pragma endregion

#pragma region Default Zombie Add Anims

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Add_Arm_L"), "../Bin/Resources/Animations/Body_Zombie/Add/Arm_L/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Add_Arm_R"), "../Bin/Resources/Animations/Body_Zombie/Add/Arm_R/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Add_Body"), "../Bin/Resources/Animations/Body_Zombie/Add/Body/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Add_Head"), "../Bin/Resources/Animations/Body_Zombie/Add/Head/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Add_Leg_L"), "../Bin/Resources/Animations/Body_Zombie/Add/Leg_L/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Add_Leg_R"), "../Bin/Resources/Animations/Body_Zombie/Add/Leg_R/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Add_Shoulder_L"), "../Bin/Resources/Animations/Body_Zombie/Add/Shoulder_L/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Add_Shoulder_R"), "../Bin/Resources/Animations/Body_Zombie/Add/Shoulder_R/")))
		return E_FAIL;

#pragma endregion

#pragma region Default Zombie Bite Anims

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Bite_Push_Down"), "../Bin/Resources/Animations/Body_Zombie/Bite/Push_Down/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Bite_Creep"), "../Bin/Resources/Animations/Body_Zombie/Bite/Creep/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Bite_Default_Front"), "../Bin/Resources/Animations/Body_Zombie/Bite/Default_Front/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Bite_Default_Back"), "../Bin/Resources/Animations/Body_Zombie/Bite/Default_Back/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Bite_Lightly_Hold"), "../Bin/Resources/Animations/Body_Zombie/Bite/Lightly_Hold/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Bite_ETC"), "../Bin/Resources/Animations/Body_Zombie/Bite/ETC/")))
		return E_FAIL;

#pragma endregion

#pragma region Default Zombie Damage Anims

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Damage_Burst"), "../Bin/Resources/Animations/Body_Zombie/Damage/Burst/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Damage_Default"), "../Bin/Resources/Animations/Body_Zombie/Damage/Default/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Damage_Down"), "../Bin/Resources/Animations/Body_Zombie/Damage/Down/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Damage_Electric_Shock"), "../Bin/Resources/Animations/Body_Zombie/Damage/Electric_Shock/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Damage_Knockback"), "../Bin/Resources/Animations/Body_Zombie/Damage/Knockback/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Damage_Lost"), "../Bin/Resources/Animations/Body_Zombie/Damage/Lost/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Damage_Stairs_Down"), "../Bin/Resources/Animations/Body_Zombie/Damage/Stairs_Down/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Damage_Stairs_Up"), "../Bin/Resources/Animations/Body_Zombie/Damage/Stairs_Up/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Damage_Stun"), "../Bin/Resources/Animations/Body_Zombie/Damage/Stun/")))
		return E_FAIL;

#pragma endregion

#pragma region Default Zombie Dead Anims

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Dead_Default"), "../Bin/Resources/Animations/Body_Zombie/Dead/Default/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Dead_Bench"), "../Bin/Resources/Animations/Body_Zombie/Dead/Bench/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Dead_Hide_Locker"), "../Bin/Resources/Animations/Body_Zombie/Dead/Hide_Locker/")))
		return E_FAIL;

#pragma endregion

#pragma region Default Zombie Lost Anims

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Lost_Bite"), "../Bin/Resources/Animations/Body_Zombie/Lost/Bite/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Lost_Dead2_Idle"), "../Bin/Resources/Animations/Body_Zombie/Lost/Dead2_Idle/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Lost_Hold"), "../Bin/Resources/Animations/Body_Zombie/Lost/Hold/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Lost_Idle"), "../Bin/Resources/Animations/Body_Zombie/Lost/Idle/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Lost_Turn"), "../Bin/Resources/Animations/Body_Zombie/Lost/Turn/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Lost_TurnOver"), "../Bin/Resources/Animations/Body_Zombie/Lost/TurnOver/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Lost_Walk"), "../Bin/Resources/Animations/Body_Zombie/Lost/Walk/")))
		return E_FAIL;

#pragma endregion

#pragma region Default Zombie Sick Anims

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Sick_FlashGranade"), "../Bin/Resources/Animations/Body_Zombie/Sick/FlashGranade/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Sick_Knife"), "../Bin/Resources/Animations/Body_Zombie/Sick/Knife/")))
		return E_FAIL;

#pragma endregion

#pragma region Default Zombie Undiscovered Anims

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_Cage"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/Cage/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_Capture"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/Capture/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_Celling_Fall"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/Celling_Fall/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_Dead"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/Dead/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_Dead_Pose"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/Dead_Pose/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_Eat"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/Eat/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_Fance"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/Fance/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_HeadBang"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/HeadBang/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_Lounge"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/Lounge/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_Prison"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/Prison/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_Railing_Fall"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/Railing_Fall/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_Railing_Stund"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/Stund/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Undiscovered_Wall"), "../Bin/Resources/Animations/Body_Zombie/Undiscovered/Wall/")))
		return E_FAIL;

#pragma endregion

#pragma region Default Zombie Gimmick Anims

		if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Gimmick_Window"), "../Bin/Resources/Animations/Body_Zombie/Gimmick/Window/")))
			return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("Body_Zombie_Gimmick_Door"), "../Bin/Resources/Animations/Body_Zombie/Gimmick/Door/")))
		return E_FAIL;

#pragma endregion

#pragma region For.CutScene Animatoin 

#pragma region CF93

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("CF93_PL0000"), "../Bin/Resources/Animations/CutScene/CF93/pl0000/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("CF93_PL0050"), "../Bin/Resources/Animations/CutScene/CF93/pl0050/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("CF93_EM0000"), "../Bin/Resources/Animations/CutScene/CF93/em0000/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("CF93_PL7800"), "../Bin/Resources/Animations/CutScene/CF93/pl7800/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("CF93_PL7850"), "../Bin/Resources/Animations/CutScene/CF93/pl7850/")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototypes_Animation(TEXT("CF93_PL7880"), "../Bin/Resources/Animations/CutScene/CF93/pl7880/")))
		return E_FAIL;

#pragma endregion // CF98


#pragma endregion 

	return S_OK;	
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed To Created : CLoader"));

		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);



	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_Critical_Section);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}

