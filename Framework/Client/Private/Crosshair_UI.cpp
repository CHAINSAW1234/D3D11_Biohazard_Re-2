#include "stdafx.h"

#include "Crosshair_UI.h"
#include "Camera_Free.h"

#define Deceleration 0.6f /* 감속 */
#define Zero 0

CCrosshair_UI::CCrosshair_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCustomize_UI{ pDevice, pContext }
{
}

CCrosshair_UI::CCrosshair_UI(const CCrosshair_UI& rhs)
    : CCustomize_UI{ rhs }
{

}

HRESULT CCrosshair_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCrosshair_UI::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_vCrosshair_OriginPos = m_fCorsshair_AimPoint = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
    m_vCrosshair_OriginScale = m_pTransformCom->Get_Scaled();

    if (true == m_IsChild)
    {
        _float4 vPoint = {};

        /* 부모 찾기 */
        list<class CGameObject*>* pUiLayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));

        for (auto& iter : *pUiLayer)
        {
            CCrosshair_UI* pUI = dynamic_cast<CCrosshair_UI*>(iter);

            if (nullptr != pUI && pUI->Get_Child() == false)
            {
                CTransform* pPointTrans = dynamic_cast<CTransform*>(iter->Get_Component(g_strTransformTag));
                vPoint = pPointTrans->Get_State_Float4(CTransform::STATE_POSITION);
                break;
            }
        }

        // -Crosshair.x
        if (vPoint.x > m_vCrosshair_OriginPos.x && Zero == m_vCrosshair_OriginPos.y)
        {
            m_eCrosshair_Type = CROSSHAIR_TYPE::CROSSHAIR_LEFT;
            m_fCorsshair_AimPoint.x = vPoint.x - m_fCrosshair_ControlDistance;
            m_vCrosshair_OriginPos.y = m_fCorsshair_AimPoint.y = vPoint.y;
            // NY : 초기화 고정 : 불러올 때 Crosshair 중 하나가 엇나가 있음
        }

        // +Crosshair.x  
        else if (vPoint.x < m_vCrosshair_OriginPos.x && Zero == m_vCrosshair_OriginPos.y)
        {
            m_eCrosshair_Type = CROSSHAIR_TYPE::CROSSHAIR_RIGHT;
            m_fCorsshair_AimPoint.x = vPoint.x + m_fCrosshair_ControlDistance;
            m_vCrosshair_OriginPos.y = m_fCorsshair_AimPoint.y = vPoint.y;
        }

        //  +Crosshair.y 
        else if (vPoint.y > m_vCrosshair_OriginPos.y && Zero == m_vCrosshair_OriginPos.x)
        {
            m_eCrosshair_Type = CROSSHAIR_TYPE::CROSSHAIR_UP;
            m_fCorsshair_AimPoint.y = vPoint.y - m_fCrosshair_ControlDistance;
            m_vCrosshair_OriginPos.x = m_fCorsshair_AimPoint.x = vPoint.x;
        }

        // -Crosshair.y 
        else //if (vPoint.y > vCrosshair_Trans.y)
        {
            m_eCrosshair_Type = CROSSHAIR_TYPE::CROSSHAIR_DOWN;
            m_fCorsshair_AimPoint.y = vPoint.y + m_fCrosshair_ControlDistance;
            m_vCrosshair_OriginPos.x = m_fCorsshair_AimPoint.x = vPoint.x;
        }
    }

    /* Point */
    else if (false == m_IsChild)
    {
        /* 만들 때 실수해서 임의로 크기 조정할 거임*/
        m_pTransformCom->Set_Scaled(10.f, 10.f, 1.f);
    }

    m_isRender = false;

    return S_OK;
}

void CCrosshair_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    /* ▶ 예비 조작 코드 */
    if (PRESSING == m_pGameInstance->Get_KeyState(VK_RBUTTON))
        m_isAiming = true;
    else
        m_isAiming = false;

    /* ▶ 동작 함수 */
    if (true == m_isAiming)
        Aiming(fTimeDelta);

    else if (false == m_isAiming)
        Aiming_Return();

    Camera_Aiming();
}

void CCrosshair_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CCrosshair_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CCrosshair_UI::Camera_Aiming()
{
    CGameObject* pCameraObj = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_ZZZCamera"))->back();

    if (nullptr != pCameraObj)
    {
        CCamera_Free* pCamerFreeObj = dynamic_cast<CCamera_Free*>(pCameraObj);

        pCamerFreeObj->Set_Crosshair_Aiming(m_isAiming); /* 조준 중인가? */
    }
}

void CCrosshair_UI::Aiming(_float fTimeDelta)
{
    /* ▶ 조준 완료 시*/
    if (true == m_IsChild && false == m_isRender)
        m_isRender = true;

    _float4 vCrosshair_Trans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
    _float3 vCrosshair_Scale = m_pTransformCom->Get_Scaled();

    m_fCrosshair_AccTimer += fTimeDelta;

    if (1.f <= m_fCrosshair_AccTimer)
        m_fCrosshair_AccTimer = 0.f;

    m_fCrosshair_Timer += fTimeDelta * m_fCrosshair_AccTimer;

    /* Crosshair를 고정시켜야 할 때 */
    if (m_fCrosshair_Timer >= 0.8f)
    {
        if (false == m_IsChild)
            m_isRender = true;

        else if (true == m_IsChild)
        {
            if (m_eCrosshair_Type == CROSSHAIR_TYPE::CROSSHAIR_LEFT)
            {
                if (vCrosshair_Trans.x <= m_fCorsshair_AimPoint.x)
                    vCrosshair_Trans.x += m_fCrosshair_Timer * 2.f;
            }

            else if (m_eCrosshair_Type == CROSSHAIR_TYPE::CROSSHAIR_RIGHT)
            {
                if (vCrosshair_Trans.x >= m_fCorsshair_AimPoint.x)
                    vCrosshair_Trans.x -= m_fCrosshair_Timer * 2.f;
            }

            else if (m_eCrosshair_Type == CROSSHAIR_TYPE::CROSSHAIR_UP)
            {
                if (vCrosshair_Trans.y <= m_fCorsshair_AimPoint.y)
                    vCrosshair_Trans.y += m_fCrosshair_Timer * 2.f;
            }

            else if (m_eCrosshair_Type == CROSSHAIR_TYPE::CROSSHAIR_DOWN)
            {
                if (vCrosshair_Trans.y >= m_fCorsshair_AimPoint.y)
                    vCrosshair_Trans.y -= m_fCrosshair_Timer * 2.f;
            }

            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCrosshair_Trans);

            /* Crosshair Min Scaled Fixed */
            m_pTransformCom->Set_Scaled(m_fFixed_MinScaled.x, m_fFixed_MinScaled.y, m_fFixed_MinScaled.z);
        }
        return;
    }

    /* ▶ 조준 중일 시 */
    if (false == m_IsChild)
        return;

    if (m_eCrosshair_Type == CROSSHAIR_TYPE::CROSSHAIR_LEFT)
    {
        if (vCrosshair_Trans.x <= m_fCorsshair_AimPoint.x - m_fCrosshair_Aiming_Ready_Distance)
        {
            vCrosshair_Trans.x += m_fCrosshair_Timer * Deceleration;
            vCrosshair_Scale.x -= m_fCrosshair_Timer * Deceleration;
        }
    }

    else if (m_eCrosshair_Type == CROSSHAIR_TYPE::CROSSHAIR_RIGHT)
    {
        if (vCrosshair_Trans.x >= m_fCorsshair_AimPoint.x + m_fCrosshair_Aiming_Ready_Distance)
        {
            vCrosshair_Trans.x -= m_fCrosshair_Timer * Deceleration;
            vCrosshair_Scale.x -= m_fCrosshair_Timer * Deceleration;
        }
    }

    else if (m_eCrosshair_Type == CROSSHAIR_TYPE::CROSSHAIR_UP)
    {
        if (vCrosshair_Trans.y <= m_fCorsshair_AimPoint.y - m_fCrosshair_Aiming_Ready_Distance)
        {
            vCrosshair_Trans.y += m_fCrosshair_Timer * Deceleration;
            vCrosshair_Scale.x -= m_fCrosshair_Timer * Deceleration;
        }
    }

    else if (m_eCrosshair_Type == CROSSHAIR_TYPE::CROSSHAIR_DOWN)
    {
        if (vCrosshair_Trans.y >= m_fCorsshair_AimPoint.y + m_fCrosshair_Aiming_Ready_Distance)
        {
            vCrosshair_Trans.y -= m_fCrosshair_Timer * Deceleration;
            vCrosshair_Scale.x -= m_fCrosshair_Timer * Deceleration;
        }
    }

    m_pTransformCom->Set_Scaled(vCrosshair_Scale.x, vCrosshair_Scale.y, vCrosshair_Scale.z);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCrosshair_Trans);
    m_fFixed_MinScaled = vCrosshair_Scale;
}

void CCrosshair_UI::Aiming_Return()
{
    if (false == m_IsChild || (true == m_IsChild && true == m_isRender))
        m_isRender = false;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vCrosshair_OriginPos);
    m_pTransformCom->Set_Scaled(m_vCrosshair_OriginScale.x, m_vCrosshair_OriginScale.y, m_vCrosshair_OriginScale.z);

    m_fCrosshair_Timer = 0.0f;
    m_fCrosshair_AccTimer = 0.0f;
}

CCustomize_UI* CCrosshair_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCrosshair_UI* pInstance = new CCrosshair_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CCrosshair_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCrosshair_UI::Clone(void* pArg)
{
    CCrosshair_UI* pInstance = new CCrosshair_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CCrosshair_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCrosshair_UI::Free()
{
    __super::Free();
}
