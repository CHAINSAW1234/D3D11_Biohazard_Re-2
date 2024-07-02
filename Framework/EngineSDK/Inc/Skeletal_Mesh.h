#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

struct SubMesh
{
    uint16_t num_indices = 0;
    uint16_t base_vertex = 0;
    uint16_t base_index = 0;
};

class ENGINE_DLL SkeletalMesh
{
public:
    static SkeletalMesh* load(const std::string& name, class Skeleton* skeleton = nullptr);

    SkeletalMesh();
    ~SkeletalMesh();

    inline SubMesh&                         sub_mesh(uint32_t idx) { return m_sub_meshes[idx]; }
    inline size_t                           num_sub_meshes() { return (uint32_t)m_sub_meshes.size(); }
    inline Skeleton*                        skeleton() { return m_skeleton; }
private:
    std::vector<SubMesh>                    m_sub_meshes;
    Skeleton*                               m_skeleton;
};

END