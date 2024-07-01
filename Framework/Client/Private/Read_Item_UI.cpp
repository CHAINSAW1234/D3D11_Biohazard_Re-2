#include "stdafx.h"
#include "Map_UI.h"
#include "Read_Item_UI.h"

#define ARROW_DISTANCE  30.f
#define INTRO_LIFE      1.5f
#define ALPHA_ZERO      _float4(0, 0, 0, 0)
#define WHITE_COLOR     _float4(1, 1, 1, 0)
#define MAX_BLENDING    0.7f

CRead_Item_UI::CRead_Item_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CInteract_UI{ pDevice, pContext }
{
}

CRead_Item_UI::CRead_Item_UI(const CRead_Item_UI& rhs)
    : CInteract_UI{ rhs }
{

}

HRESULT CRead_Item_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRead_Item_UI::Initialize(void* pArg)
{
    /*
     1. TEXT("UI_Item_Introduce") => ����� ��ø (�Ұ���)
     2. TEXT("UI_Item_Read") => ���� / �θ� ������ Background
     3. TEXT("UI_Item_Read_Arrow") => ȭ��ǥ
    */

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
               // m_vFont_Position = m_vecTextBoxes.back()->GetPosition();
               // m_vecTextBoxes.back()->Set_Position(m_vFont_Position.x - 200.f, m_vFont_Position.y, m_vFont_Position.z);
            }
        }

        else if (CustomUIDesc->wstrFileName == TEXT("UI_Item_Read"))
        {
            /* Intro �θ� */
            m_pIntro_UI = Find_ReadUI(READ_UI_TYPE::INTRODUCE_READ, false);

            if (nullptr != m_pIntro_UI)
            {
                Safe_AddRef<CRead_Item_UI*>(m_pIntro_UI);
            }

            if(true == m_IsChild)
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

                else if(2 == CustomUIDesc->iWhich_Child)
                    m_eRead_type = READ_UI_TYPE::TEXT_RIGHT_READ;

                m_pRead_Supervise = static_cast<CRead_Item_UI*>(CustomUIDesc->pSupervisor);
            }

            else
                m_eRead_type = READ_UI_TYPE::MAIN_READ; /* Read�� BackGround*/
        }

        else if (CustomUIDesc->wstrFileName == TEXT("UI_Item_Read_Arrow"))
        {
            m_eRead_type = READ_UI_TYPE::ARROW_READ;
            m_pIntro_UI = Find_ReadUI(READ_UI_TYPE::INTRODUCE_READ, false);
            m_pRead_Supervise = Find_ReadUI(READ_UI_TYPE::MAIN_READ, false);
            CRead_Item_UI* pTexture_UI = Find_ReadUI(READ_UI_TYPE::TEXTURE_READ, true);

           if (nullptr != m_pIntro_UI)
                Safe_AddRef<CRead_Item_UI*>(m_pIntro_UI);

            if (nullptr != m_pTexture_UI)
                Safe_AddRef<CRead_Item_UI*>(m_pRead_Supervise);

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
                MSG_BOX(TEXT("Read Item UI ��� ���ӿ��� [ Arrow ] �� �������� �ʾҽ��ϴ�"));
        }
    }

    if (READ_UI_TYPE::MAIN_READ == m_eRead_type)
    {
        /* ù ��°�� ����, �� ��°�� TEXT */

        m_BookText[ITEM_READ_TYPE::INCIDENT_LOG_NOTE] = { TEXT("Prototype_Component_Texture_Document2"),  TEXT("Prototype_Component_Texture_ReadType_Incident_log1"), TEXT("Prototype_Component_Texture_ReadType_Incident_log2"), TEXT("Prototype_Component_Texture_ReadType_Incident_log3"), TEXT("Prototype_Component_Texture_ReadType_Incident_log4"), TEXT("Prototype_Component_Texture_ReadType_Incident_log5") };
        m_BookText[ITEM_READ_TYPE::OPERATE_REPORT_NOTE] = { TEXT("Prototype_Component_Texture_Document2"), TEXT("Prototype_Component_Texture_Operate_Report2"), TEXT("Prototype_Component_Texture_Operate_Report3") };
        m_BookText[ITEM_READ_TYPE::TASK_NOTE] = { TEXT("Prototype_Component_Texture_DocumentBlood1"), TEXT("Prototype_Component_Texture_Task1"), TEXT("Prototype_Component_Texture_Task2"), TEXT("Prototype_Component_Texture_Task3"), TEXT("Prototype_Component_Texture_Task4"), TEXT("Prototype_Component_Texture_Task5") };
        m_BookText[ITEM_READ_TYPE::MEDICINAL_NOTE] = { TEXT("Prototype_Component_Texture_Document1"), TEXT("Prototype_Component_Texture_Medicinal1"), TEXT("Prototype_Component_Texture_Medicinal2"), TEXT("Prototype_Component_Texture_Medicinal3"), TEXT("Prototype_Component_Texture_Medicinal4") };
    }

    m_isRender = false;
    return S_OK;
}

void CRead_Item_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    /* ���� �ڵ� */
    if (m_pGameInstance->Get_KeyState('I') && READ_UI_TYPE::INTRODUCE_READ == m_eRead_type)
    {
        m_isRender = true;
        m_eBook_Type = eGara;
        Reset();
    }

    /* GARA */
    if (DOWN == m_pGameInstance->Get_KeyState('0') && READ_UI_TYPE::INTRODUCE_READ == m_eRead_type)
    {
        eGara = (ITEM_READ_TYPE)((_int)eGara + 1);

        if (eGara >= ITEM_READ_TYPE::END_NOTE)
            eGara = ITEM_READ_TYPE::INCIDENT_LOG_NOTE;
    }



    Render_Condition();
            
    /* �б��� */       
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
    if (READ_UI_TYPE::TEXT_RIGHT_READ == m_eRead_type)
        int a = 1;
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CRead_Item_UI::Render_Destory(_bool _render)
{
    //if (READ_UI_TYPE::TEXT_READ != m_eRead_type)
    //    return;

    /* �ؽ�Ʈ�� �����Ѵ�*/
    if (true == _render)
    {
        m_isRender = false;
    }

    /* �ؽ�Ʈ�� �츰�� */
    else if (false == _render)
    {
        m_isRender = true;
    }
}

void CRead_Item_UI::Introduce_Read(_float fTimeDelta)
{
    if (false == m_isRender || true == m_isRead_Start)
        return;

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

    if (true == m_pIntro_UI->m_isRead_Start)
    {
        m_isRender = true;
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

    /* ��Ʈ�ΰ� �б� ������ �˷ȴٸ� ����*/
    if (true == m_pIntro_UI->m_isRead_Start)
    {
        if (true == IsMouseHover())
        {
            if (DOWN == m_pGameInstance->Get_KeyState(VK_LBUTTON))
            {
                /* 1. ������ �� */
                if (READ_ARROW_TYPE::LEFT_ARROW == m_eRead_Arrow_Type)
                {
                    --m_pRead_Supervise->m_iBookCnt;

                    if (1 >= m_pRead_Supervise->m_iBookCnt)
                        m_pRead_Supervise->m_iBookCnt = 1;
                }

                /* �������� �� */
                else if (READ_ARROW_TYPE::RIGHT_ARROW == m_eRead_Arrow_Type)
                {
                    ++m_pRead_Supervise->m_iBookCnt;
                }
            }
        }

        m_isRender = true;
    }
}

void CRead_Item_UI::Text_Read(_float fTimeDelta)
{
    if (nullptr == m_pIntro_UI || true != m_pIntro_UI->m_isRead_Start)
        return;

   // Render_Destory(false);
    
    vector<wstring> incidentLogNotes = m_pRead_Supervise->m_BookText[m_pIntro_UI->m_eBook_Type];

    /* 1�� Texture. Text�� �� �Ʒ� ���ڷ� ������ �� ����. */
    if (m_pRead_Supervise->m_iBookCnt <= 0)
        m_pRead_Supervise->m_iBookCnt = 1;

    if (READ_UI_TYPE::TEXT_LEFT_READ == m_eRead_type)
    {
        if(0 < m_pRead_Supervise->m_iBookCnt && incidentLogNotes.size() > m_pRead_Supervise->m_iBookCnt)
            Change_Texture(incidentLogNotes[m_pRead_Supervise->m_iBookCnt], TEXT("Com_DefaultTexture"));

        if ((_int)incidentLogNotes.size() < m_pRead_Supervise->m_iBookCnt)
            m_pRead_Supervise->m_iBookCnt = (_int)incidentLogNotes.size() - 1;

        m_isRender = true;
    }

    else if (READ_UI_TYPE::TEXT_RIGHT_READ == m_eRead_type)
    {
       /* if(m_pRead_Supervise->m_iBookCnt+1 < incidentLogNotes.size())
            Change_Texture(incidentLogNotes[m_pRead_Supervise->m_iBookCnt + 1], TEXT("Com_DefaultTexture"));
        else
            m_pRead_Supervise->m_iBookCnt
        m_isRender = true;*/
    }
}

CRead_Item_UI* CRead_Item_UI::Find_ReadUI(READ_UI_TYPE _readType, _bool _child)
{
    /* �Ұ�â ã�� */
    list<class CGameObject*>* pUILayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));
    CRead_Item_UI* pRead_Intro_UI = {};

    for (auto& iter : *pUILayer)
    {
        CRead_Item_UI* pIntro_ReadUI = dynamic_cast<CRead_Item_UI*>(iter);

        if (nullptr != pIntro_ReadUI)
        {
            if (_readType == pIntro_ReadUI->m_eRead_type)
            {
                if(true == _child)
                {
                    if(true == pIntro_ReadUI->m_IsChild)
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
        return;  

    if (DOWN == m_pGameInstance->Get_KeyState(VK_RBUTTON) && true == m_isRender)
    {
        m_isRender = false;

        if(nullptr != m_pIntro_UI)
            m_pIntro_UI->m_isRead_Start = false;
    }
}

void CRead_Item_UI::Reset()
{
    m_fIntro_Timer = 0.f;
    m_fBlending = MAX_BLENDING;
    m_vCurrentColor = m_vOriginColor;
   
    if(nullptr != m_pRead_Supervise)
        m_pRead_Supervise->m_iBookCnt = 0;

    if (nullptr != m_pIntro_UI)
        m_pIntro_UI->m_isRead_Start = false;

    //if(!m_vecTextBoxes.empty())
     //   m_vecTextBoxes.back()->Set_Position(m_vFont_Position.x - 200.f, m_vFont_Position.y, m_vFont_Position.z);
}

CInteract_UI* CRead_Item_UI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
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

    if (nullptr != m_pIntro_UI)
    {
        Safe_Release<CRead_Item_UI*>(m_pIntro_UI);
        m_pIntro_UI = nullptr;
    }

    if (nullptr != m_pTexture_UI)
    {
        Safe_Release<CRead_Item_UI*>(m_pRead_Supervise);
        m_pTexture_UI = nullptr;
    }
}
