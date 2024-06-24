#include "stdafx.h"
#include "Tutorial_UI.h"

#define ALHPE_ZERO _float4(0, 0, 0, 0)
#define LIFE_TIME 2.f

CTutorial_UI::CTutorial_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCustomize_UI{ pDevice, pContext }
{
}

CTutorial_UI::CTutorial_UI(const CTutorial_UI& rhs)
    : CCustomize_UI{ rhs }
{

}

HRESULT CTutorial_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTutorial_UI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (!m_vecTextBoxes.empty())
    {
        m_vOriginTextColor = m_vecTextBoxes.back()->Get_FontColor();
        m_vecTextBoxes.back()->Set_FontColor(ALHPE_ZERO);
    }

    m_isPlay = false;
    m_iEndingType = PLAY_BUTTON::PLAY_CHANGE;
    m_fColorTimer_Limit = 1.f;
    m_Mask[0].isMask = m_isMask = false;
    m_isRender = false;
    return S_OK;
}

void CTutorial_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (DOWN == m_pGameInstance->Get_KeyState('N'))
    {
        Non_Frame();
        m_isPlay = true;
        m_isKeepPlay = false;
        m_isRender = true;
        m_fTutorial_Life_Tiemr = 0.f;
        Frame_Reset();
    }

    if (true == m_isPlay)
    {
        /* ▶ Texture */
        /* UI가 나타났음*/
        if ((_int)m_iColorCurNum >= m_iColorMaxNum)
        {
            m_isKeepPlay = true;
            m_iColorCurNum = m_iColorMaxNum;

            m_fTutorial_Life_Tiemr += fTimeDelta;
            if (LIFE_TIME <= m_fTutorial_Life_Tiemr)
            {
                m_isPlay = false;
                if (1.f <= m_fBlending)
                {
                    m_fBlending = 1.f;
                    m_isPlay = false;
                    m_isRender = false;
                }
                else
                    m_fBlending += fTimeDelta * 2.f;
            }
        }
    }

    else if (false == m_isPlay && m_fBlending < 1.f)
    {
        m_fBlending += fTimeDelta;

        if (m_fBlending > 1.f)
            m_fBlending = 1.f;
    }

    /* ▶  Font */
    if (!m_vecTextBoxes.empty())
        Play_Font(fTimeDelta);
}

void CTutorial_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CTutorial_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CTutorial_UI::Play_Font(_float fTimeDelta)
{
    /* 만약 플레이 중이라면, */
    CTextBox* pFont = m_vecTextBoxes.back();

    /*if (1.f > m_fBlending)
    {
        pFont->Set_FontColor(ALHPE_ZERO);
        return;
    }*/

    _float4 result = m_fBlending * ALHPE_ZERO + (1 - m_fBlending) * m_vOriginTextColor;
    pFont->Set_FontColor(result);
}

CCustomize_UI* CTutorial_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTutorial_UI* pInstance = new CTutorial_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CTutorial_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTutorial_UI::Clone(void* pArg)
{
    CTutorial_UI* pInstance = new CTutorial_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CTutorial_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTutorial_UI::Free()
{
    __super::Free();
}
