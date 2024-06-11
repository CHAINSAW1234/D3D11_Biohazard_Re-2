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

    if (nullptr == m_pCurrentAnimTag || nullptr == m_pCurrentModelTag)
        return E_FAIL;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_strCollasingTag = "Tool_Animlist";

    return S_OK;
}

void CTool_AnimList::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    static ImVec2 WinSize;
    WinSize = ImGui::GetWindowSize();

    ImGui::Begin("Animation_Selector");

    Show_Default();
    Show_AnimationTags();

    ImGui::End();
}

HRESULT CTool_AnimList::Add_Animations(map<string, map<string, CAnimation*>> ModelAnimations)
{
    for (auto& PairModelAnimations : ModelAnimations)
    {
        map<string, CAnimation*>            Animations;
        for (auto& PairAnimations : PairModelAnimations.second)
        {
            if (nullptr == PairAnimations.second)
                return E_FAIL;

            Animations.emplace(PairAnimations.first, PairAnimations.second);
        }

        m_ModelAnimations.emplace(PairModelAnimations.first, Animations);
    }

    for (auto& PairModelAnimations : m_ModelAnimations)
    {
        map<string, CAnimation*>            Animations;
        for (auto& PairAnimations : PairModelAnimations.second)
        {
            Safe_AddRef(PairAnimations.second);
        }
    }

    return S_OK;
}

void CTool_AnimList::Show_Default()
{
    ImGui::SeparatorText("Information");

    ImGui::Text("Current Selected Animation : ");		ImGui::SameLine();
    ImGui::Text(m_pCurrentAnimTag->c_str());

    ImGui::SeparatorText("");
}

void CTool_AnimList::Show_AnimationTags()
{
    if (false == Check_ModelExist(*m_pCurrentModelTag))
        return;

    static _float2		vSize = { 200.f, 100.f };

    if (ImGui::CollapsingHeader("Show Models ##CTool_AnimList::Show_AnimationTags()"))
    {
        ImGui::SliderFloat2("ListBoxSize ##CTool_AnimList::Show_AnimationTags()", (_float*)&vSize, 100.f, 800.f);
        if (ImGui::BeginListBox("##CTool_AnimList::Show_AnimationTags()", *(ImVec2*)&vSize))
        {
            Animations      Animations = { m_ModelAnimations[*m_pCurrentModelTag] };

            for (auto& PairAnimations : Animations)
            {
                string          strAnimTag = { PairAnimations.first };
                if (ImGui::Selectable(strAnimTag.c_str()))
                {
                    *m_pCurrentAnimTag = strAnimTag;
                }
            }

            ImGui::EndListBox();
        }
    }
}

CAnimation* CTool_AnimList::Get_Animation(const string& strAnimTag)
{
    if (false == Check_AnimExtist(strAnimTag))
        return nullptr;

    return m_ModelAnimations[*m_pCurrentModelTag][strAnimTag];
}

CAnimation* CTool_AnimList::Get_CurrentAnimation()
{
    if (false == Check_AnimExtist(*m_pCurrentAnimTag))
        return nullptr;

    return m_ModelAnimations[*m_pCurrentModelTag][*m_pCurrentAnimTag];
}

void CTool_AnimList::Add_Animation_CurrentModel(CAnimation* pAnimation)
{
    if (nullptr == pAnimation)
        return;

    if (false == Check_ModelExist(*m_pCurrentModelTag))
        return;

    string      strAnimTag = { pAnimation->Get_Name() };
    if (true == Check_AnimExtist(strAnimTag))
        return;

    m_ModelAnimations[*m_pCurrentModelTag].emplace(strAnimTag, pAnimation);
    Safe_AddRef(pAnimation);
}

_bool CTool_AnimList::Check_ModelExist(const string& strModelTag)
{
    map<string, Animations>::iterator		iter = { m_ModelAnimations.find(strModelTag) };

    return iter != m_ModelAnimations.end();
}

_bool CTool_AnimList::Check_AnimExtist(const string& strAnimTag)
{
    if (false == Check_ModelExist(*m_pCurrentModelTag))
        return false;

    Animations                  Animations = { m_ModelAnimations[*m_pCurrentModelTag] };
    Animations::iterator        iter = { Animations.find(strAnimTag) };

    return iter != Animations.end();
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

    for (auto& PairModelAnimations : m_ModelAnimations)
    {
        for (auto& PairAnimations : PairModelAnimations.second)
        {
            Safe_Release(PairAnimations.second);
            PairAnimations.second = nullptr;
        }

        PairModelAnimations.second.clear();
    }
    m_ModelAnimations.clear();
}
