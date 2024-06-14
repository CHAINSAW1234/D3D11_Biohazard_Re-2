#include "stdafx.h"
#include "Bullet_UI.h"
#include "Crosshair_UI.h"

#define MAX_BULLET 12
#define MAX_BULLET_COLOR _float4(0.5, 1.0, 0.0, 0.f)
#define MIN_BULLET_COLOR _float4(1.0, 0.0, 0.0, 0.f)

#define TEN 10
#define ALPHA_ZERO _float4(0, 0, 0, 0)

#define BULLET_UI_LIFE 2.f

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
    /*
    부모 : BackGround Black
    자식 : Font 및 Line 
    */
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
                m_pTextUI[(_int)BULLET_TEXT_TYPE::CURRENT_BULLET].vOriginTextColor = iter->Get_FontColor();

                m_fFull_CurrentBullet_Transform = pTextTrans->Get_State_Float4(CTransform::STATE_POSITION);
                m_fFull_CurrentBullet_Transform.x -= 7.f;
            }

            else
            {
                m_pTextUI[(_int)BULLET_TEXT_TYPE::STORE_BULLET].pText = iter;
                m_pTextUI[(_int)BULLET_TEXT_TYPE::STORE_BULLET].pText->Set_Text(TEXT("0"));
                m_pTextUI[(_int)BULLET_TEXT_TYPE::STORE_BULLET].vOriginTextColor = iter->Get_FontColor();

                /*임의로 내려주기*/
                {
                    CTransform* pStoreFont_Pos = dynamic_cast<CTransform*>(m_pTextUI[(_int)BULLET_TEXT_TYPE::STORE_BULLET].pText->Get_Component(g_strTransformTag));
                    _float4 vStoreFont_Pos = pStoreFont_Pos->Get_State_Float4(CTransform::STATE_POSITION);
                    vStoreFont_Pos.y += 30.f;
                }

                m_fFull_StoreBullet_Transform = pTextTrans->Get_State_Float4(CTransform::STATE_POSITION);
                m_fFull_StoreBullet_Transform.x += 7.f;


            }
        }
    }
    else
        m_fBlending += 0.1f;

    Find_Crosshair();
    m_isRender = false;
    m_isPlay = false;

    m_isBlending = true;
    m_vColor[0].isColorChange = m_isColorChange = true;
    m_vColor[0].vColor = m_vCurrentColor = ALPHA_ZERO;

    m_fOrigin_Blending = m_vColor[0].fBlender_Value = m_fBlending;
    m_vColor[0].isBlender = m_isBlending = true;

    m_isLightMask = false;
    m_isMask = true;
    m_fMaskControl.x = 0.3f;
    m_fMaskControl.y = 0.4f;
    return S_OK;
}

void CBullet_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (nullptr == m_pCrosshair)
        Find_Crosshair();
    
    if (true == m_IsChild && true == m_isRender)
    {
        /*BulletUI를 통제하는 함수*/
        Control_BulletU();
        Change_BulletUI();
    }
    

    /* 만약 크로스헤어가 출력되었다면 유지, 크로스헤어가 유지되고 끝난 뒤로부터 BULLET_UI_LIFE(2.5f)만큼 출력한다.*/
    Render_Bullet_UI(fTimeDelta);
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
            m_pTextUI[(_int)BULLET_TEXT_TYPE::CURRENT_BULLET].vOriginTextColor = MAX_BULLET_COLOR;
            m_pTextUI[0].isFull = true;
        }
    }
    /* 0 일 때 색깔 바뀌게 함*/
    else if (0 <= m_iCurrentBullet)
    {
        if (nullptr != m_pTextUI[0].pText)
        {
            m_pTextUI[0].pText->Set_FontColor(MIN_BULLET_COLOR);
            m_pTextUI[(_int)BULLET_TEXT_TYPE::CURRENT_BULLET].vOriginTextColor = MIN_BULLET_COLOR;
            m_pTextUI[0].isFull = false;
        }
    }

    else
    {
        if (true == m_pTextUI[0].isFull)
        {
            m_pTextUI[0].pText->Set_FontColor(m_fOrigin_TextColor);
            m_pTextUI[0].isFull = false;
        }
    }   

    if (MAX_BULLET <= m_iStoreBullet)
    {
        if (nullptr != m_pTextUI[1].pText && false == m_pTextUI[1].isFull)
        {
            m_pTextUI[1].pText->Set_FontColor(MAX_BULLET_COLOR);
            m_pTextUI[(_int)BULLET_TEXT_TYPE::STORE_BULLET].vOriginTextColor = MAX_BULLET_COLOR;
            m_pTextUI[1].isFull = true;
        }
    }
    else
    {
        if (true == m_pTextUI[1].isFull)
        {
            m_pTextUI[1].isFull = false;
            m_pTextUI[(_int)BULLET_TEXT_TYPE::STORE_BULLET].vOriginTextColor = _float4(1, 1, 1, 1);
            m_pTextUI[1].pText->Set_FontColor(m_fOrigin_TextColor);
        }
    }
}

void CBullet_UI::Render_Bullet_UI(_float fTimeDelta)
{
    //if (nullptr == m_pCrosshair)
    //    return;

    /* Texture*/
    if (true == m_pCrosshair->IsRender())
    {
        m_isKeepCross = true;
        m_isRender = true;
    }

   if (true == m_isKeepCross)
    {
        if (false == m_isRender)
            m_isRender = true;

        /* 이미 출력된 상태*/
        if (BULLET_UI_LIFE <= m_fBulletTimer)
        {
            /* 크로스는 꺼졌는데 켜졌으면, 끄자*/
            if (false == m_pCrosshair->IsRender())
            {
                /* 이미 출력을 끈 상태 */
                if (m_fBlending >= 1.f)
                {
                    m_fBlending = 1.f;
                    m_fBulletTimer = 0.f;
                    m_isRender = m_isKeepCross = false;
                }
                else
                    m_fBlending += fTimeDelta * 1.5f;
            }
        }
        else
        {
            if (m_fBlending <= m_fOrigin_Blending)
            {
                if (false == m_pCrosshair->IsRender())
                    m_fBulletTimer += fTimeDelta;
                else
                    m_fBlending = m_fOrigin_Blending;
            }

            else
                m_fBlending -= fTimeDelta * 1.5f;
        }
    }

    /*Font*/
   if (!m_vecTextBoxes.empty() && false  == m_isKeepCross)
   {
       m_pTextUI[(_int)BULLET_TEXT_TYPE::CURRENT_BULLET].pText->Set_FontColor(ALPHA_ZERO);
       m_pTextUI[(_int)BULLET_TEXT_TYPE::STORE_BULLET].pText->Set_FontColor(ALPHA_ZERO);

       return;
   }
   else if (!m_vecTextBoxes.empty() && true == m_isKeepCross)
   {
       _float4 result_Current = m_fBlending * ALPHA_ZERO + (1 - m_fBlending) * m_pTextUI[(_int)BULLET_TEXT_TYPE::CURRENT_BULLET].vOriginTextColor;
       _float4 result_Store = m_fBlending * ALPHA_ZERO + (1 - m_fBlending) * m_pTextUI[(_int)BULLET_TEXT_TYPE::STORE_BULLET].vOriginTextColor;

       m_pTextUI[0].pText->Set_FontColor(result_Current);
       m_pTextUI[1].pText->Set_FontColor(result_Store);
   }
}

void CBullet_UI::Find_Crosshair()
{
    /* Crosshair 찾기*/
    list<class CGameObject*>* pUiLayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));

    for (auto& iter : *pUiLayer)
    {
        CCrosshair_UI* pCrosshair = dynamic_cast<CCrosshair_UI*>(iter);

        if (nullptr != pCrosshair && true == pCrosshair->Get_IsChild())
        {
            m_pCrosshair = pCrosshair;
            break;
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
