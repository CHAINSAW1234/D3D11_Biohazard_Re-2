#include "stdafx.h"
#include "Loading_UI.h"

#define     TYPING_RETURN 1.5f
#define     TEXT_LIFE   1.f
#define     ZERO_ALPHA    _float4(0, 0, 0, 0)
#define     BLACK_COLOR    _float4(0, 0, 0, 1);

CLoading_UI::CLoading_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCustomize_UI{ pDevice, pContext }
{
}

CLoading_UI::CLoading_UI(const CLoading_UI& rhs)
    : CCustomize_UI{ rhs }
{

}

HRESULT CLoading_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CLoading_UI::Initialize(void* pArg)
{
    wstring fileName = {};

    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;

        m_iWhich_Child = CustomUIDesc->iWhich_Child;

        fileName = CustomUIDesc->wstrFileName;
    }
    
    if (fileName == TEXT("UI_Loading_Text"))
    {
        m_eLoadingType = LOADING_UI_TYPE::BACKGROUND_TEXT;
    }

    else if (fileName == TEXT("UI_Loading_bar"))
    {
        m_eLoadingType = LOADING_UI_TYPE::BAR_LOADING_UI;
    }

    else
    {
        if (false == m_IsChild)
            m_eLoadingType = LOADING_UI_TYPE::BACKGROUND_LOADING_UI;

        else if (1 == m_iWhich_Child)
            m_eLoadingType = LOADING_UI_TYPE::TYPING_BACKGROUND_LOADING_UI;

        else if (!m_vecTextBoxes.empty())
            m_eLoadingType = LOADING_UI_TYPE::TYPING_HEAD_LOADING_UI;

        else
            m_eLoadingType = LOADING_UI_TYPE::TYPING_BODY_LOADING_UI;
    }


    /* 2. 조건 */
    if (LOADING_UI_TYPE::TYPING_HEAD_LOADING_UI == m_eLoadingType)
    {
        /* 텍스트 */
        if (!m_vecTextBoxes.empty())
        {
            for (auto& iter : m_vecTextBoxes)
            {
                m_vOriginTextColor = iter->Get_FontColor();

                iter->Set_FontColor(ZERO_ALPHA);
            }

            m_iMaxText = (_int)m_vecTextBoxes.size();

            Find_Loading_BackGround();
        }
    }

    if (!m_vecTextBoxes.empty())
    {
        if (TEXT("100") == m_vecTextBoxes.back()->Get_Text())
        {
            m_isPercent = true;
        }
    }

    if (FAILED(Change_Tool()))
        return E_FAIL;

    m_isPlay = false;

    return S_OK;
}

void CLoading_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);
  
    if(false == m_isReady)
    {
        m_isReady = true;

        if (LOADING_UI_TYPE::TYPING_HEAD_LOADING_UI == m_eLoadingType)
        {
            m_vOrigin_Position = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

            list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

            /* 목표 위치 잡기 */
            for (auto& iter : *pUIList)
            {
                CLoading_UI* pBody_UI = dynamic_cast<CLoading_UI*>(iter);

                if (nullptr != pBody_UI)
                {
                    if (LOADING_UI_TYPE::TYPING_BODY_LOADING_UI == pBody_UI->m_eLoadingType)
                    {
                        CTransform* pBodyTrans = static_cast<CTransform*>(pBody_UI->Get_Component(g_strTransformTag));

                        m_fBody_Position = pBodyTrans->Get_State_Float4(CTransform::STATE_POSITION);

                        break;
                    }
                }
            }
        }
    }

    Operate_Typing(fTimeDelta);

    Operate_Ending(fTimeDelta);
}   

void CLoading_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

    Render();
}

HRESULT CLoading_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CLoading_UI::Change_Tool()
{
    if (LOADING_UI_TYPE::BAR_LOADING_UI == m_eLoadingType)
    {
        _float4 fPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
       
        fPos.x += 2.5f;
        fPos.y -= 4.f;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, fPos);

        m_SavePos[0]._41 = fPos.x;
        m_SavePos[0]._42 = fPos.y;
        m_SavePos[0]._43 = fPos.z;
        m_SavePos[0]._44 = fPos.w;
    }

    return S_OK;
}

void CLoading_UI::Operate_Typing(_float fTimeDelta)
{
    if (LOADING_UI_TYPE::BACKGROUND_TEXT == m_eLoadingType)
    {
        if(nullptr == m_pBackGroundUI)
        {
            Find_Loading_BackGround();

            if (nullptr == m_pBackGroundUI)
                MSG_BOX(TEXT("Loading_UI에서 Loading BackGround를 찾을 수 없습니다."));
        }
    }

    else if (LOADING_UI_TYPE::TYPING_HEAD_LOADING_UI == m_eLoadingType)
    {
        Typing_Head_Moving(fTimeDelta);
        //Typing_Rolling_Moving(fTimeDelta);
    }
}

void CLoading_UI::Typing_Head_Moving(_float fTimeDelta)
{
    m_fLoading_Timer += fTimeDelta;

    if (m_fLoading_Timer <= TYPING_RETURN)
    {
        _float4 vPosition = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
        
        vPosition.x += 2.f;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

        if (m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).x >= m_fBody_Position.x)
        {
            vPosition.x = m_fBody_Position.x;
           
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
        }
    }

    else
    {
        _float4 vPosition = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
       
        vPosition.x -= 1.4f;
        
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

        if (m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).x <= m_vOrigin_Position.x)
        {
            m_fLoading_Timer = 0.f;
            
            vPosition.x = m_vOrigin_Position.x;
           
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
        }
    }

    Render_Text(fTimeDelta);
}

void CLoading_UI::Typing_Rolling_Moving(_float fTimeDelta)
{
    if (m_eLoadingType == LOADING_UI_TYPE::ROLLING_LOADING_UI) 
    {
        m_pTransformCom->Turn(_float4(0, 0, 1, 0), fTimeDelta);
    }
}

void CLoading_UI::Render_Text(_float fTimeDelta)
{
    if (m_vecTextBoxes.empty())
        return;

    if (true == m_isPercent)
    {
        m_vecTextBoxes.back()->Set_Text(to_wstring(*m_pPercent));
    }
    
    if (m_iTextCnt >= m_iMaxText)
        m_iTextCnt = m_iMaxText - 1;
    
    m_fText_Timer += fTimeDelta;

    if (m_fText_Timer >= TEXT_LIFE)
    {
        m_vecTextBoxes[m_iTextCnt]->Set_FontColor(m_vOriginTextColor);
        
        m_iTextCnt++;
        
        m_fText_Timer = 0.f;
    }
}

void CLoading_UI::Render_Ending(float fTimeDelta)
{
    if (m_fBlending >= 1.f)
    {
        m_vCurrentColor = _float4(1, 1, 1, 0);
        m_fBlending = 1.f;
        return;
    }
   
    if (!m_vecTextBoxes.empty())
    {
        _float4 result = m_pBackGroundUI->m_fBlending * ZERO_ALPHA + (1 - m_pBackGroundUI->m_fBlending * 1.5f) * _float4(0, 0, 0, 0);

        for (auto& iter : m_vecTextBoxes)
        {
            iter->Set_FontColor(result);
        }
    }

    else
    {
        m_vCurrentColor = m_vColor[0].vColor = _float4(0, 0, 0, 1);

        m_fBlending += fTimeDelta;
    }
}

void CLoading_UI::Operate_Ending(_float fTimeDelta)
{
    if (true == m_isLoading_Notification)
    {
        Ending_Notification();
    }

    if (true == m_isLoadingEnd)
    {
        Render_Ending(fTimeDelta);
    }

    if (false == m_isRender)
    {
        if (!m_vecTextBoxes.empty())
        {
            for (auto& iter : m_vecTextBoxes)
            {
                iter->Set_FontColor(_float4(0.f, 0.f, 0.f, 0.f));
            }
        }

    }
}

void CLoading_UI::Ending_Notification()
{
    list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

    for (auto& iter : *pUIList)
    {
        CLoading_UI* pLoadingList = static_cast<CLoading_UI*>(iter);

        pLoadingList->m_isLoadingEnd = true;
    }

    m_isLoading_Notification = false;
}

void CLoading_UI::Find_Loading_BackGround()
{
    list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

    for (auto& iter : *pUIList)
    {
        CLoading_UI* pLoadingList = static_cast<CLoading_UI*>(iter);

        if (LOADING_UI_TYPE::BACKGROUND_LOADING_UI == pLoadingList->m_eLoadingType)
        {
            m_pBackGroundUI = pLoadingList;

           // Safe_AddRef<CLoading_UI*>(m_pBackGroundUI);
            return;
        }
    }

}

CCustomize_UI* CLoading_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLoading_UI* pInstance = new CLoading_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CLoading_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLoading_UI::Clone(void* pArg)
{
    CLoading_UI* pInstance = new CLoading_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CLoading_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLoading_UI::Free()
{
    __super::Free();

   /* if (nullptr != m_pBackGroundUI)
    {
        Safe_Release<CLoading_UI*>(m_pBackGroundUI);
        m_pBackGroundUI = nullptr;
    }*/
}
