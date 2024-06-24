#include "stdafx.h"
#include "Tool_AnimList.h"

#include "Animation.h"

CTool_AnimList::CTool_AnimList(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CTool_Selector{ pDevice, pContext }
{
}

HRESULT CTool_AnimList::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    ANIMLIST_DESC* pDesc = { static_cast<ANIMLIST_DESC*>(pArg) };
    m_pCurrentAnimTag = pDesc->pCurrentAnimationTag;
    m_pCurrentModelTag = pDesc->pCurrentModelTag; 
    m_pCurrentAnimLayerTag = pDesc->pCurrentAnimLayerTag;

    if (nullptr == m_pCurrentAnimTag || nullptr == m_pCurrentModelTag || nullptr == m_pCurrentAnimLayerTag)
        return E_FAIL;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_strCollasingTag = "Tool_AnimList";

    return S_OK;
}

void CTool_AnimList::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    static ImVec2 WinSize;
    WinSize = ImGui::GetWindowSize();

    ImGui::Begin("Animation_Selector");

    Show_Default();
    //  Show_AnimationTags();
    Show_LayerTags();

    ImGui::End();
}

void CTool_AnimList::Set_CurrentModel(CModel* pModel)
{
    Safe_Release(m_pCurrentModel);
    m_pCurrentModel = nullptr;

    m_pCurrentModel = pModel;
    Safe_AddRef(m_pCurrentModel);
}

void CTool_AnimList::Show_Default()
{
    ImGui::SeparatorText("Information");

    ImGui::Text("Current Selected Animation : ");		ImGui::SameLine();
    ImGui::Text(m_pCurrentAnimTag->c_str());

    string          strCurrentAnimLayerTag = { Convert_Wstring_String(*m_pCurrentAnimLayerTag) };
    ImGui::Text("Current Selected Animation Layer : ");		ImGui::SameLine();
    ImGui::Text(strCurrentAnimLayerTag.c_str());

    ImGui::SeparatorText("");
}

void CTool_AnimList::Show_LayerTags()
{
    if (nullptr == m_pCurrentModel)
        return;

    static _float2		vSize = { 200.f, 100.f };
    _uint               iNumAnimLayer = {};

    list<wstring>           AnimLayerTags = { m_pCurrentModel->Get_AnimationLayer_Tags() };
    for (auto& wstrAnimLayerTag : AnimLayerTags)
    {
        string              strAnimLayerTag = { Convert_Wstring_String(wstrAnimLayerTag) };
        if(ImGui::CollapsingHeader(string(strAnimLayerTag + string("##Show_LayerTags()") + to_string(iNumAnimLayer)).c_str()))
        {
            vector<CAnimation*>         Animations = { m_pCurrentModel->Get_Animations(wstrAnimLayerTag) };

            ImGui::SliderFloat2(string(string("ListBoxSize ##CTool_AnimList::Show_LayerTags()") + to_string(iNumAnimLayer)).c_str(), (_float*)&vSize, 100.f, 800.f);
            if (ImGui::BeginListBox(string(string("##CTool_AnimList::Show_LayerTags()") + to_string(iNumAnimLayer)).c_str(), *(ImVec2*)&vSize))
            {
                for (auto& pAnimation : Animations)
                {
                    if (nullptr == pAnimation)
                        continue;

                    string          strAnimTag = { pAnimation->Get_Name() };

                    if (ImGui::Selectable(strAnimTag.c_str()))
                    {
                        *m_pCurrentAnimLayerTag = wstrAnimLayerTag;
                        *m_pCurrentAnimTag = strAnimTag;
                    }

                }
            }

            ImGui::EndListBox();
        }

        ++iNumAnimLayer;
    }
}

void CTool_AnimList::Show_AnimationTags()
{
    if (nullptr == m_pCurrentModel)
        return;


    //if (ImGui::CollapsingHeader("Show Models ##CTool_AnimList::Show_AnimationTags()"))
    //{
    //    ImGui::SliderFloat2("ListBoxSize ##CTool_AnimList::Show_AnimationTags()", (_float*)&vSize, 100.f, 800.f);
    //    if (ImGui::BeginListBox("##CTool_AnimList::Show_AnimationTags()", *(ImVec2*)&vSize))
    //    {
    //        list<wstring>           AnimLayerTags = { m_pCurrentModel->Get_AnimationLayer_Tags() };

    //        for (auto& strAnimLayerTag : AnimLayerTags)
    //        {
    //            //  Animations              Animations = { m_ModelAnimations[*m_pCurrentModelTag] };
    //            list<string>        AnimationTags = { m_pCurrentModel->Get_Animation_Tags(strAnimLayerTag) };

    //            for (auto& strAnimTag : AnimationTags)
    //            {
    //                if (ImGui::Selectable(strAnimTag.c_str()))
    //                {
    //                    *m_pCurrentAnimTag = strAnimTag;
    //                }
    //            }
    //        }

    //        ImGui::EndListBox();
    //    }
    //}
}

CAnimation* CTool_AnimList::Get_CurrentAnimation()
{
    if (nullptr == m_pCurrentModel)
        return nullptr;

    vector<CAnimation*>         Animations = { m_pCurrentModel->Get_Animations(*m_pCurrentAnimLayerTag) };
    
    for (auto& pAnimation : Animations)
    {
        if (nullptr == pAnimation)
            continue;

        string          strAnimTag = { pAnimation->Get_Name() };
        string          strCurrentAnimTag = { *m_pCurrentAnimTag };

        if (strAnimTag == strCurrentAnimTag)
            return pAnimation;
    }

    return nullptr;
}

CTool_AnimList* CTool_AnimList::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CTool_AnimList* pInatnace = { new CTool_AnimList(pDevice, pContext) };

    if (FAILED(pInatnace->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CTool_AnimList"));

        Safe_Release(pInatnace);
    }

    return pInatnace;
}

void CTool_AnimList::Free()
{
    __super::Free();

    Safe_Release(m_pCurrentModel);
    m_pCurrentModel = nullptr;
}
