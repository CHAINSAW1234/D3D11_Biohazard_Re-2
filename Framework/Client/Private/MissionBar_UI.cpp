#include "stdafx.h"
#include "MissionBar_UI.h"

#define LIFE_TIME 3.f
#define ALHPE_ZERO_VEC _vector()
#define ALHPE_ZERO _float4(0, 0, 0, 0)

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

    if (false == m_IsChild)
    {
        m_isPlay = false;
        m_iEndingType = PLAY_BUTTON::PLAY_CHANGE;
        m_eMissionUI_Type = MISSION_UI_TYPE::BAR_MISSION;

        Mission_Start();
    }

    else if (true == m_IsChild) /* 부모 찾기 */
    {
        /* 화살표*/
        if (m_vecTextBoxes.empty())
        {
            m_eMissionUI_Type = MISSION_UI_TYPE::ARROW_MISSION;
            m_fBlending = 1.f;
        }

        /* 폰트*/
        else
        {
            m_eMissionUI_Type = MISSION_UI_TYPE::FONT_MISSION;
            m_strMissionText = TEXT("상점 탈출하기");

            m_vecTextBoxes.back()->Set_Text(m_strMissionText);
            m_vOriginTextColor = _float4(1, 1, 1, 1);
            m_vecTextBoxes.back()->Set_FontColor(ALHPE_ZERO_VEC);
        }

        list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));

        for (auto& iter : *pUIList)
        {
            CMissionBar_UI* pParent = dynamic_cast<CMissionBar_UI*>(iter);

            if (nullptr != pParent && false == pParent->m_IsChild)
            {
                m_pMissionBar = pParent;
                break;
            }
        }
    }
    m_SavePos[1] = m_SavePos[0];
    m_vColor[1].vColor.x = 1.f;
    m_vColor[1].vColor.y = 1.f;
    m_vColor[1].vColor.z = 1.f;
    m_fColorTimer_Limit = 1.f;
    m_isLight = false;
    m_isRender = false;
    m_fLightPosition.x = 3.f;
    m_fLightSize = 1.f;
    return S_OK;
}

void CMissionBar_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (MISSION_UI_TYPE::BAR_MISSION == m_eMissionUI_Type)
        Operater_MissionBar(fTimeDelta);

    else if (MISSION_UI_TYPE::ARROW_MISSION == m_eMissionUI_Type)
        Operater_MissionArrow(fTimeDelta);

    else if (MISSION_UI_TYPE::FONT_MISSION == m_eMissionUI_Type)
        Operater_MissionFont(fTimeDelta);
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

/* CMissionBar_UI와 관련된 함수 하나만 불러도 쓸 수 있다. */
void CMissionBar_UI::Write_Mission_Text(wstring _write)
{
    list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));

    for (auto& iter : *pUIList)
    {
        CMissionBar_UI* pMissionText = dynamic_cast<CMissionBar_UI*>(iter);

        if (nullptr != pMissionText && !pMissionText->m_vecTextBoxes.empty())
        {
            pMissionText->m_vecTextBoxes.back()->Set_Text(_write);
            break;
        }
    }
}

void CMissionBar_UI::Operater_MissionBar(_float fTimeDelta)
{
    if (DOWN == m_pGameInstance->Get_KeyState('M'))
        Mission_Start();

    if (m_iColorCurNum == 1)
    {
        m_isLight = true;
        m_fColorTimer_Limit = 0.5f;
    }
    else
    {
        m_isLight = false;  
        m_fColorTimer_Limit = 1.f;
    }

    if (m_iColorCurNum >= m_iColorMaxNum)
        m_isKeepPlay = true;

    if (true == m_isKeepPlay)
    {
        m_fLifeTimer += fTimeDelta;

        if(LIFE_TIME <= m_fLifeTimer)
        {
            m_isPlay = false;
            m_fBlending += fTimeDelta;

            if (m_fBlending >= 1.f)
            {
                m_fBlending = 1.f;
                m_isRender = false;
            }
        }
    }
    if (true == m_isLight)
        m_fLightPosition.x -= fTimeDelta * 6.f;
}

void CMissionBar_UI::Operater_MissionArrow(_float fTimeDelta)
{
    /* 부모가 렌더를 시작했다면, */
    if (true == m_pMissionBar->m_isRender)
    {
        m_fMissionArrow_Timer += fTimeDelta;
        m_isRender = true;

        /* 1. 시간이 넘었다면 다시 사라지기 */
        if (m_pMissionBar->m_fLifeTimer >= LIFE_TIME)
        {
            m_fBlending += fTimeDelta;

            if (m_fBlending >= 1.f)
                m_fBlending = 1.f;
        }
        else if (m_fMissionArrow_Timer >= 1.f)
        {
            m_fBlending -= fTimeDelta;

            if (m_fBlending <= 0.f)
                m_fBlending = 0.f;
        }
    }

    /* 시작하지 않았다면, */
    else
        m_isRender = false;
}

void CMissionBar_UI::Operater_MissionFont(_float fTimeDelta)
{
    /* 부모가 렌더를 시작했을 때*/
    if (true == m_pMissionBar->m_isRender)
    {
        if(!m_vecTextBoxes.empty())
        {
            CTextBox* pFont = m_vecTextBoxes.back();

            _float4 result = m_pMissionBar->m_fBlending * ALHPE_ZERO + (1 - m_pMissionBar->m_fBlending) * m_vOriginTextColor;
            pFont->Set_FontColor(result);
        }
    }
    /* 시작하지 않았다면, */
    else
    {
        m_vCurrentColor = ALHPE_ZERO;
        m_fMissionFont_Timer = 0.f;
    }
}

void CMissionBar_UI::Mission_Start()
{
    /*Client */
    m_isRender = m_isPlay = true;
    m_fLifeTimer = 0.f;
    m_isPlay = true;
    m_iColorCurNum = 0;
    /* Light */
    m_isMission_NonLighting = false;
    m_fLightPosition.x = 3.f;
    /* Tool */
    m_isKeepPlay = false;
}

void CMissionBar_UI::Mission_Light(_float fTimeDelta)
{
    /* if Size 8, DownGrade */
    if (m_fLightSize >= 10.f)
        m_isMission_NonLighting = true;

    // 빛 밝기
    if (false == m_isMission_NonLighting)
    {
        m_fLightSize += fTimeDelta * 10.f;
        m_fLightPosition.x += fTimeDelta * 6.f;
    }

    else if (true == m_isMission_NonLighting)
    {
        m_isPlay = false;

        m_fLightSize -= fTimeDelta * 10.f;
        m_fLightPosition.x -= fTimeDelta * 6.f;

        if (3.8f >= m_fLightSize)/////////////////
        {
            m_fLightSize = 0.f;
            m_isLight = false;
            m_isKeepPlay = true;
        }
    }
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
