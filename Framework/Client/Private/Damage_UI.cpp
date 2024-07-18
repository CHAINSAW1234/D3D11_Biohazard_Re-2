#include "stdafx.h"
#include "Damage_UI.h"
#include "Tab_Window.h"
#include "Player.h"

#define WHITE_COLOR     _float4(1, 1, 1, 1)
#define ALPHA_ZERO      _float4(0, 0, 0, 0)

#define DAMAGE_LIFE     3.f
#define DAMAGE_SPEED    1.5f

CDamage_UI::CDamage_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCustomize_UI{ pDevice, pContext }
{
}

CDamage_UI::CDamage_UI(const CDamage_UI& rhs)
    : CCustomize_UI{ rhs }
{

}

HRESULT CDamage_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDamage_UI::Initialize(void* pArg)
{
    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;
    }

    if (FAILED(Change_Tool()))
        return E_FAIL;

    return S_OK;
}

void CDamage_UI::Tick(_float fTimeDelta)
{
    if (nullptr == m_pPlayer)
    {
        Find_Player();

        if (nullptr == m_pPlayer)
            MSG_BOX(TEXT("CDamage_UI(): Player를 찾을 수 없습니다. "));
    }
    __super::Tick(fTimeDelta);

    Operate_Damage(fTimeDelta);
}   

void CDamage_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CDamage_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CDamage_UI::Change_Tool()
{
    m_Mask[0].fMaskControl = _float2(0.3f, 0.8f);

    m_vColor[0].vColor = _float4(0.572f, 0.09f, 0.09f, 0.f);

    m_isRender = false;

    return S_OK;
}

void CDamage_UI::Damage_Reset()
{
    m_fDamager_Timer = 0.f;
    m_isRender = false;
    m_isDamage_Start = false;

    m_vCurrentColor.w = 0.f;
}

void CDamage_UI::Operate_Damage(_float fTimeDelta)
{
    /* 1. 데미지를 입었을 때*/
    if (true == m_isDamage_Start)
    {
        m_isRender = true;
        m_fDamager_Timer += fTimeDelta;

        if (m_fDamager_Timer >= DAMAGE_LIFE)
        {
            m_vCurrentColor.w -= fTimeDelta * DAMAGE_SPEED;

            if (m_vCurrentColor.w <= 0.f)
            {
                m_vCurrentColor.w = 0.f;
                Damage_Reset();
            }
        }

        else
        {
            m_vCurrentColor.w += fTimeDelta * DAMAGE_SPEED;

            /* 알파는 0.8f 로 제한한다.*/
            if (m_vCurrentColor.w >= 1.f)
                m_vCurrentColor.w = 1.f;
        }
    }

    /* 2. 데미지를 입지 않았을 때*/
    else if (false == m_isDamage_Start)
        m_isRender = false;
}

CCustomize_UI* CDamage_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDamage_UI* pInstance = new CDamage_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CDamage_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDamage_UI::Clone(void* pArg)
{
    CDamage_UI* pInstance = new CDamage_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CDamage_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDamage_UI::Free()
{
    __super::Free();
}
