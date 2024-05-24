#include "Physics_Controller.h"
#include "RagDoll_Physics.h"
#include "RagDoll.h"
#include "Skeletal_Mesh.h"
#include "Skeleton.h"
#include "Bone.h"

#define JOINT_COUNT 20

PxRigidDynamic* CRagdoll_Physics::create_capsule_bone(uint32_t parent_idx, uint32_t child_idx, CRagdoll& ragdoll, float r, XMMATRIX rotation)
{
    //r *= 0.01f;

    Joint* joints = m_skeletal_mesh->skeleton()->joints();

    XMVECTOR parent_pos = XMLoadFloat3(&joints[parent_idx].bind_pos_ws(m_model));
    parent_pos = XMVectorSetW(parent_pos, 1.f);
    XMVECTOR child_pos = XMLoadFloat3(&joints[child_idx].bind_pos_ws(m_model));
    child_pos = XMVectorSetW(child_pos, 1.f);

    XMVECTOR diff = XMVectorSubtract(parent_pos, child_pos);
    float len = XMVectorGetX(XMVector3Length(diff));

    // shorten by 0.1 times to prevent overlap
    len -= len * 0.1f;

    float len_minus_2r = len - 2.0f * r;
    float half_height = len_minus_2r / 2.0f;

    XMVECTOR body_pos = XMVectorScale(XMVectorAdd(parent_pos, child_pos), 0.5f);
    body_pos = XMVectorSetW(body_pos, 1.f);

    PxShape* shape = m_Physics->createShape(PxCapsuleGeometry(r, half_height), *m_material);

    XMVECTOR rot_quat = XMQuaternionRotationMatrix(rotation);
    PxTransform local(to_quat(rot_quat));
    shape->setLocalPose(local);

    XMMATRIX inv_bind_pose = joints[parent_idx].inverse_bind_pose;
    inv_bind_pose.r[3] = XMVectorZero();
    inv_bind_pose.r[3] = XMVectorSetW(inv_bind_pose.r[3], 1.0f);
    XMMATRIX bind_pose = XMMatrixInverse(nullptr, inv_bind_pose);
    XMMATRIX bind_pose_ws = XMMatrixMultiply(m_model_without_scale, bind_pose);

    PxTransform px_transform(to_vec3(body_pos), to_quat(XMQuaternionRotationMatrix(bind_pose_ws)));

    PxRigidDynamic* body = m_Physics->createRigidDynamic(px_transform);
    body->setMass(m_mass);

    body->attachShape(*shape);

    m_ragdoll->m_rigid_bodies[parent_idx] = body;

    return body;
}

PxRigidDynamic* CRagdoll_Physics::create_capsule_bone(uint32_t parent_idx, CRagdoll& ragdoll, XMVECTOR offset, float l, float r, XMMATRIX rotation)
{

    //r *= 0.01f;

    Joint* joints = m_skeletal_mesh->skeleton()->joints();

    XMVECTOR parent_pos = XMLoadFloat3(&joints[parent_idx].bind_pos_ws(m_model)) + offset;
    parent_pos = XMVectorSetW(parent_pos, 1.f);

    PxShape* shape = m_Physics->createShape(PxCapsuleGeometry(r, l), *m_material);

    PxTransform local(to_quat(XMQuaternionRotationMatrix(rotation)));
    shape->setLocalPose(local);

    XMMATRIX invBindPose = joints[parent_idx].inverse_bind_pose;
    XMVECTOR scale, rotationQuat, translation;
    XMMatrixDecompose(&scale, &rotationQuat, &translation, invBindPose);
    XMMATRIX invBindPoseWithoutTranslation = XMMatrixScalingFromVector(scale) * XMMatrixRotationQuaternion(rotationQuat);


   // XMMATRIX inv_bind_pose = XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat3x3(&joints[parent_idx].inverse_bind_pose)));
    XMMATRIX inv_bind_pose = invBindPoseWithoutTranslation;
    inv_bind_pose.r[3] = XMVectorZero();
    inv_bind_pose.r[3] = XMVectorSetW(inv_bind_pose.r[3], 1.0f);
    XMMATRIX bind_pose = XMMatrixInverse(nullptr, inv_bind_pose);
    XMMATRIX bind_pose_ws = XMMatrixMultiply(m_model_without_scale, bind_pose);

    PxTransform px_transform(to_vec3(parent_pos), to_quat(XMQuaternionRotationMatrix(bind_pose_ws)));

    PxRigidDynamic* body = m_Physics->createRigidDynamic(px_transform);

    body->attachShape(*shape);
    body->setMass(m_mass);

    m_ragdoll->m_rigid_bodies[parent_idx] = body;

    return body;
}

PxRigidDynamic* CRagdoll_Physics::create_sphere_bone(uint32_t parent_idx, CRagdoll& ragdoll, float r)
{
    //r *= 0.01f;

    Joint* joints = m_skeletal_mesh->skeleton()->joints();
    XMVECTOR parent_pos = XMLoadFloat3(&joints[parent_idx].bind_pos_ws(m_model));
    parent_pos = XMVectorSetW(parent_pos, 1.f);

    PxShape* shape = m_Physics->createShape(PxSphereGeometry(r), *m_material);

    PxRigidDynamic* body = m_Physics->createRigidDynamic(PxTransform(to_vec3(parent_pos)));

    body->attachShape(*shape);
    body->setMass(m_mass);

    m_ragdoll->m_rigid_bodies[parent_idx] = body;

    return body;
}

void CRagdoll_Physics::create_d6_joint(PxRigidDynamic* parent, PxRigidDynamic* child, uint32_t joint_pos)
{
    Joint* joints = m_skeletal_mesh->skeleton()->joints();
    _vector p = XMLoadFloat3(&joints[joint_pos].bind_pos_ws(m_model));
    p = XMVectorSetW(p, 1.f);
    _vector q = XMQuaternionRotationMatrix(XMMatrixInverse(nullptr,joints[joint_pos].inverse_bind_pose));

    PxD6Joint* joint = PxD6JointCreate(*m_Physics,
        parent,
        parent->getGlobalPose().transformInv(PxTransform(to_vec3(p), to_quat(q))),
        child,
        child->getGlobalPose().transformInv(PxTransform(to_vec3(p), to_quat(q))));

    joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

    config_d6_joint(3.14 / 4.f, 3.14f / 4.f, -3.14f / 8.f, 3.14f / 8.f, joint);

    joint->setBreakForce(FLT_MAX, FLT_MAX); // 최대 힘과 토크를 무한대로 설정
}

void CRagdoll_Physics::create_revolute_joint(PxRigidDynamic* parent, PxRigidDynamic* child, uint32_t joint_pos, XMMATRIX rotation)
{
   Joint* joints = m_skeletal_mesh->skeleton()->joints();
   _vector p = XMLoadFloat3(&joints[joint_pos].bind_pos_ws(m_model));

    p = XMVectorSetW(p, 1.f);

    XMMATRIX parent_pose_inv = XMMatrixInverse(nullptr, to_mat4(parent->getGlobalPose()));
    XMMATRIX child_pose_inv = XMMatrixInverse(nullptr, to_mat4(child->getGlobalPose()));
    XMMATRIX joint_transform = XMMatrixTranslationFromVector(p)* rotation;

    PxRevoluteJoint* joint = PxRevoluteJointCreate(*m_Physics,
        parent,
        PxTransform(to_mat44(parent_pose_inv * joint_transform)),
        child,
        PxTransform(to_mat44(child_pose_inv * joint_transform)));

    joint->setConstraintFlag(PxConstraintFlag::eVISUALIZATION, true);

    joint->setLimit(PxJointAngularLimitPair(0.0f, PxPi));
    joint->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);

    joint->setBreakForce(FLT_MAX, FLT_MAX); // 최대 힘과 토크를 무한대로 설정
}

void CRagdoll_Physics::config_d6_joint(physx::PxReal swing0, physx::PxReal swing1, physx::PxReal twistLo, physx::PxReal twistHi, physx::PxD6Joint* joint)
{
    joint->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eLIMITED);
    joint->setMotion(physx::PxD6Axis::eSWING2, physx::PxD6Motion::eLIMITED);
    joint->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eLIMITED);

    joint->setSwingLimit(physx::PxJointLimitCone(swing0, swing1));
    joint->setTwistLimit(physx::PxJointAngularLimitPair(twistLo, twistHi));

    //PxD6Motion::Enum motion = PxD6Motion::eLIMITED;
    //joint->setMotion(PxD6Axis::eX, motion);
    //joint->setMotion(PxD6Axis::eY, motion);
    //joint->setMotion(PxD6Axis::eZ, motion);
    //// 이동 제한 값 설정 (최대 및 최소 거리)
    //PxReal linearLimitValue = 0.01f; // 1미터로 제한
    //PxJointLinearLimit linearLimit(linearLimitValue);
    //joint->setLinearLimit(linearLimit);
}

CRagdoll_Physics::CRagdoll_Physics(PxScene* Scene, PxPhysics* Physics, PxDefaultAllocator allocator, PxDefaultErrorCallback Callback,
    PxFoundation* Foundation, PxDefaultCpuDispatcher* dispatcher, PxMaterial* Material)
{
    m_Scene = Scene;
    m_Physics = Physics;
    m_allocator = allocator;
    m_error_callback = Callback;
    m_foundation = Foundation;
    m_dispatcher = dispatcher;
    m_material = Material;
}

CRagdoll_Physics::CRagdoll_Physics(const CRagdoll_Physics& rhs)
{
}

HRESULT CRagdoll_Physics::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CRagdoll_Physics::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

bool CRagdoll_Physics::load_mesh()
{
    m_skeletal_mesh = SkeletalMesh::load("../Bin/Resources/Models/LeonTest/LeonBody.fbx");

    if (!m_skeletal_mesh)
    {
        return false;
    }

    return true;
}

_bool CRagdoll_Physics::Init()
{
    m_ragdoll = new CRagdoll();

    m_selected_joints.resize(JOINT_COUNT);

    for (int i = 0; i < JOINT_COUNT; i++)
        m_selected_joints[i] = 0;

    // Load mesh.
    if (!load_mesh())
        return false;

    m_joint_names.push_back(" -");

    for (int i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
        m_joint_names.push_back(m_skeletal_mesh->skeleton()->joints()[i].name);

    for (int i = 0; i < MAX_BONES; i++)
        m_pose_transforms.transforms[i] = XMMatrixIdentity();

    m_index_stack.reserve(256);
    m_joint_pos.reserve(256);

    m_joint_mat.resize(MAX_BONES);

    update_transforms();

    create_ragdoll();

    m_ragdoll->set_kinematic(true);
    m_ragdoll_pose = new AnimRagdoll(m_skeletal_mesh->skeleton());

    return true;
}

void CRagdoll_Physics::update_transforms()
{
    XMMATRIX translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
    m_model_without_scale = XMMatrixMultiply(m_model_without_scale, translationMatrix);

    XMMATRIX scaleMatrix = XMMatrixScaling(m_scale, m_scale, m_scale);
    m_model = XMMatrixMultiply(m_model_without_scale, scaleMatrix);
    //m_character_transforms.model = m_model;

    m_model_only_scale = XMMatrixIdentity();
    m_model_only_scale = XMMatrixScaling(m_scale, m_scale, m_scale);
}

void CRagdoll_Physics::Set_Kinematic(_bool boolean)
{
    m_simulate = true;

    m_ragdoll->set_kinematic(boolean);
}

void CRagdoll_Physics::create_ragdoll()
{
    Joint* joints = m_skeletal_mesh->skeleton()->joints();

    m_skeletal_mesh->skeleton()->find_joint_index("Head");
   /* uint32_t j_neck_01_idx = m_skeletal_mesh->skeleton()->find_joint_index("Neck");
    uint32_t j_spine_03_idx = m_skeletal_mesh->skeleton()->find_joint_index("Spine2");
    uint32_t j_spine_02_idx = m_skeletal_mesh->skeleton()->find_joint_index("Spine1");
    uint32_t j_spine_01_idx = m_skeletal_mesh->skeleton()->find_joint_index("Spine");
    uint32_t j_pelvis_idx = m_skeletal_mesh->skeleton()->find_joint_index("Hips");*/

    uint32_t j_head_idx = 167;
    uint32_t j_neck_01_idx = 165;
    uint32_t j_spine_03_idx = 2;
    uint32_t j_spine_02_idx = 59;
    uint32_t j_spine_01_idx = 60;
    uint32_t j_pelvis_idx = 58;

    uint32_t j_thigh_l_idx =25;
    uint32_t j_calf_l_idx = 26;
    uint32_t j_foot_l_idx = 31;
    uint32_t j_ball_l_idx = 32;

    uint32_t j_thigh_r_idx = 39;
    uint32_t j_calf_r_idx = 40;
    uint32_t j_foot_r_idx = 45;
    uint32_t j_ball_r_idx = 46;

    uint32_t j_upperarm_l_idx = 82;
    uint32_t j_lowerarm_l_idx = 83;
    uint32_t j_hand_l_idx = 84;
    uint32_t j_middle_01_l_idx = 85;

    uint32_t j_upperarm_r_idx = 124;
    uint32_t j_lowerarm_r_idx = 125;
    uint32_t j_hand_r_idx = 126;
    uint32_t j_middle_01_r_idx = 127;

    m_ragdoll->m_rigid_bodies.resize(m_skeletal_mesh->skeleton()->num_bones());
    m_ragdoll->m_relative_joint_pos.resize(m_skeletal_mesh->skeleton()->num_bones());
    m_ragdoll->m_original_body_rotations.resize(m_skeletal_mesh->skeleton()->num_bones());
    m_ragdoll->m_body_pos_relative_to_joint.resize(m_skeletal_mesh->skeleton()->num_bones());
    m_ragdoll->m_original_joint_rotations.resize(m_skeletal_mesh->skeleton()->num_bones());

    for (int i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
        m_ragdoll->m_rigid_bodies[i] = nullptr;

    // ---------------------------------------------------------------------------------------------------------------
    // Create rigid bodies for limbs
    // ---------------------------------------------------------------------------------------------------------------

    float     r = 2.5f * m_scale;
    _matrix rot = XMMatrixIdentity();
    rot =XMMatrixRotationY(XM_PI)* XMMatrixRotationZ(XM_PI*0.5f);

    PxRigidDynamic* pelvis = create_capsule_bone(j_pelvis_idx, j_neck_01_idx, *m_ragdoll, 5.0f * m_scale, rot);
    m_ragdoll->m_rigid_bodies[2] = pelvis;
    PxRigidDynamic* head = create_capsule_bone(j_head_idx, *m_ragdoll, XMVectorSet(0.0f, 3.0f * m_scale, 0.0f,1.f), 4.0f * m_scale, 6.0f * m_scale, rot);

    PxRigidDynamic* l_leg = create_capsule_bone(j_thigh_l_idx, j_calf_l_idx, *m_ragdoll, r, rot);
    m_ragdoll->m_rigid_bodies[3] = l_leg;
    PxRigidDynamic* r_leg = create_capsule_bone(j_thigh_r_idx, j_calf_r_idx, *m_ragdoll, r, rot);

    PxRigidDynamic* l_calf = create_capsule_bone(j_calf_l_idx, j_foot_l_idx,*m_ragdoll, r, rot);
    PxRigidDynamic* r_calf = create_capsule_bone(j_calf_r_idx, j_foot_r_idx,*m_ragdoll, r, rot);

    PxRigidDynamic* l_arm = create_capsule_bone(j_upperarm_l_idx, j_lowerarm_l_idx,*m_ragdoll, r);
    PxRigidDynamic* r_arm = create_capsule_bone(j_upperarm_r_idx, j_lowerarm_r_idx,*m_ragdoll, r);

    PxRigidDynamic* l_forearm = create_capsule_bone(j_lowerarm_l_idx, j_hand_l_idx,*m_ragdoll, r);
    PxRigidDynamic* r_forearm = create_capsule_bone(j_lowerarm_r_idx, j_hand_r_idx,*m_ragdoll, r);

    PxRigidDynamic* l_hand = create_sphere_bone(j_middle_01_l_idx,*m_ragdoll, r);
    PxRigidDynamic* r_hand = create_sphere_bone(j_middle_01_r_idx,*m_ragdoll, r);

    rot = XMMatrixIdentity();

    PxRigidDynamic* l_foot = create_capsule_bone(j_foot_l_idx, j_ball_l_idx, *m_ragdoll, r, rot);
    PxRigidDynamic* r_foot = create_capsule_bone(j_foot_r_idx, j_ball_r_idx, *m_ragdoll, r, rot);

    for (int i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
    {
        uint32_t        chosen_idx;
        PxRigidDynamic* body = m_ragdoll->find_recent_body(i, m_skeletal_mesh->skeleton(), chosen_idx);

        if (!body)
            continue;

        _matrix body_global_transform = to_mat4(body->getGlobalPose());
        _matrix inv_body_global_transform = XMMatrixInverse(nullptr,body_global_transform);
        _matrix bind_pose_ws = XMMatrixMultiply(m_model_without_scale , XMMatrixInverse(nullptr,joints[i].inverse_bind_pose));
        _vector joint_pos_ws = XMLoadFloat3(&joints[i].bind_pos_ws(m_model));
        joint_pos_ws = XMVectorSetW(joint_pos_ws,1.f);

        _vector p = XMVector4Transform(joint_pos_ws,inv_body_global_transform);
        m_ragdoll->m_relative_joint_pos[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p),1.f);
        m_ragdoll->m_original_body_rotations[i] = XMQuaternionRotationMatrix(body_global_transform);

        if (m_ragdoll->m_rigid_bodies[i])
        {
            // Rigid body position relative to the joint
            _matrix m = XMMatrixInverse(nullptr,m_model * XMMatrixInverse(nullptr,joints[i].inverse_bind_pose));
            p = XMVector4Transform(XMVectorSetW(to_vec3(m_ragdoll->m_rigid_bodies[i]->getGlobalPose().p), 1.0f),m);

            m_ragdoll->m_body_pos_relative_to_joint[i] = XMVectorSet(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p),1.f);
            m_ragdoll->m_original_joint_rotations[i] = XMQuaternionRotationMatrix(bind_pose_ws);
        }
    }

    // ---------------------------------------------------------------------------------------------------------------
    // Add rigid bodies to scene
    // ---------------------------------------------------------------------------------------------------------------

    // Chest and Head
    m_Scene->addActor(*pelvis);
    m_Scene->addActor(*head);

    // Left Arm
    m_Scene->addActor(*l_arm);
    m_Scene->addActor(*l_forearm);
    m_Scene->addActor(*l_hand);

    // Right Arm
    m_Scene->addActor(*r_arm);
    m_Scene->addActor(*r_forearm);
    m_Scene->addActor(*r_hand);

    // Left Leg
    m_Scene->addActor(*l_leg);
    m_Scene->addActor(*l_calf);
    m_Scene->addActor(*l_foot);

    // Right Leg
    m_Scene->addActor(*r_leg);
    m_Scene->addActor(*r_calf);
    m_Scene->addActor(*r_foot);

    // ---------------------------------------------------------------------------------------------------------------
    // Create joints
    // ---------------------------------------------------------------------------------------------------------------

    // Chest and Head
    create_d6_joint(pelvis, head, j_neck_01_idx);

    // Chest to Thighs
    create_d6_joint(pelvis, l_leg, j_thigh_l_idx);
    create_d6_joint(pelvis, r_leg, j_thigh_r_idx);

    // Thighs to Calf
    create_revolute_joint(l_leg, l_calf, j_calf_l_idx);
    create_revolute_joint(r_leg, r_calf, j_calf_r_idx);

    // Calf to Foot
    create_d6_joint(l_calf, l_foot, j_foot_l_idx);
    create_d6_joint(r_calf, r_foot, j_foot_r_idx);

    // Chest to Upperarm
    create_d6_joint(pelvis, l_arm, j_upperarm_l_idx);
    create_d6_joint(pelvis, r_arm, j_upperarm_r_idx);

    XMMATRIX arm_rot = XMMatrixIdentity();
    //arm_rot = XMMatrixRotationX(XM_PI);

    // Upperarm to Lowerman
    create_d6_joint(l_arm, l_forearm, j_lowerarm_l_idx);

    arm_rot = XMMatrixIdentity();
    //arm_rot = XMMatrixRotationX(XM_PI);
    create_d6_joint(r_arm, r_forearm, j_lowerarm_r_idx);

    // Lowerarm to Hand
    create_d6_joint(l_forearm, l_hand, j_hand_l_idx);
    create_d6_joint(r_forearm, r_hand, j_hand_r_idx);
}

void CRagdoll_Physics::Update(_float fTimeDelta)
{
    // Update global uniforms.
    Joint* joints = m_skeletal_mesh->skeleton()->joints();

    for (int i = 0; i < m_skeletal_mesh->skeleton()->num_bones(); i++)
        m_pose_transforms.transforms[i] = XMMatrixInverse(nullptr,joints[i].inverse_bind_pose);

    // Update Skeleton
    update_animations();
}

void CRagdoll_Physics::update_animations()
{
   // Pose* rifle_pose = m_walk_sampler->sample(m_delta_seconds);

    PoseTransforms* global_transforms = nullptr;/* = m_local_to_global->generate_transforms(rifle_pose)*/;

    //if (!m_simulate)
    //{
    //    //for (int i = 0; i < m_ragdoll->m_rigid_bodies.size(); i++)
    //    //{
    //    //    if (m_ragdoll->m_rigid_bodies[i])
    //    //    {
    //    //        XMMATRIX global_transform = global_transforms->transforms[i];
    //    //        XMVECTOR body_pos_relative_to_joint = XMVectorSetW(m_ragdoll->m_body_pos_relative_to_joint[i],1.f);

    //    //        // 변환 행렬을 곱합니다.
    //    //        XMMATRIX transform = m_model * global_transform;

    //    //        // body_pos_relative_to_joint에 w=1.0f를 추가합니다.
    //    //        XMVECTOR pos_with_w = XMVectorSetW(body_pos_relative_to_joint, 1.0f);

    //    //        // 변환을 적용합니다.
    //    //        XMVECTOR temp = XMVector4Transform(pos_with_w, transform);

    //    //        _vector world_pos = XMVectorSet(XMVectorGetX(temp), XMVectorGetY(temp),XMVectorGetZ(temp), 1.f);

    //    //        XMMATRIX global_transform = global_transforms->transforms[i];

    //    //        // 두 행렬을 곱하여 최종 변환 행렬을 생성합니다.
    //    //        XMMATRIX final_transform = m_model_without_scale * global_transform;

    //    //        // 최종 변환 행렬에서 쿼터니언을 추출합니다.
    //    //        XMVECTOR joint_rot = XMQuaternionRotationMatrix(final_transform);

    //    //        glm::quat diff_rot = glm::conjugate(m_ragdoll.m_original_joint_rotations[i]) * joint_rot;

    //    //        // The apply the difference rotation to the original rigid body rotation to get the final rotation.
    //    //        glm::quat rotation = m_ragdoll.m_original_body_rotations[i] * diff_rot;

    //    //        PxTransform px_transform(to_vec3(world_pos), to_quat(rotation));

    //    //        m_ragdoll.m_rigid_bodies[i]->setGlobalPose(px_transform);
    //    //    }
    //    //}
    //}
    //else
        global_transforms = m_ragdoll_pose->apply(m_ragdoll, m_model_only_scale, m_model_without_scale);

    auto joint = m_skeletal_mesh->skeleton()->joints();
    if(global_transforms && m_vecBone)
    {
        int i = 0;
        for (auto& it : *m_vecBone)
        {
            for (int i = 0; i < 182; ++i)
            {
                if (it->Get_Name() == joint[i].name)
                {
                    if(!IsIdentityMatrix( global_transforms->transforms[i]))
                    {
                        it->Set_Combined_Matrix(global_transforms->transforms[i]);
                    }
                    //it->Set_TransformationMatrix(global_transforms->transforms[i]);
                }
            }
        }
    }

   // PoseTransforms* final_transforms = m_offset->offset(global_transforms);

   // update_bone_uniforms(final_transforms);
   // update_skeleton_debug(m_skeletal_mesh->skeleton(), global_transforms);
}


CRagdoll_Physics* CRagdoll_Physics::Create()
{
	return nullptr;
}

void CRagdoll_Physics::Free()
{
}
