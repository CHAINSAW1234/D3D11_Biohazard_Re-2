#include "stdafx.h"
#include "HPBar_UI.h"
#include "Tab_Window.h"
#include "Damage_UI.h"
#include "Player.h"

#define INVEN_POSITION _float2(-600.f, -244.f)
#define INGAME_POSITION _float2(-600.f, -100.f)
#define ALPHA_ZERO _float4(0, 0, 0, 0)
#define INGAME_LIFE 5.f

CHPBar_UI::CHPBar_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCustomize_UI{ pDevice, pContext }
{
}

CHPBar_UI::CHPBar_UI(const CHPBar_UI& rhs)
    : CCustomize_UI{ rhs }
{

}

HRESULT CHPBar_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHPBar_UI::Initialize(void* pArg)
{
    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;

        m_eHPBar = CustomUIDesc->eHPBar_Type;
    }

    if (false == m_IsChild)
    {
        if (CCustomize_UI::HPBAR_TYPE::MAIN_BAR == m_eHPBar)
        {
            m_wstrNormal = m_wstrDefaultTexturComTag;
            m_wstrWaring = TEXT("Prototype_Component_Texture_WaringHP_UI");
            m_wstrDanger = TEXT("Prototype_Component_Texture_DangerHP_UI");

            m_fOrigin_MaskControl = m_Mask[0].fMaskControl;
            m_vDefaultColor_Origin = m_vColor[0].vColor;

            /* Font */
            if (!m_vecTextBoxes.empty())
            {
                _float4 pMainBarTrans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

                CTransform* pTextTrans = static_cast<CTransform*>(m_vecTextBoxes.back()->Get_Component(g_strTransformTag));
                _float4 vTextTrans = pTextTrans->Get_State_Float4(CTransform::STATE_POSITION);
                m_fDistance_Font = _float2(abs(pMainBarTrans.x - vTextTrans.x), abs(pMainBarTrans.y - vTextTrans.y));
            }
        }
        else if (CCustomize_UI::HPBAR_TYPE::BACKGROUND_BAR == m_eHPBar)
        {
            list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

            for (auto& iter : *pUIList)
            {
                CHPBar_UI* pUI = dynamic_cast<CHPBar_UI*>(iter);

                if (nullptr != pUI)
                {
                    if (CCustomize_UI::HPBAR_TYPE::MAIN_BAR == pUI->m_eHPBar)
                    {
                        m_fOrigin_MaskControl = m_fMaskControl = pUI->m_fMaskControl;
                        m_pMainBar = pUI;
                        m_vDefaultColor_Origin = m_vColor[0].vColor = m_vCurrentColor = m_pMainBar->m_vDefaultColor_Origin;

                        break;
                    }
                }
            }
        }

        m_isLightMask = true;
    }
    else
    {
        m_fBlending = 0.5f;
    }

    Find_Main_Inventory();

    HPBar_Position_Setting();

    if (FAILED(Change_Tool()))
        return E_FAIL;

    return S_OK;
}

void CHPBar_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    HP_Condition(fTimeDelta);
  
    if (false == m_IsChild)
        Operation_HPBar(fTimeDelta);

    if (!m_vecTextBoxes.empty())
    {
        for (auto& iter : m_vecTextBoxes)
        {
            iter->Set_Position_UI(m_fFont_Position);
            _float3 pos = iter->Get_Position_UI();

            pos.x -= 600.f;
            pos.y -= 250.f;

            iter->Set_Position_UI(pos);
        }
    }
}

void CHPBar_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CHPBar_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CHPBar_UI::Start()
{
    if (false == m_IsChild) {
        Find_Player();

        m_pPlayer->RegisterObserver(this);
        OnNotify();
    }

    if (nullptr == m_pDamage_Scene)
    {
        Find_DamageUI();

        if (nullptr == m_pDamage_Scene)
            MSG_BOX(TEXT("HPBar_UI() : Damage UI를 찾을 수 없습니다. "));
        // 생성 자체를 안 해줬을 가능성이 큼
    }

    if (!m_vecTextBoxes.empty())
    {
        _float3 vFontPos = m_vecTextBoxes.back()->Get_Position_UI();

        m_fFont_Position = _float3(vFontPos.x, vFontPos.y, vFontPos.z);
    }
}

void CHPBar_UI::OnNotify()
{
    _int iPlayerHp = m_pPlayer->Get_Hp();
    
    Call_DamageUI(iPlayerHp);

    HP_TYPE eCurrentHP_TYPE = (HP_TYPE)(iPlayerHp / 2);      //       0 ~ 5 ->  0 ~ 2

    if (eCurrentHP_TYPE != m_eCurrentHP) {

        m_isRender = true;
        m_isInGame_HPBar_Render = true;

        m_eCurrentHP = eCurrentHP_TYPE;
        Change_HP(m_eCurrentHP);

        if (!m_vecTextBoxes.empty())
            m_vecTextBoxes.back()->Set_FontColor(m_vCurrentColor);
    }

}

HRESULT CHPBar_UI::Change_Tool()
{
    _float4 vPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
    _float3 vScaled = m_pTransformCom->Get_Scaled();

    m_SavePos[0] = m_pTransformCom->Get_WorldMatrix();

    m_isRender = false;

    return S_OK;
}

void CHPBar_UI::Find_DamageUI()
{
    list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

    for (auto& iter : *pUIList)
    {
        CDamage_UI* pDamage = dynamic_cast<CDamage_UI*>(iter);

        if (nullptr != pDamage)
        {
            m_pDamage_Scene = pDamage;
            return;
        }
    }
}

void CHPBar_UI::Call_DamageUI(_int iPlayerHp)
{
    if (m_iPrevHP_Type > iPlayerHp)
        m_pDamage_Scene->Set_Damage_Scene_Start(true);

    m_iPrevHP_Type = iPlayerHp;
}

void CHPBar_UI::Operation_HPBar(_float fTimeDelta)
{
    m_fShiftMaskUV_X += fTimeDelta * m_fMaskControl.x;

    /* 1. 한 바퀴(1)이 지날 때 */
    if (m_fShiftMaskUV_X >= 1.f)
    {
        /* 변수 초기화*/
        ++m_iCurrent_ColorMask_Cnt;
        if (m_iCurrent_ColorMask_Cnt > m_iColorMask_Cnt)
        {
            m_isMaskDown = true;
            m_iCurrent_ColorMask_Cnt = 0;
            m_isTimerReturn = false;
        }
    }
    else
    {
        m_isMaskDown = false;
        m_fShiftMaskUV_X = 0.f;

        /* BackGround Mask가 Main을 따라갈 수 있게 한 번씩 갱신해준다. */
        if (CCustomize_UI::HPBAR_TYPE::BACKGROUND_BAR == m_eHPBar)
            BackGroundBar(fTimeDelta);
    }

    /* 2. 만약 이미 Mask를 끈 상태라면 */
    if (true == m_isMaskDown)
    {
        if (m_fMaskControl.y <= m_fOrigin_MaskControl.y + 1.f)
            m_fMaskControl.y += fTimeDelta;
    }

    else if (false == m_isMaskDown)
    {
        if (m_fMaskControl.y >= m_fOrigin_MaskControl.y)
        {
            // 여기서 Timer를 처음으로 돌려줘야 함
            if (false == m_isTimerReturn)
            {
                m_isTimerReturn = true;
                m_fMaskTimer = 0.f;
            }

            m_fMaskControl.y -= fTimeDelta;
        }

        else
            m_fMaskControl = m_fOrigin_MaskControl;
    }
}



void CHPBar_UI::BackGroundBar(_float fTimeDelta)
{
    if (false == m_IsChild)
    {
        if (nullptr != m_pMainBar)
        {
            /* Origin */
            m_fOrigin_MaskControl = m_pMainBar->m_fOrigin_MaskControl;

            /* Variable*/
            m_fMaskTimer = m_pMainBar->m_fMaskTimer;
            m_fMaskControl.x = m_pMainBar->m_fMaskControl.x;
            m_fMaskSpeed = m_pMainBar->m_fMaskSpeed;
            m_fShiftMaskUV_X = m_pMainBar->m_fShiftMaskUV_X;
            m_isMaskDown = m_pMainBar->m_isMaskDown;
            m_vCurrentColor = m_pMainBar->m_vCurrentColor;
            m_vLightMask_Color = m_pMainBar->m_vLightMask_Color;

            CTransform* pTrans = static_cast<CTransform*>(m_pMainBar->Get_Component(g_strTransformTag));
            _float4 vBackGroundTrans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
            vBackGroundTrans.x = pTrans->Get_State_Float4(CTransform::STATE_POSITION).x;
            vBackGroundTrans.y = pTrans->Get_State_Float4(CTransform::STATE_POSITION).y;
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vBackGroundTrans);
        }
    }
}

void CHPBar_UI::Change_HP(HP_TYPE _type)
{
    CTextBox* pText = {};

    if (!m_vecTextBoxes.empty())
        pText = m_vecTextBoxes.back();

    if (HP_TYPE::FINE == _type)
    {
        m_vCurrentColor = m_vDefaultColor_Origin;
        m_vLightMask_Color = _float4(0.412f, 1.f, 0.f, 0.f);

        if (nullptr != pText)
        {
            pText->Set_Text(TEXT("정상"));
            pText->Set_FontColor(m_vCurrentColor);
        }

        if (CCustomize_UI::HPBAR_TYPE::MAIN_BAR == m_eHPBar && m_wstrNormal != m_wstrDefaultTexturComTag)
        {
            m_wstrDefaultTexturComTag = m_wstrNormal;
            Change_Texture(m_wstrNormal, TEXT("Com_DefaultTexture"));
        }
    }
    else if (HP_TYPE::CAUTION == _type)
    {
        m_vCurrentColor = _float4(1.f, 0.618f, 0.2f, 0.f);
        m_vLightMask_Color = _float4(1.f, 0.518f, 0.f, 0.f);

        if (nullptr != pText)
        {
            pText->Set_Text(TEXT("주의"));
            pText->Set_FontColor(m_vCurrentColor);
        }

        if (CCustomize_UI::HPBAR_TYPE::MAIN_BAR == m_eHPBar && m_wstrWaring != m_wstrDefaultTexturComTag)
        {
            m_wstrDefaultTexturComTag = m_wstrWaring;
            Change_Texture(m_wstrWaring, TEXT("Com_DefaultTexture"));
        }
    }
    else if (HP_TYPE::DANGER == _type)
    {
        m_vCurrentColor = _float4(1.0f, 0.f, 0.0f, 0.0f);
        m_vLightMask_Color = _float4(0.8f, 0.f, 0.f, 0.f);

        if (nullptr != pText)
        {
            pText->Set_Text(TEXT("위험"));
            pText->Set_FontColor(m_vCurrentColor);
        }

        if (CCustomize_UI::HPBAR_TYPE::MAIN_BAR == m_eHPBar && m_wstrDanger != m_wstrDefaultTexturComTag)
        {
            m_wstrDefaultTexturComTag = m_wstrDanger;
            Change_Texture(m_wstrDanger, TEXT("Com_DefaultTexture"));
        }
    }
}

void CHPBar_UI::Find_Main_Inventory()
{
    list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_TabWindow"));

    for (auto& iter : *pUIList)
    {
        CTab_Window* pUI = dynamic_cast<CTab_Window*>(iter);

        if (nullptr != pUI)
        {
            m_pMain_Inven_Render = pUI->Get_MainRender_Ptr();
            break;
        }
    }
}

void CHPBar_UI::HPBar_Position_Setting()
{
    _float4 pTrans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
    pTrans.x = INVEN_POSITION.x;
    pTrans.y = INVEN_POSITION.y;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, pTrans);

    if (!m_vecTextBoxes.empty())
    {
        _float4 vMainTrans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
        CTransform* pTextTrans = static_cast<CTransform*>(m_vecTextBoxes.back()->Get_Component(g_strTransformTag));
        _float4 vTextTrans = pTextTrans->Get_State_Float4(CTransform::STATE_POSITION);
        vTextTrans.x = vMainTrans.x - m_fDistance_Font.x;
        vTextTrans.y = vMainTrans.y - m_fDistance_Font.y;
        pTextTrans->Set_State(CTransform::STATE_POSITION, vTextTrans);
    }
}

void CHPBar_UI::HP_Condition(_float fTimeDelta)
{
    /* 1. Inventory Open or Close */
    if (false == *m_pMain_Inven_Render)
    {
        m_isRender = true;
        m_isInGame_HPBar_Render = false;

        if (false == m_IsChild)
        {
            Change_HP(m_eCurrentHP);
        }

        if (!m_vecTextBoxes.empty())
        {
            m_vecTextBoxes.back()->Set_FontColor(m_vCurrentColor);
        }

    }
    else if (true == *m_pMain_Inven_Render && false == m_isInGame_HPBar_Render)
    {
        m_isRender = false;

        if (!m_vecTextBoxes.empty())
            m_vecTextBoxes.back()->Set_FontColor(ALPHA_ZERO);
    }

    /* 2. Minus HP  */
    if (true == m_isInGame_HPBar_Render)
    {
        m_fHPLifeTimer += fTimeDelta;

        if (INGAME_LIFE <= m_fHPLifeTimer)
        {
            if (m_fBlending >= 1.f)
            {
                m_fBlending = 1.f;
                m_isRender = false;
                m_isInGame_HPBar_Render = false;
                Reset_HPBar();
            }

            else
                m_fBlending += fTimeDelta;

            if (!m_vecTextBoxes.empty())
            {
                _float4 result = m_fBlending * ALPHA_ZERO + (1 - m_fBlending * 1.5f) * _float4(0, 0, 0, 0);
                m_vecTextBoxes.back()->Set_FontColor(result);
            }
        }
    }

}

void CHPBar_UI::Reset_HPBar()
{
    m_fBlending = 0.5f;
    m_fHPLifeTimer = 0.f;
    m_isLightMask = true;

    if (!m_vecTextBoxes.empty())
    {
        m_vecTextBoxes.back()->Set_FontColor(m_vCurrentColor);
    }
}


CCustomize_UI* CHPBar_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHPBar_UI* pInstance = new CHPBar_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CHPBar_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHPBar_UI::Clone(void* pArg)
{
    CHPBar_UI* pInstance = new CHPBar_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CHPBar_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHPBar_UI::Free()
{
    __super::Free();
}
