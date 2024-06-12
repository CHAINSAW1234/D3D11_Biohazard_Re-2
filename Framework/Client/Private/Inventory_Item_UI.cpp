#include "stdafx.h"
#include "Inventory_Item_UI.h"

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
    }

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    list<class CGameObject*>* pBoxList = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));
    
    for (auto& iter : *pBoxList)
    {
        CInventory_Item_UI* pBox = dynamic_cast<CInventory_Item_UI*>(iter);

        if (nullptr != pBox)
        {
            /* Select Cursor일 때 : Select box 간 distance 찾기*/
            if (CCustomize_UI::ITEM_BOX_TYPE::SELECT_BOX == m_eBox_Type)
            {
                if (true == m_IsChild)
                {
                    if (CCustomize_UI::ITEM_BOX_TYPE::SELECT_BOX == pBox->Get_ItemBox_Type() &&
                        false == pBox->Get_IsChild())
                    {
                        m_pSelectBox = pBox;

                        CTransform* pSelectBox_Trans = dynamic_cast<CTransform*>(pBox->Get_Component(g_strTransformTag));
                        _float4 fSelectBox_Trans = pSelectBox_Trans->Get_State_Float4(CTransform::STATE_POSITION);
                        _float4 fCursor_Trans = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

                        m_fDistance_BetweenCursor = _float2(abs(fSelectBox_Trans.x - fCursor_Trans.x), abs(fSelectBox_Trans.y - fCursor_Trans.y));
                    }
                }

                else
                {
                    if (CCustomize_UI::ITEM_BOX_TYPE::DEFAULT_BOX == pBox->Get_ItemBox_Type())
                        pBox->m_pSelectBox = this;
                }
            }
        }
    }


    return S_OK;
}

void CInventory_Item_UI::Tick(_float fTimeDelta)
{   
    if (true == m_bDead)
        return;

    __super::Tick(fTimeDelta);

    /* 기본 박스 관련 */
    if(CCustomize_UI::ITEM_BOX_TYPE::DEFAULT_BOX == m_eBox_Type)
        VoidBox(); 
    
    /* 선택된 박스 관련*/
    else if (CCustomize_UI::ITEM_BOX_TYPE::SELECT_BOX == m_eBox_Type)
        SelectBox(fTimeDelta);

}

void CInventory_Item_UI::Late_Tick(_float fTimeDelta)
{
    if (true == m_bDead)
        return;

    __super::Late_Tick(fTimeDelta);
}

HRESULT CInventory_Item_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CInventory_Item_UI::VoidBox()
{
    /* X 박스만 */
    if(true == m_IsChild)
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

void CInventory_Item_UI::SelectBox(_float fTimeDelta)
{
    /* Select Box에 붙어 있는 Cursor */
    if (true == m_IsChild)
    {
        CTransform* pSelectBox_Trans = dynamic_cast<CTransform*>(m_pSelectBox->Get_Component(g_strTransformTag));

        if (nullptr != pSelectBox_Trans)
        {
            _float4 vSelectBox_Trans = pSelectBox_Trans->Get_State_Float4(CTransform::STATE_POSITION);
            vSelectBox_Trans.x -= m_fDistance_BetweenCursor.x;
            vSelectBox_Trans.y -= m_fDistance_BetweenCursor.y;

            m_pTransformCom->Move_toTargetUI(vSelectBox_Trans, 6.f, 5.f);
        }
    }

    else if (false == m_IsChild)
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

/* 박스를 클릭 했을 때 동작 방식*/
void CInventory_Item_UI::ClickOnBox()
{
 /*   list<class CGameObject*>* pBoxList = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));

    for (auto& iter : *pBoxList)
    {
        CInventory_Item_UI* pDefaultBox = dynamic_cast<CInventory_Item_UI*>(iter);

        if (nullptr != pDefaultBox)
        {
            if (ITEM_BOX_TYPE::DEFAULT_BOX == pDefaultBox->m_eBox_Type && true == pDefaultBox->m_isMouseAcess)
            {

            }
        }
    }*/
}

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
