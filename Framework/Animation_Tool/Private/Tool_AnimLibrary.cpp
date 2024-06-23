#include "stdafx.h"
#include "Tool_AnimLibrary.h"

CTool_AnimLibrary::CTool_AnimLibrary(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CTool{ pDevice, pContext }
{
}

HRESULT CTool_AnimLibrary::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    ANIMLIBRARY_DESC*           pDesc = { static_cast<ANIMLIBRARY_DESC*>(pArg) };
    m_pCurrentModelTag = pDesc->pCurrentModelTag;
    m_pCurrentAnimLayerTag = pDesc->pCurrentAnimLayerTag;
    if (nullptr == m_pCurrentModelTag || nullptr == m_pCurrentAnimLayerTag)
        return E_FAIL;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_strCollasingTag = "Tool_AnimLibrary";

    return S_OK;
}

void CTool_AnimLibrary::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    static ImVec2 WinSize;
    WinSize = ImGui::GetWindowSize();

    ImGui::Begin("Animation_Library");

    Show_Func();
    Show_Default();

    ImGui::End();
}

void CTool_AnimLibrary::Set_CurrentModel(CModel* pModel)
{
    Safe_Release(m_pCurrentModel);
    m_pCurrentModel = nullptr;

    m_pCurrentModel = pModel;
    Safe_AddRef(m_pCurrentModel);
}

void CTool_AnimLibrary::Show_Default()
{
    list<wstring>           AnimLayerTags = { m_pGameInstance->Get_AnimationLayer_Tags() };

    ImGui::SeparatorText("");
    _uint                   iNumListBox = { 0 };
    for (auto& wstrAnimLayerTag : AnimLayerTags)
    {
        string          strAnimLayerTag = { Convert_Wstring_String(wstrAnimLayerTag) };
        string          strResult = { strAnimLayerTag + string("##CollapsingHeader CTool_AnimLibrary::Show_Default()") };
        if(ImGui::CollapsingHeader(strResult.c_str()))
        {
            string      strListBoxLabel = { strAnimLayerTag + string("##ListBox CTool_AnimLibrary::Show_Default()") };
            string      strNumListBox = { to_string(iNumListBox++) };

            if (ImGui::Button(string(string("Select ##CTool_AnimLibrary::Show_Default()") + strNumListBox).c_str()))
            {
                *m_pCurrentAnimLayerTag = wstrAnimLayerTag;
            }           

            static _float2		vSize = { 200.f, 100.f };
            ImGui::SliderFloat2(string(string("ListBoxSize ##CTool_AnimLibrary::Show_Default()") + strNumListBox).c_str(), (_float*)&vSize, 100.f, 800.f);
            if(ImGui::BeginListBox(strListBoxLabel.c_str(), *(ImVec2*)&vSize))
            {
                list<string>          AnimationTags = { m_pGameInstance->Get_Animation_Tags(wstrAnimLayerTag) };
                for (auto& strAnimTag : AnimationTags)
                {
                    if (ImGui::Selectable(strAnimTag.c_str()))
                    {
                    }
                }

                ImGui::EndListBox();
            }
            
        }        
    }    
    ImGui::SeparatorText("");
}

void CTool_AnimLibrary::Show_Func()
{
    ImGui::SeparatorText("");

    string         strCurrentModelTag = { "" };
    string         strCurrentPrototypeAnimsTag = { Convert_Wstring_String(*m_pCurrentAnimLayerTag) };

    if (nullptr != m_pCurrentModelTag)
    {
        strCurrentModelTag = { *m_pCurrentModelTag };
    }

    ImGui::Text(string(string("Current Selected Model : ") + strCurrentModelTag).c_str());
    ImGui::Text(string(string("Current Selected Prototype Anim Layer : ") + strCurrentPrototypeAnimsTag).c_str());

    if (ImGui::Button("Add_Animation To Current Model"))
    {
        Add_Animations_To_CurrentModel();
    }

    ImGui::SeparatorText("");
}

void CTool_AnimLibrary::Add_Animations_To_CurrentModel()
{
    if (nullptr == m_pCurrentModel)
        return;

    if (TEXT("") == *m_pCurrentAnimLayerTag)
        return;

    if (FAILED(m_pCurrentModel->Add_Animations(*m_pCurrentAnimLayerTag, *m_pCurrentAnimLayerTag)))
    {
        MSG_BOX(TEXT("Already Exist AnimLayer"));
    }
}

CTool_AnimLibrary* CTool_AnimLibrary::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CTool_AnimLibrary*          pInatnace = { new CTool_AnimLibrary(pDevice, pContext) };

    if (FAILED(pInatnace->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CTool_AnimLibrary"));

        Safe_Release(pInatnace);
    }

    return pInatnace;
}

void CTool_AnimLibrary::Free()
{
    __super::Free();

    Safe_Release(m_pCurrentModel);
}
