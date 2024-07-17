#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "Zombie.h"

BEGIN(Client)

class CPart_Mesh_Info_Zombie final : public CBase
{
    
public:
    typedef struct tagPartMeshInfoDesc
    {
        CPart_Mesh_Info_Zombie*         pChild_Part_Mesh_Info = { nullptr };
        vector<_int>*                   pMeshTypeIndices = { nullptr };
        CMonster::PART_ID               ePartID = { CMonster::PART_ID::PART_END };
    }PART_MESH_INFO_DESC;

    enum class BODY_PART_MESH_TYPE { _BODY, _INSIDE, _FROM_PARENT_DEFICIT, _FROM_CHILD_DEFICIT, _FROM_PARENT_DAMAGE, _FROM_CHILD_DAMAGE, _END };
    enum class SHIRT_PART_MESH_TYPE { _BODY, _ARM,  _END };
    enum class PANTS_PART_MESH_TYPE { _BODY, _LEG, _SHOES, _END };

private:
    CPart_Mesh_Info_Zombie();
    CPart_Mesh_Info_Zombie(const CPart_Mesh_Info_Zombie& rhs);
    virtual ~CPart_Mesh_Info_Zombie() = default;

public:
    HRESULT                             Initialize(void* pArg);

public:
    void                                Set_AdditionalChild(CPart_Mesh_Info_Zombie* pChildMeshInfo);

public:
    void                                Break(CModel* pModel);
    _int                                Get_MeshIndex_Type(BODY_PART_MESH_TYPE eType);
    vector<list<_int>>                  Get_Child_MeshIndices();

private:
    CMonster::PART_ID                   m_ePartID = { CMonster::PART_ID::PART_END };

    CPart_Mesh_Info_Zombie*             m_pChild_Part_Mesh_Info = { nullptr };
    CPart_Mesh_Info_Zombie*             m_pAdditional_Child_Part_Mesh_Info = { nullptr };
    vector<_int>                        m_MeshTypeIndices;

public:
    static CPart_Mesh_Info_Zombie* Create(void* pArg);
    virtual void Free() override;
};

END