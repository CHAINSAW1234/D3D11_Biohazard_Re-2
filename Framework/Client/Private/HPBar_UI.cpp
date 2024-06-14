#include "stdafx.h"
#include "HPBar_UI.h"
#include "Inventory_Item_UI.h"

#define INVEN_POSITION _float2(-600.f, -244.f)
#define INGAME_POSITION _float2(-600.f, -144.f)
#define ALPHA_ZERO _float4(0, 0, 0, 0)

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

            // m_fMaskControl.y = 0.f;
            // m_Mask->fMaskControl.y = 0.f;

            m_fOrigin_MaskControl = m_Mask[0].fMaskControl;
            m_vDefaultColor_Origin = m_vColor[0].vColor;

            /* Font */
            if (!m_vecTextBoxes.empty())
            {
                _float4 pMainBarTrans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

                CTransform* pTextTrans = dynamic_cast<CTransform*>(m_vecTextBoxes.back()->Get_Component(g_strTransformTag));
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

        Change_HP(HP_TYPE::NORMAL_HP);
        m_isLightMask = true;
    }
    else
        m_fBlending = 0.5f;

    Find_Main_Inventory();
    return S_OK;
}

void CHPBar_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);
 
    Render_HPBar();

    if (true == m_isRender)
    {
        if (!m_vecTextBoxes.empty())
        {
            /* 만약 폰트 색이 알파라면 현재 hp 색으로 바꿔준다. */
           // if(m_vecTextBoxes.back()->Get_FontColor() == ALPHA_ZERO)
            m_vecTextBoxes.back()->Set_FontColor(m_vCurrentColor);
        }

        /* 예시 코드 : Change_HP에 따라서 변동될 것*/
        if(DOWN == m_pGameInstance->Get_KeyState('H'))
        {
            m_eCurrentHP = (HP_TYPE)((_uint)m_eCurrentHP + 1);
            if (m_eCurrentHP >= HP_TYPE::END_HP)
                m_eCurrentHP = HP_TYPE::NORMAL_HP;

            if (false == m_IsChild)
                Change_HP(m_eCurrentHP);
        }
    }

    if (false == m_IsChild)
        Operation_HPBar(fTimeDelta);
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

void CHPBar_UI::Render_HPBar()
{
    if (nullptr == m_pMain_Inventory)
        Find_Main_Inventory();

    else
    {
        /* Main Inventory HP Bar*/
        if (true == m_pMain_Inventory->IsRender())
        {
            HPBar_Position_Setting(true);
            m_isRender = true;
        }
        else
        {
            m_isRender = false;

            if (!m_vecTextBoxes.empty())
                m_vecTextBoxes.back()->Set_FontColor(ALPHA_ZERO);
        }
    }

    /* Show -Hp */
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
            m_iCurrent_ColorMask_Cnt = 0.f;
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
    if(false == m_IsChild)
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

            CTransform* pTrans = dynamic_cast<CTransform*>(m_pMainBar->Get_Component(g_strTransformTag));
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

    if (HP_TYPE::NORMAL_HP == _type)
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

    else if (HP_TYPE::BRUISE_HP == _type)
    {
        m_vCurrentColor = _float4(0.376f, 0.753f, 0.114f, 0.f);
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
    else if (HP_TYPE::WARING_HP == _type)
    {
        m_vCurrentColor = _float4(0.714f, 0.404f, 0.071f, 0.f);
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
    else if (HP_TYPE::DANGER_HP == _type)
    {
        m_vCurrentColor = _float4(0.651f, 0.11f, 0.11f, 0.f);
        m_vLightMask_Color = _float4(1.f, 0.f, 0.f, 0.f);

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
    list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

    for (auto& iter : *pUIList)
    {
        CInventory_Item_UI* pUI = dynamic_cast<CInventory_Item_UI*>(iter);

        if (nullptr != pUI && INVENTORY_TYPE ::MAIN_INVEN == pUI->Get_Inven_Type())
        {
            m_pMain_Inventory = pUI;
            break;
        }
    }
}

void CHPBar_UI::HPBar_Position_Setting(_bool _main)
{
    if (true == _main)
    {
        _float4 pTrans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
        pTrans.x = INVEN_POSITION.x;
        pTrans.y = INVEN_POSITION.y;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, pTrans);
    }
    else if (false == _main)
    {
        _float4 pTrans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
        pTrans.x = INGAME_POSITION.x;
        pTrans.y = INGAME_POSITION.y;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, pTrans);
    }

    if (!m_vecTextBoxes.empty())
    {
        _float4 vMainTrans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
        CTransform* pTextTrans = dynamic_cast<CTransform*>(m_vecTextBoxes.back()->Get_Component(g_strTransformTag));
        _float4 vTextTrans = pTextTrans->Get_State_Float4(CTransform::STATE_POSITION);

        vTextTrans.x = vMainTrans.x - m_fDistance_Font.x;
        vTextTrans.y = vMainTrans.y - m_fDistance_Font.y;
        pTextTrans->Set_State(CTransform::STATE_POSITION, vTextTrans);
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
