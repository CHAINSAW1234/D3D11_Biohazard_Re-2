#include "stdafx.h"
#include "Inventory_Item_UI.h"

#define SUB_INVEN_OPENING   1.5f
#define ALPHA_ZERO          _float4(0, 0, 0, 0)

#define SUBINVEN_SELECT_Z   0.25f
#define INVEN_SELECT_Z      0.f

CInventory_Item_UI::CInventory_Item_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CInventory{ pDevice, pContext }
{
}

CInventory_Item_UI::CInventory_Item_UI(const CInventory_Item_UI& rhs)
    : CInventory{ rhs }
{

}

HRESULT CInventory_Item_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInventory_Item_UI::Initialize(void* pArg)
{
    SUB_INVEN_WHICH_CHILD	eSubInven_Child = { };

    if (nullptr != pArg)
    {
        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;

        m_eBox_Type = CustomUIDesc->eBox_Type;
        m_eInventory_Type = CustomUIDesc->eInventory_Type;
        eSubInven_Child = (SUB_INVEN_WHICH_CHILD)CustomUIDesc->iWhich_Child;
    }

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

   
    list<class CGameObject*>* pBoxList = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));
    m_fOrigin_Blending = m_fBlending;

    /* ▶ Main Inventory */
    for (auto& iter : *pBoxList)
    {
        CInventory_Item_UI* pBox = dynamic_cast<CInventory_Item_UI*>(iter);

        if (nullptr != pBox)
        {
            if (CCustomize_UI::ITEM_BOX_TYPE::SELECT_BOX == m_eBox_Type)
            {
                /* 1. Select Cursor일 때 : Select box 간 distance 찾기*/
                if (true == m_IsChild)
                {
                    if (CCustomize_UI::ITEM_BOX_TYPE::SELECT_BOX == pBox->m_eBox_Type &&
                        false == pBox->Get_IsChild())
                    {
                        m_pSelectBox = pBox;

                        CTransform* pSelectBox_Trans = dynamic_cast<CTransform*>(pBox->Get_Component(g_strTransformTag));
                        _float4 fSelectBox_Trans = pSelectBox_Trans->Get_State_Float4(CTransform::STATE_POSITION);
                        _float4 fCursor_Trans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

                        m_fDistance_BetweenCursor = _float2(abs(fSelectBox_Trans.x - fCursor_Trans.x), abs(fSelectBox_Trans.y - fCursor_Trans.y));
                    }
                }

                /* 2. Default(Void) Box 일 때 .*/
                else
                {
                    if (CCustomize_UI::ITEM_BOX_TYPE::DEFAULT_BOX == pBox->m_eBox_Type)
                    {
                        /* Box은 Select Box을 가지고 있는다 */
                        pBox->m_pSelectBox = this; //★★★★★★★★Safe_AddRef()문제 해결할것
                    }

                }
            }
        }

        // ▶ 툴 수기 수정
        m_vColor[0].vColor.w = m_vCurrentColor.w = 0.f;
    }

    /* ▶ Sub Inventory */
    if (CCustomize_UI::INVENTORY_TYPE::SUB_INVEN == m_eInventory_Type)
    {
        if (!m_vecTextBoxes.empty())
        {
            m_vOriginTextColor = m_vecTextBoxes.back()->Get_FontColor();
        }

        /* 부모를 찾아서 나눈다.*/
        CInventory_Item_UI* pCenter_UI = {};

        for (auto& iter : *pBoxList)
        {
            pCenter_UI = dynamic_cast<CInventory_Item_UI*>(iter);

            if (nullptr != pCenter_UI && CCustomize_UI::INVENTORY_TYPE::SUB_INVEN == pCenter_UI->m_eInventory_Type && false == pCenter_UI->m_IsChild)
                break;

        }

        /* 1. Default Box */
        if (nullptr != pCenter_UI && eSubInven_Child == SUB_INVEN_WHICH_CHILD::DEFULAT_BOX_CHILD)
        {
            CTransform* pCenter_Trans = dynamic_cast<CTransform*>(pCenter_UI->Get_Component(g_strTransformTag));
            _float4 vCenter_Trans = pCenter_Trans->Get_State_Float4(CTransform::STATE_POSITION);
            _float4 vBox_Trans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

            /* 센터를 중심으로 위치에 따라서 인벤 번호를 매긴다 */
            if (vCenter_Trans.y == vBox_Trans.y && vCenter_Trans.x < vBox_Trans.x) // Right
            {
                m_eSubInven_Type = SUB_INVEN_BOX_POSITION::RIGHT_INVEN;
            }
            else if (vCenter_Trans.x == vBox_Trans.x && vCenter_Trans.y < vBox_Trans.y) // UP
            {
                m_eSubInven_Type = SUB_INVEN_BOX_POSITION::UP_INVEN;
            }
            else if (vCenter_Trans.x == vBox_Trans.x && vCenter_Trans.y > vBox_Trans.y) // DOWN
            {
                m_eSubInven_Type = SUB_INVEN_BOX_POSITION::DOWN_INVEN;
            }
            else  // Left
            {
                m_eSubInven_Type = SUB_INVEN_BOX_POSITION::LEFT_INVEN;
            }

            /* Select 변수를 가지고 있는다. */
            for (auto& iter : *pBoxList)
            {
                CInventory_Item_UI* pSelectBox = dynamic_cast<CInventory_Item_UI*>(iter);

                if (nullptr != pSelectBox && CCustomize_UI::ITEM_BOX_TYPE::SELECT_BOX == pSelectBox->m_eBox_Type)
                {
                    m_pSelectBox = dynamic_cast<CInventory_Item_UI*>(iter);
                    break;
                }
            }
        }

    }

    return S_OK;
}

void CInventory_Item_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    /* ▶ Box 별 동작 방식*/
    Box_Operater(fTimeDelta);

    /* ▶ Inventory 종류*/
    /* 0. Default Item Inventory */
    if (CCustomize_UI::INVENTORY_TYPE::MAIN_INVEN == m_eInventory_Type)
        Item_Inventory();

    /* 1. Sub Equip Inventory*/
    else if (CCustomize_UI::INVENTORY_TYPE::SUB_INVEN == m_eInventory_Type)
        Sub_Equipment_Inventory(fTimeDelta);
}

void CInventory_Item_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CInventory_Item_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

/* 박스 동작 방식 모음 */
void CInventory_Item_UI::Box_Operater(_float fTimeDelta)
{
    /* 선택된 박스 관련*/
    if (CCustomize_UI::ITEM_BOX_TYPE::SELECT_BOX == m_eBox_Type)
        SelectBox(fTimeDelta);

    if (false == m_isRender)
        return;

    /* 기본 박스 관련 */
    if (CCustomize_UI::ITEM_BOX_TYPE::DEFAULT_BOX == m_eBox_Type)
        VoidBox();
}

/* 빈 박스 동작 방식 */
void CInventory_Item_UI::VoidBox()
{
    /* 얘가 디폴트 박스일 때만 이 함수에 들어가는데 이때 인벤토리 메인일 때만 들어가고 */
    if (CCustomize_UI::INVENTORY_TYPE::MAIN_INVEN == m_eInventory_Type)
    {
        if (true == m_IsChild) // 그중에서 차일드만 들어가고, 그래서 이 조건은 없애줘도 되고 빈객체 ㅁ안에 X
        {
            CTransform* pSelectBox_Trans = {};

            /* 1. Select Box 관련*/
            if (nullptr != m_pSelectBox)
            {
                // 그렇게 될 때 그 빈 박스에 Select 박스랑 맞닿아 있는 지 확인 하고 만야 ㄱtrue가 되면 그 빈 박스의 위체에 맞춰서 Select box 를 거기로 옮기는 거임
                pSelectBox_Trans = dynamic_cast<CTransform*>(m_pSelectBox->Get_Component(g_strTransformTag));

                if (true == IsMouseHover())
                {
                    //이게 Defu
                    pSelectBox_Trans->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));
                    m_isMouseAcess = true; // 이거는이 디폴트 박스에 놓여져 이쓴 지 확인하는 용이라 혹시 나중에 스일수 있어서 사용 있는 지 ㅁ랐음
                }
                else
                    m_isMouseAcess = false;
            }
        }
    }
}

/* 선택 박스 동작 방식  */
void CInventory_Item_UI::SelectBox(_float fTimeDelta)
{
    /* ▶ Select Box*/
    if (false == m_IsChild)
    {
        if (nullptr == m_isMainRender || nullptr == m_isSubRender)
            return;

        if (true == *m_isMainRender)
        {
            m_isRender = true;

            if (false == m_IsChild)
            {
                /* Mouse를 클릭 했을 때와 그렇지 않을 때의 alpha 값이 다르다. */
                if (PRESSING == m_pGameInstance->Get_KeyState(VK_LBUTTON))
                {
                    Change_BoxType(true);
                }
                if (UP == m_pGameInstance->Get_KeyState(VK_LBUTTON))
                {
                    /* 나중에 다른 경우로 캔슬 시킬 예정*/
                    Change_BoxType(false);
                }
            }
        }

        else if (true == *m_isSubRender)
            m_isRender = true;

        else
            m_isRender = false;
    }

    /* ▶ Cursor*/
    else if (true == m_IsChild)
    {
        if (true == *m_pSelectBox->m_isMainRender)
        {
            m_isRender = true;

            CTransform* pSelectBox_Trans = dynamic_cast<CTransform*>(m_pSelectBox->Get_Component(g_strTransformTag));
            if (nullptr != pSelectBox_Trans)
            {
                _float4 vSelectBox_Trans = pSelectBox_Trans->Get_State_Float4(CTransform::STATE_POSITION);
                vSelectBox_Trans.x -= m_fDistance_BetweenCursor.x;
                vSelectBox_Trans.y -= m_fDistance_BetweenCursor.y;
                vSelectBox_Trans.z = INVEN_SELECT_Z;
                m_pTransformCom->Move_toTargetUI(vSelectBox_Trans, 6.f, 5.f);
            }
        }
        else
            m_isRender = false;
    }
}

/* 박스를 클릭 했을 때 동작 방식*/
void CInventory_Item_UI::ClickOnBox()
{
}

/* TAB 옵션 내 아이템 인벤토리 처리 */
void CInventory_Item_UI::Item_Inventory()
{
    if (DOWN == m_pGameInstance->Get_KeyState(VK_TAB))
    {
        /* 1. Inventory를 키기 직전 : 기본 위치 찾아주기 */
        if (false == m_isRender)
        {
            /* 첫 번째 Defualt Box 찾아주기*/
            list<class CGameObject*>* pBoxList = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));
            CInventory_Item_UI* pDefaultBox = {};

            for (auto& iter : *pBoxList)
            {
                pDefaultBox = dynamic_cast<CInventory_Item_UI*>(iter);

                if (nullptr != pDefaultBox &&
                    ITEM_BOX_TYPE::DEFAULT_BOX == pDefaultBox->m_eBox_Type && true == pDefaultBox->m_IsChild)
                    break;
            }

            if (nullptr != pDefaultBox)
            {
                CTransform* pDefaultBox_Trans = dynamic_cast<CTransform*>(pDefaultBox->Get_Component(g_strTransformTag));

                /* ◈ 매번 할당할 바에 아예 멤버 변수로 등록해놓기. ◈*/
                /* 1. Defualt Box 를 가장 첫번 째 Defualt Box에 놓는다 */
                if (nullptr != m_pSelectBox)
                {
                    CTransform* pSelectTrans = dynamic_cast<CTransform*>(m_pSelectBox->Get_Component(g_strTransformTag));
                    pSelectTrans->Set_State(CTransform::STATE_POSITION, pDefaultBox_Trans->Get_State_Float4(CTransform::STATE_POSITION));
                }

                /* 2. Cursor를 가장 첫번 째 Defualt Box에 놓는다  => 안된다!!!!!!!!!! */
                /*if (ITEM_BOX_TYPE::SELECT_BOX == m_eBox_Type && true == m_IsChild)
                {
                    _float4 vDefaultBox = pDefaultBox_Trans->Get_State_Float4(CTransform::STATE_POSITION);
                    vDefaultBox.x -= m_fDistance_BetweenCursor.x;
                    vDefaultBox.y -= m_fDistance_BetweenCursor.y;
                    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vDefaultBox);
                }*/
            }
        }

        Inventory_Render(!m_isRender);
    }

    if (nullptr != m_pSelectBox)
        m_pSelectBox->m_isMainRender = &m_isRender;
}

/* 서브 장비 인벤토리 처리 */
void CInventory_Item_UI::Sub_Equipment_Inventory(_float fTimeDelta)
{
    /* 1. 아이템 창 OPEN */
    if (DOWN == m_pGameInstance->Get_KeyState('1'))
    {
        Sub_SelectBox(SUB_INVEN_BOX_POSITION::LEFT_INVEN);
        Sub_Iventory_Reset();
    }

    else if (DOWN == m_pGameInstance->Get_KeyState('2'))
    {
        Sub_SelectBox(SUB_INVEN_BOX_POSITION::UP_INVEN);
        Sub_Iventory_Reset();
    }

    else if (DOWN == m_pGameInstance->Get_KeyState('3'))
    {
        Sub_SelectBox(SUB_INVEN_BOX_POSITION::RIGHT_INVEN);
        Sub_Iventory_Reset();
    }

    else if (DOWN == m_pGameInstance->Get_KeyState('4'))
    {
        Sub_SelectBox(SUB_INVEN_BOX_POSITION::DOWN_INVEN);
        Sub_Iventory_Reset();
    }

    /* 2. 아이템 창 CLOSE */
    if (true == m_isRender)
    {
        if (!(PRESSING == m_pGameInstance->Get_KeyState('1') || PRESSING == m_pGameInstance->Get_KeyState('2') || PRESSING == m_pGameInstance->Get_KeyState('3') || PRESSING == m_pGameInstance->Get_KeyState('4')))
            m_fOpenInven_Timer += fTimeDelta;

        if (SUB_INVEN_OPENING <= m_fOpenInven_Timer)
        {
            /* 2. 모든 Texture Color 보간 */
            if (m_fBlending <= 1)
            {
                m_vCurrentColor.w = 0.f;
                m_fBlending += fTimeDelta * 4.f;

                if (nullptr != m_pSelectBox)
                    m_pSelectBox->m_fBlending += fTimeDelta * 4.f;
            }

            else
            {
                /* Sub Inventory UI*/
                m_fBlending = 1.f;
                m_isRender = false;
                m_fOpenInven_Timer = 0.f;

                /* Select Box */
                if (nullptr != m_pSelectBox)
                {
                    m_pSelectBox->m_isRender = false;
                    m_pSelectBox->m_isSubRender = &m_isRender;
                    m_pSelectBox->m_fBlending = 0.f;
                }
            }
        }
    }

    /* 1. FONT COLOR  보간*/
    if (!m_vecTextBoxes.empty())
    {
        CTextBox* pFont = m_vecTextBoxes.back();

        if (false == m_isRender|| 1.f <= m_fBlending)
        {
            pFont->Set_FontColor(ALPHA_ZERO);
            return;
        }

        m_isColorChange = true;
        m_isBlending = true;

        _float4 vFontColor_Result = m_fBlending * ALPHA_ZERO + (1 - m_fBlending) * m_vOriginTextColor;
        pFont->Set_FontColor(vFontColor_Result);
    }

    if (nullptr != m_pSelectBox)
        m_pSelectBox->m_isSubRender = &m_isRender;
}


/* Sub Inventory에서 Select 할 박스 판별  */
void CInventory_Item_UI::Sub_SelectBox(SUB_INVEN_BOX_POSITION _eBoxType)
{
    /* 1. Find elect Box */
    CTransform* pSelectBox_Trans = {};

    if (nullptr != m_pSelectBox)
        pSelectBox_Trans = dynamic_cast<CTransform*>(m_pSelectBox->Get_Component(g_strTransformTag));

    /* 2. Choose Default Box */
    if (_eBoxType == m_eSubInven_Type)
    {
        if (nullptr != pSelectBox_Trans)
        {
            _float4 vBoxTrans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
            vBoxTrans.z = SUBINVEN_SELECT_Z;
            pSelectBox_Trans->Set_State(CTransform::STATE_POSITION, vBoxTrans);
        }
    }
}

/* Sub Inventory를 실행했을 때 [출력/초기화] 해야 할 것들 */
void CInventory_Item_UI::Sub_Iventory_Reset()
{
    list<class CGameObject*>* pBoxList = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));

    for (auto& iter : *pBoxList)
    {
        CInventory_Item_UI* pInven = dynamic_cast<CInventory_Item_UI*>(iter);

        if (nullptr != pInven && CCustomize_UI::INVENTORY_TYPE::SUB_INVEN == pInven->m_eInventory_Type)
        {
            pInven->Inventory_Render(true);
            pInven->m_fOpenInven_Timer = 0.f;
            pInven->m_fInterpolation_Timer = 0.f;
            pInven->m_fBlending = m_fOrigin_Blending;
            pInven->m_vCurrentColor = pInven->m_vColor[0].vColor = ALPHA_ZERO;
        }
    }
}


/* 박스 체인지 */
void CInventory_Item_UI::Change_BoxType(_bool _item)
{
    if (ITEM_BOX_TYPE::DEFAULT_BOX == m_eBox_Type)
    {
        /* Full Item*/
        if (true == _item)
            Change_Texture(TEXT("Prototype_Component_Texture_Box_Store"), TEXT("Com_DefaultTexture"));

        /* Non Item */
        else if (false == _item)
            Change_Texture(m_wstrDefaultTexturComTag, TEXT("Com_DefaultTexture"));
    }

    else if (ITEM_BOX_TYPE::SELECT_BOX == m_eBox_Type)
    {
        /* Full Item*/
        if (true == _item)
            Change_Texture(TEXT("Prototype_Component_Texture_Box_Select_Click"), TEXT("Com_DefaultTexture"));

        /* Non Item */
        else if (false == _item)
            Change_Texture(m_wstrDefaultTexturComTag, TEXT("Com_DefaultTexture"));
    }
}


CCustomize_UI* CInventory_Item_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInventory_Item_UI* pInstance = new CInventory_Item_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CInventory_Item_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CInventory_Item_UI::Clone(void* pArg)
{
    CInventory_Item_UI* pInstance = new CInventory_Item_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CInventory_Item_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInventory_Item_UI::Free()
{
    __super::Free();
}
