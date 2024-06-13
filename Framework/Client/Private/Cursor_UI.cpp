#include "stdafx.h"
#include "Cursor_UI.h"

CCursor_UI::CCursor_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCustomize_UI{ pDevice, pContext }
{
}

CCursor_UI::CCursor_UI(const CCursor_UI& rhs)
    : CCustomize_UI{ rhs }
{

}

HRESULT CCursor_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCursor_UI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CCursor_UI::Tick(_float fTimeDelta)
{
    POINT ptPos = {};

    GetCursorPos(&ptPos);
    ScreenToClient(g_hWnd, &ptPos);

    _float4 vMouse = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
    vMouse.x = m_pGameInstance->Get_ViewMousePos().x - (g_iWinSizeX * 0.5f);
    vMouse.y = -(m_pGameInstance->Get_ViewMousePos().y + (g_iWinSizeY * 0.5f));
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMouse);

    __super::Tick(fTimeDelta);
}

void CCursor_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CCursor_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

CCustomize_UI* CCursor_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCursor_UI* pInstance = new CCursor_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CCursor_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCursor_UI::Clone(void* pArg)
{
    CCursor_UI* pInstance = new CCursor_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CCursor_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCursor_UI::Free()
{
    __super::Free();
}
