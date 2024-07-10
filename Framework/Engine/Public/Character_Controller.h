#pragma once

#include "Physics_Component.h"

BEGIN(Engine)

class ENGINE_DLL CCharacter_Controller : public CPhysics_Component
{
public:
	CCharacter_Controller(PxController* Controller,class CGameObject* pCharacter,PxScene* pScene,
		PxPhysics* pPhysics,class CTransform* pTransform, vector<class CBone*>* pBones, _int iId,const std::string& name = "");
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
	PxRigidDynamic*				create_capsule_bone(uint32_t parent_idx, uint32_t child_idx, class CRagdoll& ragdoll, float r = 0.5f, XMMATRIX rotation = XMMatrixIdentity(),COLLIDER_TYPE eType = COLLIDER_TYPE::HEAD);
	PxRigidDynamic*				create_capsule_bone(uint32_t parent_idx, class CRagdoll& ragdoll, XMVECTOR offset, float l, float r = 0.5f, XMMATRIX rotation = XMMatrixIdentity(), COLLIDER_TYPE eType = COLLIDER_TYPE::HEAD);
	PxRigidDynamic*				create_sphere_bone(uint32_t parent_idx, class CRagdoll& ragdoll, float r, COLLIDER_TYPE eType);
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
#pragma endregion

#pragma region Interaction with Player
public:
	void						Set_Hit(_bool boolean)
	{
		m_bHit = boolean;
	}
	_bool						Is_Hit()
	{
		return m_bHit;
	}
	void						Set_Force(_float4 vForce, COLLIDER_TYPE eType)
	{
		m_vForce = vForce;
		m_eType = eType;

		switch (eType)
		{
		case COLLIDER_TYPE::HEAD:
			m_pHitPart = m_HeadCollider;
			break;
		case COLLIDER_TYPE::CHEST:
			m_pHitPart = m_BodyCollider;
			break;
		case COLLIDER_TYPE::PELVIS:
			m_pHitPart = m_Pelvis_Collider;
			break;
		case COLLIDER_TYPE::ARM_L:
			m_pHitPart = m_Left_Arm_Collider;
			break;
		case COLLIDER_TYPE::ARM_R:
			m_pHitPart = m_Right_Arm_Collider;
			break;
		case COLLIDER_TYPE::FOREARM_L:
			m_pHitPart = m_Left_ForeArm_Collider;
			break;
		case COLLIDER_TYPE::FOREARM_R:
			m_pHitPart = m_Right_ForeArm_Collider;
			break;
		case COLLIDER_TYPE::HAND_L:
			m_pHitPart = m_Left_Hand_Collider;
			break;
		case COLLIDER_TYPE::HAND_R:
			m_pHitPart = m_Right_Hand_Collider;
			break;
		case COLLIDER_TYPE::CALF_L:
			m_pHitPart = m_Left_Shin_Collider;
			break;
		case COLLIDER_TYPE::CALF_R:
			m_pHitPart = m_Right_Shin_Collider;
			break;
		case COLLIDER_TYPE::LEG_L:
			m_pHitPart = m_Left_Leg_Collider;
			break;
		case COLLIDER_TYPE::LEG_R:
			m_pHitPart = m_Right_Leg_Collider;
			break;
		case COLLIDER_TYPE::FOOT_L:
			m_pHitPart = m_Left_Foot_Collider;
			break;
		case COLLIDER_TYPE::FOOT_R:
			m_pHitPart = m_Right_Foot_Collider;
			break;
		}
	}
	_float4								Get_Force()
	{
		return m_vForce;
	}
	COLLIDER_TYPE						Get_Hit_Collider_Type()
	{
		return m_eType;
	}
	void                                SetBoneIndex();
	virtual void						SetPosition(_float4 vPos) override;
	PxRigidDynamic*						GetHitPart()
	{
		return m_pHitPart;
	}
	_bool								IsBigAttack()
	{
		return m_bBigAttack;
	}
	void								SetBigAttack(_bool boolean)
	{
		m_bBigAttack = boolean;
	}
	_uint								GetHitCount_STG()
	{
		return m_iHitCount_STG;
	}
	void								Increase_Hit_Count_STG()
	{
		++m_iHitCount_STG;
	}
	void								Insert_Hit_Point_STG(_float4 vPoint)
	{
		m_vecBlockPoint_STG.push_back(vPoint);
	}
	void								Insert_Hit_Normal_STG(_float4 vNormal)
	{
		m_vecBlockNormal_STG.push_back(vNormal);
	}
	vector<_float4>*					GetBlockPoint_STG()
	{
		return &m_vecBlockPoint_STG;
	}
	vector<_float4>*					GetNormalPoint_STG()
	{
		return &m_vecBlockNormal_STG;
	}
	vector<PxRigidDynamic*>*			GetHitPart_STG()
	{
		return &m_vecHitPart_STG;
	}
	vector<COLLIDER_TYPE>*				GetColliderType_STG()
	{
		return &m_vecColliderType_STG;
	}
	void								Insert_HitPart_STG(PxRigidDynamic* pHitPart)
	{
		m_vecHitPart_STG.push_back(pHitPart);
	}
	void								Insert_Collider_Type(COLLIDER_TYPE eType)
	{
		m_vecColliderType_STG.push_back(eType);

		switch (eType)
		{
		case COLLIDER_TYPE::HEAD:
			m_vecHitPart_STG.push_back(m_HeadCollider);
			break;
		case COLLIDER_TYPE::CHEST:
			m_vecHitPart_STG.push_back(m_BodyCollider);
			break;
		case COLLIDER_TYPE::PELVIS:
			m_vecHitPart_STG.push_back(m_Pelvis_Collider);
			break;
		case COLLIDER_TYPE::ARM_L:
			m_vecHitPart_STG.push_back(m_Left_Arm_Collider);
			break;
		case COLLIDER_TYPE::ARM_R:
			m_vecHitPart_STG.push_back(m_Right_Arm_Collider);
			break;
		case COLLIDER_TYPE::FOREARM_L:
			m_vecHitPart_STG.push_back(m_Left_ForeArm_Collider);
			break;
		case COLLIDER_TYPE::FOREARM_R:
			m_vecHitPart_STG.push_back(m_Right_ForeArm_Collider);
			break;
		case COLLIDER_TYPE::HAND_L:
			m_vecHitPart_STG.push_back(m_Left_Hand_Collider);
			break;
		case COLLIDER_TYPE::HAND_R:
			m_vecHitPart_STG.push_back(m_Right_Hand_Collider);
			break;
		case COLLIDER_TYPE::CALF_L:
			m_vecHitPart_STG.push_back(m_Left_Shin_Collider);
			break;
		case COLLIDER_TYPE::CALF_R:
			m_vecHitPart_STG.push_back(m_Right_Shin_Collider);
			break;
		case COLLIDER_TYPE::LEG_L:
			m_vecHitPart_STG.push_back(m_Left_Leg_Collider);
			break;
		case COLLIDER_TYPE::LEG_R:
			m_vecHitPart_STG.push_back(m_Right_Leg_Collider);
			break;
		case COLLIDER_TYPE::FOOT_L:
			m_vecHitPart_STG.push_back(m_Left_Foot_Collider);
			break;
		case COLLIDER_TYPE::FOOT_R:
			m_vecHitPart_STG.push_back(m_Right_Foot_Collider);
			break;
		}
	}

	void						Insert_Collider_Type_Ragdoll(COLLIDER_TYPE eType)
	{
		m_vecColliderType_STG.push_back(eType);
	}

	_bool						Is_Hit_Decal_Ray()
	{
		return m_bHit_DecalRay;
	}
	void						Set_Hit_Decal_Ray(_bool boolean)
	{
		m_bHit_DecalRay = boolean;
	}

#pragma region Partial Ragdoll
public:
	void						Release_PartialCollider(COLLIDER_TYPE eType);
#pragma endregion

private:
	_bool											m_bHit = { false };
	_float4											m_vForce;
	COLLIDER_TYPE									m_eType;
	_int											m_iId = { 0 };
	vector<_int>                                    m_vecBoneIndex;
	PxRigidDynamic*									m_pHitPart = { nullptr };
	_bool											m_bBigAttack = { false };
	vector<_float4>									m_vecBlockPoint_STG;
	vector<_float4>									m_vecBlockNormal_STG;
	_uint											m_iHitCount_STG = { 0 };
	vector<PxRigidDynamic*>							m_vecHitPart_STG;
	vector<COLLIDER_TYPE>							m_vecColliderType_STG;
	_bool											m_bHit_DecalRay = { false };
#pragma endregion

public:
	virtual void Free() override;
};

END