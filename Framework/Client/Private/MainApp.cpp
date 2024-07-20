#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "FSM.h"
#include "Loading_UI.h"


CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);		
}

HRESULT CMainApp::Initialize()
{
	ENGINE_DESC		EngineDesc = {};

	EngineDesc.hWnd = g_hWnd;
	EngineDesc.isWindowed = true;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;

	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Fonts()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_For_Static()))
		return E_FAIL;
	
	if (FAILED(Open_Level(LEVEL_GAMEPLAY)))
		return E_FAIL;

#ifdef _DEBUG
	m_pGameInstance->On_Off_DebugRender();
#endif

	
	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	m_pGameInstance->Tick_Engine(fTimeDelta);

#ifdef DISPLAY_FRAME
	m_fTimeAcc += fTimeDelta;
#endif
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;
	
#ifdef DISPLAY_FRAME
	++m_iRenderCnt;

	if (m_fTimeAcc >= 1.f)
	{
		wsprintf(m_szFPS, TEXT("FPS : %d"), m_iRenderCnt);
		m_fTimeAcc = 0.f;
		m_iRenderCnt = 0;
	}
#endif

	m_pGameInstance->Begin_Draw(_float4(0.f, 0.f, 1.f, 1.f));

	//	m_pGameInstance->Render_Font(TEXT("Font_Default"), m_szFPS, _float2(0.f, 0.f), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);

	m_pGameInstance->Draw();

#ifdef DISPLAY_FRAME 

		m_pGameInstance->Render_Font(TEXT("Font_Default"), m_szFPS, _float2(0.f, 0.f), XMVectorSet(1.f, 0.f, 0.f, 1.f), 0.f);

#endif

		m_pGameInstance->End_Draw();

	return	S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component_For_Static()
{
	/* For.Prototype_GameObject_Loading_UI*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Loading_UI"),
		CLoading_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}


HRESULT CMainApp::Ready_Fonts()
{
	// MakeSpriteFont "³Ø½¼lv1°íµñ Bold" /FontSize:30 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 142.spritefont
	/*if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_Default"), TEXT("../Bin/Resources/Fonts/141ex.spriteFont"))))
		return E_FAIL;*/
	// ¿£Áø¿¡¼­ ÀÌ¹Ì ¸¸µë


	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_CG10"), TEXT("../Bin/Resources/Fonts/ClearGothic10.spriteFont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_CG11"), TEXT("../Bin/Resources/Fonts/ClearGothic11.spriteFont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_CG12"), TEXT("../Bin/Resources/Fonts/ClearGothic12.spriteFont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_CG13"), TEXT("../Bin/Resources/Fonts/ClearGothic13.spriteFont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_CG14"), TEXT("../Bin/Resources/Fonts/ClearGothic14.spriteFont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_CG15"), TEXT("../Bin/Resources/Fonts/ClearGothic15.spriteFont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_CG16"), TEXT("../Bin/Resources/Fonts/ClearGothic16.spriteFont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_CG17"), TEXT("../Bin/Resources/Fonts/ClearGothic17.spriteFont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_CG18"), TEXT("../Bin/Resources/Fonts/ClearGothic18.spriteFont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_CG19"), TEXT("../Bin/Resources/Fonts/ClearGothic19.spriteFont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_CG20"), TEXT("../Bin/Resources/Fonts/ClearGothic20.spriteFont"))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_CGBold10"), TEXT("../Bin/Resources/Fonts/ClearGothicBold10.spriteFont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_CGBold11"), TEXT("../Bin/Resources/Fonts/ClearGothicBold11.spriteFont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_CGBold12"), TEXT("../Bin/Resources/Fonts/ClearGothicBold12.spriteFont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_CGBold13"), TEXT("../Bin/Resources/Fonts/ClearGothicBold13.spriteFont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_CGBold14"), TEXT("../Bin/Resources/Fonts/ClearGothicBold14.spriteFont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_CGBold15"), TEXT("../Bin/Resources/Fonts/ClearGothicBold15.spriteFont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_CGBold16"), TEXT("../Bin/Resources/Fonts/ClearGothicBold16.spriteFont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_CGBold17"), TEXT("../Bin/Resources/Fonts/ClearGothicBold17.spriteFont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_CGBold18"), TEXT("../Bin/Resources/Fonts/ClearGothicBold18.spriteFont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_CGBold19"), TEXT("../Bin/Resources/Fonts/ClearGothicBold19.spriteFont"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(m_pDevice, m_pContext, TEXT("Font_CGBold20"), TEXT("../Bin/Resources/Fonts/ClearGothicBold20.spriteFont"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eLevelID)
{
	if (LEVEL_LOADING == eLevelID)
		return E_FAIL;

	m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID));

	return	S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CMainApp"));
		
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}
