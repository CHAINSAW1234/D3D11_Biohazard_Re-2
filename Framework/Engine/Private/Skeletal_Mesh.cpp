#include "Skeletal_Mesh.h"
#include "RagDoll_Physics.h"
#include "Skeleton.h"

#define JOINT_COUNT 10

SkeletalMesh* SkeletalMesh::load(const std::string& name, Skeleton* skeleton)
{
    _uint		iOption = { aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded };

    const aiScene* scene;
    Assimp::Importer importer;
    scene = importer.ReadFile(name, iOption);

    SkeletalMesh* skeletal_mesh = new SkeletalMesh();

    if (skeleton)
        skeletal_mesh->m_skeleton = skeleton;
    else
        skeletal_mesh->m_skeleton = Skeleton::create(scene);

    return skeletal_mesh;
}

SkeletalMesh::SkeletalMesh()
{
}

SkeletalMesh::~SkeletalMesh()
{
}