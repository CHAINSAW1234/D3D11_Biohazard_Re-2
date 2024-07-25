#include "stdafx.h"
#include "Cut_Scene_Manager.h"
#include "Cut_Scene.h"

IMPLEMENT_SINGLETON(CCut_Scene_Manager)

CCut_Scene_Manager::CCut_Scene_Manager()
{
}

HRESULT CCut_Scene_Manager::Initialize()
{
    m_CutScenes.resize(static_cast<_uint>(CF_ID::_END));

    return S_OK;
}

HRESULT CCut_Scene_Manager::Add_CutScene(CF_ID eCutScene, CCut_Scene* pCutScene)
{
    if (static_cast<_uint>(eCutScene) >= static_cast<_uint>(CF_ID::_END))
        return E_FAIL;

    Safe_Release(m_CutScenes[static_cast<_uint>(eCutScene)]);
    m_CutScenes[static_cast<_uint>(eCutScene)] = pCutScene;
    Safe_AddRef(m_CutScenes[static_cast<_uint>(eCutScene)]);

    return S_OK;
}

void CCut_Scene_Manager::Play_CutScene(CF_ID eCutScene)
{
    if (static_cast<_uint>(eCutScene) >= static_cast<_uint>(CF_ID::_END))
        return;

    m_CutScenes[static_cast<_uint>(eCutScene)]->Play();
}

CGameObject* CCut_Scene_Manager::Get_CutScene(CF_ID eCutScene)
{
    if (static_cast<_uint>(eCutScene) >= static_cast<_uint>(CF_ID::_END))
        return nullptr;

    return m_CutScenes[static_cast<_uint>(eCutScene)];
}

void CCut_Scene_Manager::Free()
{
    __super::Free();

    for (auto& pCutScene : m_CutScenes)
    {
        Safe_Release(pCutScene);
        pCutScene = nullptr;
    }
    m_CutScenes.clear();
}
