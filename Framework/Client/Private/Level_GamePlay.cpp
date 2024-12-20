#include "stdafx.h"
#include "Level_GamePlay.h"

#include "Camera_Free.h"
#include "Zombie.h"
#include "CustomCollider.h"

/* UI */
#include "Customize_UI.h"
#include "Inventory_Item_UI.h"

/* Player */
#include "Player.h"
#include "Zombie.h"

/* MapObj*/
#include"InteractProps.h"

/*CubeMap*/
#include"EnvCube.h"
#include "ImGui_Manager.h"

/*Sound*/
#include"EnvSound.h"


/* Manager */
#include "Call_Center.h"
#include "Cut_Scene_Manager.h"
#include "BGM_Player.h"


CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Managers()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_EnvSounds()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(g_strCameraTag)))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_TabWindow()))
		return E_FAIL;

	if (FAILED(Ready_LandObject()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	if (FAILED(Ready_RegionCollider()))
		return E_FAIL;

//#ifdef MAP_INTERACT
	if (FAILED(Ready_CutScene()))
		return E_FAIL;
//#endif

	if (FAILED(Ready_EnvCube()))
		return E_FAIL;

	m_pGameInstance->SetSimulate(true);

#ifdef IMGUI
	CImgui_Manager::Get_Instance()->Set_GraphicDevice(m_pDevice, m_pContext);
	CImgui_Manager::Get_Instance()->Initialize();
	CImgui_Manager::Get_Instance()->Tick();
	CImgui_Manager::Get_Instance()->Render();
#endif

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
#ifdef IMGUI
	CImgui_Manager::Get_Instance()->Tick();
#endif

	__super::Tick(fTimeDelta);

	m_pBGM_Player->Tick(fTimeDelta);
	m_pGameInstance->Add_ShadowLight(CPipeLine::DIRECTION, g_strDirectionalTag);
}

HRESULT CLevel_GamePlay::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level_GamePlay."));

#ifdef IMGUI
	CImgui_Manager::Get_Instance()->Render();
#endif

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.bShadow = true;
	XMStoreFloat4(&LightDesc.vDirection, XMVectorSetW(XMVector3Normalize(XMVectorSet(-3.f, -7.f, 0.F, 0.f)), 0.f));
	LightDesc.vPosition = _float4(-LightDesc.vDirection.x * 50, -LightDesc.vDirection.y * 50, -LightDesc.vDirection.z * 50, 1.f);

	//LightDesc.fRange = 1000000.f;

	LightDesc.vDiffuse = _float4(.2f, .2f, .2f, 1.f);
	LightDesc.vAmbient = _float4(.0f, .0f, .0f, 1.f);
	LightDesc.vSpecular = _float4(0.4f, 0.4f, 0.4f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(g_strDirectionalTag, LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.bRender = false;
	LightDesc.bShadow = false;
	XMStoreFloat4(&LightDesc.vDirection, XMVectorSetW(XMVector3Normalize(XMVectorSet(-1.f, -1.f, 0.F, 0.f)), 0.f));
	LightDesc.vPosition = _float4(0.f, 0.f, 0.f, 1.f);

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(g_strDirectionalTag, LightDesc)))
		return E_FAIL;

	m_pGameInstance->Add_ShadowLight(CPipeLine::DIRECTION, g_strDirectionalTag);

	if (FAILED(Load_Light(TEXT("../Bin/Data/Level_InteractObj"), LEVEL_GAMEPLAY)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_EnvSounds()
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_EnvSounds"), TEXT("Prototype_GameObject_EnvSound"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC		CameraDesc{};

	CameraDesc.fMouseSensor = 0.1f;
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.0f;
	CameraDesc.vEye = _float4(-10.f, 10.f, 0.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fSpeedPerSec = 10.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Camera_Event"), &CameraDesc)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Camera_Gimmick"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Managers()
{
	m_pCall_Center = CCall_Center::Get_Instance();
	if (nullptr == m_pCall_Center)
		return E_FAIL;

	m_pCutSceneManager = CCut_Scene_Manager::Get_Instance();
	if (nullptr == m_pCall_Center)
		return E_FAIL;

	m_pBGM_Player = CBGM_Player::Get_Instance();
	if (nullptr == m_pCall_Center)
		return E_FAIL;

	if (FAILED(m_pCutSceneManager->Initialize()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_TabWindow()
{
	/*HotKey*/
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_HotKey"))))
		return E_FAIL;

	/* 7. Tab_Widow */
	CUI::UI_DESC UIDesc = {};
	UIDesc.vPos = { g_iWinSizeX * 0.5f, g_iWinSizeY * 0.5f, 0.9f };
	UIDesc.vSize = { g_iWinSizeX * 1.f, g_iWinSizeY * 1.f };
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_TabWindow"), TEXT("Prototype_GameObject_Tab_Window"), &UIDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LandObject()
{
	if (FAILED(Ready_Decal(TEXT("Layer_Decal"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_LandBackGround(TEXT("Layer_LandBackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_PropManager()
{
	//m_pPropManager = CProp_Manager::Create();
	//if (m_pPropManager == nullptr)
	//	return E_FAIL;
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const wstring& strLayerTag/*, CLandObject::LANDOBJECT_DESC& LandObjectDesc*/)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Player"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const wstring& strLayerTag)
{
	//희히 넘 바쁜 관계로 함수 못팠어요 - 예은
//#ifdef MAP_TEST //- map
	//if (FAILED(Load_Monster(TEXT("../Bin/Data/Level_Test/Layer_Monster.dat"), strLayerTag, g_Level)))
	//	return E_FAIL;
//#endif

#ifdef MAP_INTERACT

	if (FAILED(Load_Monster(TEXT("../Bin/Data/Level_InteractObj/Layer_Monster.dat"), strLayerTag, g_Level)))
		return E_FAIL;

#endif

#ifdef TEST_ZOMBIE_MAINHALL
	//CZombie::ZOMBIE_FEMALE_DESC      ObjectDesc;
	//ObjectDesc.eBodyModelType = { ZOMBIE_BODY_TYPE::_FEMALE };
	//ObjectDesc.ePantsType = { static_cast<ZOMBIE_FEMALE_PANTS>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_FEMALE_PANTS::_END) - 1)) };
	//ObjectDesc.eFaceType = { static_cast<ZOMBIE_FEMALE_FACE>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_FEMALE_FACE::_END) - 1)) };
	//ObjectDesc.eShirtsType = { static_cast<ZOMBIE_FEMALE_SHIRTS>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_FEMALE_SHIRTS::_END) - 1)) };

	/*CZombie::ZOMBIE_MALE_DESC      ObjectDesc;
	ObjectDesc.eBodyModelType = { ZOMBIE_BODY_TYPE::_MALE };
	ObjectDesc.ePantsType = { static_cast<ZOMBIE_MALE_PANTS>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_MALE_PANTS::_END) - 1)) };
	ObjectDesc.eFaceType = { static_cast<ZOMBIE_MALE_FACE>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_MALE_FACE::_END) - 1)) };
	ObjectDesc.eShirtsType = { static_cast<ZOMBIE_MALE_SHIRTS>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_MALE_SHIRTS::_END) - 1)) };*/

	ZOMBIE_FEMALE_SHIRTS::_END;
	ZOMBIE_FEMALE_PANTS::_END;
	CZombie::ZOMBIE_FEMALE_DESC      ObjectDesc;
	ObjectDesc.eBodyModelType = { ZOMBIE_BODY_TYPE::_FEMALE };
	ObjectDesc.ePantsType = { static_cast<ZOMBIE_FEMALE_PANTS>(3) };
	ObjectDesc.eFaceType = { static_cast<ZOMBIE_FEMALE_FACE>(0) };
	ObjectDesc.eShirtsType = { static_cast<ZOMBIE_FEMALE_SHIRTS>(4) };

	ObjectDesc.eStart_Type = ZOMBIE_START_TYPE::_IDLE;
	ObjectDesc.eLocation = LOCATION_MAP_VISIT::MAIN_HOLL;

	ObjectDesc.eStart_Type = ZOMBIE_START_TYPE::_IDLE;
	ObjectDesc.eLocation = LOCATION_MAP_VISIT::MAIN_HOLL;
	ObjectDesc.bJumpScare = true;
	ObjectDesc.iJumpScareType = 0;
	_matrix         WorldMatrix = { XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixTranslation(35.809f, 0.227f, 2.779f) };
	XMStoreFloat4x4(&ObjectDesc.worldMatrix, WorldMatrix);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Zombie"), &ObjectDesc)))
		return E_FAIL;

	ObjectDesc.bJumpScare = true;
	ObjectDesc.iJumpScareType = 1;
	WorldMatrix = { XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixTranslation(14.261f, 4.f, -22.797f) };
	XMStoreFloat4x4(&ObjectDesc.worldMatrix, WorldMatrix);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Zombie"), &ObjectDesc)))
		return E_FAIL;

#endif

	SetUp_DeadMonsters();

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_LandBackGround(const wstring& strLayerTag)
{
#ifdef MAP_NOTHING

#endif
#ifdef MAP_INTERACT
	if (FAILED(Load_Layer(TEXT("../Bin/Data/Level_InteractObj"), LEVEL_GAMEPLAY)))
		return E_FAIL;
#endif
#ifdef MAP_TEST
	if (FAILED(Load_Layer(TEXT("../Bin/Data/Level_Test"), LEVEL_GAMEPLAY)))
		return E_FAIL;
#endif
#ifdef Map_J
	if (FAILED(Load_Layer(TEXT("../Bin/Data/Level_NYY"), LEVEL_GAMEPLAY)))
		return E_FAIL;
#endif
#ifdef Map_Ye
	if (FAILED(Load_Layer(TEXT("../Bin/Data/Level_Ye"), LEVEL_GAMEPLAY)))
		return E_FAIL;
#endif
#ifdef Map_TEST2
	if (FAILED(Load_Layer(TEXT("../Bin/Data/Level_TEST2"), LEVEL_GAMEPLAY)))
		return E_FAIL;
#endif



	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_NavMesh_Debug"))))
		return E_FAIL;


	if (FAILED(Ready_PropManager()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_RegionCollider()
{

#ifdef MAP_INTERACT
	if (FAILED(Load_Collider(TEXT("../Bin/Data/Level_InteractObj"), TEXT("Layer_Collider"))))
		return E_FAIL;
	if (FAILED(Load_Collider(TEXT("../Bin/Data/Level_InteractObj"), TEXT("Layer_Jombie_Collider"), _float4(1.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
#endif
#ifdef MAP_TEST
	if (FAILED(Load_Collider(TEXT("../Bin/Data/Level_Test"), TEXT("Layer_Jombie_Collider"), _float4(1.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if (FAILED(Load_Collider(TEXT("../Bin/Data/Level_Test"), TEXT("Layer_Collider"))))
		return E_FAIL;
#endif
	//if (FAILED(Load_Collider(TEXT("../Bin/Data/Level_InteractObj"), TEXT("Layer_EventCollider"))))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_CutScene()
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_CutScene"), TEXT("Prototype_GameObject_CutScene_CF92"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_CutScene"), TEXT("Prototype_GameObject_CutScene_CF93"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_CutScene"), TEXT("Prototype_GameObject_CutScene_CF94"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_CutScene"), TEXT("Prototype_GameObject_CutScene_CF95"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_CutScene"), TEXT("Prototype_GameObject_CutScene_CF120"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_CutScene"), TEXT("Prototype_GameObject_CutScene_CF150"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_CutScene"), TEXT("Prototype_GameObject_CutScene_CF151"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_CutScene"), TEXT("Prototype_GameObject_CutScene_CF152"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_CutScene"), TEXT("Prototype_GameObject_CutScene_CF153"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_CutScene"), TEXT("Prototype_GameObject_CutScene_CF190"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_EnvCube()
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_EnvCubeMap"), TEXT("Prototype_GameObject_EnvCube"))))
		return E_FAIL;

	m_pCubeMap = static_cast<CEnvCube*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_EnvCubeMap"), 0));

	m_pGameInstance->Set_CubeMap(m_pCubeMap->Get_Texture());

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const wstring& strLayerTag)
{
	for (size_t i = 0; i < 20; i++)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Effect"))))
			return E_FAIL;
	}


	return S_OK;
}

void CLevel_GamePlay::UI_Distinction(wstring& selectedFilePath)
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

HRESULT CLevel_GamePlay::Ready_Layer_UI(const wstring& strLayerTag)
{
	ifstream inputFileStream;
	wstring selectedFilePath;

	/////////////////////////* ▶  ▶  ▶  ▶  ▶  Crosshair  */////////////////////////////
	/* 1. Crosshair */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Crosshair.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 1. UI_ShotGun_Crosshair */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_ShotGun_Crosshair.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);


	///////////////////////////* ▶  ▶  ▶  ▶  ▶   BULLET  */////////////////////////////
	/* 5. Bullet_UI */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/Bullet_UI.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 5. UI_Bullet_Grenade */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Bullet_Grenade.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	///////////////////////////* ▶  ▶  ▶  ▶  ▶   */////////////////////////////

	///* 5. UI_Sub_Inventory */
	//selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Sub_Inventory.dat");
	//inputFileStream.open(selectedFilePath, ios::binary);
	//UI_Distinction(selectedFilePath);
	//CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 5. UI_MissionBar */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_MissionBar.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 8. UI_Selecter */
	for (_uint i = 0; i <= 5; i++)
	{
		selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Selector.dat");
		inputFileStream.open(selectedFilePath, ios::binary);
		UI_Distinction(selectedFilePath);
		CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);
	}

	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Hold.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);
	
	/////////////////////////* ▶ ▶  ▶  ▶  ▶ LayOut  */////////////////////////////
	/* 4. UI_LayOut */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Layout_BackGround.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 4. UI_Layout_Key */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Layout_Key.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 4. UI_Layout_Statue */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Layout_Statue.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 4. UI_HintLayout */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Layout_Inven.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 4. UI_HintLayout */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Layout_Hint.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 4. UI_HintLayout */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Layout_Map.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/////////////////////////* ▶ ▶  ▶  ▶  ▶  Map */////////////////////////////
	m_isMapType = true;

	/* 9. Map_Mask */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/Map_Mask.dat");//
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 9. Map_BackGround */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/Map_BackGround.dat");//
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	///////////////////////////*▶ ▶  ▶  ▶  ▶ Map : Font */////////////////////////////

	/* 9. UI_Map_Font */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Map_Font.dat");//
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 9. UI_Map_Font2 */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/Map_Font2.dat");//
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 9. UI_Map_Font3 */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/Map_Font3.dat");//
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 9. UI_Map_Floor_Type */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Map_Floor_Type.dat");//
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	///////////////////////////* ▶  ▶  ▶  ▶  ▶Map : Floor*/////////////////////////////
	/* 9. UI_Map */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Map.dat");//
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 10. UI_Map_Floor2 */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Map_Floor2.dat");//
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 10. UI_Map_Floor3 */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Map_Floor3.dat");//
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	///////////////////////////* ▶  ▶  ▶  ▶  ▶ Map : Door */////////////////////////////
	/* 9. UI_Map_Door */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Map_Door.dat");//
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 9. UI_Map_Door_Floor2 */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Map_Door_Floor2.dat");//
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	///////////////////////////*▶  ▶  ▶  ▶  ▶Map : Window  */////////////////////////////
	/* 9. UI_Map_Window */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Map_Window.dat");//
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	///////////////////////////* ▶  ▶  ▶  ▶  ▶Map : Item */////////////////////////////
	/* 9. UI_Map_Item */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Map_Item.dat");//
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 9. UI_Map_Item_Floor2 */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Map_Item_Floor2.dat");//
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 9. UI_Map_Item_Floor3 */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Map_Item_Floor3.dat");//
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 9. Map_Search_Type */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/Map_Search_Type.dat");//
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 9. Map_Line */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/Map_Line.dat");//
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 9. UI_Map_Player */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Map_Player.dat");//
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 9. Map_Target */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/Map_Target.dat");//
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 9. Map_BackGround */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Map_Target_Notify.dat");//
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 9. Map_BackGround */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Map_Announcement.dat");//
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	m_isMapType = false;

	///////////////////////////* ▶ Map : END */////////////////////////////

	/* 8. UI_Tutorial */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Tutorial.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 4. UI_MainHP */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/MainHPBar_UI.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/*5. BackGroundHPBar_UI */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/BackGroundHPBar_UI.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* Get Item */
	/* 9. UI_Item_Introduce */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Item_Introduce.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 10. UI_Item_Read_Write */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Item_Read.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 10. UI_Item_Read_Write */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_ReadUI_ItemType.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 11. UI_Item_Read_Arrow */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Item_Read_Arrow.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	/* 12. UI_Damage */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Damage.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);

	///////////////////////////* ▶  ▶  ▶  ▶  ▶    */////////////////////////////
	/* 2. Cursor */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Cursor.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CreatFromDat(inputFileStream, strLayerTag, nullptr, selectedFilePath);


	return S_OK;

}

HRESULT CLevel_GamePlay::Ready_Decal(const wstring& strLayerTag)
{
	for (size_t i = 0; i < 30; ++i)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Decal_SSD"))))
			return E_FAIL;
	}

	return S_OK;
}

void CLevel_GamePlay::CreatFromDat(ifstream& inputFileStream, wstring strListName, CGameObject* pGameParentsObj, wstring fileName, _int iWhich_Child, CGameObject* pSupervisor)
{
	CCustomize_UI::CUSTOM_UI_DESC CustomizeUIDesc;

	if (fileName == TEXT("UI_Map_Floor3") || fileName == TEXT("UI_Map_Font2") || fileName == TEXT("UI_Map_Floor2") || fileName == TEXT("UI_Map_Window") || fileName == TEXT("UI_Map") || fileName == TEXT("Map_Mask_Font"))
		inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.eMapUI_Type), sizeof(LOCATION_MAP_VISIT));

	if (fileName == TEXT("UI_Item_Introduce") || fileName == TEXT("UI_Item_Read_Arrow"))
		inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.eMapUI_Type), sizeof(LOCATION_MAP_VISIT));

	if (fileName == TEXT("UI_Map_Item") || fileName == TEXT("UI_Map_Item_Floor2") || fileName == TEXT("UI_Map_Item_Floor3"))
		inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.eMapUI_Type), sizeof(LOCATION_MAP_VISIT));

	if (fileName == TEXT("UI_Map_Item") || fileName == TEXT("UI_Map_Item_Floor2") || fileName == TEXT("UI_Map_Item_Floor3"))
		inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.eItem_Number), sizeof(ITEM_NUMBER));

	if (fileName == TEXT("UI_Map_Door") || fileName == TEXT("UI_Map_Door_Floor2") || fileName == TEXT("UI_Map_Door_Floor3"))
		inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.eDoor_Type), sizeof(DOOR_TYPE));

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

		if (TEXT("UI_Map_Font") == fileName || TEXT("UI_Map") == fileName)
			TextBoxDesc.isUI_Render = true;
		else
			TextBoxDesc.isUI_Render = false;

		CustomizeUIDesc.vecTextBoxDesc.push_back(TextBoxDesc);
	}

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.IsChild), sizeof(_bool));

	if (0 == CustomizeUIDesc.fMaxFrame && TEXT("") != CustomizeUIDesc.wstrDefaultTexturPath)
	{
		/* For.Prototype_Component_Texture_ */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, CustomizeUIDesc.wstrDefaultTexturComTag,
			CTexture::Create(m_pDevice, m_pContext, CustomizeUIDesc.wstrDefaultTexturPath)))) {
			int a = 0;
		}
	}

	else if (0 < CustomizeUIDesc.fMaxFrame && TEXT("") != CustomizeUIDesc.wstrDefaultTexturPath)
	{
		/* For.Prototype_Component_Texture_ */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, CustomizeUIDesc.wstrDefaultTexturComTag,
			CTexture::Create(m_pDevice, m_pContext, CustomizeUIDesc.wstrDefaultTexturPath, (_uint)CustomizeUIDesc.fMaxFrame)))) {
			int a = 0;
		}
	}

	if (TEXT("") != CustomizeUIDesc.wstrMaskPath)
	{
		/* For.Prototype_Component_Texture_ */
		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, CustomizeUIDesc.wstrMaskComTag,
			CTexture::Create(m_pDevice, m_pContext, CustomizeUIDesc.wstrMaskPath)))) {
			int a = 0;
		}
	}

	if (false == CustomizeUIDesc.IsChild)
		CustomizeUIDesc.iWhich_Child = 0;
	else
		CustomizeUIDesc.iWhich_Child = iWhich_Child;

	/* 자식인데 nullptr 이라면*/
	if (true == CustomizeUIDesc.IsChild)
		CustomizeUIDesc.pSupervisor = pSupervisor;

	else
		CustomizeUIDesc.pSupervisor = nullptr;

	CustomizeUIDesc.wstrFileName = fileName;

	// ▶ 객체 생성
	/* 1. Crosshair */
	if (TEXT("UI_ShotGun_Crosshair") == fileName || TEXT("UI_Crosshair") == fileName)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Crosshair_UI"), &CustomizeUIDesc)))
			MSG_BOX(TEXT("Failed to Add Clone"));
	}

	/* 2. Cursor */
	else if (TEXT("UI_Cursor") == fileName)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Cursor_UI"), &CustomizeUIDesc)))
			MSG_BOX(TEXT("Failed to Add Clone"));
	}

	/* 2. Inventory_Select_Box */
	else if (TEXT("UI_Inventory_SelectBox") == fileName)
	{
		CustomizeUIDesc.eBox_Type = CCustomize_UI::ITEM_BOX_TYPE::SELECT_BOX;

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Inventory_Item_UI"), &CustomizeUIDesc)))
			MSG_BOX(TEXT("Failed to Add Clone"));
	}

	/* 4. BackGroundHPBar_UI */
	else if (TEXT("BackGroundHPBar_UI") == fileName)
	{
		CustomizeUIDesc.eHPBar_Type = CCustomize_UI::HPBAR_TYPE::BACKGROUND_BAR;

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_HPBar_UI"), &CustomizeUIDesc)))
			MSG_BOX(TEXT("Failed to Add Clone"));
	}

	/* 4. MainHPBar_UI */
	else if (TEXT("MainHPBar_UI") == fileName)
	{
		CustomizeUIDesc.eHPBar_Type = CCustomize_UI::HPBAR_TYPE::MAIN_BAR;

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_HPBar_UI"), &CustomizeUIDesc)))
			MSG_BOX(TEXT("Failed to Add Clone"));
	}

	/* 4. MainHPBar_UI */
	else if (TEXT("Bullet_UI") == fileName || TEXT("UI_Bullet_Grenade") == fileName)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Bullet_UI"), &CustomizeUIDesc)))
			MSG_BOX(TEXT("Failed to Add Clone"));
	}

	/* Sub Inventory */
	else if (TEXT("UI_Sub_Inventory") == fileName)
	{
		CustomizeUIDesc.eBox_Type = CCustomize_UI::ITEM_BOX_TYPE::DEFAULT_BOX;
		CustomizeUIDesc.eInventory_Type = CCustomize_UI::INVENTORY_TYPE::SUB_INVEN;

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Inventory_Item_UI"), &CustomizeUIDesc)))
			MSG_BOX(TEXT("Failed to Add Clone"));
	}

	/* UI_Hold */
	else if (TEXT("UI_Hold") == fileName)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Hold_UI"), &CustomizeUIDesc)))
			MSG_BOX(TEXT("Failed to Add Clone"));
	}

	/* MissionBar */
	else if (TEXT("UI_MissionBar") == fileName)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_MissionBar_UI"), &CustomizeUIDesc)))
			MSG_BOX(TEXT("Failed to Add Clone"));
	}

	/* Tutorial */
	else if (TEXT("UI_Tutorial") == fileName)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Tutorial_UI"), &CustomizeUIDesc)))
			MSG_BOX(TEXT("Failed to Add Clone"));
	}

	/* UI_Selecter */
	else if (TEXT("UI_Selector") == fileName)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Selector_UI"), &CustomizeUIDesc)))
			MSG_BOX(TEXT("Failed to Add Clone"));
	}

	/* Read_Item_UI */
	else if (TEXT("UI_Item_Introduce") == fileName || TEXT("UI_Item_Read") == fileName || TEXT("UI_Item_Read_Arrow") == fileName || TEXT("UI_ReadUI_ItemType") == fileName)
	{
		CustomizeUIDesc.wstrFileName = fileName;

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Read_Item_UI"), &CustomizeUIDesc)))
			MSG_BOX(TEXT("Failed to Add Clone"));
	}

	/* 4. UI_LayOut */
	else if (TEXT("UI_Layout_BackGround") == fileName || TEXT("UI_Layout_Key") == fileName || TEXT("UI_Layout_Statue") == fileName
		|| TEXT("UI_Layout_Inven") == fileName || TEXT("UI_Layout_Hint") == fileName || TEXT("UI_Layout_Map") == fileName)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_LayOut_UI"), &CustomizeUIDesc)))
			MSG_BOX(TEXT("Failed to Add Clone"));
	}

	else if (TEXT("UI_Damage") == fileName)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Damage_UI"), &CustomizeUIDesc)))
			MSG_BOX(TEXT("Failed to Add Clone"));
	}

	//////////////////////////* Map *///////////////////////////
	else if (TEXT("UI_Map") == fileName || TEXT("UI_Map_Floor2") == fileName || TEXT("UI_Map_Floor3") == fileName
		|| TEXT("UI_Map_Door") == fileName || TEXT("UI_Map_Door_Floor2") == fileName
		|| TEXT("UI_Map_Window") == fileName  
		|| TEXT("UI_Map_Font") == fileName || TEXT("Map_Font2") == fileName || TEXT("Map_Font3") == fileName)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Main_Map_UI"), &CustomizeUIDesc)))
			MSG_BOX(TEXT("Failed to Add Clone"));
	}

	else if (TEXT("UI_Map_Announcement") == fileName)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Announcement_Map_UI"), &CustomizeUIDesc)))
			MSG_BOX(TEXT("Failed to Add Clone"));
		
	}
	else if (TEXT("Map_Target") == fileName || TEXT("UI_Map_Target_Notify") == fileName)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Targeting_Map_UI"), &CustomizeUIDesc)))
			MSG_BOX(TEXT("Failed to Add Clone"));
	}

	else if (TEXT("UI_Map_Item") == fileName || TEXT("UI_Map_Item_Floor2") == fileName || TEXT("UI_Map_Item_Floor3") == fileName)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Item_Map_UI"), &CustomizeUIDesc)))
			MSG_BOX(TEXT("Failed to Add Clone"));
	}

	else if (TEXT("UI_Map_Floor_Type") == fileName)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Floor_Map_UI"), &CustomizeUIDesc)))
			MSG_BOX(TEXT("Failed to Add Clone"));
	}

	else if (TEXT("UI_Map_Player") == fileName)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Player_Map_UI"), &CustomizeUIDesc)))
			MSG_BOX(TEXT("Failed to Add Clone"));
	}

	else if (TEXT("Map_Mask") == fileName || TEXT("Map_BackGround") == fileName || TEXT("Map_Search_Type") == fileName || TEXT("Map_Line") == fileName)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Static_Map_UI"), &CustomizeUIDesc)))
			MSG_BOX(TEXT("Failed to Add Clone"));
	}
	//////////////////////////* Map *///////////////////////////

	CGameObject* pGameObj = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"))->back();

	/* 자식인데 nullptr 이라면*/
	if (true == CustomizeUIDesc.IsChild && pSupervisor == nullptr)
		CustomizeUIDesc.pSupervisor = pGameObj;

	else if (true == CustomizeUIDesc.IsChild && pSupervisor != nullptr)
		CustomizeUIDesc.pSupervisor = pSupervisor;

	else
		CustomizeUIDesc.pSupervisor = pGameObj;

	CustomizeUIDesc.pImmediateSuperior = pGameObj;

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
			CreatFromDat(inputFileStream, strListName, pGameObj, fileName, iWhich_Child + 1, CustomizeUIDesc.pSupervisor);
		}
	}

	if (nullptr == pGameParentsObj)
	{
		inputFileStream.close();
	}
}

HRESULT CLevel_GamePlay::Load_Collider(const wstring& strFile, const wstring& strColLayerTag, _float4 vColliderColor)
{
	wstring	strFilePath = strFile + L"\\\\" + strColLayerTag + L".dat";

	HANDLE		hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return S_OK;


	DWORD	dwByte(0);

	_uint iObjectNum = { 0 };
	if (!ReadFile(hFile, &iObjectNum, sizeof(_uint), &dwByte, nullptr))
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	for (_uint i = 0; iObjectNum > i; ++i)
	{
		_float4x4 WorldMatrix = {};
		if (!ReadFile(hFile, &WorldMatrix, sizeof(_float4x4), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
		_int iNum = { 0 };

		if (!ReadFile(hFile, &iNum, sizeof(_int), &dwByte, NULL))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		_int iDir = { 0 };




		if (!ReadFile(hFile, &iDir, sizeof(_int), &dwByte, NULL))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		_int iRegionNum = { 0 };

		if (!ReadFile(hFile, &iRegionNum, sizeof(_int), &dwByte, NULL))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		_int iFloor = { 0 };
		if (!ReadFile(hFile, &iFloor, sizeof(_int), &dwByte, NULL))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		CCustomCollider::COLLIDER_DESC collider_desc = {};
		collider_desc.worldMatrix = WorldMatrix;
		collider_desc.iColNum = iNum;
		collider_desc.iDir = iDir;
		collider_desc.iRegionNum = iRegionNum;
		collider_desc.iFloor = iFloor;
		collider_desc.vColor = vColliderColor;
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strColLayerTag, TEXT("Prototype_GameObject_Collider"), &collider_desc)))
		{
			MSG_BOX(TEXT("Failed to Add_Clone Prototype_GameObject_Monster: CImGUI"));
			return E_FAIL;
		}

	}
	CloseHandle(hFile);
	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Layer(const wstring& strFilePath, _uint iLevel)
{
	wstring strPerfectFilePath = strFilePath + TEXT("\\\\Layer_List.dat");

	HANDLE		hFile = CreateFile(strPerfectFilePath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD	dwByte(0);

	_uint iObjectNum = { 0 };
	if (!ReadFile(hFile, &iObjectNum, sizeof(_uint), &dwByte, nullptr))
	{
		CloseHandle(hFile);
		return E_FAIL;
	}

	for (_uint i = 0; iObjectNum > i; ++i)
	{
		string strLayer;
		_uint dwLen = { 0 };
		if (!ReadFile(hFile, &dwLen, sizeof(_uint), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
		char* szLayerListName = new char[dwLen / sizeof(char) + 1];
		if (!ReadFile(hFile, szLayerListName, dwLen, &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
		szLayerListName[dwLen / sizeof(char)] = '\0';
		strLayer = szLayerListName;
		delete[] szLayerListName;

		_tchar	szTemp[MAX_PATH] = { L"" };
		MultiByteToWideChar(CP_ACP, 0, strLayer.c_str(), (_uint)strlen(strLayer.c_str()), szTemp, MAX_PATH);
		if (FAILED(m_pGameInstance->Add_Layer(iLevel, szTemp)))
			continue;

		if (FAILED(Load_Object(strFilePath, szTemp, iLevel)))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

	}
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Object(const wstring& strFilePath, const wstring& strLayerName, _uint iLevel)
{
	wstring strLayerFile = strFilePath + TEXT("\\\\") + strLayerName + TEXT(".dat");

	HANDLE		hFile = CreateFile(strLayerFile.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD	dwByte(0);

	_uint iObjectNum = { 0 };
	if (!ReadFile(hFile, &iObjectNum, sizeof(_uint), &dwByte, nullptr))
		return E_FAIL;

	_uint iMonsterNum = { 0 };

	for (_uint i = 0; iObjectNum > i; ++i)
	{
		_uint iLength = { 0 };

		CGameObject::GAMEOBJECT_DESC ObjectDesc = {};

		if (!ReadFile(hFile, &ObjectDesc.bAnim, sizeof(_bool), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (!ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
		wchar_t* strModelComponent = new wchar_t[iLength / sizeof(wchar_t) + 1];
		if (!ReadFile(hFile, strModelComponent, iLength, &dwByte, nullptr))
		{
			delete[] strModelComponent;

			CloseHandle(hFile);
			return E_FAIL;
		}
		strModelComponent[iLength / sizeof(wchar_t)] = L'\0';
		ObjectDesc.strModelComponent = strModelComponent;
		delete[] strModelComponent;


		if (!ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
		wchar_t* strObjectPrototype = new wchar_t[iLength / sizeof(wchar_t) + 1];
		if (!ReadFile(hFile, strObjectPrototype, iLength, &dwByte, nullptr))
		{
			delete[] strObjectPrototype;

			CloseHandle(hFile);
			return E_FAIL;
		}
		strObjectPrototype[iLength / sizeof(wchar_t)] = L'\0';
		ObjectDesc.strObjectPrototype = strObjectPrototype;
		delete[] strObjectPrototype;
		wstring strObjectName = ObjectDesc.strObjectPrototype;

		if (!ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
		char* strGamePrototypeName = new char[iLength / sizeof(char) + 1];
		if (!ReadFile(hFile, strGamePrototypeName, iLength, &dwByte, nullptr))
		{
			delete[] strGamePrototypeName;

			CloseHandle(hFile);
			return E_FAIL;
		}
		strGamePrototypeName[iLength / sizeof(char)] = '\0';
		ObjectDesc.strGamePrototypeName = strGamePrototypeName;
		delete[] strGamePrototypeName;

		_uint iIndex = { 0 };
		if (!ReadFile(hFile, &iIndex, sizeof(_uint), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (!ReadFile(hFile, &ObjectDesc.worldMatrix, sizeof(_float4x4), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

		ObjectDesc.iIndex = iMonsterNum++;

		_uint iSize = {};
		if (!ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, NULL)) {
			CloseHandle(hFile);
			return E_FAIL;
		}

		ObjectDesc.BelongIndexs.resize(iSize);

		for (size_t i = 0; i < iSize; i++)
		{
			_int iNum = { 0 };

			if (!ReadFile(hFile, &iNum, sizeof(_int), &dwByte, NULL)) {
				CloseHandle(hFile);
				return E_FAIL;
			}

			ObjectDesc.BelongIndexs[i] = iNum;


		}

		if (!ReadFile(hFile, &ObjectDesc.iRegionDir, sizeof(_int), &dwByte, NULL)) {
			CloseHandle(hFile);
			return E_FAIL;
		}


		if (!ReadFile(hFile, &ObjectDesc.iRegionNum, sizeof(_int), &dwByte, NULL)) {
			CloseHandle(hFile);
			return E_FAIL;
		}


		if (!ReadFile(hFile, &ObjectDesc.iPropType, sizeof(_int), &dwByte, NULL)) {
			CloseHandle(hFile);
			return E_FAIL;
		}


		if (!ReadFile(hFile, &ObjectDesc.iFloor, sizeof(_int), &dwByte, NULL)) {
			CloseHandle(hFile);
			return E_FAIL;
		}


		if (!ReadFile(hFile, &ObjectDesc.iPartObj, sizeof(_int), &dwByte, NULL)) {
			CloseHandle(hFile);
			return E_FAIL;
		}

		CInteractProps::INTERACTPROPS_DESC tagInteractprops = {};

		if (ObjectDesc.iPartObj != OBJ_NOPART)
		{
			tagInteractprops.bAnim = ObjectDesc.bAnim;
			tagInteractprops.BelongIndexs = ObjectDesc.BelongIndexs;
			tagInteractprops.iIndex = ObjectDesc.iIndex;
			tagInteractprops.iPropType = ObjectDesc.iPropType;
			tagInteractprops.iFloor = ObjectDesc.iFloor;
			tagInteractprops.iRegionDir = ObjectDesc.iRegionDir;
			tagInteractprops.iRegionNum = ObjectDesc.iRegionNum;
			tagInteractprops.worldMatrix = ObjectDesc.worldMatrix;
			tagInteractprops.strGamePrototypeName = ObjectDesc.strGamePrototypeName;
			tagInteractprops.strModelComponent = ObjectDesc.strModelComponent;
			tagInteractprops.strObjectPrototype = ObjectDesc.strObjectPrototype;
			tagInteractprops.iPartObj = ObjectDesc.iPartObj;


			switch (tagInteractprops.iPartObj)
			{
			case OBJ_DOOR:
			{
				if (!ReadFile(hFile, &tagInteractprops.tagDoor.bLock, sizeof(_bool), &dwByte, NULL))
				{
					CloseHandle(hFile);
					return E_FAIL;
				}

				if (!ReadFile(hFile, &tagInteractprops.tagDoor.iLockType, sizeof(_int), &dwByte, NULL))
				{
					CloseHandle(hFile);
					return E_FAIL;
				}
				if (tagInteractprops.tagDoor.iLockType == 0)
				{
					if (!ReadFile(hFile, &tagInteractprops.tagDoor.iEmblemType, sizeof(_int), &dwByte, NULL))
					{
						CloseHandle(hFile);
						return E_FAIL;
					}
				}

			}
			break;

			case OBJ_CABINET:
			{

				if (!ReadFile(hFile, &tagInteractprops.tagCabinet.bLock, sizeof(_bool), &dwByte, NULL))
				{
					CloseHandle(hFile);
					return E_FAIL;
				}

				if (!ReadFile(hFile, &tagInteractprops.tagCabinet.iLockType, sizeof(_int), &dwByte, NULL))
				{
					CloseHandle(hFile);
					return E_FAIL;
				}

				for (size_t i = 0; i < 10; i++)
				{
					_int iNum = { 0 };


					if (!ReadFile(hFile, &iNum, sizeof(_int), &dwByte, NULL))
					{
						CloseHandle(hFile);
						return E_FAIL;
					}

					tagInteractprops.tagCabinet.iLockNum[i] = iNum;

				}
				if (!ReadFile(hFile, &tagInteractprops.tagCabinet.bItem, sizeof(_bool), &dwByte, NULL))
				{
					CloseHandle(hFile);
					return E_FAIL;
				}

				if (!ReadFile(hFile, &tagInteractprops.tagCabinet.iItemIndex, sizeof(_int), &dwByte, NULL))
				{
					CloseHandle(hFile);
					return E_FAIL;
				}


				if (!ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr))
				{
					CloseHandle(hFile);
					return E_FAIL;
				}
				wchar_t* szName = new wchar_t[iLength / sizeof(wchar_t) + 1];
				if (!ReadFile(hFile, szName, iLength, &dwByte, nullptr))
				{
					CloseHandle(hFile);
					return E_FAIL;
				}
				szName[iLength / sizeof(wchar_t)] = L'\0';
				tagInteractprops.tagCabinet.Name = szName;
				delete[] szName;



			}
			break;

			case OBJ_WINDOW:

				break;

			case OBJ_SHUTTER:

				break;

			case OBJ_STATUE:

				break;

			case OBJ_BIGSTATUE:

				for (size_t i = 0; i < 10; i++)
				{
					_int iNum = { 0 };


					if (!ReadFile(hFile, &iNum, sizeof(_int), &dwByte, NULL))
					{
						CloseHandle(hFile);
						return E_FAIL;
					}

					tagInteractprops.tagBigStatue.iLockNum[i] = iNum;

				}
				break;

			case OBJ_HALL_STATUE:

				break;
			case OBJ_ITEM:
			{
				if (!ReadFile(hFile, &tagInteractprops.tagItemDesc.iItemIndex, sizeof(_int), &dwByte, NULL))
				{
					CloseHandle(hFile);
					return E_FAIL;
				}
				if (!ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr))
				{
					CloseHandle(hFile);
					return E_FAIL;
				}
				wchar_t* szName = new wchar_t[iLength / sizeof(wchar_t) + 1];
				if (!ReadFile(hFile, szName, iLength, &dwByte, nullptr))
				{
					CloseHandle(hFile);
					return E_FAIL;
				}
				szName[iLength / sizeof(wchar_t)] = L'\0';
				tagInteractprops.tagItemDesc.Name = szName;
				delete[] szName;
			}
			break;
			case OBJ_LADDER:
				break;
			case OBJ_LEVER:
				break;
			case OBJ_MOVINGSHELF:
				break;
			case OBJ_CARDREADER:
				break;
			case OBJ_CHAIR:
				break;
			}

			//파트오브젝트의 게임 프로토타입
			if (!ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr))
			{
				CloseHandle(hFile);
				return E_FAIL;
			}
			wchar_t* strMainPartObjProtoType = new wchar_t[iLength / sizeof(wchar_t) + 1];
			if (!ReadFile(hFile, strMainPartObjProtoType, iLength, &dwByte, nullptr))
			{
				CloseHandle(hFile);
				return E_FAIL;
			}
			strMainPartObjProtoType[iLength / sizeof(wchar_t)] = L'\0';
			strObjectName = strMainPartObjProtoType;
			delete[] strMainPartObjProtoType;

		}
		if (ObjectDesc.iPartObj != OBJ_NOPART)
		{
			if (FAILED(m_pGameInstance->Add_Clone(iLevel, strLayerName, strObjectName, &tagInteractprops)))
			{
				CloseHandle(hFile);
				return E_FAIL;
			}
		}
		else
			if (FAILED(m_pGameInstance->Add_Clone(iLevel, strLayerName, strObjectName, &ObjectDesc)))
			{
				CloseHandle(hFile);
				return E_FAIL;
			}


	}
	CloseHandle(hFile);
	return S_OK;
}

HRESULT CLevel_GamePlay::Load_Monster(const wstring& strFilePath, const wstring& strLayerName, _uint iLevel)
{
	_uint iMonsterNum = { 0 };
	HANDLE		hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;
	wchar_t			szFileName[MAX_PATH] = { TEXT("") };
	_wsplitpath_s(strFilePath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);


	DWORD	dwByte(0);

	_uint iObjectNum = { 0 };
	if (!ReadFile(hFile, &iObjectNum, sizeof(_uint), &dwByte, nullptr))
		return E_FAIL;

	for (_uint i = 0; iObjectNum > i; ++i)
	{
		_uint iLength = { 0 };
		CZombie::ZOMBIE_MALE_DESC		ObjectDesc;

#pragma region 와우 랜덤!
		/*_int			iRandomBody = { m_pGameInstance->GetRandom_Int(static_cast<_int>(ZOMBIE_BODY_TYPE::_MALE), static_cast<_int>(ZOMBIE_BODY_TYPE::_MALE_BIG)) };

		if (ZOMBIE_BODY_TYPE::_MALE == static_cast<ZOMBIE_BODY_TYPE>(iRandomBody))
		{
			ObjectDesc.eBodyModelType = { ZOMBIE_BODY_TYPE::_MALE };
			ObjectDesc.ePantsType = { static_cast<ZOMBIE_MALE_PANTS>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_MALE_PANTS::_END) - 1)) };
			ObjectDesc.eFaceType = { static_cast<ZOMBIE_MALE_FACE>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_MALE_FACE::_END) - 1)) };
			ObjectDesc.eShirtsType = { static_cast<ZOMBIE_MALE_SHIRTS>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_MALE_SHIRTS::_END) - 1)) };

			if (!ReadFile(hFile, &ObjectDesc.worldMatrix, sizeof(_float4x4), &dwByte, nullptr))
			{
				CloseHandle(hFile);
				return E_FAIL;
			}

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerName, TEXT("Prototype_GameObject_Zombie"), &ObjectDesc)))
			{
				CloseHandle(hFile);
				return E_FAIL;
			}
		}

		else if (ZOMBIE_BODY_TYPE::_FEMALE == static_cast<ZOMBIE_BODY_TYPE>(iRandomBody))
		{
			CZombie::ZOMBIE_FEMALE_DESC		ObjectDesc;
			ObjectDesc.eBodyModelType = { ZOMBIE_BODY_TYPE::_FEMALE };
			ObjectDesc.ePantsType = { static_cast<ZOMBIE_FEMALE_PANTS>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_FEMALE_PANTS::_END) - 1)) };
			ObjectDesc.eFaceType = { static_cast<ZOMBIE_FEMALE_FACE>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_FEMALE_FACE::_END) - 1)) };
			ObjectDesc.eShirtsType = { static_cast<ZOMBIE_FEMALE_SHIRTS>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_FEMALE_SHIRTS::_END) - 1)) };


			if (!ReadFile(hFile, &ObjectDesc.worldMatrix, sizeof(_float4x4), &dwByte, nullptr))
			{
				CloseHandle(hFile);
				return E_FAIL;
			}

			if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerName, TEXT("Prototype_GameObject_Zombie"), &ObjectDesc)))
			{
				CloseHandle(hFile);
				return E_FAIL;
			}
		}

		else if (ZOMBIE_BODY_TYPE::_MALE_BIG == static_cast<ZOMBIE_BODY_TYPE>(iRandomBody))
		{
			CZombie::ZOMBIE_MALE_BIG_DESC		ObjectDesc;
			ObjectDesc.eBodyModelType = { ZOMBIE_BODY_TYPE::_MALE_BIG };
			ObjectDesc.ePantsType = { static_cast<ZOMBIE_MALE_BIG_PANTS>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_MALE_BIG_PANTS::_END) - 1)) };
			ObjectDesc.eFaceType = { static_cast<ZOMBIE_MALE_BIG_FACE>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_MALE_BIG_FACE::_END) - 1)) };
			ObjectDesc.eShirtsType = { static_cast<ZOMBIE_MALE_BIG_SHIRTS>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_MALE_BIG_SHIRTS::_END) - 1)) };



		}*/
#pragma endregion 

		if (!ReadFile(hFile, &ObjectDesc.worldMatrix, sizeof(_float4x4), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
#pragma region kim예은의 로드 방식
		if (!ReadFile(hFile, &ObjectDesc.eLocation, sizeof(_int), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
		if (!ReadFile(hFile, &ObjectDesc.eStart_Type, sizeof(_int), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
		if (ObjectDesc.eStart_Type == ZOMBIE_START_TYPE::_END)
			ObjectDesc.eStart_Type = ZOMBIE_START_TYPE::_CREEP;
		if (!ReadFile(hFile, &ObjectDesc.eBodyModelType, sizeof(_int), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
		_int aArg = {};
		if (!ReadFile(hFile, &aArg, sizeof(_int), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
		if (!ReadFile(hFile, &ObjectDesc.ePantsType, sizeof(_int), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
		if (!ReadFile(hFile, &ObjectDesc.eFaceType, sizeof(_int), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
		if (!ReadFile(hFile, &ObjectDesc.eShirtsType, sizeof(_int), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
		if (!ReadFile(hFile, &ObjectDesc.eHatType, sizeof(_int), &dwByte, nullptr))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}

#pragma endregion 


		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerName, TEXT("Prototype_GameObject_Zombie"), &ObjectDesc)))
		{
			CloseHandle(hFile);
			return E_FAIL;
		}
	}
	CloseHandle(hFile);
	return S_OK;
}


HRESULT CLevel_GamePlay::SetUp_DeadMonsters()
{
	CZombie::ZOMBIE_MALE_DESC		ObjectDesc;
	ObjectDesc.eBodyModelType = { ZOMBIE_BODY_TYPE::_MALE };
	ObjectDesc.ePantsType = { static_cast<ZOMBIE_MALE_PANTS>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_MALE_PANTS::_END) - 1)) };
	ObjectDesc.eFaceType = { static_cast<ZOMBIE_MALE_FACE>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_MALE_FACE::_END) - 1)) };
	ObjectDesc.eShirtsType = { static_cast<ZOMBIE_MALE_SHIRTS>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_MALE_SHIRTS::_END) - 1)) };

	ObjectDesc.eStart_Type = ZOMBIE_START_TYPE::_HIDE_LOCKER;
	ObjectDesc.eLocation = LOCATION_MAP_VISIT::RIGHT_WESH_ROOM;
	//	ObjectDesc.eLocation = LOCATION_MAP_VISIT::MAIN_HOLL;

	_matrix			WorldMatrix = { XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixTranslation(0.f, -10.f, 0.f) };
	XMStoreFloat4x4(&ObjectDesc.worldMatrix, WorldMatrix);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Zombie"), &ObjectDesc)))
		return E_FAIL;

	ObjectDesc.eBodyModelType = { ZOMBIE_BODY_TYPE::_MALE };
	ObjectDesc.ePantsType = { static_cast<ZOMBIE_MALE_PANTS>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_MALE_PANTS::_END) - 1)) };
	ObjectDesc.eFaceType = { static_cast<ZOMBIE_MALE_FACE>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_MALE_FACE::_END) - 1)) };
	ObjectDesc.eShirtsType = { static_cast<ZOMBIE_MALE_SHIRTS>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_MALE_SHIRTS::_END) - 1)) };

	ObjectDesc.eStart_Type = ZOMBIE_START_TYPE::_DEAD_FAKE4;
	ObjectDesc.eLocation = LOCATION_MAP_VISIT::WEST_OFFICE;
	//	ObjectDesc.eLocation = LOCATION_MAP_VISIT::MAIN_HOLL;

	WorldMatrix = { XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixTranslation(0.f, -10.f, 0.f) };
	XMStoreFloat4x4(&ObjectDesc.worldMatrix, WorldMatrix);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Zombie"), &ObjectDesc)))
		return E_FAIL;


	ObjectDesc.eBodyModelType = { ZOMBIE_BODY_TYPE::_MALE };
	ObjectDesc.ePantsType = { static_cast<ZOMBIE_MALE_PANTS>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_MALE_PANTS::_END) - 1)) };
	ObjectDesc.eFaceType = { static_cast<ZOMBIE_MALE_FACE>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_MALE_FACE::_END) - 1)) };
	ObjectDesc.eShirtsType = { static_cast<ZOMBIE_MALE_SHIRTS>(m_pGameInstance->GetRandom_Int(0, static_cast<_int>(ZOMBIE_MALE_SHIRTS::_END) - 1)) };

	ObjectDesc.eStart_Type = ZOMBIE_START_TYPE::_DEAD_FAKE4;
	ObjectDesc.eLocation = LOCATION_MAP_VISIT::EAST_OFFICE;
	//	ObjectDesc.eLocation = LOCATION_MAP_VISIT::MAIN_HOLL;

	WorldMatrix = { XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixTranslation(0.f, -10.f, 0.f) };
	XMStoreFloat4x4(&ObjectDesc.worldMatrix, WorldMatrix);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Zombie"), &ObjectDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Terrain"))))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_GamePlay"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
	//Safe_Release(m_pTexture);

	Safe_Release(m_pCall_Center);
	Safe_Release(m_pCutSceneManager);
	Safe_Release(m_pBGM_Player);
}