#include "stdafx.h"
#include "..\Public\Loader.h"
#include <process.h>

#include "GameInstance.h"
#include "Camera_Free.h"
#include "Body_Player.h"
#include "Head_Player.h"
#include "Hair_Player.h"
#include "BackGround.h"
#include "Terrain.h"
#include "Monster.h"
#include "Weapon.h"
#include "Player.h"
#include "Effect.h"
#include "Sky.h"
#include "Props.h"
#include "CustomCollider.h"
#include "NavMesh_Debug.h"
#include"Map.h"


/* UI */
#include "Customize_UI.h"
#include "Inventory_Item_UI.h"
#include "Tab_Window.h"
#include "Button_UI.h"
#include "Crosshair_UI.h"
#include "Cursor_UI.h"
#include "HPBar_UI.h"
#include "Bullet_UI.h"
#include "Title_UI.h"
#include "MissionBar_UI.h"

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
		hr = Loading_For_GamePlay();
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	LeaveCriticalSection(&m_Critical_Section);

	return S_OK;
}

HRESULT CLoader::Load_Prototype()
{
	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext))))
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
#pragma endregion
	
	///* For.Prototype_GameObject_Part_Weapon */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Part_Weapon"),
	//	CWeapon::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	/* For.Prototype_GameObject_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	///* For.Prototype_GameObject_Effect */
	//if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect"),
	//	CEffect::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

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

	/* For.Prototype_GameObject_Button_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Button_UI"),
		CButton_UI::Create(m_pDevice, m_pContext))))
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
		Inform->strGameObjectPrototypeName = strGameObjectPrototypeName; // 메쉬 파일(바이너리화)한거 
		delete[] strGameObjectPrototypeName;

		m_pGameInstance->Add_Prototype(m_eNextLevelID , Inform->wstrModelPrototypeName, CModel::Create(m_pDevice, m_pContext,Inform->strModelPath.c_str(), XMMatrixIdentity()));

		if(Inform->wstrGameObjectPrototypeName.find(TEXT("Merge")) == wstring::npos)
			m_pGameInstance->Add_Prototype(Inform->wstrGameObjectPrototypeName, CProps::Create(m_pDevice, m_pContext));
		else
			m_pGameInstance->Add_Prototype(Inform->wstrGameObjectPrototypeName, CMap::Create(m_pDevice, m_pContext));


		Safe_Delete(Inform);

	}
	CloseHandle(hFile);
	return S_OK;
}

HRESULT CLoader::Loading_For_Logo()
{
	m_strLoadingText = TEXT("Now Loadin ... Texture");

	m_strLoadingText = TEXT("Now Loading ... Model");

	m_strLoadingText = TEXT("Now Loading ... Shader");

	m_strLoadingText = TEXT("Now Loading ... Object Prototype");

	/* For.Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	m_strLoadingText = TEXT("Loading Complete");

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay()
{
	m_strLoadingText = TEXT("Now Loading Texture");

#pragma region UI Texture
	/*Prototype_Component_Texture_HP_Mask*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_HP_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/HP/IMANGE_05.png")))))
		return E_FAIL;

	/*Prototype_Component_Texture_HP_Mask*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_BackGroundHP_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/HP/IMANGE_08.png")))))
		return E_FAIL;

	/*Prototype_Component_Texture_Mask*/
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Mask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"), 1))))
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
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Tab_Winodw/Tab_Window_BackGround.dds")))))
		return E_FAIL;
#pragma endregion

	/* Prototype_Component_Texture_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/SkyBox/Sky_%d.dds"), 4))))
		return E_FAIL;

#pragma region Model
	m_strLoadingText = TEXT("Now Loading ... Model");

	_matrix			TransformMatrix = { XMMatrixIdentity() };
	_matrix			LeonTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));

	/* Prototype_Component_Model_Boss_MantisShrimp */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_LeonBody"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/LeonTest/LeonBody.fbx",
			LeonTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Boss_MantisShrimp */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_LeonFace"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/LeonTest/LeonFace.fbx",
			LeonTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Boss_MantisShrimp */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Model_LeonHair"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/LeonTest/LeonHair.fbx",
			LeonTransformMatrix))))
		return E_FAIL;

	m_strLoadingText = TEXT("Now Loading ... Navigation Mesh");
	/* For.Prototype_Component_Navigation */
	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Navigation_Debug_Model"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/NavMesh/NavMesh.fbx", TransformMatrix))))
		return E_FAIL;
#pragma endregion

	if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Models/NavMesh/NavMesh.bin")))))
		return E_FAIL;

	m_strLoadingText = TEXT("Now Loading ... Object");

#pragma region YeEun Add
#ifdef USE_UI
	if (FAILED(Load_Field_Prototype(TEXT("../Bin/Data/Level_UI/Make_Prototype.dat"))))
		return E_FAIL;
#endif
#ifdef USE_Player_Control
	if (FAILED(Load_Field_Prototype(TEXT("../Bin/Data/Level_Police/Make_Prototype.dat"))))
		return E_FAIL;
#endif
#ifdef USE_MapObject
	if (FAILED(Load_Field_Prototype(TEXT("../Bin/Data/Level_Test/Make_Prototype.dat"))))
		return E_FAIL;
#endif
#pragma endregion

	m_strLoadingText = TEXT("Loading Complete.");

	m_isFinished = true;

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

