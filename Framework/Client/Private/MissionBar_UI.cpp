#include "stdafx.h"
#include "MissionBar_UI.h"
#include "CustomCollider.h"
#include "Player.h"
#include "Tab_Window.h"

#define LIFE_TIME 3.f
#define ALHPA_ZERO_VEC _vector()
#define ALPHA_ZERO _float4(0, 0, 0, 0)

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

        /* 부모가 정보 전달의 연결다리가 될 것이다. */
        Find_Player();

        Find_MissionVariable();

        // Mission_Text();
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

            m_vecTextBoxes.back()->Set_Text(TEXT("경찰서로 이동하기"));

            m_vOriginTextColor = _float4(1, 1, 1, 1);

            m_vecTextBoxes.back()->Set_FontColor(ALHPA_ZERO_VEC);

        }

        list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

        for (auto& iter : *pUIList)
        {
            CMissionBar_UI* pParent = dynamic_cast<CMissionBar_UI*>(iter);

            if (nullptr != pParent && false == pParent->m_IsChild)
            {
                m_pMissionBar = pParent;

               // Safe_AddRef<CMissionBar_UI*>(m_pMissionBar);
                break;
            }
        }
    }

    if(nullptr == m_pTabWindow)
    {
        CGameObject* pTabWindow = m_pGameInstance->Get_GameObject(g_Level, TEXT("Layer_TabWindow"), 0);
        
        m_pTabWindow = static_cast<CTab_Window*>(pTabWindow);
    }

    if (FAILED(Change_Tool()))
        return E_FAIL;

    return S_OK;
}

void CMissionBar_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (nullptr == m_pTabWindow)
        return;

    if(false == *m_pTabWindow->Get_MainRender_Ptr())
    {
        TabWindow_Mission();

        m_isTabDead = *m_pTabWindow->Get_MainRender_Ptr();
    }

    else if (m_isTabDead != *m_pTabWindow->Get_MainRender_Ptr())
    {
        Mission_Variable_Return();

        m_isTabDead = *m_pTabWindow->Get_MainRender_Ptr();
    }
    else
    {
        InGame_Mission(fTimeDelta);
    }
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

HRESULT CMissionBar_UI::Change_Tool()
{
    m_SavePos[2] = m_SavePos[1] = m_SavePos[0];
    m_vColor[1].vColor.x = 1.f;
    m_vColor[1].vColor.y = 1.f;
    m_vColor[1].vColor.z = 1.f;
    m_vColor[1].vColor.w = 0.5f;

    m_fColorTimer_Limit = 1.f;
    m_isRender = false;
    m_fLightSize = 1.f;

    /*Client */
    m_isPlay = true;
    m_fLifeTimer = 0.f;
    m_iColorCurNum = 0;

    /* Light */
    m_isMission_NonLighting = false;
    m_fLightPosition.x = 3.f;
    m_fLightPositionY = 0.f;

    /* Tool */
    m_isKeepPlay = false;
    m_isLight = false;

    return S_OK;
}

void CMissionBar_UI::Mission_Text(wstring _write)
{
    //vector<wstring> pTextVec =
    //{
    //    /* 경찰서 기본 미션 */
    //    L"주변 환경 탐색하기",
    //    L"상점 탈출하기",
    //    L"경찰서로 이동하기",
    //    L"주변 환경 탐색하기",
    //    L"동쪽 지역 조사하기",
    //    L"메인 홀로 돌아가기",
    //    L"지하로 가는 길 찾기",
    //    L"메달 3개 찾기",
    //    L"나가는 길 찾기",

    //    /* 이후 */
    //    L"전력 패널 부품 찾기",
    //    L"주차장 키 카드 획득",
    //    L"주차장 탈출하기",
    //};

    m_pEventsQueue.push(_write);
}

void CMissionBar_UI::Find_MissionVariable()
{
    CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_Player"))->front());

    if (nullptr != pPlayer)
    {
        m_isMissionClear = pPlayer->Get_MissionClear_Ptr();

        if (nullptr == m_isMissionClear)
        {
            MSG_BOX(TEXT("CMissionBar_UI() : Failed Mission Setting"));

            return;
        }
    }
}

void CMissionBar_UI::MissionCollider_Access()
{
    if (nullptr == m_pPlayer)
        return;

    if (true == *m_isMissionClear)
        Mission_Start();
}

/* CMissionBar_UI와 관련된 함수 하나만 불러도 쓸 수 있다. */
void CMissionBar_UI::Write_Mission_Text(wstring _write)
{
    list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

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

void CMissionBar_UI::TabWindow_Mission()
{
    if (MISSION_UI_TYPE::BAR_MISSION == m_eMissionUI_Type)
    {
        m_fLightPosition.y = m_fLightPositionY;

        m_iColorCurNum = static_cast<_uint>(m_iColorMaxNum);

        m_isKeepPlay = true;

        m_isPlay = true;

        m_isRender = true;

        m_fBlending = 0.2f;
    }

    else if (MISSION_UI_TYPE::ARROW_MISSION == m_eMissionUI_Type)
    {
        m_isRender = true;

        m_fBlending = 0.f;
    }

    else if (MISSION_UI_TYPE::FONT_MISSION == m_eMissionUI_Type)
    {
        m_isRender = true; 

        if (!m_vecTextBoxes.empty())
        {
            for (auto& pFont : m_vecTextBoxes)
            {
                if (TEXT("") == pFont->Get_Text())
                {
                    pFont->Set_Text(TEXT("경찰서로 이동하기"));
                }

                pFont->Set_FontColor(m_vOriginTextColor);
            }
        }
    }
}

void CMissionBar_UI::InGame_Mission(_float fTimeDelta)
{
    if (MISSION_UI_TYPE::BAR_MISSION == m_eMissionUI_Type)
    {
        m_fLightPosition.y = m_fLightPositionY;

        if (nullptr == m_pPlayer)
        {
            Find_Player();

            if (nullptr == m_pPlayer)
                MSG_BOX(TEXT("CMissionBar_UI() : Failed Find Player"));
        }

        MissionCollider_Access();

        Operater_MissionBar(fTimeDelta);
    }

    else if (MISSION_UI_TYPE::ARROW_MISSION == m_eMissionUI_Type)
    {
        Operater_MissionArrow(fTimeDelta);
    }

    else if (MISSION_UI_TYPE::FONT_MISSION == m_eMissionUI_Type)
    {
        Operater_MissionFont(fTimeDelta);
    }
}

void CMissionBar_UI::Operater_MissionBar(_float fTimeDelta)
{
    if (m_iColorCurNum == 1)
    {
        m_isLight = true;

        m_fColorTimer_Limit = 0.4f;
    }

    else
    {
        m_isLight = false;  

        m_fColorTimer_Limit = 1.f;
    }

    if (m_iColorCurNum >= (_uint)m_iColorMaxNum)
    {
        m_isKeepPlay = true;
    }

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
    {
        m_fLightPosition.x -= fTimeDelta * 7.f;
    }
}

void CMissionBar_UI::Operater_MissionArrow(_float fTimeDelta)
{
    if (true == m_pMissionBar->m_isRender)
    {
        m_fMissionArrow_Timer += fTimeDelta;

        m_isRender = true;

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
            for(auto& pFont : m_vecTextBoxes)
            {
                _float4 result = m_pMissionBar->m_fBlending * ALPHA_ZERO + (1 - m_pMissionBar->m_fBlending) * m_vOriginTextColor;
                
                pFont->Set_FontColor(result);
            }
        }
    }
    /* 시작하지 않았다면, */
    else
    {
        m_vCurrentColor = ALPHA_ZERO;

        m_fMissionFont_Timer = 0.f;
    }
}

void CMissionBar_UI::Mission_Start()
{
    if (!m_pMission_ColliderQueue.empty())
        m_pMission_ColliderQueue.pop();


    m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_mission.mp3"), 0.5f);

    /*Client */
    m_isFontStart = true;

    m_isRender = m_isPlay = true;

    m_fLifeTimer = 0.f;

    m_isPlay = true;

    m_iColorCurNum = 0;

    /* Light */
    m_isMission_NonLighting = false;

    m_fLightPosition.x = 3.f;

    /* Tool */
    m_isKeepPlay = false;

    m_isLight = false;

    *m_isMissionClear = false;
}

void CMissionBar_UI::Mission_Variable_Return()
{
    if (MISSION_UI_TYPE::BAR_MISSION == m_eMissionUI_Type)
    {
        m_iColorCurNum = 0;

        m_isKeepPlay = false;

        m_isPlay = false;

        m_isRender = false;

        m_fBlending = 1.f;
    }

    else if (MISSION_UI_TYPE::ARROW_MISSION == m_eMissionUI_Type)
    {
        m_isRender = false;

        m_fBlending = 1.f;
    }

    else if (MISSION_UI_TYPE::FONT_MISSION == m_eMissionUI_Type)
    {
        m_isRender = false;

        if (!m_vecTextBoxes.empty())
        {
            for (auto& pFont : m_vecTextBoxes)
                pFont->Set_FontColor(ALPHA_ZERO);
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

  /*  if (nullptr != m_pMissionBar)
        Safe_Release<CMissionBar_UI*>(m_pMissionBar);*/

  /*  for(auto& iter : m_pMissionCollVec)
        Safe_Release(iter);
    m_pMissionCollVec.clear();*/
 
}
