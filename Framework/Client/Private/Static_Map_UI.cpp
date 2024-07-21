#include "stdafx.h"
#include "Static_Map_UI.h"

#define ALPHA_ZERO _float4(0.f, 0.f, 0.f, 0.f)

CStatic_Map_UI::CStatic_Map_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMap_Manager{ pDevice, pContext }
{
}

CStatic_Map_UI::CStatic_Map_UI(const CStatic_Map_UI& rhs)
    : CMap_Manager{ rhs }
{

}

HRESULT CStatic_Map_UI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CStatic_Map_UI::Initialize(void* pArg)
{
    if (pArg != nullptr)
    {
        if (FAILED(__super::Initialize(pArg)))
            return E_FAIL;
    }

    return S_OK;
}

void CStatic_Map_UI::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);
}


void CStatic_Map_UI::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CStatic_Map_UI::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CStatic_Map_UI::Change_Tool()
 {
    return S_OK;
}

void CStatic_Map_UI::Rendering()
{
    
}

CMap_Manager* CStatic_Map_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CStatic_Map_UI* pInstance = new CStatic_Map_UI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Created : CStatic_Map_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CStatic_Map_UI::Clone(void* pArg)
{
    CStatic_Map_UI* pInstance = new CStatic_Map_UI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CStatic_Map_UI"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CStatic_Map_UI::Free()
{
    __super::Free();
}
