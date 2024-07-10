#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

inline PxQuat               to_quat(const _fvector& q)
{
    return PxQuat(XMVectorGetX(q), XMVectorGetY(q), XMVectorGetZ(q), XMVectorGetW(q));
}

inline _vector              to_quat(const PxQuat& q)
{
    return XMVectorSet(q.x, q.y, q.z, q.w);
}

inline PxVec3               to_vec3(const _fvector& vec3)
{
    return PxVec3(XMVectorGetX(vec3), XMVectorGetY(vec3), XMVectorGetZ(vec3));
}

inline _vector              to_vec3(const PxVec3& vec3)
{
    return XMVectorSet(vec3.x, vec3.y, vec3.z, 1.f);
}

inline XMVECTOR             QuaternionFromVectors(const XMVECTOR& _vec1, const XMVECTOR& _vec2) {
    _vector vec1 = XMVector3Normalize(_vec1);
    _vector vec2 = XMVector3Normalize(_vec2);

    float dot = XMVectorGetX(XMVector3Dot(vec1, vec2));

    if (dot < -0.999999f)
    {
        XMVECTOR orthogonalVector = XMVector3Orthogonal(vec1);
        return XMQuaternionRotationAxis(orthogonalVector, XM_PI);
    }

    XMVECTOR axis = XMVector3Cross(vec1, vec2);

    float angle = acosf(dot);
    XMVECTOR quaternion = XMQuaternionRotationAxis(axis, angle);

    return quaternion;
}

inline XMVECTOR CalculateTransitionQuaternion(XMVECTOR q1, XMVECTOR q2)
{
    XMVECTOR q1Inverse = XMQuaternionInverse(q1);

    XMVECTOR transitionQuaternion = XMQuaternionMultiply(XMQuaternionNormalize(q1Inverse), XMQuaternionNormalize(q2));

    transitionQuaternion = XMQuaternionNormalize(transitionQuaternion);

    return transitionQuaternion;
}

inline XMVECTOR VectorToQuaternion(XMVECTOR direction)
{
    direction = XMVector3Normalize(direction);

    XMVECTOR zAxis = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

    if (XMVector3NearEqual(direction, zAxis, XMVectorSet(1e-6f, 1e-6f, 1e-6f, 1e-6f)))
    {
        return XMQuaternionIdentity();
    }

    if (XMVector3NearEqual(direction, XMVectorNegate(zAxis), XMVectorSet(1e-6f, 1e-6f, 1e-6f, 1e-6f)))
    {
        return XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XM_PI);
    }

    XMVECTOR rotationAxis = XMVector3Cross(zAxis, direction);
    rotationAxis = XMVector3Normalize(rotationAxis);

    float dotProduct = XMVectorGetX(XMVector3Dot(zAxis, direction));
    float rotationAngle = acosf(dotProduct);

    XMVECTOR quaternion = XMQuaternionRotationAxis(rotationAxis, rotationAngle);

    return quaternion;
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

    PxRigidDynamic*                     create_capsule_bone(uint32_t parent_idx, uint32_t child_idx,class CRagdoll& ragdoll, float r = 0.5f, XMMATRIX rotation = XMMatrixIdentity(),COLLIDER_TYPE eType = COLLIDER_TYPE::_END);
    PxRigidDynamic*                     create_capsule_bone(uint32_t parent_idx, class CRagdoll& ragdoll, XMVECTOR offset, float l, float r = 0.5f, XMMATRIX rotation = XMMatrixIdentity(), COLLIDER_TYPE eType = COLLIDER_TYPE::_END);
    PxRigidDynamic*                     create_sphere_bone(uint32_t parent_idx, class CRagdoll& ragdoll, float r, COLLIDER_TYPE eType = COLLIDER_TYPE::_END);
    PxD6Joint*                          create_d6_joint(PxRigidDynamic* parent, PxRigidDynamic* child, uint32_t Bone_Pos, uint32_t Joint_Pos);
    PxD6Joint*                          create_d6_joint_Foot(PxRigidDynamic* parent, PxRigidDynamic* child, uint32_t Bone_Pos, uint32_t Joint_Pos);
    PxD6Joint*                          create_d6_joint_Head(PxRigidDynamic* parent, PxRigidDynamic* child, uint32_t Bone_Pos, uint32_t Joint_Pos);
    void                                create_revolute_joint(PxRigidDynamic* parent, PxRigidDynamic* child, uint32_t joint_pos, XMMATRIX rotation = XMMatrixIdentity());
    void                                config_d6_joint(physx::PxReal swing0, physx::PxReal swing1, physx::PxReal twistLo, physx::PxReal twistHi, physx::PxD6Joint* joint);
public:
	CRagdoll_Physics(PxScene* Scene,PxPhysics* Physics, PxDefaultAllocator allocator, PxDefaultErrorCallback Callback,
        PxFoundation* Foundation,PxDefaultCpuDispatcher* dispatcher, PxMaterial* Material);
	CRagdoll_Physics(const CRagdoll_Physics& rhs);
	virtual ~CRagdoll_Physics() = default;
public:
	virtual HRESULT                     Initialize_Prototype();
	virtual HRESULT                     Initialize(void* pArg);

public:
    _bool                               Init(const string& name);
    void                                create_ragdoll();
    void                                Update(_float fTimeDelta);
    _bool                               load_mesh(const string& name);
    void                                update_transforms();
    void                                Set_Kinematic(_bool boolean);
    void                                update_animations();
    void                                Init_Ragdoll();
    void                                create_joint();
    void			                    SetBone_Ragdoll(vector<class CBone*>* vecBone)
    {
        m_vecBone = vecBone;
    }
    bool                                IsIdentityMatrix(const _matrix& matrix)
    {
        if (XMVectorGetX(matrix.r[0]) == 1.0f && XMVectorGetY(matrix.r[0]) == 0.0f &&
            XMVectorGetZ(matrix.r[0]) == 0.0f && XMVectorGetW(matrix.r[0]) == 0.0f &&

            XMVectorGetX(matrix.r[1]) == 0.0f && XMVectorGetY(matrix.r[1]) == 1.0f &&
            XMVectorGetZ(matrix.r[1]) == 0.0f && XMVectorGetW(matrix.r[1]) == 0.0f &&

            XMVectorGetX(matrix.r[2]) == 0.0f && XMVectorGetY(matrix.r[2]) == 0.0f &&
            XMVectorGetZ(matrix.r[2]) == 1.0f && XMVectorGetW(matrix.r[2]) == 0.0f &&

            XMVectorGetX(matrix.r[3]) == 0.0f && XMVectorGetY(matrix.r[3]) == 0.0f &&
            XMVectorGetZ(matrix.r[3]) == 0.0f && XMVectorGetW(matrix.r[3]) == 1.0f)
        {
            return true;
        }

        return false;
    }
    void                                SetWorldMatrix(_float4x4* pWorldMat)
    {
        m_pWorldMatrix = pWorldMat;
    }
    void                                SetRotationMatrix(_float4x4* pRotMat)
    {
        m_RotationMatrix = *pRotMat;
    }
    void                                SetTransform(class CTransform* pTransform);
    void                                SetSimulate(_bool boolean);
    void                                ResetForce();
    void                                Add_Force(_float4 vForce,COLLIDER_TYPE eType);
    void                                SetCulling(_bool boolean)
    {
        m_bCulling = boolean;
    }
    void                                SetBoneIndex();
    void                                Set_Index_CCT(_uint Index)
    {
        m_iId = Index;
    }
    _int                                Find_BoneIndex(const string& strRootTag);
    FORCEINLINE PxRigidDynamic*         GetRigidBody(COLLIDER_TYPE eType)
    {
        switch (eType)
        {
        case COLLIDER_TYPE::HEAD:
           return m_Head;
            break;
        case COLLIDER_TYPE::CHEST:
            return m_Chest;
            break;
        case COLLIDER_TYPE::PELVIS:
            return m_Pelvis;
            break;
        case COLLIDER_TYPE::ARM_L:
            return m_Arm_L;
            break;
        case COLLIDER_TYPE::ARM_R:
            return m_Arm_R;
            break;
        case COLLIDER_TYPE::FOREARM_L:
            return m_ForeArm_L;
            break;
        case COLLIDER_TYPE::FOREARM_R:
            return m_ForeArm_R;
            break;
        case COLLIDER_TYPE::HAND_L:
            return m_Hand_L;
            break;
        case COLLIDER_TYPE::HAND_R:
            return m_Hand_R;
            break;
        case COLLIDER_TYPE::CALF_L:
            return m_Calf_L;
            break;
        case COLLIDER_TYPE::CALF_R:
            return m_Calf_R;
            break;
        case COLLIDER_TYPE::LEG_L:
            return m_Leg_L;
            break;
        case COLLIDER_TYPE::LEG_R:
            return m_Leg_R;
            break;
        case COLLIDER_TYPE::FOOT_L:
            return m_Foot_L;
            break;
        case COLLIDER_TYPE::FOOT_R:
            return m_Foot_R;
            break;
        }

        return nullptr;         // 현진 수정임 : warning 잡음 
    }
    FORCEINLINE void Insert_Rigid_Body(vector<PxRigidDynamic*>* pRigidBodies, vector<COLLIDER_TYPE>* pColliders,COLLIDER_TYPE eType)
    {
        pColliders->push_back(eType);

        switch (eType)
        {
        case COLLIDER_TYPE::HEAD:
            pRigidBodies->push_back(m_Head);
            break;
        case COLLIDER_TYPE::CHEST:
            pRigidBodies->push_back(m_Chest);
            break;
        case COLLIDER_TYPE::PELVIS:
            pRigidBodies->push_back(m_Pelvis);
            break;
        case COLLIDER_TYPE::ARM_L:
            pRigidBodies->push_back(m_Arm_L);
            break;
        case COLLIDER_TYPE::ARM_R:
            pRigidBodies->push_back(m_Arm_R);
            break;
        case COLLIDER_TYPE::FOREARM_L:
            pRigidBodies->push_back(m_ForeArm_L);
            break;
        case COLLIDER_TYPE::FOREARM_R:
            pRigidBodies->push_back(m_ForeArm_R);
            break;
        case COLLIDER_TYPE::HAND_L:
            pRigidBodies->push_back(m_Hand_L);
            break;
        case COLLIDER_TYPE::HAND_R:
            pRigidBodies->push_back(m_Hand_R);
            break;
        case COLLIDER_TYPE::CALF_L:
            pRigidBodies->push_back(m_Calf_L);
            break;
        case COLLIDER_TYPE::CALF_R:
            pRigidBodies->push_back(m_Calf_R);
            break;
        case COLLIDER_TYPE::LEG_L:
            pRigidBodies->push_back(m_Leg_L);
            break;
        case COLLIDER_TYPE::LEG_R:
            pRigidBodies->push_back(m_Leg_R);
            break;
        case COLLIDER_TYPE::FOOT_L:
            pRigidBodies->push_back(m_Foot_L);
            break;
        case COLLIDER_TYPE::FOOT_R:
            pRigidBodies->push_back(m_Foot_R);
            break;
        }
    }
    _float4     GetBodyPosition();
public:
	static CRagdoll_Physics*            Create();

private:
    PxScene*                                        m_Scene = { nullptr };
    PxPhysics*                                      m_Physics = { nullptr };
    PxDefaultAllocator                              m_allocator;
    PxDefaultErrorCallback                          m_error_callback;
    PxFoundation*                                   m_foundation = nullptr;
    PxDefaultCpuDispatcher*                         m_dispatcher = nullptr;
    PxMaterial*                                     m_material = nullptr;

    PoseTransforms                                  m_pose_transforms;
    _matrix                                         m_model;
    _matrix                                         m_model_without_scale = XMMatrixIdentity();
    _matrix                                         m_model_only_scale = XMMatrixIdentity();
    float                                           m_scale = 1.f;
    float                                           m_ui_scale = m_scale;
    float                                           m_mass = 14.f;

    class SkeletalMesh*                             m_skeletal_mesh = { nullptr };
    class CRagdoll*                                 m_ragdoll = { nullptr };

    int32_t                                         m_selected_node = -1;
    std::vector<_vector>                            m_joint_pos;
    std::vector<_matrix>                            m_joint_mat;
    std::vector<std::pair<int32_t, bool>>           m_index_stack;

    std::vector<_int>                               m_joint_Index;
    std::vector<int32_t>                            m_selected_joints;
    std::vector<std::string>                        m_joint_names;
    class AnimRagdoll*                              m_ragdoll_pose = { nullptr };
    bool                                            m_simulate = false;
    vector<class CBone*>*                           m_vecBone = { nullptr };
    _bool                                           m_bRagdoll = { false };

    PxRigidDynamic*                                 m_Head = { nullptr };
    PxRigidDynamic*                                 m_Pelvis = { nullptr };
    PxRigidDynamic*                                 m_Chest = { nullptr };
    PxRigidDynamic*                                 m_Arm_L = { nullptr };
    PxRigidDynamic*                                 m_Arm_R = { nullptr };
    PxRigidDynamic*                                 m_ForeArm_L = { nullptr };
    PxRigidDynamic*                                 m_ForeArm_R = { nullptr };
    PxRigidDynamic*                                 m_Leg_R = { nullptr };
    PxRigidDynamic*                                 m_Leg_L = { nullptr };
    PxRigidDynamic*                                 m_Calf_L = { nullptr };
    PxRigidDynamic*                                 m_Calf_R = { nullptr };
    PxRigidDynamic*                                 m_Foot_L = { nullptr };
    PxRigidDynamic*                                 m_Foot_R = { nullptr };
    PxRigidDynamic*                                 m_Hand_R = { nullptr };
    PxRigidDynamic*                                 m_Hand_L = { nullptr };

    _bool                                           m_bJoint_Set = { false };

    _float4x4*                                      m_pWorldMatrix;
    _float4x4                                       m_RotationMatrix;
    PoseTransforms                                  m_Global_transforms;

    PxFilterData                                    m_FilterData;
    class CTransform*                               m_pTransform = { nullptr };

    _bool                                           m_bRagdoll_AddForce = { false };
    _bool                                           m_bCulling = { false };

    vector<_int>                                    m_vecBoneIndex;
    PxD6Joint*                                      m_pNeckJoint = { nullptr };
    PxD6Joint*                                      m_pUpSpine_Joint = { nullptr };
    PxD6Joint*                                      m_pSpine_Joint = { nullptr };
    PxD6Joint*                                      m_pClavicle_L_Joint = { nullptr };
    PxD6Joint*                                      m_pClavicle_R_Joint = { nullptr };
    PxD6Joint*                                      m_pElbow_L_Joint = { nullptr };
    PxD6Joint*                                      m_pElbow_R_Joint = { nullptr };
    PxD6Joint*                                      m_pWrist_L_Joint = { nullptr };
    PxD6Joint*                                      m_pWrist_R_Joint = { nullptr };
    PxD6Joint*                                      m_pHip_Joint_L = { nullptr };
    PxD6Joint*                                      m_pHip_Joint_R = { nullptr };
    PxD6Joint*                                      m_pKnee_Joint_L = { nullptr };
    PxD6Joint*                                      m_pKnee_Joint_R = { nullptr };
    PxD6Joint*                                      m_pAnkle_Joint_L = { nullptr };
    PxD6Joint*                                      m_pAnkle_Joint_R = { nullptr };

    _uint                                           m_iId = { 0 };

    uint32_t m_head_idx = { 0 };		//neck_1
    uint32_t m_neck_01_idx = { 0 };	//neck_0
    uint32_t m_spine_03_idx = { 0 };		//hips
    uint32_t m_spine_02_idx = { 0 };		//spine_2
    uint32_t m_spine_01_idx = { 0 };		//spine_1
    uint32_t m_pelvis_idx = { 0 };		//spine_0
             
    uint32_t m_thigh_l_idx = { 0 };		//l_leg_femur
    uint32_t m_calf_l_idx = { 0 };		//l_leg_tibia
    uint32_t m_foot_l_idx = { 0 };		//l_leg_ankle
    uint32_t m_ball_l_idx = { 0 };		//l_leg_ball
             
    uint32_t m_thigh_r_idx = { 0 };		//r_leg_femur
    uint32_t m_calf_r_idx = { 0 };		//r_leg_tibia
    uint32_t m_foot_r_idx = { 0 };		//r_leg_ankle
    uint32_t m_ball_r_idx = { 0 };		//r_leg_ball
             
    uint32_t m_upperarm_l_idx = { 0 };	//l_arm_humerus
    uint32_t m_lowerarm_l_idx = { 0 };	//l_arm_radius
    uint32_t m_hand_l_idx = { 0 };		//l_arm_wrist
    uint32_t m_middle_01_l_idx = { 0 };	//l_hand_middle_0

    uint32_t m_upperarm_r_idx = { 0 };	//r_arm_humerus
    uint32_t m_lowerarm_r_idx = { 0 };	//r_arm_radius
    uint32_t m_hand_r_idx = { 0 };		//r_arm_wrist
    uint32_t m_middle_01_r_idx = { 0 };	//r_hand_middle_0


    uint32_t m_head_idx_Bone = { 0 };		//neck_1
    uint32_t m_neck_01_idx_Bone = { 0 };	//neck_0
    uint32_t m_spine_03_idx_Bone = { 0 };		//hips
    uint32_t m_spine_02_idx_Bone = { 0 };		//spine_2
    uint32_t m_spine_01_idx_Bone = { 0 };		//spine_1
    uint32_t m_pelvis_idx_Bone = { 0 };		//spine_0

    uint32_t m_thigh_l_idx_Bone = { 0 };		//l_leg_femur
    uint32_t m_calf_l_idx_Bone = { 0 };		//l_leg_tibia
    uint32_t m_foot_l_idx_Bone = { 0 };		//l_leg_ankle
    uint32_t m_ball_l_idx_Bone = { 0 };		//l_leg_ball

    uint32_t m_thigh_r_idx_Bone = { 0 };		//r_leg_femur
    uint32_t m_calf_r_idx_Bone = { 0 };		//r_leg_tibia
    uint32_t m_foot_r_idx_Bone = { 0 };		//r_leg_ankle
    uint32_t m_ball_r_idx_Bone = { 0 };		//r_leg_ball

    uint32_t m_upperarm_l_idx_Bone = { 0 };	//l_arm_humerus
    uint32_t m_lowerarm_l_idx_Bone = { 0 };	//l_arm_radius
    uint32_t m_hand_l_idx_Bone = { 0 };		//l_arm_wrist
    uint32_t m_middle_01_l_idx_Bone = { 0 };	//l_hand_middle_0

    uint32_t m_upperarm_r_idx_Bone = { 0 };	//r_arm_humerus
    uint32_t m_lowerarm_r_idx_Bone = { 0 };	//r_arm_radius
    uint32_t m_hand_r_idx_Bone = { 0 };		//r_arm_wrist
    uint32_t m_middle_01_r_idx_Bone = { 0 };	//r_hand_middle_0
public:
	virtual void Free() override;
};

END