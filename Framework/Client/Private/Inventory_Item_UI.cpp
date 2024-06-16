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

    /* �� Main Inventory */
    for (auto& iter : *pBoxList)
    {
        CInventory_Item_UI* pBox = dynamic_cast<CInventory_Item_UI*>(iter);

        if (nullptr != pBox)
        {
            if (CCustomize_UI::ITEM_BOX_TYPE::SELECT_BOX == m_eBox_Type)
            {
                /* 1. Select Cursor�� �� : Select box �� distance ã��*/
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

                /* 2. Default(Void) Box �� �� .*/
                else
                {
                    if (CCustomize_UI::ITEM_BOX_TYPE::DEFAULT_BOX == pBox->m_eBox_Type)
                    {
                        /* Box�� Select Box�� ������ �ִ´� */
                        pBox->m_pSelectBox = this; //�ڡڡڡڡڡڡڡ�Safe_AddRef()���� �ذ��Ұ�
                    }

                }
            }
        }

        // �� �� ���� ����
        m_vColor[0].vColor.w = m_vCurrentColor.w = 0.f;
    }

    /* �� Sub Inventory */
    if (CCustomize_UI::INVENTORY_TYPE::SUB_INVEN == m_eInventory_Type)
    {
        if (!m_vecTextBoxes.empty())
        {
            m_vOriginTextColor = m_vecTextBoxes.back()->Get_FontColor();
        }

        /* �θ� ã�Ƽ� ������.*/
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

            /* ���͸� �߽����� ��ġ�� ���� �κ� ��ȣ�� �ű�� */
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

            /* Select ������ ������ �ִ´�. */
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

    /* �� Box �� ���� ���*/
    Box_Operater(fTimeDelta);

    /* �� Inventory ����*/
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

/* �ڽ� ���� ��� ���� */
void CInventory_Item_UI::Box_Operater(_float fTimeDelta)
{
    /* ���õ� �ڽ� ����*/
    if (CCustomize_UI::ITEM_BOX_TYPE::SELECT_BOX == m_eBox_Type)
        SelectBox(fTimeDelta);

    if (false == m_isRender)
        return;

    /* �⺻ �ڽ� ���� */
    if (CCustomize_UI::ITEM_BOX_TYPE::DEFAULT_BOX == m_eBox_Type)
        VoidBox();
}

/* �� �ڽ� ���� ��� */
void CInventory_Item_UI::VoidBox()
{
    /* �갡 ����Ʈ �ڽ��� ���� �� �Լ��� ���µ� �̶� �κ��丮 ������ ���� ���� */
    if (CCustomize_UI::INVENTORY_TYPE::MAIN_INVEN == m_eInventory_Type)
    {
        if (true == m_IsChild) // ���߿��� ���ϵ常 ����, �׷��� �� ������ �����൵ �ǰ� ��ü ���ȿ� X
        {
            CTransform* pSelectBox_Trans = {};

            /* 1. Select Box ����*/
            if (nullptr != m_pSelectBox)
            {
                // �׷��� �� �� �� �� �ڽ��� Select �ڽ��� �´�� �ִ� �� Ȯ�� �ϰ� ���� ��true�� �Ǹ� �� �� �ڽ��� ��ü�� ���缭 Select box �� �ű�� �ű�� ����
                pSelectBox_Trans = dynamic_cast<CTransform*>(m_pSelectBox->Get_Component(g_strTransformTag));

                if (true == IsMouseHover())
                {
                    //�̰� Defu
                    pSelectBox_Trans->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));
                    m_isMouseAcess = true; // �̰Ŵ��� ����Ʈ �ڽ��� ������ �̾� �� Ȯ���ϴ� ���̶� Ȥ�� ���߿� ���ϼ� �־ ��� �ִ� �� ������
                }
                else
                    m_isMouseAcess = false;
            }
        }
    }
}

/* ���� �ڽ� ���� ���  */
void CInventory_Item_UI::SelectBox(_float fTimeDelta)
{
    /* �� Select Box*/
    if (false == m_IsChild)
    {
        if (nullptr == m_isMainRender || nullptr == m_isSubRender)
            return;

        if (true == *m_isMainRender)
        {
            m_isRender = true;

            if (false == m_IsChild)
            {
                /* Mouse�� Ŭ�� ���� ���� �׷��� ���� ���� alpha ���� �ٸ���. */
                if (PRESSING == m_pGameInstance->Get_KeyState(VK_LBUTTON))
                {
                    Change_BoxType(true);
                }
                if (UP == m_pGameInstance->Get_KeyState(VK_LBUTTON))
                {
                    /* ���߿� �ٸ� ���� ĵ�� ��ų ����*/
                    Change_BoxType(false);
                }
            }
        }

        else if (true == *m_isSubRender)
            m_isRender = true;

        else
            m_isRender = false;
    }

    /* �� Cursor*/
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

/* �ڽ��� Ŭ�� ���� �� ���� ���*/
void CInventory_Item_UI::ClickOnBox()
{
}

/* TAB �ɼ� �� ������ �κ��丮 ó�� */
void CInventory_Item_UI::Item_Inventory()
{
    if (DOWN == m_pGameInstance->Get_KeyState(VK_TAB))
    {
        /* 1. Inventory�� Ű�� ���� : �⺻ ��ġ ã���ֱ� */
        if (false == m_isRender)
        {
            /* ù ��° Defualt Box ã���ֱ�*/
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

                /* �� �Ź� �Ҵ��� �ٿ� �ƿ� ��� ������ ����س���. ��*/
                /* 1. Defualt Box �� ���� ù�� ° Defualt Box�� ���´� */
                if (nullptr != m_pSelectBox)
                {
                    CTransform* pSelectTrans = dynamic_cast<CTransform*>(m_pSelectBox->Get_Component(g_strTransformTag));
                    pSelectTrans->Set_State(CTransform::STATE_POSITION, pDefaultBox_Trans->Get_State_Float4(CTransform::STATE_POSITION));
                }

                /* 2. Cursor�� ���� ù�� ° Defualt Box�� ���´�  => �ȵȴ�!!!!!!!!!! */
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

/* ���� ��� �κ��丮 ó�� */
void CInventory_Item_UI::Sub_Equipment_Inventory(_float fTimeDelta)
{
    /* 1. ������ â OPEN */
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

    /* 2. ������ â CLOSE */
    if (true == m_isRender)
    {
        if (!(PRESSING == m_pGameInstance->Get_KeyState('1') || PRESSING == m_pGameInstance->Get_KeyState('2') || PRESSING == m_pGameInstance->Get_KeyState('3') || PRESSING == m_pGameInstance->Get_KeyState('4')))
            m_fOpenInven_Timer += fTimeDelta;

        if (SUB_INVEN_OPENING <= m_fOpenInven_Timer)
        {
            /* 2. ��� Texture Color ���� */
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

    /* 1. FONT COLOR  ����*/
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


/* Sub Inventory���� Select �� �ڽ� �Ǻ�  */
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

/* Sub Inventory�� �������� �� [���/�ʱ�ȭ] �ؾ� �� �͵� */
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


/* �ڽ� ü���� */
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
