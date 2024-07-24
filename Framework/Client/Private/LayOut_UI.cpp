#include "stdafx.h"
#include "LayOut_UI.h"
#include "Tab_Window.h"
#include "Camera_Gimmick.h"
#include "InteractProps.h"

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
        
        m_pLayout_BackGround = static_cast<CLayOut_UI*>(CustomUIDesc->pSupervisor);

        if(CustomUIDesc->wstrFileName == TEXT("UI_Layout_BackGround"))
        {
            m_eLayout_Type = LAYOUT_TYPE::LAYOUT_BACKGROUND;

            m_eTabLayout_Type = LAYOUT_TAB_TYPE::BACKGROUND_UI_TYPE;
        }

        else if (CustomUIDesc->wstrFileName == TEXT("UI_Layout_Hint"))
        {
            m_eLayout_Type = LAYOUT_TYPE::LAYOUT_TAB;

            m_eTabLayout_Type = LAYOUT_TAB_TYPE::HINT_UI_TYPE;
        }

        else if (CustomUIDesc->wstrFileName == TEXT("UI_Layout_Inven"))
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

            m_eLayout_Type = LAYOUT_TYPE::LAYOUT_TAB;

            m_eTabLayout_Type = LAYOUT_TAB_TYPE::INVEN_UI_TYPE;
        }

        else if (CustomUIDesc->wstrFileName == TEXT("UI_Layout_Map"))
        {
            m_eLayout_Type = LAYOUT_TYPE::LAYOUT_TAB;

            m_eTabLayout_Type = LAYOUT_TAB_TYPE::MINMAP_UI_TYPE;
        }

        else if (CustomUIDesc->wstrFileName == TEXT("UI_Layout_Key"))
        {
            m_eLayout_Type = LAYOUT_TYPE::LAYOUT_KEY;

            m_eTabLayout_Type = LAYOUT_TAB_TYPE::KEY_UI_TYPE;
        }

        else if (CustomUIDesc->wstrFileName == TEXT("UI_Layout_Statue"))
        {
            m_eLayout_Type = LAYOUT_TYPE::LAYOUT_STATUE;

            m_eTabLayout_Type = LAYOUT_TAB_TYPE::BACKGROUND_UI_TYPE;
        }

    }

    if (LAYOUT_TAB_TYPE::BACKGROUND_UI_TYPE == m_eTabLayout_Type)
    {
        Find_TabWindow();

        m_isMainRender = m_pTab_Window->Get_MainRender_Ptr();
    }

    if (FAILED(Change_Tool()))
        return E_FAIL;

    m_isPrevRender = m_isRender = false;

    m_pLayout_BackGround = static_cast<CLayOut_UI*>(Find_Layout_BackGround());

    return S_OK;
}

void CLayOut_UI::Start()
{
   
}

void CLayOut_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (nullptr == m_isGimmickCamera_Layout_Type)
    {
        Find_GimmickCamera();

        if (nullptr == m_isGimmickCamera_Layout_Type)
        {
            MSG_BOX(TEXT("CLayOut_UI() : Gimmick Camera를 찾을 수 없습니다."));
        }
    }

    if (nullptr == m_pTab_Window && m_eTabLayout_Type != LAYOUT_TAB_TYPE::BACKGROUND_UI_TYPE)
    {
        Find_TabWindow();

        if (nullptr == m_pTab_Window)
        {
            MSG_BOX(TEXT("LayOut에 Render를 구분할 Object(Tab Window)를 불러올 수 없습니다."));
            /* Tab Window 가 nullptr 이 아닌 지 확인해주세요.*/
        }
    }

    if (nullptr == m_pLayout_BackGround)
    {
        m_pLayout_BackGround = static_cast<CLayOut_UI*>(Find_Layout_BackGround());
        if (nullptr == m_pLayout_BackGround)
        {
            MSG_BOX(TEXT("LayOut에 m_pLayout_BackGround 불러올 수 없습니다."));
            /* Tab Window 가 nullptr 이 아닌 지 확인해주세요.*/
        }
    }


    _float4 vObjPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

    vObjPos.z = 0.1f;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vObjPos);

    Layout_Tab();

    Layout_Key();

    Layout_Statue();
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

HRESULT CLayOut_UI::Change_Tool()
{
    if (LAYOUT_TAB_TYPE::BACKGROUND_UI_TYPE == m_eTabLayout_Type)
    {
        m_isMask = m_Mask[0].isMask = true;
        m_fMaskControl = m_Mask[0].fMaskControl = _float2(0.652f, 0.428f);
        m_vColor[0].vColor.w = 0.8f;
    }

    if (!m_vecTextBoxes.empty())
    {
        for (auto& iter : m_vecTextBoxes)
        {
            iter->Set_FontColor(ALPHA_ZERO);
        }
    }

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

             //Safe_AddRef<CTab_Window*>(m_pTab_Window);
        }
    }
}

void CLayOut_UI::Typing_Menu_LayOut()
{
    if (!m_vecTextBoxes.empty())
    {
        for (auto& iter : m_vecTextBoxes)
        {
            switch ((MENU_HOVER_TYPE)m_iMenu_HoverType)
            {
            case MENU_HOVER_TYPE::UNMOUNTED_MENU :
                iter->Set_Text(TEXT("아이템을 사용합니다."));
                break;

            case MENU_HOVER_TYPE::INSPECTION_MENU:
                iter->Set_Text(TEXT("아이템을 검사합니다."));
                break;

            case MENU_HOVER_TYPE::COMBINATION_MENU:
                iter->Set_Text(TEXT("아이템을 조합합니다."));
                break;

            case MENU_HOVER_TYPE::SHORTCUTS_MENU:
                iter->Set_Text(TEXT("아이템을 폐기합니다."));
                break;

            case MENU_HOVER_TYPE::END_MENU:
                iter->Set_Text(TEXT(""));
                break;

            default :
                iter->Set_Text(TEXT("아이템을 사용합니다."));
                break;

            }
        }
    }
}

void CLayOut_UI::Find_GimmickCamera()
{
    CCamera_Gimmick* pCameraGimmick = static_cast<CCamera_Gimmick*>(m_pGameInstance->Get_GameObject(g_Level, g_strCameraTag, 2));

    m_isGimmickCamera_Layout_Type = pCameraGimmick->Get_Layout_Type_Ptr();

    if (nullptr == m_isGimmickCamera_Layout_Type)
    {
        MSG_BOX(TEXT("Gimmick Camera를 찾을 수 없음 "));
    }
}

CGameObject* CLayOut_UI::Find_Layout_BackGround()
{
    list<CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

    for (auto& iter : *pUIList)
    {
        CLayOut_UI* pLayout = dynamic_cast<CLayOut_UI*>(iter);

        if (nullptr != pLayout)
        {
            if (LAYOUT_TYPE::LAYOUT_BACKGROUND == pLayout->m_eLayout_Type)
            {
                return pLayout;
            }
        }
    }

    return nullptr;
}

void CLayOut_UI::Layout_Tab()
{
    if (LAYOUT_TYPE::LAYOUT_TAB == m_eLayout_Type || LAYOUT_TYPE::LAYOUT_BACKGROUND == m_eLayout_Type)
    {
        if (LAYOUT_TAB_TYPE::BACKGROUND_UI_TYPE == m_eTabLayout_Type)
        {
            if (false == (*m_isMainRender))
            {
                _float4 pos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
                
                pos.z = 0.01f;

                m_fBlending = 0.f;

                m_isRender = true;
               
                m_eRenderLayout_Type = (LAYOUT_TAB_TYPE) * (m_pTab_Window->Get_Window_RenderType_Ptr());
            }

            else
            {
                _float4 pos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

                pos.z = 0.f;

                m_pTransformCom->Set_State(CTransform::STATE_POSITION, pos);

                m_fBlending = 0.f;

                m_isRender = false;
                m_eRenderLayout_Type = LAYOUT_TAB_TYPE::END_UI_TYPE;
            }

        }

        else
        {
            /* 내 렌더랑 호출할 렌더랑 같을 때 */
            if (nullptr != m_pLayout_BackGround)
            {
                if (m_eTabLayout_Type == m_pLayout_BackGround->m_eRenderLayout_Type)
                {
                    m_isRender = true;
                }

                else
                {
                    m_isRender = false;
                }
            }

            /* Redner 작동*/
            if (m_isPrevRender != m_isRender)
            {
                if (true == m_isRender)
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
        }

        if (true == m_isInvenCheck_Typing)
        {
            Typing_Menu_LayOut();
        }
    }
}

void CLayOut_UI::Layout_Key()
{
    if (LAYOUT_TYPE::LAYOUT_KEY != m_eLayout_Type || false == *m_pTab_Window->Get_MainRender_Ptr())
        return;

    if (*m_isGimmickCamera_Layout_Type == CInteractProps::INTERACT_GIMMICK_TYPE::KEY_GIMMICK)
    {
        m_isRender = true;

        if (!m_vecTextBoxes.empty())
        {
            for (auto& iter : m_vecTextBoxes)
            {
                iter->Set_FontColor(_float4(1, 1, 1, 1));
            }
        }
    }
    else
    {
        m_isRender = false;

        if (!m_vecTextBoxes.empty())
        {
            for (auto& iter : m_vecTextBoxes)
            {
                iter->Set_FontColor(_float4(0, 0, 0, 0));
            }
        }
    }
}

void CLayOut_UI::Layout_Statue()
{
    if (LAYOUT_TYPE::LAYOUT_STATUE != m_eLayout_Type || false == *m_pTab_Window->Get_MainRender_Ptr())
        return;

    if (*m_isGimmickCamera_Layout_Type == CInteractProps::INTERACT_GIMMICK_TYPE::LOCK_GIMMICK)
    {
        m_isRender = true;

        if (!m_vecTextBoxes.empty())
        {
            for (auto& iter : m_vecTextBoxes)
            {
                iter->Set_FontColor(_float4(1, 1, 1, 1));
            }
        }
    }
    else
    {
        m_isRender = false;

        if (!m_vecTextBoxes.empty())
        {
            for (auto& iter : m_vecTextBoxes)
            {
                iter->Set_FontColor(_float4(0, 0, 0, 0));
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

   
}
