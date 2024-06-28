#include "stdafx.h"
#include "Map_UI.h"
#include "Tab_Window.h"
#include "Player.h"

#define RED                     _float4(0.8, 0, 0, 0)
#define BLUE                    _float4(0.0, 0.7569, 0.85, 0.0)
#define ALPHA_ZERO              _float4(0, 0, 0, 0)

#define BLENDING                0.7f
#define BLENDING_SPEED          5.f /* ���� ���� �ٲ� �� ���� �ӵ� ���� ��*/

#define TARGET_SPEED           10.f
#define ZERO                    0.f

#define MINMAP_X_SCALED       1011.f
#define MINMAP_Y_SCALED       743.f

/* 1�� ũ��*/
#define MODELMAP_X_SCALED     82.0969925
#define MODELMAP_Y_SCALED     60.4390125   

/* 2�� ũ��*/
#define MODELMAP_X_SCALED     82.0969925
#define MODELMAP_Y_SCALED     60.4390125  

/* 3�� ũ��*/
#define MODELMAP_X_SCALED     82.0969925
#define MODELMAP_Y_SCALED     60.4390125  

#define FLOOR_TYPE_BLENDING   0.244f /* Floor ���� �� ������ ��*/

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
        m_wstrFile = CustomUIDesc->wstrFileName;

        Find_MapStateType(CustomUIDesc);
    }

    /* �θ�� �������� ���� ����*/
    if(MAP_UI_TYPE::MAIN_MAP == m_eMapComponent_Type)
    {
        if (false == m_IsChild)
        {
            m_isRender = false;
            m_vColor[0].vColor = m_vCurrentColor = _float4(0, 0, 0, 0);
        }

        /* �ʼ� ��� */
        if (m_iWhichChild == (_uint)MAP_CHILD_TYPE::BACKGROUND_MAP)
        {
            m_isPlay = false;
            m_vColor[0].vColor = m_vCurrentColor = ALPHA_ZERO;
            m_vColor[0].fBlender_Value = m_fBlending = 1.f;
            m_vColor[0].isBlender = m_isBlending = true;
        }
    }

    else if (MAP_UI_TYPE::MASK_MAP == m_eMapComponent_Type)
    {
        m_vColor[0].vColor = m_vCurrentColor = _float4(0, 0, 0, 0);
    }

    else if (MAP_UI_TYPE::PLAYER_MAP == m_eMapComponent_Type)
    {
        Find_Player();

        if (nullptr != m_pPlayer)
        {
            m_pPlayerTransform = static_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));
            // Safe_AddRef<CTransform*>(m_pPlayerTransform);
        }

        m_vPlayer_MovePos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
        
        m_vPlayer_InitPos.x = 0.f + m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).x;
        m_vPlayer_InitPos.y = 0.f + m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).y;
    }

    else if (MAP_UI_TYPE::SEARCH_TYPE_MAP == m_eMapComponent_Type)
    {
        _float4 vTransform = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

        vTransform.x += 100.f;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTransform);
    }
        
    else if (MAP_UI_TYPE::TARGET_MAP == m_eMapComponent_Type)
    {
        if (true == m_IsChild)
        {
            list<CGameObject*>* pMapUI_List = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

            if (nullptr != pMapUI_List)
            {
                CGameObject* pMainObj = pMapUI_List->back();
                CMap_UI* pMainTarget = static_cast<CMap_UI*>(pMainObj);

                if (nullptr != pMainObj)
                {
                    if (pMainTarget->m_eMapComponent_Type == MAP_UI_TYPE::TARGET_MAP)
                    {
                        if (false == pMainTarget->m_IsChild) /* ���� ����̶�� */
                            m_pTarget_Main = pMainTarget;

                        else if (true == pMainTarget->m_IsChild)
                            m_pTarget_Main = pMainTarget->m_pTarget_Main;

                        if (nullptr != m_pTarget_Main)
                        {
                            //Safe_AddRef<CMap_UI*>(m_pTarget_Main);

                            CTransform* pMainTarget_Transform = static_cast<CTransform*>(m_pTarget_Main->Get_Component(g_strTransformTag));

                            m_pTarget_Transform = pMainTarget_Transform;

                            _float4 vMainTarget_Trans = pMainTarget_Transform->Get_State_Float4(CTransform::STATE_POSITION);
                            _float4 vSubTarget_Trans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

                            if (vMainTarget_Trans.x > vSubTarget_Trans.x && ZERO == vSubTarget_Trans.y)
                            {
                                m_eSubTarget_Type = SUB_TARGET_TYPE::LEFT_TARGET;
                                m_fTarget_Distance = vMainTarget_Trans.x - vSubTarget_Trans.x;
                            }

                            else if (vMainTarget_Trans.x < vSubTarget_Trans.x && ZERO == vSubTarget_Trans.y)
                            {
                                m_eSubTarget_Type = SUB_TARGET_TYPE::RIGHT_TARGET;
                                m_fTarget_Distance = abs(vSubTarget_Trans.x - vMainTarget_Trans.x);
                            }

                            else if (vMainTarget_Trans.y > vSubTarget_Trans.y)
                            {
                                m_eSubTarget_Type = SUB_TARGET_TYPE::DOWN_TARGET;
                                m_fTarget_Distance = vMainTarget_Trans.y - vSubTarget_Trans.y;
                            }

                            else
                            {
                                m_eSubTarget_Type = SUB_TARGET_TYPE::UP_TARGET;
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
        //m_ItemStore_Vec(3, nullptr);

        Find_Player_Target(); /* �÷��̾� Ÿ���� */
        Find_Item(); /* ������ */

        if (m_ItemStore_Vec[0].empty() || m_ItemStore_Vec[1].empty() || m_ItemStore_Vec[2].empty())
        {
            MSG_BOX(TEXT("������ ���Ϳ� �������� ���������� �������� �ʾҽ��ϴ�."));
        }

        if (nullptr != m_pTarget_Main)
        {
            CTransform* pMainTarget_Transform = static_cast<CTransform*>(m_pTarget_Main->Get_Component(g_strTransformTag));

            m_fTargetNotify_Distance.x = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).x - pMainTarget_Transform->Get_State_Float4(CTransform::STATE_POSITION).x ;
            m_fTargetNotify_Distance.y = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).y - pMainTarget_Transform->Get_State_Float4(CTransform::STATE_POSITION).y;
        }
        else
            MSG_BOX(TEXT("CMap_UI : Target_Notify���� Player Main Target�� �������� �ʾҽ��ϴ�."));

        if (!m_vecTextBoxes.empty())
        {
            _float4 vTextBox_Texture = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

           _float4 vText = m_vecTextBoxes.back()->GetPosition();

           m_fTargetNotify_TextBox_Distance.x = vTextBox_Texture.x - vText.x;
           m_fTargetNotify_TextBox_Distance.y = vTextBox_Texture.y - vText.y;
        }
    }

    else if (MAP_UI_TYPE::FLOOR_TYPE_MAP == m_eMapComponent_Type)
    {
        m_isRender = true;

        if (!m_vecTextBoxes.empty())
            m_vOriginTextColor = m_vecTextBoxes.back()->Get_FontColor();
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
    m_fOrigin_Blending = m_vColor[0].fBlender_Value;
    m_vColor[0].vColor.w = 0.f;

    Search_TabWindow();

    return S_OK;
}

void CMap_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if (nullptr == m_pInMap_Player)
    {
        Find_InMap_Player();

        if (nullptr == m_pInMap_Player)
            MSG_BOX(TEXT("Map UI�� Target�� InMap ���� Player�� ã�� �� �����ϴ�."));
    }

    /* 1. Render */
    Render_Condition(fTimeDelta);

    /* 2. Floor Selector�� ���õ� �� */
    if (MAP_UI_TYPE::FLOOR_TYPE_MAP == m_eMapComponent_Type)
        FloorType_Search();

    /* 3. Player �����ӿ� ���õ� �� */
    if (MAP_UI_TYPE::PLAYER_MAP == m_eMapComponent_Type)
        Map_Player_Control(fTimeDelta);

    /* 4. Map�� ���õ� �� */
    if (m_eFloorType == m_pInMap_Player->m_eCurrent_Floor || MAP_FLOOR_TYPE::FLOOR_FREE == m_eFloorType)
    {
        // EX CODE
        if (false == m_isGara)
        {
            Search_Map_Type(MAP_STATE_TYPE::SEARCH_CLEAR_STATE, MAIN_HOLL);
            Search_Map_Type(MAP_STATE_TYPE::SEARCH_STATE, ENTRANCE);
        }

        /* Transform */
        Transform_Condition(fTimeDelta);
    }
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
    if (_mapType == m_eMap_Location && (MAP_CHILD_TYPE::BACKGROUND_MAP == (MAP_CHILD_TYPE)m_iWhichChild && m_eMapComponent_Type == MAP_UI_TYPE::MAIN_MAP))
     {
         m_isGara = true;
         Change_Search_Type(_searType);
     }
}

void CMap_UI::Change_Search_Type(MAP_STATE_TYPE _searType)
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

void CMap_UI::Find_InMap_Player()
{
    list<CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

    if (nullptr != pUIList)
    {
        for (auto& iter : *pUIList)
        {
            m_pInMap_Player = dynamic_cast<CMap_UI*>(iter);

            if (nullptr != m_pInMap_Player && MAP_UI_TYPE::PLAYER_MAP == m_pInMap_Player->m_eMapComponent_Type)
            {
                m_pInMap_PlayerTrans = static_cast<CTransform*>(m_pInMap_Player->Get_Component(g_strTransformTag));

                //Safe_AddRef<CMap_UI*>(m_pInMap_Player);
                //Safe_AddRef<CTransform*>(m_pInMap_PlayerTrans);
                return;
            }
        }
    }  
     
}

void CMap_UI::Find_Player_Target()
{
    list<CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));

    if (nullptr != pUIList)
    {
        for (auto& iter : *pUIList)
        {
            CMap_UI* pTarget_UI = dynamic_cast<CMap_UI*>(iter);

            if (nullptr != pTarget_UI && MAP_UI_TYPE::TARGET_MAP == pTarget_UI->m_eMapComponent_Type)
            {
                m_pTarget_Main = pTarget_UI;

               // Safe_AddRef<CMap_UI*>(m_pTarget_Main);

                m_pTarget_Transform = static_cast<CTransform*>(m_pTarget_Main->Get_Component(g_strTransformTag));

                return;
            }
        }
    }
}

/* Floor�� Sotring �ؼ� ������ �ش�.  */
void CMap_UI::Floor_Sort()
{
    list<CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));
    vector<CMap_UI*> FloorVec;

    if (nullptr == pUIList)
        return;

    for (auto* iter : *pUIList)
    {
        CMap_UI* pFloor = dynamic_cast<CMap_UI*>(iter);

        if (nullptr != pFloor)
        {
            if (MAP_UI_TYPE::FLOOR_TYPE_MAP == pFloor->m_eMapComponent_Type)
            {
                FloorVec.push_back(pFloor);
            }
        }
    }

    if (!FloorVec.empty())
    {
        MAP_FLOOR_TYPE eType = (MAP_FLOOR_TYPE)0;

        for (_uint i = 0; i < FloorVec.size(); i++)
            FloorVec[i]->m_eSelect_Floor = static_cast<MAP_FLOOR_TYPE>(i);

        auto CompareByY = [](CMap_UI* a, CMap_UI* b) {
            CTransform* pTransformA = static_cast<CTransform*>(a->Get_Component(g_strTransformTag));
            CTransform* pTransformB = static_cast<CTransform*>(b->Get_Component(g_strTransformTag));

            _float4 vA = pTransformA->Get_State_Float4(CTransform::STATE_POSITION);
            _float4 vB = pTransformB->Get_State_Float4(CTransform::STATE_POSITION);

            return vA.y < vB.y;
            };

        sort(FloorVec.begin(), FloorVec.end(), CompareByY);
    }

    for (_uint i = 0; i < FloorVec.size(); i++) {
        FloorVec[i]->m_eSelect_Floor = static_cast<MAP_FLOOR_TYPE>(i);
    }
}

/* Render�� ���� �� ��ġ ���� */
void CMap_UI::Transform_Condition(_float fTimeDelta)
{
    if (false == m_isTransfrom_Setting)
    {
        Transform_Adjustment();

        if (false == m_isTransfrom_Setting)
            MSG_BOX(TEXT("CMap_UI���� ���𰡰� Transform Setting�� �������� �ʾҽ��ϴ�."));
    }

    /* Target�� ������ �� */
    if (MAP_UI_TYPE::TARGET_NOTIFY == m_eMapComponent_Type)
    {
        if (true == m_isRender)
        {
            _float4 vTarget_Transform = m_pTarget_Transform->Get_State_Float4(CTransform::STATE_POSITION);
            _float4 vTargetNotify_Transform = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

            vTargetNotify_Transform.x = vTarget_Transform.x + m_fTargetNotify_Distance.x;
            vTargetNotify_Transform.y = vTarget_Transform.y + m_fTargetNotify_Distance.y;

            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTargetNotify_Transform);
        }

        if (!m_vecTextBoxes.empty())
        {
            _float4 vTextBoxTexture = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
            _float3 fTextBox = m_vecTextBoxes.back()->GetPosition();

            fTextBox.x = vTextBoxTexture.x - m_fTargetNotify_TextBox_Distance.x;
            fTextBox.y = vTextBoxTexture.y - m_fTargetNotify_TextBox_Distance.y;

            for(auto& iter : m_vecTextBoxes)
                iter->Set_Position(fTextBox.x, fTextBox.y, fTextBox.z);
        }
    }

    /* ���콺�� ������ �� */
    if (true == m_isRender)
        Mouse_Pos(fTimeDelta);
}

void CMap_UI::Mouse_Pos(_float fTimeDelta)
{
    if (MAP_UI_TYPE::MAIN_MAP == m_eMapComponent_Type)
    {
        m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
    }

    if (MAP_UI_TYPE::MAIN_MAP == m_eMapComponent_Type || MAP_UI_TYPE::FONT_MAP == m_eMapComponent_Type ||
        MAP_UI_TYPE::DOOR_MAP == m_eMapComponent_Type || MAP_UI_TYPE::WINDOW_MAP == m_eMapComponent_Type ||
        MAP_UI_TYPE::PLAYER_MAP == m_eMapComponent_Type ||  MAP_UI_TYPE::ITEM_MAP == m_eMapComponent_Type ||
        MAP_UI_TYPE::TARGET_MAP == m_eMapComponent_Type || MAP_UI_TYPE::TARGET_NOTIFY == m_eMapComponent_Type)
    {
        if (PRESSING == m_pGameInstance->Get_KeyState(VK_LBUTTON))
        {
            POINT		ptDeltaPos = m_pGameInstance->Get_MouseDeltaPos();

            /* �ӵ� ���� �����ֱ� */
            if ((_long)0 != ptDeltaPos.x)
            {
                _float4 pos = {};

                pos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

                if (MAP_UI_TYPE::TARGET_MAP == m_eMapComponent_Type)
                    pos.x -= (fTimeDelta * (_float)ptDeltaPos.x * TARGET_SPEED);
                else
                    pos.x += fTimeDelta * (_float)ptDeltaPos.x * m_fMouseSensor;

                m_pTransformCom->Set_State(CTransform::STATE_POSITION, pos);

                if (MAP_UI_TYPE::PLAYER_MAP == m_eMapComponent_Type)
                    m_vPlayer_MovePos.x += fTimeDelta * (_float)ptDeltaPos.x * m_fMouseSensor;

            }

            if ((_long)0 != ptDeltaPos.y)
            {
                _float4 pos = {};

                pos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

                if (MAP_UI_TYPE::TARGET_MAP == m_eMapComponent_Type)
                    pos.y += (fTimeDelta * (_float)ptDeltaPos.y * TARGET_SPEED);
                else
                    pos.y -= fTimeDelta * (_float)ptDeltaPos.y * m_fMouseSensor;

                m_pTransformCom->Set_State(CTransform::STATE_POSITION, pos);

                if (MAP_UI_TYPE::PLAYER_MAP == m_eMapComponent_Type)
                    m_vPlayer_MovePos.y -= fTimeDelta * (_float)ptDeltaPos.y * m_fMouseSensor;
            }
        }
    }
}

void CMap_UI::Transform_Adjustment()
{
    /* Main */
    if (MAP_UI_TYPE::MAIN_MAP == m_eMapComponent_Type || MAP_UI_TYPE::DOOR_MAP == m_eMapComponent_Type
        || MAP_UI_TYPE::WINDOW_MAP == m_eMapComponent_Type || MAP_UI_TYPE::FONT_MAP == m_eMapComponent_Type
        || MAP_UI_TYPE::ITEM_MAP == m_eMapComponent_Type || MAP_UI_TYPE::TARGET_MAP == m_eMapComponent_Type)
    {
        _float4 vMainPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

        if (MAP_UI_TYPE::TARGET_MAP == m_eMapComponent_Type)
            vMainPos.z = 0.08f;
        else
            vMainPos.z = 0.2f;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMainPos);

        m_isTransfrom_Setting = true;
    }

    else if (MAP_UI_TYPE::PLAYER_MAP == m_eMapComponent_Type)
    {
        _float4 vMainPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

        if (false == m_IsChild)
            vMainPos.z = 0.1f;

        else if (true == m_IsChild)
            vMainPos.z = 0.2f;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMainPos);

        m_isTransfrom_Setting = true;
    }

    else if (MAP_UI_TYPE::MASK_MAP == m_eMapComponent_Type || MAP_UI_TYPE::NAMELINE_MAP == m_eMapComponent_Type ||
        MAP_UI_TYPE::SEARCH_TYPE_MAP == m_eMapComponent_Type ||  MAP_UI_TYPE::FLOOR_TYPE_MAP == m_eMapComponent_Type)
    {
        _float4 vMainPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
        vMainPos.z = 0.0f;
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMainPos);

        m_isTransfrom_Setting = true;
    }

    /* Mask */
    else if (MAP_UI_TYPE::BACKGROUND_MAP == m_eMapComponent_Type)
    {
        _float4 vMainPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
        vMainPos.z = 0.9f;
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMainPos);

        m_isTransfrom_Setting = true;
    }

    else if (MAP_UI_TYPE::FONT_MAP == m_eMapComponent_Type)
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

    else if (MAP_UI_TYPE::TARGET_NOTIFY == m_eMapComponent_Type)
    {
        m_isTransfrom_Setting = true;
    }
}

void CMap_UI::Player_Transform(_float fTimeDelta)
{
    if (nullptr == m_pPlayerTransform)
    {
        Find_Player();

        if (nullptr == m_pPlayerTransform)
        {
            MSG_BOX(TEXT("MinMap�� Player�� ã�� �� �����ϴ�."));
            return;
        }
    }

    _float2 fComparison = {};

    // MINMAP�� MODELMAP�� ũ�� ���� ���
    fComparison.x = MINMAP_X_SCALED / MODELMAP_X_SCALED;
    fComparison.y = MINMAP_Y_SCALED / MODELMAP_Y_SCALED;

    _float2 Moving_Value = {};

    Moving_Value.x = 0.f - m_pPlayerTransform->Get_State_Float4(CTransform::STATE_POSITION).x;
    Moving_Value.y = 0.f - m_pPlayerTransform->Get_State_Float4(CTransform::STATE_POSITION).z; // �÷��̾��� �ʱ� ��ġ�� ���� ��ġ�� ���� ���

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

void CMap_UI::EX_ColorChange()
{
    if (MAP_UI_TYPE::MAIN_MAP == m_eMapComponent_Type)
    {
        /* ���� �ڵ�*/
        if (DOWN == m_pGameInstance->Get_KeyState('I') && false == m_IsChild)
        {
            Search_Map_Type(MAP_STATE_TYPE::SEARCH_STATE, (LOCATION_MAP_VISIT)m_iCnt);

            if (false == m_IsChild)
                m_iCnt++;
        }
    }
}

void CMap_UI::FloorType_Search()
{
    if (false == m_isRender)
    {
        if(!m_vecTextBoxes.empty())
            m_vecTextBoxes.back()->Set_FontColor(ALPHA_ZERO);

        return;
    }

    if (m_eSelect_Floor >= MAP_FLOOR_TYPE::FLOOR_FREE)
    {
        Floor_Sort();

        if (m_eSelect_Floor >= MAP_FLOOR_TYPE::FLOOR_FREE)
            MSG_BOX(TEXT("Floor ������ ����� ������� �ʾҽ��ϴ�.")); // Floor type�� ��ġ�� �ȴ�.
    }

    if (MAP_UI_TYPE::FLOOR_TYPE_MAP == m_eMapComponent_Type)
    {
        if (true == IsMouseHover())
        {
            if (DOWN == m_pGameInstance->Get_KeyState(VK_LBUTTON))
            {
                m_pInMap_Player->m_eCurrent_Floor = m_eSelect_Floor;

                // �������Ե� �Ѱ���� �Ѵ�.
            }
        }

        if (m_pInMap_Player->m_eCurrent_Floor == m_eSelect_Floor)
        {
            if (!m_vecTextBoxes.empty())
            {
                m_vecTextBoxes.back()->Set_FontColor(_float4(1, 1, 1, 1));
                m_vCurrentColor = _float4(1, 1, 1, 1);
                m_fBlending = FLOOR_TYPE_BLENDING;
            }
        }
        else
        {
            m_vecTextBoxes.back()->Set_FontColor(m_vOriginTextColor);
            m_fBlending = 0.f;
        }
    }
}

/* ���� ��� ����*/
void CMap_UI::Render_Condition(_float fTimeDelta)
{
    //////////    :   Player Render State
    if (MAP_UI_TYPE::PLAYER_MAP == m_eMapComponent_Type)
    {
        if (nullptr != m_pTab_Window)
        {
            if (true == m_pTab_Window->Get_MinMapRender())
            {
                if (m_pInMap_Player->m_eCurrent_Floor == m_eCurrent_PlayerFloor)
                    m_isRender = true;

                else
                    m_isRender = false;
            }
            else
                m_isRender = false;
        }

        return;
    }

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

                else if (false == m_isRender)
                    iter->Set_FontColor(ALPHA_ZERO);
            }

            m_isPrevRender = m_isRender;
        }
    }

    //////////    :   Texture Render 
    if (nullptr == m_pTab_Window)
    {
        Search_TabWindow();

        if (nullptr == m_pTab_Window)
            MSG_BOX(TEXT("Map_UI���� Tab Window�� ������ �� �����ϴ�."));
    }

    if (nullptr != m_pTab_Window)
    {
        /* 1. Map Open */
        if (true == m_pTab_Window->Get_MinMapRender())
        {
            /* �� Target Notify Type */
            if (MAP_UI_TYPE::TARGET_NOTIFY == m_eMapComponent_Type)
            {
                _float4 vMainTarget_Transform = m_pTarget_Transform->Get_State_Float4(CTransform::STATE_POSITION);
                _float3 vMainTarget_Scaled = m_pTarget_Transform->Get_Scaled();

                if (true == MainTarget_Hover(vMainTarget_Transform, vMainTarget_Scaled))
                {
                    m_isRender = true;
                }
                else
                    m_isRender = false;
            }

            else if (m_eFloorType == m_pInMap_Player->m_eCurrent_Floor || MAP_FLOOR_TYPE::FLOOR_FREE == m_eFloorType)
            {
                if(true == m_isLastPosition)
                {
                    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vLastPosition);
                    m_isLastPosition = false;
                }

                m_isRender = true;

                if (m_fBlending <= m_fOrigin_Blending)
                {
                    m_fBlending = m_fOrigin_Blending;
                }

                else
                    m_fBlending -= fTimeDelta * BLENDING_SPEED;
            }

            else
            {
                if (true == m_isRender)
                {
                    if (m_fBlending >= 1.f)
                    {
                        m_fBlending = 1.f;
                        m_isRender = false;
                      
                    }

                    else 
                    {
                        /* 1. ��ü ������ ��ġ ���� */
                        if (false == m_isLastPosition)
                        {
                          //  m_vLastPosition = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
                            m_isLastPosition = true;
                        }

                        /* 2. ��ü Blending ���� */
                        m_fBlending += fTimeDelta * BLENDING_SPEED;
                    }
                }
            }
        }

        /* Close MinMap */
        else
        {
            m_isRender = false;
        }
    }

    /* Player �� �����ϴ� Player Target */
    else if (MAP_UI_TYPE::TARGET_MAP == m_eMapComponent_Type)
    {
        Map_Target_Control(fTimeDelta);
    }
}

/* Main Target�� ITEM�� Hovering �ϴ� ����*/
_bool CMap_UI::MainTarget_Hover(_float4 _mainPos, _float3 _scaled) // Ÿ���� ��� ������ item �簢���� ���ٸ�
{
    _float2 fMainPos = _float2(_mainPos.x, _mainPos.y);

    if(!m_ItemStore_Vec[(_int)m_pInMap_Player->m_eCurrent_Floor - 2].empty())
    {
        for (auto& iter : m_ItemStore_Vec[(_int)m_pInMap_Player->m_eCurrent_Floor - 2])
        {
            CTransform* pItem_Transform = static_cast<CTransform*>(iter->Get_Component(g_strTransformTag));

            _float4 vItem_Transform = pItem_Transform->Get_State_Float4(CTransform::STATE_POSITION);
            _float3 vSize = pItem_Transform->Get_Scaled();

            _float4 vPosition = { vItem_Transform.x + g_iWinSizeX * 0.5f, -vItem_Transform.y + g_iWinSizeY * 0.5f, vItem_Transform.z, 0 };

            if (vItem_Transform.x - (vSize.x / 2) <= fMainPos.x && vItem_Transform.y - (vSize.y / 2) <= fMainPos.y
                && vItem_Transform.x + (vSize.x / 2) >= fMainPos.x && vItem_Transform.y + (vSize.y / 2) >= fMainPos.y)
            {
                return true;
            }
        }
    }

    return false;
}

/* PLAYER �� �� �󿡼� �����̴� ��� */
void CMap_UI::Map_Player_Control(_float fTimeDelta)
{
    /* ������ ���� �� ����� ���� ���ƾ� �Ѵ�. */
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

        return;
    }

    else if(false == m_isRender && m_pInMap_Player->m_eCurrent_Floor == m_eCurrent_PlayerFloor)
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
        Player_Transform(fTimeDelta);
    }
}

/* PLAYER TARGETING�� �� �󿡼� �����̴� ��� */
void CMap_UI::Map_Target_Control(_float fTimeDelta)
{
    /* ó�� ��µ� �� �θ�� Player�� ��� �־�� �ϸ�,
        �ڽ��� �θ� ����ٳ�� �Ѵ�. */

    if(m_isPrevRender != m_isRender)
    {
        if (false == m_IsChild)
        {
            if (nullptr == m_pInMap_Player)
                MSG_BOX(TEXT("Map_UI���� Target �θ� Map Player�� �߰����� ���߽��ϴ�."));

            _float4 vTarget = m_pInMap_PlayerTrans->Get_State_Float4(CTransform::STATE_POSITION);

            vTarget.z = 0.08f;

            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTarget);
        }

        else if (true == m_IsChild)
        {
            if (nullptr == m_pTarget_Transform)
                MSG_BOX(TEXT("Map_UI���� Target�� �� �ϳ��� Main Target�� �߰����� ���߽��ϴ�."));

            if (m_eSubTarget_Type == SUB_TARGET_TYPE::LEFT_TARGET)
            {
                _float4 vMainTarget = m_pTarget_Transform->Get_State_Float4(CTransform::STATE_POSITION);
                _float4 vCurrentLine = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

                vCurrentLine.y = vMainTarget.y;
                vCurrentLine.x = vMainTarget.x + m_fTarget_Distance;
                vCurrentLine.z = 0.08f;

                m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentLine);
            }

            else if (m_eSubTarget_Type == SUB_TARGET_TYPE::RIGHT_TARGET)
            {
                _float4 vMainTarget = m_pTarget_Transform->Get_State_Float4(CTransform::STATE_POSITION);
                _float4 vCurrentLine = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

                vCurrentLine.y = vMainTarget.y;
                vCurrentLine.x = vMainTarget.x - m_fTarget_Distance;
                vCurrentLine.z = 0.08f;

                m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentLine);
            }

            else if (m_eSubTarget_Type == SUB_TARGET_TYPE::UP_TARGET)
            {
                _float4 vMainTarget = m_pTarget_Transform->Get_State_Float4(CTransform::STATE_POSITION);
                _float4 vCurrentLine = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

                vCurrentLine.x = vMainTarget.x;
                vCurrentLine.y = vMainTarget.y - m_fTarget_Distance;
                vCurrentLine.z = 0.08f;

                m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentLine);
            }

            else
            {
                _float4 vMainTarget = m_pTarget_Transform->Get_State_Float4(CTransform::STATE_POSITION);
                _float4 vCurrentLine = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

                vCurrentLine.x = vMainTarget.x;
                vCurrentLine.y = vMainTarget.y + m_fTarget_Distance;
                vCurrentLine.z = 0.08f;

                m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurrentLine);
            }
        }

        m_isPrevRender = m_isRender;
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
        //        Safe_AddRef(m_pTab_Window);
                return;
            }
        }
    }
}
   
/* Type�� ���� ���� �Լ� */
void CMap_UI::Find_MapStateType(CUSTOM_UI_DESC* CustomUIDesc)
{
    /* 1 Floor */
    if (TEXT("UI_Map") == m_wstrFile)
    {
        m_eMapComponent_Type = MAP_UI_TYPE::MAIN_MAP;
        m_eFloorType = MAP_FLOOR_TYPE::FLOOR_1;
    }

    else if (TEXT("UI_Map_Font") == m_wstrFile)
    {
        m_eMapComponent_Type = MAP_UI_TYPE::FONT_MAP;
        m_eFloorType = MAP_FLOOR_TYPE::FLOOR_1;
    }

    else if (TEXT("UI_Map_Door") == m_wstrFile)
    {
        m_eMapComponent_Type = MAP_UI_TYPE::DOOR_MAP;
        m_eFloorType = MAP_FLOOR_TYPE::FLOOR_1;
    }

    else if (TEXT("UI_Map_Window") == m_wstrFile)
    {
        m_eMapComponent_Type = MAP_UI_TYPE::WINDOW_MAP;
        m_eFloorType = MAP_FLOOR_TYPE::FLOOR_1;
    }

    else if (m_wstrFile == TEXT("UI_Map_Item"))
    {
        m_eMapComponent_Type = MAP_UI_TYPE::ITEM_MAP;
        m_eFloorType = MAP_FLOOR_TYPE::FLOOR_1;
    }

    /* 2 Floor */
    else if (m_wstrFile == TEXT("UI_Map_Floor2"))
    {
        m_eMapComponent_Type = MAP_UI_TYPE::MAIN_MAP;
        m_eFloorType = MAP_FLOOR_TYPE::FLOOR_2;
    }

    else if (m_wstrFile == TEXT("UI_Map_Font2"))
    {
        m_eMapComponent_Type = MAP_UI_TYPE::FONT_MAP;
        m_eFloorType = MAP_FLOOR_TYPE::FLOOR_2;
    }

    /* ���� */
    else if (m_wstrFile == TEXT("UI_Map_Player"))
    {
        m_eMapComponent_Type = MAP_UI_TYPE::PLAYER_MAP;
    }

    else if (m_wstrFile == TEXT("Map_Line"))
    {
        m_eMapComponent_Type = MAP_UI_TYPE::NAMELINE_MAP;
    }

    else if (m_wstrFile == TEXT("Map_Search_Type"))
    {
        m_eMapComponent_Type = MAP_UI_TYPE::SEARCH_TYPE_MAP;
    }

    else if (m_wstrFile == TEXT("Map_Target"))
    {
        m_eMapComponent_Type = MAP_UI_TYPE::TARGET_MAP;
    }

    else if (m_wstrFile == TEXT("Map_BackGround"))
    {
        m_eMapComponent_Type = MAP_UI_TYPE::BACKGROUND_MAP;
    }

    else if (TEXT("Map_Mask") == m_wstrFile)
    {
        m_eMapComponent_Type = MAP_UI_TYPE::MASK_MAP;
    }

    else if (TEXT("Map_Mask_Font") == m_wstrFile)
    {
        m_eMapComponent_Type = MAP_UI_TYPE::FONT_MASK_MAP;
    }

    else if (TEXT("UI_Map_Floor_Type") == m_wstrFile)
    {
        m_eMapComponent_Type = MAP_UI_TYPE::FLOOR_TYPE_MAP;
    }

    else if (TEXT("UI_Map_Floor3") == m_wstrFile)
    {
        m_eMapComponent_Type = MAP_UI_TYPE::MAIN_MAP;
        m_eFloorType = MAP_FLOOR_TYPE::FLOOR_3;

    }

    else if (TEXT("UI_Map_Item_Floor2") == m_wstrFile)
    {
        m_eMapComponent_Type = MAP_UI_TYPE::ITEM_MAP;
        m_eFloorType = MAP_FLOOR_TYPE::FLOOR_2;

    }

    else if (TEXT("UI_Map_Door_Floor2") == m_wstrFile)
    {
        m_eMapComponent_Type = MAP_UI_TYPE::DOOR_MAP;
        m_eFloorType = MAP_FLOOR_TYPE::FLOOR_2;

    }

    else if (TEXT("Map_Font2") == m_wstrFile)
    {
        m_eMapComponent_Type = MAP_UI_TYPE::FONT_MAP;
        m_eFloorType = MAP_FLOOR_TYPE::FLOOR_2;
    }

    else if (TEXT("Map_Font3") == m_wstrFile)
    {
        m_eMapComponent_Type = MAP_UI_TYPE::FONT_MAP;
        m_eFloorType = MAP_FLOOR_TYPE::FLOOR_3;
    }

    else if (TEXT("UI_Map_Item_Floor3") == m_wstrFile)
    {
        m_eMapComponent_Type = MAP_UI_TYPE::ITEM_MAP;
        m_eFloorType = MAP_FLOOR_TYPE::FLOOR_3;
    }

    else if (TEXT("UI_Map_Target_Notify") == m_wstrFile)
    {
        m_eMapComponent_Type = MAP_UI_TYPE::TARGET_NOTIFY;
    }
}

void CMap_UI::Find_Item()
{
    list<CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));
    m_ItemStore_Vec.resize(3);

    if (nullptr != pUIList)
    {
        for (auto& iter : *pUIList)
        {
            CMap_UI* pItem = dynamic_cast<CMap_UI*>(iter);

            if (nullptr != pItem && MAP_UI_TYPE::ITEM_MAP == pItem->m_eMapComponent_Type)
            {
                if (pItem->m_eFloorType == MAP_FLOOR_TYPE::FLOOR_1)
                    m_ItemStore_Vec[0].push_back(pItem);

                else if (pItem->m_eFloorType == MAP_FLOOR_TYPE::FLOOR_2)
                    m_ItemStore_Vec[1].push_back(pItem);

                else if (pItem->m_eFloorType == MAP_FLOOR_TYPE::FLOOR_3)
                    m_ItemStore_Vec[2].push_back(pItem);
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

   // Safe_Release<CTransform*>(m_pPlayerTransform);

    //if (nullptr != m_pPlayerTransform)
    //{
    //    Safe_Release<CTransform*>(m_pPlayerTransform);
    //    m_pPlayerTransform = nullptr;
    //}

 /*   if (nullptr != m_pInMap_Player)
    {
        Safe_Release<CMap_UI*>(m_pInMap_Player);
        m_pInMap_Player = nullptr;
    }

    if (nullptr != m_pInMap_PlayerTrans)
    {
        Safe_Release<CTransform*>(m_pInMap_PlayerTrans);
        m_pInMap_PlayerTrans = nullptr;
    }

    if (nullptr != m_pTab_Window)
    {
        Safe_Release<CTab_Window*>(m_pTab_Window);
        m_pTab_Window = nullptr;
    }*/

  /*  if(nullptr != m_pTarget_Main)
    {
        Safe_Release<CMap_UI*>(m_pTarget_Main);
        m_pTarget_Main = nullptr;
    }*/

}
