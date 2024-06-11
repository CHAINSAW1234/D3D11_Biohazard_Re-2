#include "stdafx.h"

#include "Inventory.h"
#include "Camera_Free.h"

CInventory::CInventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CCustomize_UI{ pDevice, pContext }
{
}

CInventory::CInventory(const CInventory& rhs)
    : CCustomize_UI{ rhs }
{

}

HRESULT CInventory::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInventory::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_isRender = false;

    return S_OK;
}

void CInventory::Tick(_float fTimeDelta)
{
    if (DOWN == m_pGameInstance->Get_KeyState(VK_TAB))
    {
        m_isRender = !m_isRender;

        CGameObject* pGameObj = m_pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("Layer_ZZZCamera"))->back();

        if(nullptr != pGameObj)
        {
            CCamera_Free* pCameraObj = dynamic_cast<CCamera_Free*>(pGameObj);
            pCameraObj->Set_FixedMouse(!m_isRender);
        }
    }

    __super::Tick(fTimeDelta);
}

void CInventory::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta); 
}

HRESULT CInventory::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

void CInventory::Free()
{
    __super::Free();
}
