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
    if (nullptr != pArg)
    {
        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;

        m_eBox_Type = CustomUIDesc->eBox_Type;
        m_eInventory_Type = CustomUIDesc->eInventory_Type;
        m_eSubInven_Child = (SUB_INVEN_WHICH_CHILD)CustomUIDesc->iWhich_Child;
    }

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;


    list<class CGameObject*>* pBoxList = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));
    m_fOrigin_Blending = m_fBlending;

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
            {
                Find_SelectBox();
                break;
            }

        }

        /* 1. Default Box */
        if (nullptr != pCenter_UI && m_eSubInven_Child == SUB_INVEN_WHICH_CHILD::DEFULAT_BOX_CHILD)
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
                m_eSubInven_Type = SUB_INVEN_BOX_POSITION::LEFT_INVEN;

            /* Select 변수를 가지고 있는다. */
        }
    }

    if(false == m_IsChild)
        Find_SelectBox();

    return S_OK;
}

void CInventory_Item_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    if(nullptr == m_pSelectBox && m_eSubInven_Child == SUB_INVEN_WHICH_CHILD::DEFULAT_BOX_CHILD)
    {
        Find_SelectBox();

        if (nullptr == m_pSelectBox)
            MSG_BOX(TEXT("Inventory_Item_UI() : Select Box를 찾지 못했습니다."));
    }

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

HRESULT CInventory_Item_UI::Change_Tool()
{
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
    if (CCustomize_UI::INVENTORY_TYPE::MAIN_INVEN == m_eInventory_Type)
    {
        if (true == m_IsChild)
        {
            CTransform* pSelectBox_Trans = {};

            /* 1. Select Box 관련*/
            if (nullptr != m_pSelectBox)
            {
                pSelectBox_Trans = dynamic_cast<CTransform*>(m_pSelectBox->Get_Component(g_strTransformTag));

                if (true == IsMouseHover())
                {
                    pSelectBox_Trans->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));
                    m_isMouseAcess = true;
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
    if (true == m_isMainRender)
        return;

    /* ▶ Select Box*/
    if (false == m_IsChild)
    {
        if (true == *m_isSubRender)
            m_isRender = true;

        else
            m_isRender = false;
    }

    /* ▶ Cursor*/
    else if (true == m_IsChild)
        m_isRender = false;
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
            }
        }

        //Inventory_Render(!m_isRender);
    }
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
        if (!(PRESSING == m_pGameInstance->Get_KeyState('1') || PRESSING == m_pGameInstance->Get_KeyState('2') ||
            PRESSING == m_pGameInstance->Get_KeyState('3') || PRESSING == m_pGameInstance->Get_KeyState('4') ||
            true == m_isInvenOpen))
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

        if (false == m_isRender || 1.f <= m_fBlending)
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
        pSelectBox_Trans = static_cast<CTransform*>(m_pSelectBox->Get_Component(g_strTransformTag));

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

void CInventory_Item_UI::Reset_Call(_bool bInput)
{
    Set_IsRender(bInput);
    Set_InvenOpen(bInput);

    if(true == bInput)
        m_isMainInven_Open = true;

    else if(false == bInput)
        m_isMainInven_Open = false;

    if (nullptr == m_pSelectBox)
    {
        Find_SelectBox();
        
        if(nullptr == m_pSelectBox)
            MSG_BOX(TEXT("Inventory_Item_UI() : Main Inven과 연결 중에 문제가 생겼습니다.")); /* Select Box를 없애지 못함 */
    }

    if (nullptr != m_pSelectBox)
        m_pSelectBox->m_isMainRender = bInput;

    //Sub_SelectBox(SUB_INVEN_BOX_POSITION::LEFT_INVEN);
    //Sub_SelectBox(SUB_INVEN_BOX_POSITION::UP_INVEN);
    //Sub_SelectBox(SUB_INVEN_BOX_POSITION::RIGHT_INVEN);
    //Sub_SelectBox(SUB_INVEN_BOX_POSITION::DOWN_INVEN);

    //Sub_Iventory_Reset();

    //if (false == bInput)
    //{

    //}
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
          //  pInven->Inventory_Render(true);
            pInven->m_fOpenInven_Timer = 0.f;
            pInven->m_fInterpolation_Timer = 0.f;
            pInven->m_isRender = true;
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

void CInventory_Item_UI::Find_SelectBox()
{
    list<class CGameObject*>* pBoxList = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));

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

    //if(nullptr != m_pSelectBox)
    //{
    //    Safe_Release<CInventory_Item_UI*>(m_pSelectBox);
    //    m_pSelectBox = nullptr;
    //}
}

