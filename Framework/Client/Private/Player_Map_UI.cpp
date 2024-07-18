#include "stdafx.h"
#include "Player_Map_UI.h"
#include "Static_Map_UI.h"
#include "Item_Map_UI.h"
#include "Main_Map_UI.h"
#include "Tab_Window.h"
#include "Player.h"

#define MINMAP_X_FLOOR1     1111.f
#define MINMAP_Y_FLOOR1     761.f

#define MINMAP_X_FLOOR2     1079.713f
#define MINMAP_Y_FLOOR2     585.407f

#define MINMAP_X_FLOOR3     925.068f
#define MINMAP_Y_FLOOR3     556.878


/* 2�� ũ��*/
#define MODELMAP_X_FLOO2     82.0431098f
#define MODELMAP_Y_FLOO2     38.3414335f

/* 3�� ũ��*/
#define MODELMAP_X_FLOO3     70.7840423f
#define MODELMAP_Y_FLOO3     38.0877256f

/* 1�� ũ��*/
#define MODELMAP_X_FLOO1     82.0969925f
#define MODELMAP_Y_FLOO1     60.4390125f

/* 2�� ũ��*/
#define MODELMAP_X_FLOO2      82.0431098f
#define MODELMAP_Y_FLOO2      38.3414335f

/* 3�� ũ��*/
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
    m_isPrevRender = true;

    if (nullptr != m_pPlayer)
    {
        m_pPlayerTransform = static_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));
    }

    return S_OK;
}

void CPlayer_Map_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    Open_Map();

    Rendering();

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
    /* 1. Map Inventory�� ų ���� Map Player�� �����Ѵ�. */
    if (nullptr != m_pTab_Window)
    {
        if (true == m_pTab_Window->Get_MinMapRender())
        {
            if (m_pMapPlayer->m_eCurrent_ViewFloor == m_ePlayer_Floor)
                m_isRender = true;

            else
            {
                m_isRender = false;
            }
        }
        else
        {
            /* Min Map�� ���� �� �ٸ� ���� Blending �Ǵ� ���� ������� �Ѵ�.*/
            m_isRender = false;

            m_fBlending = 0.f;

            m_pMapPlayer->m_eCurrent_ViewFloor = m_ePlayer_Floor;

        }
    }

    Map_Player_Setting();
}

/* Map Player�� Main Map ��ü���� �Ÿ��� ��� ���� ���� ���´���? */
_bool CPlayer_Map_UI::IsDistanceMeasured_Completely(_bool _find)
{
    list<CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

    for (auto& iter : *pUIList)
    {
        CMain_Map_UI* pMapMain = dynamic_cast<CMain_Map_UI*>(iter);
        CItem_Map_UI* pMapItem = dynamic_cast<CItem_Map_UI*>(iter);

        if (nullptr != pMapMain)
        {
            if (true == _find) /* �Ÿ� ��� �� */
            {
                if (*pMapMain->Distance_End_Ptr() == false)
                    return false;
            }
            else
            {
                *pMapMain->Distance_End_Ptr() = false;
            }
        }

        else if (nullptr != pMapItem)
        {
            if (true == _find) /* �Ÿ� ��� �� */
            {
                if (*pMapItem->Distance_ItemEnd() == false)
                    return false;
            }
            else
            {
                *pMapItem->Distance_ItemEnd() = false;
            }
        }
    }

    return true;
}


void CPlayer_Map_UI::Open_Map()
{
    /* �� ó�� ���� ������ �� �÷��̾��� ��ġ�� Floor�� �����ؾ� �Ѵ�. */
    if (m_isPrevRender != m_pTab_Window->Get_Dead() || false == m_pTab_Window->Get_MinMapRender())
    {
        if (true == m_isPrevRender)
        {
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_fOriginPos);
        }

        /* 1. ���� Player�� ��� �ִ� Floor�� Render �� ���̴�. */
        m_pMapPlayer->m_eCurrent_ViewFloor = static_cast<MAP_FLOOR_TYPE>(m_pPlayer->Get_Player_Floor());
        m_ePlayer_Floor = m_pMapPlayer->m_eCurrent_ViewFloor;

        /* 2. Map�� �� ������ ���� �÷��̾ �����ϴ� �÷ξ��� ũ�⸦ ������ �� */
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
        m_isPrevRender = m_pTab_Window->Get_Dead();
    }

    if (true == m_pTab_Window->Get_Dead())
    {
        m_fOriginPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
    }
}

void CPlayer_Map_UI::Map_Player_Control(_float fTimeDelta)
{
    if (true == m_isRender)
    {
        ///////////  :   �� ����
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

        if (true == m_IsChild)
        {
            _float4 mapPlayer = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
            _float4 vPlayerPos = m_pMapPlayer_Transform->Get_State_Float4(CTransform::STATE_POSITION);
            mapPlayer.x = vPlayerPos.x;
            mapPlayer.y = vPlayerPos.y;
            mapPlayer.z = vPlayerPos.z;
            mapPlayer.w = vPlayerPos.w;

            m_pTransformCom->Set_State(CTransform::STATE_POSITION, mapPlayer);
        }
      
        return;
    }

    else if (false == m_isRender)
    {
        _matrix playerMatrix = m_pPlayerTransform->Get_WorldMatrix();

        ///////////  :   ȸ�� 
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

           ///////////  :   ���� ��
        m_pTransformCom->Set_WorldMatrix(mapPlayer);

        if (false == m_IsChild)
            m_pTransformCom->Set_Scaled(55.f, 55.f, 1);

        else if (true == m_IsChild)
            m_pTransformCom->Set_Scaled(25.f, 25.f, 1);

        ///////////  :   �̵�
        Map_Player_Moving(fTimeDelta);

        if (true == m_IsChild)
        {
            _float4 mapPlayer = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
            _float4 vPlayerPos = m_pMapPlayer_Transform->Get_State_Float4(CTransform::STATE_POSITION);
            mapPlayer.x = vPlayerPos.x;
            mapPlayer.y = vPlayerPos.y;
            mapPlayer.z = vPlayerPos.z;
            mapPlayer.w = vPlayerPos.w;

            m_pTransformCom->Set_State(CTransform::STATE_POSITION, mapPlayer);
        }
    }
}

void CPlayer_Map_UI::Map_Player_Moving(_float fTimeDelta)
{
    _float2 fComparison = {};
    _float2 Moving_Value = {};

    // MINMAP�� MODELMAP�� ũ�� ���� ���

    if(MODELMAP_X_FLOO1 == m_fCurrent_ModelScaled.x && MODELMAP_Y_FLOO1 == m_fCurrent_ModelScaled.y)
    {
        fComparison.x = (MINMAP_X_FLOOR1) / m_fCurrent_ModelScaled.x;
        fComparison.y = (MINMAP_Y_FLOOR1) / m_fCurrent_ModelScaled.y;

        m_vPlayer_MovePos = m_pPlayerTransform->Get_State_Float4(CTransform::STATE_POSITION);
        Moving_Value.x = -(m_vPlayer_MovePos.x + 2.f);
        Moving_Value.y = -(m_vPlayer_MovePos.z + 13.f); // �÷��̾��� �ʱ� ��ġ�� ���� ��ġ�� ���� ���
    }

    else if (MODELMAP_X_FLOO2 == m_fCurrent_ModelScaled.x && MODELMAP_Y_FLOO2 == m_fCurrent_ModelScaled.y)
    {
        fComparison.x = (MINMAP_X_FLOOR2) / m_fCurrent_ModelScaled.x;
        fComparison.y = (MINMAP_Y_FLOOR2) / m_fCurrent_ModelScaled.y;

        m_vPlayer_MovePos = m_pPlayerTransform->Get_State_Float4(CTransform::STATE_POSITION);
        Moving_Value.x = -(m_vPlayer_MovePos.x + 1.15f);
        Moving_Value.y = -(m_vPlayer_MovePos.z + 15.5f); // �÷��̾��� �ʱ� ��ġ�� ���� ��ġ�� ���� ���
    }

    else if (MODELMAP_X_FLOO3 == m_fCurrent_ModelScaled.x && MODELMAP_Y_FLOO3 == m_fCurrent_ModelScaled.y)
    {
        fComparison.x = (_float)(MINMAP_X_FLOOR3) / m_fCurrent_ModelScaled.x;
        fComparison.y = (_float)(MINMAP_Y_FLOOR3) / m_fCurrent_ModelScaled.y;

        m_vPlayer_MovePos = m_pPlayerTransform->Get_State_Float4(CTransform::STATE_POSITION);
        Moving_Value.x = -(m_vPlayer_MovePos.x + 2.f);
        Moving_Value.y = -(m_vPlayer_MovePos.z + 16.f); // �÷��̾��� �ʱ� ��ġ�� ���� ��ġ�� ���� ���
    }
    //offset.x /= fComparison.x;
    //offset.y /= fComparison.y;

    //Moving_Value.x = 0.f - m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).x;
    //Moving_Value.y = 0.f - m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).z; // �÷��̾��� �ʱ� ��ġ�� ���� ��ġ�� ���� ���


    // MINMAP ������ ���� �̵� ���� ����
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
    /* �� Map Player <-> Main Map �Ÿ� ��� �Ϸ� �ÿ�  */
    if (true == IsDistanceMeasured_Completely(true) || m_eCurrent_ViewFloor != m_ePrevCurrent_ViewFloor)
    {
        m_ePrevCurrent_ViewFloor = m_eCurrent_ViewFloor;
        /* 1. Prev Render �� ���� ������ �־��ش� */
        m_isPrevRender = m_pTab_Window->Get_Dead();

        list<CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));
        _float4 vPlayertrans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

        /* 2. Player Pos : �� ���� �ִ� �� Ȯ�� �ߴ� ���� ���� ���� : false ��Ų��.*/
        m_pMapPlayer->m_isPlayer_FloorSetting = false;

        /* 4. �÷��̾ backGround �߽����� �ű� */
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

        /* 5. �� ��ü(Main Map)���� �÷��̾��� ��ġ���� ���� �Ÿ� ��ŭ �̵���Ų�� */
        _vector Player = XMVectorSet(m_vBackGround_Center.x, m_vBackGround_Center.y, 0.f, vPlayertrans.w);

        for (auto& iter : *pUIList)
        {
            CMain_Map_UI* pMapMain = dynamic_cast<CMain_Map_UI*>(iter);
            CItem_Map_UI* pMapItem = dynamic_cast<CItem_Map_UI*>(iter);

            if (nullptr != pMapMain)
            {
                CTransform* pMapTrans = static_cast<CTransform*>(pMapMain->Get_Component(g_strTransformTag));

                /* Player���� ���� �� �Ÿ� ������ ��ġ�� �ش� Main ��ü�� �־��ش�. */
                _float4 vMain_Result = (Player + *pMapMain->Player_Between_Distance_Ptr());

                /* Z ���� ��Ʈ������ �ʵ��� ���� */
                vMain_Result.z = pMapTrans->Get_State_Float4(CTransform::STATE_POSITION).z;

                pMapTrans->Set_State(CTransform::STATE_POSITION, vMain_Result);
            }

            if (nullptr != pMapItem)
            {
                CTransform* pItemTrans = static_cast<CTransform*>(pMapItem->Get_Component(g_strTransformTag));

                /* Player���� ���� �� �Ÿ� ������ ��ġ�� �ش� Main ��ü�� �־��ش�. */
                _float4 vMain_Result = (Player + *pMapItem->Player_Between_Item_Distance());

                /* Z ���� ��Ʈ������ �ʵ��� ���� */
                vMain_Result.z = pItemTrans->Get_State_Float4(CTransform::STATE_POSITION).z;

                pItemTrans->Set_State(CTransform::STATE_POSITION, vMain_Result);
            }
        }

        /* �Ÿ��� �ٽ� ��� ���� m_isMainEnd�� ���ش�. */
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