#include "stdafx.h"
#include "Part_Mesh_Info_Zombie.h"

CPart_Mesh_Info_Zombie::CPart_Mesh_Info_Zombie()
{
}

CPart_Mesh_Info_Zombie::CPart_Mesh_Info_Zombie(const CPart_Mesh_Info_Zombie& rhs)
{
}

HRESULT CPart_Mesh_Info_Zombie::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    PART_MESH_INFO_DESC*            pDesc = { static_cast<PART_MESH_INFO_DESC*>(pArg) };


    if (nullptr == pDesc->pMeshTypeIndices)
        return E_FAIL;

    m_ePartID = pDesc->ePartID;
    m_MeshTypeIndices = *(pDesc->pMeshTypeIndices);
    m_pChild_Part_Mesh_Info = pDesc->pChild_Part_Mesh_Info;

    Safe_AddRef(m_pChild_Part_Mesh_Info);

    return S_OK;
}

void CPart_Mesh_Info_Zombie::Set_AdditionalChild(CPart_Mesh_Info_Zombie* pChildMeshInfo)
{
    Safe_Release(m_pAdditional_Child_Part_Mesh_Info);
    
    m_pAdditional_Child_Part_Mesh_Info = pChildMeshInfo;
    Safe_AddRef(m_pAdditional_Child_Part_Mesh_Info);
}

void CPart_Mesh_Info_Zombie::Break(CModel* pModel)
{
    //  Hide
    _int            iBodyMeshIndex = { m_MeshTypeIndices[static_cast<_uint>(BODY_PART_MESH_TYPE::_BODY)] };
    if (-1 != iBodyMeshIndex)
        pModel->Hide_Mesh(iBodyMeshIndex, true);

    _int            iInsideBodyMeshIndex = { m_MeshTypeIndices[static_cast<_uint>(BODY_PART_MESH_TYPE::_INSIDE)] };
    if (-1 != iInsideBodyMeshIndex)
        pModel->Hide_Mesh(iInsideBodyMeshIndex, true);

    //  Appear
    _int            iFromParentDeficitMeshIndex = { m_MeshTypeIndices[static_cast<_uint>(BODY_PART_MESH_TYPE::_FROM_PARENT_DEFICIT)] };
    if (-1 != iFromParentDeficitMeshIndex)
        pModel->Hide_Mesh(iFromParentDeficitMeshIndex, false);

    _int            iFromChildDeficitMeshIndex = { m_MeshTypeIndices[static_cast<_uint>(BODY_PART_MESH_TYPE::_FROM_CHILD_DEFICIT)] };
    if (-1 != iFromChildDeficitMeshIndex)
        pModel->Hide_Mesh(iFromChildDeficitMeshIndex, false);

    _int            iFromParentDamageMeshIndex = { m_MeshTypeIndices[static_cast<_uint>(BODY_PART_MESH_TYPE::_FROM_PARENT_DAMAGE)] };
    if (-1 != iFromParentDamageMeshIndex)
        pModel->Hide_Mesh(iFromParentDamageMeshIndex, false);

    _int            iFromChildDamageMeshIndex = { m_MeshTypeIndices[static_cast<_uint>(BODY_PART_MESH_TYPE::_FROM_CHILD_DAMAGE)] };
    if (-1 != iFromChildDamageMeshIndex)
        pModel->Hide_Mesh(iFromChildDamageMeshIndex, false);
}

_int CPart_Mesh_Info_Zombie::Get_MeshIndex_Type(BODY_PART_MESH_TYPE eType)
{
    if (eType >= BODY_PART_MESH_TYPE::_END)
        return -1;

    return m_MeshTypeIndices[static_cast<_uint>(eType)];
}

vector<list<_int>> CPart_Mesh_Info_Zombie::Get_Child_MeshIndices()
{
    vector<list<_int>>         ChildMeshIndices;
    ChildMeshIndices.resize(static_cast<_uint>(CMonster::PART_ID::PART_END));
    
    _int            iMeshIndex = { -1 };

    iMeshIndex = { m_MeshTypeIndices[static_cast<_uint>(BODY_PART_MESH_TYPE::_FROM_CHILD_DEFICIT)] };
    if (-1 != iMeshIndex)
        ChildMeshIndices[static_cast<_uint>(m_ePartID)].emplace_back(iMeshIndex);

    iMeshIndex = { m_MeshTypeIndices[static_cast<_uint>(BODY_PART_MESH_TYPE::_FROM_CHILD_DAMAGE)] };
    if (-1 != iMeshIndex)
        ChildMeshIndices[static_cast<_uint>(m_ePartID)].emplace_back(iMeshIndex);

    if (nullptr != m_pAdditional_Child_Part_Mesh_Info)
    {
        for (auto& iMeshIndex : m_pAdditional_Child_Part_Mesh_Info->m_MeshTypeIndices)
        {
            if (-1 == iMeshIndex)
                continue;

            ChildMeshIndices[static_cast<_uint>(m_pAdditional_Child_Part_Mesh_Info->m_ePartID)].emplace_back(iMeshIndex);
        }

    }

    CPart_Mesh_Info_Zombie*         pChildMeshInfo = { m_pChild_Part_Mesh_Info };
    while (nullptr != pChildMeshInfo)
    {
        for (auto& iChildMeshIndex : pChildMeshInfo->m_MeshTypeIndices)
        {
            if (-1 != iChildMeshIndex)
                ChildMeshIndices[static_cast<_uint>(m_ePartID)].emplace_back(iChildMeshIndex);
        }

        if (nullptr != pChildMeshInfo->m_pAdditional_Child_Part_Mesh_Info)
        {
            for (auto& iMeshIndex : pChildMeshInfo->m_pAdditional_Child_Part_Mesh_Info->m_MeshTypeIndices)
            {
                if (-1 == iMeshIndex)
                    continue;

                ChildMeshIndices[static_cast<_uint>(pChildMeshInfo->m_pAdditional_Child_Part_Mesh_Info->m_ePartID)].emplace_back(iMeshIndex);
            }
        }

        pChildMeshInfo = pChildMeshInfo->m_pChild_Part_Mesh_Info;
    }

    return ChildMeshIndices;
}

CPart_Mesh_Info_Zombie* CPart_Mesh_Info_Zombie::Create(void* pArg)
{
    CPart_Mesh_Info_Zombie*             pInstance = { new CPart_Mesh_Info_Zombie() };
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Created : CPart_Mesh_Info_Zombie"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPart_Mesh_Info_Zombie::Free()
{
    __super::Free();

    Safe_Release(m_pChild_Part_Mesh_Info);
    Safe_Release(m_pAdditional_Child_Part_Mesh_Info);
}
