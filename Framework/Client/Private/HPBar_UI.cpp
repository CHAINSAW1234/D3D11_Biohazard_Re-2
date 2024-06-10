#include "stdafx.h"
#include "HPBar_UI.h"

CHPBar_UI::CHPBar_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCustomize_UI{ pDevice, pContext }
{
}

CHPBar_UI::CHPBar_UI(const CHPBar_UI& rhs)
    : CCustomize_UI{ rhs }
{

}

HRESULT CHPBar_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHPBar_UI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CHPBar_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);
}

void CHPBar_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CHPBar_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}


CCustomize_UI* CHPBar_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHPBar_UI* pInstance = new CHPBar_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CHPBar_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHPBar_UI::Clone(void* pArg)
{
    CHPBar_UI* pInstance = new CHPBar_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CHPBar_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHPBar_UI::Free()
{
    __super::Free();
}
