#include "stdafx.h"
#include "HPBar_UI.h"

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

        //m_eHPBar = CustomUIDesc->eHPBar_Type;
    }

    m_wstrNormal = m_wstrDefaultTexturComTag;
    m_wstrWaring = TEXT("Prototype_Component_Texture_WaringHP_UI");
    m_wstrDanger = TEXT("Prototype_Component_Texture_DangerHP_UI");

    //if (CCustomize_UI::HPBAR_TYPE::MAIN_BAR == m_eHPBar)
    //{

    //}

    m_fOrigin_MaskControl = m_Mask[0].fMaskControl;
    m_vDefaultColor_Origin = m_vColor[0].vColor;

    return S_OK;
}

void CHPBar_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (DOWN == m_pGameInstance->Get_KeyState('H'))
    {
        m_eCurrentHP = (HP_TYPE)((_uint)m_eCurrentHP + 1);
        if (m_eCurrentHP >= HP_TYPE::END_HP)
            m_eCurrentHP = HP_TYPE::NORMAL_HP;

        Change_HP(m_eCurrentHP);
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

void CHPBar_UI::MainBar(_float fTimeDelta)
{
    m_fShiftMaskUV_X += fTimeDelta * m_fMaskControl.x;
    
    /* 1. 한 바퀴(1)이 지날 때 */
    if (m_fShiftMaskUV_X >= 1.f)
    {
        m_isMaskDown = !m_isMaskDown;

        if (true == m_isMaskDown)
        {
            /* 만약 이미 Mask를 끈 상태라면 */
            m_isLightMask = false;
            
            m_fMaskControl.x += fTimeDelta;
            m_fMaskControl.y += fTimeDelta;
        }
        
        else if (true == m_isMaskDown)
        {
            m_isLightMask = true;

            /* Control 다시 돌려놓기 */
            if (m_fMaskControl.x >= m_fOrigin_MaskControl.x)
            {
                m_fMaskControl.x -= fTimeDelta;
                m_fMaskControl.y -= fTimeDelta;
            }
            else
                m_fMaskControl = m_fOrigin_MaskControl;
        }
    }
}

void CHPBar_UI::MainBar_MaskDown(_float fTimeDelta)
{
}

void CHPBar_UI::Change_HP(HP_TYPE _type)
{
    if (HP_TYPE::NORMAL_HP == _type)
    {
        m_vCurrentColor = m_vDefaultColor_Origin;
        m_vLightMask_Color = _float4(0.412f, 1.f, 0.f, 0.f);

        if (/*CCustomize_UI::HPBAR_TYPE::MAIN_BAR == m_eHPBar && */m_wstrNormal != m_wstrDefaultTexturComTag)
        {
            m_wstrDefaultTexturComTag = m_wstrNormal;
            Change_Texture(m_wstrNormal, TEXT("Com_DefaultTexture"));
        }
    }

    else if (HP_TYPE::BRUISE_HP == _type)
    {
        m_vCurrentColor = _float4(0.376f, 0.753f, 0.114f, 0.f);
        m_vLightMask_Color = _float4(0.412f, 1.f, 0.f, 0.f);

        if (/*CCustomize_UI::HPBAR_TYPE::MAIN_BAR == m_eHPBar && */m_wstrNormal != m_wstrDefaultTexturComTag)
        {
            m_wstrDefaultTexturComTag = m_wstrNormal;
            Change_Texture(m_wstrNormal, TEXT("Com_DefaultTexture"));
        }
    }
    else if (HP_TYPE::WARING_HP == _type)
    {
        m_vCurrentColor = _float4(0.714f, 0.404f, 0.071f, 0.f);
        m_vLightMask_Color = _float4(1.f, 0.518f, 0.f, 0.f);

        if (/*CCustomize_UI::HPBAR_TYPE::MAIN_BAR == m_eHPBar && */m_wstrWaring != m_wstrDefaultTexturComTag)
        {
            m_wstrDefaultTexturComTag = m_wstrWaring;
            Change_Texture(m_wstrWaring, TEXT("Com_DefaultTexture"));
        }
    }
    else if (HP_TYPE::DANGER_HP == _type)
    {
        m_vCurrentColor = _float4(0.651f, 0.11f, 0.11f, 0.f);
        m_vLightMask_Color = _float4(1.f, 0.f, 0.f, 0.f);

        if (/*CCustomize_UI::HPBAR_TYPE::MAIN_BAR == m_eHPBar && */m_wstrDanger != m_wstrDefaultTexturComTag)
        {
            m_wstrDefaultTexturComTag = m_wstrDanger;
            Change_Texture(m_wstrDanger, TEXT("Com_DefaultTexture"));
        }
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
