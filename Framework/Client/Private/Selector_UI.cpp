#include "stdafx.h"
#include "Selector_UI.h"

#define ALHPE_ZERO _float4(0, 0, 0, 0)

CSelector_UI::CSelector_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCustomize_UI{ pDevice, pContext }
{
}

CSelector_UI::CSelector_UI(const CSelector_UI& rhs)
    : CCustomize_UI{ rhs }
{

}

HRESULT CSelector_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSelector_UI::Initialize(void* pArg)
{
    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;
    }

    /* Tool*/
    m_fColorTimer_Limit = 1.f;
    m_iEndingType = PLAY_BUTTON::PLAY_DEFAULT;

    return S_OK;
}

void CSelector_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

}

void CSelector_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CSelector_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

CCustomize_UI* CSelector_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSelector_UI* pInstance = new CSelector_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CSelector_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSelector_UI::Clone(void* pArg)
{
    CSelector_UI* pInstance = new CSelector_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CSelector_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSelector_UI::Free()
{
    __super::Free();

}
