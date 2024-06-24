#include "stdafx.h"
#include "Map_UI.h"
#include "Read_Item_UI.h"

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
    /*
     1. TEXT("UI_Item_Introduce") => 경관의 수첩 (소개글)
     2. TEXT("UI_Item_Read") => 메인 / 부모 검정색 Background
     3. TEXT("UI_Item_Read_Arrow") => 화살표
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

        else if (CustomUIDesc->wstrFileName == TEXT("UI_Item_Read")) /* 경관의 수첩*/
        {
            m_eRead_type = READ_UI_TYPE::MAIN_READ;
            m_pIntro_UI = Find_ReadUI(READ_UI_TYPE::INTRODUCE_READ, false);
            m_eBook_Type = BOOK_TYPE::OFFICER_NOTE_BOOK;

            if (nullptr != m_pIntro_UI)
                Safe_AddRef<CRead_Item_UI*>(m_pIntro_UI);

            if(true == m_IsChild)
            {
                m_vecBookTexture.resize(2);
                m_vecBookTexture[0].push_back(m_wstrDefaultTexturComTag);
                m_vecBookTexture[1].push_back(TEXT("Prototype_Component_Texture_NoteBook_Texture2"));
            }
        }

        else if (CustomUIDesc->wstrFileName == TEXT("UI_Item_Read_Arrow"))
        {
            m_eRead_type = READ_UI_TYPE::ARROW_READ;
            m_pIntro_UI = Find_ReadUI(READ_UI_TYPE::INTRODUCE_READ, false);
            m_pMain_UI = Find_ReadUI(READ_UI_TYPE::MAIN_READ, true);

            if (nullptr != m_pIntro_UI)
                Safe_AddRef<CRead_Item_UI*>(m_pIntro_UI);

            if (nullptr != m_pMain_UI)
                Safe_AddRef<CRead_Item_UI*>(m_pMain_UI);

            if (nullptr != m_pMain_UI)
            {
                CTransform* pMain_Trans = static_cast<CTransform*>(m_pMain_UI->Get_Component(g_strTransformTag));
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

    m_isRender = false;
    return S_OK;
}

void CRead_Item_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    /* 예시 코드 */
    if (m_pGameInstance->Get_KeyState('I') && READ_UI_TYPE::INTRODUCE_READ == m_eRead_type)
    {
        m_isRender = true;
        Reset();
    }

    Render_Condition();
            
    /* 분기점 */       
    if (READ_UI_TYPE::INTRODUCE_READ == m_eRead_type)
        Introduce_Read(fTimeDelta);

    else if (READ_UI_TYPE::MAIN_READ == m_eRead_type)
        Main_Read();

    else if (READ_UI_TYPE::ARROW_READ == m_eRead_type)
        Arrow_Read();
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

void CRead_Item_UI::Main_Read()
{
    if (nullptr == m_pIntro_UI)
        return;

    if(true == m_IsChild)
    {
        /* 인트로가 읽기 시작을 알렸다면 진행*/
        if (true == m_pIntro_UI->m_isRead_Start)
            m_isRender = true;

        if (m_iBookCnt != m_iBook_PrevCnt)
        {
            Change_Texture(m_vecBookTexture[m_iBookCnt][0], TEXT("Com_DefaultTexture"));
            m_wstrDefaultTexturComTag = m_vecBookTexture[m_iBookCnt][0];

            m_iBook_PrevCnt = m_iBookCnt;
        }
    }
}

void CRead_Item_UI::Arrow_Read()
{
    if (nullptr == m_pIntro_UI || nullptr == m_pMain_UI)
        return;
        
    /* 인트로가 읽기 시작을 알렸다면 진행*/
    if (true == m_pIntro_UI->m_isRead_Start)
    {
        if (true == IsMouseHover())
        {
            if (DOWN == m_pGameInstance->Get_KeyState(VK_LBUTTON))
            {
                /* 1. 왼쪽일 때 */
                if (READ_ARROW_TYPE::LEFT_ARROW == m_eRead_Arrow_Type)
                {
                    --m_pMain_UI->m_iBookCnt;

                    if (0 >= m_pMain_UI->m_iBookCnt)
                        m_pMain_UI->m_iBookCnt = 0;
                }

                /* 오른쪽일 때 */
                else if (READ_ARROW_TYPE::RIGHT_ARROW == m_eRead_Arrow_Type)
                {
                    ++m_pMain_UI->m_iBookCnt;

                    if (m_pMain_UI->m_vecBookTexture.size() <= m_pMain_UI->m_iBookCnt)
                        m_pMain_UI->m_iBookCnt = (_int)m_pMain_UI->m_vecBookTexture.size() - 1;
                }
            }
        }
        m_isRender = true;
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
   
    if(nullptr != m_pMain_UI)
        m_pMain_UI->m_iBookCnt = 0;

    if (nullptr != m_pIntro_UI)
        m_pIntro_UI->m_isRead_Start = false;

    //if(!m_vecTextBoxes.empty())
     //   m_vecTextBoxes.back()->Set_Position(m_vFont_Position.x - 200.f, m_vFont_Position.y, m_vFont_Position.z);
}

CCustomize_UI * CRead_Item_UI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
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
        Safe_Release(m_pIntro_UI);

    if(nullptr != m_pMain_UI)
        Safe_Release(m_pMain_UI);
}
