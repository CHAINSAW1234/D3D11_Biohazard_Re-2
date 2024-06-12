#include "stdafx.h"
#include "Bullet_UI.h"
#define MAX_BULLET 12
#define MAX_BULLET_COLOR _float4(0.8510, 0.8902, 0.8471, 0.f)
#define TEN 10

CBullet_UI::CBullet_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCustomize_UI{ pDevice, pContext }
{
}

CBullet_UI::CBullet_UI(const CBullet_UI& rhs)
    : CCustomize_UI{ rhs }
{

}

HRESULT CBullet_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBullet_UI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (true == m_IsChild)
    {
         for (auto& iter : m_vecTextBoxes)
         {
             CTransform* pTextTrans = dynamic_cast<CTransform*>(iter->Get_Component(g_strTransformTag));

             if (m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).x > pTextTrans->Get_State_Float4(CTransform::STATE_POSITION).x)
             {
                 m_pTextUI[(_int)BULLET_TEXT_TYPE::CURRENT_BULLET].pText = iter;
                 m_pTextUI[(_int)BULLET_TEXT_TYPE::CURRENT_BULLET].pText->Set_Text(TEXT("0"));

                 m_fFull_CurrentBullet_Transform = pTextTrans->Get_State_Float4(CTransform::STATE_POSITION);
                 m_fFull_CurrentBullet_Transform.x -= 7.f;
                 m_fOrigin_TextColor = m_vColor[0].vColor;
             }

             else
             {
                 m_pTextUI[(_int)BULLET_TEXT_TYPE::STORE_BULLET].pText = iter;
                 m_pTextUI[(_int)BULLET_TEXT_TYPE::STORE_BULLET].pText->Set_Text(TEXT("0"));

                 m_fFull_StoreBullet_Transform = pTextTrans->Get_State_Float4(CTransform::STATE_POSITION);
                 m_fFull_StoreBullet_Transform.x += 7.f;
                 m_fOrigin_TextColor = m_vColor[0].vColor;
             }
         }
    }

    
    m_isLightMask = false;
    m_isMask = true;
    m_fMaskControl.x = 0.3f;
    m_fMaskControl.y = 0.4f;
    return S_OK;
}

void CBullet_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (true == m_IsChild)
    {
        Control_BulletU();
        Change_BulletUI();
    }
}

void CBullet_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CBullet_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK; 
}

void CBullet_UI::Control_BulletU()
{
    /* Test Code */
    if (DOWN == m_pGameInstance->Get_KeyState('O'))
    {
        if (nullptr != m_pTextUI[0].pText)
        {
            m_iCurrentBullet++;
            m_pTextUI[(_int)BULLET_TEXT_TYPE::CURRENT_BULLET].pText->Set_Text(to_wstring(m_iCurrentBullet));
        }
    }

    if (DOWN == m_pGameInstance->Get_KeyState('P'))
    {
        if (nullptr != m_pTextUI[1].pText)
        {
            m_iStoreBullet++;
            m_pTextUI[(_int)BULLET_TEXT_TYPE::STORE_BULLET].pText->Set_Text(to_wstring(m_iStoreBullet));
        }
    }

}

void CBullet_UI::Change_BulletUI()
{
    /* 10의 자리 숫자시 살짝 위치 옮겨줌*/
    if (10 <= m_iCurrentBullet)
    {
        CTransform* pTextTrans = dynamic_cast<CTransform*>(m_pTextUI[0].pText->Get_Component(g_strTransformTag));
        pTextTrans->Set_State(CTransform::STATE_POSITION, m_fFull_CurrentBullet_Transform);
    }

    if (10 <= m_iStoreBullet)
    {
        CTransform* pTextTrans = dynamic_cast<CTransform*>(m_pTextUI[1].pText->Get_Component(g_strTransformTag));
        pTextTrans->Set_State(CTransform::STATE_POSITION, m_fFull_StoreBullet_Transform);
    }
    

    /* 12자리 일 때 색깔 바뀌게 함*/
    if (MAX_BULLET <= m_iCurrentBullet)
    {
        if (nullptr != m_pTextUI[0].pText && false == m_pTextUI[0].isFull)
        {
            m_pTextUI[0].pText->Set_FontColor(MAX_BULLET_COLOR);
            m_pTextUI[0].isFull = true;
        }
    }
    else
    {
        if (true == m_pTextUI[0].isFull)
        {
            m_pTextUI[0].isFull = false;
            m_pTextUI[0].pText->Set_FontColor(m_fOrigin_TextColor);
        }
    }   

    if (MAX_BULLET <= m_iStoreBullet)
    {
        if (nullptr != m_pTextUI[1].pText && false == m_pTextUI[1].isFull)
        {
            m_pTextUI[1].pText->Set_FontColor(MAX_BULLET_COLOR);
            m_pTextUI[1].isFull = true;
        }
    }
    else
    {
        if (true == m_pTextUI[1].isFull)
        {
            m_pTextUI[1].isFull = false;
            m_pTextUI[1].pText->Set_FontColor(m_fOrigin_TextColor);
        }
    }
}

CCustomize_UI* CBullet_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBullet_UI* pInstance = new CBullet_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CBullet_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBullet_UI::Clone(void* pArg)
{
    CBullet_UI* pInstance = new CBullet_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CBullet_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBullet_UI::Free()
{
    __super::Free();
}
