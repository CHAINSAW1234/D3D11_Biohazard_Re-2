#pragma once

#include "Physics_Component.h"

BEGIN(Engine)

class ENGINE_DLL CCharacter_Controller : public CPhysics_Component
{
public:
	CCharacter_Controller(PxController* Controller,class CGameObject* pCharacter,PxScene* pScene,
		PxPhysics* pPhysics,class CTransform* pTransform, vector<class CBone*>* pBones, const std::string& name = "");
	CCharacter_Controller(const CCharacter_Controller& rhs);
	virtual ~CCharacter_Controller() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
public:
	virtual void Update() override;
public:
	static CCharacter_Controller* Create();

	virtual void Move(_float4 Dir, _float fTimeDelta) override;
	virtual void Move(PxVec3 Dir, _float fTimeDelta) override;
	virtual _float4 GetTranslation() override;
	virtual void	Release_Px() override;
	virtual _float4 GetPosition_Float4() override; 
	virtual _float4 GetPosition_Float4_Zombie() override; 

	class CGameObject* m_pCharacter = { nullptr };

#pragma region 편의성 함수들
public:
	_bool			IsGrounded(PxController* Controller);
	FORCEINLINE PxQuat               to_quat(const _fvector& q)
	{
		return PxQuat(XMVectorGetX(q), XMVectorGetY(q), XMVectorGetZ(q), XMVectorGetW(q));
	}

	FORCEINLINE _vector              to_quat(const PxQuat& q)
	{
		return XMVectorSet(q.x, q.y, q.z, q.w);
	}

	FORCEINLINE PxVec3               to_vec3(const _fvector& vec3)
	{
		return PxVec3(XMVectorGetX(vec3), XMVectorGetY(vec3), XMVectorGetZ(vec3));
	}

	FORCEINLINE _vector              to_vec3(const PxVec3& vec3)
	{
		return XMVectorSet(vec3.x, vec3.y, vec3.z, 1.f);
	}

	FORCEINLINE XMVECTOR             QuaternionFromVectors(const XMVECTOR& _vec1, const XMVECTOR& _vec2) {
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

	FORCEINLINE XMVECTOR CalculateTransitionQuaternion(XMVECTOR q1, XMVECTOR q2)
	{
		XMVECTOR q1Inverse = XMQuaternionInverse(q1);

		XMVECTOR transitionQuaternion = XMQuaternionMultiply(XMQuaternionNormalize(q1Inverse), XMQuaternionNormalize(q2));

		transitionQuaternion = XMQuaternionNormalize(transitionQuaternion);

		return transitionQuaternion;
	}

	FORCEINLINE XMVECTOR VectorToQuaternion(XMVECTOR direction)
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


	FORCEINLINE XMMATRIX to_mat4(const PxMat44& mat)
	{
		return XMMATRIX(
			mat[0][0], mat[0][1], mat[0][2], mat[0][3],
			mat[1][0], mat[1][1], mat[1][2], mat[1][3],
			mat[2][0], mat[2][1], mat[2][2], mat[2][3],
			mat[3][0], mat[3][1], mat[3][2], mat[3][3]
		);
	}

	FORCEINLINE PxMat44 to_mat44(const XMMATRIX& mat)
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

#pragma endregion

#pragma region Collider
public:
	void						Create_Collider();
	virtual void				Build_Skeleton(const std::string& name) override;
	void						Update_Collider();
	void                        load_mesh(const string& name);
	PxRigidDynamic*				create_capsule_bone(uint32_t parent_idx, uint32_t child_idx, class CRagdoll& ragdoll, float r = 0.5f, XMMATRIX rotation = XMMatrixIdentity());
	PxRigidDynamic*				create_capsule_bone(uint32_t parent_idx, class CRagdoll& ragdoll, XMVECTOR offset, float l, float r = 0.5f, XMMATRIX rotation = XMMatrixIdentity());
	PxRigidDynamic*				create_sphere_bone(uint32_t parent_idx, class CRagdoll& ragdoll, float r);
private:
	PxTransform					ColliderTransform_Body = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform					ColliderTransform_Head = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform					ColliderTransform_Right_Arm = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform					ColliderTransform_Left_Arm = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform					ColliderTransform_Right_ForeArm = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform					ColliderTransform_Left_ForeArm = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform					ColliderTransform_Pelvis = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform					ColliderTransform_Left_Leg = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform					ColliderTransform_Right_Leg = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform					ColliderTransform_Left_Shin = { PxTransform(PxVec3(0.f,0.f,0.f)) };
	PxTransform					ColliderTransform_Right_Shin = { PxTransform(PxVec3(0.f,0.f,0.f)) };

	PxShape*					m_Shape_Head = { nullptr };
	PxShape*					m_Shape_Body = { nullptr };
	PxShape*					m_Shape_Right_Arm = { nullptr };
	PxShape*					m_Shape_Left_Arm = { nullptr };
	PxShape*					m_Shape_Right_ForeArm = { nullptr };
	PxShape*					m_Shape_Left_ForeArm = { nullptr };
	PxShape*					m_Shape_Pelvis = { nullptr };
	PxShape*					m_Shape_Left_Leg = { nullptr };
	PxShape*					m_Shape_Right_Leg = { nullptr };
	PxShape*					m_Shape_Left_Shin = { nullptr };
	PxShape*					m_Shape_Right_Shin = { nullptr };

	PxRigidDynamic*				m_BodyCollider = { nullptr };
	PxRigidDynamic*				m_HeadCollider = { nullptr };
	PxRigidDynamic*				m_Left_Arm_Collider = { nullptr };
	PxRigidDynamic*				m_Right_Arm_Collider = { nullptr };
	PxRigidDynamic*				m_Left_ForeArm_Collider = { nullptr };
	PxRigidDynamic*				m_Right_ForeArm_Collider = { nullptr };
	PxRigidDynamic*				m_Pelvis_Collider = { nullptr };
	PxRigidDynamic*				m_Left_Leg_Collider = { nullptr };
	PxRigidDynamic*				m_Right_Leg_Collider = { nullptr };
	PxRigidDynamic*				m_Left_Shin_Collider = { nullptr };
	PxRigidDynamic*				m_Right_Shin_Collider = { nullptr };
	PxRigidDynamic*				m_Left_Hand_Collider = { nullptr };
	PxRigidDynamic*				m_Right_Hand_Collider = { nullptr };
	PxRigidDynamic*				m_Left_Foot_Collider = { nullptr };
	PxRigidDynamic*				m_Right_Foot_Collider = { nullptr };

	class SkeletalMesh*			m_skeletal_mesh = { nullptr };
	class CTransform*			m_pTransform = { nullptr };
	_float4x4*					m_pWorldMatrix;
	_float4x4*					m_pRotationMatrix;
	PoseTransforms              m_Global_transforms;
	vector<class CBone*>*		m_vecBone = { nullptr };
	vector<PxRigidDynamic*>		m_vecCollider;
	class CRagdoll*				m_ragdoll = { nullptr };
	PoseTransforms              m_pose_transforms;

	PxMaterial*					m_material = {nullptr};
	float                       m_mass = 1.0f;
#pragma endregion
public:
	virtual void Free() override;
};

END