#include "stdafx.h"
#include "Player_Map_UI.h"
#include "Main_Map_UI.h"
#include "CustomCollider.h"
#include "Player.h"
#include "Tab_Window.h"

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
    }

    /* 1. �θ��� ��� : �ʿ� ���� �� ���, �������� ���� ���̴�. */
    if (false == m_IsChild)
    {
        m_isRender = false;
        m_vColor[0].vColor = m_vCurrentColor = ALPHA_ZERO;
    }

    /* �ʼ� ��� */
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

    m_isMouse_Control = true;
    return S_OK;
}

void CMain_Map_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    Rendering();
    //Player_BetweenDistance();
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

/* ���� Ÿ���� Ȯ���ؼ� ������ ���Ѵ�. */
void CMain_Map_UI::Region_Type()
{
    if (nullptr == m_pPlayer)
        return;

    if (abs((int)m_eMap_Location) > 100)
        return;

    /* 1. ó�� Player�� �������� �� */
    if (MAP_STATE_TYPE::NONE_STATE == m_eMapState && true == m_pPlayer->Get_Player_Region_Array()[m_eMap_Location])
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
    /* Map Inventory�� ������ ���� ����� ���̴�.  */
    if (false == m_pTab_Window->Get_MinMapRender())
        m_isPrevRender = m_pTab_Window->Get_MinMapRender();

    /* �� Map Player�� Main�� ��� ��ü���� �Ÿ��� ���ϴ� �Լ�  */
    /* 1. Player�� �� ���� �ִ� �� Ȯ���ߴٸ�,  */
    if (true == *m_pMapPlayer->Get_PlayerFloorSetting())
    {
        /* 2. Map�� �״� �� Ȯ���ߴٸ�,  */
        if (m_isPrevRender != m_pTab_Window->Get_MinMapRender())
        {
            /* 3. �÷��̾�� ��ü�� �Ÿ��� ���Ѵ�  */
            _float4 vMainTrans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
            _float4 vPlayertrans = m_pMapPlayer_Transform->Get_State_Float4(CTransform::STATE_POSITION);

            _vector Main = XMVectorSet(vMainTrans.x, vMainTrans.y, vMainTrans.z, vMainTrans.w);
            _vector Player = XMVectorSet(vPlayertrans.x, vPlayertrans.y, vPlayertrans.z, vPlayertrans.w);

            /* �Ÿ��� ���� �� player�� �ش� ��ġ�������� ��������� �̵� */
            m_vMapOpen_Player_Distance = Main - Player;

            /* ��� ��ü���� �Ÿ��� ���� ���� Ȯ���ϱ� ���� ����� ���̴�. */
            m_isMainEnd = true;
        }
    }
}

void CMain_Map_UI::Player_BetweenDistance()
{
   /* Map Inventory�� ������ ���� ����� ���̴�.  */
   if (false == m_pTab_Window->Get_MinMapRender())
       m_isPrevRender = m_pTab_Window->Get_MinMapRender();

   /* �� Map Player�� Main�� ��� ��ü���� �Ÿ��� ���ϴ� �Լ�  */
   /* 1. Player�� �� ���� �ִ� �� Ȯ���ߴٸ�,  */
   if (true == *m_pMapPlayer->Get_PlayerFloorSetting())
   {
       /* 2. Map�� �״� �� Ȯ���ߴٸ�,  */
       if (m_isPrevRender != m_pTab_Window->Get_MinMapRender())
       {
           /* 3. �÷��̾�� ��ü�� �Ÿ��� ���Ѵ�  */
           _float4 vMainTrans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
           _float4 vPlayertrans = m_pMapPlayer_Transform->Get_State_Float4(CTransform::STATE_POSITION);

           _vector Main = XMVectorSet(vMainTrans.x, vMainTrans.y, vMainTrans.z, vMainTrans.w);
           _vector Player = XMVectorSet(vPlayertrans.x, vPlayertrans.y, vPlayertrans.z, vPlayertrans.w);

           /* �Ÿ��� ���� �� player�� �ش� ��ġ�������� ��������� �̵� */
           m_vMapOpen_Player_Distance = Main - Player;

           /* ��� ��ü���� �Ÿ��� ���� ���� Ȯ���ϱ� ���� ����� ���̴�. */
           m_isMainEnd = true;
       }
   }
}

void CMain_Map_UI::Change_Search_Type(MAP_STATE_TYPE _searType)
{
    if (MAP_STATE_TYPE::NONE_STATE == _searType) // �⺻
    {
        m_vCurrentColor = ALPHA_ZERO;
        m_fOrigin_Blending = 0.f;
    }

    else if (MAP_STATE_TYPE::SEARCH_STATE == _searType) // ���� ��
    {
        m_vCurrentColor = RED;
        m_fOrigin_Blending = BLENDING;
    }

    else if (MAP_STATE_TYPE::SEARCH_CLEAR_STATE == _searType) // ���� �Ϸ�
    {
        m_vCurrentColor = BLUE;
        m_fOrigin_Blending = BLENDING;
    }
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
