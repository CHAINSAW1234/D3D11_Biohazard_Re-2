#include "stdafx.h"
#include "Player_Map_UI.h"
#include "Main_Map_UI.h"
#include "Tab_Window.h"

#include "CustomCollider.h"
#include "Player.h"
#include "Door.h"

#define ALPHA_ZERO              _float4(0.0f, 0.0f, 0.0f, 0.0f)
#define RED                     _float4(0.8f, 0.f, 0.f, 0.f)
#define BLUE                    _float4(0.0f, 0.7569f, 0.85f, 0.0f)
#define BLENDING                0.7f

CMain_Map_UI::CMain_Map_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMap_Manager{ pDevice, pContext }
{
}

CMain_Map_UI::CMain_Map_UI(const CMain_Map_UI& rhs)
    : CMap_Manager{ rhs }
{

}

HRESULT CMain_Map_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMain_Map_UI::Initialize(void* pArg)
{
    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;

        m_iWhichChild = CustomUIDesc->iWhich_Child;

        if (m_eDoor_Type > DOOR_TYPE::END_DOOR)
            m_eDoor_Type = DOOR_TYPE::END_DOOR;

        m_eDoor_Type = CustomUIDesc->eDoor_Type;
    }

    if (MAP_UI_TYPE::DOOR_MAP == m_eMapComponent_Type)
    {
        Find_DoorObj();
    }

    if (FAILED(Change_Tool()))
        return E_FAIL;

    m_isMouse_Control = true;

    m_vOriginPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

    return S_OK;
}

void CMain_Map_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (m_DoorList.empty())
    {
        Find_DoorObj();
    }

    Rendering();

    Player_BetweenDistance();

    Region_State();

    Door_State();
}

void CMain_Map_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CMain_Map_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMain_Map_UI::Change_Tool()
{
    /* 부모일 경우 : 필요 없는 뒷 배경*/
    if (false == m_IsChild)
    {
        m_isRender = false;
        m_vColor[0].vColor = m_vCurrentColor = ALPHA_ZERO;
    }

    if (m_iWhichChild == static_cast<_uint>(MAP_CHILD_TYPE::BACKGROUND_MAP))
    {
        m_isPlay = false;
        m_vColor[0].vColor = m_vCurrentColor = ALPHA_ZERO;
        m_vColor[0].fBlender_Value = m_fBlending = 1.f;
        m_vColor[0].isBlender = m_isBlending = true;
    }

    if (m_iWhichChild == static_cast<_uint>(MAP_CHILD_TYPE::LINE_MAP))
    {
        m_vColor[0].vColor = m_vCurrentColor = _float4(0.f, 0.f, 0.f, 0);
    }

    return S_OK;
}

void CMain_Map_UI::Region_State()
{
    if (nullptr == m_pPlayer)
        return;

    if (abs(static_cast<_int>(m_eMap_Location)) > 100)
        return;

    /* 2. 아이템을 전부 먹었을 때  */
    if (false == m_isEnd_OnesRole && true == m_isMapSearch_Clear/* && MAP_STATE_TYPE::SEARCH_STATE == m_eMapState*/)
    {
        Search_Map_Type(MAP_STATE_TYPE::SEARCH_CLEAR_STATE, m_eMap_Location);

        m_isEnd_OnesRole = true;
    }

    else if (MAP_STATE_TYPE::NONE_STATE == m_eMapState && true == m_pPlayer->Get_Player_Region_Array()[m_eMap_Location])
    {
        Search_Map_Type(MAP_STATE_TYPE::SEARCH_STATE, m_eMap_Location);
    }
}

void CMain_Map_UI::Search_Map_Type(MAP_STATE_TYPE _searType, LOCATION_MAP_VISIT _mapType)
{
    if (_mapType == m_eMap_Location && 
        (MAP_CHILD_TYPE::BACKGROUND_MAP == static_cast<MAP_CHILD_TYPE>(m_iWhichChild) && m_eMapComponent_Type == MAP_UI_TYPE::MAIN_MAP))
    {
        Change_Search_Type(_searType, false);
    }
}

void CMain_Map_UI::Rendering()
{	
    if (false == m_pTab_Window->Get_MinMapRender())
    {
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vOriginPos);

        m_isPrevRender = m_pTab_Window->Get_MinMapRender();
    }

    /* ▶ Map Player와 Main의 모든 객체와의 거리 */
    if (true == *m_pMapPlayer->Get_PlayerFloorSetting())
    {
        if (m_isPrevRender != m_pTab_Window->Get_MinMapRender())
        {
            _float4 vMainTrans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
            _float4 vPlayertrans = m_pMapPlayer_Transform->Get_State_Float4(CTransform::STATE_POSITION);

            _vector Main = XMVectorSet(vMainTrans.x, vMainTrans.y, vMainTrans.z, vMainTrans.w);
            _vector Player = XMVectorSet(vPlayertrans.x, vPlayertrans.y, vPlayertrans.z, vPlayertrans.w);

            /* 여기서 뭔가를 해야할까? */
            m_vMapOpen_Player_Distance = Main - Player;

            m_vMapOpen_Player_Distance.m128_f32[0] = m_vMapOpen_Player_Distance.m128_f32[0] * ((_float)g_iWinSizeX / 1600.f);
            m_vMapOpen_Player_Distance.m128_f32[1] = m_vMapOpen_Player_Distance.m128_f32[1] * ((_float)g_iWinSizeY / 900.f);

            m_isMainEnd = true;
        }
    }
}

void CMain_Map_UI::Player_BetweenDistance()
{
   if (true == m_pTab_Window->Get_Dead())
       m_isPrevRender = m_pTab_Window->Get_Dead();

   /* ▶ Player와 Main의 모든 객체와의 거리  */
   if (true == *m_pMapPlayer->Get_PlayerFloorSetting())
   {
       if (m_isPrevRender != m_pTab_Window->Get_Dead() || *m_pMapPlayer->Get_ViewFloor_Type() != m_ePrevViewFloor)
       {
           m_ePrevViewFloor = *m_pMapPlayer->Get_ViewFloor_Type();

           _float4 vMainTrans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
           _float4 vPlayertrans = m_pMapPlayer_Transform->Get_State_Float4(CTransform::STATE_POSITION);

           _vector Main = XMVectorSet(vMainTrans.x, vMainTrans.y, vMainTrans.z, vMainTrans.w);
           _vector Player = XMVectorSet(vPlayertrans.x, vPlayertrans.y, vPlayertrans.z, vPlayertrans.w);

           m_vMapOpen_Player_Distance = Main - Player;

           m_isMainEnd = true;
       }
   }
}

void CMain_Map_UI::Door_State()
{
    if (MAP_UI_TYPE::DOOR_MAP == m_eMapComponent_Type)
    {
        /* Player가 어떤 무언가에 접촉했을 때만 돌길 원함 */
        for (auto& iter : m_DoorList)
        {
            if (m_eDoor_Type == static_cast<DOOR_TYPE>(iter->Get_PropType()))
            {
                /* 접촉만 했을 때 */
                if (true == iter->Get_FirstInteract())
                {
                    if (static_cast<DOOR_TYPE>(iter->Get_PropType() == DOOR_TYPE::ENTRANCE_UP_DOOR))
                    {
                        m_pPlayer->Set_MissionEntrance(true);
                    }

                    if (true == iter->Get_Interact())/* 문을 열었는가*/
                    {
                        Search_Door_Type(MAP_STATE_TYPE::SEARCH_CLEAR_STATE, static_cast<DOOR_TYPE>(iter->Get_PropType()));

                        m_isBlending = false;
                    }

                    else if (false == iter->Get_Interact()) 
                    {
                        Search_Door_Type(MAP_STATE_TYPE::SEARCH_STATE, static_cast<DOOR_TYPE>(iter->Get_PropType()));

                        m_isBlending = false;
                    }
                    // m_isBackColor_Change = true; /* Black에 관련된 color만 변경 */
                }

                /* 접촉하지 않았을 때 */
                else if (false == iter->Get_FirstInteract())
                {
                    Search_Door_Type(MAP_STATE_TYPE::NONE_STATE, static_cast<DOOR_TYPE>(iter->Get_PropType()));
                }
            }
        }
    }
}

void CMain_Map_UI::Search_Door_Type(MAP_STATE_TYPE _searType, DOOR_TYPE _searchDoor)
{
    Change_Search_Type(_searType, true);
}

void CMain_Map_UI::Find_DoorObj()
{
    list<CGameObject*>* pDoorList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_Door"));

    if (nullptr == pDoorList)
        return;

    for (auto& iter : *pDoorList)
    {
        CDoor* pDoor = static_cast<CDoor*>(iter);

        m_DoorList.push_back(pDoor);
    }
}

void CMain_Map_UI::Change_Search_Type(MAP_STATE_TYPE _searType, _bool isDoor)
{
    if (MAP_STATE_TYPE::NONE_STATE == _searType) // 기본
    {
        m_vCurrentColor = ALPHA_ZERO;

        m_fOrigin_Blending = 0.f;
    }

    else if (MAP_STATE_TYPE::SEARCH_STATE == _searType) // 수색 중
    {
        if (false == isDoor)
            m_vCurrentColor = RED;

        else if (true == isDoor)
            m_vCurrentColor = _float4(0.4f, 0.15f, 0.05f, 1.f);

        m_fOrigin_Blending = BLENDING;
    }

    else if (MAP_STATE_TYPE::SEARCH_CLEAR_STATE == _searType) // 수색 완료
    {
        if (false == isDoor)
            m_vCurrentColor = BLUE;

        else if (true == isDoor)
            m_vCurrentColor = _float4(0.2f, 0.2f, 0.35f, 1.f);
        
        m_fOrigin_Blending = BLENDING;
    }

    m_eMapState = _searType;
}


CMap_Manager* CMain_Map_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMain_Map_UI* pInstance = new CMain_Map_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CMain_Map_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMain_Map_UI::Clone(void* pArg)
{
    CMain_Map_UI* pInstance = new CMain_Map_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CMain_Map_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMain_Map_UI::Free()
{
    __super::Free();
}
