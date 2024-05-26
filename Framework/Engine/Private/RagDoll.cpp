#include "Physics_Controller.h"
#include "RagDoll.h"
#include "RagDoll_Physics.h"
#include "Skeleton.h"

#define JOINT_COUNT 10

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

void CRagdoll::Free()
{
}

PxRigidDynamic* CRagdoll::find_recent_body(uint32_t idx, Skeleton* skeleton,uint32_t& chosen_idx)
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

void CRagdoll::set_kinematic(bool state)
{
    if(m_rigid_bodies.empty() == false)
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

AnimRagdoll::AnimRagdoll(Skeleton* skeleton) :
    m_skeleton(skeleton)
{
}

AnimRagdoll::~AnimRagdoll()
{
}

PoseTransforms* AnimRagdoll::apply(CRagdoll* ragdoll,_matrix model_scale, _matrix model_rotation)
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
            XMVECTOR global_joint_pos = XMVector4Transform(ragdoll->m_relative_joint_pos[i], global_transform);
            global_joint_pos = XMVectorSetW(global_joint_pos,1.f);

            XMVECTOR body_rot_quat = XMQuaternionRotationMatrix(global_transform);
            XMVECTOR diff_rot = XMQuaternionMultiply(XMQuaternionConjugate(ragdoll->m_original_body_rotations[i]), body_rot_quat);

            XMMATRIX translation = XMMatrixTranslationFromVector(global_joint_pos);

            XMVECTOR final_rotation_quat = XMQuaternionMultiply(joints[i].original_rotation, diff_rot);
            XMMATRIX rotation = XMMatrixRotationQuaternion(final_rotation_quat);

            XMMATRIX model = model_scale * model_rotation;

            //m_transforms.transforms[i] = XMMatrixInverse(nullptr, model) * translation * rotation * model_scale;
            m_transforms.transforms[i] = model_scale * rotation  *translation*XMMatrixInverse(nullptr, model);
        }
    }

    return &m_transforms;
}
