#include "stdafx.h"

#include "ImGui_Manager.h"
#include "GameInstance.h"
#include "Tab_Window.h"
#include "Player.h"

#include "ItemProp.h"


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

void CImgui_Manager::Start()
{
    m_pMeshViewer = m_pTabWindow->GetMeshViewer();
    Safe_AddRef(m_pMeshViewer);
}

void CImgui_Manager::Tick()
{
    if (true == m_isStart)
    {
        m_isStart = false;
        Start();
    }

    ResetVariable();//벨류 리셋

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();


    if (DOWN == m_pGameInstance->Get_KeyState('Z'))
    {
        m_bTabWindow_Debuger = !m_bTabWindow_Debuger;
        m_bPlayer_Debuger = !m_bPlayer_Debuger;
        m_bShader_Debuger = !m_bShader_Debuger;
    }

    if (true == m_bTabWindow_Debuger)
        Window_TabWindow_Debuger();

    if (true == m_bPlayer_Debuger)
        Window_Player_Debuger();

    if (true == m_bShader_Debuger)
        Window_Shader_Debuger();

    if (false == m_pMeshViewer->Get_Dead() && true == m_bTabWindow_Debuger)
        Gizmo(m_pMeshViewer->Get_Transform());

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

    if (ImGui::Button(u8"PickUp ITEM", ImVec2(100.f, 30.f)) && TEXT("") != m_wstrCurrent_Item)
    {
        _uint iFindNum = static_cast<_uint>(ITEM_NUMBER_END);
        for (_uint i = 0; i < m_vecItemNums.size(); i++)
        {
            if (m_wstrCurrent_Item == m_vecItemNums[i])
            {
                iFindNum = i;
            }
        }

        if (iFindNum == static_cast<_uint>(ITEM_NUMBER_END))
            return;
        
        list<CGameObject*>* pOBJLayer = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_InteractObj"));
        for (auto& iter : *pOBJLayer)
        {
            CItemProp* pItemProp = dynamic_cast<CItemProp*>(iter);
            if (nullptr != pItemProp)
            {
                if (false == pItemProp->Get_Dead() && iFindNum == pItemProp->Get_iItemIndex())
                {
                    m_pPlayer->PickUp_Item(iter);
                    break;
                }
            }
        }
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

void CImgui_Manager::Window_Shader_Debuger()
{
    ImGui::Begin("Shader_Debug");

#ifdef _DEBUG
    if (ImGui::Checkbox("Debug_Render", &m_bDebugRender))
    {
        m_pGameInstance->On_Off_DebugRender();
    }
#endif

    NL;

    if (ImGui::Checkbox("SSAO", &m_bShaderOnOff[SSAO]))
    {
        m_pGameInstance->Set_ShaderState(SSAO, m_bShaderOnOff[SSAO]);
    }

    SL;

    if (ImGui::Checkbox("SSR", &m_bShaderOnOff[SSR]))
    {
        m_pGameInstance->Set_ShaderState(SSR, m_bShaderOnOff[SSR]);
    }

    SL;

    if (ImGui::Checkbox("DOF", &m_bShaderOnOff[DOF]))
    {
        m_pGameInstance->Set_ShaderState(DOF, m_bShaderOnOff[DOF]);
    }

    NL;

    if (ImGui::Checkbox("VOLUMETRIC", &m_bShaderOnOff[VOLUMETRIC]))
    {
        m_pGameInstance->Set_ShaderState(VOLUMETRIC, m_bShaderOnOff[VOLUMETRIC]);
    }

    SL;

    if (ImGui::Checkbox("FXAA", &m_bShaderOnOff[FXAA]))
    {
        m_pGameInstance->Set_ShaderState(FXAA, m_bShaderOnOff[FXAA]);
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

void CImgui_Manager::EditTransform(CTransform* pTransformCom)
{
#ifndef _DEBUG
    return;
#endif
    ImGuizmo::BeginFrame();

    _float4x4 matrix = pTransformCom->Get_WorldFloat4x4();

    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

    if (ImGui::RadioButton("Translate", m_eGizmoState == ImGuizmo::TRANSLATE))
        m_eGizmoState = ImGuizmo::TRANSLATE;
    ImGui::SameLine();

    if (ImGui::RadioButton("Rotate", m_eGizmoState == ImGuizmo::ROTATE))
        m_eGizmoState = ImGuizmo::ROTATE;
    ImGui::SameLine();

    if (ImGui::RadioButton("Scale", m_eGizmoState == ImGuizmo::SCALE))
        m_eGizmoState = ImGuizmo::SCALE;

    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    ImGuizmo::DecomposeMatrixToComponents(matrix.m[0], matrixTranslation, matrixRotation, matrixScale);
    ImGui::InputFloat3("Tr", matrixTranslation);
    ImGui::InputFloat3("Rt", matrixRotation);
    ImGui::InputFloat3("Sc", matrixScale);
    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix.m[0]);

    if (m_eGizmoState != ImGuizmo::SCALE)
    {
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
            mCurrentGizmoMode = ImGuizmo::LOCAL;

        ImGui::SameLine();

        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
            mCurrentGizmoMode = ImGuizmo::WORLD;
    }

    static bool useSnap(false);
    ImGui::Checkbox("##UseSnap", &useSnap);
    ImGui::SameLine();

    static _float snap[3] = { 1.f, 1.f, 1.f };
    switch (m_eGizmoState)
    {
    case ImGuizmo::TRANSLATE:
        ImGui::InputFloat3("Snap", &snap[0]);
        break;
    case ImGuizmo::ROTATE:
        ImGui::InputFloat("Angle Snap", &snap[0]);
        break;
    case ImGuizmo::SCALE:
        ImGui::InputFloat("Scale Snap", &snap[0]);
        break;
    }

    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    _float4x4 ViewMatrix, ProjMatrix;
    ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
    ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
    ImGuizmo::Manipulate(ViewMatrix.m[0], ProjMatrix.m[0], m_eGizmoState, mCurrentGizmoMode, matrix.m[0], NULL, useSnap ? &snap[0] : NULL);
    pTransformCom->Set_WorldMatrix(matrix);
}

void CImgui_Manager::Gizmo(CTransform* pTransform)
{
    ImGui::Begin("Editor");

    if (ImGuizmo::IsUsing())
    {
        ImGui::Text("Using gizmo");
    }
    else
    {
        ImGui::Text(ImGuizmo::IsOver() ? "Over gizmo" : "");
        ImGui::SameLine();
        ImGui::Text(ImGuizmo::IsOver(ImGuizmo::TRANSLATE) ? "Over translate gizmo" : "");
        ImGui::SameLine();
        ImGui::Text(ImGuizmo::IsOver(ImGuizmo::ROTATE) ? "Over rotate gizmo" : "");
        ImGui::SameLine();
        ImGui::Text(ImGuizmo::IsOver(ImGuizmo::SCALE) ? "Over scale gizmo" : "");
    }

    ImGui::Separator();

    EditTransform(pTransform);

    ImGui::End();
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

    Safe_Release(m_pMeshViewer);

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
        ITEM_STRING(woodbarricade01a);
        break;
        ITEM_STRING(blastingfuse01a);
        break;
        ITEM_STRING(_9vbattery01a);
        break;
        ITEM_STRING(gunpowder01a);
        break;
        ITEM_STRING(gunpowder01b);
        break;
        ITEM_STRING(strengtheningyellow01a);
        break;
        ITEM_STRING(vp70powerup);
        break;
        ITEM_STRING(vp70longmagazine);
        break;
        ITEM_STRING(shotgunpartsstock_00);
        break;
        ITEM_STRING(shotgunpartsbarrel);
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
        ITEM_STRING(kingscepter01a);
        break;
        ITEM_STRING(virginheart01a);
        break;
        ITEM_STRING(blankkey01a);
        break;
        ITEM_STRING(statuebook01a);
        break;
        ITEM_STRING(statuehand01a);
        break;
        ITEM_STRING(virginmedal01a);
        break;
        ITEM_STRING(diakey01a);
        break;
        ITEM_STRING(virginmedal02a);
        break;
        ITEM_STRING(chaincutter01a);
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
        ITEM_STRING(HandGun);
        break;
        ITEM_STRING(ShotGun);
        break;
        ITEM_STRING(Flash_Bomb);
        break;
        ITEM_STRING(Grenade);
        break;
        ITEM_STRING(vp70stock);
        break;
        ITEM_STRING(portablesafe);
        break;
    default:
        break;
    }

    return wstring();
}