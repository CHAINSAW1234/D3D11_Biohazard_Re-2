#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "Camera_Free.h"
#include "Monster.h"

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
	
	if(FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_LandObject()))
		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pGameInstance->Add_ShadowLight(TEXT("LIGHT_TEST_POINT"));
	m_pGameInstance->Set_ShadowSpotLight(TEXT("LIGHT_TEST_SPOT"));
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

	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.bShadow = false;
	//LightDesc.vDirection = _float4(0.f,-1.f,0.f,0.f);
	LightDesc.vPosition = _float4(0.f, 100000.f, 0.f, 1.f);
	LightDesc.fRange = 1000000.f;

	LightDesc.vDiffuse = _float4(.5f, .5f, .5f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vSpecular = _float4(0.2f, 0.2f, 0.2f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(g_strDirectionalTag, LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.bShadow = true;
	LightDesc.vPosition =_float4(-5.f, 10.f, 0.f, 1.f);
	LightDesc.fRange = 20.f;

	LightDesc.vDiffuse = _float4(1.f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.8f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 0.4f, 0.4f, 1.f);
	if (FAILED(m_pGameInstance->Add_Light(TEXT("LIGHT_TEST_POINT"), LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.bShadow = true;
	LightDesc.vPosition = _float4(10, 10.f, 0.f, 1.f);
	LightDesc.fRange = 20.f;

	LightDesc.vDiffuse = _float4(0.f, 1.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.8f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(0.4f, 1.f, 0.4f, 1.f);
	if (FAILED(m_pGameInstance->Add_Light(TEXT("LIGHT_TEST_POINT"), LightDesc)))
		return E_FAIL;

	LightDesc.eType = LIGHT_DESC::TYPE_SPOT;
	LightDesc.bShadow = true;
	LightDesc.vPosition = _float4(0, 30.f, 0.f, 1.f);

	LightDesc.fRange = 40.f;
	LightDesc.vDirection = _float4(0.f,-1.f,0.f,0.f);
	LightDesc.fCutOff = XMConvertToRadians(60.f);
	LightDesc.fOutCutOff = XMConvertToRadians(90);

	LightDesc.vDiffuse = _float4(1.f, 0.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(0.4f, 0.4f, 1.f, 1.f);
	
	if (FAILED(m_pGameInstance->Add_Light(TEXT("LIGHT_TEST_SPOT"), LightDesc)))
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
	///* 랜드오브젝트용 객체들에게 필요한 데이터를 구한다.*/
	/*CLandObject::LANDOBJECT_DESC		LandObjectDesc = {};
	LandObjectDesc.pTerrainTransform = (CTransform*)(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), g_strTransformTag));
	LandObjectDesc.pTerrainVIBuffer = (CVIBuffer_Terrain*)(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));	*/

	///* 구한정보들을 각 랜드오브젝트르 생성할 때 던진다. */
	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player")/*, LandObjectDesc*/)))
		return E_FAIL;

	/* 구한정보들을 각 랜드오브젝트르 생성할 때 던진다. */
	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_LandBackGround(TEXT("Layer_LandBackGround"))))
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
	for (size_t i = 0; i < 1; i++)
	{
		CMonster::MONSTER_DESC Desc{};
		Desc.Index = static_cast<_int>(i);
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"),&Desc)))
			return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_LandBackGround(const wstring & strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_ForkLift"))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Terrain"))))
	//	return E_FAIL;

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

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring & strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

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
