#include "stdafx.h"
#include "Hold_UI.h"

#define ALPHA_ZERO _float4(0.f, 0.f, 0.f, 0.f)


CHold_UI::CHold_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCustomize_UI{ pDevice, pContext }
{
}

CHold_UI::CHold_UI(const CHold_UI& rhs)
    : CCustomize_UI{ rhs }
{

}

HRESULT CHold_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHold_UI::Initialize(void* pArg)
{
    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;

        if (true == m_IsChild)
        {
            CGameObject* pParent = CustomUIDesc->pSupervisor;

            CHold_UI* pParentHold = static_cast<CHold_UI*>(pParent);

            CTransform* pParentTransform = static_cast<CTransform*>(pParentHold->Get_Component(g_strTransformTag));

            _float4 vParentTransform = pParentTransform->Get_State_Float4(CTransform::STATE_POSITION);

            _float4 vThisTransform = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

            m_fDistance.x = vParentTransform.x - vThisTransform.x;
            m_fDistance.y = vParentTransform.y - vThisTransform.y;

            m_pParentTransform = pParentTransform;
            m_pParentObj = pParentHold;
        }
    }

    if (!m_vecTextBoxes.empty())
    {
        for (auto& iter : m_vecTextBoxes)
        {
            CTransform* pFontTrans = static_cast<CTransform*>(iter->Get_Component(g_strTransformTag));

            m_fFontDistance.y = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).y - pFontTrans->Get_State_Float4(CTransform::STATE_POSITION).y;

            m_fFontDistance.x = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).x - pFontTrans->Get_State_Float4(CTransform::STATE_POSITION).x;
        }
    }

    /* Tool*/
    if (FAILED(Change_Tool()))
        return E_FAIL;

    Find_Player();

    m_isMaskCircle = true;

    m_isMask = false;

    m_isPlay = false;

    return S_OK;
}

void CHold_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    m_isMask = false;

    if(nullptr == m_pPlayer)
        Find_Player();

    if (true == m_IsChild)
    {
        if (nullptr != m_pParentTransform)
        {
            if (true == m_pParentObj->m_isRender)
                m_isRender = true;
            else if (false == m_pParentObj->m_isRender)
                m_isRender = false;

            m_fBlending = m_pParentObj->m_fBlending;

            _float2 result = {};

            _float4 vParentTransform = m_pParentTransform->Get_State_Float4(CTransform::STATE_POSITION);

            vParentTransform.x = vParentTransform.x - m_fDistance.x;
            vParentTransform.y = vParentTransform.y - m_fDistance.y;

            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vParentTransform);
        }
        else
            MSG_BOX(TEXT("CHold_UI() : Parent Transform을 찾을 수 없습니다."));
    }

    else if (false == m_IsChild)
    {
        Calc_Position();

        if (true == m_isHold)
        {
            m_isRender = true;

            if (m_fBlending <= 0.7f)
            {
                m_fBlending = 0.7f;

                m_fMaskTimer += fTimeDelta * 0.7f;
            }
            else
                m_fBlending -= fTimeDelta * 1.8f;
        }

        else if (false == m_isHold)
        {
            if (m_fBlending >= 1.f)
            {
                m_fBlending = 1.f;

                m_fMaskTimer = 0.f;

                m_isRender = false;
            }
            else
                m_fBlending += fTimeDelta * 1.8f;
        }
    }

    if (!m_vecTextBoxes.empty())
    {
        _float4 result = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

        result.x = result.x - m_fFontDistance.x;

        result.y = result.y - m_fFontDistance.y;

        for (auto& iter : m_vecTextBoxes)
        {
            CTransform* resultTrans = static_cast<CTransform*>(iter->Get_Component(g_strTransformTag));

            resultTrans->Set_State(CTransform::STATE_POSITION, result);

            if (false == m_isRender)
            {
                iter->Set_FontColor(_float4(0.f, 0.f, 0.f, 0.f));
            }

            else if (true == m_isRender)
            {
                _float4 result = m_fBlending * ALPHA_ZERO + (1 - m_fBlending) * _float4(1.f, 1.f, 1.f, 1.f);

                m_vecTextBoxes.back()->Set_FontColor(result);
            }
        }
    }
}

void CHold_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

void CHold_UI::Hold_Position(_float4 _pos)
{
    m_isRender = true;

    m_isHold = true;

    m_vTargetPos = _pos;

    m_isPlay = true;
}

void CHold_UI::Calc_Position()
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

HRESULT CHold_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CHold_UI::Change_Tool()
{

    return S_OK;
}

CCustomize_UI* CHold_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHold_UI* pInstance = new CHold_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CHold_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}



CGameObject* CHold_UI::Clone(void* pArg)
{
    CHold_UI* pInstance = new CHold_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CHold_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHold_UI::Free()
{
    __super::Free();
}
