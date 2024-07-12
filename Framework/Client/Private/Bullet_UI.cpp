#include "stdafx.h"
#include "Bullet_UI.h"
#include "Crosshair_UI.h"
#include "Tab_Window.h"
#include "Player.h"

#define MAX_BULLET_COLOR _float4(0.5, 1.0, 0.0, 0.f)
#define MIN_BULLET_COLOR _float4(1.0, 0.0, 0.0, 0.f)

#define TEN 10
#define ZERO 0

#define ALPHA_ZERO _float4(0, 0, 0, 0)

#define BULLET_UI_LIFE 2.f
#define BULLET_BACKGROUND 999

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
    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;

        m_wstrFile = CustomUIDesc->wstrFileName;
    }

    if(TEXT("Bullet_UI") == m_wstrFile)
    {
        m_iEqipType = (_int)CPlayer::EQUIP::HG;

        if (true == m_IsChild)
        {
            for (auto& iter : m_vecTextBoxes)
            {
                CTransform* pTextTrans = static_cast<CTransform*>(iter->Get_Component(g_strTransformTag));

                if (m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).x > pTextTrans->Get_State_Float4(CTransform::STATE_POSITION).x)
                {
                    m_pTextUI[(_int)BULLET_TEXT_TYPE::CURRENT_BULLET].pText = iter;
                    m_pTextUI[(_int)BULLET_TEXT_TYPE::CURRENT_BULLET].pText->Set_Text(TEXT("10"));
                    m_pTextUI[(_int)BULLET_TEXT_TYPE::CURRENT_BULLET].vOriginTextColor = iter->Get_FontColor();
                    m_fOrigin_TextColor = iter->Get_FontColor();

                    m_fFull_CurrentBullet_Transform = pTextTrans->Get_State_Float4(CTransform::STATE_POSITION);
                    m_fFull_CurrentBullet_Transform.x -= 7.f;

                    m_pTextUI[(_int)BULLET_TEXT_TYPE::CURRENT_BULLET].iBulletCnt = m_iCurrentBullet = 10;

                }

                else
                {
                    m_pTextUI[(_int)BULLET_TEXT_TYPE::STORE_BULLET].pText = iter;
                    m_pTextUI[(_int)BULLET_TEXT_TYPE::STORE_BULLET].pText->Set_Text(TEXT("0"));
                    m_pTextUI[(_int)BULLET_TEXT_TYPE::STORE_BULLET].vOriginTextColor = iter->Get_FontColor();
                    m_fOrigin_TextColor = iter->Get_FontColor();

                    /*임의로 내려주기*/
                    {
                        CTransform* pStoreFont_Pos = static_cast<CTransform*>(m_pTextUI[(_int)BULLET_TEXT_TYPE::STORE_BULLET].pText->Get_Component(g_strTransformTag));
                        _float4 vStoreFont_Pos = pStoreFont_Pos->Get_State_Float4(CTransform::STATE_POSITION);
                        vStoreFont_Pos.y += 30.f;
                    }

                    m_fFull_StoreBullet_Transform = pTextTrans->Get_State_Float4(CTransform::STATE_POSITION);
                    m_fFull_StoreBullet_Transform.x += 7.f;

                    m_pTextUI[(_int)BULLET_TEXT_TYPE::STORE_BULLET].iBulletCnt = m_iStoreBullet = 0;
                }
            }
        }
        else
        {
            m_fBlending += 0.1f;
            m_iEqipType = BULLET_BACKGROUND;
        }
    }

    else if (TEXT("UI_Bullet_Grenade") == m_wstrFile)
    {
        m_iEqipType = (_int)CPlayer::EQUIP::GRENADE;
    }

    if (!m_vecTextBoxes.empty())
    {
        m_vOriginTextColor = m_vecTextBoxes.back()->Get_FontColor();

        for (auto& iter : m_vecTextBoxes)
        {
            iter->Set_FontColor(ALPHA_ZERO);
        }
    }

    /* Find 함수 */
    Find_Crosshair();
    Find_Player();

    /* Tool */
    {
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

        m_iCurrentBullet = 1;
    }

    return S_OK;
}

void CBullet_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

  
    if (m_pPlayer->Get_Equip() == (_int)CPlayer::EQUIP::STG || CPlayer::EQUIP::HG == m_pPlayer->Get_Equip())
    {
        if (m_iEqipType == (_int)CPlayer::EQUIP::STG || m_iEqipType == (_int)CPlayer::EQUIP::HG || m_iEqipType == BULLET_BACKGROUND)
            Render_Bullet_UI(fTimeDelta);

        else
        {
            m_fBulletTimer = 0.f;
            m_fBlending = 1.f;
            m_isRender = m_isKeepCross = false;
        }
    }

    else if (CPlayer::EQUIP::GRENADE == m_pPlayer->Get_Equip() || CPlayer::EQUIP::FLASHBANG == m_pPlayer->Get_Equip())
    {
        if (m_iEqipType == (_int)CPlayer::EQUIP::GRENADE || m_iEqipType == BULLET_BACKGROUND)
            Change_Grenade(fTimeDelta, (_int)CPlayer::EQUIP::GRENADE);

        else  if (m_iEqipType == (_int)CPlayer::EQUIP::FLASHBANG || m_iEqipType == BULLET_BACKGROUND)
            Change_Grenade(fTimeDelta, (_int)CPlayer::EQUIP::FLASHBANG);

        else
        {
            m_fBulletTimer = 0.f;
            m_fBlending = 1.f;
            m_isRender = m_isKeepCross = false;
        }
    }

    else if(CPlayer::EQUIP::NONE == m_pPlayer->Get_Equip())
    {
        if (m_fBlending >= 1.f)
            m_fBlending = 1.f;

        else
            m_fBlending += fTimeDelta * 1.5f;
    }

    //  Bullet_Font();
    if (!m_vecTextBoxes.empty())
    {
        _float4 result = m_fBlending * ALPHA_ZERO + (1 - m_fBlending) * _float4(1.f, 1.f, 1.f, 1.f);
        m_vecTextBoxes.back()->Set_FontColor(result);
    }

 /*   if (!m_vecTextBoxes.empty())
    {
        _float4 result = m_fBlending * ALPHA_ZERO + (1 - m_fBlending) * _float4(1.f, 1.f, 1.f, 1.f);
        m_vecTextBoxes.back()->Set_FontColor(result);
    }*/

    if (m_iCurrentBullet < 1)
        Mission_Complete();

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

void CBullet_UI::Mission_Complete()
{
    if (true == m_isTutiorial)
        return;

    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(g_Level, TEXT("Layer_Player"), 0));

    if (true == (*pPlayer->Get_Tutorial_Notify()))
        return;

    m_isTutiorial = true;
    *pPlayer->Get_Tutorial_Notify() = true;
    *pPlayer->Get_Tutorial_Type() = UI_TUTORIAL_TYPE::TUTORIAL_REROAD;
}

void CBullet_UI::Bullet_Font()
{
    /*Font*/
    if(m_iEqipType == CPlayer::EQUIP::HG)
    {
        if (nullptr == m_pTextUI[0].pText || nullptr == m_pTextUI[1].pText)
            return;

        if (!m_vecTextBoxes.empty() && false == m_isKeepCross)
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

    else if(m_iEqipType == CPlayer::EQUIP::GRENADE)
    {
        if (!m_vecTextBoxes.empty())
        {
            _float4 result = m_fBlending * ALPHA_ZERO + (1 - m_fBlending) * _float4(1.f, 1.f, 1.f, 1.f);
            m_vecTextBoxes.back()->Set_FontColor(result);
        }
    }
}

/* 수류탄 */
void CBullet_UI::Change_Grenade(_float fTimeDelta, _int _grenadeType)
{
    /* Font : 수류탄 개수 넣어주기 */
    if (static_cast<CPlayer::EQUIP>(_grenadeType) == CPlayer::EQUIP::FLASHBANG)
    {
        if (true == m_IsChild)
            m_isRender = true;
    }

    else if (static_cast<CPlayer::EQUIP>(_grenadeType) == CPlayer::EQUIP::GRENADE)
    {
        if (false == m_IsChild)
            m_isRender = true;
    }

    if(!m_vecTextBoxes.empty())
        m_vecTextBoxes.back()->Set_Text(to_wstring(m_iCurrentBullet));

    /* 이미 출력을 끈 상태 */
    if (m_fBlending <= 0.f)
        m_fBlending = 0.f;

    else
        m_fBlending -= fTimeDelta * 1.5f;
}


void CBullet_UI::Start()
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(g_Level, TEXT("Layer_Player"), 0));
    if (nullptr == pPlayer) {
        MSG_BOX(TEXT("CBullet_UI::Start()"));
        return;
    }

    pPlayer->RegisterObserver(this);

    OnNotify();
}


void CBullet_UI::OnNotify()
{
    if (false == m_IsChild)
        return;

    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(g_Level, TEXT("Layer_Player"), 0));
    if (nullptr == pPlayer)
        return;
    
    ITEM_NUMBER eItem_Number = pPlayer->Get_Equip_As_ITEM_NUMBER();

    if (eItem_Number == ITEM_NUMBER_END)
        return;

    CTab_Window* pTabWindow = dynamic_cast<CTab_Window*>(m_pGameInstance->Get_GameObject(g_Level, TEXT("Layer_TabWindow"), 0));
    if (nullptr == pTabWindow)
        return;

    m_iCurrentBullet = pTabWindow->Get_Search_Item_Quantity(eItem_Number);
    switch (eItem_Number) {
    case HandGun:
        m_iStoreBullet = pTabWindow->Get_Search_Item_Quantity(handgun_bullet01a);
        break;
    case ShotGun:
        m_iStoreBullet = pTabWindow->Get_Search_Item_Quantity(shotgun_bullet01a);
        break;
    case Grenade:
        m_iStoreBullet = pTabWindow->Get_Search_Item_Quantity(Grenade);
        break;
    case Flash_Bomb:
        m_iStoreBullet = pTabWindow->Get_Search_Item_Quantity(Flash_Bomb);
        break;
    }

    m_iMaxBullet = pPlayer->Get_MaxBullet();

    Change_BulletUI();
}

void CBullet_UI::Change_BulletUI()
{
    if (nullptr == m_pTextUI[0].pText || nullptr == m_pTextUI[1].pText)
        return;

    if (m_iCurrentBullet != m_pTextUI[(_int)BULLET_TEXT_TYPE::CURRENT_BULLET].iBulletCnt)
    {
        m_pTextUI[(_int)BULLET_TEXT_TYPE::CURRENT_BULLET].iBulletCnt = m_iCurrentBullet;
        m_pTextUI[(_int)BULLET_TEXT_TYPE::CURRENT_BULLET].pText->Set_Text(to_wstring(m_pTextUI[(_int)BULLET_TEXT_TYPE::CURRENT_BULLET].iBulletCnt));

    }

    if (m_iStoreBullet != m_pTextUI[(_int)BULLET_TEXT_TYPE::STORE_BULLET].iBulletCnt)
    {
        m_pTextUI[(_int)BULLET_TEXT_TYPE::STORE_BULLET].iBulletCnt = m_iStoreBullet;
        m_pTextUI[(_int)BULLET_TEXT_TYPE::STORE_BULLET].pText->Set_Text(to_wstring(m_pTextUI[(_int)BULLET_TEXT_TYPE::STORE_BULLET].iBulletCnt));
    }

    /* 10의 자리 숫자시 살짝 위치 옮겨줌*/
    if (10 <= m_pTextUI[(_int)BULLET_TEXT_TYPE::CURRENT_BULLET].iBulletCnt)
    {
        CTransform* pTextTrans = static_cast<CTransform*>(m_pTextUI[0].pText->Get_Component(g_strTransformTag));
        pTextTrans->Set_State(CTransform::STATE_POSITION, m_fFull_CurrentBullet_Transform);
    }

    if (10 <= m_iStoreBullet != m_pTextUI[(_int)BULLET_TEXT_TYPE::STORE_BULLET].iBulletCnt)
    {
        CTransform* pTextTrans = static_cast<CTransform*>(m_pTextUI[1].pText->Get_Component(g_strTransformTag));
        pTextTrans->Set_State(CTransform::STATE_POSITION, m_fFull_StoreBullet_Transform);
    }

    /* 12자리 일 때 색깔 바뀌게 함*/
    if (m_iMaxBullet <= m_pTextUI[(_int)BULLET_TEXT_TYPE::CURRENT_BULLET].iBulletCnt)
    {
        if (nullptr != m_pTextUI[0].pText && false == m_pTextUI[0].isFull)
        {
            m_pTextUI[0].pText->Set_FontColor(MAX_BULLET_COLOR);
            m_pTextUI[(_int)BULLET_TEXT_TYPE::CURRENT_BULLET].vOriginTextColor = MAX_BULLET_COLOR;
            m_pTextUI[0].isFull = true;
        }
    }
    /* 0 일 때 색깔 바뀌게 함*/
    else if (0 >= m_pTextUI[(_int)BULLET_TEXT_TYPE::CURRENT_BULLET].iBulletCnt)
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
        m_pTextUI[0].pText->Set_FontColor(m_fOrigin_TextColor);
        m_pTextUI[(_int)BULLET_TEXT_TYPE::CURRENT_BULLET].vOriginTextColor = m_fOrigin_TextColor;
        m_pTextUI[0].isFull = false;
    }
}

void CBullet_UI::Render_Bullet_UI(_float fTimeDelta)
{
    if (nullptr == m_pCrosshair)
        return;

    /* Texture*/
    if (true == m_pCrosshair->Render_Type())
    {
        m_isKeepCross = true;
        m_isRender = true;
        m_fBulletTimer = 0.f;
    }

    if (true == m_isKeepCross)
    {
        if (false == m_isRender)
            m_isRender = true;

        /* 이미 출력된 상태*/
        if (BULLET_UI_LIFE <= m_fBulletTimer)
        {
            /* 크로스는 꺼졌는데 켜졌으면, 끄자*/
            if (false == m_pCrosshair->Get_IsRender())
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
                if (false == m_pCrosshair->Get_IsRender())
                    m_fBulletTimer += fTimeDelta;
                else
                    m_fBlending = m_fOrigin_Blending;
            }

            else
                m_fBlending -= fTimeDelta * 1.5f;
        }
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

          //  Safe_AddRef<CCrosshair_UI*>(m_pCrosshair);
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

 /*   if(nullptr != m_pCrosshair)
    {
        Safe_Release<CCrosshair_UI*>(m_pCrosshair);
        m_pCrosshair = nullptr;
    }*/
}
