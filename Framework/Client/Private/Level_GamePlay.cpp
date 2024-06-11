#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "Camera_Free.h"
#include "Monster.h"
#include"CustomCollider.h"

/* UI */
#include "Customize_UI.h"
#include "Inventory_Item_UI.h"


CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;
	
	/*if (FAILED(Ready_Lights()))
		return E_FAIL;*/
	
	if(FAILED(Ready_Layer_Camera(TEXT("Layer_ZZZCamera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	//if (FAILED(Ready_LandObject()))
	//	return E_FAIL;

 	if (FAILED(Ready_Layer_UI(TEXT(""))))
		return E_FAIL;

	//if (FAILED(Ready_RegionCollider()))
	//	return E_FAIL;


	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	/*m_pGameInstance->Add_ShadowLight(CPipeLine::DIRECTION, g_strDirectionalTag);
	m_pGameInstance->Add_ShadowLight(CPipeLine::POINT, TEXT("LIGHT_TEST_POINT"));
	m_pGameInstance->Add_ShadowLight(CPipeLine::SPOT, TEXT("LIGHT_TEST_SPOT"));
	*/
	/*if (GetAsyncKeyState('T') & 0x0001)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Particle_Red"))))
			return;
	}*/
	
}

HRESULT CLevel_GamePlay::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level_GamePlay."));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.bShadow = true;
	XMStoreFloat4(&LightDesc.vDirection, XMVectorSetW(XMVector3Normalize(XMVectorSet(-3.f, -7.f, 0.f, 0.f)), 0.f));
	LightDesc.vPosition = _float4(-LightDesc.vDirection.x * 60, -LightDesc.vDirection.y * 60, -LightDesc.vDirection.z * 60, 1.f);

	//LightDesc.fRange = 1000000.f;

	LightDesc.vDiffuse = _float4(.5f, .5f, .5f, .5f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(0.4f, 0.4f, 0.4f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(g_strDirectionalTag, LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.bShadow = true;
	LightDesc.vPosition =_float4(-2.f, 3.f, 0.f, 1.f);
	LightDesc.fRange = 5.f;

	LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.8f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 0.4f, 0.4f, 1.f);
	if (FAILED(m_pGameInstance->Add_Light(TEXT("LIGHT_TEST_POINT"), LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.bShadow = true;
	LightDesc.vPosition = _float4(2.f, 3.f, 0.f, 1.f);
	LightDesc.fRange = 5.f;

	LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.8f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(0.4f, 1.f, 0.4f, 1.f);
	if (FAILED(m_pGameInstance->Add_Light(TEXT("LIGHT_TEST_POINT"), LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT_DESC::TYPE_SPOT;
	LightDesc.bShadow = true;
	LightDesc.vPosition = _float4(0, 4.f, -2.f, 1.f);

	LightDesc.fRange = 6.f;
	LightDesc.vDirection = _float4(0.f,-1.f, 0.f,0.f);
	LightDesc.fCutOff = XMConvertToRadians(60.f);
	LightDesc.fOutCutOff = XMConvertToRadians(90);

	LightDesc.vDiffuse = _float4(0.f, 0.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(0.4f, 0.4f, 1.f, 1.f);
	
	if (FAILED(m_pGameInstance->Add_Light(TEXT("LIGHT_TEST_SPOT"), LightDesc)))
		return E_FAIL;

	if (FAILED(Load_Light(TEXT("../Bin/Data/Level_0"), LEVEL_GAMEPLAY)))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring & strLayerTag)
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
	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LandObject()
{
	/*CLandObject::LANDOBJECT_DESC		LandObjectDesc = {};
	LandObjectDesc.pTerrainTransform = (CTransform*)(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), g_strTransformTag));
	LandObjectDesc.pTerrainVIBuffer = (CVIBuffer_Terrain*)(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));	*/
	if (FAILED(Ready_Layer_LandBackGround(TEXT("Layer_LandBackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player")/*, LandObjectDesc*/)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const wstring & strLayerTag/*, CLandObject::LANDOBJECT_DESC& LandObjectDesc*/)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Player"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const wstring & strLayerTag)
{
	/*for (size_t i = 0; i < 20; i++)
	{
		CMonster::MONSTER_DESC Desc{};
		Desc.Index = static_cast<_int>(i);
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"),&Desc)))
			return E_FAIL;
	}*/
	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_LandBackGround(const wstring & strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Props"))))
	//	return E_FAIL;
	if (FAILED(Load_Layer(TEXT("../Bin/Data/Level_Test"), LEVEL_GAMEPLAY)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_RegionCollider()
{
	if (FAILED(Load_Collider(TEXT("../Bin/Data/Level_0"), TEXT("Layer_Collider"))))
		return E_FAIL;

	return S_OK;
}
HRESULT CLevel_GamePlay::Ready_Layer_Effect(const wstring & strLayerTag)
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
	/* �Ǻ�*/
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

	/* 1. Crosshair */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Crosshair.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CCustomize_UI::CreatUI_FromDat(inputFileStream, nullptr, TEXT("Prototype_GameObject_Crosshair_UI"), m_pDevice, m_pContext);

	/* 2. Cursor */
	//selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Cursor.dat");
	//inputFileStream.open(selectedFilePath, ios::binary);
	//UI_Distinction(selectedFilePath);
	//CreatFromDat(inputFileStream, (""), nullptr, selectedFilePath);

	/* 3. Inventory_Item*/
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Inventory_Item.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CCustomize_UI::CreatUI_FromDat(inputFileStream, nullptr, TEXT("Prototype_GameObject_Inventory_Item_UI"), m_pDevice, m_pContext);

	/* 4. Inventory SelectBox */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/UI_Inventory_SelectBox.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CCustomize_UI::CreatUI_FromDat(inputFileStream, nullptr, TEXT("Prototype_GameObject_Inventory_Item_UI"), m_pDevice, m_pContext);

	/* 5. BackGroundHPBar_UI */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/BackGroundHPBar_UI.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CCustomize_UI::CreatUI_FromDat(inputFileStream, nullptr, TEXT("Prototype_GameObject_HPBar_UI"), m_pDevice, m_pContext);

	/* 6. UI_MainHP */
	selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/MainHPBar_UI.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	UI_Distinction(selectedFilePath);
	CCustomize_UI::CreatUI_FromDat(inputFileStream, nullptr, TEXT("Prototype_GameObject_HPBar_UI"), m_pDevice, m_pContext);

	/* 7. Tab_Widow */
	selectedFilePath = TEXT("../Bin/DataFiles/Scene_TabWindow/TabWin_BackGround.dat");
	inputFileStream.open(selectedFilePath, ios::binary);
	CCustomize_UI::CreatUI_FromDat(inputFileStream, nullptr, TEXT("Prototype_GameObject_Tab_Window"), m_pDevice, m_pContext);

	
	return S_OK;

}

HRESULT CLevel_GamePlay::Load_Collider(const wstring& strFile, const wstring& strColLayerTag)
{

	wstring	strFilePath = strFile + L"\\\\Layer_Collider.dat";

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

		CCustomCollider::COLLIDER_DESC collider_desc = {};
		collider_desc.worldMatrix = WorldMatrix;
		collider_desc.iColNum = iNum;
		collider_desc.iDir = iDir;
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_Collider"), TEXT("Prototype_GameObject_Collider"), &collider_desc)))
		{
			MSG_BOX(TEXT("Failed to Add_Clone Prototype_GameObject_Monster: CImGUI"));
			return E_FAIL;
		}




	}
	CloseHandle(hFile);
	return S_OK;
}
HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring & strLayerTag)
{
	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Terrain"))))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	return S_OK;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay*		pInstance = new CLevel_GamePlay(pDevice, pContext);

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
}