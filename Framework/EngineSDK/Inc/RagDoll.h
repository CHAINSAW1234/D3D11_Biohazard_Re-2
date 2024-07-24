#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CRagdoll : public CBase
{
public:
    CRagdoll();
    CRagdoll(const CRagdoll& rhs);
    virtual ~CRagdoll() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* pArg);

public:
    static CRagdoll* Create();
public:
    std::vector<PxRigidDynamic*>                m_rigid_bodies;
    std::vector<PxRigidDynamic*>                m_rigid_bodies_BreakPart;
    std::vector<PxRigidDynamic*>                m_rigid_bodies_BreakPart_Cloth_Arm_L;
    std::vector<PxRigidDynamic*>                m_rigid_bodies_BreakPart_Cloth_Arm_R;
    std::vector<PxRigidDynamic*>                m_rigid_bodies_BreakPart_Cloth_Leg_L;
    std::vector<PxRigidDynamic*>                m_rigid_bodies_BreakPart_Cloth_Leg_R;
    std::vector<_vector>                        m_relative_joint_pos;
    std::vector<_vector>                        m_original_body_rotations;

    std::vector<_vector>                        m_body_pos_relative_to_joint;
    std::vector<_vector>                        m_original_joint_rotations;

    std::vector<_vector>                        m_relative_joint_pos_BreakPart;
    std::vector<_vector>                        m_original_body_rotations_BreakPart;
    std::vector<_vector>                        m_body_pos_relative_to_joint_BreakPart;
    std::vector<_vector>                        m_original_joint_rotations_BreakPart;

    std::vector<_vector>                        m_relative_joint_pos_BreakPart_Cloth_Arm_L;
    std::vector<_vector>                        m_original_body_rotations_BreakPart_Cloth_Arm_L;
    std::vector<_vector>                        m_body_pos_relative_to_joint_BreakPart_Cloth_Arm_L;
    std::vector<_vector>                        m_original_joint_rotations_BreakPart_Cloth_Arm_L;

    std::vector<_vector>                        m_relative_joint_pos_BreakPart_Cloth_Arm_R;
    std::vector<_vector>                        m_original_body_rotations_BreakPart_Cloth_Arm_R;
    std::vector<_vector>                        m_body_pos_relative_to_joint_BreakPart_Cloth_Arm_R;
    std::vector<_vector>                        m_original_joint_rotations_BreakPart_Cloth_Arm_R;


    std::vector<_vector>                        m_relative_joint_pos_BreakPart_Cloth_Leg_L;
    std::vector<_vector>                        m_original_body_rotations_BreakPart_Cloth_Leg_L;
    std::vector<_vector>                        m_body_pos_relative_to_joint_BreakPart_Cloth_Leg_L;
    std::vector<_vector>                        m_original_joint_rotations_BreakPart_Cloth_Leg_L;


    std::vector<_vector>                        m_relative_joint_pos_BreakPart_Cloth_Leg_R;
    std::vector<_vector>                        m_original_body_rotations_BreakPart_Cloth_Leg_R;
    std::vector<_vector>                        m_body_pos_relative_to_joint_BreakPart_Cloth_Leg_R;
    std::vector<_vector>                        m_original_joint_rotations_BreakPart_Cloth_Leg_R;


    PxRigidDynamic* find_recent_body(uint32_t idx, class Skeleton* skeleton, uint32_t& chosen_idx);
    PxRigidDynamic* find_recent_body(uint32_t idx, class Skeleton* skeleton);
    PxRigidDynamic* find_recent_body_BreakPart(uint32_t idx, class Skeleton* skeleton, uint32_t& chosen_idx);
    PxRigidDynamic* find_recent_body_Cloth_Arm_L(uint32_t idx, class Skeleton* skeleton, uint32_t& chosen_idx);
    PxRigidDynamic* find_recent_body_Cloth_Arm_R(uint32_t idx, class Skeleton* skeleton, uint32_t& chosen_idx);
    PxRigidDynamic* find_recent_body_Cloth_Leg_L(uint32_t idx, class Skeleton* skeleton, uint32_t& chosen_idx);
    PxRigidDynamic* find_recent_body_Cloth_Leg_R(uint32_t idx, class Skeleton* skeleton, uint32_t& chosen_idx);
    void                                        set_kinematic(bool state);
    void                                        set_kinematic_Partial(bool state);
    XMVECTOR                                    pos_from_transform(const XMMATRIX& m)
    {
        return m.r[3];
    }
    void                                        WakeUp();
public:
    virtual void Free() override;
};

class ENGINE_DLL AnimRagdoll : public CBase
{
public:
    AnimRagdoll(Skeleton* skeleton);
    ~AnimRagdoll();
    struct PoseTransforms* apply(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation);
    struct PoseTransforms* apply_BreakPart(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart);
    struct PoseTransforms* apply_BreakPart_Leg(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart);
    struct PoseTransforms* apply_BreakPart_Cloth_Arm_L(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart);
    struct PoseTransforms* apply_BreakPart_Cloth_Arm_R(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart);
    struct PoseTransforms* apply_BreakPart_Cloth_Leg_L(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart);
    struct PoseTransforms* apply_BreakPart_Cloth_Leg_R(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart);

    void apply_NoReturn(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation);
    void apply_BreakPart_NoReturn(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart);
    void apply_BreakPart_Leg_NoReturn(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart);
    void apply_BreakPart_Cloth_Arm_L_NoReturn(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart);
    void apply_BreakPart_Cloth_Arm_R_NoReturn(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart);
    void apply_BreakPart_Cloth_Leg_L_NoReturn(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart);
    void apply_BreakPart_Cloth_Leg_R_NoReturn(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart);

    struct PoseTransforms* GetRagdollTransform()
    {
        return &m_transforms;
    }
    struct PoseTransforms* GetRagdollTransform_BreakPart()
    {
        return &m_transforms_BreakPart;
    }
    struct PoseTransforms* GetRagdollTransform_Leg()
    {
        return &m_transforms_BreakPart_Leg;
    }
    struct PoseTransforms* GetRagdollTransform_Arm_L()
    {
        return &m_transforms_BreakPart_Cloth_Arm_L;
    }
    struct PoseTransforms* GetRagdollTransform_Arm_R()
    {
        return &m_transforms_BreakPart_Cloth_Arm_R;
    }
    struct PoseTransforms* GetRagdollTransform_Leg_L()
    {
        return &m_transforms_BreakPart_Cloth_Leg_L;
    }
    struct PoseTransforms* GetRagdollTransform_Leg_R()
    {
        return &m_transforms_BreakPart_Cloth_Leg_R;
    }

private:
    Skeleton* m_skeleton;
    PoseTransforms  m_transforms;
    PoseTransforms  m_transforms_BreakPart;
    PoseTransforms  m_transforms_BreakPart_Leg;
    PoseTransforms  m_transforms_BreakPart_Cloth_Arm_L;
    PoseTransforms  m_transforms_BreakPart_Cloth_Arm_R;
    PoseTransforms  m_transforms_BreakPart_Cloth_Leg_L;
    PoseTransforms  m_transforms_BreakPart_Cloth_Leg_R;
};

END