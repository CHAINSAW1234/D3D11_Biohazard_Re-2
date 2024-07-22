#include "stdafx.h"
#include "Title_UI.h"
#include "Level_Loading.h"

#define ALPHA_ZERO           _float4(0, 0, 0, 0)
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
    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;

        if (TEXT("Title_UI") == CustomUIDesc->wstrFileName)
        { 
            m_eTitleType = static_cast<TITLE_TYPE>(TITLE_TYPE::MAIN_TITLE);
        }

        else if(TEXT("UI_Title_Select") == CustomUIDesc->wstrFileName)
        { 
            m_eTitleType = static_cast<TITLE_TYPE>(TITLE_TYPE::SELECT_TITLE);
        }
    }

    if(static_cast<TITLE_TYPE>(TITLE_TYPE::MAIN_TITLE) == m_eTitleType)
    {
        /* Logo 사이즈 좀 줄이자 */
        list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));
        
        if(nullptr != pUIList)
        {
            CTitle_UI* pParentTitle = dynamic_cast<CTitle_UI*>(pUIList->back());

            if (nullptr != pParentTitle)
            {
                if(false == pParentTitle->m_IsChild)
                {
                    _float3 fSize = m_pTransformCom->Get_Scaled();

                    fSize.y -= 50.f;

                    m_pTransformCom->Set_Scaled(fSize.x, fSize.y, fSize.z);

                    m_SavePos[2]._11 = m_SavePos[1]._11 = m_SavePos[0]._11 = fSize.x;
                    m_SavePos[2]._22 = m_SavePos[1]._22 = m_SavePos[0]._22 = fSize.y;
                    m_SavePos[2]._33 = m_SavePos[1]._33 = m_SavePos[0]._33 = fSize.z;
                }
            }
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
                if (TEXT("게임 시작") == pText->Get_Text())
                {
                    m_isGameStart_Text = true;
                    pText->Set_FontColor(ALPHA_ZERO);
                }

                if (true == m_IsChild) 
                {
                    m_vOriginTextColor = pText->Get_FontColor();

                    pText->Set_FontColor(ALPHA_ZERO);

                    /*Position 저장 */
                    CTransform* pTextTrans = static_cast<CTransform*>(pText->Get_Component(g_strTransformTag));

                    m_vOriginOption_Pos = pTextTrans->Get_State_Float4(CTransform::STATE_POSITION);
                }
            }
        }
    }

    else if (static_cast<TITLE_TYPE>(TITLE_TYPE::SELECT_TITLE) == m_eTitleType)
    {
        m_isRender = false;
    }

    if (FAILED(Change_Tool()))
        return E_FAIL;

    return S_OK;
}

void CTitle_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if(false == m_isSound)
    {
        m_isSound = true;

        m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_titleLogoClear.mp3"), CH_TICK_2D, 0.2f);
    }

    MainTitle_Operate(fTimeDelta);

    SelectTitle_Operate(fTimeDelta);

    Color_Rendering(fTimeDelta);

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

HRESULT CTitle_UI::Change_Tool()
{
    m_isPlay = false;

    if (false == m_IsChild)
    {
        m_vColor[0].isBlender = m_isBlending = true;

        m_vColor[0].fBlender_Value = m_fBlending = 0.f;

        m_vColor[0].isColorChange = m_isColorChange = true;

        m_vColor[0].vColor = _float4(0.f, 0.f, 0.f, 1.f);
    }

    return S_OK;
}

void CTitle_UI::TitleGame_Start(_float fTimeDelta)
{
    if (nullptr != m_pLogo && true == m_pLogo->m_isPlay)
    {
        if (ALPHA_ZERO == m_pLogo->m_vCurrentColor)
        {
            if(nullptr != m_pTitle_BackGround)
            {
                m_pTitle_BackGround->m_isTitleGame_Start = true;
            }
        }
    }

    else if (false == m_IsChild)
    {
        m_fLogoStart_Timer += fTimeDelta;

        if (m_fLogoStart_Timer >= 2.f)
        {
            if (nullptr != m_pText && true == m_pText->IsMouseHover())
            {
                m_fLogoStart_Timer = 0.f;

                m_pLogo->m_isPlay = true;
            }
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
            {
                m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_titleInGame.mp3"), CH_TICK_2D, 0.2f);

                // m_isSelectCharector = true;
                m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY));
            }
        }

        /* 오른쪽으로 밀기 */
        if (fOptionTextTrans.x < m_vOriginOption_Pos.x + OPTION_MOVE_DISTANCE)
        {
            m_vecTextBoxes.back()->Set_FontColor(FONT_LIGHT_COLOR);

            if(false == m_isCheckSound)
            {
                m_isCheckSound = true;

                m_pGameInstance->PlaySoundEffect_2D(TEXT("UI"), TEXT("sound_ui_titleSelectOption.mp3"), CH_TICK_2D, 0.2f);
            }

            fOptionTextTrans.x += fTimeDelta * 200.f;
            
            pOptionTextTrans->Set_State(CTransform::STATE_POSITION, fOptionTextTrans);
        }
        else
        {
            m_isCheckSound = false;
            
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

void CTitle_UI::Color_Rendering(_float fTimeDelta)
{
    if (true == m_IsChild)
        return;

    m_fColorTimer += fTimeDelta;

    if (m_fColorTimer >= 1.5f)
        m_isColorLight = true;

    if (true == m_isColorLight)
    {
        if (m_fBlending >= 0.10f)
            m_isColorLightType = true;

        else if (m_fBlending <= 0.f)
        {
            m_isColorLightType = false;

            m_iLightCnt++;
        }

        if (true == m_isColorLightType)
            m_fBlending -= fTimeDelta;

        else if (false == m_isColorLightType)
            m_fBlending += fTimeDelta;


        if (m_iLightCnt == 4)
        {
            m_fBlending = 0.f;

            m_iLightCnt = 0;

            m_fColorTimer = 0.f;

            m_isColorLight = false;
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

void CTitle_UI::MainTitle_Operate(_float fTimeDelta)
{
    /* 부모 (BackGround) 가 Logo를 찾아야 함*/
    if (nullptr == m_pLogo && false == m_IsChild)
        Find_Logo();

    if (false == m_pTitle_BackGround->m_isTitleGame_Start)
        TitleGame_Start(fTimeDelta);

    /* 인 게임 안에 들어가기 시작할 때 :  폰트 움직임 */
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
    {
        Option_Click(fTimeDelta);
    }
}

void CTitle_UI::SelectTitle_Operate(_float fTimeDelta)
{
    if (static_cast<TITLE_TYPE>(TITLE_TYPE::SELECT_TITLE) == m_eTitleType)
    {
        m_isRender = true;
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
