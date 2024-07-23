#include "stdafx.h"
#include "Targeting_Map_UI.h"
#include "Item_Map_UI.h"
#include "Player_Map_UI.h"
#include "Main_Map_UI.h"
#include "Tab_Window.h"
#include "Player.h"

#define ZERO 0
#define ALPHA_ZERO _float4(0.f, 0.f, 0.f, 0.f)

CTargeting_Map_UI::CTargeting_Map_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMap_Manager{ pDevice, pContext }
{
}

CTargeting_Map_UI::CTargeting_Map_UI(const CTargeting_Map_UI& rhs)
    : CMap_Manager{ rhs }
{

}

HRESULT CTargeting_Map_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTargeting_Map_UI::Initialize(void* pArg)
{
    CGameObject* pSupervisor = { nullptr };

    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;
        
        pSupervisor = CustomUIDesc->pSupervisor;
    }

    if (MAP_UI_TYPE::TARGET_MAP == m_eMapComponent_Type)
    {
        if (true == m_IsChild)
        {
            list<CGameObject*>* pMapUI_List = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

            if (nullptr != pMapUI_List)
            {
                CGameObject* pMainObj = pMapUI_List->back();
                CTargeting_Map_UI* pMainTarget = static_cast<CTargeting_Map_UI*>(pSupervisor);

                if (nullptr != pMainObj)
                {
                    if (pMainTarget->m_eMapComponent_Type == MAP_UI_TYPE::TARGET_MAP)
                    {
                        m_pMain_Target = static_cast<CTargeting_Map_UI*>(pSupervisor);

                        if (nullptr != m_pMain_Target)
                        {
                            CTransform* pMainTarget_Transform = static_cast <CTransform*>(m_pMain_Target->Get_Component(g_strTransformTag));

                            m_pMainTarget_Transform = pMainTarget_Transform;

                            _float4 vMainTarget_Trans = pMainTarget_Transform->Get_State_Float4(CTransform::STATE_POSITION);
                            _float4 vSubTarget_Trans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

                            if (vMainTarget_Trans.x > vSubTarget_Trans.x && ZERO == vSubTarget_Trans.y)
                            {
                                m_eCrosshair_Type = CROSSHAIR_TARGET_TYPE::LEFT_TARGET;
                                m_fTarget_Distance = vMainTarget_Trans.x - vSubTarget_Trans.x;
                            }

                            else if (vMainTarget_Trans.x < vSubTarget_Trans.x && ZERO == vSubTarget_Trans.y)
                            {
                                m_eCrosshair_Type = CROSSHAIR_TARGET_TYPE::RIGHT_TARGET;
                                m_fTarget_Distance = vSubTarget_Trans.x - vMainTarget_Trans.x;
                            }

                            else if (vMainTarget_Trans.y > vSubTarget_Trans.y)
                            {
                                m_eCrosshair_Type = CROSSHAIR_TARGET_TYPE::DOWN_TARGET;
                                m_fTarget_Distance = vMainTarget_Trans.y - vSubTarget_Trans.y;
                            }

                            else
                            {
                                m_eCrosshair_Type = CROSSHAIR_TARGET_TYPE::UP_TARGET;
                                m_fTarget_Distance = vSubTarget_Trans.y - vMainTarget_Trans.y;
                            }
                        }
                    }
                }
            }
        }
    }

    else if (MAP_UI_TYPE::TARGET_NOTIFY == m_eMapComponent_Type)
    {
        /* 1. Player Targeting */
        list<CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

        if (nullptr != pUIList)
        {
            for (auto& iter : *pUIList)
            {
                CTargeting_Map_UI* pTarget_UI = dynamic_cast<CTargeting_Map_UI*>(iter);

                if (nullptr != pTarget_UI && MAP_UI_TYPE::TARGET_MAP == pTarget_UI->m_eMapComponent_Type)
                {
                    m_pMain_Target = static_cast<CTargeting_Map_UI*>(iter);

                    break;
                }
            }
        }

        m_pMainTarget_Transform = static_cast<CTransform*>(m_pMain_Target->Get_Component(g_strTransformTag));

        /* 2. Between Object Distance */
        if (nullptr != m_pMain_Target)
        {
            CTransform* pMainTarget_Transform = static_cast<CTransform*>(m_pMain_Target->Get_Component(g_strTransformTag));

            m_fTargetNotify_Distance.x = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).x - pMainTarget_Transform->Get_State_Float4(CTransform::STATE_POSITION).x;
            m_fTargetNotify_Distance.y = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).y - pMainTarget_Transform->Get_State_Float4(CTransform::STATE_POSITION).y;
        }
        else
            MSG_BOX(TEXT("CMap_UI : Target_Notify에서 Player Main Target이 설정되지 않았습니다."));

        if (!m_vecTextBoxes.empty())
        {
            _float4 vTextBox_Texture = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

            _float4 vText = m_vecTextBoxes.back()->GetPosition();

            m_fTargetNotify_TextBox_Distance.x = vTextBox_Texture.x - vText.x;
            m_fTargetNotify_TextBox_Distance.y = vTextBox_Texture.y - vText.y;
        }

        /* 3. Store Items */
        Find_Item(); /* 아이템 */
    }

    /* Tool */
    if (FAILED(Change_Tool()))
        return E_FAIL;

    return S_OK;
}

void CTargeting_Map_UI::Tick(_float fTimeDelta)
{
    Find_NotifyText_RenderState();

    Notify_Font_Position();

    Targeting_Render(fTimeDelta);

    Verification_MapType();

    __super::Tick(fTimeDelta);
}

void CTargeting_Map_UI::Late_Tick(_float fTimeDelta)
{
    Targeting_Control();

    __super::Late_Tick(fTimeDelta);
}

HRESULT CTargeting_Map_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CTargeting_Map_UI::Change_Tool()
{
    m_isMouse_Control = true;

    m_isRender = false;

    m_fOrigin_Blending = m_vColor[0].fBlender_Value;

    m_vColor[0].vColor.w = 0.f;

    return S_OK;
}

void CTargeting_Map_UI::Find_Item()
{
    if (!m_ItemStore_Vec.empty())
        return;

    list<CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));
    
    m_ItemStore_Vec.resize(3);

    if (nullptr != pUIList)
    {
        for (auto& iter : *pUIList)
        {
            CItem_Map_UI* pItem = dynamic_cast<CItem_Map_UI*>(iter);

            if (nullptr != pItem)
            {
                if (pItem->Get_Floor_Type() == MAP_FLOOR_TYPE::FLOOR_1)
                    m_ItemStore_Vec[0].push_back(pItem);

                else if (pItem->Get_Floor_Type() == MAP_FLOOR_TYPE::FLOOR_2)
                    m_ItemStore_Vec[1].push_back(pItem);

                else if (pItem->Get_Floor_Type() == MAP_FLOOR_TYPE::FLOOR_3)
                    m_ItemStore_Vec[2].push_back(pItem);
            }
        }
    }

    if (m_ItemStore_Vec[0].empty() || m_ItemStore_Vec[1].empty() || m_ItemStore_Vec[2].empty())
    {
        MSG_BOX(TEXT("다차원 벡터에 아이템이 정상적으로 보관되지 않았습니다."));
    }
}

void CTargeting_Map_UI::Find_NotifyText_RenderState()
{
    /* Render 찾아주기 */
    if (m_vecTextBoxes.empty())
    {
        if (nullptr == m_isNotifyRender_Ptr)
        {
            list<CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

            for (auto& iter : *pUIList)
            {
                CTargeting_Map_UI* pTarget = dynamic_cast<CTargeting_Map_UI*>(iter);

                if (nullptr != pTarget)
                {
                    if (pTarget->m_eMapComponent_Type == MAP_UI_TYPE::TARGET_NOTIFY)
                    {
                        /* Text 박스를 가지고 있는 객체라면, */
                        if (!pTarget->m_vecTextBoxes.empty())
                        {
                            m_isNotifyRender_Ptr = (&pTarget->m_isNotifyRender);
                            break;
                        }
                    }
                }
            }
        }
    }
}

/* 아이템을 먹음으로써 Map 상태가 어떻게 변화하였는가? */
void CTargeting_Map_UI::Verification_MapType()
{
    if (true == m_isSearchForVerification)
    {
        /* 1. 모든 아이템을 먹었는가? */
        for (auto& iter : m_ItemStore_Vec[m_eFloorVerification])
        {
            LOCATION_MAP_VISIT a = iter->Get_Map_Location_Type();

            if (a == RECEPT_ROOM)
                int e = 1;

            if (m_eLocationVerification == iter->Get_Map_Location_Type())
            {
                
                if (false == iter->Get_Dead())
                {
                    m_isSearchForVerification = false;

                    return;
                }
            }
        }
        
        list<CGameObject*>* pMapUI_List = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

        /* 2. or One Item*/
        for (auto& iter : *pMapUI_List)
        {
            CMain_Map_UI* pMain = dynamic_cast<CMain_Map_UI*>(iter);

            if (nullptr != pMain)
            {
                if (m_eLocationVerification == static_cast<_ubyte>(pMain->Get_Map_Location_Type()))
                {
                    *(pMain->Map_Clear_Ptr()) = true;

                    m_isSearchForVerification = false;

                    break;
                }
            }
        }

        m_isSearchForVerification = false;
    }
}


void CTargeting_Map_UI::Targeting_Render(_float fTimeDelta)
{
    /* ▶ Target Notify Type */
    if (MAP_UI_TYPE::TARGET_NOTIFY == m_eMapComponent_Type)
    {
        if (nullptr == m_pMainTarget_Transform)
            return;

        if (m_ItemStore_Vec.empty())
            return;

        _float4 vMainTarget_Transform = m_pMainTarget_Transform->Get_State_Float4(CTransform::STATE_POSITION);
        _float3 vMainTarget_Scaled = m_pMainTarget_Transform->Get_Scaled();

        /* Render 확인 */
        if (true == Item_Hovering(vMainTarget_Transform, vMainTarget_Scaled))
        {
            if (!m_vecTextBoxes.empty())
            {
                _float4 vTextBoxTexture = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
                _float3 fTextBox = m_vecTextBoxes.back()->GetPosition();

                fTextBox.x = vTextBoxTexture.x - m_fTargetNotify_TextBox_Distance.x;
                fTextBox.y = vTextBoxTexture.y - m_fTargetNotify_TextBox_Distance.y;

                for (auto& iter : m_vecTextBoxes)
                {
                    CTransform* pTransText = static_cast<CTransform*>(iter->Get_Component(g_strTransformTag));
                    iter->State(fTextBox);
                }

                m_isNotifyRender = true;
            }
        }

        else
        {
            m_isNotifyRender = false;
        }

        /* Render 결과 */
        if (MAP_UI_TYPE::TARGET_NOTIFY == m_eMapComponent_Type)
        {
            if(!m_vecTextBoxes.empty())
            {
                if (true == m_isNotifyRender)
                    m_isRender = true;

                else if (false == m_isNotifyRender)
                    m_isRender = false;
            }
            else
            {
                if (true == *m_isNotifyRender_Ptr)
                    m_isRender = true;

                else if (false == *m_isNotifyRender_Ptr)
                    m_isRender = false;
            }
        }

        /* Font Color */
        if (!m_vecTextBoxes.empty())
        {
            if (false == m_isFont_Render)
            {
                for (auto& iter : m_vecTextBoxes)
                {
                    iter->Set_FontColor(ALPHA_ZERO);
                }
            }

            else if(true == m_isFont_Render)
            {
                for (auto& iter : m_vecTextBoxes)
                {
                    iter->Set_FontColor(m_vOriginTextColor);
                }
            }
        }
    }
}

void CTargeting_Map_UI::Targeting_Control()
{
    if (MAP_UI_TYPE::TARGET_MAP == m_eMapComponent_Type)
    {
        /* 처음 출력될 땐 부모는 Player를 잡고 있어야 하며, 자식은 부모를 따라다녀야 한다. */

        if (m_isPrevRender != m_pTab_Window->Get_MinMapRender())
        {
            m_isPrevRender = m_pTab_Window->Get_MinMapRender();

            if (false == m_IsChild)
            {
                _float4 vTarget = m_pMapPlayer_Transform->Get_State_Float4(CTransform::STATE_POSITION);

                vTarget.z = 0.08f;

                m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTarget);
            }

            else if (true == m_IsChild)
            {
                if (nullptr == m_pMainTarget_Transform)
                {
                    MSG_BOX(TEXT("Targeting_Map_UI() : Target들 중 하나가 Main Target을 발견하지 못했습니다."));
                    return;
                }

                if (m_eCrosshair_Type == CROSSHAIR_TARGET_TYPE::LEFT_TARGET)
                {
                    _float4 vMainTarget = m_pMainTarget_Transform->Get_State_Float4(CTransform::STATE_POSITION);
                    _float4 vCurrentLine = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

                    vCurrentLine.y = vMainTarget.y;
                    vCurrentLine.x = vMainTarget.x - m_fTarget_Distance;
                    vCurrentLine.z = 0.08f;

                    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentLine);
                }

                else if (m_eCrosshair_Type == CROSSHAIR_TARGET_TYPE::RIGHT_TARGET)
                {
                    _float4 vMainTarget = m_pMainTarget_Transform->Get_State_Float4(CTransform::STATE_POSITION);
                    _float4 vCurrentLine = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

                    vCurrentLine.y = vMainTarget.y;
                    vCurrentLine.x = vMainTarget.x + m_fTarget_Distance;
                    vCurrentLine.z = 0.08f;

                    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentLine);
                }
                
                else if (m_eCrosshair_Type == CROSSHAIR_TARGET_TYPE::UP_TARGET)
                {
                    _float4 vMainTarget = m_pMainTarget_Transform->Get_State_Float4(CTransform::STATE_POSITION);
                    _float4 vCurrentLine = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

                    vCurrentLine.x = vMainTarget.x + 3.f;
                    vCurrentLine.y = vMainTarget.y + m_fTarget_Distance;
                    vCurrentLine.z = 0.08f;

                    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentLine);
                }

                else
                {
                    _float4 vMainTarget = m_pMainTarget_Transform->Get_State_Float4(CTransform::STATE_POSITION);
                    _float4 vCurrentLine = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

                    vCurrentLine.x = vMainTarget.x + 3.f;
                    vCurrentLine.y = vMainTarget.y - m_fTarget_Distance;
                    vCurrentLine.z = 0.08f;

                    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentLine);
                }
            }
        }
    }

    else if (MAP_UI_TYPE::TARGET_NOTIFY == m_eMapComponent_Type)
    {
        _float4 vTarget_Transform = m_pMainTarget_Transform->Get_State_Float4(CTransform::STATE_POSITION);
        _float4 vTargetNotify_Transform = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

        vTargetNotify_Transform.x = vTarget_Transform.x + m_fTargetNotify_Distance.x;
        vTargetNotify_Transform.y = vTarget_Transform.y + m_fTargetNotify_Distance.y;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTargetNotify_Transform);
    }
}

void CTargeting_Map_UI::Notify_Font_Position()
{
    if (MAP_UI_TYPE::TARGET_NOTIFY == m_eMapComponent_Type)
    {
        /* Text Render UI로 바꿔주기 */
        for (auto& iter : m_vecTextBoxes)
        {
            iter->Set_isTransformBase(false);
            iter->Set_isUIRender(true);
        }

        if (!m_vecTextBoxes.empty())
        {
            _float4 vTextBoxTexture = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
            _float3 fTextBox = m_vecTextBoxes.back()->GetPosition();

            fTextBox.x = vTextBoxTexture.x - m_fTargetNotify_TextBox_Distance.x;
            fTextBox.y = vTextBoxTexture.y - m_fTargetNotify_TextBox_Distance.y;

            for (auto& iter : m_vecTextBoxes)
            {
                CTransform* pTransText = static_cast<CTransform*>(iter->Get_Component(g_strTransformTag));

                iter->State(fTextBox);

                m_isFont_Render = m_isRender;
            }
        }
    }
}


/* Main Target이 ITEM을 Hovering 하는 여부*/
_bool CTargeting_Map_UI::Item_Hovering(_float4 _mainPos, _float3 _scaled) // 타겟팅 가운데 중점이 item 사각형에 들어간다면
{
    _float2 fMainPos = _float2(_mainPos.x, _mainPos.y);

    if (nullptr == m_pMapPlayer)
        return false;

    if ((_int)*m_pMapPlayer->Get_ViewFloor_Type() - 2 < 0)
        return false;

    if (!m_ItemStore_Vec[(_int)*m_pMapPlayer->Get_ViewFloor_Type() - 2].empty())
    {
        for (auto& iter : m_ItemStore_Vec[(_int)*m_pMapPlayer->Get_ViewFloor_Type() - 2])
        {
            if (iter->Get_IsRender() != false)
            {
                CTransform* pItem_Transform = static_cast<CTransform*>(iter->Get_Component(g_strTransformTag));

                _float4 vItem_Transform = pItem_Transform->Get_State_Float4(CTransform::STATE_POSITION);
                _float3 vSize = pItem_Transform->Get_Scaled();

                _float4 vPosition = { vItem_Transform.x + g_iWinSizeX * 0.5f, -vItem_Transform.y + g_iWinSizeY * 0.5f, vItem_Transform.z, 0 };

                if (vItem_Transform.x - (vSize.x / 2) <= fMainPos.x && vItem_Transform.y - (vSize.y / 2) <= fMainPos.y
                    && vItem_Transform.x + (vSize.x / 2) >= fMainPos.x && vItem_Transform.y + (vSize.y / 2) >= fMainPos.y)
                {
                    m_wstrItem_Name = *iter->Item_Name();

                    if (!m_vecTextBoxes.empty())
                    {
                        for(auto& iter : m_vecTextBoxes)
                        {
                            if (m_wstrItem_Name == TEXT(""))
                            {
                                m_wstrItem_Name = TEXT("필수 요건");
                            }
                            iter->Set_Text(m_wstrItem_Name);
                        }
                    }

                    return true;
                }
            }
        }
    }

    return false;
}

CMap_Manager* CTargeting_Map_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTargeting_Map_UI* pInstance = new CTargeting_Map_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CTargeting_Map_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTargeting_Map_UI::Clone(void* pArg)
{
    CTargeting_Map_UI* pInstance = new CTargeting_Map_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CTargeting_Map_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTargeting_Map_UI::Free()
{
    __super::Free();
}
