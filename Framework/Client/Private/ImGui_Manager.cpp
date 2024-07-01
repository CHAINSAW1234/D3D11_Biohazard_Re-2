#include "stdafx.h"

#include "ImGui_Manager.h"
#include "GameInstance.h"
#include "Tab_Window.h"
#include "Player.h"


#define SL  ImGui::SameLine()
#define NL  ImGui::NewLine()

#define ITEM_STRING(name) case ITEM_NUMBER::name: return L###name

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



ITEM_NUMBER CImgui_Manager::Classify_String_To_ItemNum(wstring wstrItemNum)
{
    _uint i = 0;

    for (auto& iter : m_vecItemNums)
    {
        if (iter == wstrItemNum)
            return static_cast<ITEM_NUMBER>(i);

        i++;
    }

    /*
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
    */
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
    list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_TabWindow"));

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

wstring CImgui_Manager::Classify_ItemNum_To_String(ITEM_NUMBER eItemNum)
{
    switch (eItemNum)
    {
        ITEM_STRING(emergencyspray01a);
        break;
        ITEM_STRING(greenherb01a);
        break;
        ITEM_STRING(redherb01a);
        break;
        ITEM_STRING(blueherb01a);
        break;
        ITEM_STRING(herbsgg01a);
        break;
        ITEM_STRING(herbsgr01a);
        break;
        ITEM_STRING(herbsgb01a);
        break;
        ITEM_STRING(herbsggb01a);
        break;
        ITEM_STRING(herbsggg01a);
        break;
        ITEM_STRING(herbsgrb01a);
        break;
        ITEM_STRING(herbsrb01a);
        break;
        ITEM_STRING(greenherbitem01a);
        break;
        ITEM_STRING(redherbitem01a);
        break;
        ITEM_STRING(blueherbitem01a);
        break;
        ITEM_STRING(handgun_bullet01a);
        break;
        ITEM_STRING(shotgun_bullet01a);
        break;
        ITEM_STRING(submachinegun_bullet01a);
        break;
        ITEM_STRING(magnumbulleta);
        break;
        ITEM_STRING(biggun_bullet01a);
        break;
        ITEM_STRING(inkribbon01a);
        break;
        ITEM_STRING(woodbarricade01a);
        break;
        ITEM_STRING(blastingfuse01a);
        break;
        ITEM_STRING(gunpowder01a);
        break;
        ITEM_STRING(gunpowder01b);
        break;
        ITEM_STRING(strengtheningyellow01a);
        break;
        ITEM_STRING(vp70csparts01a);
        break;
        ITEM_STRING(laserpointer01a);
        break;
        ITEM_STRING(shotgunparts01a);
        break;
        ITEM_STRING(submgparts01a);
        break;
        ITEM_STRING(deserteaglecsparts01a);
        break;
        ITEM_STRING(lasersightparts01a);
        break;
        ITEM_STRING(ladysmithcsparts01a);
        break;
        ITEM_STRING(vp70longmagazine);
        break;
        ITEM_STRING(vp70powerup);
        break;
        ITEM_STRING(vp70stock);
        break;
        ITEM_STRING(ladysmithcsparts);
        break;
        ITEM_STRING(lasersight);
        break;
        ITEM_STRING(ladysmithpowerup);
        break;
        ITEM_STRING(browninghlongmagazine);
        break;
        ITEM_STRING(shotgunpartsstock);
        break;
        ITEM_STRING(shotgunpartsbarrel);
        break;
        ITEM_STRING(submgpartsmagazine);
        break;
        ITEM_STRING(submgpartssilencer);
        break;
        ITEM_STRING(deserteaglecspartsshight);
        break;
        ITEM_STRING(deserteaglecspartsbarrel);
        break;
        ITEM_STRING(grenadelauncherstock);
        break;
        ITEM_STRING(flamethrowerpowerup);
        break;
        ITEM_STRING(sparkshotpowerup);
        break;
        ITEM_STRING(oillighter01a);
        break;
        ITEM_STRING(keypickingtool01a);
        break;
        ITEM_STRING(photofilma01a);
        break;
        ITEM_STRING(photofilmb01a);
        break;
        ITEM_STRING(photofilmc01a);
        break;
        ITEM_STRING(photofilmd01a);
        break;
        ITEM_STRING(photofilme01a);
        break;
        ITEM_STRING(backdoorkey01a);
        break;
        ITEM_STRING(keycase01a);
        break;
        ITEM_STRING(extensioncord01a);
        break;
        ITEM_STRING(squarecrank01a);
        break;
        ITEM_STRING(unicornmedal01a);
        break;
        ITEM_STRING(spadekey01a);
        break;
        ITEM_STRING(cardkeylv101a);
        break;
        ITEM_STRING(cardkeylv201a);
        break;
        ITEM_STRING(valvehandle01a);
        break;
        ITEM_STRING(starsbadge01a);
        break;
        ITEM_STRING(kingscepter01a);
        break;
        ITEM_STRING(hourglasskey01a);
        break;
        ITEM_STRING(virginheart01a);
        break;
        ITEM_STRING(bluejewelrybox01a);
        break;
        ITEM_STRING(redjewelrybox01a);
        break;
        ITEM_STRING(bishopplug01a);
        break;
        ITEM_STRING(rookplug01a);
        break;
        ITEM_STRING(kingplug01a);
        break;
        ITEM_STRING(serpentstone01a);
        break;
        ITEM_STRING(eaglestone01a);
        break;
        ITEM_STRING(jaggerstone01a);
        break;
        ITEM_STRING(handcuffskey01a);
        break;
        ITEM_STRING(unicorndebris01a);
        break;
        ITEM_STRING(sunstone01a);
        break;
        ITEM_STRING(doorknob01a);
        break;
        ITEM_STRING(drivergrip01a);
        break;
        ITEM_STRING(driverheadplus01a);
        break;
        ITEM_STRING(driverheadsocket01a);
        break;
        ITEM_STRING(blankkey01a);
        break;
        ITEM_STRING(wetwastepaper01a);
        break;
        ITEM_STRING(statuebook01a);
        break;
        ITEM_STRING(statuehand01a);
        break;
        ITEM_STRING(floppydisk01a);
        break;
        ITEM_STRING(virginmedal01a);
        break;
        ITEM_STRING(diakey01a);
        break;
        ITEM_STRING(carkey01a);
        break;
        ITEM_STRING(condenser01a);
        break;
        ITEM_STRING(cardkeylv001a);
        break;
        ITEM_STRING(virginmedal02a);
        break;
        ITEM_STRING(pocketbook01a);
        break;
        ITEM_STRING(loveremblem01a);
        break;
        ITEM_STRING(gearsmal01a);
        break;
        ITEM_STRING(gear01a);
        break;
        ITEM_STRING(gardenkey01a);
        break;
        ITEM_STRING(knightplug01a);
        break;
        ITEM_STRING(pawnplug01a);
        break;
        ITEM_STRING(queenplug01a);
        break;
        ITEM_STRING(blisterpack01a);
        break;
        ITEM_STRING(swordofdamocles01a);
        break;
        ITEM_STRING(orphanagekey01a);
        break;
        ITEM_STRING(clubkey01a);
        break;
        ITEM_STRING(manholeopener01a);
        break;
        ITEM_STRING(plastickcontainer01a);
        break;
        ITEM_STRING(plastickcontainer02a);
        break;
        ITEM_STRING(eaglekey01a);
        break;
        ITEM_STRING(heartkey01a);
        break;
        ITEM_STRING(videotape01a);
        break;
        ITEM_STRING(eaglemedal01a);
        break;
        ITEM_STRING(brokeneaglekey01a);
        break;
        ITEM_STRING(wolfkey01a);
        break;
        ITEM_STRING(cardkeylv202a);
        break;
        ITEM_STRING(valvehandle02a);
        break;
        ITEM_STRING(listtagwatchb01a);
        break;
        ITEM_STRING(medicinebottle01a);
        break;
        ITEM_STRING(medicinebottle02a);
        break;
        ITEM_STRING(medicinebottle03a);
        break;
        ITEM_STRING(jointsnplugs01a);
        break;
        ITEM_STRING(listtagpartsmaster01a);
        break;
        ITEM_STRING(clisttagmaster01a);
        break;
        ITEM_STRING(signalmodulator01a);
        break;
        ITEM_STRING(trophy01a);
        break;
        ITEM_STRING(memorysticklock01a);
        break;
        ITEM_STRING(memorystickunlock01a);
        break;
        ITEM_STRING(liftkey01a);
        break;
        ITEM_STRING(llisttagguests01a);
        break;
        ITEM_STRING(llisttaggeneral01a);
        break;
        ITEM_STRING(llisttagadvanced01a);
        break;
        ITEM_STRING(listtagpartsgeneral01a);
        break;
        ITEM_STRING(listtagpartsadvanced01a);
        break;
        ITEM_STRING(clisttagguests01a);
        break;
        ITEM_STRING(clisttaggeneral01a);
        break;
        ITEM_STRING(clisttagadvanced01a);
        break;
        ITEM_STRING(videotape02a);
        break;
        ITEM_STRING(modisk01a);
        break;
        ITEM_STRING(samplecartridge01a);
        break;
        ITEM_STRING(samplecartridge02a);
        break;
        ITEM_STRING(antiviraldrugs01a);
        break;
        ITEM_STRING(attachecase01a);
        break;
        ITEM_STRING(leverswitches01a);
        break;
        ITEM_STRING(prismpillar01a);
        break;
        ITEM_STRING(prismpillar02a);
        break;
        ITEM_STRING(prismpillar03a);
        break;
        ITEM_STRING(laboratoryfuse01a);
        break;
        ITEM_STRING(laboratoryfuse02a);
        break;
        ITEM_STRING(pendant01a);
        break;
        ITEM_STRING(scissors01a);
        break;
        ITEM_STRING(chaincutter01a);
        break;
        ITEM_STRING(cushiondoll01a);
        break;
        ITEM_STRING(rpddocument01a);
        break;
        ITEM_STRING(rpddocumentblood01a);
        break;
        ITEM_STRING(diary01a);
        break;
        ITEM_STRING(document01a);
        break;
        ITEM_STRING(pamphlet01a);
        break;
        ITEM_STRING(guidepamphlet01a);
        break;
        ITEM_STRING(memo01a);
        break;
        ITEM_STRING(raccoonmonthly01a);
        break;
        ITEM_STRING(sewercopamphlet01a);
        break;
        ITEM_STRING(report01a);
        break;
        ITEM_STRING(nestlcokout01a);
        break;
        ITEM_STRING(sewerhintposter01a);
        break;
        ITEM_STRING(rpdreport01a);
        break;
        ITEM_STRING(rpdreport01b);
        break;
        ITEM_STRING(chesshints01a);
        break;
        ITEM_STRING(labopc01a);
        break;
        ITEM_STRING(labopc01b);
        break;
        ITEM_STRING(labopc01c);
        break;
        ITEM_STRING(raccoonfigure01a);
        break;
        ITEM_STRING(voicerecorder01a);
        break;
        ITEM_STRING(mappolice01a);
        break;
        ITEM_STRING(mapunderground01a);
        break;
        ITEM_STRING(mapsewer01a);
        break;
        ITEM_STRING(mapraccooncity01a);
        break;
        ITEM_STRING(maplaboratoryhigh01a);
        break;
        ITEM_STRING(maplaboratorymiddle01a);
        break;
        ITEM_STRING(mapupperpolice01a);
        break;
        ITEM_STRING(clairesbag01a);
        break;
        ITEM_STRING(oldkey01a);
        break;
        ITEM_STRING(keytag01a);
        break;
        ITEM_STRING(clairebox01a);
        break;
        ITEM_STRING(leonbox01a);
        break;
        ITEM_STRING(portablesafe01a);
        break;
        ITEM_STRING(cookiescan01a);
        break;
        ITEM_STRING(woodbox01a);
        break;
        ITEM_STRING(HandGun);
        break;
        ITEM_STRING(ShotGun);
        break;

    default:
        break;
    }

}