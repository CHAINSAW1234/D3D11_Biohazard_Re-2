#include "stdafx.h"
#include "Tool_AnimList.h"

#include "Animation.h"

CTool_AnimList::CTool_AnimList()
{
}

HRESULT CTool_AnimList::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CTool_AnimList::Tick(_float fTimeDelta)
{

}

CAnimation* CTool_AnimList::Get_Animation(const string& strAnimTag)
{
    map<string, CAnimation*>::iterator      iter = { m_Animations.find(strAnimTag) };
    if (iter == m_Animations.end())
        return nullptr;
    
    return m_Animations[strAnimTag];
}

CTool_AnimList* CTool_AnimList::Create(void* pArg)
{
    CTool_AnimList*         pInatnace = { new CTool_AnimList() };

    if (FAILED(pInatnace->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CTool_AnimList"));

        Safe_Release(pInatnace);
    }

    return pInatnace;
}

void CTool_AnimList::Free()
{
    for (auto& Pair : m_Animations)
        Safe_Release(Pair.second);    
    m_Animations.clear();
}
