#include "stdafx.h"

#include "ImGui_Manager.h"
#include "GameInstance.h"
#include "Tab_Window.h"
#include "Player.h"


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


    if (FAILED(Refer_Tabwindow()))
        return E_FAIL;

    if (FAILED(Refer_Player()))
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


    if (DOWN == m_pGameInstance->Get_KeyState('Z'))
    {
        m_bTabWindow_Debuger = !m_bTabWindow_Debuger;
        m_bPlayer_Debuger = !m_bPlayer_Debuger;
    }

    if (true == m_bTabWindow_Debuger)
        Window_TabWindow_Debuger();

    if (true == m_bPlayer_Debuger)
        Window_Player_Debuger();

}

void CImgui_Manager::Render()
{
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImgui_Manager::Window_TabWindow_Debuger()
{
    ImGui::Begin("Tab_Window_Debug");

    if (ImGui::BeginListBox(u8"##ITEM_NUM_LIST", ImVec2(200.f, 300.f)))
    {
        _int TextlistCount = 0;

        for (auto& iter : m_vecItemNums)
        {
            string strText = WStringToString(iter);

            if (ImGui::Selectable(strText.c_str(), m_icurrent_Item == TextlistCount))
            {
                m_wstrCurrent_Item = iter;
                m_icurrent_Item = TextlistCount;
            }
            TextlistCount++;
        }
        ImGui::EndListBox();
    }

    ImGui::InputInt("##ItemCount", &m_iItemCount, 1);

    ImGui::Text(u8"도움말 : 만약 탬이 바로 보이지 않는다면\n인벤창을 껐다 키시면 됩니다");

    if (ImGui::Button(u8"Add ITEM", ImVec2(100.f, 30.f)) && TEXT("") != m_wstrCurrent_Item)
    {
        m_pTabWindow->AddItem_ToInven(Classify_String_To_ItemNum(m_wstrCurrent_Item), m_iItemCount);
    }

    ImGui::End();
}

void CImgui_Manager::Window_Player_Debuger()
{
    ImGui::Begin("Player_Debug");

    ImGui::InputInt("##PlayerHP", &m_iPlayerHP, 1);

    if (ImGui::Button(u8"SetPlayerHP", ImVec2(100.f, 30.f)))
    {
        m_pPlayer->Set_Hp(m_iPlayerHP);
    }

    ImGui::End();
}

wstring CImgui_Manager::Classify_ItemNum_To_String(ITEM_NUMBER eItemNum)
{
    switch (eItemNum)
    {
    case Client::emergencyspray01a:
        return TEXT("emergencyspray01a");
        break;
    case Client::greenherb01a:
        return TEXT("greenherb01a");
        break;
    case Client::redherb01a:
        return TEXT("redherb01a");
        break;
    case Client::blueherb01a:
        return TEXT("blueherb01a");
        break;
    case Client::herbsgg01a:
        return TEXT("herbsgg01a");
        break;
    case Client::herbsgr01a:
        return TEXT("herbsgr01a");
        break;
    case Client::herbsgb01a:
        return TEXT("herbsgb01a");
        break;
    case Client::herbsggb01a:
        return TEXT("herbsggb01a");
        break;
    case Client::herbsggg01a:
        return TEXT("herbsggg01a");
        break;
    case Client::herbsgrb01a:
        return TEXT("herbsgrb01a");
        break;
    case Client::herbsrb01a:
        return TEXT("herbsrb01a");
        break;
    case Client::greenherbitem01a:
        return TEXT("greenherbitem01a");
        break;
    case Client::redherbitem01a:
        return TEXT("redherbitem01a");
        break;
    case Client::blueherbitem01a:
        return TEXT("blueherbitem01a");
        break;
    case Client::handgun_bullet01a:
        return TEXT("handgun_bullet01a");
        break;
    case Client::shotgun_bullet01a:
        return TEXT("shotgun_bullet01a");
        break;
    case Client::submachinegun_bullet01a:
        return TEXT("submachinegun_bullet01a");
        break;
    case Client::magnumbulleta:
        return TEXT("magnumbulleta");
        break;
    case Client::biggun_bullet01a:
        return TEXT("biggun_bullet01a");
        break;
    case Client::inkribbon01a:
        return TEXT("inkribbon01a");
        break;
    case Client::woodbarricade01a:
        return TEXT("woodbarricade01a");
        break;
    case Client::blastingfuse01a:
        return TEXT("blastingfuse01a");
        break;
    case Client::gunpowder01a:
        return TEXT("gunpowder01a");
        break;
    case Client::gunpowder01b:
        return TEXT("gunpowder01b");
        break;
    case Client::strengtheningyellow01a:
        return TEXT("strengtheningyellow01a");
        break;
    case Client::HandGun:
        return TEXT("HandGun");
        break;

    default:
        break;
    }


    return TEXT("");
}

ITEM_NUMBER CImgui_Manager::Classify_String_To_ItemNum(wstring wstrItemNum)
{
    if (TEXT("emergencyspray01a") == wstrItemNum)
        return emergencyspray01a;

    else if (TEXT("greenherb01a") == wstrItemNum)
        return greenherb01a;

    else if(TEXT("redherb01a") == wstrItemNum)
        return redherb01a;

    else if(TEXT("blueherb01a") == wstrItemNum)
        return blueherb01a;

    else if (TEXT("herbsgg01a") == wstrItemNum)
        return herbsgg01a;

    else if (TEXT("herbsgr01a") == wstrItemNum)
        return herbsgr01a;

    else if (TEXT("herbsgb01a") == wstrItemNum)
        return herbsgb01a;

    else if (TEXT("herbsggb01a") == wstrItemNum)
        return herbsggb01a;

    else if (TEXT("herbsggg01a") == wstrItemNum)
        return herbsggg01a;

    else if (TEXT("herbsgrb01a") == wstrItemNum)
        return herbsgrb01a;

    else if (TEXT("herbsrb01a") == wstrItemNum)
        return herbsrb01a;

    else if (TEXT("greenherbitem01a") == wstrItemNum)
        return greenherbitem01a;

    else if (TEXT("redherbitem01a") == wstrItemNum)
        return redherbitem01a;

    else if (TEXT("blueherbitem01a") == wstrItemNum)
        return blueherbitem01a;

    else if (TEXT("handgun_bullet01a") == wstrItemNum)
        return handgun_bullet01a;

    else if (TEXT("shotgun_bullet01a") == wstrItemNum)
        return shotgun_bullet01a;

    else if (TEXT("submachinegun_bullet01a") == wstrItemNum)
        return submachinegun_bullet01a;

    else if (TEXT("magnumbulleta") == wstrItemNum)
        return magnumbulleta;

    else if (TEXT("biggun_bullet01a") == wstrItemNum)
        return biggun_bullet01a;

    else if (TEXT("inkribbon01a") == wstrItemNum)
        return inkribbon01a;
    
    else if (TEXT("woodbarricade01a") == wstrItemNum)
        return woodbarricade01a;

    else if (TEXT("blastingfuse01a") == wstrItemNum)
        return blastingfuse01a;

    else if (TEXT("gunpowder01a") == wstrItemNum)
        return gunpowder01a;

    else if (TEXT("gunpowder01b") == wstrItemNum)
        return gunpowder01b;

    else if (TEXT("strengtheningyellow01a") == wstrItemNum)
        return strengtheningyellow01a;

    else if (TEXT("HandGun") == wstrItemNum)
        return HandGun;


    return ITEM_NUMBER_END;
}

string CImgui_Manager::WStringToString(const std::wstring& wstr)
{
    if (wstr.empty())
        return std::string();

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);

    std::string strTo(size_needed, 0);

    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);

    return strTo;
}

HRESULT CImgui_Manager::Refer_Player()
{
    list<class CGameObject*>* pPlayerList = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

    for (auto& iter : *pPlayerList)
    {
        CPlayer* pTabWindow = nullptr;

        pTabWindow = dynamic_cast<CPlayer*>(iter);

        if (pTabWindow == nullptr)
            continue;

        m_pPlayer = pTabWindow;
        Safe_AddRef(m_pPlayer);
        break;
    }

    if (nullptr == m_pPlayer)
        return E_FAIL;

    return S_OK;
}

HRESULT CImgui_Manager::Refer_Tabwindow()
{
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

    for (_uint i = 0; i < static_cast<_uint>(ITEM_NUMBER_END); i++)
    {
        m_vecItemNums.push_back(Classify_ItemNum_To_String(static_cast<ITEM_NUMBER>(i)));
    }

    return S_OK;
}

void CImgui_Manager::Free()
{
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);

    Safe_Release(m_pTabWindow);
    Safe_Release(m_pPlayer);


    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();
}