#include "stdafx.h"
#include "Map_UI.h"

#define RED         _float4(0.8, 0, 0, 0)
#define BLUE        _float4(0.0, 0.7569, 0.85, 0.0)
#define ALPHA_ZERO  _float4(0, 0, 0, 0)
#define BLENDING    0.7f

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
        m_eMapComponent_Type = CustomUIDesc->eMapComponent_Type;
    }

    /* �θ�� �������� ���� ����*/
    if(CCustomize_UI::MAP_UI_TYPE::MAIN_MAP == m_eMapComponent_Type)
    {
        if (false == m_IsChild)
            m_isRender = false;

        /* �ʼ� ��� */
        if (m_iWhichChild == (_uint)MAP_CHILD_TYPE::BACKGROUND_MAP)
        {
            m_isPlay = false;
            m_vColor[0].vColor = m_vCurrentColor = ALPHA_ZERO;
            m_vColor[0].fBlender_Value = m_fBlending = 1.f;
            m_vColor[0].isBlender = m_isBlending = true;
        }
    }

   
    return S_OK;
}

void CMap_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);
    
    // ����
    if (false == m_isGara)
    {
        Search_Map_Type(MAP_STATE_TYPE::SEARCH_CLEAR_STATE, MAIN_HOLL);
        Search_Map_Type(MAP_STATE_TYPE::SEARCH_STATE, ENTRANCE);
    }
   

    Transform_Adjustment();
    Mouse_Pos(fTimeDelta);

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
    const list<CGameObject*>* pMapUI_List = m_pGameInstance->Find_Layer(m_pGameInstance->Get_CurrentLevel(), TEXT("Layer_UI"));

    if(nullptr != pMapUI_List)
    {
        for (auto& iter : *pMapUI_List)
        {
            CMap_UI* pMap = dynamic_cast<CMap_UI*>(iter);

            if (nullptr != pMap)
            {
                if (_mapType == pMap->m_eMap_Location &&
                    CCustomize_UI::MAP_UI_TYPE::FONT_MASK_MAP != pMap->m_eMapComponent_Type &&
                    CCustomize_UI::MAP_UI_TYPE::MASK_MAP != pMap->m_eMapComponent_Type &&
                    MAP_CHILD_TYPE::BACKGROUND_MAP == (MAP_CHILD_TYPE)pMap->m_iWhichChild)
                {
                    m_isGara = true;
                    Change_Search_Type(_searType, pMap);
                }
            }
        }
    }
}

void CMap_UI::Change_Search_Type(MAP_STATE_TYPE _searType, CMap_UI* pMap)
{
    if (MAP_STATE_TYPE::NONE_STATE == _searType) // �⺻
    {
        pMap->m_vCurrentColor = ALPHA_ZERO;
        pMap->m_fBlending = 0.f;
    }

    else if (MAP_STATE_TYPE::SEARCH_STATE == _searType) // ���� ��
    {
        pMap->m_vCurrentColor = RED;
        pMap->m_fBlending = BLENDING;
    }

    else if (MAP_STATE_TYPE::SEARCH_CLEAR_STATE == _searType) // ���� �Ϸ�
    {
        pMap->m_vCurrentColor = BLUE;
        pMap->m_fBlending = BLENDING;
    }
}

void CMap_UI::Mouse_Pos(_float fTimeDelta)
{
    if (CCustomize_UI::MAP_UI_TYPE::MAIN_MAP == m_eMapComponent_Type || CCustomize_UI::MAP_UI_TYPE::FONT_MAP == m_eMapComponent_Type)
    {
        if (PRESSING == m_pGameInstance->Get_KeyState(VK_LBUTTON))
        {
            POINT		ptDeltaPos = m_pGameInstance->Get_MouseDeltaPos();

            if ((_long)0 != ptDeltaPos.x)
            {
                _float4 pos = {};
                pos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
                pos.x += fTimeDelta * (_float)ptDeltaPos.x * fMouseSensor;
                m_pTransformCom->Set_State(CTransform::STATE_POSITION, pos);

                /* FONT */
                if (!m_vecTextBoxes.empty())
                {
                    for (auto* iter : m_vecTextBoxes)
                    {
                        _float3 fOriginFont = iter->GetPosition();
                        fOriginFont.x += fTimeDelta * (_float)ptDeltaPos.x * fMouseSensor;

                        CTransform* pos = dynamic_cast<CTransform*>(iter->Get_Component(g_strTransformTag));
                        pos->Set_State(CTransform::STATE_POSITION, _float4(fOriginFont.x, fOriginFont.y, fOriginFont.z, 1.f));
                    }
                }
            }


            if ((_long)0 != ptDeltaPos.y)
            {
                _float4 pos = {};
                pos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
                pos.y -= fTimeDelta * (_float)ptDeltaPos.y * fMouseSensor;
                m_pTransformCom->Set_State(CTransform::STATE_POSITION, pos);

                /* FONT */
                if (!m_vecTextBoxes.empty())
                {
                    for (auto* iter : m_vecTextBoxes)
                    {
                        _float3 fOriginFont = iter->GetPosition();
                        fOriginFont.y -= fTimeDelta * (_float)ptDeltaPos.y * fMouseSensor;

                        CTransform* pos = dynamic_cast<CTransform*>(iter->Get_Component(g_strTransformTag));
                        pos->Set_State(CTransform::STATE_POSITION, _float4(fOriginFont.x, fOriginFont.y, fOriginFont.z, 1.f));
                    }
                }
            }
        }
    }
}

void CMap_UI::Transform_Adjustment()
{
    /* Main */
    if (CCustomize_UI::MAP_UI_TYPE::MAIN_MAP == m_eMapComponent_Type)
    {
        _float4 vMainPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
        vMainPos.z = 0.1f;
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMainPos);
    }

    /* Mask */
    else if (CCustomize_UI::MAP_UI_TYPE::MASK_MAP == m_eMapComponent_Type)
    {
        _float4 vMainPos = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
        vMainPos.z = 0.05f;
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMainPos);
    }

    else if (CCustomize_UI::MAP_UI_TYPE::FONT_MAP == m_eMapComponent_Type)
    {
        if (!m_vecTextBoxes.empty())
        {
            for (auto& iter : m_vecTextBoxes)
            {
                CTransform* pFontTrans = dynamic_cast<CTransform*>(iter->Get_Component(g_strTransformTag));
                _float4 vFontTrans = pFontTrans->Get_State_Float4(CTransform::STATE_POSITION);
                vFontTrans.z = 0.0f;
                pFontTrans->Set_State(CTransform::STATE_POSITION, vFontTrans);
            }
        }
    }
}

void CMap_UI::EX_ColorChange()
{
    if (CCustomize_UI::MAP_UI_TYPE::MAIN_MAP == m_eMapComponent_Type)
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

}
