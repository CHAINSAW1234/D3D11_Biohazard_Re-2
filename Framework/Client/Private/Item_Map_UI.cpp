#include "stdafx.h"
#include "Item_Map_UI.h"
#include "Player.h"
#include "Tab_Window.h"
#include "Player_Map_UI.h"

#define INCIDENCE   997
#define STORE_BOX   998
#define OTHER       999
#define LADDER      995

CItem_Map_UI::CItem_Map_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMap_Manager{ pDevice, pContext }
{
}

CItem_Map_UI::CItem_Map_UI(const CItem_Map_UI& rhs)
    : CMap_Manager{ rhs }
{

}

HRESULT CItem_Map_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CItem_Map_UI::Initialize(void* pArg)
{
    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;

        m_eItem_Type = CustomUIDesc->eItem_Number;
    }

    m_isMouse_Control = true;

    Item_Name_Selection(); /* 아이템 및 사건 파일 모음 */

    return S_OK;
}

void CItem_Map_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    Rendering();
    Player_BetweenDistance();
}

void CItem_Map_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CItem_Map_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

/* 몇 층에 어떤 지역의 어떤 아이템을 삭제할 것인가? */
void CItem_Map_UI::Destory_Item(MAP_FLOOR_TYPE _floorType, LOCATION_MAP_VISIT _locationType, ITEM_NUMBER _ItemType)
{
    CGameObject* pItem = Search_Item(_floorType, _locationType, _ItemType);

    if (nullptr == pItem)
        MSG_BOX(TEXT("나옹이가 아직 지정하지 않은 ENUM이거나 안 되는 READ ITEM 입니당.")); // 경관의 수첩은 잠시 막아놓음

    else
    {
        CItem_Map_UI* pMapItem = static_cast<CItem_Map_UI*>(pItem);
        pMapItem->m_isRender = false;
        *pItem->Get_Dead_Ptr() = true;
    }
}

void CItem_Map_UI::Rendering()
{
    /* 과거에 온 지역이랑 현재의 지역이 맞지 않는다면.*/
    if (m_ePrevRegion != (LOCATION_MAP_VISIT)m_pPlayer->Get_Player_Region())
    {
        /* 과거의 지역에 현재의 지역을 집어넣고*/
        m_ePrevRegion = (LOCATION_MAP_VISIT)m_pPlayer->Get_Player_Region();

        /* 렌더 상태가 아닐 때, 현재의 지역과 아이템대상의 지역이 맞다면, */
        if (false == m_isItemRender && (LOCATION_MAP_VISIT)m_pPlayer->Get_Player_Region() == m_eMap_Location)
            m_isItemRender = true;

        if (INCIDENCE == m_eItem_Type || STORE_BOX == m_eItem_Type || OTHER == m_eItem_Type)
            m_isItemRender = true;
    }

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
            m_isItemEnd = true;
        }
    }

    if (false == m_pTab_Window->Get_MinMapRender())
        return;

    if (true == m_isItemRender)
    {
        if (true != m_bDead)
            m_isRender = true;
       else
            m_isRender = false;
    }

    else if (false == m_isItemRender)
        m_isRender = false;
}

void CItem_Map_UI::Player_BetweenDistance()
{
    /* Map Inventory를 오픈할 때만 사용할 것이다.  */
    if (false == m_pTab_Window->Get_MinMapRender())
        m_isPrevRender = m_pTab_Window->Get_MinMapRender();

    /* ▶ Map Player와 Main의 모듣 객체와의 거리를 구하는 함수  */
    /* 1. Player가 몇 층에 있는 지 확인했다면,  */
    if (true == *m_pMapPlayer->Get_PlayerFloorSetting())
    {
        /* 2. Map을 켰는 지 확인했다면, 내가 존재했던 view와 과거 view가 같지 않다면*/
        if (m_isPrevRender != m_pTab_Window->Get_MinMapRender() || *m_pMapPlayer->Get_ViewFloor_Type() != m_ePrevViewFloor)
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
            m_isItemEnd = true;
        }
    }
}

void CItem_Map_UI::Item_Name_Selection()
{
    switch ((_int)m_eItem_Type)
    {
    case ITEM_NUMBER::emergencyspray01a :
        m_wstrItemNumber = TEXT("스프레이");
        break;

    case ITEM_NUMBER::greenherb01a :
        m_wstrItemNumber = TEXT("그린 허브");
        break;

    case ITEM_NUMBER::redherb01a :
        m_wstrItemNumber = TEXT("레드 허브");
        break;

    case ITEM_NUMBER::blueherb01a:
        m_wstrItemNumber = TEXT("블루 허브");
        break;

    case ITEM_NUMBER::handgun_bullet01a:
        m_wstrItemNumber = TEXT("핸드건 총알");
        break;

    case ITEM_NUMBER::shotgun_bullet01a :
        m_wstrItemNumber = TEXT("샷건 총알");
        break;

    case ITEM_NUMBER::woodbarricade01a:
        m_wstrItemNumber = TEXT("나무 바리게이트");
        break;

    case ITEM_NUMBER::blastingfuse01a:
        m_wstrItemNumber = TEXT("발파용 퓨즈");
        break;

    case ITEM_NUMBER::_9vbattery01a:
        m_wstrItemNumber = TEXT("배터리");
        break;

    case ITEM_NUMBER::gunpowder01a:
        m_wstrItemNumber = TEXT("화약");
        break;

    case ITEM_NUMBER::gunpowder01b:
        m_wstrItemNumber = TEXT("화약");
        break;

    case ITEM_NUMBER::strengtheningyellow01a:
        m_wstrItemNumber = TEXT("화약");
        break;

    case LADDER :
        m_wstrItemNumber = TEXT("사다리");
        break;

    case INCIDENCE:
    {
        m_wstrItemNumber = TEXT("사건");

        /* 사건에 따라 분류할 예정*/
    }
    break;

    case STORE_BOX:
        m_wstrItemNumber = TEXT("저장고");
        break;

    case OTHER:
        m_wstrItemNumber = TEXT("힌트");
        break;
    }


}

/* 찾고자 하는 아이템 */
CGameObject* CItem_Map_UI::Search_Item(MAP_FLOOR_TYPE _floorType, LOCATION_MAP_VISIT _locationType, ITEM_NUMBER _ItemType)
{
    list<CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

    for (auto& iter : *pUIList)
    {
        CItem_Map_UI* pUI = dynamic_cast<CItem_Map_UI*>(iter);

        if (nullptr != pUI)
        {
            if (_ItemType == pUI->m_eItem_Type)
            {
                if (_locationType == pUI->m_eMap_Location)
                {
                    if (_floorType == pUI->m_eFloorType)
                        return pUI;
                }
            }
        }
    }

    return nullptr;
}

CMap_Manager* CItem_Map_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CItem_Map_UI* pInstance = new CItem_Map_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CItem_Map_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CItem_Map_UI::Clone(void* pArg)
{
    CItem_Map_UI* pInstance = new CItem_Map_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CItem_Map_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CItem_Map_UI::Free()
{
    __super::Free();
}
