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
        Find_DoorObj();

    Rendering();
    Player_BetweenDistance();

    /* Door : Object가 없을 시 터질 수 있음, Noting 하려면 이거 주석하셈*/
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
    /* 1. 부모일 경우 : 필요 없는 뒷 배경, 렌더하지 않을 것이다. */
    if (false == m_IsChild)
    {
        m_isRender = false;
        m_vColor[0].vColor = m_vCurrentColor = ALPHA_ZERO;
    }

    /* 필수 요소 */
    if (m_iWhichChild == (_uint)MAP_CHILD_TYPE::BACKGROUND_MAP)
    {
        m_isPlay = false;
        m_vColor[0].vColor = m_vCurrentColor = ALPHA_ZERO;
        m_vColor[0].fBlender_Value = m_fBlending = 1.f;
        m_vColor[0].isBlender = m_isBlending = true;
    }

    if (m_iWhichChild == (_uint)MAP_CHILD_TYPE::LINE_MAP)
    {
        m_vColor[0].vColor = m_vCurrentColor = _float4(0.f, 0.f, 0.f, 0);
    }

    return S_OK;
}

/* 지역 타입을 확인해서 색깔을 정한다. */
void CMain_Map_UI::Region_State()
{
    if (true == m_isEnd_OnesRole)
        return;

    if (nullptr == m_pPlayer)
        return;

    if (abs((int)m_eMap_Location) > 100)
        return;

    /* 2. 아이템을 전부 먹었을 때  */
    /* 맵이 수색 중이고 아이템을 전부 먹었을 때 */
    if (true == m_isMapSearch_Clear && MAP_STATE_TYPE::SEARCH_STATE == m_eMapState)
    {
        Search_Map_Type(MAP_STATE_TYPE::SEARCH_CLEAR_STATE, m_eMap_Location);

        m_isEnd_OnesRole = true;
    }

    /* 1. 처음 Player가 입장했을 때 */
    else if (MAP_STATE_TYPE::NONE_STATE == m_eMapState && true == m_pPlayer->Get_Player_Region_Array()[m_eMap_Location])
    {
        Search_Map_Type(MAP_STATE_TYPE::SEARCH_STATE, m_eMap_Location);
    }
}

void CMain_Map_UI::Search_Map_Type(MAP_STATE_TYPE _searType, LOCATION_MAP_VISIT _mapType)
{
    if (_mapType == m_eMap_Location && (MAP_CHILD_TYPE::BACKGROUND_MAP == (MAP_CHILD_TYPE)m_iWhichChild && m_eMapComponent_Type == MAP_UI_TYPE::MAIN_MAP))
    {
        Change_Search_Type(_searType);
    }
}

void CMain_Map_UI::Rendering()
{	
    /* Inventory를 오픈할 때만 사용할 것이다.  */
    if (false == m_pTab_Window->Get_MinMapRender())
    {
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vOriginPos);
        m_isPrevRender = m_pTab_Window->Get_MinMapRender();
    }

    /* ▶ Map Player와 Main의 모듣 객체와의 거리를 구하는 함수  */
    /* 1. Player가 몇 층에 있는 지 확인했다면,  */
    if (true == *m_pMapPlayer->Get_PlayerFloorSetting())
    {
        /* 2. Map을 켰는 지 확인했다면,  */
        if (m_isPrevRender != m_pTab_Window->Get_MinMapRender())
        {
            /* 3. 플레이어와 객체의 거리를 구한다  */
            _float4 vMainTrans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
            _float4 vPlayertrans = m_pMapPlayer_Transform->Get_State_Float4(CTransform::STATE_POSITION);

            _vector Main = XMVectorSet(vMainTrans.x, vMainTrans.y, vMainTrans.z, vMainTrans.w);
            _vector Player = XMVectorSet(vPlayertrans.x, vPlayertrans.y, vPlayertrans.z, vPlayertrans.w);

            /* 거리를 구할 때 player는 해당 위치에서부터 자유자재로 이동 */
            m_vMapOpen_Player_Distance = Main - Player;

            /* 모든 객체와의 거리를 구한 것을 확인하기 위해 사용할 것이다. */
            m_isMainEnd = true;
        }
    }
}

void CMain_Map_UI::Player_BetweenDistance()
{
   /* Map Inventory를 오픈할 때만 사용할 것이다.  */
   if (true == m_pTab_Window->Get_Dead())
       m_isPrevRender = m_pTab_Window->Get_Dead();

   /* ▶ Map Player와 Main의 모듣 객체와의 거리를 구하는 함수  */
   /* 1. Player가 몇 층에 있는 지 확인했다면,  */
   if (true == *m_pMapPlayer->Get_PlayerFloorSetting())
   {
       /* 2. Map을 켰는 지 확인했다면, 내가 존재했던 view와 과거 view가 같지 않다면*/
       if (m_isPrevRender != m_pTab_Window->Get_Dead() || *m_pMapPlayer->Get_ViewFloor_Type() != m_ePrevViewFloor)
       {
           m_ePrevViewFloor = *m_pMapPlayer->Get_ViewFloor_Type();

           /* 3. 플레이어와 객체의 거리를 구한다  */
           _float4 vMainTrans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
           _float4 vPlayertrans = m_pMapPlayer_Transform->Get_State_Float4(CTransform::STATE_POSITION);

           _vector Main = XMVectorSet(vMainTrans.x, vMainTrans.y, vMainTrans.z, vMainTrans.w);
           _vector Player = XMVectorSet(vPlayertrans.x, vPlayertrans.y, vPlayertrans.z, vPlayertrans.w);

           /* 거리를 구할 때 player는 해당 위치에서부터 자유자재로 이동 */
           m_vMapOpen_Player_Distance = Main - Player;

           /* 모든 객체와의 거리를 구0한0 것을0 확인하기 위해 사용할 것이다. */
           m_isMainEnd = true;
       }
   }
}

void CMain_Map_UI::Door_State()
{
    if (MAP_UI_TYPE::DOOR_MAP == m_eMapComponent_Type)
    {
        if (m_DoorList.empty())
        {
            // MSG_BOX(TEXT("Main Map UI() : Door Map이 List를 찾을 수 없습니다."));
            return;
        }

        /* Player가 어떤 무언가에 접촉했을 때만 돌길 원함 */
        for (auto& iter : m_DoorList)
        {
            if(m_eDoor_Type == (DOOR_TYPE)iter->Get_PropType())
            {
                /* 접촉만 했을 때 */
                if (true == iter->Get_FirstInteract())
                {
                    if (true == iter->Get_Interact())/* 문을 열었는가*/
                    {
                        Search_Door_Type(MAP_STATE_TYPE::SEARCH_STATE, (DOOR_TYPE)iter->Get_PropType());
                    }

                    else if (false == iter->Get_Interact()) /* 열지 않았는가*/
                    {
                        Search_Door_Type(MAP_STATE_TYPE::SEARCH_CLEAR_STATE, (DOOR_TYPE)iter->Get_PropType());
                    }

                    m_isBackColor_Change = true; /* Black에 관련된 color만 변경 */
                }

                /* 접촉하지 않았을 때 */
                else if (false == iter->Get_FirstInteract())
                {
                    Search_Door_Type(MAP_STATE_TYPE::NONE_STATE, (DOOR_TYPE)iter->Get_PropType());
                }
            }
        }
    }
}

void CMain_Map_UI::Search_Door_Type(MAP_STATE_TYPE _searType, DOOR_TYPE _searchDoor)
{
    Change_Search_Type(_searType);
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

void CMain_Map_UI::Change_Search_Type(MAP_STATE_TYPE _searType)
{
    if (MAP_STATE_TYPE::NONE_STATE == _searType) // 기본
    {
        m_vCurrentColor = ALPHA_ZERO;
        m_fOrigin_Blending = 0.f;

    }

    else if (MAP_STATE_TYPE::SEARCH_STATE == _searType) // 수색 중
    {
        m_vCurrentColor = RED;
        m_fOrigin_Blending = BLENDING;
    }

    else if (MAP_STATE_TYPE::SEARCH_CLEAR_STATE == _searType) // 수색 완료
    {
        m_vCurrentColor = BLUE;
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
