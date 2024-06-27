#include "stdafx.h"
#include "Title_UI.h"
#include "Level_Loading.h"

#define ALPHA_ZERO           _float4(0, 0, 0, 0)
//#define FONT_LIGHT_COLOR    _float4(0.929f, 0.925f, 0.894f, 0)
#define FONT_LIGHT_COLOR    _float4(1, 1, 1, 0)
#define OPTION_MOVE_DISTANCE     30.f

CTitle_UI::CTitle_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCustomize_UI{ pDevice, pContext }
{
}

CTitle_UI::CTitle_UI(const CTitle_UI& rhs)
    : CCustomize_UI{ rhs }
{

}

HRESULT CTitle_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTitle_UI::Initialize(void* pArg)
{
    /*
    부모 : BackGround
    자식 : Logo
    Text : BackGround 내부
    */
    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;
    }

    if (false == m_IsChild)
    {
        /* 텍스트 지정*/
        if (!m_vecTextBoxes.empty())
            m_pText = m_vecTextBoxes.back();

        m_pTitle_BackGround = this;

        //Safe_AddRef<CTitle_UI*>(m_pTitle_BackGround);
        Find_Logo();
    }

    else if (true == m_IsChild)
    {
        m_iEndingType = PLAY_BUTTON::PLAY_DEFAULT;

        /* BackGround 가지고 있기 */
        list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

        for (auto& iter : *pUIList)
        {
            m_pTitle_BackGround = dynamic_cast<CTitle_UI*>(iter);

            if (nullptr != m_pTitle_BackGround && false == m_pTitle_BackGround->m_IsChild)
                break;
        }

        m_wstrLogo_Broken = TEXT("Prototype_Component_Texture_LogoBroken");
    }

    for (auto& iter : m_vecTextBoxes)
    {
        CTextBox* pText = dynamic_cast<CTextBox*>(iter);

        if (nullptr != pText)
        {
            /* "게임 시작" Font라면,*/
            if (TEXT("게임 시작") == pText->Get_Text())
            {
                m_isGameStart_Text = true;
                pText->Set_FontColor(ALPHA_ZERO);
            }

            /* Option Font라면,*/
            if (true == m_IsChild) /* 로고가 아닌 폰트라면 가리기 */
            {
                m_vOriginTextColor = pText->Get_FontColor();
                pText->Set_FontColor(ALPHA_ZERO);

                /*Position 저장 */
                CTransform* pTextTrans = static_cast<CTransform*>(pText->Get_Component(g_strTransformTag));
                m_vOriginOption_Pos = pTextTrans->Get_State_Float4(CTransform::STATE_POSITION);
            }
        }
    }

    m_isPlay = false;

    return S_OK;
}

void CTitle_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    ///* 부모 (BackGround) 가 Logo를 찾아야 함*/
    if (nullptr == m_pLogo && false == m_IsChild)
        Find_Logo();

    if (false == m_pTitle_BackGround->m_isTitleGame_Start)
        TitleGame_Start();

    ///* 인 게임 안에 들어가기 시작할 때 :  폰트 움직임 */
    else if (true == m_pTitle_BackGround->m_isTitleGame_Start)
    {
        if (nullptr != m_pLogo)
        {
            if (m_pLogo->m_fCurrentColor_Timer >= 1.3f)
            {
                m_pLogo->m_isKeepPlay = true;
                m_pLogo->m_isRender = false;
            }
        }

        InGame_Start(fTimeDelta);
    }

    /* 옵션 창 안이라면, */
    if (true == m_isInOption)
        Option_Click(fTimeDelta);

   /* if (true == m_IsChild)
    {
        if (!m_vecTextBoxes.empty())
        {
            CTextBox* pFont = m_vecTextBoxes.back();

            _float4 result = m_fBlending * ALPHA_ZERO + (1 - m_fBlending) * m_vOriginTextColor;
            pFont->Set_FontColor(result);
        }
    }*/
}

void CTitle_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CTitle_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CTitle_UI::TitleGame_Start()
{
    /* 만약 로고의 플레이가 완료 되었다면, */
    if (nullptr != m_pLogo && true == m_pLogo->m_isPlay)
    {
        if (ALPHA_ZERO == m_pLogo->m_vCurrentColor)
        {
            if(nullptr != m_pTitle_BackGround)
                m_pTitle_BackGround->m_isTitleGame_Start = true;
        }
    }

    else if (false == m_IsChild)
    {
        /* text를 건들었다면 play*/
        if (nullptr != m_pText && true == m_pText->IsMouseHover())
        {
            m_pLogo->m_isPlay = true;
        }
    }

   
}

void CTitle_UI::InGame_Start(_float fTimeDelta)
{
    if (true == m_isInOption)
        return;

    /* TEXT Color Change*/
    if(false == m_isGameStart_Text)
    {
        /* 보간하며 Text 출력 */
        if(!m_vecTextBoxes.empty())
        {
            _vector vOriginColor = XMLoadFloat4(&m_vOriginTextColor);
            _vector vResult = m_vecTextBoxes.back()->Get_FontColor() - vOriginColor;

            /* 1. 보간 중*/
            if (0.1f <= XMVector3Length(vResult).m128_f32[0])
            {
                m_fInterpolation_Timer += fTimeDelta;
                _float      fColorRatio = m_fInterpolation_Timer * 1.5f;

                _vector vStartColor = m_vecTextBoxes.back()->Get_FontColor();
                _vector vEndColor = m_vOriginTextColor;
                _vector vCurrentColor = XMVectorLerp(vStartColor, vEndColor, fColorRatio);

                /* 보간 결과 값*/
                m_vecTextBoxes.back()->Set_FontColor(vCurrentColor);
            }

            /* 2. 보간이 끝났다면*/
            else
                m_isInOption = true;
        }
    }
}

void CTitle_UI::Option_Click(_float fTimeDelta)
{
    CTransform* pOptionTextTrans = static_cast<CTransform*>(m_vecTextBoxes.back()->Get_Component(g_strTransformTag));
    _float4 fOptionTextTrans = pOptionTextTrans->Get_State_Float4(CTransform::STATE_POSITION);

    if(true == IsMouseHover())
    {
        if(TEXT("스토리") == m_vecTextBoxes.back()->Get_Text())
        {
            if(m_pGameInstance->Get_KeyState(VK_LBUTTON))
                m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY));
        }

        m_vecTextBoxes.back()->Set_FontColor(FONT_LIGHT_COLOR);

        /* 오른쪽으로 밀기 */
        if (fOptionTextTrans.x < m_vOriginOption_Pos.x + OPTION_MOVE_DISTANCE)
        {
            fOptionTextTrans.x += fTimeDelta * 200.f;
            pOptionTextTrans->Set_State(CTransform::STATE_POSITION, fOptionTextTrans);
        }
        else
        {
            fOptionTextTrans.x = m_vOriginOption_Pos.x + OPTION_MOVE_DISTANCE;
            pOptionTextTrans->Set_State(CTransform::STATE_POSITION, fOptionTextTrans);
        }
    }
    else
    {
        m_vecTextBoxes.back()->Set_FontColor(m_vOriginTextColor);

        /* 왼쪽 밀기 */
        if (fOptionTextTrans.x > m_vOriginOption_Pos.x)
        {
            fOptionTextTrans.x -= fTimeDelta * 200.f;
            pOptionTextTrans->Set_State(CTransform::STATE_POSITION, fOptionTextTrans);
        }
        else
        {
            pOptionTextTrans->Set_State(CTransform::STATE_POSITION, m_vOriginOption_Pos);
        }
    }
}

void CTitle_UI::Find_Logo()
{
    /* 로고를 Play 해준다 */
    list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

    if (nullptr != pUIList)
    {
        for (auto& iter : *pUIList)
        {
            m_pLogo = dynamic_cast<CTitle_UI*>(iter);

            //Safe_AddRef<CTitle_UI*>(m_pLogo);

            if (nullptr != m_pLogo && true == m_pLogo->m_IsChild)
                break;
        }
    }
}

CCustomize_UI* CTitle_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTitle_UI* pInstance = new CTitle_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CTitle_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTitle_UI::Clone(void* pArg)
{
    CTitle_UI* pInstance = new CTitle_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CTitle_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTitle_UI::Free()
{
    __super::Free();

  /*  if(nullptr != m_pTitle_BackGround)
    {
        Safe_Release< CTitle_UI*>(m_pTitle_BackGround);
        m_pTitle_BackGround = nullptr;
    }
    
    if(nullptr != m_pLogo)
    {
        Safe_Release< CTitle_UI*>(m_pLogo);
        m_pLogo = nullptr;
    }*/
}
