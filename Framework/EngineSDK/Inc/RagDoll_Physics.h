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

inline XMVECTOR QuaternionFromVectors(const XMVECTOR& a, const XMVECTOR& b) {
    // 입력 벡터를 정규화합니다.
    XMVECTOR normA = XMVector3Normalize(a);
    XMVECTOR normB = XMVector3Normalize(b);

    // 두 벡터의 점곱을 계산합니다.
    float dotProduct = XMVectorGetX(XMVector3Dot(normA, normB));

    // 두 벡터가 거의 반대 방향일 때를 처리합니다.
    const float epsilon = 1e-6f;
    if (dotProduct < -1.0f + epsilon) {
        // 기본 축을 선택합니다. 여기서는 y축을 사용합니다.
        XMVECTOR orthoVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        // 하지만 입력 벡터가 y축과 거의 평행한 경우, 다른 축을 선택합니다.
        if (fabs(XMVectorGetY(normA)) > 0.999f) {
            orthoVector = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
        }
        XMVECTOR axis = XMVector3Cross(normA, orthoVector);
        return XMQuaternionRotationAxis(axis, XM_PI);
    }

    // 두 벡터의 교차곱을 계산합니다.
    XMVECTOR crossProduct = XMVector3Cross(normA, normB);

    // 쿼터니언의 w 성분을 계산합니다.
    float w = sqrtf((1.0f + dotProduct) * 2.0f);
    float invW = 1.0f / w;

    // 결과 쿼터니언을 생성합니다.
    XMVECTOR quaternion = XMVectorSet(
        XMVectorGetX(crossProduct) * invW,
        XMVectorGetY(crossProduct) * invW,
        XMVectorGetZ(crossProduct) * invW,
        w * 0.5f
    );

    return quaternion;
}

// 이 함수는 쿼터니언 q1에서 q2로의 변환 쿼터니언을 계산합니다.
inline XMVECTOR CalculateTransitionQuaternion(XMVECTOR q1, XMVECTOR q2)
{
    // 먼저, q1의 역쿼터니언을 계산합니다.
    XMVECTOR q1Inverse = XMQuaternionInverse(q1);

    // 변환 쿼터니언은 q1의 역쿼터니언과 q2의 곱으로 계산됩니다.
    XMVECTOR transitionQuaternion = XMQuaternionMultiply(q1Inverse, q2);

    // 결과 쿼터니언을 정규화합니다.
    transitionQuaternion = XMVector4Normalize(transitionQuaternion);

    return transitionQuaternion;
}


inline XMVECTOR VectorToQuaternion(XMVECTOR direction)
{
    // 방향 벡터를 정규화합니다.
    direction = XMVector3Normalize(direction);

    // 기본 축 (보통은 z축)을 정의합니다.
    XMVECTOR zAxis = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

    // 방향 벡터와 z축이 거의 같으면 기본 쿼터니언을 반환합니다.
    if (XMVector3NearEqual(direction, zAxis, XMVectorSet(1e-6f, 1e-6f, 1e-6f, 1e-6f)))
    {
        return XMQuaternionIdentity();
    }

    // 방향 벡터와 z축이 거의 반대 방향이면, x축을 기준으로 180도 회전하는 쿼터니언을 반환합니다.
    if (XMVector3NearEqual(direction, XMVectorNegate(zAxis), XMVectorSet(1e-6f, 1e-6f, 1e-6f, 1e-6f)))
    {
        return XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XM_PI);
    }

    // 회전 축을 계산합니다.
    XMVECTOR rotationAxis = XMVector3Cross(zAxis, direction);
    rotationAxis = XMVector3Normalize(rotationAxis);

    // 회전 각도를 계산합니다.
    float dotProduct = XMVectorGetX(XMVector3Dot(zAxis, direction));
    float rotationAngle = acosf(dotProduct);

    // 회전축과 각도로부터 쿼터니언을 생성합니다.
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
    void  Set_Kinematic(_bool boolean);
    void update_animations();
    void Init_Ragdoll();
    void create_joint();
    void			SetBone_Ragdoll(vector<class CBone*>* vecBone)
    {
        m_vecBone = vecBone;
    }
    bool IsIdentityMatrix(const _matrix& matrix)
    {
        // 항등 행렬의 주 대각선 원소와 나머지 원소를 확인
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
    void    SetWorldMatrix(_float4x4 WorldMat)
    {
        m_WorldMatrix = WorldMat;
    }
    void    SetRotationMatrix(_float4x4 WorldMat)
    {
        m_RotationMatrix = WorldMat;
    }
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
    class AnimRagdoll* m_ragdoll_pose = { nullptr };
    bool    m_simulate = false;
    vector<class CBone*>* m_vecBone = { nullptr };
    _bool               m_bRagdoll = { false };

    PxRigidDynamic*     m_Head = { nullptr };
    PxRigidDynamic*     m_Pelvis = { nullptr };
    PxRigidDynamic*     m_Arm_L = { nullptr };
    PxRigidDynamic*     m_Arm_R = { nullptr };
    PxRigidDynamic*     m_ForeArm_L = { nullptr };
    PxRigidDynamic*     m_ForeArm_R = { nullptr };
    PxRigidDynamic*     m_Leg_R = { nullptr };
    PxRigidDynamic*     m_Leg_L = { nullptr };
    PxRigidDynamic*     m_Calf_L = { nullptr };
    PxRigidDynamic*     m_Calf_R = { nullptr };
    PxRigidDynamic*     m_Foot_L = { nullptr };
    PxRigidDynamic*     m_Foot_R = { nullptr };
    PxRigidDynamic*     m_Hand_R = { nullptr };
    PxRigidDynamic*     m_Hand_L = { nullptr };

    _bool               m_bJoint_Set = { false };

    _float4x4           m_WorldMatrix;
    _float4x4           m_RotationMatrix;
    PoseTransforms* m_Global_transforms = { nullptr };
public:
	virtual void Free() override;
};

END