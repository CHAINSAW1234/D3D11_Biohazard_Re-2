#include "stdafx.h"
#include "Announcement_Map_UI.h"
#include "Tab_Window.h"
#include "Player.h"

CAnnouncement_Map_UI::CAnnouncement_Map_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMap_Manager{ pDevice, pContext }
{
}

CAnnouncement_Map_UI::CAnnouncement_Map_UI(const CAnnouncement_Map_UI& rhs)
    : CMap_Manager{ rhs }
{

}

HRESULT CAnnouncement_Map_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAnnouncement_Map_UI::Initialize(void* pArg)
{
    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;
    }

    Find_Variable_Condition();

    m_isRender = false;

    m_isGetMap_Item = false;
    
    return S_OK;
}

void CAnnouncement_Map_UI::Tick(_float fTimeDelta)
{
    if (false == m_IsChild)
    {
        m_isColorChange = true;
        m_vCurrentColor.w = 0.3f;

        m_fBlending = 0.f;

        _float4 pos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

        pos.z = 0.f;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, pos);
    }

    if (true == m_isGetMap_Item)
    {
        m_isRender = true;

        if (!m_vecTextBoxes.empty())
        {
            for (auto& iter : m_vecTextBoxes)
            {
                iter->Set_FontColor(_float4(1.f, 1.f, 1.f, 1.f));
            }
        }
    }

    if (DOWN == m_pGameInstance->Get_KeyState(VK_SPACE))
    {
        m_isRender = false;

        CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Player"))->front());

        pPlayer->Set_isCamTurn(false);

        m_pGameInstance->Set_IsPaused(false);

        m_isGetMap_Item = false;

        if (!m_vecTextBoxes.empty())
        {
            for (auto& iter : m_vecTextBoxes)
            {
                iter->Set_FontColor(_float4(0.f, 0.f, 0.f, 0.f));
            }
        }
    }
}

void CAnnouncement_Map_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CAnnouncement_Map_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CAnnouncement_Map_UI::Change_Tool()
{
    return S_OK;
}

void CAnnouncement_Map_UI::Find_Variable_Condition()
{
    CGameObject* pTabWindow = m_pGameInstance->Get_GameObject(g_Level, TEXT("Layer_TabWindow"), 0);

    CTab_Window* pTab = static_cast<CTab_Window*>(pTabWindow);

    m_isGetMap_Item = pTab->Get_MapItem_Ptr();
}

CMap_Manager* CAnnouncement_Map_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAnnouncement_Map_UI* pInstance = new CAnnouncement_Map_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CAnnouncement_Map_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAnnouncement_Map_UI::Clone(void* pArg)
{
    CAnnouncement_Map_UI* pInstance = new CAnnouncement_Map_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CAnnouncement_Map_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAnnouncement_Map_UI::Free()
{
    __super::Free();
}
