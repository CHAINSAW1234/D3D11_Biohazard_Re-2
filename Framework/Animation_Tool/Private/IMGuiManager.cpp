#include "stdafx.h"
#include "IMGuiManager.h"

#include "AnimationEditor.h"
#include "Debuger.h"

CIMGuiManager::CIMGuiManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice { pDevice}
    , m_pContext {pContext}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

CIMGuiManager::CIMGuiManager(const CIMGuiManager& rhs)
{
}

HRESULT CIMGuiManager::Initialize()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    /* ImGui 초기화 */
    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(m_pDevice, m_pContext);

    ZeroMemory(&m_Editors, sizeof(m_Editors));

    CAnimationEditor*       pAnimEditor = { CAnimationEditor::Create(m_pDevice, m_pContext) };
    if (nullptr == pAnimEditor)
        return E_FAIL;

    m_Editors[EDITOR_TYPE::EDITOR_ANIMATION] = pAnimEditor;

    ZeroMemory(&m_Debugers, sizeof(m_Debugers));

    m_Debugers[DEBUGER_TYPE::DEBUGER_OBJECT] = CDebuger::Create(m_pDevice, m_pContext);

    return S_OK;
}

void CIMGuiManager::Tick(_float fTimeDelta)
{
    m_fAccTimeForFrame += fTimeDelta;
    ++m_iFrameCnt;

    if (m_fAccTimeForFrame >= 1.f)
    {
        m_fAccTimeForFrame -= 1.f;
        m_iPreFrame = m_iFrameCnt;
        m_iFrameCnt = 0;
    }

    if (true == Check_LevelChange())
    {
        if (FAILED(Reset_All()))
            return;
    }

    if (DOWN == CGameInstance::Get_Instance()->Get_KeyState('V'))
    {
        OnOffImgui();
    }   



    IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing Dear ImGui context. Refer to examples app!");

    // Examples Apps (accessible from the "Examples" menu)
    static bool show_app_main_menu_bar = false;
    static bool show_app_console = false;
    static bool show_app_custom_rendering = false;
    static bool show_app_documents = false;
    static bool show_app_log = false;
    static bool show_app_layout = false;
    static bool show_app_property_editor = false;
    static bool show_app_simple_overlay = false;
    static bool show_app_auto_resize = false;
    static bool show_app_constrained_resize = false;
    static bool show_app_fullscreen = false;
    static bool show_app_long_text = false;
    static bool show_app_window_titles = false;

    // Dear ImGui Tools (accessible from the "Tools" menu)
    static bool show_tool_metrics = false;
    static bool show_tool_debug_log = false;
    static bool show_tool_id_stack_tool = false;
    static bool show_tool_style_editor = false;
    static bool show_tool_about = false;

    if (show_tool_metrics)
        ImGui::ShowMetricsWindow(&show_tool_metrics);
    if (show_tool_debug_log)
        ImGui::ShowDebugLogWindow(&show_tool_debug_log);
    if (show_tool_id_stack_tool)
        ImGui::ShowIDStackToolWindow(&show_tool_id_stack_tool);
    if (show_tool_style_editor)
    {
        ImGui::Begin("Dear ImGui Style Editor", &show_tool_style_editor);
        ImGui::ShowStyleEditor();
        ImGui::End();
    }
    if (show_tool_about)
        ImGui::ShowAboutWindow(&show_tool_about);

    // Demonstrate the various window flags. Typically you would just use the default!
    static bool no_titlebar = false;
    static bool no_scrollbar = false;
    static bool no_menu = false;
    static bool no_move = false;
    static bool no_resize = false;
    static bool no_collapse = false;
    static bool no_close = false;
    static bool no_nav = false;
    static bool no_background = false;
    static bool no_bring_to_front = false;
    static bool unsaved_document = false;

    ImGuiWindowFlags window_flags = 0;
    if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
    if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
    if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
    if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
    if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
    if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
    if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
    if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
    if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;

    if (true == m_isEnable)
    {
        /* 실제 에디터에 대한 구성 */
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        /* IMGUI STYLE */
        ImGuiStyle& style = ImGui::GetStyle();
        style.FrameRounding = 12.f;
        style.GrabRounding = 12.f;
        style.FrameBorderSize = 1.f;
        style.Colors[ImGuiCol_TabHovered] = ImVec4(1.f, 0.f, 0.f, 0.5f);
        style.Colors[ImGuiCol_TabActive] = ImVec4(1.f, 0.f, 0.f, 1.0f);
        /*========================================================================*/

        ImGui::Begin(u8"Editor_Select");

        ImGui::Text("Frame : %d", m_iPreFrame);

        // Menu Bar
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Menu"))
            {
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Examples"))
            {
                ImGui::MenuItem("Main menu bar", NULL, &show_app_main_menu_bar);

                ImGui::SeparatorText("Mini apps");
                ImGui::MenuItem("Console", NULL, &show_app_console);
                ImGui::MenuItem("Custom rendering", NULL, &show_app_custom_rendering);
                ImGui::MenuItem("Documents", NULL, &show_app_documents);
                ImGui::MenuItem("Log", NULL, &show_app_log);
                ImGui::MenuItem("Property editor", NULL, &show_app_property_editor);
                ImGui::MenuItem("Simple layout", NULL, &show_app_layout);
                ImGui::MenuItem("Simple overlay", NULL, &show_app_simple_overlay);

                ImGui::SeparatorText("Concepts");
                ImGui::MenuItem("Auto-resizing window", NULL, &show_app_auto_resize);
                ImGui::MenuItem("Constrained-resizing window", NULL, &show_app_constrained_resize);
                ImGui::MenuItem("Fullscreen window", NULL, &show_app_fullscreen);
                ImGui::MenuItem("Long text display", NULL, &show_app_long_text);
                ImGui::MenuItem("Manipulating window titles", NULL, &show_app_window_titles);

                ImGui::EndMenu();
            }
            //if (ImGui::MenuItem("MenuItem")) {} // You can also use MenuItem() inside a menu bar!
            if (ImGui::BeginMenu("Tools"))
            {
#ifndef IMGUI_DISABLE_DEBUG_TOOLS
                const bool has_debug_tools = true;
#else
                const bool has_debug_tools = false;
#endif
                ImGui::MenuItem("Metrics/Debugger", NULL, &show_tool_metrics, has_debug_tools);
                ImGui::MenuItem("Debug Log", NULL, &show_tool_debug_log, has_debug_tools);
                ImGui::MenuItem("ID Stack Tool", NULL, &show_tool_id_stack_tool, has_debug_tools);
                ImGui::MenuItem("Style Editor", NULL, &show_tool_style_editor);
                bool is_debugger_present = ImGui::GetIO().ConfigDebugIsDebuggerPresent;
                if (ImGui::MenuItem("Item Picker", NULL, false, has_debug_tools && is_debugger_present))
                    ImGui::DebugStartItemPicker();
                if (!is_debugger_present)
                    ImGui::SetItemTooltip("Requires io.ConfigDebugIsDebuggerPresent=true to be set.\n\nWe otherwise disable the menu option to avoid casual users crashing the application.\n\nYou can however always access the Item Picker in Metrics->Tools.");
                ImGui::Separator();
                ImGui::MenuItem("About Dear ImGui", NULL, &show_tool_about);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::Text("dear imgui says hello! (%s) (%d)", IMGUI_VERSION, IMGUI_VERSION_NUM);
        ImGui::Spacing();
        if (ImGui::CollapsingHeader("Help"))
        {
            ImGui::SeparatorText("ABOUT THIS DEMO:");
            ImGui::BulletText("Sections below are demonstrating many aspects of the library.");
            ImGui::BulletText("The \"Examples\" menu above leads to more demo contents.");
            ImGui::BulletText("The \"Tools\" menu above gives access to: About Box, Style Editor,\n"
                "and Metrics/Debugger (general purpose Dear ImGui debugging tool).");

            ImGui::SeparatorText("PROGRAMMER GUIDE:");
            ImGui::BulletText("See the ShowDemoWindow() code in imgui_demo.cpp. <- you are here!");
            ImGui::BulletText("See comments in imgui.cpp.");
            ImGui::BulletText("See example applications in the examples/ folder.");
            ImGui::BulletText("Read the FAQ at https://www.dearimgui.com/faq/");
            ImGui::BulletText("Set 'io.ConfigFlags |= NavEnableKeyboard' for keyboard controls.");
            ImGui::BulletText("Set 'io.ConfigFlags |= NavEnableGamepad' for gamepad controls.");

            ImGui::SeparatorText("USER GUIDE:");
            ImGui::ShowUserGuide();
        }

        if (ImGui::CollapsingHeader("Configuration"))
        {
            ImGuiIO& io = ImGui::GetIO();

            if (ImGui::TreeNode("Configuration##2"))
            {
                ImGui::SeparatorText("General");
                ImGui::CheckboxFlags("io.ConfigFlags: NavEnableKeyboard", &io.ConfigFlags, ImGuiConfigFlags_NavEnableKeyboard);
                ImGui::CheckboxFlags("io.ConfigFlags: NavEnableGamepad", &io.ConfigFlags, ImGuiConfigFlags_NavEnableGamepad);
                ImGui::CheckboxFlags("io.ConfigFlags: NavEnableSetMousePos", &io.ConfigFlags, ImGuiConfigFlags_NavEnableSetMousePos);
                ImGui::CheckboxFlags("io.ConfigFlags: NoMouse", &io.ConfigFlags, ImGuiConfigFlags_NoMouse);
                if (io.ConfigFlags & ImGuiConfigFlags_NoMouse)
                {
                    // The "NoMouse" option can get us stuck with a disabled mouse! Let's provide an alternative way to fix it:
                    if (fmodf((float)ImGui::GetTime(), 0.40f) < 0.20f)
                    {
                        ImGui::SameLine();
                        ImGui::Text("<<PRESS SPACE TO DISABLE>>");
                    }
                    if (ImGui::IsKeyPressed(ImGuiKey_Space))
                        io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
                }
                ImGui::CheckboxFlags("io.ConfigFlags: NoMouseCursorChange", &io.ConfigFlags, ImGuiConfigFlags_NoMouseCursorChange);
                ImGui::Checkbox("io.ConfigInputTrickleEventQueue", &io.ConfigInputTrickleEventQueue);
                ImGui::Checkbox("io.MouseDrawCursor", &io.MouseDrawCursor);

                ImGui::SeparatorText("Widgets");
                ImGui::Checkbox("io.ConfigInputTextCursorBlink", &io.ConfigInputTextCursorBlink);
                ImGui::Checkbox("io.ConfigInputTextEnterKeepActive", &io.ConfigInputTextEnterKeepActive);
                ImGui::Checkbox("io.ConfigDragClickToInputText", &io.ConfigDragClickToInputText);
                ImGui::Checkbox("io.ConfigWindowsResizeFromEdges", &io.ConfigWindowsResizeFromEdges);
                ImGui::Checkbox("io.ConfigWindowsMoveFromTitleBarOnly", &io.ConfigWindowsMoveFromTitleBarOnly);
                ImGui::Checkbox("io.ConfigMacOSXBehaviors", &io.ConfigMacOSXBehaviors);
                ImGui::Text("Also see Style->Rendering for rendering options.");

                ImGui::SeparatorText("Debug");
                ImGui::Checkbox("io.ConfigDebugIsDebuggerPresent", &io.ConfigDebugIsDebuggerPresent);
                ImGui::BeginDisabled();
                ImGui::Checkbox("io.ConfigDebugBeginReturnValueOnce", &io.ConfigDebugBeginReturnValueOnce); // .
                ImGui::EndDisabled();
                ImGui::Checkbox("io.ConfigDebugBeginReturnValueLoop", &io.ConfigDebugBeginReturnValueLoop);
                ImGui::Checkbox("io.ConfigDebugIgnoreFocusLoss", &io.ConfigDebugIgnoreFocusLoss);
                ImGui::Checkbox("io.ConfigDebugIniSettings", &io.ConfigDebugIniSettings);

                ImGui::TreePop();
                ImGui::Spacing();
            }

            if (ImGui::TreeNode("Backend Flags"))
            {
                // FIXME: Maybe we need a BeginReadonly() equivalent to keep label bright?
                ImGui::BeginDisabled();
                ImGui::CheckboxFlags("io.BackendFlags: HasGamepad", &io.BackendFlags, ImGuiBackendFlags_HasGamepad);
                ImGui::CheckboxFlags("io.BackendFlags: HasMouseCursors", &io.BackendFlags, ImGuiBackendFlags_HasMouseCursors);
                ImGui::CheckboxFlags("io.BackendFlags: HasSetMousePos", &io.BackendFlags, ImGuiBackendFlags_HasSetMousePos);
                ImGui::CheckboxFlags("io.BackendFlags: RendererHasVtxOffset", &io.BackendFlags, ImGuiBackendFlags_RendererHasVtxOffset);
                ImGui::EndDisabled();
                ImGui::TreePop();
                ImGui::Spacing();
            }

            if (ImGui::TreeNode("Style"))
            {
                ImGui::ShowStyleEditor();
                ImGui::TreePop();
                ImGui::Spacing();
            }

            if (ImGui::TreeNode("Capture/Logging"))
            {
                ImGui::LogButtons();

                if (ImGui::Button("Copy \"Hello, world!\" to clipboard"))
                {
                    ImGui::LogToClipboard();
                    ImGui::LogText("Hello, world!");
                    ImGui::LogFinish();
                }
                ImGui::TreePop();
            }
        }

        if (ImGui::CollapsingHeader("Window options"))
        {
            if (ImGui::BeginTable("split", 3))
            {
                ImGui::TableNextColumn(); ImGui::Checkbox("No titlebar", &no_titlebar);
                ImGui::TableNextColumn(); ImGui::Checkbox("No scrollbar", &no_scrollbar);
                ImGui::TableNextColumn(); ImGui::Checkbox("No menu", &no_menu);
                ImGui::TableNextColumn(); ImGui::Checkbox("No move", &no_move);
                ImGui::TableNextColumn(); ImGui::Checkbox("No resize", &no_resize);
                ImGui::TableNextColumn(); ImGui::Checkbox("No collapse", &no_collapse);
                ImGui::TableNextColumn(); ImGui::Checkbox("No close", &no_close);
                ImGui::TableNextColumn(); ImGui::Checkbox("No nav", &no_nav);
                ImGui::TableNextColumn(); ImGui::Checkbox("No background", &no_background);
                ImGui::TableNextColumn(); ImGui::Checkbox("No bring to front", &no_bring_to_front);
                ImGui::TableNextColumn(); ImGui::Checkbox("Unsaved document", &unsaved_document);
                ImGui::EndTable();
            }
        }

        ImGui::End();

        _uint iIdx = { 0 };
        for (auto& pEditor : m_Editors)
        {
            if (nullptr == pEditor)
                continue;

            Update_CurrentEditor(pEditor, (EDITOR_TYPE)iIdx++);

            pEditor->Tick(fTimeDelta);
        }

        for (auto& pDebuger : m_Debugers)
        {
            if (nullptr == pDebuger)
                continue;

            pDebuger->Tick(fTimeDelta);
        }
    }
}

HRESULT CIMGuiManager::Render()
{
    if (true == m_isEnable)
    {
        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    return S_OK;
}

HRESULT CIMGuiManager::Reset_All()
{
    for (_uint i = 0; i < EDITOR_TYPE::EDITOR_END; ++i)
    {
        if (nullptr == m_Editors[i])
            continue; 

        if (FAILED(m_Editors[i]->Reset()))
        {
            MSG_BOX(TEXT("Reset Failed ImguiEditor"));

            return E_FAIL;
        }
    }

    for (_uint i = 0; i < DEBUGER_TYPE::DEBUGER_END; ++i)
    {
        if (nullptr == m_Debugers[i])
            continue;

        if (FAILED(m_Debugers[i]->Reset()))
        {
            MSG_BOX(TEXT("Reset Failed ImguiEditor"));

            return E_FAIL;
        }
    }

    return S_OK;
}

_bool CIMGuiManager::Check_LevelChange()
{
    static LEVEL eCurrentLevel = LEVEL::LEVEL_END;
    static LEVEL ePreLevel = LEVEL::LEVEL_END;

    _bool isLevelChanged = { false };

    eCurrentLevel = (LEVEL)CGameInstance::Get_Instance()->Get_CurrentLevel();

    if (LEVEL::LEVEL_LOADING != eCurrentLevel 
        && ePreLevel != eCurrentLevel)
    {
        isLevelChanged = true;
    }
    else
    {
        isLevelChanged = false;
    }

    ePreLevel = eCurrentLevel;

    return isLevelChanged;
}

void CIMGuiManager::Update_CurrentEditor(class CEditor* pEditor, EDITOR_TYPE eType)
{
    if (true == pEditor->Get_Focus())
    {
        if (eType != m_CurrentEditor)
        {
            if (EDITOR_END != m_CurrentEditor)
                m_Editors[m_CurrentEditor]->Set_Focus(false);

            m_CurrentEditor = eType;
        }
    }
}

CIMGuiManager* CIMGuiManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CIMGuiManager* pInstance = new CIMGuiManager(pDevice, pContext);
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed To Create : ImGuiManager"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CIMGuiManager::Free()
{
    __super::Free();

    ImGui_ImplWin32_Shutdown();
    ImGui_ImplDX11_Shutdown();

    for (auto& pEditor : m_Editors)
    {
        Safe_Release(pEditor);
    }

    for (auto& pDebuger : m_Debugers)
    {
        Safe_Release(pDebuger);
    }

    ImGui::DestroyContext();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
