#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

inline PxQuat to_quat(const _fvector& q)
{
    return PxQuat(XMVectorGetX(q), XMVectorGetY(q), XMVectorGetZ(q), XMVectorGetW(q));
}

inline _vector to_quat(const PxQuat& q)
{
    return XMVectorSet(q.x, q.y, q.z, q.w);
}

inline PxVec3 to_vec3(const _fvector& vec3)
{
    return PxVec3(XMVectorGetX(vec3), XMVectorGetY(vec3), XMVectorGetZ(vec3));
}

inline _vector to_vec3(const PxVec3& vec3)
{
    return XMVectorSet(vec3.x, vec3.y, vec3.z, 1.f);
}

inline XMMATRIX to_mat4(const PxMat44& mat)
{
    return XMMATRIX(
        mat[0][0], mat[0][1], mat[0][2], mat[0][3],
        mat[1][0], mat[1][1], mat[1][2], mat[1][3],
        mat[2][0], mat[2][1], mat[2][2], mat[2][3],
        mat[3][0], mat[3][1], mat[3][2], mat[3][3]
    );
}

inline PxMat44 to_mat44(const XMMATRIX& mat)
{
    PxMat44 newMat;
    XMFLOAT4X4 temp;
    XMStoreFloat4x4(&temp, mat);

    newMat[0][0] = temp.m[0][0];
    newMat[0][1] = temp.m[0][1];
    newMat[0][2] = temp.m[0][2];
    newMat[0][3] = temp.m[0][3];

    newMat[1][0] = temp.m[1][0];
    newMat[1][1] = temp.m[1][1];
    newMat[1][2] = temp.m[1][2];
    newMat[1][3] = temp.m[1][3];

    newMat[2][0] = temp.m[2][0];
    newMat[2][1] = temp.m[2][1];
    newMat[2][2] = temp.m[2][2];
    newMat[2][3] = temp.m[2][3];

    newMat[3][0] = temp.m[3][0];
    newMat[3][1] = temp.m[3][1];
    newMat[3][2] = temp.m[3][2];
    newMat[3][3] = temp.m[3][3];

    return newMat;
}

class ENGINE_DLL CRagdoll_Physics : public CBase
{
public:

    PxRigidDynamic* create_capsule_bone(uint32_t parent_idx, uint32_t child_idx,class CRagdoll& ragdoll, float r = 0.5f, XMMATRIX rotation = XMMatrixIdentity());
    PxRigidDynamic* create_capsule_bone(uint32_t parent_idx, class CRagdoll& ragdoll, XMVECTOR offset, float l, float r = 0.5f, XMMATRIX rotation = XMMatrixIdentity());
    PxRigidDynamic* create_sphere_bone(uint32_t parent_idx, class CRagdoll& ragdoll, float r);
    void create_d6_joint(PxRigidDynamic* parent, PxRigidDynamic* child, uint32_t joint_pos);
    void create_revolute_joint(PxRigidDynamic* parent, PxRigidDynamic* child, uint32_t joint_pos, XMMATRIX rotation = XMMatrixIdentity());
    void config_d6_joint(physx::PxReal swing0, physx::PxReal swing1, physx::PxReal twistLo, physx::PxReal twistHi, physx::PxD6Joint* joint);
public:
	CRagdoll_Physics(PxScene* Scene,PxPhysics* Physics, PxDefaultAllocator allocator, PxDefaultErrorCallback Callback,
        PxFoundation* Foundation,PxDefaultCpuDispatcher* dispatcher, PxMaterial* Material);
	CRagdoll_Physics(const CRagdoll_Physics& rhs);
	virtual ~CRagdoll_Physics() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
    _bool Init();
    void create_ragdoll();
    void Update(_float fTimeDelta);
    _bool load_mesh();
    void  update_transforms();
public:
	static CRagdoll_Physics* Create();

private:
    PxScene* m_Scene = { nullptr };
    PxPhysics* m_Physics = { nullptr };
    PxDefaultAllocator      m_allocator;
    PxDefaultErrorCallback  m_error_callback;
    PxFoundation* m_foundation = nullptr;
    PxDefaultCpuDispatcher* m_dispatcher = nullptr;
    PxMaterial* m_material = nullptr;

    PoseTransforms m_pose_transforms;

    _matrix                        m_model;
    _matrix                        m_model_without_scale = XMMatrixIdentity();
    _matrix                        m_model_only_scale = XMMatrixIdentity();
    float                          m_scale = 1.f;
    float                          m_ui_scale = m_scale;
    float                          m_mass = 1.0f;

    class SkeletalMesh* m_skeletal_mesh = { nullptr };
    class CRagdoll* m_ragdoll = { nullptr };

    int32_t                               m_selected_node = -1;
    std::vector<_vector>                  m_joint_pos;
    std::vector<_matrix>                  m_joint_mat;
    std::vector<std::pair<int32_t, bool>> m_index_stack;

    std::vector<_int>        m_joint_Index;
    std::vector<int32_t>     m_selected_joints;
    std::vector<std::string> m_joint_names;
public:
	virtual void Free() override;
};

END