#include "stdafx.h"
#include "Loading_UI.h"

#define     TYPING_RETURN 1.5f
#define     TEXT_LIFE   1.f
#define     ZERO_ALPHA    _float4(0, 0, 0, 0)

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
    /* 
    부모 : BackGround
    자식 : 타자기 BackGround
    의 자식 : 타자기 몸체, 타자기 머리
    의 자식 : 타자기 머리에 텍스쳐
    */
    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;
        m_iWhich_Child = CustomUIDesc->iWhich_Child;
    }

    /* 1. Type 맞추기*/
    if (false == m_IsChild)
        m_eLoadingType = LOADING_UI_TYPE::BACKGROUND_LOADING_UI;

    else if (1 == m_iWhich_Child)
        m_eLoadingType = LOADING_UI_TYPE::TYPING_BACKGROUND_LOADING_UI;

    else if (!m_vecTextBoxes.empty())
        m_eLoadingType = LOADING_UI_TYPE::TYPING_HEAD_LOADING_UI;

    else 
        m_eLoadingType = LOADING_UI_TYPE::TYPING_BODY_LOADING_UI;

    /* 2. 조건 */
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

        /* 텍스트 */
        if (!m_vecTextBoxes.empty())
        {
            for (auto& iter : m_vecTextBoxes)
            {
                m_vOriginTextColor = iter->Get_FontColor();
                iter->Set_FontColor(ZERO_ALPHA);

                
            }

            m_iMaxText = (_int)m_vecTextBoxes.size();
        }
    }

    /* 2. Type에 따른 조건 */
    m_isPlay = false;

    return S_OK;
}

void CLoading_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (LOADING_UI_TYPE::TYPING_HEAD_LOADING_UI == m_eLoadingType)
        Typing_Head_Moving(fTimeDelta);
}   

void CLoading_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CLoading_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
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

void CLoading_UI::Render_Text(_float fTimeDelta)
{
    if (m_vecTextBoxes.empty())
        return;

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
}
