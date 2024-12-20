#include "Physics_Controller.h"
#include "RagDoll.h"
#include "RagDoll_Physics.h"
#include "Skeleton.h"

#define JOINT_COUNT 10

#define MODEL_SCALE_INVERSE 100.f

CRagdoll::CRagdoll()
{
}

CRagdoll::CRagdoll(const CRagdoll& rhs)
{
}

HRESULT CRagdoll::Initialize_Prototype()
{
    return E_NOTIMPL;
}

HRESULT CRagdoll::Initialize(void* pArg)
{
    return E_NOTIMPL;
}

CRagdoll* CRagdoll::Create()
{
    return nullptr;
}

void CRagdoll::WakeUp()
{
    if (m_rigid_bodies.empty() == false)
    {
        for (int i = 0; i < m_rigid_bodies.size(); i++)
        {
            if (m_rigid_bodies[i])
            {
                m_rigid_bodies[i]->wakeUp();
            }
        }
    }
}

void CRagdoll::Free()
{
}

PxRigidDynamic* CRagdoll::find_recent_body(uint32_t idx, Skeleton* skeleton, uint32_t& chosen_idx)
{
    Joint* joints = skeleton->joints();

    chosen_idx = idx;
    PxRigidDynamic* body = m_rigid_bodies[idx];

    while (!body)
    {
        if (joints[idx].parent_index == -1)
        {
            return nullptr;
        }

        idx = joints[idx].parent_index;
        body = m_rigid_bodies[idx];
        chosen_idx = idx;
    }

    return body;
}

PxRigidDynamic* CRagdoll::find_recent_body(uint32_t idx, Skeleton* skeleton)
{
    Joint* joints = skeleton->joints();

    PxRigidDynamic* body = m_rigid_bodies[idx];

    while (!body)
    {
        if (joints[idx].parent_index == -1)
        {
            return nullptr;
        }

        idx = joints[idx].parent_index;
        body = m_rigid_bodies[idx];
    }

    return body;
}

PxRigidDynamic* CRagdoll::find_recent_body_BreakPart(uint32_t idx, Skeleton* skeleton, uint32_t& chosen_idx)
{
    Joint* joints = skeleton->joints();

    PxRigidDynamic* body;

    if (m_rigid_bodies_BreakPart[idx] == nullptr)
    {
        chosen_idx = idx;

        idx = joints[idx].parent_index_BreakPart;
        body = m_rigid_bodies[idx];
        chosen_idx = idx;
    }
    else
    {
        body = m_rigid_bodies_BreakPart[idx];
    }

    return body;
}

PxRigidDynamic* CRagdoll::find_recent_body_Cloth_Arm_R(uint32_t idx, Skeleton* skeleton, uint32_t& chosen_idx)
{
    Joint* joints = skeleton->joints();

    PxRigidDynamic* body;

    if (m_rigid_bodies_BreakPart_Cloth_Arm_R[idx] == nullptr)
    {
        chosen_idx = idx;

        idx = joints[idx].parent_index_BreakPart_Cloth_Arm_R;
        body = m_rigid_bodies[idx];
        chosen_idx = idx;
    }
    else
    {
        body = m_rigid_bodies_BreakPart_Cloth_Arm_R[idx];
    }

    return body;
}

PxRigidDynamic* CRagdoll::find_recent_body_Cloth_Leg_L(uint32_t idx, Skeleton* skeleton, uint32_t& chosen_idx)
{
    Joint* joints = skeleton->joints();

    PxRigidDynamic* body;

    if (m_rigid_bodies_BreakPart_Cloth_Leg_L[idx] == nullptr)
    {
        chosen_idx = idx;

        idx = joints[idx].parent_index_BreakPart_Cloth_Leg_L;
        body = m_rigid_bodies[idx];
        chosen_idx = idx;
    }
    else
    {
        body = m_rigid_bodies_BreakPart_Cloth_Leg_L[idx];
    }

    return body;
}

PxRigidDynamic* CRagdoll::find_recent_body_Cloth_Leg_R(uint32_t idx, Skeleton* skeleton, uint32_t& chosen_idx)
{
    Joint* joints = skeleton->joints();

    PxRigidDynamic* body;

    if (m_rigid_bodies_BreakPart_Cloth_Leg_R[idx] == nullptr)
    {
        chosen_idx = idx;

        idx = joints[idx].parent_index_BreakPart_Cloth_Leg_R;
        body = m_rigid_bodies[idx];
        chosen_idx = idx;
    }
    else
    {
        body = m_rigid_bodies_BreakPart_Cloth_Leg_R[idx];
    }

    return body;
}

PxRigidDynamic* CRagdoll::find_recent_body_Cloth_Arm_L(uint32_t idx, Skeleton* skeleton, uint32_t& chosen_idx)
{
    Joint* joints = skeleton->joints();

    PxRigidDynamic* body;

    if (m_rigid_bodies_BreakPart_Cloth_Arm_L[idx] == nullptr)
    {
        chosen_idx = idx;

        idx = joints[idx].parent_index_BreakPart_Cloth_Arm_L;
        body = m_rigid_bodies[idx];
        chosen_idx = idx;
    }
    else
    {
        body = m_rigid_bodies_BreakPart_Cloth_Arm_L[idx];
    }

    return body;
}

void CRagdoll::set_kinematic(bool state)
{
    if (m_rigid_bodies.empty() == false)
    {
        for (int i = 0; i < m_rigid_bodies.size(); i++)
        {
            if (m_rigid_bodies[i])
            {
                m_rigid_bodies[i]->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, !state);

                if (!state)
                    m_rigid_bodies[i]->wakeUp();
            }
        }
    }
}

void CRagdoll::set_kinematic_Partial(bool state)
{
    if (m_rigid_bodies.empty() == false)
    {
        for (int i = 0; i < m_rigid_bodies.size(); i++)
        {
            if (m_rigid_bodies[i])
            {
                m_rigid_bodies[i]->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, state);

                if (!state)
                    m_rigid_bodies[i]->wakeUp();
            }
        }
    }
}

void CRagdoll::set_kinematic_JumpScare(bool state)
{
    if (m_rigid_bodies.empty() == false)
    {
        for (int i = 0; i < m_rigid_bodies.size(); i++)
        {
            if (m_rigid_bodies[i])
            {
                m_rigid_bodies[i]->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, state);
            }
        }
    }
}

AnimRagdoll::AnimRagdoll(Skeleton* skeleton) :
    m_skeleton(skeleton)
{
}

AnimRagdoll::~AnimRagdoll()
{
}

PoseTransforms* AnimRagdoll::apply(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation)
{
    if (ragdoll->m_rigid_bodies.size() > 0)
    {
        Joint* joints = m_skeleton->joints();

        for (uint32_t i = 0; i < m_skeleton->num_bones(); i++)
        {
            uint32_t        chosen_idx;
            PxRigidDynamic* body = ragdoll->find_recent_body(i, m_skeleton, chosen_idx);

            if (body == nullptr)
            {
                m_transforms.transforms[i] = XMMatrixIdentity();
                continue;
            }
            XMMATRIX global_transform = to_mat4(body->getGlobalPose());
            global_transform.r[3] = XMVectorSet(XMVectorGetX(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetY(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetZ(global_transform.r[3]) * MODEL_SCALE_INVERSE, 1.f);
            XMVECTOR global_joint_pos = XMVector4Transform(ragdoll->m_relative_joint_pos[i], global_transform);
            global_joint_pos = XMVectorSetW(global_joint_pos, 1.f);

            XMVECTOR body_rot_quat = XMQuaternionRotationMatrix(global_transform);
            XMVECTOR diff_rot = XMQuaternionMultiply(XMQuaternionConjugate(ragdoll->m_original_body_rotations[i]), body_rot_quat);

            global_joint_pos = XMVectorScale(global_joint_pos, 1.f);
            XMMATRIX translation = XMMatrixTranslationFromVector(global_joint_pos);

            XMVECTOR final_rotation_quat = XMQuaternionMultiply(joints[i].original_rotation, diff_rot);
            XMMATRIX rotation = XMMatrixRotationQuaternion(final_rotation_quat);

            XMMATRIX model = model_scale * model_rotation;

            m_transforms.transforms[i] = model_scale * rotation * translation * XMMatrixInverse(nullptr, model);
        }
    }

    return &m_transforms;
}

PoseTransforms* AnimRagdoll::apply_BreakPart(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart)
{
    if (ragdoll->m_rigid_bodies.size() > 0)
    {
        Joint* joints = m_skeleton->joints();

        for (uint32_t i = 0; i < m_skeleton->num_bones(); i++)
        {
            uint32_t        chosen_idx;
            PxRigidDynamic* body = ragdoll->find_recent_body_BreakPart(i, m_skeleton, chosen_idx);

            if (body == nullptr || (*pBreakPart)[i] == false)
            {
                m_transforms_BreakPart.transforms[i] = XMMatrixIdentity();
                continue;
            }

            XMMATRIX global_transform = to_mat4(body->getGlobalPose());
            global_transform.r[3] = XMVectorSet(XMVectorGetX(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetY(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetZ(global_transform.r[3]) * MODEL_SCALE_INVERSE, 1.f);
            XMVECTOR global_joint_pos = XMVector4Transform(ragdoll->m_relative_joint_pos[i], global_transform);
            global_joint_pos = XMVectorSetW(global_joint_pos, 1.f);

            XMVECTOR body_rot_quat = XMQuaternionRotationMatrix(global_transform);
            XMVECTOR diff_rot = XMQuaternionMultiply(XMQuaternionConjugate(ragdoll->m_original_body_rotations[i]), body_rot_quat);

            global_joint_pos = XMVectorScale(global_joint_pos, 1.f);
            XMMATRIX translation = XMMatrixTranslationFromVector(global_joint_pos);

            XMVECTOR final_rotation_quat = XMQuaternionMultiply(joints[i].original_rotation, diff_rot);
            XMMATRIX rotation = XMMatrixRotationQuaternion(final_rotation_quat);

            XMMATRIX model = model_scale * model_rotation;

            m_transforms_BreakPart.transforms[i] = model_scale * rotation * translation * XMMatrixInverse(nullptr, model);
        }
    }

    return &m_transforms_BreakPart;
}

PoseTransforms* AnimRagdoll::apply_BreakPart_Leg(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart)
{
    if (ragdoll->m_rigid_bodies.size() > 0)
    {
        Joint* joints = m_skeleton->joints();

        for (uint32_t i = 0; i < m_skeleton->num_bones(); i++)
        {
            uint32_t        chosen_idx;
            PxRigidDynamic* body = ragdoll->find_recent_body_BreakPart(i, m_skeleton, chosen_idx);

            if (body == nullptr || (*pBreakPart)[i] == false)
            {
                m_transforms_BreakPart_Leg.transforms[i] = XMMatrixIdentity();
                continue;
            }

            XMMATRIX global_transform = to_mat4(body->getGlobalPose());
            global_transform.r[3] = XMVectorSet(XMVectorGetX(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetY(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetZ(global_transform.r[3]) * MODEL_SCALE_INVERSE, 1.f);
            XMVECTOR global_joint_pos = XMVector4Transform(ragdoll->m_relative_joint_pos_BreakPart[i], global_transform);
            global_joint_pos = XMVectorSetW(global_joint_pos, 1.f);

            XMVECTOR body_rot_quat = XMQuaternionRotationMatrix(global_transform);
            XMVECTOR diff_rot = XMQuaternionMultiply(XMQuaternionConjugate(ragdoll->m_original_body_rotations_BreakPart[i]), body_rot_quat);

            global_joint_pos = XMVectorScale(global_joint_pos, 1.f);
            XMMATRIX translation = XMMatrixTranslationFromVector(global_joint_pos);

            XMVECTOR final_rotation_quat = XMQuaternionMultiply(joints[i].original_rotation, diff_rot);
            XMMATRIX rotation = XMMatrixRotationQuaternion(final_rotation_quat);

            XMMATRIX model = model_scale * model_rotation;

            m_transforms_BreakPart_Leg.transforms[i] = model_scale * rotation * translation * XMMatrixInverse(nullptr, model);
        }
    }

    return &m_transforms_BreakPart_Leg;
}

PoseTransforms* AnimRagdoll::apply_BreakPart_Cloth_Leg_R(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart)
{
    if (ragdoll->m_rigid_bodies.size() > 0)
    {
        Joint* joints = m_skeleton->joints();

        for (uint32_t i = 0; i < m_skeleton->num_bones(); i++)
        {
            //uint32_t        chosen_idx;
            PxRigidDynamic* body = ragdoll->m_rigid_bodies[joints[i].parent_index_BreakPart_Cloth_Leg_R];

            if (body == nullptr || (*pBreakPart)[i] == false)
            {
                m_transforms_BreakPart_Cloth_Leg_R.transforms[i] = XMMatrixIdentity();
                continue;
            }

            XMMATRIX global_transform = to_mat4(body->getGlobalPose());
            global_transform.r[3] = XMVectorSet(XMVectorGetX(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetY(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetZ(global_transform.r[3]) * MODEL_SCALE_INVERSE, 1.f);
            XMVECTOR global_joint_pos = XMVector4Transform(ragdoll->m_relative_joint_pos_BreakPart_Cloth_Leg_R[i], global_transform);
            global_joint_pos = XMVectorSetW(global_joint_pos, 1.f);

            XMVECTOR body_rot_quat = XMQuaternionRotationMatrix(global_transform);
            XMVECTOR diff_rot = XMQuaternionMultiply(XMQuaternionConjugate(ragdoll->m_original_body_rotations_BreakPart_Cloth_Leg_R[i]), body_rot_quat);

            global_joint_pos = XMVectorScale(global_joint_pos, 1.f);
            XMMATRIX translation = XMMatrixTranslationFromVector(global_joint_pos);

            XMVECTOR final_rotation_quat = XMQuaternionMultiply(joints[i].original_rotation, diff_rot);
            XMMATRIX rotation = XMMatrixRotationQuaternion(final_rotation_quat);

            XMMATRIX model = model_scale * model_rotation;

            m_transforms_BreakPart_Cloth_Leg_R.transforms[i] = model_scale * rotation * translation * XMMatrixInverse(nullptr, model);
        }
    }

    return &m_transforms_BreakPart_Cloth_Leg_R;
}

void AnimRagdoll::apply_NoReturn(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation)
{
    if (ragdoll->m_rigid_bodies.size() > 0)
    {
        Joint* joints = m_skeleton->joints();

        for (uint32_t i = 0; i < m_skeleton->num_bones(); i++)
        {
            uint32_t        chosen_idx;
            PxRigidDynamic* body = ragdoll->find_recent_body(i, m_skeleton, chosen_idx);

            if (body == nullptr)
            {
                m_transforms.transforms[i] = XMMatrixIdentity();
                continue;
            }
            XMMATRIX global_transform = to_mat4(body->getGlobalPose());
            global_transform.r[3] = XMVectorSet(XMVectorGetX(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetY(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetZ(global_transform.r[3]) * MODEL_SCALE_INVERSE, 1.f);
            XMVECTOR global_joint_pos = XMVector4Transform(ragdoll->m_relative_joint_pos[i], global_transform);
            global_joint_pos = XMVectorSetW(global_joint_pos, 1.f);

            XMVECTOR body_rot_quat = XMQuaternionRotationMatrix(global_transform);
            XMVECTOR diff_rot = XMQuaternionMultiply(XMQuaternionConjugate(ragdoll->m_original_body_rotations[i]), body_rot_quat);

            global_joint_pos = XMVectorScale(global_joint_pos, 1.f);
            XMMATRIX translation = XMMatrixTranslationFromVector(global_joint_pos);

            XMVECTOR final_rotation_quat = XMQuaternionMultiply(joints[i].original_rotation, diff_rot);
            XMMATRIX rotation = XMMatrixRotationQuaternion(final_rotation_quat);

            XMMATRIX model = model_scale * model_rotation;

            m_transforms.transforms[i] = model_scale * rotation * translation * XMMatrixInverse(nullptr, model);
        }
    }
}

void AnimRagdoll::apply_BreakPart_NoReturn(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart)
{
    if (ragdoll->m_rigid_bodies.size() > 0)
    {
        Joint* joints = m_skeleton->joints();

        for (uint32_t i = 0; i < m_skeleton->num_bones(); i++)
        {
            uint32_t        chosen_idx;
            PxRigidDynamic* body = ragdoll->find_recent_body_BreakPart(i, m_skeleton, chosen_idx);

            if (body == nullptr || (*pBreakPart)[i] == false)
            {
                m_transforms_BreakPart.transforms[i] = XMMatrixIdentity();
                continue;
            }

            XMMATRIX global_transform = to_mat4(body->getGlobalPose());
            global_transform.r[3] = XMVectorSet(XMVectorGetX(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetY(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetZ(global_transform.r[3]) * MODEL_SCALE_INVERSE, 1.f);
            XMVECTOR global_joint_pos = XMVector4Transform(ragdoll->m_relative_joint_pos[i], global_transform);
            global_joint_pos = XMVectorSetW(global_joint_pos, 1.f);

            XMVECTOR body_rot_quat = XMQuaternionRotationMatrix(global_transform);
            XMVECTOR diff_rot = XMQuaternionMultiply(XMQuaternionConjugate(ragdoll->m_original_body_rotations[i]), body_rot_quat);

            global_joint_pos = XMVectorScale(global_joint_pos, 1.f);
            XMMATRIX translation = XMMatrixTranslationFromVector(global_joint_pos);

            XMVECTOR final_rotation_quat = XMQuaternionMultiply(joints[i].original_rotation, diff_rot);
            XMMATRIX rotation = XMMatrixRotationQuaternion(final_rotation_quat);

            XMMATRIX model = model_scale * model_rotation;

            m_transforms_BreakPart.transforms[i] = model_scale * rotation * translation * XMMatrixInverse(nullptr, model);
        }
    }
}

void AnimRagdoll::apply_BreakPart_Leg_NoReturn(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart)
{
    if (ragdoll->m_rigid_bodies.size() > 0)
    {
        Joint* joints = m_skeleton->joints();

        for (uint32_t i = 0; i < m_skeleton->num_bones(); i++)
        {
            uint32_t        chosen_idx;
            PxRigidDynamic* body = ragdoll->find_recent_body_BreakPart(i, m_skeleton, chosen_idx);

            if (body == nullptr || (*pBreakPart)[i] == false)
            {
                m_transforms_BreakPart_Leg.transforms[i] = XMMatrixIdentity();
                continue;
            }

            XMMATRIX global_transform = to_mat4(body->getGlobalPose());
            global_transform.r[3] = XMVectorSet(XMVectorGetX(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetY(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetZ(global_transform.r[3]) * MODEL_SCALE_INVERSE, 1.f);
            XMVECTOR global_joint_pos = XMVector4Transform(ragdoll->m_relative_joint_pos_BreakPart[i], global_transform);
            global_joint_pos = XMVectorSetW(global_joint_pos, 1.f);

            XMVECTOR body_rot_quat = XMQuaternionRotationMatrix(global_transform);
            XMVECTOR diff_rot = XMQuaternionMultiply(XMQuaternionConjugate(ragdoll->m_original_body_rotations_BreakPart[i]), body_rot_quat);

            global_joint_pos = XMVectorScale(global_joint_pos, 1.f);
            XMMATRIX translation = XMMatrixTranslationFromVector(global_joint_pos);

            XMVECTOR final_rotation_quat = XMQuaternionMultiply(joints[i].original_rotation, diff_rot);
            XMMATRIX rotation = XMMatrixRotationQuaternion(final_rotation_quat);

            XMMATRIX model = model_scale * model_rotation;

            m_transforms_BreakPart_Leg.transforms[i] = model_scale * rotation * translation * XMMatrixInverse(nullptr, model);
        }
    }
}

void AnimRagdoll::apply_BreakPart_Cloth_Arm_L_NoReturn(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart)
{
    if (ragdoll->m_rigid_bodies.size() > 0)
    {
        Joint* joints = m_skeleton->joints();

        for (uint32_t i = 0; i < m_skeleton->num_bones(); i++)
        {
            //uint32_t        chosen_idx;
            //PxRigidDynamic* body = ragdoll->m_rigid_bodies[joints[i].parent_index_BreakPart_Cloth_Arm_L];
            PxRigidDynamic* body = ragdoll->m_rigid_bodies_BreakPart_Cloth_Arm_L[i];

            if (body == nullptr || (*pBreakPart)[i] == false)
            {
                m_transforms_BreakPart_Cloth_Arm_L.transforms[i] = XMMatrixIdentity();
                continue;
            }

            XMMATRIX global_transform = to_mat4(body->getGlobalPose());
            global_transform.r[3] = XMVectorSet(XMVectorGetX(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetY(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetZ(global_transform.r[3]) * MODEL_SCALE_INVERSE, 1.f);
            XMVECTOR global_joint_pos = XMVector4Transform(ragdoll->m_relative_joint_pos_BreakPart_Cloth_Arm_L[i], global_transform);
            global_joint_pos = XMVectorSetW(global_joint_pos, 1.f);

            XMVECTOR body_rot_quat = XMQuaternionRotationMatrix(global_transform);
            XMVECTOR diff_rot = XMQuaternionMultiply(XMQuaternionConjugate(ragdoll->m_original_body_rotations_BreakPart_Cloth_Arm_L[i]), body_rot_quat);

            global_joint_pos = XMVectorScale(global_joint_pos, 1.f);
            XMMATRIX translation = XMMatrixTranslationFromVector(global_joint_pos);

            XMVECTOR final_rotation_quat = XMQuaternionMultiply(joints[i].original_rotation, diff_rot);
            XMMATRIX rotation = XMMatrixRotationQuaternion(final_rotation_quat);

            XMMATRIX model = model_scale * model_rotation;

            m_transforms_BreakPart_Cloth_Arm_L.transforms[i] = model_scale * rotation * translation * XMMatrixInverse(nullptr, model);
        }
    }
}

void AnimRagdoll::apply_BreakPart_Cloth_Arm_R_NoReturn(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart)
{
    if (ragdoll->m_rigid_bodies.size() > 0)
    {
        Joint* joints = m_skeleton->joints();

        for (uint32_t i = 0; i < m_skeleton->num_bones(); i++)
        {
            //uint32_t        chosen_idx;
            //PxRigidDynamic* body = ragdoll->m_rigid_bodies[joints[i].parent_index_BreakPart_Cloth_Arm_R];
            PxRigidDynamic* body = ragdoll->m_rigid_bodies_BreakPart_Cloth_Arm_R[i];

            if (body == nullptr || (*pBreakPart)[i] == false)
            {
                m_transforms_BreakPart_Cloth_Arm_R.transforms[i] = XMMatrixIdentity();
                continue;
            }

            XMMATRIX global_transform = to_mat4(body->getGlobalPose());
            global_transform.r[3] = XMVectorSet(XMVectorGetX(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetY(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetZ(global_transform.r[3]) * MODEL_SCALE_INVERSE, 1.f);
            XMVECTOR global_joint_pos = XMVector4Transform(ragdoll->m_relative_joint_pos_BreakPart_Cloth_Arm_R[i], global_transform);
            global_joint_pos = XMVectorSetW(global_joint_pos, 1.f);

            XMVECTOR body_rot_quat = XMQuaternionRotationMatrix(global_transform);
            XMVECTOR diff_rot = XMQuaternionMultiply(XMQuaternionConjugate(ragdoll->m_original_body_rotations_BreakPart_Cloth_Arm_R[i]), body_rot_quat);

            global_joint_pos = XMVectorScale(global_joint_pos, 1.f);
            XMMATRIX translation = XMMatrixTranslationFromVector(global_joint_pos);

            XMVECTOR final_rotation_quat = XMQuaternionMultiply(joints[i].original_rotation, diff_rot);
            XMMATRIX rotation = XMMatrixRotationQuaternion(final_rotation_quat);

            XMMATRIX model = model_scale * model_rotation;

            m_transforms_BreakPart_Cloth_Arm_R.transforms[i] = model_scale * rotation * translation * XMMatrixInverse(nullptr, model);
        }
    }
}

void AnimRagdoll::apply_BreakPart_Cloth_Leg_L_NoReturn(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart)
{
    if (ragdoll->m_rigid_bodies.size() > 0)
    {
        Joint* joints = m_skeleton->joints();

        for (uint32_t i = 0; i < m_skeleton->num_bones(); i++)
        {
            //uint32_t        chosen_idx;
            //PxRigidDynamic* body = ragdoll->m_rigid_bodies[joints[i].parent_index_BreakPart_Cloth_Leg_L];
            PxRigidDynamic* body = ragdoll->m_rigid_bodies_BreakPart_Cloth_Leg_L[i];

            if (body == nullptr || (*pBreakPart)[i] == false)
            {
                m_transforms_BreakPart_Cloth_Leg_L.transforms[i] = XMMatrixIdentity();
                continue;
            }

            XMMATRIX global_transform = to_mat4(body->getGlobalPose());
            global_transform.r[3] = XMVectorSet(XMVectorGetX(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetY(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetZ(global_transform.r[3]) * MODEL_SCALE_INVERSE, 1.f);
            XMVECTOR global_joint_pos = XMVector4Transform(ragdoll->m_relative_joint_pos_BreakPart_Cloth_Leg_L[i], global_transform);
            global_joint_pos = XMVectorSetW(global_joint_pos, 1.f);

            XMVECTOR body_rot_quat = XMQuaternionRotationMatrix(global_transform);
            XMVECTOR diff_rot = XMQuaternionMultiply(XMQuaternionConjugate(ragdoll->m_original_body_rotations_BreakPart_Cloth_Leg_L[i]), body_rot_quat);

            global_joint_pos = XMVectorScale(global_joint_pos, 1.f);
            XMMATRIX translation = XMMatrixTranslationFromVector(global_joint_pos);

            XMVECTOR final_rotation_quat = XMQuaternionMultiply(joints[i].original_rotation, diff_rot);
            XMMATRIX rotation = XMMatrixRotationQuaternion(final_rotation_quat);

            XMMATRIX model = model_scale * model_rotation;

            m_transforms_BreakPart_Cloth_Leg_L.transforms[i] = model_scale * rotation * translation * XMMatrixInverse(nullptr, model);
        }
    }
}

void AnimRagdoll::apply_BreakPart_Cloth_Leg_R_NoReturn(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart)
{
    if (ragdoll->m_rigid_bodies.size() > 0)
    {
        Joint* joints = m_skeleton->joints();

        for (uint32_t i = 0; i < m_skeleton->num_bones(); i++)
        {
            //uint32_t        chosen_idx;
            //PxRigidDynamic* body = ragdoll->m_rigid_bodies[joints[i].parent_index_BreakPart_Cloth_Leg_R];
            PxRigidDynamic* body = ragdoll->m_rigid_bodies_BreakPart_Cloth_Leg_R[i];

            if (body == nullptr || (*pBreakPart)[i] == false)
            {
                m_transforms_BreakPart_Cloth_Leg_R.transforms[i] = XMMatrixIdentity();
                continue;
            }

            XMMATRIX global_transform = to_mat4(body->getGlobalPose());
            global_transform.r[3] = XMVectorSet(XMVectorGetX(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetY(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetZ(global_transform.r[3]) * MODEL_SCALE_INVERSE, 1.f);
            XMVECTOR global_joint_pos = XMVector4Transform(ragdoll->m_relative_joint_pos_BreakPart_Cloth_Leg_R[i], global_transform);
            global_joint_pos = XMVectorSetW(global_joint_pos, 1.f);

            XMVECTOR body_rot_quat = XMQuaternionRotationMatrix(global_transform);
            XMVECTOR diff_rot = XMQuaternionMultiply(XMQuaternionConjugate(ragdoll->m_original_body_rotations_BreakPart_Cloth_Leg_R[i]), body_rot_quat);

            global_joint_pos = XMVectorScale(global_joint_pos, 1.f);
            XMMATRIX translation = XMMatrixTranslationFromVector(global_joint_pos);

            XMVECTOR final_rotation_quat = XMQuaternionMultiply(joints[i].original_rotation, diff_rot);
            XMMATRIX rotation = XMMatrixRotationQuaternion(final_rotation_quat);

            XMMATRIX model = model_scale * model_rotation;

            m_transforms_BreakPart_Cloth_Leg_R.transforms[i] = model_scale * rotation * translation * XMMatrixInverse(nullptr, model);
        }
    }
}

PoseTransforms* AnimRagdoll::apply_BreakPart_Cloth_Arm_L(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart)
{
    if (ragdoll->m_rigid_bodies.size() > 0)
    {
        Joint* joints = m_skeleton->joints();

        for (uint32_t i = 0; i < m_skeleton->num_bones(); i++)
        {
            //uint32_t        chosen_idx;
            PxRigidDynamic* body = ragdoll->m_rigid_bodies[joints[i].parent_index_BreakPart_Cloth_Arm_L];

            if (body == nullptr || (*pBreakPart)[i] == false)
            {
                m_transforms_BreakPart_Cloth_Arm_L.transforms[i] = XMMatrixIdentity();
                continue;
            }

            XMMATRIX global_transform = to_mat4(body->getGlobalPose());
            global_transform.r[3] = XMVectorSet(XMVectorGetX(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetY(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetZ(global_transform.r[3]) * MODEL_SCALE_INVERSE, 1.f);
            XMVECTOR global_joint_pos = XMVector4Transform(ragdoll->m_relative_joint_pos_BreakPart_Cloth_Arm_L[i], global_transform);
            global_joint_pos = XMVectorSetW(global_joint_pos, 1.f);

            XMVECTOR body_rot_quat = XMQuaternionRotationMatrix(global_transform);
            XMVECTOR diff_rot = XMQuaternionMultiply(XMQuaternionConjugate(ragdoll->m_original_body_rotations_BreakPart_Cloth_Arm_L[i]), body_rot_quat);

            global_joint_pos = XMVectorScale(global_joint_pos, 1.f);
            XMMATRIX translation = XMMatrixTranslationFromVector(global_joint_pos);

            XMVECTOR final_rotation_quat = XMQuaternionMultiply(joints[i].original_rotation, diff_rot);
            XMMATRIX rotation = XMMatrixRotationQuaternion(final_rotation_quat);

            XMMATRIX model = model_scale * model_rotation;

            m_transforms_BreakPart_Cloth_Arm_L.transforms[i] = model_scale * rotation * translation * XMMatrixInverse(nullptr, model);
        }
    }

    return &m_transforms_BreakPart_Cloth_Arm_L;
}

PoseTransforms* AnimRagdoll::apply_BreakPart_Cloth_Arm_R(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart)
{
    if (ragdoll->m_rigid_bodies.size() > 0)
    {
        Joint* joints = m_skeleton->joints();

        for (uint32_t i = 0; i < m_skeleton->num_bones(); i++)
        {
            //uint32_t        chosen_idx;
            PxRigidDynamic* body = ragdoll->m_rigid_bodies[joints[i].parent_index_BreakPart_Cloth_Arm_R];

            if (body == nullptr || (*pBreakPart)[i] == false)
            {
                m_transforms_BreakPart_Cloth_Arm_R.transforms[i] = XMMatrixIdentity();
                continue;
            }

            XMMATRIX global_transform = to_mat4(body->getGlobalPose());
            global_transform.r[3] = XMVectorSet(XMVectorGetX(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetY(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetZ(global_transform.r[3]) * MODEL_SCALE_INVERSE, 1.f);
            XMVECTOR global_joint_pos = XMVector4Transform(ragdoll->m_relative_joint_pos_BreakPart_Cloth_Arm_R[i], global_transform);
            global_joint_pos = XMVectorSetW(global_joint_pos, 1.f);

            XMVECTOR body_rot_quat = XMQuaternionRotationMatrix(global_transform);
            XMVECTOR diff_rot = XMQuaternionMultiply(XMQuaternionConjugate(ragdoll->m_original_body_rotations_BreakPart_Cloth_Arm_R[i]), body_rot_quat);

            global_joint_pos = XMVectorScale(global_joint_pos, 1.f);
            XMMATRIX translation = XMMatrixTranslationFromVector(global_joint_pos);

            XMVECTOR final_rotation_quat = XMQuaternionMultiply(joints[i].original_rotation, diff_rot);
            XMMATRIX rotation = XMMatrixRotationQuaternion(final_rotation_quat);

            XMMATRIX model = model_scale * model_rotation;

            m_transforms_BreakPart_Cloth_Arm_R.transforms[i] = model_scale * rotation * translation * XMMatrixInverse(nullptr, model);
        }
    }

    return &m_transforms_BreakPart_Cloth_Arm_R;
}

PoseTransforms* AnimRagdoll::apply_BreakPart_Cloth_Leg_L(CRagdoll* ragdoll, _matrix model_scale, _matrix model_rotation, vector<_bool>* pBreakPart)
{
    if (ragdoll->m_rigid_bodies.size() > 0)
    {
        Joint* joints = m_skeleton->joints();

        for (uint32_t i = 0; i < m_skeleton->num_bones(); i++)
        {
            //uint32_t        chosen_idx;
            PxRigidDynamic* body = ragdoll->m_rigid_bodies[joints[i].parent_index_BreakPart_Cloth_Leg_L];

            if (body == nullptr || (*pBreakPart)[i] == false)
            {
                m_transforms_BreakPart_Cloth_Leg_L.transforms[i] = XMMatrixIdentity();
                continue;
            }

            XMMATRIX global_transform = to_mat4(body->getGlobalPose());
            global_transform.r[3] = XMVectorSet(XMVectorGetX(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetY(global_transform.r[3]) * MODEL_SCALE_INVERSE, XMVectorGetZ(global_transform.r[3]) * MODEL_SCALE_INVERSE, 1.f);
            XMVECTOR global_joint_pos = XMVector4Transform(ragdoll->m_relative_joint_pos_BreakPart_Cloth_Leg_R[i], global_transform);
            global_joint_pos = XMVectorSetW(global_joint_pos, 1.f);

            XMVECTOR body_rot_quat = XMQuaternionRotationMatrix(global_transform);
            XMVECTOR diff_rot = XMQuaternionMultiply(XMQuaternionConjugate(ragdoll->m_original_body_rotations_BreakPart_Cloth_Leg_L[i]), body_rot_quat);

            global_joint_pos = XMVectorScale(global_joint_pos, 1.f);
            XMMATRIX translation = XMMatrixTranslationFromVector(global_joint_pos);

            XMVECTOR final_rotation_quat = XMQuaternionMultiply(joints[i].original_rotation, diff_rot);
            XMMATRIX rotation = XMMatrixRotationQuaternion(final_rotation_quat);

            XMMATRIX model = model_scale * model_rotation;

            m_transforms_BreakPart_Cloth_Leg_L.transforms[i] = model_scale * rotation * translation * XMMatrixInverse(nullptr, model);
        }
    }

    return &m_transforms_BreakPart_Cloth_Leg_L;
}
