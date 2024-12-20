#include "stdafx.h"
#include "Tutorial_UI.h"
#include "Player.h"

#define ALPHA_ZERO _float4(0, 0, 0, 0)
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
    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;

        m_pTutorial_Supervise = static_cast<CTutorial_UI*>(CustomUIDesc->pSupervisor);

        if (nullptr == m_pTutorial_Supervise)
            m_pTutorial_Supervise = this;
    }

    /* 2. Parent */
    if(false == m_IsChild)
    {
        Find_Player();

        m_pisTutorial_Notify = m_pPlayer->Get_Tutorial_Notify();

        m_eTutorial_Type = m_pPlayer->Get_Tutorial_Type();
    }

    if (FAILED(Change_Tool()))
        return E_FAIL;

    Non_Frame();

    Frame_Reset();

    return S_OK;
}

void CTutorial_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);
    
    if(false == m_isReady)
    {
        m_isReady = true;

        /* 1. Font */
        if (!m_vecTextBoxes.empty())
        {
            m_vOriginTextColor = m_vecTextBoxes.back()->Get_FontColor();

            m_vecTextBoxes.back()->Set_FontColor(ALPHA_ZERO);

            m_Origin_TextPosition.x = m_vecTextBoxes.back()->GetPosition().x;
            m_Origin_TextPosition.y = m_vecTextBoxes.back()->GetPosition().y;
        }
    }

    if (false == m_IsChild)
    {
        Find_TextBox();

        First_Behaivor(fTimeDelta);
    }
    else
    {
        if (true == m_pTutorial_Supervise->m_isRender)
            m_isRender = true;

        else if (false == m_pTutorial_Supervise->m_isRender)
            m_isRender = false;
    }

    Player_First_Interact();

    Tutorial_Start();

    Operation_Tutorial(fTimeDelta);
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

HRESULT CTutorial_UI::Change_Tool()
{
    m_isPlay = false;

    m_iEndingType = PLAY_BUTTON::PLAY_CHANGE;

    m_fColorTimer_Limit = 1.f;

    m_Mask[0].isMask = m_isMask = false;

    m_isRender = false;

    return S_OK;
}

void CTutorial_UI::First_Behaivor(_float fTimeDelta)
{
    if (true == (*m_pPlayer->Get_Tutorial_Notify()) || true == m_isTutorial_Start)
        return;

    m_fPlayTimer += fTimeDelta;

    if(m_fPlayTimer > 3.f)
        m_isStart = true;

    if(true == m_isStart)
    {
        if(false == m_isTutorial[(_int)UI_TUTORIAL_TYPE::TUTORIAL_AROUND])
        {
            if (true == (*m_pPlayer->Get_Tutorial_Notify()))
                return;

            *m_pisTutorial_Notify = true;

            *m_eTutorial_Type = UI_TUTORIAL_TYPE::TUTORIAL_AROUND;

            m_isTutorial[(_int)UI_TUTORIAL_TYPE::TUTORIAL_AROUND] = true;
        }

        /*else if (false == m_isTutorial[(_int)UI_TUTORIAL_TYPE::TUTORIAL_WALK])
        {
            if (true == (*m_pPlayer->Get_Tutorial_Notify()))
                return;

            *m_pPlayer->Get_Tutorial_Notify() = true;
            *m_pPlayer->Get_Tutorial_Type() = UI_TUTORIAL_TYPE::TUTORIAL_WALK;
            m_isTutorial[(_int)UI_TUTORIAL_TYPE::TUTORIAL_WALK] = true;
        }*/
    }
}

void CTutorial_UI::Play_Font(_float fTimeDelta)
{
    /* 만약 플레이 중이라면, */
    CTextBox* pFont = m_vecTextBoxes.back();
    _float4 result = m_fBlending * ALPHA_ZERO + (1 - m_fBlending) * m_vOriginTextColor;
    pFont->Set_FontColor(result);
}

void CTutorial_UI::Player_First_Interact()
{
    if (true == m_IsChild)
        return;

    if (nullptr == pTutorial_TextBox[0] || nullptr == pTutorial_TextBox[1])
        MSG_BOX(TEXT("Tutorial UI에서 생성된 TextBox가 없습니다."));

    pTutorial_TextBox[1]->Set_Position_UI(m_fTextPosition);
    
    _float3 pos = pTutorial_TextBox[1]->Get_Position_UI();

    pos.x += 5.f;

    pTutorial_TextBox[1]->Set_Position_UI(pos);

    if (true == (*m_pisTutorial_Notify) && false == m_isTutorial_Start)
    {
        switch (*m_eTutorial_Type)
        {
        case UI_TUTORIAL_TYPE::TUTORIAL_WALK:
            pTutorial_TextBox[0]->Set_Text(TEXT("빨리걷기"));
            pTutorial_TextBox[1]->Set_Text(TEXT("S"));
            break;

        case UI_TUTORIAL_TYPE::TUTORIAL_AROUND:
            pTutorial_TextBox[0]->Set_Text(TEXT("둘러보기"));
            pTutorial_TextBox[1]->Set_Text(TEXT("E"));
            break;

        case UI_TUTORIAL_TYPE::TUTORIAL_RUN :
            pTutorial_TextBox[0]->Set_Text(TEXT("뛰기"));
            pTutorial_TextBox[1]->Set_Text(TEXT("3"));
            break;

        case UI_TUTORIAL_TYPE::TUTORIAL_AIM :   
            pTutorial_TextBox[0]->Set_Text(TEXT(" 겨누기"));
            pTutorial_TextBox[1]->Set_Text(TEXT("L"));
            break;

        case UI_TUTORIAL_TYPE::INVENTORY_OPEN:
            pTutorial_TextBox[0]->Set_Text(TEXT("인벤토리"));
            pTutorial_TextBox[1]->Set_Text(TEXT("T"));
            break;

        case UI_TUTORIAL_TYPE::TUTORIAL_REROAD :
            pTutorial_TextBox[0]->Set_Text(TEXT(" 재장전"));
            pTutorial_TextBox[1]->Set_Text(TEXT("R"));
            break;

        case UI_TUTORIAL_TYPE::TUTORIAL_EQUIP :
            pTutorial_TextBox[0]->Set_Text(TEXT("장착하기"));
            pTutorial_TextBox[1]->Set_Text(TEXT("2"));
            break;

        case UI_TUTORIAL_TYPE::TUTORIAL_HOLD:
            pTutorial_TextBox[0]->Set_Text(TEXT("지탱하기"));
            pTutorial_TextBox[1]->Set_Text(TEXT("U"));
            break;
        }
    }
}

void CTutorial_UI::Find_TextBox()
{
    if (nullptr != pTutorial_TextBox[0] && nullptr != pTutorial_TextBox[1])
        return;

    list<class CGameObject*>* pUILayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));
    _int i = { 0 };

    for (auto& iter : *pUILayer)
    {
        CTutorial_UI* pTutorial = dynamic_cast<CTutorial_UI*>(iter);

        if (nullptr != pTutorial)
        {
            if (!pTutorial->m_vecTextBoxes.empty())
            {
                pTutorial_TextBox[i] = pTutorial->m_vecTextBoxes.back();
                i++;
            }
        }
    }

    if (pTutorial_TextBox[0]->GetPosition().y > pTutorial_TextBox[1]->GetPosition().y)
    {
        CTextBox* pTemp = pTutorial_TextBox[0];

        pTutorial_TextBox[0] = pTutorial_TextBox[1];

        pTutorial_TextBox[1] = pTemp;
    }

    m_fTextPosition = pTutorial_TextBox[1]->Get_Position_UI();
}

void CTutorial_UI::Tutorial_Start()
{
    if (true == m_isTutorial_Start)
        return;

    if (true == *m_pTutorial_Supervise->m_pisTutorial_Notify)
    {
        Non_Frame();

        m_isTutorial_Start = true;

        m_isPlay = true;

        m_isKeepPlay = false;

        m_isRender = true;

        m_fTutorial_Life_Tiemr = 0.f;

        Frame_Reset();
    }
}

void CTutorial_UI::Operation_Tutorial(_float fTimeDelta)
{
    if (true == m_isPlay)
    {
        if ((_int)m_iColorCurNum >= m_iColorMaxNum)
        {
            m_isKeepPlay = true;

            m_iColorCurNum = m_iColorMaxNum;

            m_fTutorial_Life_Tiemr += fTimeDelta;

            if (LIFE_TIME <= m_fTutorial_Life_Tiemr)
                m_isPlay = false;
        }
    }

    else if (false == m_isPlay && m_fBlending < 1.f)
    {
        m_fBlending += fTimeDelta;

        if (m_fBlending > 1.f)
        {
            *m_pTutorial_Supervise->m_pisTutorial_Notify = false;

            m_isTutorial_Start = false;

            m_fBlending = 1.f;
            m_isPlay = false;
            m_isRender = false;

            m_fBlending = 1.f;

            if (false == m_IsChild && UI_TUTORIAL_TYPE::TUTORIAL_AROUND == *m_eTutorial_Type)
            {
                m_pPlayer->MissionClear_Font(TEXT("경찰서로 이동하기"), static_cast<_ubyte>(MISSION_TYPE::FOLD_ENTRANCE_MISSION));
            }
        }
    }

    if (!m_vecTextBoxes.empty())
    {
        Play_Font(fTimeDelta);
    }
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
