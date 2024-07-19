#include "stdafx.h"
#include "Read_Item_UI.h"
#include "Player.h"

#include "Player.h"

#define ARROW_DISTANCE  30.f
#define INTRO_LIFE      1.5f
#define ALPHA_ZERO      _float4(0, 0, 0, 0)
#define WHITE_COLOR     _float4(1, 1, 1, 0)
#define MAX_BLENDING    0.7f

CRead_Item_UI::CRead_Item_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCustomize_UI{ pDevice, pContext }
{
}

CRead_Item_UI::CRead_Item_UI(const CRead_Item_UI& rhs)
    : CCustomize_UI{ rhs }
{

}

HRESULT CRead_Item_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRead_Item_UI::Initialize(void* pArg)
{
    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;

        if (CustomUIDesc->wstrFileName == TEXT("UI_Item_Introduce"))
        {
            m_eRead_type = READ_UI_TYPE::INTRODUCE_READ;
            m_vOriginColor = m_vCurrentColor = m_vColor[0].vColor;
            m_vColor[0].fBlender_Value = m_fBlending = MAX_BLENDING;
            m_vColor[0].vColor = _float4(0, 0, 0, 0);

            if (!m_vecTextBoxes.empty())
            {
                m_vecTextBoxes.back()->Set_FontColor(ALPHA_ZERO);
            }
        }

        else if (CustomUIDesc->wstrFileName == TEXT("UI_Item_Read"))
        {
            /* 조절을 위한 줌을 받아온다 .*/
            if (false == m_IsChild)
            {
                CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Player"))->front());
                m_pZoomOff = pPlayer->Get_ZoomOff();
            }

            /* Intro 부모 */
            m_pIntro_UI = Find_ReadUI(READ_UI_TYPE::INTRODUCE_READ, false);

            if (nullptr != m_pIntro_UI)
            {
                //Safe_AddRef<CRead_Item_UI*>(m_pIntro_UI);
            }

            if (true == m_IsChild)
            {
                if (1 == CustomUIDesc->iWhich_Child)
                {
                    list<class CGameObject*>* pUILayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));
                    CRead_Item_UI* pReadItem = static_cast<CRead_Item_UI*>(pUILayer->back());

                    if (READ_UI_TYPE::TEXTURE_READ == pReadItem->m_eRead_type)
                        m_eRead_type = READ_UI_TYPE::TEXT_LEFT_READ;

                    else
                        m_eRead_type = READ_UI_TYPE::TEXTURE_READ;
                }

                else if (2 == CustomUIDesc->iWhich_Child)
                    m_eRead_type = READ_UI_TYPE::TEXT_RIGHT_READ;

                m_pRead_Supervise = static_cast<CRead_Item_UI*>(CustomUIDesc->pSupervisor);
            }

            else
                m_eRead_type = READ_UI_TYPE::MAIN_READ; /* Read의 BackGround*/
        }

        else if (CustomUIDesc->wstrFileName == TEXT("UI_Item_Read_Arrow"))
        {
            m_eRead_type = READ_UI_TYPE::ARROW_READ;
            m_pIntro_UI = Find_ReadUI(READ_UI_TYPE::INTRODUCE_READ, false);
            m_pRead_Supervise = Find_ReadUI(READ_UI_TYPE::MAIN_READ, false);
            CRead_Item_UI* pTexture_UI = Find_ReadUI(READ_UI_TYPE::TEXTURE_READ, true);

            if (nullptr != pTexture_UI)
            {
                CTransform* pMain_Trans = static_cast<CTransform*>(pTexture_UI->Get_Component(g_strTransformTag));
                if (0.f <= m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).x)
                {
                    m_eRead_Arrow_Type = READ_ARROW_TYPE::RIGHT_ARROW;

                    _float4 vMainTrans = pMain_Trans->Get_State_Float4(CTransform::STATE_POSITION);
                    _float3 vMainScaled = pMain_Trans->Get_Scaled();

                    vMainTrans.x += (vMainScaled.x + ARROW_DISTANCE);
                    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMainTrans);
                }

                else
                {
                    m_eRead_Arrow_Type = READ_ARROW_TYPE::LEFT_ARROW;

                    _float4 vMainTrans = pMain_Trans->Get_State_Float4(CTransform::STATE_POSITION);
                    _float3 vMainScaled = pMain_Trans->Get_Scaled();

                    vMainTrans.x -= (vMainScaled.x - ARROW_DISTANCE);
                    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMainTrans);
                }
            }
            else
                MSG_BOX(TEXT("Read Item UI 사용 중임에도 [ Arrow ] 가 설정되지 않았습니다"));
        }
    }

    if (READ_UI_TYPE::MAIN_READ == m_eRead_type)
    {
        /* 첫 번째는 사진, 두 번째는 TEXT */
        m_BookText[ITEM_READ_TYPE::INCIDENT_LOG_NOTE]   = { TEXT("Prototype_Component_Texture_Document2"),  TEXT("Prototype_Component_Texture_ReadType_Incident_log1"), TEXT("Prototype_Component_Texture_ReadType_Incident_log2"), TEXT("Prototype_Component_Texture_ReadType_Incident_log3"), TEXT("Prototype_Component_Texture_ReadType_Incident_log4"), TEXT("Prototype_Component_Texture_ReadType_Incident_log5") };
        m_BookText[ITEM_READ_TYPE::OPERATE_REPORT_NOTE] = { TEXT("Prototype_Component_Texture_Document2"), TEXT("Prototype_Component_Texture_Operate_Report2"), TEXT("Prototype_Component_Texture_Operate_Report3") };
        m_BookText[ITEM_READ_TYPE::TASK_NOTE]           = { TEXT("Prototype_Component_Texture_DocumentBlood1"), TEXT("Prototype_Component_Texture_Task1"), TEXT("Prototype_Component_Texture_Task2"), TEXT("Prototype_Component_Texture_Task3"), TEXT("Prototype_Component_Texture_Task4"), TEXT("Prototype_Component_Texture_Task5") };
        m_BookText[ITEM_READ_TYPE::MEDICINAL_NOTE]      = { TEXT("Prototype_Component_Texture_Document1"), TEXT("Prototype_Component_Texture_Medicinal1"), TEXT("Prototype_Component_Texture_Medicinal2"), TEXT("Prototype_Component_Texture_Medicinal3"), TEXT("Prototype_Component_Texture_Medicinal4") };
        m_BookText[ITEM_READ_TYPE::GUNPOWDER_NOTE]      = { TEXT("Prototype_Component_Texture_Memo1"), TEXT("Prototype_Component_Texture_Gunpowder_Note1"), TEXT("Prototype_Component_Texture_Gunpowder_Note2"), TEXT("Prototype_Component_Texture_Gunpowder_Note3") };
        m_BookText[ITEM_READ_TYPE::FIX_LOCKER_NOTE]     = { TEXT("Prototype_Component_Texture_GuidePamphlet1"), TEXT("Prototype_Component_Texture_Fix_Lock_Note1") , TEXT("Prototype_Component_Texture_Fix_Lock_Note2") };
        m_BookText[ITEM_READ_TYPE::HAND_HELD_SAFE_NOTE] = { TEXT("Prototype_Component_Texture_Document1"), TEXT("Prototype_Component_Texture_Portable_Safe1") , TEXT("Prototype_Component_Texture_Portable_Safe2") };
        m_BookText[ITEM_READ_TYPE::RICKER_NOTE]         = { TEXT("Prototype_Component_Texture_DocumentBlood1"), TEXT("Prototype_Component_Texture_ReadType_Ricker1") , TEXT("Prototype_Component_Texture_ReadType_Ricker2") , TEXT("Prototype_Component_Texture_ReadType_Ricker3") , TEXT("Prototype_Component_Texture_ReadType_Ricker4") };
        m_BookText[ITEM_READ_TYPE::SAFE_PASSWARD_NOTE]  = { TEXT("Prototype_Component_Texture_Document1"), TEXT("Prototype_Component_Texture_Safe_PassWard_Note1") , TEXT("Prototype_Component_Texture_Safe_PassWard_Note2") , TEXT("Prototype_Component_Texture_Safe_PassWard_Note3")};
        m_BookText[ITEM_READ_TYPE::PAMPHLET]            = { TEXT("Prototype_Component_Texture_GuidePamphlet1"), TEXT("Prototype_Component_Texture_PamphletNote1"), TEXT("Prototype_Component_Texture_PamphletNote2"), TEXT("Prototype_Component_Texture_PamphletNote3"), TEXT("Prototype_Component_Texture_PamphletNote4") };
       
        m_BookText[ITEM_READ_TYPE::OFFICER_NOTE]        = { TEXT("Prototype_Component_Texture_GuidePamphlet1"), TEXT("Prototype_Component_Texture_ReadType_Police_Note1"), TEXT("Prototype_Component_Texture_ReadType_Police_Note2") };
    }
    
    m_isRender = false;

    return S_OK;
}

void CRead_Item_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    /* 예시 코드 */
    if (true == m_isReadCall)
    {
        m_isRender = true;
        Reset();
        m_isReadCall = false;
    }

    Render_Condition();

    /* 분기점 */
    if (READ_UI_TYPE::INTRODUCE_READ == m_eRead_type)
        Introduce_Read(fTimeDelta);

    else if (READ_UI_TYPE::ARROW_READ == m_eRead_type)
        Arrow_Read();

    else if (READ_UI_TYPE::TEXTURE_READ == m_eRead_type)
        Texture_Read();

    else if (READ_UI_TYPE::TEXT_LEFT_READ == m_eRead_type || READ_UI_TYPE::TEXT_RIGHT_READ == m_eRead_type)
        Text_Read(fTimeDelta);
}

void CRead_Item_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CRead_Item_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRead_Item_UI::Change_Tool()
{
    return S_OK;
}

void CRead_Item_UI::Render_Destory(_bool _render)
{
   /* if (READ_UI_TYPE::TEXT_READ != m_eRead_type)
        return;*/

    /* 텍스트를 삭제한다*/
    if (true == _render)
    {
        m_isRender = false;
    }

    /* 텍스트를 살린다 */
    else if (false == _render)
    {
        m_isRender = true;
    }
}

void CRead_Item_UI::Introduce_Read(_float fTimeDelta)
{
    if (false == m_isRender || true == m_isRead_Start)
    {
        m_fBlending = 1.f;
        m_vCurrentColor.w = 0.f;
        return;
    }

    m_fIntro_Timer += fTimeDelta;
    m_vCurrentColor.w = 0.f;

    if (INTRO_LIFE <= m_fIntro_Timer)
    {
        m_fBlending += fTimeDelta * 2.f;

        if (m_fBlending >= 1.f)
        {
            m_fBlending = 1.f;
            m_isRead_Start = true;
            m_isRender = false;
        }
    }

    /* FONT */
    if (!m_vecTextBoxes.empty())
    {
        _float4 result = m_fBlending * ALPHA_ZERO + (1 - m_fBlending * 1.5f) * WHITE_COLOR;
        m_vecTextBoxes.back()->Set_FontColor(result);
    }
}

void CRead_Item_UI::Texture_Read()
{
    if (nullptr == m_pIntro_UI)
        return;

    if (true == m_pIntro_UI->m_isRender)
    {
        m_isRender = true;

        if (m_isPrevRender == m_isRender)
            return;

        m_isPrevRender = true;
        vector<wstring> incidentLogNotes = m_pRead_Supervise->m_BookText[m_pIntro_UI->m_eBook_Type];
        Change_Texture(incidentLogNotes[0], TEXT("Com_DefaultTexture"));
    }
}

void CRead_Item_UI::Arrow_Read()
{
    if (nullptr == m_pRead_Supervise)
        m_pRead_Supervise = Find_ReadUI(READ_UI_TYPE::MAIN_READ, false);

    if (nullptr == m_pIntro_UI || nullptr == m_pRead_Supervise)
        return;

    /* 인트로가 읽기 시작을 알렸다면 진행*/
    if (true == m_pIntro_UI->m_isRead_Start)
    {
        if (false == m_isChange)
        {
            m_pRead_Supervise->m_isChange = true;
            m_pRead_Supervise->m_iBookCnt = 1;

            m_isChange = true;
        }

        if (true == IsMouseHover())
        {
            if (DOWN == m_pGameInstance->Get_KeyState(VK_LBUTTON))
            {
                /* 1. 왼쪽일 때 */
                if (READ_ARROW_TYPE::LEFT_ARROW == m_eRead_Arrow_Type)
                {
                    --m_pRead_Supervise->m_iBookCnt;

                    if (1 >= m_pRead_Supervise->m_iBookCnt)
                    {
                        m_pRead_Supervise->m_iBookCnt = 1;
                        m_pRead_Supervise->m_isChange = true;
                    }
                }

                /* 오른쪽일 때 */
                else if (READ_ARROW_TYPE::RIGHT_ARROW == m_eRead_Arrow_Type)
                {
                    ++m_pRead_Supervise->m_iBookCnt;
                    m_pRead_Supervise->m_isChange = true;

                }
            }
        }

        m_isRender = true;
    }
}

void CRead_Item_UI::Start()
{
    if (m_eRead_type == READ_UI_TYPE::INTRODUCE_READ)
    {
        list<class CGameObject*>* pUILayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));

        for (auto& iter : *pUILayer)
        {
            CRead_Item_UI* pRead = dynamic_cast<CRead_Item_UI*>(iter);

            if (nullptr != pRead)
                m_ReadVec.push_back(pRead);
        }
    }
}

void CRead_Item_UI::Set_ReadItem_Type(ITEM_READ_TYPE _readType)
{
    /* intro를 불러야 의미 있음 */
    if (m_eRead_type == READ_UI_TYPE::INTRODUCE_READ)
    {
        if (!m_ReadVec.empty())
        {
            for (auto& iter : m_ReadVec)
            {
                if(READ_UI_TYPE::INTRODUCE_READ == m_eRead_type)
                {
                    iter->m_isReadCall = true;
                    m_eBook_Type = _readType;   
                    return;
                }
            }
        }
    }

}

void CRead_Item_UI::Text_Read(_float fTimeDelta)
{
    if (nullptr == m_pIntro_UI || true != m_pIntro_UI->m_isRead_Start)
        return;

    if (false == m_pRead_Supervise->m_isChange)
        return;

    // Render_Destory(false);

    vector<wstring> incidentLogNotes = m_pRead_Supervise->m_BookText[m_pIntro_UI->m_eBook_Type];

    /* 1은 Texture. Text는 그 아래 숫자로 내려갈 수 없다. */
    if (m_pRead_Supervise->m_iBookCnt <= 0)
        m_pRead_Supervise->m_iBookCnt = 1;

    if (READ_UI_TYPE::TEXT_LEFT_READ == m_eRead_type)
    {
        if (0 < m_pRead_Supervise->m_iBookCnt && incidentLogNotes.size() > m_pRead_Supervise->m_iBookCnt)
            Change_Texture(incidentLogNotes[m_pRead_Supervise->m_iBookCnt], TEXT("Com_DefaultTexture"));

        if ((_int)incidentLogNotes.size() < m_pRead_Supervise->m_iBookCnt)
            m_pRead_Supervise->m_iBookCnt = (_int)incidentLogNotes.size() - 1;

        m_isRender = true;

        m_pRead_Supervise->m_isChange = false;
        //  m_pGameInstance->Change_Sound_2D(CHANNELID::CH30, TEXT("ui_ingame_media.bnk.2_35.mp3"));

    }

    else if (READ_UI_TYPE::TEXT_RIGHT_READ == m_eRead_type)
    {
        //  m_pGameInstance->Change_Sound_2D(CHANNELID::CH30, TEXT("ui_ingame_media.bnk.2_35.mp3"));

        m_isRender = false;
    }
}

CRead_Item_UI* CRead_Item_UI::Find_ReadUI(READ_UI_TYPE _readType, _bool _child)
{
    /* 소개창 찾기 */
    list<class CGameObject*>* pUILayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));
    CRead_Item_UI* pRead_Intro_UI = {};

    for (auto& iter : *pUILayer)
    {
        CRead_Item_UI* pIntro_ReadUI = dynamic_cast<CRead_Item_UI*>(iter);

        if (nullptr != pIntro_ReadUI)
        {
            if (_readType == pIntro_ReadUI->m_eRead_type)
            {
                if (true == _child)
                {
                    if (true == pIntro_ReadUI->m_IsChild)
                        return pIntro_ReadUI;
                }
                else
                    return pIntro_ReadUI;
            }
        }
    }
    return nullptr;
}

void CRead_Item_UI::Render_Condition()
{
    if (false == m_isRender)
    {
        if (nullptr != m_pZoomOff)
            *m_pZoomOff = false;

        return;
    }

    /* R Button 시에 삭제 */
    if (DOWN == m_pGameInstance->Get_KeyState(VK_RBUTTON))
    {
        m_isRender = false;
        m_isPrevRender = false;
        m_isChange = false;

        CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Player"))->front());
        pPlayer->Set_isCamTurn(false);
        m_pGameInstance->Set_IsPaused(false);

        if (nullptr != m_pIntro_UI)
            m_pIntro_UI->m_isRead_Start = false;

        if (nullptr != m_pZoomOff)
            *m_pZoomOff = true;
    }
}

void CRead_Item_UI::Reset()
{
    m_fIntro_Timer = 0.f;
    m_fBlending = MAX_BLENDING;
    m_vCurrentColor = m_vOriginColor;

    if (nullptr != m_pRead_Supervise)
        m_pRead_Supervise->m_iBookCnt = 0;

    if (nullptr != m_pIntro_UI)
        m_pIntro_UI->m_isRead_Start = false;
}

CCustomize_UI* CRead_Item_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRead_Item_UI* pInstance = new CRead_Item_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CRead_Item_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRead_Item_UI::Clone(void* pArg)
{
    CRead_Item_UI* pInstance = new CRead_Item_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CRead_Item_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRead_Item_UI::Free()
{
    __super::Free();

    /*if (nullptr != m_pIntro_UI)
    {
        Safe_Release<CRead_Item_UI*>(m_pIntro_UI);
        m_pIntro_UI = nullptr;
    }

    if (nullptr != m_pTexture_UI)
    {
        Safe_Release<CRead_Item_UI*>(m_pRead_Supervise);
        m_pTexture_UI = nullptr;
    }*/
}
