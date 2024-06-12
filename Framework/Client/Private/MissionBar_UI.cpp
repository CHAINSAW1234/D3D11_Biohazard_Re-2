#include "stdafx.h"
#include "MissionBar_UI.h"

CMissionBar_UI::CMissionBar_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCustomize_UI{ pDevice, pContext }
{
}

CMissionBar_UI::CMissionBar_UI(const CMissionBar_UI& rhs)
    : CCustomize_UI{ rhs }
{

}

HRESULT CMissionBar_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMissionBar_UI::Initialize(void* pArg)
{
    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;
    }

    m_isPlay = false;

    return S_OK;
}

void CMissionBar_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (DOWN == m_pGameInstance->Get_KeyState('M'))
        m_isMission = true;

}

void CMissionBar_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CMissionBar_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

CCustomize_UI* CMissionBar_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMissionBar_UI* pInstance = new CMissionBar_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CMissionBar_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMissionBar_UI::Clone(void* pArg)
{
    CMissionBar_UI* pInstance = new CMissionBar_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CMissionBar_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMissionBar_UI::Free()
{
    __super::Free();

}
