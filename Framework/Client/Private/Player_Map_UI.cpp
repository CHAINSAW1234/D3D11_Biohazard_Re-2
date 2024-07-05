#include "stdafx.h"
#include "Player_Map_UI.h"
#include "Static_Map_UI.h"
#include "Main_Map_UI.h"
#include "Tab_Window.h"
#include "Player.h"

#define MINMAP_X_SCALED       1011.f
#define MINMAP_Y_SCALED       743.f

/* 1층 크기*/
#define MODELMAP_X_FLOO1     82.0969925f
#define MODELMAP_Y_FLOO1     60.4390125f 

/* 2층 크기*/
#define MODELMAP_X_FLOO2      82.0431098f
#define MODELMAP_Y_FLOO2      38.3414335f

/* 3층 크기*/
#define MODELMAP_X_FLOO3     70.7840423f
#define MODELMAP_Y_FLOO3     38.0877256f

CPlayer_Map_UI::CPlayer_Map_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMap_Manager{ pDevice, pContext }
{
}

CPlayer_Map_UI::CPlayer_Map_UI(const CPlayer_Map_UI& rhs)
    : CMap_Manager{ rhs }
{

}

HRESULT CPlayer_Map_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer_Map_UI::Initialize(void* pArg)
{
    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;

        m_pSupervisor = CustomUIDesc->pSupervisor;
    }

    CGameObject* pBackGround = Find_BackGround();

    if (nullptr != pBackGround)
    {
        CTransform* pBackGroundTrans = static_cast<CTransform*>(pBackGround->Get_Component(g_strTransformTag));

        m_vBackGround_Center = pBackGroundTrans->Get_State_Float4(CTransform::STATE_POSITION);
    }

    m_fCurrent_ModelScaled.x = MODELMAP_X_FLOO1;
    m_fCurrent_ModelScaled.y = MODELMAP_Y_FLOO1;

    m_vPlayer_MovePos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

    m_vPlayer_InitPos.x = 0.f + m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).x;
    m_vPlayer_InitPos.y = 0.f + m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).y;

    m_isMouse_Control = true;

    return S_OK;
}

void CPlayer_Map_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    Rendering();
    Open_Map();
    Map_Player_Control(fTimeDelta);
}

void CPlayer_Map_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CPlayer_Map_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CPlayer_Map_UI::Rendering()
{
    /* 1. Map Inventory를 킬 때만 Map Player를 렌더한다. */
    if (nullptr != m_pTab_Window)
    {
        if (true == m_pTab_Window->Get_MinMapRender())
        {
            if (m_pMapPlayer->m_eCurrent_ViewFloor == m_ePlayer_Floor)
                m_isRender = true;

            else
                m_isRender = false;
        }
        else
            m_isRender = false;
    }

    Map_Player_Setting();
}

/* Map Player와 Main Map 객체들의 거리를 재는 것을 전부 끝냈느냐? */
_bool CPlayer_Map_UI::IsDistanceMeasured_Completely(_bool _find)
{
    list<CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

    for (auto& iter : *pUIList)
    {
        CMain_Map_UI* pMapMain = dynamic_cast<CMain_Map_UI*>(iter);

        if (nullptr != pMapMain)
        {
            if (true == _find)
            {
                if (*pMapMain->Distance_End() == false)
                    return false;
            }
            else
            {
                *pMapMain->Distance_End() = false;
            }
        }
    }

    return true;
}


void CPlayer_Map_UI::Open_Map()
{
    /* ▶ 처음 맵을 열었을 때 플레이어의 위치에 Floor가 존재해야 한다. */
    if (m_isPrevRender != m_pTab_Window->Get_MinMapRender())
    { 
        /* 1. 현재 Player가 들고 있는 Floor를 Render 할 것이다. */
        m_pMapPlayer->m_eCurrent_ViewFloor = static_cast<MAP_FLOOR_TYPE>(m_pPlayer->Get_Player_Floor());
        m_ePlayer_Floor = m_pMapPlayer->m_eCurrent_ViewFloor;

        /* 2. Map을 열 때마다 현재 플레이어가 존재하는 플로어의 크기를 정해줄 것 */
        if (MAP_FLOOR_TYPE::FLOOR_1 == m_ePlayer_Floor)
        {
            m_fCurrent_ModelScaled.x = MODELMAP_X_FLOO1;
            m_fCurrent_ModelScaled.y = MODELMAP_Y_FLOO1;
        }

        else if (MAP_FLOOR_TYPE::FLOOR_2 == m_ePlayer_Floor)
        {
            m_fCurrent_ModelScaled.x = MODELMAP_X_FLOO2;
            m_fCurrent_ModelScaled.y = MODELMAP_Y_FLOO2;
        }

        else if (MAP_FLOOR_TYPE::FLOOR_3 == m_ePlayer_Floor)
        {
            m_fCurrent_ModelScaled.x = MODELMAP_X_FLOO3;
            m_fCurrent_ModelScaled.y = MODELMAP_Y_FLOO3;
        }

        m_isPlayer_FloorSetting = true;
        m_isPrevRender = m_pTab_Window->Get_MinMapRender();
    }
}

void CPlayer_Map_UI::Map_Player_Control(_float fTimeDelta)
{
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

    else if (false == m_isRender && m_eCurrent_ViewFloor == m_ePlayer_Floor)
    {
        _matrix playerMatrix = m_pTransformCom->Get_WorldMatrix();

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

     /*   _float4 vPlayerPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
        mapPlayer._41 = vPlayerPos.x;
        mapPlayer._42 = vPlayerPos.y;
        mapPlayer._43 = vPlayerPos.z;
        mapPlayer._44 = vPlayerPos.w;*/

        ///////////  :   설정 값
        m_pTransformCom->Set_WorldMatrix(mapPlayer);

        if (false == m_IsChild)
            m_pTransformCom->Set_Scaled(55.f, 55.f, 1);

        else if (true == m_IsChild)
            m_pTransformCom->Set_Scaled(25.f, 25.f, 1);

        ///////////  :   이동
        Map_Player_Moving(fTimeDelta);
    }
}

void CPlayer_Map_UI::Map_Player_Moving(_float fTimeDelta)
{
    _float2 fComparison = {};

    // MINMAP과 MODELMAP의 크기 비율 계산
    fComparison.x = MINMAP_X_SCALED / m_fCurrent_ModelScaled.x;
    fComparison.y = MINMAP_Y_SCALED / m_fCurrent_ModelScaled.y;

    //offset.x /= fComparison.x;
    //offset.y /= fComparison.y;

    _float2 Moving_Value = {};

    Moving_Value.x = 0.f - m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).x;
    Moving_Value.y = 0.f - m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).z; // 플레이어의 초기 위치와 현재 위치의 차이 계산

    // MINMAP 비율에 맞춰 이동 값을 조정
    Moving_Value.x *= fComparison.x;
    Moving_Value.y *= fComparison.y;

    _float4 vMiniMapPlayer = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
    vMiniMapPlayer.x = m_vPlayer_MovePos.x + Moving_Value.x;
    vMiniMapPlayer.y = m_vPlayer_MovePos.y + Moving_Value.y - (m_vPlayer_InitPos.y * 0.5f);

    if (false == m_IsChild)
        vMiniMapPlayer.z = 0.1f;

    else if (true == m_IsChild)
         vMiniMapPlayer.z = 0.09f;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMiniMapPlayer);
}

void CPlayer_Map_UI::Map_Player_Setting()
{
    /* ▶ Map Player <-> Main Map 거리 재기 완료 시에  */
    if (true == IsDistanceMeasured_Completely(true))
    {
        /* 1. Prev Render 에 현재 렌더를 넣어준다 */
        m_isPrevRender = m_pTab_Window->Get_MinMapRender();

        list<CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));
        _float4 vPlayertrans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

        /* 2. Player Pos : 몇 층에 있는 지 확인 했는 가에 대한 변수 : false 시킨다.*/
        m_pMapPlayer->m_isPlayer_FloorSetting = false;

        /* 3. Map Player와 BackGround Center와의 거리를 구한다. */
       // offset.x = m_vBackGround_Center.x - vPlayertrans.x;
       // offset.y = m_vBackGround_Center.y - vPlayertrans.y;

        /* 4. 플레이어를 backGround 중심으로 옮김 */
        vPlayertrans.x = m_vBackGround_Center.x;
        vPlayertrans.y = m_vBackGround_Center.y;

        for (auto& iter : *pUIList)
        {
            CPlayer_Map_UI* pPlayer = dynamic_cast<CPlayer_Map_UI*>(iter);

            if (nullptr != pPlayer)
            {
                if (pPlayer->m_eMapComponent_Type == MAP_UI_TYPE::PLAYER_MAP)
                {
                    CTransform* pPlayerTrans = static_cast<CTransform*>(pPlayer->Get_Component(g_strTransformTag));
                    pPlayerTrans->Set_State(CTransform::STATE_POSITION, vPlayertrans);
                }
            }
        }

        /* 5. 각 객체(Main Map)들을 플레이어의 위치에서 구한 거리 만큼 이동시킨다 */
        _vector Player = XMVectorSet(m_vBackGround_Center.x, m_vBackGround_Center.y, 0.f, vPlayertrans.w);

        for (auto& iter : *pUIList)
        {
            CMain_Map_UI* pMapMain = dynamic_cast<CMain_Map_UI*>(iter);

            if (nullptr != pMapMain)
            {
                CTransform* pMapTrans = static_cast<CTransform*>(pMapMain->Get_Component(g_strTransformTag));

                /* Player에서 각자 잰 거리 까지의 위치를 해당 Main 객체에 넣어준다. */
                _float4 vMain_Result = (Player + *pMapMain->Player_Between_Distance());

                /* Z 값이 흐트러지지 않도록 정리 */
                vMain_Result.z = pMapTrans->Get_State_Float4(CTransform::STATE_POSITION).z;

                pMapTrans->Set_State(CTransform::STATE_POSITION, vMain_Result);

            }
        }

        /* 거리를 다시 재기 위한 m_isMainEnd를 꺼준다. */
        IsDistanceMeasured_Completely(false);
    }
}


CMap_Manager* CPlayer_Map_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayer_Map_UI* pInstance = new CPlayer_Map_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CPlayer_Map_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayer_Map_UI::Clone(void* pArg)
{
    CPlayer_Map_UI* pInstance = new CPlayer_Map_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CPlayer_Map_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_Map_UI::Free()
{
    __super::Free();
}
