#include "stdafx.h"
#include "Selector_UI.h"
#include "Player.h"
#include "InteractProps.h"
#include "Tab_Window.h"

#define ALPHA_ZERO              _float4(0, 0, 0, 0)

#define OUT_DISTANCE            5.f
#define INTERACTIVE_DISTANCE    2.f

#define CHECK_SIZE              50.f
#define X_TYPEY                 50.f

CSelector_UI::CSelector_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCustomize_UI{ pDevice, pContext }
{
}

CSelector_UI::CSelector_UI(const CSelector_UI& rhs)
    : CCustomize_UI{ rhs }
{

}

HRESULT CSelector_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSelector_UI::Initialize(void* pArg)
{
    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;

        list<CGameObject*>* pUILayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));

        if (true == m_IsChild)
        {
            CSelector_UI* pSelector = dynamic_cast<CSelector_UI*>(pUILayer->back());

            if (nullptr != pSelector)
            {
                if (pSelector == CustomUIDesc->pSupervisor)
                {
                    m_pSelectorParent = static_cast<CSelector_UI*>(pSelector);
                }
            }
        }
    }

    /* Find Player / Object */
    Find_Player();

    Find_TabWindow();

    /* Tool*/
    if (FAILED(Change_Tool()))
        return E_FAIL;

    return S_OK;
}

void CSelector_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    Exception_Handle();

    Operate_Selector(fTimeDelta);
}

void CSelector_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CSelector_UI::Render()
{
    if (false == *m_pTab_Window->Get_MainRender_Ptr())
        return E_FAIL;

    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSelector_UI::Change_Tool()
{
    m_fColorTimer_Limit = 1.f;
    m_iEndingType = PLAY_BUTTON::PLAY_DEFAULT;
    m_vColor[0].fBlender_Value = m_fBlending = 0.f;
    m_isPlay = false;

    m_vColor[0].isBlender = m_isBlending = true;
    m_vColor[0].vColor = m_vCurrentColor = ALPHA_ZERO;
    m_vColor[0].isColorChange = m_isColorChange = true;

    /* Client*/
    m_fOriginSize = m_pTransformCom->Get_Scaled();
    m_isRender = false;

    /* 1. Texture*/
    m_wstrInteractive_Tag = m_wstrDefaultTexturComTag;
    m_wstrNonInteractive_Tag = TEXT("Prototype_Component_Texture_Perspective_Selecter_Check");

    return S_OK;
}

CGameObject* CSelector_UI::Destroy_Selector()
{
    m_isOutDistance = true;

    return nullptr;
}

void CSelector_UI::Select_Type(_bool _Interact, _float4 _objPos)
{
    m_isInteractive = _Interact;

    m_vTargetPos = _objPos;

    m_isUsing = true;

    m_isOutDistance = false;
}

void CSelector_UI::Calc_Position()
{
    _float fDistance = Distance_Player(m_vTargetPos);

    _vector                         vTargetPos
        = { XMLoadFloat4(&m_vTargetPos) };

    _matrix							ViewMatrix
        = { m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW) };

    _matrix							ProjMatrix
        = { m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ) };

    _matrix							VPMatrix
        = { ViewMatrix * ProjMatrix };

    _vector                         vViewPos
        = { XMVector3TransformCoord(vTargetPos, ViewMatrix) };

    m_isCull
        = { XMVectorGetZ(vViewPos) < 0.f };

    _vector			                vProjSpacePosition
        = { XMVector3TransformCoord(vTargetPos, VPMatrix) };

    _matrix                          OrthoProjMatrix
        = { XMLoadFloat4x4(&m_ProjMatrix) };

    _matrix                         OrthoProjMatrixInv
        = { XMMatrixInverse(nullptr, OrthoProjMatrix) };

    _vector                         vOrthoWorldPosition
        = { XMVector3TransformCoord(vProjSpacePosition, OrthoProjMatrixInv) };


    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(XMVectorGetX(vOrthoWorldPosition), XMVectorGetY(vOrthoWorldPosition), 0.5f, 1.f));


    if (false == m_isCull)
    {
        m_pTransformCom->Set_Scaled(40.f, 40.f, 1.f);
    }
}

void CSelector_UI::Rendering(_float fTimeDelta)
{
    /* 1. 보여주면 안 될때*/
    if(false == m_IsChild)
    {
        if (false == m_isUsing ||
            m_vTargetPos == _float4(0.f, 0.f, 0.f, 0.f) ||
            m_vTargetPos == _float4(0.f, 0.f, 0.f, 1.f))
        {
            Reset();

            return;
        }

        /* 2. Destory()를 통해 끝냄을 알릴 때*/
        if (true == m_isOutDistance)
        {
            if (m_fBlending >= 1.f)
            {
                m_isRender = false;

                m_isUsing = false;
            }
            else
                m_fBlending += fTimeDelta;

            return;
        }

        else if (false == m_isOutDistance)
        {
            /* 1. 컬링을 한다 */
            if (true == m_isCull)
            {
                m_isRender = false;

                m_fBlending = 1.f;
            }

            /* 2. 컬링을 하지 않는다. */
            else if (false == m_isCull)
            {
                m_isRender = true;

                _float4 a = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
                m_fBlending = 0.f;
            }
        }
    }

    else if (true == m_IsChild)
    {
        CTransform* pTransform = static_cast<CTransform*>(m_pSelectorParent->Get_Component(g_strTransformTag));
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, pTransform->Get_State_Float4(CTransform::STATE_POSITION));

        if (true == m_pSelectorParent->m_isRender)
        {
            m_fBlending = m_pSelectorParent->m_fBlending;
            m_isRender = true;
        }

        else if (false == m_pSelectorParent->m_isRender)
        {
            m_isRender = false;
        }
    }
}

void CSelector_UI::Operate_Selector(_float fTimeDelta)
{
    Calc_Position();

    Rendering(fTimeDelta);
}

void CSelector_UI::Exception_Handle()
{
    if (nullptr == m_pPlayer)
    {
        Find_Player();
    }
} 


void CSelector_UI::Reset()
{
    m_fBlending = 1.f;

    m_isRender = false;

    m_isUsing = false;

    m_isInteractive = false;

    m_isOutDistance = false;
}


void CSelector_UI::Find_TabWindow()
{
    list<class CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_TabWindow"));

    for (auto& iter : *pUIList)
    {
        CTab_Window* pTabWin = dynamic_cast<CTab_Window*>(iter);

        if (nullptr != pTabWin)
        {
            m_pTab_Window = pTabWin;

            //  Safe_AddRef<CTab_Window*>(m_pTab_Window);
        }
    }

}
CCustomize_UI* CSelector_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSelector_UI* pInstance = new CSelector_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CSelector_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}



CGameObject* CSelector_UI::Clone(void* pArg)
{
    CSelector_UI* pInstance = new CSelector_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CSelector_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSelector_UI::Free()
{
    __super::Free();
}
