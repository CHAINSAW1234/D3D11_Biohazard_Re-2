#include "stdafx.h"
#include "Player_Map_UI.h"
#include "Floor_Map_UI.h"
#include "Tab_Window.h"

#define ALPHA_ZERO				_float4(0.f, 0.f, 0.f, 0.f)
#define FLOOR_TYPE_BLENDING     0.244f 

CFloor_Map_UI::CFloor_Map_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMap_Manager{ pDevice, pContext }
{
}

CFloor_Map_UI::CFloor_Map_UI(const CFloor_Map_UI& rhs)
    : CMap_Manager{ rhs }
{

}

HRESULT CFloor_Map_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFloor_Map_UI::Initialize(void* pArg)
{
    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;

        CUSTOM_UI_DESC* CustomUIDesc = (CUSTOM_UI_DESC*)pArg;
    }

    m_isRender = true;

    return S_OK;
}

void CFloor_Map_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

    Rendering();

    Floor_Select();
}

void CFloor_Map_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CFloor_Map_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CFloor_Map_UI::Change_Tool()
{
    return S_OK;
}


/* Floor를 검색할 때 */
void CFloor_Map_UI::Floor_Select()
{
    /* ▶ Render 중이 아니라면 return  */
    if (false == m_isRender)
    {
        if (!m_vecTextBoxes.empty())
            m_vecTextBoxes.back()->Set_FontColor(ALPHA_ZERO);

        return;
    }

    /* ▶ Floor Search 설정 */
    if (m_eSelect_Floor >= MAP_FLOOR_TYPE::FLOOR_FREE)
    {
        Floor_Sort();

        if (m_eSelect_Floor >= MAP_FLOOR_TYPE::FLOOR_FREE)
            MSG_BOX(TEXT("Floor 설정이 제대로 적용되지 않았습니다.")); // Floor type을 고치면 된다.
    }

    /* ▶ 직접 Select 해서 이동할 Floor */
    if (MAP_UI_TYPE::FLOOR_TYPE_MAP == m_eMapComponent_Type)
    {
        if (true == IsMouseHover())
        {
            if (DOWN == m_pGameInstance->Get_KeyState(VK_LBUTTON))
            {
                *m_pMapPlayer->Get_ViewFloor_Type() = m_eSelect_Floor;
            }
        }
    }
}


void CFloor_Map_UI::Rendering()
{
    /* 1. Map Open */
    if (true == m_pTab_Window->Get_MinMapRender())
    {
         /* Plyaer가 가지고 있는 플로어와 Select된 플로어가 같다 */
         if (*m_pMapPlayer->Get_ViewFloor_Type() == m_eSelect_Floor)
         {
             if (!m_vecTextBoxes.empty())
             {
                 if (m_fBlending != FLOOR_TYPE_BLENDING)
                 {
                     m_vecTextBoxes.back()->Set_FontColor(_float4(1, 1, 1, 1));
                     m_vCurrentColor = _float4(1, 1, 1, 1);
                     m_fBlending = FLOOR_TYPE_BLENDING;
                 }
             }
         }
         else
         {
             if (m_fBlending < 0.f)
             {
                 m_vecTextBoxes.back()->Set_FontColor(m_vOriginTextColor);

                 m_fBlending = 0.f;
             }
         }
    }

    else if (false == m_pTab_Window->Get_MinMapRender())
    {
        m_vecTextBoxes.back()->Set_FontColor(ALPHA_ZERO);

        m_fBlending = 0.f;
    }

}

/* Floor 구분.  */
void CFloor_Map_UI::Floor_Sort()
{
    list<CGameObject*>* pUIList = m_pGameInstance->Find_Layer(g_Level, TEXT("Layer_UI"));
    vector<CFloor_Map_UI*> FloorVec;

    if (nullptr == pUIList)
        return;

    for (auto* iter : *pUIList)
    {
        CFloor_Map_UI* pFloor = dynamic_cast<CFloor_Map_UI*>(iter);

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

        auto CompareByY = [](CFloor_Map_UI* a, CFloor_Map_UI* b) {
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


CMap_Manager* CFloor_Map_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFloor_Map_UI* pInstance = new CFloor_Map_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CFloor_Map_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFloor_Map_UI::Clone(void* pArg)
{
    CFloor_Map_UI* pInstance = new CFloor_Map_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CFloor_Map_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFloor_Map_UI::Free()
{
    __super::Free();
}
