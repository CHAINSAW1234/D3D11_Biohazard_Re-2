#include "stdafx.h"

#include "ImGui_Manager.h"
#include "GameInstance.h"
#include "Tab_Window.h"


#define SL  ImGui::SameLine()
#define NL  ImGui::NewLine()

IMPLEMENT_SINGLETON(CImgui_Manager)

CImgui_Manager::CImgui_Manager()
    : m_pGameInstance { CGameInstance::Get_Instance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CImgui_Manager::Initialize()
{
    ::ShowWindow(g_hWnd, SW_SHOWDEFAULT);
    ::UpdateWindow(g_hWnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    m_ImGuiIO = ImGui::GetIO();
    (void)m_ImGuiIO;
    m_ImGuiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    m_ImGuiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    m_ImGuiIO.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 18.0f, NULL, m_ImGuiIO.Fonts->GetGlyphRangesKorean());

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);

    list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));

    for (auto& iter : *pUIList)
    {
        CTab_Window* pTabWindow = nullptr;

        pTabWindow = dynamic_cast<CTab_Window*>(iter);

        if (pTabWindow == nullptr)
            continue;

        m_pTabWindow = pTabWindow;
        Safe_AddRef(m_pTabWindow);
        break;
    }

    if (nullptr == m_pTabWindow)
        return E_FAIL;

    return S_OK;
}

void CImgui_Manager::Set_GraphicDevice(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    m_pDevice = { pDevice };
    m_pContext = { pContext };
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

void CImgui_Manager::ResetVariable()
{

}

void CImgui_Manager::Tick()
{
    ResetVariable();//벨류 리셋

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();


    if (PRESSING == m_pGameInstance->Get_KeyState('Z'))
    {
        //m_bTabWindow_Debuger = !m_bTabWindow_Debuger;
        //m_pTabWindow->AddItem_ToInven(static_cast<ITEM_NUMBER>(m_iItemNum));

        if (DOWN == m_pGameInstance->Get_KeyState('1'))
            m_pTabWindow->AddItem_ToInven(static_cast<ITEM_NUMBER>(1));
        if (DOWN == m_pGameInstance->Get_KeyState('2'))
            m_pTabWindow->AddItem_ToInven(static_cast<ITEM_NUMBER>(2));
        if (DOWN == m_pGameInstance->Get_KeyState('3'))
            m_pTabWindow->AddItem_ToInven(static_cast<ITEM_NUMBER>(3));
        if (DOWN == m_pGameInstance->Get_KeyState('4'))
            m_pTabWindow->AddItem_ToInven(static_cast<ITEM_NUMBER>(4));
        if (DOWN == m_pGameInstance->Get_KeyState('5'))
            m_pTabWindow->AddItem_ToInven(static_cast<ITEM_NUMBER>(5));
        if (DOWN == m_pGameInstance->Get_KeyState('6'))
            m_pTabWindow->AddItem_ToInven(static_cast<ITEM_NUMBER>(6));
        if (DOWN == m_pGameInstance->Get_KeyState('7'))
            m_pTabWindow->AddItem_ToInven(static_cast<ITEM_NUMBER>(25));

    }

    if (true == m_bTabWindow_Debuger)
        Window_TabWindow_Debuger();

}

void CImgui_Manager::Render()
{
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImgui_Manager::Window_TabWindow_Debuger()
{
    ImGui::Begin("Tab_Window");

    ImGui::InputInt("Input_ItemNum", &m_iItemNum);

    if (ImGui::Button("Get_Item", ImVec2(110, 20)))
    {
        m_pTabWindow->AddItem_ToInven(static_cast<ITEM_NUMBER>(m_iItemNum));
    }

    ImGui::End();
}


void CImgui_Manager::Free()
{
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);

    Safe_Release(m_pTabWindow);


    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();
}