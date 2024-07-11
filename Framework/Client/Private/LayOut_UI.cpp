#include "stdafx.h"
#include "LayOut_UI.h"
#include "Tab_Window.h"

#define WHITE_COLOR     _float4(1, 1, 1, 1)
#define ALPHA_ZERO      _float4(0, 0, 0, 0)

CLayOut_UI::CLayOut_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCustomize_UI{ pDevice, pContext }
{
}

CLayOut_UI::CLayOut_UI(const CLayOut_UI& rhs)
    : CCustomize_UI{ rhs }
{

}

HRESULT CLayOut_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLayOut_UI::Initialize(void* pArg)
{
    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;
        
        m_iWhich_Child = CustomUIDesc->iWhich_Child;
        m_pLayout_BackGround = static_cast<CLayOut_UI*>(CustomUIDesc->pSupervisor);

        if (0 == m_iWhich_Child)
        {
            m_eLayout_Type = LAYOUT_UI_TYPE::BACKGROUND_UI_TYPE;
        }

        else if (1 == m_iWhich_Child)
        {
            list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));
            _int iLastType = { -1 };
            _int iMaxSize = { -1 };

            for (auto& iter : *pUIList)
            {
                CLayOut_UI* pLayout = dynamic_cast<CLayOut_UI*>(iter);

                if (nullptr != pLayout)
                {
                    if((_int)pLayout->m_eLayout_Type <= (_int)LAYOUT_UI_TYPE::HINT_UI_TYPE)
                    {
                        if(iMaxSize < (_int)pLayout->m_eLayout_Type)
                        {
                            iLastType = (_int)pLayout->m_eLayout_Type;
                        }
                    }
                }
            }

            if (iLastType == -1)
                m_eLayout_Type = LAYOUT_UI_TYPE::MINMAP_UI_TYPE;

            else if (iLastType == (_int)LAYOUT_UI_TYPE::MINMAP_UI_TYPE)
                m_eLayout_Type = LAYOUT_UI_TYPE::INVEN_UI_TYPE;

            else if (iLastType == (_int)LAYOUT_UI_TYPE::INVEN_UI_TYPE)
                m_eLayout_Type = LAYOUT_UI_TYPE::HINT_UI_TYPE;
        }
        
        else
        {
            list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

            if (nullptr != pUIList)
            {
                CLayOut_UI* pLayout = static_cast<CLayOut_UI*>(pUIList->back());

                if (pLayout->m_eLayout_Type == LAYOUT_UI_TYPE::MINMAP_UI_TYPE)
                    m_eLayout_Type = LAYOUT_UI_TYPE::MINMAP_UI_TYPE;

                else if (pLayout->m_eLayout_Type == LAYOUT_UI_TYPE::INVEN_UI_TYPE)
                    m_eLayout_Type = LAYOUT_UI_TYPE::INVEN_UI_TYPE;

                else if (pLayout->m_eLayout_Type == LAYOUT_UI_TYPE::HINT_UI_TYPE)
                    m_eLayout_Type = LAYOUT_UI_TYPE::HINT_UI_TYPE;
            }
        }
    }

    if (LAYOUT_UI_TYPE::BACKGROUND_UI_TYPE == m_eLayout_Type)
    {
        m_isMask = m_Mask[0].isMask = true;
        m_fMaskControl = m_Mask[0].fMaskControl = _float2(0.652f, 0.428f);
        m_vColor[0].vColor.w = 0.8f;

        Find_TabWindow();
        m_isMainRender = m_pTab_Window->Get_MainRender_Ptr();

    }

    if (!m_vecTextBoxes.empty())
    {
        for (auto& iter : m_vecTextBoxes)
        {
            iter->Set_FontColor(ALPHA_ZERO);
        }
    }

    if (m_eLayout_Type == LAYOUT_UI_TYPE::INVEN_UI_TYPE)
    {
        if (!m_vecTextBoxes.empty())
        {
            for (auto& iter : m_vecTextBoxes)
            {
                if (iter->Get_Text() == TEXT("아이템을 검사합니다"))
                {
                    CTransform* pTextTrans = static_cast<CTransform*>(iter->Get_Component(g_strTransformTag));
                    
                    m_vOriginTextPos = pTextTrans->Get_State_Float4(CTransform::STATE_POSITION);

                    m_isInvenCheck_Typing = true;

                    break;
                }
            }
        }
    }

    _float4 vObjPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

    vObjPos.z = 0.01f;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vObjPos);

    m_isPrevRender = m_isRender = false;

    return S_OK;
}

void CLayOut_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (LAYOUT_UI_TYPE::BACKGROUND_UI_TYPE == m_eLayout_Type)
    {
        if (nullptr == m_pTab_Window)
        {
            Find_TabWindow();

            if (nullptr == m_pTab_Window)
            {
                MSG_BOX(TEXT("LayOut에 Render를 구분할 Object(Tab Window)를 불러올 수 없습니다."));
                /* Tab Window 가 nullptr 이 아닌 지 확인해주세요.*/
            }
        }

        else
        {
            if (false == (*m_isMainRender))
            {
                m_isRender = true;
                m_eRenderLayout_Type = (LAYOUT_UI_TYPE) *(m_pTab_Window->Get_Window_RenderType_Ptr());
            }

            else
            {
                m_isRender = false;
                m_eRenderLayout_Type = LAYOUT_UI_TYPE::END_UI_TYPE;
            }
        }
    }
  
    /* 내 렌더랑 호출할 렌더랑 같을 때 */
    if(nullptr != m_pLayout_BackGround)
    {
        if (m_eLayout_Type == m_pLayout_BackGround->m_eRenderLayout_Type)
        {
            m_isRender = true;
        }

        else
        {
            m_isRender = false;
        }
    }

    /* Redner 작동*/
    if(m_isPrevRender != m_isRender)
    {
        if(true == m_isRender)
        {
            if (!m_vecTextBoxes.empty())
            {
                for (auto& iter : m_vecTextBoxes)
                    iter->Set_FontColor(WHITE_COLOR);
            }
        }

        else if (false == m_isRender)
        {
            if (!m_vecTextBoxes.empty())
            {
                for (auto& iter : m_vecTextBoxes)
                    iter->Set_FontColor(ALPHA_ZERO);
            }
        }

        m_isPrevRender = m_isRender;
    }

    if (true == m_isInvenCheck_Typing)
    {
        Typing_Menu_LayOut();
    }
}   

void CLayOut_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CLayOut_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CLayOut_UI::Find_TabWindow()
{
    list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_TabWindow"));

    for (auto& iter : *pUIList)
    {
        CTab_Window* pTabWin = dynamic_cast<CTab_Window*>(iter);

        if (nullptr != pTabWin)
        {
            m_pTab_Window = pTabWin;

          //  Safe_AddRef<CTab_Window*>(m_pTab_Window);
        }
    }
}

void CLayOut_UI::Typing_Menu_LayOut()
{
    if (!m_vecTextBoxes.empty())
    {

        for (auto& iter : m_vecTextBoxes)
        {
            _float4 originPos = m_vOriginTextPos;

            switch ((MENU_HOVER_TYPE)m_iMenu_HoverType)
            {
            case MENU_HOVER_TYPE::UNMOUNTED_MENU :
                iter->Set_Text(TEXT("장착된 무기를 해제합니다."));
                originPos.x -= 20.f;
                iter->Set_Position(originPos);
                break;

            case MENU_HOVER_TYPE::INSPECTION_MENU:
                iter->Set_Position(originPos);
                iter->Set_Text(TEXT("아이템을 검사합니다."));
                break;

            case MENU_HOVER_TYPE::COMBINATION_MENU:
                iter->Set_Position(originPos);
                iter->Set_Text(TEXT("아이템을 조합합니다."));
                break;

            case MENU_HOVER_TYPE::SHORTCUTS_MENU:
                iter->Set_Position(originPos);
                iter->Set_Text(TEXT("단축키를 등록합니다."));
                break;

            case MENU_HOVER_TYPE::END_MENU:
                iter->Set_Text(TEXT(""));
                break;

            }
        }
    }
}

CCustomize_UI* CLayOut_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLayOut_UI* pInstance = new CLayOut_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CLayOut_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLayOut_UI::Clone(void* pArg)
{
    CLayOut_UI* pInstance = new CLayOut_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CLayOut_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLayOut_UI::Free()
{
    __super::Free();

    //if(nullptr != m_pTab_Window)
    //{
    //    Safe_Release<CTab_Window*>(m_pTab_Window);
    //    m_pTab_Window = nullptr;
    //}
}
