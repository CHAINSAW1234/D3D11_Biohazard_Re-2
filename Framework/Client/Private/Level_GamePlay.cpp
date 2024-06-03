#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "Camera_Free.h"
#include "Monster.h"
#include "Customize_UI.h"

#include"CustomCollider.h"
CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;
	
	if (FAILED(Ready_Lights()))
		return E_FAIL;
	
	if(FAILED(Ready_Layer_Camera(TEXT("Layer_ZZZCamera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_LandObject()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT(""))))
		return E_FAIL;

	if (FAILED(Ready_RegionCollider()))
		return E_FAIL;


	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_pGameInstance->Add_ShadowLight(CPipeLine::DIRECTION, g_strDirectionalTag);
	m_pGameInstance->Add_ShadowLight(CPipeLine::POINT, TEXT("LIGHT_TEST_POINT"));
	m_pGameInstance->Add_ShadowLight(CPipeLine::SPOT, TEXT("LIGHT_TEST_SPOT"));
	if (GetAsyncKeyState('T') & 0x0001)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Particle_Red"))))
			return;
	}
	
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
	if (FAILED(Load_Layer(TEXT("../Bin/Data/Level_0"), LEVEL_GAMEPLAY)))
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

HRESULT CLevel_GamePlay::Ready_Layer_UI(const wstring& strLayerTag)
{
	wstring selectedFilePath = TEXT("../Bin/DataFiles/UI_Data/test2.dat");
	ifstream inputFileStream;
	inputFileStream.open(selectedFilePath, ios::binary);
	CreatFromDat(inputFileStream, (""), nullptr);



	return S_OK;
}

void CLevel_GamePlay::CreatFromDat(ifstream& inputFileStream, string strListName, CGameObject* pGameParentsObj)
{
	CCustomize_UI::CUSTOM_UI_DESC CustomizeUIDesc;

	_char TexturePath[MAX_PATH] = "";

	inputFileStream.read(reinterpret_cast<_char*>(TexturePath), sizeof(_char) * MAX_PATH);

	CustomizeUIDesc.strTexturePath = wstring(TexturePath, TexturePath + strlen(TexturePath));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.fWorldMatrix), sizeof(_float4x4));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.iColorMaxNum), sizeof(_uint));

	_int iColorMAxnum = CustomizeUIDesc.iColorMaxNum;

	for (_int i = 0; i < iColorMAxnum; i++)
	{
		inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.vColor[i]), sizeof(CCustomize_UI::Value_Color));
	}

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.isPlay), sizeof(_bool));
	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.fColorTimer_Limit), sizeof(_float));
	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.iEndingType), sizeof(_int));

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.iTextBox), sizeof(_int));

	for (_int i = 0; i < CustomizeUIDesc.iTextBox; i++)
	{
		CTextBox::TextBox_DESC TextboxDesc = {};
		inputFileStream.read(reinterpret_cast<_char*>(&TextboxDesc), sizeof(CTextBox::TextBox_DESC));
		CustomizeUIDesc.TextBoxDesc.push_back(TextboxDesc);
	}

	inputFileStream.read(reinterpret_cast<_char*>(&CustomizeUIDesc.IsChild), sizeof(_bool));

	std::string strFullfilePath;
	strFullfilePath.assign(CustomizeUIDesc.strTexturePath.begin(), CustomizeUIDesc.strTexturePath.end());

	wstring wstrFilePath = StringToWstring(strFullfilePath);

	_tchar      szDir[MAX_PATH] = TEXT("");
	_tchar      szFileName[MAX_PATH] = TEXT("");
	_tchar		szEXT[MAX_PATH] = TEXT("");
	_tchar      chBase[MAX_PATH] = TEXT("Bin");
	wstring     wstrTexturePath = { TEXT("../") };
	wstring     wstrFileName = TEXT("");

	_wsplitpath_s(wstrFilePath.c_str(), nullptr, 0, szDir, MAX_PATH, szFileName, MAX_PATH, szEXT, MAX_PATH);

	wstrFileName = szFileName;

	string filename;
	filename.assign(wstrFileName.begin(), wstrFileName.end());

	const wchar_t* foundPosition = wcsstr(szDir, chBase);

	if (foundPosition == nullptr)
		return;

	wstrTexturePath += foundPosition;
	wstrTexturePath += szFileName;
	wstrTexturePath += szEXT;

	wstring     wstrPrototype = { TEXT("Prototype_Component_Texture_") };
	wstrPrototype += szFileName;

	/* For.Prototype_Component_Texture_ */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, wstrPrototype, CTexture::Create(m_pDevice, m_pContext, wstrTexturePath)))) {

	}
	//ShowMassageBox("텍스쳐 생성에 실패했습니다.");

	CustomizeUIDesc.strTextureComTag = wstrPrototype;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_CCustomize_UI"), &CustomizeUIDesc)))
		MSG_BOX(TEXT("Failed to Add Clone"));

	CGameObject* pGameObj = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"))->back();

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
			CreatFromDat(inputFileStream, filename, pGameObj);
		}
	}

	if (nullptr == pGameParentsObj)
	{
		inputFileStream.close();
	}
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