#include "stdafx.h"
#include "Tool_AnimList.h"

#include "Animation.h"

CTool_AnimList::CTool_AnimList(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CTool_Selector{ pDevice, pContext }
{
}

HRESULT CTool_AnimList::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_strCollasingTag = "Tool_Animlist";

    return S_OK;
}

void CTool_AnimList::Tick(_float fTimeDelta)
{
    static string		strSelectTag = { "Select Animation" };

    _uint			iNumAnim = { static_cast<_uint>(m_Animations.size()) };
    string*         AnimTags = { new string[iNumAnim] };

    _uint			iIndex = { 0 };
    for (auto& Pair : m_Animations)
    {
        AnimTags[iIndex++] = Pair.first;
    }

    if (ImGui::CollapsingHeader(m_strCollasingTag.c_str()))
    {
        if (ImGui::BeginCombo("Animation ## AnimListTool", strSelectTag.c_str()))
        {
            for (_uint i = 0; i < iNumAnim; ++i)
            {
                _bool		isSelected = { AnimTags[i] == strSelectTag };
                if (ImGui::Selectable(AnimTags[i].c_str(), isSelected))
                {
                    strSelectTag = AnimTags[i];
                    m_strCurrentAnimTag = strSelectTag;
                }
                if (true == isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            Safe_Delete_Array(AnimTags);
            ImGui::EndCombo();
        }
    }    
}

CAnimation* CTool_AnimList::Get_Animation(const string& strAnimTag)
{
    map<string, CAnimation*>::iterator      iter = { m_Animations.find(strAnimTag) };
    if (iter == m_Animations.end())
        return nullptr;
    
    return m_Animations[strAnimTag];
}

CAnimation* CTool_AnimList::Get_CurrentAnimation()
{
    map<string, CAnimation*>::iterator      iter = { m_Animations.find(m_strCurrentAnimTag) };
    if (iter == m_Animations.end())
        return nullptr;

    return m_Animations[m_strCurrentAnimTag];
}

void CTool_AnimList::Add_Animation(CAnimation* pAnimation)
{
    if (nullptr == pAnimation)
        return;

    string                                  strAnimTag = { pAnimation->Get_Name() };
    map<string, CAnimation*>::iterator      iter = { m_Animations.find(strAnimTag)};

    if (iter != m_Animations.end())
        return;

    m_Animations[strAnimTag] = pAnimation;
    Safe_AddRef(pAnimation);
}

CTool_AnimList* CTool_AnimList::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    CTool_AnimList*         pInatnace = { new CTool_AnimList(pDevice, pContext) };

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

    for (auto& Pair : m_Animations)
        Safe_Release(Pair.second);    
    m_Animations.clear();
}
