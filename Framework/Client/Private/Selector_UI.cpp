#include "stdafx.h"
#include "Selector_UI.h"
#include "Player.h"
#include "InteractProps.h"

#define ALHPE_ZERO              _float4(0, 0, 0, 0)

#define OUT_DISTANCE            5.f
#define INTERACTIVE_DISTANCE    2.f

#define CHECK_SIZE              50.f
#define X_TYPEY                 50.f

CSelector_UI::CSelector_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCustomize_UI{ pDevice, pContext }
{
}

CSelector_UI::CSelector_UI(const CSelector_UI& rhs)
    : CCustomize_UI{ rhs }
{

}

HRESULT CSelector_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSelector_UI::Initialize(void* pArg)
{
    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;

        if (true == m_IsChild)
            m_pSupervise = CustomUIDesc->pSupervisor;
    }

    /* �θ� �ȿ� ������� �ʴٸ� ȭ��ǥ */
    if (true == m_IsChild)
    {
        list<CGameObject*>* pUILayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));

        for (auto& iter : *pUILayer)
        {
            CSelector_UI* pSelector = dynamic_cast<CSelector_UI*>(iter);

            if (nullptr != pSelector && false == pSelector->m_IsChild)
            {
                CTransform* pParentTrans = static_cast<CTransform*>(pSelector->Get_Component(g_strTransformTag));
                _float4 vParentPos = pParentTrans->Get_State_Float4(CTransform::STATE_POSITION);
                _float3 fParentScaled = pParentTrans->Get_Scaled();

                _float4 vCurrentChild = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);

                if (vCurrentChild.y < vParentPos.y - 10.f)
                {
                    m_isArrow = true;
                    m_pParent = pSelector;

                    m_eSelectorType = SELECTOR_TYPE::OUTCHECK_SELECTOR;

                    break;
                }
                else
                    m_eSelectorType = SELECTOR_TYPE::INCHECK_SELECTOR;
            }
        }
    }
    else if (false == m_IsChild)
    {
        m_eSelectorType = SELECTOR_TYPE::BACKGROUND_SELECTOR;

        if (m_SelectorObj_Vec.empty())
            Find_Selector_Obj();
    }

    /* Find Player / Object */
    Find_Player();

    /* Y�� ������ */
    if (false == m_isArrow)
    {
        m_vXTransform = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
        m_vXTransform.y += X_TYPEY;
    }

    /* Tool*/
    m_fColorTimer_Limit = 1.f;
    m_iEndingType = PLAY_BUTTON::PLAY_DEFAULT;
    m_vColor[0].fBlender_Value = m_fBlending = 0.f;
    m_isPlay = false;

    m_vColor[0].isBlender = m_isBlending = true;
    m_vColor[0].vColor = m_vCurrentColor = ALHPE_ZERO;
    m_vColor[0].isColorChange = m_isColorChange = true;

    /* Client*/
    m_fOriginSize = m_pTransformCom->Get_Scaled();
    m_isRender = false;

    /* 1. Texture*/
    m_wstrInteractive_Tag = m_wstrDefaultTexturComTag;
    m_wstrNonInteractive_Tag = TEXT("Prototype_Component_Texture_Perspective_Selecter_Check"); 

    return S_OK;
}

void CSelector_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    Exception_Handle();
    Operate_Selector(fTimeDelta);
}

void CSelector_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CSelector_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CSelector_UI::Destroy_Selector()
{
    for (auto& iter : m_SelectorObj_Vec)
    {
        iter->m_isUsing = false;
    }
}

/* true => ��ȣ�ۿ� ���� false => */
void CSelector_UI::Select_Type(_bool _Interact)
{
    for (auto& iter : m_SelectorObj_Vec)
        iter->m_isInteractive = _Interact;
}


void CSelector_UI::Render_Selector_UI(CGameObject* _obj, _float fTimeDelta)
{
    _float fDistance = Distance_Player(_obj);

    if (fDistance >= OUT_DISTANCE)
    {
        if (m_fBlending >= 1.f)
        {
            m_fBlending = 1.f;
            m_isRender = false;
        }
        else
            m_fBlending += fTimeDelta;
        return;
    }

    else
    {
        /* 1. ���� Render�� false ��� fBlending�� ���� ������. */
        m_isRender = true;
        if (m_fBlending <= 0.f)
        {
            m_fBlending = 0.f;
        }
        else
            m_fBlending -= fTimeDelta;

        /* 2. �信 ���� ��ġ ���� */
        CTransform* pObjTrans = static_cast<CTransform*>(_obj->Get_Component(g_strTransformTag));
        _float3 fCurrentSize = {};

        _float4 vTargetPosition = pObjTrans->Get_State_Float4(CTransform::STATE_POSITION);
        _vector vViewPos = XMVector3TransformCoord(vTargetPosition, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
        _vector vProjPos = XMVector3TransformCoord(vViewPos, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

        _float2 vProjPosition = { (XMVectorGetX(vProjPos) + 1.f) * 0.5f * g_iWinSizeX, (1.f - XMVectorGetY(vProjPos)) * 0.5f * g_iWinSizeY };

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ g_iWinSizeX * -0.5f + vProjPosition.x, g_iWinSizeY * 0.5f - vProjPosition.y, 0.8f, 1.f });

        /* 3. �Ÿ����� �־��� ���� Size�� Ŀ����. */
        if (fDistance <= INTERACTIVE_DISTANCE)
        {
            /*4. ��ȣ�ۿ��� �����ϸ� �� �Լ��� ���߰� �ٸ� �Լ��� ����*/
            fCurrentSize = m_fOriginSize;
            m_isInteractive = true;
        }

        else
            fCurrentSize *= fDistance;
    }

    /* y �� ����*/
    if (false == m_isArrow)
    {
        if (m_vXTransform.y - X_TYPEY > m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION).y)
        {
            _float4 vPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
            vPos.y = m_vXTransform.y - X_TYPEY;
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
        }
    }
}

void CSelector_UI::Operate_Selector(_float fTimeDelta)
{
    if (true != m_isUsing)
    {
        Reset();
        return;
    }

    /* ��ȣ�ۿ��� �Ұ����� �� : Check */
    if (false == m_isInteractive)
    {
        Render_Selector_UI(m_pInteractObj, fTimeDelta);

         /* Texture ���� */
        if (m_wstrDefaultTexturComTag != m_wstrNonInteractive_Tag && true == m_IsChild && false == m_isArrow)
        {
            m_wstrDefaultTexturComTag = m_wstrNonInteractive_Tag;
            Change_Texture(m_wstrNonInteractive_Tag, TEXT("Com_DefaultTexture"));
        }

        if (true == m_isArrow)
            m_isRender = false;
    }

    /* ��ȣ�ۿ��� ������ �� : X */
    else if (true == m_isInteractive)
    {
         /* Texture ���� */
        if (m_wstrDefaultTexturComTag != m_wstrInteractive_Tag && true == m_IsChild && false == m_isArrow)
        {
            m_wstrDefaultTexturComTag = m_wstrInteractive_Tag;
            Change_Texture(m_wstrInteractive_Tag, TEXT("Com_DefaultTexture"));
        }

        if (true == m_isArrow)
            m_isRender = true;
    }
}

void CSelector_UI::Exception_Handle()
{
    if (nullptr == m_pPlayer)
    {
        Find_Player();
    }
}

void CSelector_UI::Find_Selector_Obj()
{
    list<CGameObject*>* pUILayer = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_UI"));

    if (nullptr == pUILayer)
        return;

    for (auto& iter : *pUILayer)
    {
        CSelector_UI* pSelectorObj = dynamic_cast<CSelector_UI*>(iter);

        if(nullptr != pSelectorObj)
        {
            if (this == pSelectorObj->m_pSupervise)
            {
               m_SelectorObj_Vec.push_back(pSelectorObj);

                Safe_AddRef(m_SelectorObj_Vec.back());
            }
        }
    }
}

void CSelector_UI::Reset()
{
    m_isRender = false;
    m_isInteractive = false;
}

CCustomize_UI* CSelector_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSelector_UI* pInstance = new CSelector_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CSelector_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSelector_UI::Clone(void* pArg)
{
    CSelector_UI* pInstance = new CSelector_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CSelector_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSelector_UI::Free()
{
    __super::Free();

    for (auto& iter : m_SelectorObj_Vec)
    {
        Safe_Release(iter);
    }
    m_SelectorObj_Vec.clear();
}
