#include "stdafx.h"
#include "Cursor_UI.h"

#define BLUR_SPEED  2.f

CCursor_UI::CCursor_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCustomize_UI{ pDevice, pContext }
{
}

CCursor_UI::CCursor_UI(const CCursor_UI& rhs)
    : CCustomize_UI{ rhs }
{

}

HRESULT CCursor_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCursor_UI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (false == m_IsChild)
    {
        m_eCursor_Type = CURSOR_TYPE::CURSOR_BLUR;
        m_vCurrentColor.w = m_vColor[0].vColor.w = 0.f;
        m_vColor[0].fBlender_Value = m_fBlending = 1.f;
    }

    else if (true == m_IsChild)
    {
        m_eCursor_Type = CURSOR_TYPE::CURSOR;

        CCursor_UI* pBlur = static_cast<CCursor_UI*>(m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"))->back());
        m_pBlurCursor_Trans = static_cast<CTransform*>(pBlur->Get_Component(g_strTransformTag));
        
        m_fBlur_Distance.x = abs(m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).x - m_pBlurCursor_Trans->Get_State_Float4(CTransform::STATE_POSITION).x) + 3.f;
        m_fBlur_Distance.y = abs(m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).y - m_pBlurCursor_Trans->Get_State_Float4(CTransform::STATE_POSITION).y) - 4.f;
        
        if (nullptr == m_pBlurCursor_Trans)
            MSG_BOX(TEXT("CCursor_UI에서 자식이 부모(Blur Cursor)를 제대로 가져오지 못함."));
    }
    /* Z 값 위치*/
    _float4 pTrans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
    pTrans.z = 0.0f;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, pTrans);

    m_isRender = false;
    return S_OK;
}

void CCursor_UI::Tick(_float fTimeDelta)
{
    ShowCursor(FALSE);
    Inven_Open(fTimeDelta);

    __super::Tick(fTimeDelta);
}

void CCursor_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CCursor_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CCursor_UI::Cursor_Blur(_float fTimeDelta)
{
    if (true == m_isCursor_Blur)
    {
        if (m_fBlending >= 1.f)
            m_isCursor_Blur_Blending = false;

        else if (m_fBlending <= 0.f)
            m_isCursor_Blur_Blending = true;

        if (false == m_isCursor_Blur_Blending)
        {
            m_fBlending -= fTimeDelta * BLUR_SPEED;
        }
        else if (true == m_isCursor_Blur_Blending)
        {
            m_fBlending += fTimeDelta * BLUR_SPEED;
        }
    }

    else
    {
        if (m_fBlending >= 1.f)
        {
            m_fBlending = 1.f;
            m_isRender = false;

            return;
        }

        else 
            m_fBlending += fTimeDelta * BLUR_SPEED;
    }
}

void CCursor_UI::Inven_Open(_float fTimeDelta)
{
    if (CURSOR_TYPE::CURSOR == m_eCursor_Type)
    {
         if (false == m_isInven_Open)
         {
             m_isRender = false;
             return;
         }
        
        m_isRender = true;

        POINT ptMouse = m_pGameInstance->Get_MouseCurPos();

        _float4 vMouseCurrentPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

        GetCursorPos(&ptMouse);
        ScreenToClient(g_hWnd, &ptMouse);

        _float4 vMouse = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
        vMouse.x = m_pGameInstance->Get_ViewMousePos().x - (g_iWinSizeX * 0.5f) + 10.f;
        vMouse.y = -ptMouse.y + (g_iWinSizeY * 0.5f) - 10.f;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMouse);

        /* 부모 커서 관련 */
        BlurCursor_Transform();
    }
    
    else if (CURSOR_TYPE::CURSOR_BLUR == m_eCursor_Type)
    {
        m_isRender = true;
        Cursor_Blur(fTimeDelta);
    }
}

void CCursor_UI::BlurCursor_Transform()
{
    _float4 pTrans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
    pTrans.x -= m_fBlur_Distance.x;
    pTrans.y -= m_fBlur_Distance.y;

    m_pBlurCursor_Trans->Set_State(CTransform::STATE_POSITION, pTrans);
}

CCustomize_UI* CCursor_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCursor_UI* pInstance = new CCursor_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CCursor_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCursor_UI::Clone(void* pArg)
{
    CCursor_UI* pInstance = new CCursor_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CCursor_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCursor_UI::Free()
{
    __super::Free();
}
