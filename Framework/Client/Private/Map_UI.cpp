#include "stdafx.h"
#include "Map_UI.h"
#include "Tab_Window.h"
#include "Player.h"

#define RED         _float4(0.8, 0, 0, 0)
#define BLUE        _float4(0.0, 0.7569, 0.85, 0.0)
#define ALPHA_ZERO  _float4(0, 0, 0, 0)

#define BLENDING    0.7f
#define ZERO        0.f

CMap_UI::CMap_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCustomize_UI{ pDevice, pContext }
{
}

CMap_UI::CMap_UI(const CMap_UI& rhs)
    : CCustomize_UI{ rhs }
{

}

HRESULT CMap_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMap_UI::Initialize(void* pArg)
{
    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;
        
        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;

        m_iWhichChild = CustomUIDesc->iWhich_Child;
        m_eMapComponent_Type = CustomUIDesc->eMapComponent_Type;
    }

    /* 부모는 렌더하지 않을 것임*/
    if(CCustomize_UI::MAP_UI_TYPE::MAIN_MAP == m_eMapComponent_Type)
    {
        if (false == m_IsChild)
        {
            m_isRender = false;
            m_vColor[0].vColor = m_vCurrentColor = _float4(0, 0, 0, 0);
        }

        /* 필수 요소 */
        if (m_iWhichChild == (_uint)MAP_CHILD_TYPE::BACKGROUND_MAP)
        {
            m_isPlay = false;
            m_vColor[0].vColor = m_vCurrentColor = ALPHA_ZERO;
            m_vColor[0].fBlender_Value = m_fBlending = 1.f;
            m_vColor[0].isBlender = m_isBlending = true;
        }
    }

    else if (CCustomize_UI::MAP_UI_TYPE::MASK_MAP == m_eMapComponent_Type)
    {
        m_vColor[0].vColor = m_vCurrentColor = _float4(0, 0, 0, 0);
    }

    else if (CCustomize_UI::MAP_UI_TYPE::PLAYER_MAP == m_eMapComponent_Type)
    {
        Find_Player();

        if (nullptr != m_pPlayer)
        {
            m_pPlayerTransform = static_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));

            Safe_AddRef<CTransform*>(m_pPlayerTransform);
        }

        m_vPlayer_InitPosition = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
        
        m_vPlayer_InitPosition_Distance = m_vPlayer_InitPosition;
    }

    else if (CCustomize_UI::MAP_UI_TYPE::NAMELINE_MAP == m_eMapComponent_Type)
    {
        _float4 vTransform = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

        vTransform.x += 15.f;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTransform);
    }

    else if (CCustomize_UI::MAP_UI_TYPE::TARGET_MAP == m_eMapComponent_Type)
    {
        if(false == m_IsChild)
        {
            Find_InMap_Player();

            if (nullptr == m_pInMap_Player)
                MSG_BOX(TEXT("Map UI의 Target이 InMap 안의 Player를 찾을 수 없습니다."));
        }

        else if (true == m_IsChild)
        {
            list<CGameObject*>* pMapUI_List = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

            if (nullptr != pMapUI_List)
            {
                CGameObject* pMainObj = pMapUI_List->back();
                CMap_UI* pMainTarget = static_cast<CMap_UI*>(pMainObj);

                if (nullptr != pMainObj)
                {
                    if (pMainTarget->m_eMapComponent_Type == CCustomize_UI::MAP_UI_TYPE::TARGET_MAP)
                    {
                        if (false == pMainTarget->m_IsChild) /* 직속 상관이라면 */
                            m_pTarget_Main = pMainTarget;

                        else if (true == pMainTarget->m_IsChild)
                            m_pTarget_Main = pMainTarget->m_pTarget_Main;

                        if (nullptr != m_pTarget_Main)
                        {
                            CTransform* pMainTarget_Transform = static_cast<CTransform*>(m_pTarget_Main->Get_Component(g_strTransformTag));

                            m_pTarget_Transform = pMainTarget_Transform;

                            _float4 vMainTarget_Trans = pMainTarget_Transform->Get_State_Float4(CTransform::STATE_POSITION);
                            _float4 vSubTarget_Trans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

                            if (vMainTarget_Trans.x > vSubTarget_Trans.x && ZERO == vSubTarget_Trans.y)
                            {
                                m_eSubTarget_Type = SUB_TARGET_TYPE::LEFT_TARGET;
                                m_fTarget_Distance = abs(vMainTarget_Trans.x - vSubTarget_Trans.x);
                            }

                            else if (vMainTarget_Trans.x < vSubTarget_Trans.x && ZERO == vSubTarget_Trans.y)
                            {
                                m_eSubTarget_Type = SUB_TARGET_TYPE::RIGHT_TARGET;
                                m_fTarget_Distance = abs(vSubTarget_Trans.x - vMainTarget_Trans.x);
                            }

                            else if (vMainTarget_Trans.y > vSubTarget_Trans.y )
                            {
                                m_eSubTarget_Type = SUB_TARGET_TYPE::DOWN_TARGET;
                                m_fTarget_Distance = abs(vSubTarget_Trans.y - vMainTarget_Trans.y);
                            }
                            else
                            {
                                m_eSubTarget_Type = SUB_TARGET_TYPE::UP_TARGET;
                                m_fTarget_Distance = abs(vMainTarget_Trans.y - vSubTarget_Trans.y);
                            }
                        }
                    }
                }
            }
        }
    }
        
    if (!m_vecTextBoxes.empty())
    {
        m_vOriginTextColor = m_vecTextBoxes.back()->Get_FontColor();

        for (auto& iter : m_vecTextBoxes)
        {
            iter->Set_FontColor(ALPHA_ZERO);
        }
    }

    m_isRender = false;
    Search_TabWindow();

    return S_OK;
}

void CMap_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    // 예비
    if (false == m_isGara)
    {
        Search_Map_Type(MAP_STATE_TYPE::SEARCH_CLEAR_STATE, MAIN_HOLL);
        Search_Map_Type(MAP_STATE_TYPE::SEARCH_STATE, ENTRANCE);
    }

    /* 1. Render */
    Render_Condition();

    /* 2. Transform */
    Transform_Condition(fTimeDelta);

    /* Player*/
    if (CCustomize_UI::MAP_UI_TYPE::PLAYER_MAP == m_eMapComponent_Type)
        Map_Player_Control(fTimeDelta);

    else if (CCustomize_UI::MAP_UI_TYPE::TARGET_MAP == m_eMapComponent_Type)
        Map_Target_Control(fTimeDelta);
}

void CMap_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CMap_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CMap_UI::Search_Map_Type(MAP_STATE_TYPE _searType, LOCATION_MAP_VISIT _mapType)
{
    const list<CGameObject*>* pMapUI_List = m_pGameInstance->Find_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_UI"));

    if(nullptr != pMapUI_List)
    {
        for (auto& iter : *pMapUI_List)
        {
            CMap_UI* pMap = dynamic_cast<CMap_UI*>(iter);

            if (nullptr != pMap)
            {
                if (_mapType == pMap->m_eMap_Location &&
                    CCustomize_UI::MAP_UI_TYPE::FONT_MASK_MAP != pMap->m_eMapComponent_Type &&
                    CCustomize_UI::MAP_UI_TYPE::MASK_MAP != pMap->m_eMapComponent_Type &&
                    CCustomize_UI::MAP_UI_TYPE::ITEM_MAP != pMap->m_eMapComponent_Type &&
                    CCustomize_UI::MAP_UI_TYPE::WINDOW_MAP != pMap->m_eMapComponent_Type &&
                    MAP_CHILD_TYPE::BACKGROUND_MAP == (MAP_CHILD_TYPE)pMap->m_iWhichChild)
                {
                    m_isGara = true;
                    Change_Search_Type(_searType, pMap);
                }
            }
        }   
    }
}

void CMap_UI::Change_Search_Type(MAP_STATE_TYPE _searType, CMap_UI* pMap)
{
    if (MAP_STATE_TYPE::NONE_STATE == _searType) // 기본
    {
        pMap->m_vCurrentColor = ALPHA_ZERO;
        pMap->m_fBlending = 0.f;
    }

    else if (MAP_STATE_TYPE::SEARCH_STATE == _searType) // 수색 중
    {
        pMap->m_vCurrentColor = RED;
        pMap->m_fBlending = BLENDING;
    }

    else if (MAP_STATE_TYPE::SEARCH_CLEAR_STATE == _searType) // 수색 완료
    {
        pMap->m_vCurrentColor = BLUE;
        pMap->m_fBlending = BLENDING;
    }
}

void CMap_UI::Find_InMap_Player()
{
    list<CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

    if (nullptr != pUIList)
    {
        for (auto& iter : *pUIList)
        {
            m_pInMap_Player = dynamic_cast<CMap_UI*>(iter);

            if (nullptr != m_pInMap_Player && CCustomize_UI::MAP_UI_TYPE::PLAYER_MAP == m_pInMap_Player->m_eMapComponent_Type)
            {
                m_pInMap_PlayerTrans = static_cast<CTransform*>(m_pInMap_Player->Get_Component(g_strTransformTag));
                
                Safe_AddRef<CMap_UI*>(m_pInMap_Player);
                Safe_AddRef<CTransform*>(m_pInMap_PlayerTrans);

                return;
            }
        }
    }

}

void CMap_UI::Transform_Condition(_float fTimeDelta)
{
    if(false == m_isTransfrom_Setting)
    {
        Transform_Adjustment();

        if (false == m_isTransfrom_Setting)
            MSG_BOX(TEXT("CMap_UI에서 무언가가 Transform Setting이 설정되지 않았습니다."));
    }

    Mouse_Pos(fTimeDelta);
}

void CMap_UI::Mouse_Pos(_float fTimeDelta)
{
    if (CCustomize_UI::MAP_UI_TYPE::MAIN_MAP == m_eMapComponent_Type)
        m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

    if (CCustomize_UI::MAP_UI_TYPE::MAIN_MAP == m_eMapComponent_Type    || CCustomize_UI::MAP_UI_TYPE::FONT_MAP == m_eMapComponent_Type ||
        CCustomize_UI::MAP_UI_TYPE::DOOR_MAP == m_eMapComponent_Type    || CCustomize_UI::MAP_UI_TYPE::WINDOW_MAP == m_eMapComponent_Type ||
        CCustomize_UI::MAP_UI_TYPE::PLAYER_MAP == m_eMapComponent_Type  || CCustomize_UI::MAP_UI_TYPE::TARGET_MAP == m_eMapComponent_Type || 
        CCustomize_UI::MAP_UI_TYPE::ITEM_MAP == m_eMapComponent_Type)
    {
        if (PRESSING == m_pGameInstance->Get_KeyState(VK_LBUTTON))
        {
            POINT		ptDeltaPos = m_pGameInstance->Get_MouseDeltaPos();

            if ((_long)0 != ptDeltaPos.x)
            {
                _float4 pos = {};
                pos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
                pos.x += fTimeDelta * (_float)ptDeltaPos.x * fMouseSensor;
                m_pTransformCom->Set_State(CTransform::STATE_POSITION, pos);

                if (CCustomize_UI::MAP_UI_TYPE::PLAYER_MAP == m_eMapComponent_Type)
                    m_vPlayer_InitPosition.x += fTimeDelta * (_float)ptDeltaPos.x * fMouseSensor;
            }

            if ((_long)0 != ptDeltaPos.y)
            {
                _float4 pos = {};
                pos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
                pos.y -= fTimeDelta * (_float)ptDeltaPos.y * fMouseSensor;
                m_pTransformCom->Set_State(CTransform::STATE_POSITION, pos);
                
                if (CCustomize_UI::MAP_UI_TYPE::PLAYER_MAP == m_eMapComponent_Type)
                    m_vPlayer_InitPosition.y -= fTimeDelta * (_float)ptDeltaPos.y * fMouseSensor;
            }
        }
    }
}

void CMap_UI::Transform_Adjustment()
{
    /* Main */
    if (CCustomize_UI::MAP_UI_TYPE::MAIN_MAP == m_eMapComponent_Type        || CCustomize_UI::MAP_UI_TYPE::DOOR_MAP == m_eMapComponent_Type
        || CCustomize_UI::MAP_UI_TYPE::WINDOW_MAP == m_eMapComponent_Type   || CCustomize_UI::MAP_UI_TYPE::FONT_MAP == m_eMapComponent_Type
        || CCustomize_UI::MAP_UI_TYPE::ITEM_MAP == m_eMapComponent_Type     || CCustomize_UI::MAP_UI_TYPE::TARGET_MAP == m_eMapComponent_Type)
    {
        _float4 vMainPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
        vMainPos.z = 0.1f;
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMainPos);

        m_isTransfrom_Setting = true;
    }

    else if (CCustomize_UI::MAP_UI_TYPE::PLAYER_MAP == m_eMapComponent_Type)
    {
        _float4 vMainPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
       
        if (false == m_IsChild)
            vMainPos.z = 0.1f;

        else if (true == m_IsChild)
            vMainPos.z = 0.11f;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMainPos);

        m_isTransfrom_Setting = true;
    }

    /* Mask */
    else if (CCustomize_UI::MAP_UI_TYPE::MASK_MAP == m_eMapComponent_Type ||
        CCustomize_UI::MAP_UI_TYPE::NAMELINE_MAP == m_eMapComponent_Type ||
        CCustomize_UI::MAP_UI_TYPE::SEARCH_TYPE_MAP == m_eMapComponent_Type ||
        CCustomize_UI::MAP_UI_TYPE::TARGET_MAP == m_eMapComponent_Type)
    {
        _float4 vMainPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
        vMainPos.z = 0.0f;
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMainPos);

        m_isTransfrom_Setting = true;
    }

    /* Mask */
    else if (CCustomize_UI::MAP_UI_TYPE::BACKGROUND_MAP == m_eMapComponent_Type )
    {
        _float4 vMainPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
        vMainPos.z = 0.9f;
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMainPos);

        m_isTransfrom_Setting = true;
    }

    else if (CCustomize_UI::MAP_UI_TYPE::FONT_MAP == m_eMapComponent_Type)
    {
        /* Font */
        if (!m_vecTextBoxes.empty())
        {
            for (auto& iter : m_vecTextBoxes)
            {
                CTransform* pFontTrans = static_cast<CTransform*>(iter->Get_Component(g_strTransformTag));
                _float4 vFontTrans = pFontTrans->Get_State_Float4(CTransform::STATE_POSITION);
                vFontTrans.z = 0.1f;
                pFontTrans->Set_State(CTransform::STATE_POSITION, vFontTrans);
            }
        }

        /* Texture */
        _float4 pTextureTrans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
        pTextureTrans.z = 0.1f;
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, pTextureTrans);

        m_isTransfrom_Setting = true;
    }
}

void CMap_UI::EX_ColorChange()
{   
    if (CCustomize_UI::MAP_UI_TYPE::MAIN_MAP == m_eMapComponent_Type)
    {
        /* 예시 코드*/
        if (DOWN == m_pGameInstance->Get_KeyState('I') && false == m_IsChild)
        {
            Search_Map_Type(MAP_STATE_TYPE::SEARCH_STATE, (LOCATION_MAP_VISIT)m_iCnt);

            if (false == m_IsChild)
                m_iCnt++;
        }
    }
}

void CMap_UI::Render_Condition()
{
    //////////    :   Font Render 
    if (!m_vecTextBoxes.empty())
    {
        if (m_isPrevRender != m_isRender)
        {
            for (auto& iter : m_vecTextBoxes)
            {
                m_isPrevRender = m_isRender;

                if (true == m_isRender)
                    iter->Set_FontColor(m_vOriginTextColor);

                else if(false == m_isRender)
                    iter->Set_FontColor(ALPHA_ZERO);
            }
        }
    }
        
    //////////    :   Texture Render 
    if (nullptr == m_pTab_Window)
    {
        Search_TabWindow();

        if (nullptr == m_pTab_Window)
            MSG_BOX(TEXT("Map_UI에서 Tab Window을 참조할 수 없습니다."));
    }

    if(nullptr != m_pTab_Window)
    {
        if (true == m_pTab_Window->Get_MinMapRender())
        {
            if (CCustomize_UI::MAP_UI_TYPE::PLAYER_MAP == m_eMapComponent_Type && m_IsChild)
                m_fBlending = 0.f;

            m_isRender = true;
        }
        else
            m_isRender = false;
    }
}

void CMap_UI::Map_Player_Control(_float fTimeDelta)
{
    /* 지도를 켰을 땐 계산을 하지 말아야 한다. */
    if (true == m_isRender)
    {
        ///////////  :   블러 조정
        if (false == m_IsChild)
        {
            if (m_fBlending >= 1.f)
                m_isBlurBlending = true;

            else if (m_fBlending <= 0.f)
                m_isBlurBlending = false;

            if (true == m_isBlurBlending)
                m_fBlending -= fTimeDelta * 1.5f;

            else if (false == m_isBlurBlending)
                m_fBlending += fTimeDelta * 1.5f;
        }
        return;
    }

    m_fBlending = 1.f;

    _matrix playerMatrix = m_pPlayerTransform->Get_WorldMatrix();

    ///////////  :   회전 
    _vector col0 = XMVector4Normalize(playerMatrix.r[0]);
    _vector col1 = XMVector4Normalize(playerMatrix.r[1]);
    _vector col2 = XMVector4Normalize(playerMatrix.r[2]);
    _vector col3 = XMVector4Normalize(playerMatrix.r[3]);

    _float4x4 mapPlayer = m_pTransformCom->Get_WorldFloat4x4();
    mapPlayer = XMMatrixIdentity();
    mapPlayer._11 = XMVectorGetX(col0);
    mapPlayer._12 = XMVectorGetZ(col0);
    mapPlayer._21 = XMVectorGetX(col2);
    mapPlayer._22 = XMVectorGetZ(col2);

    ///////////  :   이동
    _float4 vPlayerTrans = m_pPlayerTransform->Get_State_Float4(CTransform::STATE_POSITION);
    _float2 fPlayerDistance = {};

    fPlayerDistance.x = 0.f - vPlayerTrans.x;
    fPlayerDistance.y = 0.f - vPlayerTrans.z;

    mapPlayer._41 = (m_vPlayer_InitPosition.x + m_vPlayer_InitPosition_Distance.x) + (fPlayerDistance.x * 12.7f); // 13.3f; //160 / 8 -> 20 / 1.5
    mapPlayer._42 = (m_vPlayer_InitPosition.y - m_vPlayer_InitPosition_Distance.y * 0.5f) + fPlayerDistance.y * 13.7f; // 14.6f; // 90 / 4 -> 22
    mapPlayer._44 = 1.f;

    ///////////  :   설정 값
    m_pTransformCom->Set_WorldMatrix(mapPlayer);

    if(false == m_IsChild)
        m_pTransformCom->Set_Scaled(55.f, 55.f, 1);

    else if(true == m_IsChild)
        m_pTransformCom->Set_Scaled(25.f, 25.f, 1);
}

void CMap_UI::Map_Target_Control(_float fTimeDelta)
{
    /* 부모는 Player를 잡아야 함*/
    if (false == m_IsChild)
    {
        if (nullptr == m_pInMap_Player)
            MSG_BOX(TEXT("Map_UI에서 Target 부모가 Map Player를 발견하지 못했습니다."));

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pInMap_PlayerTrans->Get_State_Float4(CTransform::STATE_POSITION));
    }

    else if (true == m_IsChild)
    {
        if (nullptr == m_pTarget_Transform)
            MSG_BOX(TEXT("Map_UI에서 Target들 중 하나가 Main Target을 발견하지 못했습니다."));

        if (m_eSubTarget_Type == SUB_TARGET_TYPE::LEFT_TARGET)
        {
            _float4 vMainTarget = m_pTarget_Transform->Get_State_Float4(CTransform::STATE_POSITION);
            _float4 vCurrentLine = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

            vCurrentLine.y = vMainTarget.y;
            vCurrentLine.x = vMainTarget.x + m_fTarget_Distance;

            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentLine);
        }

        else if (m_eSubTarget_Type == SUB_TARGET_TYPE::RIGHT_TARGET)
        {
            _float4 vMainTarget = m_pTarget_Transform->Get_State_Float4(CTransform::STATE_POSITION);
            _float4 vCurrentLine = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

            vCurrentLine.y = vMainTarget.y;
            vCurrentLine.x = vMainTarget.x - m_fTarget_Distance;

            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentLine);
        }

        else if (m_eSubTarget_Type == SUB_TARGET_TYPE::UP_TARGET)
        {
            _float4 vMainTarget = m_pTarget_Transform->Get_State_Float4(CTransform::STATE_POSITION);
            _float4 vCurrentLine = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

            vCurrentLine.x = vMainTarget.x;
            vCurrentLine.y = vMainTarget.y - m_fTarget_Distance;

            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentLine);
        }

        else
        {
            _float4 vMainTarget = m_pTarget_Transform->Get_State_Float4(CTransform::STATE_POSITION);
            _float4 vCurrentLine = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

            vCurrentLine.x = vMainTarget.x;
            vCurrentLine.y = vMainTarget.y + m_fTarget_Distance;

            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentLine);
        }
    }
}

void CMap_UI::Search_TabWindow()
{
    list<CGameObject*>* pTab_Window_List = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

    if(nullptr != pTab_Window_List)
    {   
        for (auto& iter : *pTab_Window_List)
        {
            m_pTab_Window = dynamic_cast<CTab_Window*>(iter);

            if (nullptr != m_pTab_Window)
            {
               // Safe_AddRef(m_pTab_Window);
                return;
            }
        }
    }
}
   

CCustomize_UI* CMap_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMap_UI* pInstance = new CMap_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CMap_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMap_UI::Clone(void* pArg)
{
    CMap_UI* pInstance = new CMap_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CMap_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMap_UI::Free()
{
    __super::Free();

    /*if(nullptr != m_pTab_Window)
        Safe_Release(m_pTab_Window);*/

    Safe_Release<CTransform*>(m_pPlayerTransform);
    
    if(nullptr != m_pInMap_Player)
        Safe_Release<CMap_UI*>(m_pInMap_Player);

    if (nullptr != m_pInMap_PlayerTrans)
        Safe_Release<CTransform*>(m_pInMap_PlayerTrans);
}
